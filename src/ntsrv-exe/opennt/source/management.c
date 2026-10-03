/* NTSRV-private management; project adaptation,
 * not an original OpenNT mirror. Physical separation only: existing function
 * bodies, state authority and lock/resource contracts are preserved. */
#include <service_internal.h>

static DWORD service_win32record_depth(const OPENNT_BASE_CONNECTION *connection);
static void service_copy_management_text(PCSTR text,ULONG length,
    WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS]);
static void service_copy_management_image(PVDMINFO info,WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS]);
static BOOL service_same_management_wait(HANDLE left,HANDLE right);
static OPENNT_BASE_MANAGEMENT_LABEL *service_find_management_label(
    OPENNT_BASE_WORKER_WATCH *watch,PDOSRECORD record,HANDLE parent_wait);
static void service_prune_management_labels(OPENNT_BASE_WORKER_WATCH *watch,
    PCONSOLERECORD console);
static void service_capture_management_label(OPENNT_BASE_WORKER_WATCH *watch,
    PDOSRECORD record,ULONG task_id,const WCHAR *image);
static void service_capture_management_label_from_info(OPENNT_BASE_WORKER_WATCH *watch,
    PDOSRECORD record);
static OPENNT_BASE_WORKER_WATCH *service_find_management_watch_for_console(
    OPENNT_BASE_SERVICE *service,HANDLE console);
static void service_copy_management_record(OPENNT_BASE_WORKER_WATCH *watch,
    OPENNT_BASE_WORKER_INFO *item);
static void service_sort_management_records(OPENNT_BASE_WORKER_INFO *entries,uint32_t count);
static void service_copy_win32record(OPENNT_BASE_CONNECTION *native,
    OPENNT_BASE_WORKER_INFO *item);

static DWORD service_win32record_depth(const OPENNT_BASE_CONNECTION *connection)
{
    const LIST_ENTRY *link;
    DWORD depth=0;
    /* Only broker-admitted Direct records exist in this management stack. */
    for(link=connection->win32records.Flink;link!=&connection->win32records;link=link->Flink)
        if(!CONTAINING_RECORD(link,OPENNT_BASE_WIN32RECORD,link)->completed)++depth;
    return depth;
}

/* The task manager is deliberately a projection of the original VDM lists,
 * not a second task registry.  All original list mutations in this
 * composition occur under service->lock; the original locks below preserve
 * the source owners' own DOS/WOW traversal contract as well. */
static void service_copy_management_text(PCSTR text,ULONG length,
    WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS])
{
    int copied;
    if (!text || !length) return;
    copied=MultiByteToWideChar(CP_ACP,0,(LPCCH)text,(int)length,
        image,OPENNT_BASE_WORKER_IMAGE_CHARS-1);
    if (copied>0) image[copied]=L'\0';
}

static void service_copy_management_image(PVDMINFO info,WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS])
{
    if (info) service_copy_management_text(info->AppName,info->AppLen,image);
}

static BOOL service_same_management_wait(HANDLE left,HANDLE right)
{
    /* BaseSrvGetVDMExitCode compares this source carrier after stripping the
     * historical low-bit tag.  The display sidecar uses the same identity. */
    return (((ULONG_PTR)left & ~(ULONG_PTR)1)==((ULONG_PTR)right & ~(ULONG_PTR)1));
}

void service_clear_management_labels(OPENNT_BASE_WORKER_WATCH *watch)
{
    while (watch && !IsListEmpty(&watch->management_labels)) {
        LIST_ENTRY *link=RemoveHeadList(&watch->management_labels);
        HeapFree(GetProcessHeap(),0,CONTAINING_RECORD(link,
            OPENNT_BASE_MANAGEMENT_LABEL,link));
    }
}

