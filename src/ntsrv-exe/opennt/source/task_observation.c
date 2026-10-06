/* Project-added read-only process facts. No execution, scheduler, receipts,
 * Console membership, Job objects or process termination belongs here. */
#include <service_internal.h>
#include "common/native_image.h"
#include <wchar.h>

typedef struct SERVICE_OBSERVATION {
    LIST_ENTRY link;
    OPENNT_BASE_SERVICE *service;
    DWORD generation,direct_request;
    uint64_t occurrence,direct;
    DWORD psp;
    HANDLE process,wait;
    common_task_trace_node row;
} SERVICE_OBSERVATION;

/* Documented native-width ProcessBasicInformation layout, queried only for
 * inherited parent identity. No PEB dereference or remote-memory read.
 * https://learn.microsoft.com/windows/win32/api/winternl/nf-winternl-ntqueryinformationprocess */
typedef struct OBSERVATION_BASIC_INFORMATION {
    LONG exit_status;PVOID peb;ULONG_PTR affinity;LONG priority;
    ULONG_PTR pid,parent;
} OBSERVATION_BASIC_INFORMATION;
typedef LONG (NTAPI *OBSERVATION_QUERY)(HANDLE,ULONG,PVOID,ULONG,PULONG);

static SERVICE_OBSERVATION *observation_process(OPENNT_BASE_SERVICE *service,HANDLE process)
{
    LIST_ENTRY *link;
    SERVICE_COMPARE_HANDLES same=(SERVICE_COMPARE_HANDLES)GetProcAddress(
        GetModuleHandleW(L"kernelbase.dll"),"CompareObjectHandles");
    if(!same)return NULL;
    for(link=service->observations.Flink;link!=&service->observations;link=link->Flink) {
        SERVICE_OBSERVATION *node=CONTAINING_RECORD(link,SERVICE_OBSERVATION,link);
        if(node->process && same(node->process,process))return node;
    }
    return NULL;
}

static VOID CALLBACK observation_exit(void *context,BOOLEAN timed_out)
{
    SERVICE_OBSERVATION *node=context;
    DWORD code;
    if(timed_out)return; /* FALSE means the process object was signalled. */
    EnterCriticalSection(&node->service->lock);
    /* Process signal is the fact. STILL_ACTIVE may also be a legitimate
     * exit code; never use it instead of the actual signalled object. */
    if(GetExitCodeProcess(node->process,&code)) {
        node->row.reserved=code;node->row.flags|=TASK_TRACE_EXIT_KNOWN;
    }
    node->row.state=TASK_TRACE_EXITED;
    LeaveCriticalSection(&node->service->lock);
}

static DWORD observation_add(OPENNT_BASE_SERVICE *service,DWORD generation,
    DWORD direct_request,HANDLE process,uint64_t parent,DWORD flags,SERVICE_OBSERVATION **out)
{
    SERVICE_OBSERVATION *node;
    DWORD machine=0,subsystem=0,chars=260,error;
    LIST_ENTRY *link;uint32_t per_worker=0;
    *out=NULL;
    if(service->observation_count>=SERVICE_OBSERVATION_MAX || service->next_observation==UINT32_MAX)
        return ERROR_NOT_ENOUGH_QUOTA;
    for(link=service->observations.Flink;link!=&service->observations;link=link->Flink)
        if(CONTAINING_RECORD(link,SERVICE_OBSERVATION,link)->generation==generation)++per_worker;
    if(per_worker==TASK_TRACE_MAX_NODES)return ERROR_NOT_ENOUGH_QUOTA;
    node=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*node));
    if(!node)return ERROR_NOT_ENOUGH_MEMORY;
    if(!DuplicateHandle(GetCurrentProcess(),process,GetCurrentProcess(),&node->process,
        SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,0)) {
        error=GetLastError();HeapFree(GetProcessHeap(),0,node);return error;
    }
    node->service=service;node->generation=generation;node->direct_request=direct_request;
    node->row.node=(UINT64_C(1)<<32)|++service->next_observation;
    node->row.parent=parent;node->row.process_id=GetProcessId(node->process);
    node->row.task=direct_request;
    node->row.relation=direct_request ? TASK_TRACE_DIRECT : TASK_TRACE_OBSERVED;
    node->row.source=direct_request ? TASK_TRACE_SOURCE_RECORD : TASK_TRACE_SOURCE_HOOK;
    node->row.kind=UINT32_MAX; /* Missing native image must not look like DOS. */
    node->row.state=TASK_TRACE_LIVE;
    if(flags&CREATE_SUSPENDED)node->row.flags|=TASK_TRACE_REQUESTED_SUSPENDED;
    error=common_native_process_image(node->process,&machine,&subsystem);
    if(!error)node->row.kind=machine==IMAGE_FILE_MACHINE_AMD64 ? MANAGEMENT_KIND_WIN64 : MANAGEMENT_KIND_WIN32;
    else node->row.state=TASK_TRACE_UNCERTAIN; /* Missing image is not launch failure. */
    if(!QueryFullProcessImageNameW(node->process,0,node->row.image,&chars))
        lstrcpyW(node->row.image,L"<UNKNOWN>");
    /* Insert CREATE under the lock before a signalled-process callback may
     * run. Failed registration has no callback and rolls back only this fact. */
    InsertTailList(&service->observations,&node->link);++service->observation_count;
    if(!RegisterWaitForSingleObject(&node->wait,node->process,observation_exit,node,
        INFINITE,WT_EXECUTEONLYONCE)) {
        error=GetLastError();RemoveEntryList(&node->link);--service->observation_count;
        CloseHandle(node->process);HeapFree(GetProcessHeap(),0,node);return error;
    }
    *out=node;return ERROR_SUCCESS;
}

