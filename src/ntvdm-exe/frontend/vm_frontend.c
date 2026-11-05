#include "vm_frontend.h"
#include "common/frontend-base/service.h"
#include "common/base_srv/frontend_client.h"
#include "common/base_srv/base_rpc_client.h"
#include "common/protocol/frontend_lifecycle.h"

typedef struct vm_frontend {
    struct vm_frontend *next;
    common_frontend_location location;
    common_frontend_key key;
    HANDLE notification,retire,changed,broker,input_ready;
    frontend_base_service *service;
    ntvdm_vm_io io;
    void *context;
    BOOL closing;
    DWORD close_error,wire_generation,sequence;
} vm_frontend;

struct ntvdm_vm_frontends {
    uint64_t payload;
    vm_frontend *entries;
};

/* Process-lifetime sequence, including successive payload registries. */
static SRWLOCK incarnation_lock=SRWLOCK_INIT;
static uint64_t incarnation;

static vm_frontend **find_entry(ntvdm_vm_frontends *registry,
    const common_frontend_location *location)
{
    vm_frontend **entry;
    if(!registry || !location || registry->payload!=location->payload)return NULL;
    for(entry=&registry->entries;*entry;entry=&(*entry)->next)
        if((*entry)->location.vm==location->vm &&
            (*entry)->location.incarnation==location->incarnation)return entry;
    return NULL;
}

static DWORD retirement(void *context,DWORD *action)
{
    vm_frontend *entry=context;
    return OpenNtBaseClientRetireWorkerlessFrontend(&entry->key,action);
}
static DWORD prepare(void *context)
{(void)context;return ERROR_SUCCESS;}
static DWORD request(void *context,DWORD *id,HANDLE *worker)
{
    vm_frontend *entry=context;
    return OpenNtBaseClientFrontendRequest(&entry->key,id,worker);
}
static DWORD attach(void *context,DWORD id,HANDLE pipe,HANDLE ready,DWORD *generation)
{
    vm_frontend *entry=context;
    return OpenNtBaseClientAttachFrontendRequest(&entry->key,id,pipe,ready,generation);
}
static void generation(void *context,DWORD value)
{
    vm_frontend *entry=context;
    entry->wire_generation=value;entry->sequence=0;
}
static BOOL active(void *context)
{(void)context;return TRUE;}
static DWORD dispatch(void *context,const console_io_request *request,
    console_io_reply *reply,frontend_base_result *result)
{
    vm_frontend *entry=context;
    DWORD error;
    if(request->version!=CONSOLE_IO_VERSION)return ERROR_REVISION_MISMATCH;
    if(!entry->wire_generation || request->generation!=entry->wire_generation)return ERROR_ACCESS_DENIED;
    if(!request->sequence || entry->sequence==UINT32_MAX || request->sequence!=entry->sequence+1 ||
        request->operation<CONSOLE_IO_WRITE || request->operation>CONSOLE_IO_PRESENT_BARRIER ||
        request->operation==38)return ERROR_INVALID_DATA;
    entry->sequence=request->sequence;
    error=entry->io.dispatch(entry->context,request,reply,result);
    /* Wire identity belongs to this authenticated channel, not guest/provider
     * state. The provider owns operation data and result only. */
    reply->version=CONSOLE_IO_VERSION;reply->generation=entry->wire_generation;
    reply->sequence=request->sequence;
    return error;
}
static DWORD input_pending(void *context,BOOL *pending)
{
    vm_frontend *entry=context;
    return entry->io.input_pending(entry->context,pending);
}
static void finish(void *context)
{
    vm_frontend *entry=context;
    entry->io.finish(entry->context);
}
static DWORD channel_start(void *context,DWORD id,HANDLE worker,void **output)
{
    vm_frontend *entry=context;
    frontend_base_channel *channel=NULL;
    const frontend_base_channel_ops ops={prepare,attach,generation,active,
        dispatch,input_pending,finish};
    DWORD error=frontend_base_channel_start(id,worker,entry->input_ready,&ops,entry,&channel);
    *output=channel;
    return error;
}
static DWORD channel_stop(void *context,void *channel)
{(void)context;return frontend_base_channel_stop(channel);}
static HANDLE channel_thread(void *context,void *channel)
{(void)context;return frontend_base_channel_thread(channel);}
static DWORD disconnected(void *context)
{
    vm_frontend *entry=context;
    return OpenNtBaseClientFrontendIoDisconnected(&entry->key);
}
static DWORD fault(void *context)
{
    vm_frontend *entry=context;
    return OpenNtBaseClientCloseFrontendInstance(&entry->key);
}
static void free_entry(vm_frontend *entry)
{
    if(entry->changed)CloseHandle(entry->changed);
    if(entry->notification)CloseHandle(entry->notification);
    if(entry->retire)CloseHandle(entry->retire);
    if(entry->broker)CloseHandle(entry->broker);
    HeapFree(GetProcessHeap(),0,entry);
}

DWORD ntvdm_vm_frontends_create(uint64_t payload,ntvdm_vm_frontends **output)
{
    ntvdm_vm_frontends *registry;
    if(!output)return ERROR_INVALID_PARAMETER;
    *output=NULL;
    if(!payload)return ERROR_INVALID_PARAMETER;
    registry=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*registry));
    if(!registry)return ERROR_NOT_ENOUGH_MEMORY;
    registry->payload=payload;*output=registry;
    return ERROR_SUCCESS;
}

