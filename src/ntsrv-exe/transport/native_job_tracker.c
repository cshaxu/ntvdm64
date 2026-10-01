#include "native_job_tracker.h"
#include <winternl.h>

typedef NTSTATUS (NTAPI *OPENNT_NT_QUERY_INFORMATION_PROCESS)(HANDLE,
    PROCESSINFOCLASS,PVOID,ULONG,PULONG);
typedef struct OPENNT_NATIVE_JOB_NODE OPENNT_NATIVE_JOB_NODE;
struct OPENNT_NATIVE_JOB_TRACKER {
    HANDLE port,thread;
    CRITICAL_SECTION lock;
    LIST_ENTRY nodes;
    OPENNT_NATIVE_JOB_REPORT report;
};
struct OPENNT_NATIVE_JOB_NODE {
    LIST_ENTRY link;
    OPENNT_NATIVE_JOB_TRACKER *tracker;
    HANDLE job;
    void *owner;
    DWORD generation;
    DWORD request;
    DWORD root_process;
    BOOL bound;
};
static void native_list_initialize(LIST_ENTRY *head)
{ head->Flink=head->Blink=head; }
static BOOL native_list_empty(const LIST_ENTRY *head)
{ return head->Flink==head; }
static void native_list_insert_tail(LIST_ENTRY *head,LIST_ENTRY *entry)
{
    entry->Blink=head->Blink;entry->Flink=head;
    head->Blink->Flink=entry;head->Blink=entry;
}
static void native_list_remove(LIST_ENTRY *entry)
{ entry->Blink->Flink=entry->Flink;entry->Flink->Blink=entry->Blink; }
static LIST_ENTRY *native_list_remove_head(LIST_ENTRY *head)
{ LIST_ENTRY *entry=head->Flink;native_list_remove(entry);return entry; }
static DWORD native_parent_process(DWORD process)
{
    PROCESS_BASIC_INFORMATION information;
    OPENNT_NT_QUERY_INFORMATION_PROCESS query;
    HANDLE handle;DWORD parent=0;
    query=(OPENNT_NT_QUERY_INFORMATION_PROCESS)GetProcAddress(
        GetModuleHandleW(L"ntdll.dll"),"NtQueryInformationProcess");
    if(!query)return 0;
    handle=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,process);
    if(!handle)return 0;
    if(query(handle,ProcessBasicInformation,&information,sizeof(information),NULL)>=0)
        parent=(DWORD)(ULONG_PTR)information.Reserved3;
    CloseHandle(handle);return parent;
}
static void native_job_node_delete(OPENNT_NATIVE_JOB_NODE *node)
{
    if(node->job)CloseHandle(node->job);
    HeapFree(GetProcessHeap(),0,node);
}
static DWORD WINAPI native_job_completion_thread(void *context)
{
    OPENNT_NATIVE_JOB_TRACKER *tracker=context;
    for(;;) {
        DWORD event=0,payload=0;ULONG_PTR key=0;LPOVERLAPPED overlapped=NULL;
        OPENNT_NATIVE_JOB_NODE *node;void *owner=NULL;
        if(!GetQueuedCompletionStatus(tracker->port,&event,&key,&overlapped,INFINITE))continue;
        if(!key && !overlapped)return 0;
        node=(OPENNT_NATIVE_JOB_NODE *)key;
        payload=(DWORD)(ULONG_PTR)overlapped;
        EnterCriticalSection(&tracker->lock);owner=node->owner;LeaveCriticalSection(&tracker->lock);
        if(owner && event==JOB_OBJECT_MSG_NEW_PROCESS)
            tracker->report(owner,node->generation,node->request,event,payload,
                native_parent_process(payload),node->root_process);
        else if(owner && event==JOB_OBJECT_MSG_EXIT_PROCESS)
            tracker->report(owner,node->generation,node->request,event,payload,0,node->root_process);
        else if(event==JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO) {
            if(owner)tracker->report(owner,node->generation,node->request,event,0,0,node->root_process);
            EnterCriticalSection(&tracker->lock);
            native_list_remove(&node->link);node->owner=NULL;
            LeaveCriticalSection(&tracker->lock);
            native_job_node_delete(node);
        }
    }
}
DWORD OpenNtNativeJobTrackerOpen(OPENNT_NATIVE_JOB_TRACKER **output,
    OPENNT_NATIVE_JOB_REPORT report)
{
    OPENNT_NATIVE_JOB_TRACKER *tracker;
    if(!output || !report)return ERROR_INVALID_PARAMETER;
    *output=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*tracker));
    if(!*output)return ERROR_NOT_ENOUGH_MEMORY;
    tracker=*output;tracker->report=report;InitializeCriticalSection(&tracker->lock);
    native_list_initialize(&tracker->nodes);
    tracker->port=CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,1);
    if(!tracker->port)goto fail;
    tracker->thread=CreateThread(NULL,0,native_job_completion_thread,tracker,0,NULL);
    if(tracker->thread)return ERROR_SUCCESS;
