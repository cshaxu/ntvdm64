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
static void service_copy_vdm_management_record(OPENNT_BASE_WORKER_WATCH *watch,
    OPENNT_BASE_WORKER_INFO *item);
static void service_sort_management_records(OPENNT_BASE_WORKER_INFO *entries,uint32_t count);
static void service_copy_win32record(OPENNT_BASE_CONNECTION *native,
    OPENNT_BASE_WORKER_INFO *item);
static void service_copy_worker(OPENNT_BASE_WORKER_WATCH *,OPENNT_BASE_WORKER_INFO *);
static DWORD service_copy_management_tree(OPENNT_BASE_SERVICE *,
    OPENNT_BASE_WORKER_INFO **,uint32_t *,uint32_t *);

static DWORD service_win32record_depth(const OPENNT_BASE_CONNECTION *connection)
{
    const LIST_ENTRY *link;
    DWORD depth=0;
    /* Only broker-admitted Direct records exist in this management stack. */
    for(link=connection->win32records.Flink;link!=&connection->win32records;link=link->Flink)
        if(!CONTAINING_RECORD(link,OPENNT_BASE_WIN32RECORD,link)->completed)++depth;
    return depth;
}

static OPENNT_BASE_MANAGEMENT_LABEL *service_wow_label(OPENNT_BASE_WORKER_WATCH *watch,
    PWOWRECORD record)
{
    LIST_ENTRY *link;
    for(link=watch->management_labels.Flink;link!=&watch->management_labels;link=link->Flink) {
        OPENNT_BASE_MANAGEMENT_LABEL *label=CONTAINING_RECORD(link,OPENNT_BASE_MANAGEMENT_LABEL,link);
        if(label->wow_record==record && service_same_management_wait(label->parent_wait,record->hWaitForParent))
            return label;
    }
    return NULL;
}

/* Original WOW list owns lifetime. Sidecars retain only admission identity and
 * text that original GetNext frees; prune before capturing reused addresses. */
static void service_capture_wow_labels(OPENNT_BASE_WORKER_WATCH *watch)
{
    LIST_ENTRY *link,*next;
    PWOWRECORD record;
    if(!WOWHead || WOWHead->SequenceNumber!=watch->process.SequenceNumber) {
        service_clear_management_labels(watch);return;
    }
    for(link=watch->management_labels.Flink;link!=&watch->management_labels;link=next) {
        OPENNT_BASE_MANAGEMENT_LABEL *label=CONTAINING_RECORD(link,OPENNT_BASE_MANAGEMENT_LABEL,link);
        BOOL live=FALSE;
        next=link->Flink;
        for(record=WOWHead->WOWRecord;record;record=record->WOWRecordNext)
            if(label->wow_record==record && service_same_management_wait(label->parent_wait,record->hWaitForParent))
                {live=TRUE;break;}
        if(!live){RemoveEntryList(link);HeapFree(GetProcessHeap(),0,label);}
    }
    for(record=WOWHead->WOWRecord;record;record=record->WOWRecordNext) {
        OPENNT_BASE_MANAGEMENT_LABEL *label=service_wow_label(watch,record);
        if(!label) {
            if(watch->service->next_management_task==UINT64_MAX)continue;
            label=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*label));
            if(!label)continue; /* Never prevent original delivery. */
            label->wow_record=record;label->parent_wait=record->hWaitForParent;
            label->identity=++watch->service->next_management_task;
            InsertTailList(&watch->management_labels,&label->link);
        }
        label->task=record->iTask;
        if(record->lpVDMInfo)service_copy_management_image(record->lpVDMInfo,label->image);
    }
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
    if(!watch)return;
    if(watch->wow) {
        (void)RtlEnterCriticalSection(&BaseSrvWOWCriticalSection);
        service_capture_wow_labels(watch);
        RtlLeaveCriticalSection(&BaseSrvWOWCriticalSection);
        return;
    }
    if (watch->kind!=OPENNT_BASE_WORKER_DOS) return;
    (void)RtlEnterCriticalSection(&BaseSrvDOSCriticalSection);
    for (console=DOSHead;console;console=console->Next) {
        if (console->SequenceNumber!=watch->process.SequenceNumber) continue;
        for (dos=console->DOSRecord;dos;dos=dos->DOSRecordNext)
            service_capture_management_label_from_info(watch,dos);
        break;
    }
    RtlLeaveCriticalSection(&BaseSrvDOSCriticalSection);
}

