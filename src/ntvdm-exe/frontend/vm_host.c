#include "vm_host.h"
#include "common/base_srv/base_rpc_client.h"

typedef struct vm_payload {
    struct vm_payload *next;
    uint64_t identity;
    ntvdm_vm_frontends *frontends;
    ULONG users;
    BOOL closing,releasing;
} vm_payload;

struct ntvdm_vm_host {
    SRWLOCK lock;
    CONDITION_VARIABLE changed;
    uint64_t sequence;
    BOOL closing;
    vm_payload *payloads;
};

static vm_payload **find_payload(ntvdm_vm_host *host,uint64_t identity)
{
    vm_payload **item;
    for(item=&host->payloads;*item;item=&(*item)->next)
        if((*item)->identity==identity)return item;
    return NULL;
}
/* Pins only the payload lifetime. Its registry serializes short list changes,
 * not RPC, callbacks or joins; independent VMs remain independently usable. */
static DWORD acquire_payload(ntvdm_vm_host *host,uint64_t identity,vm_payload **out)
{
    vm_payload **item;
    DWORD error=ERROR_SUCCESS;
    AcquireSRWLockExclusive(&host->lock);
    item=find_payload(host,identity);
    if(host->closing)error=ERROR_OPERATION_ABORTED;
    else if(!item)error=ERROR_NOT_FOUND;
    else if((*item)->closing)error=ERROR_OPERATION_ABORTED;
    else if((*item)->users==MAXDWORD)error=ERROR_ARITHMETIC_OVERFLOW;
    else {++(*item)->users;*out=*item;}
    ReleaseSRWLockExclusive(&host->lock);return error;
}
static void release_payload(ntvdm_vm_host *host,vm_payload *payload)
{
    AcquireSRWLockExclusive(&host->lock);
    --payload->users;
    WakeAllConditionVariable(&host->changed);
    ReleaseSRWLockExclusive(&host->lock);
}
DWORD ntvdm_vm_host_create(ntvdm_vm_host **output)
{
    ntvdm_vm_host *host;
    if(!output)return ERROR_INVALID_PARAMETER;
    *output=NULL;
    host=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*host));
    if(!host)return ERROR_NOT_ENOUGH_MEMORY;
    InitializeSRWLock(&host->lock);InitializeConditionVariable(&host->changed);
    *output=host;return ERROR_SUCCESS;
}
DWORD ntvdm_vm_payload_begin(ntvdm_vm_host *host,uint64_t *identity)
{
    vm_payload *payload=NULL;
    DWORD error=ERROR_OPERATION_ABORTED;
    if(!identity)return ERROR_INVALID_PARAMETER;
    *identity=0;
    if(!host)return ERROR_INVALID_PARAMETER;
    AcquireSRWLockExclusive(&host->lock);
    if(host->closing)goto done;
    if(host->sequence==UINT64_MAX){error=ERROR_ARITHMETIC_OVERFLOW;goto done;}
    payload=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*payload));
    if(!payload){error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
    payload->identity=++host->sequence;
    error=ntvdm_vm_frontends_create(payload->identity,&payload->frontends);
    if(error){HeapFree(GetProcessHeap(),0,payload);goto done;}
    payload->next=host->payloads;host->payloads=payload;*identity=payload->identity;
done:
    ReleaseSRWLockExclusive(&host->lock);return error;
}
DWORD ntvdm_vm_payload_end(ntvdm_vm_host *host,uint64_t identity)
{
    vm_payload **item,*payload;
    DWORD error=ERROR_NOT_FOUND;
    if(!host)return ERROR_INVALID_PARAMETER;
    AcquireSRWLockExclusive(&host->lock);
    for(;;) {
        item=find_payload(host,identity);
        if(!item)goto done;
        payload=*item;payload->closing=TRUE;
        if(!payload->releasing && !payload->users)break;
        if(!SleepConditionVariableSRW(&host->changed,&host->lock,INFINITE,0)) {
            error=GetLastError();goto done;
        }
    }
    payload->releasing=TRUE;
    ReleaseSRWLockExclusive(&host->lock);
    error=ntvdm_vm_frontends_destroy(payload->frontends);
    AcquireSRWLockExclusive(&host->lock);
    item=find_payload(host,identity);
    if(!error) {*item=payload->next;HeapFree(GetProcessHeap(),0,payload);}
    else payload->releasing=FALSE;
    WakeAllConditionVariable(&host->changed);
done:
    ReleaseSRWLockExclusive(&host->lock);return error;
}
DWORD ntvdm_vm_host_register(ntvdm_vm_host *host,uint64_t payload,uint64_t vm,
    HANDLE input_ready,const ntvdm_vm_io *io,void *context,common_frontend_location *identity)
{
    vm_payload *item=NULL;
    DWORD error;
    if(!identity)return ERROR_INVALID_PARAMETER;
    ZeroMemory(identity,sizeof(*identity));
    if(!host)return ERROR_INVALID_PARAMETER;
    error=acquire_payload(host,payload,&item);
    if(!error) {
        error=ntvdm_vm_frontend_register(item->frontends,vm,input_ready,io,context,identity);
        release_payload(host,item);
    }
    return error;
}
DWORD ntvdm_vm_host_origin(ntvdm_vm_host *host,const common_frontend_location *identity,HANDLE *origin)
{
    vm_payload *item=NULL;
    DWORD error;
    if(!origin)return ERROR_INVALID_PARAMETER;
    *origin=NULL;
    if(!host || !identity)return ERROR_INVALID_PARAMETER;
    error=acquire_payload(host,identity->payload,&item);
    if(!error) {
        error=ntvdm_vm_frontend_origin(item->frontends,identity,origin);
        release_payload(host,item);
    }
    return error;
}
DWORD ntvdm_vm_host_launch_context(ntvdm_vm_host *host,
    const common_frontend_location *identity,HANDLE *frontend,HANDLE *execution)
{
    DWORD error;
    if(!frontend || !execution || frontend==execution)return ERROR_INVALID_PARAMETER;
    *frontend=NULL;*execution=NULL;
    error=ntvdm_vm_host_origin(host,identity,frontend);
    if(!error)error=OpenNtBaseClientAcquireConsoleContext(*frontend,execution);
    if(error) {
        if(*frontend)CloseHandle(*frontend);
        if(*execution)CloseHandle(*execution);
        *frontend=NULL;*execution=NULL;
    }
    return error;
}
DWORD ntvdm_vm_host_retire(ntvdm_vm_host *host,const common_frontend_location *identity)
{
    vm_payload *item=NULL;
    DWORD error;
    if(!host || !identity)return ERROR_INVALID_PARAMETER;
    error=acquire_payload(host,identity->payload,&item);
    if(!error) {
        error=ntvdm_vm_frontend_retire(item->frontends,identity);
        release_payload(host,item);
    }
    return error;
}
DWORD ntvdm_vm_host_close(ntvdm_vm_host *host)
{
    DWORD error=ERROR_SUCCESS;
    uint64_t identity;
    if(!host)return ERROR_SUCCESS;
    AcquireSRWLockExclusive(&host->lock);
    host->closing=TRUE;
    while(host->payloads && !error) {
        identity=host->payloads->identity;
        ReleaseSRWLockExclusive(&host->lock);
        error=ntvdm_vm_payload_end(host,identity);
        if(error==ERROR_NOT_FOUND)error=ERROR_SUCCESS; /* Another closer won. */
        AcquireSRWLockExclusive(&host->lock);
    }
    ReleaseSRWLockExclusive(&host->lock);return error;
}
DWORD ntvdm_vm_host_destroy(ntvdm_vm_host *host)
{
    DWORD error=ntvdm_vm_host_close(host);
    if(!error && host)HeapFree(GetProcessHeap(),0,host);
    return error;
}