DWORD ntvdm_vm_frontend_register(ntvdm_vm_frontends *registry,uint64_t vm,
    HANDLE input_ready,const ntvdm_vm_io *io,void *context,common_frontend_location *identity)
{
    vm_frontend *entry;
    DWORD error;
    const frontend_base_service_ops ops={retirement,prepare,request,channel_start,
        channel_stop,channel_thread,disconnected,fault};
    if(!identity)return ERROR_INVALID_PARAMETER;
    ZeroMemory(identity,sizeof(*identity));
    if(!registry || !input_ready || input_ready==INVALID_HANDLE_VALUE || !io ||
        !io->dispatch || !io->input_pending || !io->finish)return ERROR_INVALID_PARAMETER;
    for(entry=registry->entries;entry;entry=entry->next)
        if(entry->location.vm==vm)return ERROR_ALREADY_EXISTS;
    entry=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*entry));
    if(!entry)return ERROR_NOT_ENOUGH_MEMORY;
    entry->location.payload=registry->payload;entry->location.vm=vm;
    AcquireSRWLockExclusive(&incarnation_lock);
    if(incarnation!=UINT64_MAX)entry->location.incarnation=++incarnation;
    ReleaseSRWLockExclusive(&incarnation_lock);
    if(!entry->location.incarnation){free_entry(entry);return ERROR_ARITHMETIC_OVERFLOW;}
    entry->io=*io;entry->context=context;entry->input_ready=input_ready;
    error=OpenNtBaseClientBrokerProcess(&entry->broker);
    if(!error)error=OpenNtBaseClientRegisterEmbeddedFrontend(&entry->location,&entry->key,
        &entry->notification,&entry->retire);
    if(error){free_entry(entry);return error;}
    /* Once registered, failure leaves an owned rollback entry. Never discard
     * its key or borrowed context while the broker still owns the instance. */
    entry->next=registry->entries;registry->entries=entry;*identity=entry->location;
    error=OpenNtBaseClientFrontendStateChanged(&entry->key,&entry->changed);
    if(!error)error=frontend_base_service_start(entry->notification,entry->retire,
        entry->broker,entry->changed,&ops,entry,&entry->service);
    if(error) {
        entry->closing=TRUE;
        entry->close_error=OpenNtBaseClientCloseFrontendInstance(&entry->key);
    }
    return error;
}

DWORD ntvdm_vm_frontend_origin(ntvdm_vm_frontends *registry,
    const common_frontend_location *identity,HANDLE *origin)
{
    vm_frontend **found;
    DWORD wait;
    if(!origin)return ERROR_INVALID_PARAMETER;
    *origin=NULL;found=find_entry(registry,identity);
    if(!found)return ERROR_NOT_FOUND;
    if((*found)->closing || !(*found)->service)return ERROR_INVALID_STATE;
    wait=WaitForSingleObject(frontend_base_service_thread((*found)->service),0);
    if(wait==WAIT_OBJECT_0)return ERROR_OPERATION_ABORTED;
    if(wait==WAIT_FAILED)return GetLastError();
    if(!DuplicateHandle(GetCurrentProcess(),(*found)->notification,GetCurrentProcess(),
        origin,SYNCHRONIZE,FALSE,0))return GetLastError();
    return ERROR_SUCCESS;
}

DWORD ntvdm_vm_frontend_retire(ntvdm_vm_frontends *registry,
    const common_frontend_location *identity)
{
    vm_frontend **found=find_entry(registry,identity),*entry;
    DWORD error,wait,action=OPENNT_FRONTEND_RETIRE_NONE;
    BOOL lost;
    if(!found)return ERROR_NOT_FOUND;
    entry=*found;
    lost=WaitForSingleObject(entry->broker,0)==WAIT_OBJECT_0;
    if(!lost && !entry->closing) {
        entry->closing=TRUE;
        entry->close_error=OpenNtBaseClientCloseFrontendInstance(&entry->key);
    }
    if(!lost && entry->close_error)return entry->close_error;
    if(entry->service) {
        wait=WaitForSingleObject(frontend_base_service_thread(entry->service),INFINITE);
        if(wait!=WAIT_OBJECT_0)return GetLastError();
        error=frontend_base_service_stop(entry->service);
        if(error)return error;
        entry->service=NULL;
    } else if(!lost) {
        HANDLE waits[2]={entry->broker,entry->retire};
        error=retirement(entry,&action);
        if(error)return error;
        if(action!=OPENNT_FRONTEND_RETIRE_FINAL_CLOSE) {
            wait=WaitForMultipleObjects(2,waits,FALSE,INFINITE);
            if(wait!=WAIT_OBJECT_0 && wait!=WAIT_OBJECT_0+1)return GetLastError();
        }
    }
    lost=WaitForSingleObject(entry->broker,0)==WAIT_OBJECT_0;
    if(!lost) {
        error=OpenNtBaseClientUnregisterFrontendInstance(&entry->key);
        if(error)return error;
    }
    *found=entry->next;free_entry(entry);
    return ERROR_SUCCESS;
}

DWORD ntvdm_vm_frontends_destroy(ntvdm_vm_frontends *registry)
{
    DWORD error;
    if(!registry)return ERROR_SUCCESS;
    while(registry->entries) {
        error=ntvdm_vm_frontend_retire(registry,&registry->entries->location);
        if(error)return error;
    }
    HeapFree(GetProcessHeap(),0,registry);
    return ERROR_SUCCESS;
}