void service_capture_wow_management_labels(OPENNT_BASE_CONNECTION *connection)
{
    LIST_ENTRY *link;
    for(link=connection->service->worker_watches.Flink;
        link!=&connection->service->worker_watches;link=link->Flink) {
        OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
        if(watch->process.SequenceNumber==connection->process.SequenceNumber && watch->wow) {
            service_capture_initial_management_labels(watch);break;
        }
    }
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

static void service_copy_vdm_management_record(OPENNT_BASE_WORKER_WATCH *watch,
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
        service_capture_wow_labels(watch);
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
                if(!item->image[0]) {
                    OPENNT_BASE_MANAGEMENT_LABEL *label=service_wow_label(watch,selected_wow);
                    if(label)lstrcpynW(item->image,label->image,OPENNT_BASE_WORKER_IMAGE_CHARS);
                }
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

static OPENNT_BASE_MANAGEMENT_KEY service_management_key(OPENNT_BASE_SERVICE *service,
    DWORD category,DWORD generation,uint64_t object)
{
    OPENNT_BASE_MANAGEMENT_KEY key={service->management_epoch,category,generation,object};
    return key;
}

static void service_copy_worker(OPENNT_BASE_WORKER_WATCH *watch,OPENNT_BASE_WORKER_INFO *item)
{
    LIST_ENTRY *entry;
    ZeroMemory(item,sizeof(*item));
    item->sequence=watch->process.SequenceNumber;
    item->key=service_management_key(watch->service,MANAGEMENT_WORKER,item->sequence,0);
    item->process_id=GetProcessId(watch->process.ProcessHandle);
    item->kind=watch->kind==OPENNT_BASE_WORKER_NATIVE ? 2u : watch->wow ? 1u : 0u;
    item->state=VDM_READY;
    item->started_filetime=((uint64_t)watch->started.dwHighDateTime<<32)|watch->started.dwLowDateTime;
    if(watch->kind==OPENNT_BASE_WORKER_NATIVE) {
        for(entry=watch->service->connections.Flink;entry!=&watch->service->connections;entry=entry->Flink) {
            OPENNT_BASE_CONNECTION *native=CONTAINING_RECORD(entry,OPENNT_BASE_CONNECTION,service_link);
            if(native->process.SequenceNumber==item->sequence && native->native_worker) {
                service_copy_win32record(native,item);break;
            }
        }
    } else service_copy_vdm_management_record(watch,item);
    item->display_state=item->state==0 ? MANAGEMENT_UNKNOWN :
        item->stack_depth ? MANAGEMENT_BUSY : MANAGEMENT_IDLE;
    if(watch->termination_requested || (item->state&0x80000000u) ||
        WaitForSingleObject(watch->process.ProcessHandle,0)!=WAIT_TIMEOUT) {
        item->state|=0x80000000u;item->display_state=MANAGEMENT_CLOSING;
    } else item->actions=MANAGEMENT_CAN_CLOSE;
    if(watch->frontend_root_generation && !watch->wow) {
        item->parent=service_management_key(watch->service,MANAGEMENT_FRONTEND,
            watch->frontend_root_generation,0);
        item->depth=1;
    }
    if(!item->image[0])lstrcpynW(item->image,item->stack_depth ? L"<UNKNOWN>" : L"<EMPTY>",
        OPENNT_BASE_WORKER_IMAGE_CHARS);
}

/* Copied rows have a single caller-owned allocation. Capacity changes are
 * local serialization work, not another registry, lifetime or lock. */
static DWORD service_append_management(OPENNT_BASE_WORKER_INFO **rows,uint32_t *count,
    uint32_t *capacity,const OPENNT_BASE_WORKER_INFO *item)
{
    if(*count==*capacity) {
        uint32_t next=*capacity ? *capacity*2 : 16;
        OPENNT_BASE_WORKER_INFO *resized;
        if(next<*capacity || (size_t)next>SIZE_MAX/sizeof(**rows))return ERROR_ARITHMETIC_OVERFLOW;
        resized=*rows ? HeapReAlloc(GetProcessHeap(),0,*rows,(size_t)next*sizeof(**rows)) :
            HeapAlloc(GetProcessHeap(),0,(size_t)next*sizeof(**rows));
        if(!resized)return ERROR_NOT_ENOUGH_MEMORY;
        *rows=resized;*capacity=next;
    }
    (*rows)[(*count)++]=*item;
    return ERROR_SUCCESS;
}

static DWORD service_append_worker(OPENNT_BASE_WORKER_WATCH *watch,
    OPENNT_BASE_WORKER_INFO **rows,uint32_t *count,uint32_t *capacity)
{
    OPENNT_BASE_WORKER_INFO worker;
    DWORD error;
    service_copy_worker(watch,&worker);
    error=service_append_management(rows,count,capacity,&worker);
    if(error || !watch->wow)return error;
    (void)RtlEnterCriticalSection(&BaseSrvWOWCriticalSection);
    if(WOWHead && WOWHead->SequenceNumber==worker.sequence) {
        PWOWRECORD record;
        for(record=WOWHead->WOWRecord;record;record=record->WOWRecordNext) {
            OPENNT_BASE_MANAGEMENT_LABEL *label=service_wow_label(watch,record);
            OPENNT_BASE_WORKER_INFO task={0};
            /* Missing metadata cannot become a fabricated stable selector. */
            if(!label || !label->identity){error=ERROR_NOT_ENOUGH_MEMORY;break;}
            task.key=service_management_key(watch->service,MANAGEMENT_WOW_TASK,worker.sequence,label->identity);
            task.parent=worker.key;task.depth=1;task.kind=1;task.task=record->iTask;
            task.display_state=record->fDispatched ? MANAGEMENT_BUSY : MANAGEMENT_IDLE;
            lstrcpynW(task.image,label->image[0] ? label->image : L"<UNKNOWN>",OPENNT_BASE_WORKER_IMAGE_CHARS);
            /* WOW tasks have no individual host PID/creation time or proven
             * targeted-close API. Do not turn DEL into a whole-worker kill. */
            error=service_append_management(rows,count,capacity,&task);
            if(error)break;
        }
    }
    RtlLeaveCriticalSection(&BaseSrvWOWCriticalSection);
    return error;
}

static OPENNT_BASE_CONNECTION *service_management_root(OPENNT_BASE_SERVICE *service,DWORD generation)
{
    LIST_ENTRY *link;
    for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(root->process.SequenceNumber==generation && root->frontend_capability &&
            WaitForSingleObject(root->process.ProcessHandle,0)==WAIT_TIMEOUT)return root;
    }
    return NULL;
}

static DWORD service_copy_management_tree(OPENNT_BASE_SERVICE *service,
    OPENNT_BASE_WORKER_INFO **rows,uint32_t *count,uint32_t *capacity)
{
    LIST_ENTRY *link,*child;
    DWORD error=0;
    /* Registered roots followed immediately by their authenticated workers. */
    for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        OPENNT_BASE_WORKER_INFO item={0};
        FILETIME started,ignored;
        DWORD pending=0,tasks=0;
        if(service_management_root(service,root->process.SequenceNumber)!=root)continue;
        item.key=service_management_key(service,MANAGEMENT_FRONTEND,root->process.SequenceNumber,0);
        item.process_id=GetProcessId(root->process.ProcessHandle);
        item.display_state=root->frontend_closing ? MANAGEMENT_CLOSING : MANAGEMENT_IDLE;
        if(!root->frontend_closing && !OpenNtBaseServiceFrontendUsage(root,item.process_id,
            root->process.SequenceNumber,&pending,&tasks) && (pending || tasks))
            item.display_state=MANAGEMENT_BUSY;
        item.actions=root->frontend_closing ? 0 : MANAGEMENT_CAN_CLOSE;
        if(GetProcessTimes(root->process.ProcessHandle,&started,&ignored,&ignored,&ignored))
            item.started_filetime=((uint64_t)started.dwHighDateTime<<32)|started.dwLowDateTime;
        lstrcpynW(item.image,L"NTCON",OPENNT_BASE_WORKER_IMAGE_CHARS);
        error=service_append_management(rows,count,capacity,&item);if(error)return error;
        for(child=service->worker_watches.Flink;child!=&service->worker_watches;child=child->Flink) {
            OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(child,OPENNT_BASE_WORKER_WATCH,link);
            if(watch->frontend_root_generation!=root->process.SequenceNumber || watch->wow)continue;
            error=service_append_worker(watch,rows,count,capacity);if(error)return error;
        }
    }
    /* Tombstones derive solely from surviving association metadata. No extra
     * root registry, PID reopening or synthetic lifetime is introduced. */
    for(link=service->worker_watches.Flink;link!=&service->worker_watches;link=link->Flink) {
        OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
        OPENNT_BASE_WORKER_INFO missing={0};
        BOOL first=TRUE;
        if(watch->wow || !watch->frontend_root_generation) {
            error=service_append_worker(watch,rows,count,capacity);if(error)return error;
            continue;
        }
        if(service_management_root(service,watch->frontend_root_generation))continue;
        for(child=service->worker_watches.Flink;child!=link;child=child->Flink) {
            OPENNT_BASE_WORKER_WATCH *prior=CONTAINING_RECORD(child,OPENNT_BASE_WORKER_WATCH,link);
            if(prior->frontend_root_generation==watch->frontend_root_generation && !prior->wow)
                {first=FALSE;break;}
        }
        if(!first)continue;
        missing.key=service_management_key(service,MANAGEMENT_FRONTEND,watch->frontend_root_generation,0);
        missing.process_id=watch->frontend_root_pid;missing.display_state=MANAGEMENT_MISSING;
        lstrcpynW(missing.image,L"NTCON <MISSING>",OPENNT_BASE_WORKER_IMAGE_CHARS);
        error=service_append_management(rows,count,capacity,&missing);if(error)return error;
        for(child=link;child!=&service->worker_watches;child=child->Flink) {
            OPENNT_BASE_WORKER_WATCH *member=CONTAINING_RECORD(child,OPENNT_BASE_WORKER_WATCH,link);
            if(member->frontend_root_generation!=watch->frontend_root_generation || member->wow)continue;
            error=service_append_worker(member,rows,count,capacity);if(error)return error;
        }
    }
    for(link=service->gui_records.Flink;link!=&service->gui_records;link=link->Flink) {
        OPENNT_BASE_WIN32RECORD *record=CONTAINING_RECORD(link,OPENNT_BASE_WIN32RECORD,link);
        OPENNT_BASE_WORKER_INFO item={0};
        FILETIME started,ignored;
        if(record->completed || !record->gui_process ||
            WaitForSingleObject(record->gui_process,0)!=WAIT_TIMEOUT)continue;
        item.key=service_management_key(service,MANAGEMENT_GUI_TARGET,record->worker_generation,record->request);
        item.process_id=record->process_id;item.kind=2;item.task=record->request;
        item.display_state=MANAGEMENT_BUSY;item.actions=MANAGEMENT_CAN_CLOSE;
        if(GetProcessTimes(record->gui_process,&started,&ignored,&ignored,&ignored))
            item.started_filetime=((uint64_t)started.dwHighDateTime<<32)|started.dwLowDateTime;
        lstrcpynW(item.image,record->image,OPENNT_BASE_WORKER_IMAGE_CHARS);
        error=service_append_management(rows,count,capacity,&item);if(error)return error;
    }
    return ERROR_SUCCESS;
}