void service_observation_bind(OPENNT_BASE_SERVICE *service,DWORD generation,
    DWORD request,HANDLE target)
{
    SERVICE_OBSERVATION *node;
    /* Optional diagnostics: failure must not change Direct bind/resume. */
    if(!observation_process(service,target))
        (void)observation_add(service,generation,request,target,0,0,&node);
}

DWORD OpenNtBaseServiceObserveNativeCreation(OPENNT_BASE_SERVICE *service,HANDLE reporter,
    HANDLE child,DWORD flags,uint64_t *identity)
{
    SERVICE_OBSERVATION *parent,*node;
    OBSERVATION_BASIC_INFORMATION basic={0};ULONG bytes=0;
    OBSERVATION_QUERY query=(OBSERVATION_QUERY)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),
        "NtQueryInformationProcess");
    DWORD error=ERROR_ACCESS_DENIED;
    if(!identity)return ERROR_INVALID_PARAMETER;
    *identity=0;
    if(!service || !reporter || !child)return ERROR_INVALID_PARAMETER;
    if(!query)return ERROR_CALL_NOT_IMPLEMENTED;
    if(query(child,0,&basic,sizeof(basic),&bytes)<0 || bytes!=sizeof(basic))return ERROR_NOT_SUPPORTED;
    /* Parent overrides/unsupported ancestry remain gaps. A known reporter
     * cannot nominate some unrelated same-user process as its own child. */
    if(basic.parent!=(ULONG_PTR)GetProcessId(reporter))return ERROR_NOT_SUPPORTED;
    EnterCriticalSection(&service->lock);
    parent=observation_process(service,reporter);
    if(!parent || !service_find_worker_watch(service,parent->generation))goto done;
    node=observation_process(service,child);
    if(node) {
        if(node->generation==parent->generation && node->row.parent==parent->row.node)
            {*identity=node->row.node;error=ERROR_SUCCESS;} /* Idempotent actual object, not PID replay. */
        goto done;
    }
    error=observation_add(service,parent->generation,0,child,parent->row.node,flags,&node);
    if(error==ERROR_NOT_ENOUGH_QUOTA)parent->row.flags|=TASK_TRACE_TRUNCATED;
    if(!error)*identity=node->row.node;
done:
    LeaveCriticalSection(&service->lock);return error;
}

uint64_t service_observation_direct_id(OPENNT_BASE_SERVICE *service,DWORD generation,DWORD request)
{
    LIST_ENTRY *link;
    for(link=service->observations.Flink;link!=&service->observations;link=link->Flink) {
        SERVICE_OBSERVATION *node=CONTAINING_RECORD(link,SERVICE_OBSERVATION,link);
        if(node->generation==generation && node->direct_request==request)return node->row.node;
    }
    return request;
}