static OPENNT_BASE_MANAGEMENT_LABEL *service_find_management_label(
    OPENNT_BASE_WORKER_WATCH *watch,PDOSRECORD record,HANDLE parent_wait)
{
    LIST_ENTRY *link;
    if (!watch || !record) return NULL;
    for (link=watch->management_labels.Flink;
         link!=&watch->management_labels;link=link->Flink) {
        OPENNT_BASE_MANAGEMENT_LABEL *label=CONTAINING_RECORD(link,
            OPENNT_BASE_MANAGEMENT_LABEL,link);
        if (label->record==record &&
            service_same_management_wait(label->parent_wait,parent_wait)) return label;
    }
    return NULL;
}

static void service_prune_management_labels(OPENNT_BASE_WORKER_WATCH *watch,
    PCONSOLERECORD console)
{
    LIST_ENTRY *link,*next;
    if (!watch || !console) return;
    for (link=watch->management_labels.Flink;
         link!=&watch->management_labels;link=next) {
        OPENNT_BASE_MANAGEMENT_LABEL *label=CONTAINING_RECORD(link,
            OPENNT_BASE_MANAGEMENT_LABEL,link);
        PDOSRECORD dos;
        BOOL live=FALSE;
        next=link->Flink;
        for (dos=console->DOSRecord;dos;dos=dos->DOSRecordNext) {
            if (dos==label->record &&
                service_same_management_wait(dos->hWaitForParent,label->parent_wait)) {
                live=TRUE;
                break;
            }
        }
        if (!live) {
            RemoveEntryList(link);
            HeapFree(GetProcessHeap(),0,label);
        }
    }
}

static void service_capture_management_label(OPENNT_BASE_WORKER_WATCH *watch,
    PDOSRECORD record,ULONG task_id,const WCHAR *image)
{
    OPENNT_BASE_MANAGEMENT_LABEL *label;
    if (!watch || !record || !image || !image[0]) return;
    label=service_find_management_label(watch,record,record->hWaitForParent);
    if (!label) {
        label=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*label));
        if (!label) return; /* The display projection never alters delivery. */
        label->record=record;
        label->parent_wait=record->hWaitForParent;
        InsertTailList(&watch->management_labels,&label->link);
    }
    label->task=task_id;
    lstrcpynW(label->image,image,OPENNT_BASE_WORKER_IMAGE_CHARS);
}

static void service_capture_management_label_from_info(OPENNT_BASE_WORKER_WATCH *watch,
    PDOSRECORD record)
{
    WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS]={0};
    if (!record || !record->lpVDMInfo) return;
    service_copy_management_image(record->lpVDMInfo,image);
    service_capture_management_label(watch,record,record->lpVDMInfo->iTask,image);
}

void service_capture_initial_management_labels(OPENNT_BASE_WORKER_WATCH *watch)
{
    PCONSOLERECORD console;
    PDOSRECORD dos;
    if (!watch || watch->kind!=OPENNT_BASE_WORKER_DOS) return;
    (void)RtlEnterCriticalSection(&BaseSrvDOSCriticalSection);
    for (console=DOSHead;console;console=console->Next) {
        if (console->SequenceNumber!=watch->process.SequenceNumber) continue;
        for (dos=console->DOSRecord;dos;dos=dos->DOSRecordNext)
            service_capture_management_label_from_info(watch,dos);
        break;
    }
    RtlLeaveCriticalSection(&BaseSrvDOSCriticalSection);
}

static OPENNT_BASE_WORKER_WATCH *service_find_management_watch_for_console(
    OPENNT_BASE_SERVICE *service,HANDLE console)
{
    LIST_ENTRY *link;
    if (!service || !console) return NULL;
    for (link=service->worker_watches.Flink;
         link!=&service->worker_watches;link=link->Flink) {
        OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,
            OPENNT_BASE_WORKER_WATCH,link);
        if (watch->kind==OPENNT_BASE_WORKER_DOS && watch->console==console) return watch;
    }
    return NULL;
}

