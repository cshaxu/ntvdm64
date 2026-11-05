#include "vm_host.h"

typedef struct vm_payload {
    struct vm_payload *next;
    uint64_t identity;
    ntvdm_vm_frontends *frontends;
} vm_payload;

struct ntvdm_vm_host {
    SRWLOCK lock;
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
static DWORD release_payload(vm_payload **item)
{
    vm_payload *payload=*item;
    DWORD error=ntvdm_vm_frontends_destroy(payload->frontends);
    if(error)return error;
    *item=payload->next;HeapFree(GetProcessHeap(),0,payload);
    return ERROR_SUCCESS;
}
DWORD ntvdm_vm_host_create(ntvdm_vm_host **output)
{
    ntvdm_vm_host *host;
    if(!output)return ERROR_INVALID_PARAMETER;
    *output=NULL;
    host=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*host));
    if(!host)return ERROR_NOT_ENOUGH_MEMORY;
    InitializeSRWLock(&host->lock);*output=host;
    return ERROR_SUCCESS;
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
    vm_payload **item;
    DWORD error;
    if(!host)return ERROR_INVALID_PARAMETER;
    AcquireSRWLockExclusive(&host->lock);
    item=find_payload(host,identity);
    error=item ? release_payload(item) : ERROR_NOT_FOUND;
    ReleaseSRWLockExclusive(&host->lock);return error;
}
DWORD ntvdm_vm_host_register(ntvdm_vm_host *host,uint64_t payload,uint64_t vm,
    HANDLE input_ready,const ntvdm_vm_io *io,void *context,common_frontend_location *identity)
{
    vm_payload **item;
    DWORD error=ERROR_OPERATION_ABORTED;
    if(!identity)return ERROR_INVALID_PARAMETER;
    ZeroMemory(identity,sizeof(*identity));
    if(!host)return ERROR_INVALID_PARAMETER;
    AcquireSRWLockExclusive(&host->lock);
    item=find_payload(host,payload);
    if(!host->closing)error=item ? ntvdm_vm_frontend_register((*item)->frontends,
        vm,input_ready,io,context,identity) : ERROR_NOT_FOUND;
    ReleaseSRWLockExclusive(&host->lock);return error;
}
DWORD ntvdm_vm_host_origin(ntvdm_vm_host *host,const common_frontend_location *identity,HANDLE *origin)
{
    vm_payload **item;
    DWORD error=ERROR_OPERATION_ABORTED;
    if(!origin)return ERROR_INVALID_PARAMETER;
    *origin=NULL;
    if(!host || !identity)return ERROR_INVALID_PARAMETER;
    AcquireSRWLockExclusive(&host->lock);
    item=find_payload(host,identity->payload);
    if(!host->closing)error=item ? ntvdm_vm_frontend_origin((*item)->frontends,identity,origin) : ERROR_NOT_FOUND;
    ReleaseSRWLockExclusive(&host->lock);return error;
}
DWORD ntvdm_vm_host_retire(ntvdm_vm_host *host,const common_frontend_location *identity)
{
    vm_payload **item;
    DWORD error;
    if(!host || !identity)return ERROR_INVALID_PARAMETER;
    AcquireSRWLockExclusive(&host->lock);
    item=find_payload(host,identity->payload);
    error=item ? ntvdm_vm_frontend_retire((*item)->frontends,identity) : ERROR_NOT_FOUND;
    ReleaseSRWLockExclusive(&host->lock);return error;
}
DWORD ntvdm_vm_host_close(ntvdm_vm_host *host)
{
    DWORD error=ERROR_SUCCESS;
    if(!host)return ERROR_SUCCESS;
    AcquireSRWLockExclusive(&host->lock);
    host->closing=TRUE;
    while(host->payloads && !error)error=release_payload(&host->payloads);
    ReleaseSRWLockExclusive(&host->lock);return error;
}
DWORD ntvdm_vm_host_destroy(ntvdm_vm_host *host)
{
    DWORD error=ntvdm_vm_host_close(host);
    if(!error && host)HeapFree(GetProcessHeap(),0,host);
    return error;
}