void service_observation_copy(OPENNT_BASE_SERVICE *service,DWORD generation,
    common_task_trace_node *rows,uint32_t *count,uint32_t *coverage)
{
    LIST_ENTRY *link;uint32_t index;
    for(link=service->observations.Flink;link!=&service->observations;link=link->Flink) {
        SERVICE_OBSERVATION *node=CONTAINING_RECORD(link,SERVICE_OBSERVATION,link);
        if(node->generation!=generation)continue;
        *coverage|=node->row.flags&TASK_TRACE_TRUNCATED;
        for(index=0;index<*count;++index)if(rows[index].node==node->row.node)break;
        if(index<*count) {
            /* Independent process state never completes the execution record. */
            rows[index].state=node->row.state;rows[index].flags|=node->row.flags;
            rows[index].reserved=node->row.reserved;
            rows[index].dos_psp=node->row.dos_psp;
        } else if(*count<TASK_TRACE_MAX_NODES) {
            rows[*count]=node->row;
            if(node->direct_request)rows[*count].flags|=TASK_TRACE_HISTORY;
            ++*count;
        } else *coverage|=TASK_TRACE_TRUNCATED;
    }
}

DWORD service_observation_dos_bind(OPENNT_BASE_SERVICE *service,DWORD generation,
    uint64_t identity,DWORD task,PCWSTR image)
{
    LIST_ENTRY *link;uint32_t per_worker=0;SERVICE_OBSERVATION *node;
    if(!identity)return ERROR_INVALID_PARAMETER;
    for(link=service->observations.Flink;link!=&service->observations;link=link->Flink) {
        node=CONTAINING_RECORD(link,SERVICE_OBSERVATION,link);
        if(node->generation!=generation)continue;
        if(node->row.node==identity)return node->row.kind==MANAGEMENT_KIND_DOS ? ERROR_SUCCESS : ERROR_INVALID_DATA;
        ++per_worker;
    }
    if(service->observation_count>=SERVICE_OBSERVATION_MAX || per_worker>=TASK_TRACE_MAX_NODES)
        return ERROR_NOT_ENOUGH_QUOTA;
    node=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*node));
    if(!node)return ERROR_NOT_ENOUGH_MEMORY;
    node->service=service;node->generation=generation;node->direct=identity;
    node->direct_request=UINT32_MAX; /* Direct-history marker, not a native request. */
    node->row.node=identity;node->row.task=task;node->row.relation=TASK_TRACE_DIRECT;
    node->row.source=TASK_TRACE_SOURCE_RECORD;node->row.kind=MANAGEMENT_KIND_DOS;
    node->row.state=TASK_TRACE_UNCERTAIN; /* Delivery does not prove guest entry. */
    lstrcpynW(node->row.image,image && image[0] ? image : L"<UNKNOWN>",260);
    InsertTailList(&service->observations,&node->link);++service->observation_count;
    return ERROR_SUCCESS;
}