void service_capture_checked_management_label(OPENNT_BASE_SERVICE *service,
    HANDLE console,const BASE_CHECKVDM_MSG *command)
{
    OPENNT_BASE_WORKER_WATCH *watch;
    PCONSOLERECORD source_console;
    PDOSRECORD dos;
    WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS]={0};
    if (!service || !console || !command || !command->AppName || !command->AppLen) return;
    watch=service_find_management_watch_for_console(service,console);
    if (!watch) return;
    service_copy_management_text(command->AppName,command->AppLen,image);
    if (!image[0]) return;
    (void)RtlEnterCriticalSection(&BaseSrvDOSCriticalSection);
    for (source_console=DOSHead;source_console;source_console=source_console->Next) {
        if (source_console->hConsole!=console) continue;
        for (dos=source_console->DOSRecord;dos;dos=dos->DOSRecordNext) {
            if (service_same_management_wait(dos->hWaitForParent,
                    command->WaitObjectForParent)) {
                service_capture_management_label(watch,dos,command->iTask,image);
                break;
            }
        }
        break;
    }
    RtlLeaveCriticalSection(&BaseSrvDOSCriticalSection);
}

static void service_copy_management_record(OPENNT_BASE_WORKER_WATCH *watch,
    OPENNT_BASE_WORKER_INFO *item)
{
    PCONSOLERECORD console;
    PDOSRECORD dos,selected=NULL;
    PWOWRECORD wow,selected_wow=NULL;
    if(watch->kind==OPENNT_BASE_WORKER_NATIVE) {
        item->kind=2u; /* Win32 text worker */
        /* Execution request/member publication is a separate binding. A
         * claimed process alone is not proof of native command readiness. */
        item->state=0;
        return;
    }
    if (watch->wow) {
        (void)RtlEnterCriticalSection(&BaseSrvWOWCriticalSection);
        if (WOWHead && WOWHead->SequenceNumber==watch->process.SequenceNumber) {
            for (wow=WOWHead->WOWRecord;wow;wow=wow->WOWRecordNext) {
                ++item->stack_depth;
                if (!selected_wow || (!selected_wow->fDispatched && wow->fDispatched))
                    selected_wow=wow;
            }
            if (selected_wow) {
                item->kind=1u; /* WOW16 / Win16 worker */
                item->state=selected_wow->fDispatched ? VDM_BUSY : VDM_READY;
                item->task=selected_wow->iTask;
                service_copy_management_image(selected_wow->lpVDMInfo,item->image);
            }
        }
        RtlLeaveCriticalSection(&BaseSrvWOWCriticalSection);
        return;
    }
    (void)RtlEnterCriticalSection(&BaseSrvDOSCriticalSection);
    for (console=DOSHead;console;console=console->Next) {
        if (console->SequenceNumber!=watch->process.SequenceNumber) continue;
        service_prune_management_labels(watch,console);
        for (dos=console->DOSRecord;dos;dos=dos->DOSRecordNext) {
            /* This chain is the original BaseSrv execution stack.  A returned
             * record is retained only until the parent consumes its exit code;
             * it is not a running child.  READY is the resident PermCom and
             * likewise has visible depth zero. */
            if (dos->VDMState==VDM_BUSY || dos->VDMState==VDM_TO_TAKE_A_COMMAND) {
                ++item->stack_depth;
                selected=dos; /* Tail is the currently selected child. */
            }
        }
        break;
    }
    if (selected) {
        OPENNT_BASE_MANAGEMENT_LABEL *label;
        item->kind=0u; /* DOS worker */
        item->state=selected->VDMState;
        if (selected->lpVDMInfo) {
            item->task=selected->lpVDMInfo->iTask;
            service_copy_management_image(selected->lpVDMInfo,item->image);
        } else if ((label=service_find_management_label(watch,selected,
                selected->hWaitForParent))!=NULL) {
            item->task=label->task;
            lstrcpynW(item->image,label->image,OPENNT_BASE_WORKER_IMAGE_CHARS);
        }
    }
    RtlLeaveCriticalSection(&BaseSrvDOSCriticalSection);
}