DWORD OpenNtBaseServiceSnapshotCopy(OPENNT_BASE_SERVICE *service,uint64_t *epoch,
    OPENNT_BASE_WORKER_INFO **entries,uint32_t *count)
{
    OPENNT_BASE_WORKER_INFO *copy=NULL;
    uint32_t needed=0,capacity=0;
    DWORD error;
    if (!epoch || !entries || !count) return ERROR_INVALID_PARAMETER;
    *epoch=0; *entries=NULL; *count=0;
    if (!service) return ERROR_INVALID_PARAMETER;
    /* One traversal under the authority's lock; no count/copy race or retry.
     * Metadata allocation failure returns no partial, misleading tree. */
    EnterCriticalSection(&service->lock);
    error=service_copy_management_tree(service,&copy,&needed,&capacity);
    if(error)goto done;
    *epoch=service->management_epoch; *entries=copy; *count=needed;
    copy=NULL; error=ERROR_SUCCESS;
done:
    LeaveCriticalSection(&service->lock);
    if (copy) HeapFree(GetProcessHeap(),0,copy);
    return error;
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
        service_copy_worker(watch,item);
    }
    *count=index;
    service_sort_management_records(entries,index);
    LeaveCriticalSection(&service->lock);
    return ERROR_SUCCESS;
}