DWORD OpenNtBaseServiceObserveDosEvent(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,const common_dos_observation *fact,BOOL gap)
{
    OPENNT_BASE_SERVICE *service;OPENNT_BASE_WORKER_WATCH *watch;
    SERVICE_OBSERVATION *root=NULL,*node=NULL,*parent=NULL;LIST_ENTRY *link;
    uint32_t per_worker=0;DWORD error=ERROR_NOT_FOUND;
    if(!connection || !fact || !OpenNtBaseServicePeer(connection,pid,generation))return ERROR_ACCESS_DENIED;
    if(fact->flags || fact->psp>UINT16_MAX || fact->parent_psp>UINT16_MAX ||
        (fact->occurrence && fact->occurrence==fact->parent) ||
        (fact->event!=DOS_OBSERVATION_GAP && (!fact->occurrence || !fact->psp || !fact->direct)) ||
        (fact->event!=DOS_OBSERVATION_ENTER && fact->event!=DOS_OBSERVATION_EXIT && fact->event!=DOS_OBSERVATION_GAP) ||
        wcsnlen(fact->image,260)==260)return ERROR_INVALID_DATA;
    service=connection->service;EnterCriticalSection(&service->lock);
    watch=service_find_worker_watch(service,generation);
    if(!watch || watch->wow || watch->kind==OPENNT_BASE_WORKER_NATIVE || !connection->process.fVDM)
        {error=ERROR_ACCESS_DENIED;goto done;}
    for(link=service->observations.Flink;link!=&service->observations;link=link->Flink) {
        SERVICE_OBSERVATION *item=CONTAINING_RECORD(link,SERVICE_OBSERVATION,link);
        if(item->generation!=generation)continue;
        ++per_worker;
        if(item->row.node==fact->direct && item->row.relation==TASK_TRACE_DIRECT &&
            item->row.kind==MANAGEMENT_KIND_DOS && !item->process)root=item;
        if(item->occurrence && item->occurrence==fact->occurrence)node=item;
        if(item->occurrence && item->occurrence==fact->parent)parent=item;
        if(gap || fact->event==DOS_OBSERVATION_GAP)item->row.flags|=TASK_TRACE_GAP_OBSERVATION;
    }
    if(fact->event==DOS_OBSERVATION_GAP){error=ERROR_SUCCESS;goto done;}
    if(!root)goto done; /* Actual delivered Direct identity, not current task. */
    if(node) {
        if(node->direct!=fact->direct || node->psp!=fact->psp)
            {error=ERROR_INVALID_DATA;goto done;}
        if(fact->event==DOS_OBSERVATION_EXIT)node->row.state=TASK_TRACE_EXITED;
        /* An ENTER replay cannot turn a terminated occurrence live again. */
        error=ERROR_SUCCESS;goto done;
    }
    if(fact->event==DOS_OBSERVATION_EXIT)goto done;
    if(fact->parent && (!parent || parent==root && !root->occurrence))
        {root->row.flags|=TASK_TRACE_GAP_OBSERVATION;parent=NULL;}
    if(parent && parent->psp!=fact->parent_psp){error=ERROR_INVALID_DATA;goto done;}
    if(!root->occurrence)node=root;
    else {
        if(service->observation_count>=SERVICE_OBSERVATION_MAX || per_worker>=TASK_TRACE_MAX_NODES ||
            service->next_observation==UINT32_MAX) {
            root->row.flags|=TASK_TRACE_TRUNCATED;error=ERROR_NOT_ENOUGH_QUOTA;goto done;
        }
        node=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*node));
        if(!node){error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
        node->service=service;node->generation=generation;node->direct=fact->direct;
        node->row.node=(UINT64_C(1)<<32)|++service->next_observation;
        node->row.relation=TASK_TRACE_OBSERVED;node->row.source=TASK_TRACE_SOURCE_DOS;
        node->row.kind=MANAGEMENT_KIND_DOS;node->row.task=fact->psp;
        lstrcpynW(node->row.image,fact->image[0] ? fact->image : L"<UNKNOWN>",260);
        InsertTailList(&service->observations,&node->link);++service->observation_count;
    }
    node->occurrence=fact->occurrence;
    node->psp=fact->psp;
    node->row.dos_psp=fact->psp;
    node->row.parent=parent ? parent->row.node : 0;
    /* Last confirmed guest entry is not proof of current foreground or TSR
     * residency. Only the original termination callback proves EXIT. */
    node->row.state=TASK_TRACE_UNCERTAIN;error=ERROR_SUCCESS;
done:
    LeaveCriticalSection(&service->lock);return error;
}

void service_observation_stop(OPENNT_BASE_SERVICE *service)
{
    /* Calls are drained. Callback may need service->lock, so never hold it
     * while unregistering/draining or free a still-observed process handle. */
    while(!IsListEmpty(&service->observations)) {
        LIST_ENTRY *entry=RemoveHeadList(&service->observations);
        SERVICE_OBSERVATION *node=CONTAINING_RECORD(entry,SERVICE_OBSERVATION,link);
        if(node->wait && !UnregisterWaitEx(node->wait,INVALID_HANDLE_VALUE))
            RaiseFailFastException(NULL,NULL,0);
        if(node->process)CloseHandle(node->process);
        HeapFree(GetProcessHeap(),0,node);
    }
    service->observation_count=0;
}