fail:
    { DWORD error=GetLastError();if(tracker->port)CloseHandle(tracker->port);
      DeleteCriticalSection(&tracker->lock);HeapFree(GetProcessHeap(),0,tracker);*output=NULL;return error; }
}
DWORD OpenNtNativeJobTrackerCreate(OPENNT_NATIVE_JOB_TRACKER *tracker,void *owner,
    DWORD generation,DWORD request)
{
    JOBOBJECT_ASSOCIATE_COMPLETION_PORT association;
    OPENNT_NATIVE_JOB_NODE *node;
    if(!tracker || !owner || !request)return ERROR_INVALID_PARAMETER;
    node=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*node));
    if(!node)return ERROR_NOT_ENOUGH_MEMORY;
    node->tracker=tracker;node->owner=owner;node->generation=generation;node->request=request;
    node->job=CreateJobObjectW(NULL,NULL);
    if(!node->job)goto fail;
    association.CompletionKey=node;association.CompletionPort=tracker->port;
    if(!SetInformationJobObject(node->job,JobObjectAssociateCompletionPortInformation,
        &association,sizeof(association)))goto fail;
    EnterCriticalSection(&tracker->lock);native_list_insert_tail(&tracker->nodes,&node->link);
    LeaveCriticalSection(&tracker->lock);return ERROR_SUCCESS;
fail:
    { DWORD error=GetLastError();native_job_node_delete(node);return error; }
}
static OPENNT_NATIVE_JOB_NODE *native_job_find(OPENNT_NATIVE_JOB_TRACKER *tracker,
    void *owner,DWORD generation,DWORD request)
{
    LIST_ENTRY *link;
    for(link=tracker->nodes.Flink;link!=&tracker->nodes;link=link->Flink) {
        OPENNT_NATIVE_JOB_NODE *node=CONTAINING_RECORD(link,OPENNT_NATIVE_JOB_NODE,link);
        if(node->owner==owner && node->generation==generation && node->request==request)return node;
    }
    return NULL;
}
DWORD OpenNtNativeJobTrackerAssign(OPENNT_NATIVE_JOB_TRACKER *tracker,void *owner,
    DWORD generation,DWORD request,HANDLE process)
{
    OPENNT_NATIVE_JOB_NODE *node;DWORD error=ERROR_NOT_FOUND;
    if(!tracker || !process)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&tracker->lock);
    node=native_job_find(tracker,owner,generation,request);
    if(node && !node->bound) {
        if(AssignProcessToJobObject(node->job,process)) {
            node->root_process=GetProcessId(process);node->bound=TRUE;error=ERROR_SUCCESS;
        }
        else error=GetLastError();
    }
    LeaveCriticalSection(&tracker->lock);return error;
}
void OpenNtNativeJobTrackerDiscard(OPENNT_NATIVE_JOB_TRACKER *tracker,void *owner,
    DWORD generation,DWORD request)
{
    OPENNT_NATIVE_JOB_NODE *node=NULL;if(!tracker)return;
    EnterCriticalSection(&tracker->lock);node=native_job_find(tracker,owner,generation,request);
    if(node && !node->bound)native_list_remove(&node->link);else node=NULL;
    LeaveCriticalSection(&tracker->lock);if(node)native_job_node_delete(node);
}
void OpenNtNativeJobTrackerClose(OPENNT_NATIVE_JOB_TRACKER *tracker)
{
    if(!tracker)return;
    PostQueuedCompletionStatus(tracker->port,0,0,NULL);WaitForSingleObject(tracker->thread,INFINITE);
    CloseHandle(tracker->thread);CloseHandle(tracker->port);
    while(!native_list_empty(&tracker->nodes)) {
        OPENNT_NATIVE_JOB_NODE *node=CONTAINING_RECORD(native_list_remove_head(&tracker->nodes),
            OPENNT_NATIVE_JOB_NODE,link);native_job_node_delete(node);
    }
    DeleteCriticalSection(&tracker->lock);HeapFree(GetProcessHeap(),0,tracker);
}