static void service_sort_management_records(OPENNT_BASE_WORKER_INFO *entries,uint32_t count)
{
    uint32_t index,insert;
    /* Original lists are ownership chains, not a UI ordering contract. */
    for (index=1;index<count;++index) {
        OPENNT_BASE_WORKER_INFO value=entries[index];
        for (insert=index;insert &&
            (entries[insert-1].sequence>value.sequence ||
             (entries[insert-1].sequence==value.sequence && entries[insert-1].task>value.task));--insert)
            entries[insert]=entries[insert-1];
        entries[insert]=value;
    }
}

static void service_copy_win32record(OPENNT_BASE_CONNECTION *native,
    OPENNT_BASE_WORKER_INFO *item)
{
    OPENNT_BASE_WIN32RECORD *record=NULL;
    {
        LIST_ENTRY *link;
        for(link=native->win32records.Blink;link!=&native->win32records;link=link->Blink) {
            OPENNT_BASE_WIN32RECORD *candidate=CONTAINING_RECORD(link,OPENNT_BASE_WIN32RECORD,link);
            if(!candidate->completed){record=candidate;break;}
        }
    }
    /* Only admitted Direct requests populate TASK/STACK. */
    item->state=native->native_inflight ? VDM_BUSY : VDM_READY;
    item->stack_depth=service_win32record_depth(native);
    item->task=record ? record->request : 0;
    lstrcpynW(item->image,record ? record->image : L"<EMPTY>",
        OPENNT_BASE_WORKER_IMAGE_CHARS);
    if(WaitForSingleObject(native->native_stop,0)==WAIT_OBJECT_0)item->state|=0x80000000u;
}

DWORD OpenNtBaseServiceSnapshot(OPENNT_BASE_SERVICE *service,uint64_t *epoch,
    OPENNT_BASE_WORKER_INFO *entries,uint32_t capacity,uint32_t *count)
{
    LIST_ENTRY *link;
    uint32_t needed=0,index=0;
    if (!service || !epoch || !count) return ERROR_INVALID_PARAMETER;
    *count=0; *epoch=0;
    EnterCriticalSection(&service->lock);
    for (link=service->worker_watches.Flink;link!=&service->worker_watches;link=link->Flink) ++needed;
    *epoch=service->management_epoch;
    *count=needed;
    if (needed>capacity || (needed && !entries)) {
        LeaveCriticalSection(&service->lock);
        return ERROR_INSUFFICIENT_BUFFER;
    }
    for (link=service->worker_watches.Flink;link!=&service->worker_watches;link=link->Flink) {
        OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
        OPENNT_BASE_WORKER_INFO *item=&entries[index++];
        ZeroMemory(item,sizeof(*item));
        item->sequence=watch->process.SequenceNumber;
        item->process_id=(DWORD)(ULONG_PTR)watch->process.ClientId.UniqueProcess;
        item->kind=watch->wow ? 1u : 0u;
        item->state=watch->termination_requested ? 0x80000000u : VDM_READY;
        item->started_filetime=((uint64_t)watch->started.dwHighDateTime<<32)|watch->started.dwLowDateTime;
        service_copy_management_record(watch,item);
        if(watch->kind==OPENNT_BASE_WORKER_NATIVE) {
            LIST_ENTRY *entry;
            for(entry=service->connections.Flink;entry!=&service->connections;entry=entry->Flink) {
                OPENNT_BASE_CONNECTION *native=CONTAINING_RECORD(entry,OPENNT_BASE_CONNECTION,service_link);
                if(native->process.SequenceNumber!=watch->process.SequenceNumber || !native->native_worker)continue;
                service_copy_win32record(native,item);
                break;
            }
        }
        if (watch->termination_requested) item->state|=0x80000000u;
        /* A resident worker without an original VDMINFO has no active
         * product task. Do not misrepresent its historical launch image as
         * current work. */
        if (!item->image[0])
            lstrcpynW(item->image,L"<EMPTY>",OPENNT_BASE_WORKER_IMAGE_CHARS);
    }
    *count=index;
    service_sort_management_records(entries,index);
    LeaveCriticalSection(&service->lock);
    return ERROR_SUCCESS;
}
