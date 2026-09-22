#include "wow_user_task_lifecycle.h"

#include "wow_class_words_binding.h"
#include "wow_task_profile_bindings.h"
#include "opennt-abi/host-compat/include/thread_start_compat.h"
#include "ntvdm-exe/session/session.h"

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0)
#endif

static void list_initialize(LIST_ENTRY *head)
{
    head->Flink = head;
    head->Blink = head;
}

static void list_insert_tail(LIST_ENTRY *head, LIST_ENTRY *entry)
{
    entry->Blink = head->Blink;
    entry->Flink = head;
    head->Blink->Flink = entry;
    head->Blink = entry;
}

static void list_remove(LIST_ENTRY *entry)
{
    entry->Blink->Flink = entry->Flink;
    entry->Flink->Blink = entry->Blink;
    entry->Flink = NULL;
    entry->Blink = NULL;
}

typedef struct wow_user_task_lifecycle_thread {
    struct wow_user_task_lifecycle_thread *next;
    wow_user_task_lifecycle *owner;
    wow_task_order_thread thread;
    wow_task_order_queue queue;
    wow_task_client_info client;
    wow_class_client_context classes;
    wow_user_message_bridge messages;
    HANDLE queue_event;
    DWORD thread_id;
} wow_user_task_lifecycle_thread;

static wow_user_task_lifecycle_thread *current_task(
    wow_user_task_lifecycle *owner)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    wow_user_task_lifecycle_thread *task;

    if (!owner || !binding || binding->runtime != owner->runtime ||
            !binding->thread) return NULL;
    task = (wow_user_task_lifecycle_thread *)binding->thread->host_context;
    return task && task->owner == owner ? task : NULL;
}

static void WINAPI release_lock(wow_task_order_thread *thread)
{
    wow_user_task_lifecycle_thread *task = thread ? thread->host_context : NULL;
    if (!task || !wow_user_runtime_leave(wow_user_runtime_current()))
        opennt_exit_thread(ERROR_INVALID_STATE);
}

static void WINAPI acquire_lock(wow_task_order_thread *thread)
{
    wow_user_task_lifecycle_thread *task = thread ? thread->host_context : NULL;
    if (!task || !wow_user_runtime_enter(wow_user_runtime_current()))
        opennt_exit_thread(ERROR_INVALID_STATE);
}

static void WINAPI check_death(wow_task_order_thread *thread)
{
    session *owner = session_thread_current();
    if (!thread || !owner || owner->state != SESSION_STATE_ACTIVE)
        opennt_exit_thread(ERROR_PROCESS_ABORTED);
}

static void WINAPI deliver_apc(wow_task_order_thread *thread)
{
    (void)thread;
    /* Original taskman reaches this only after an alertable termination APC.
     * The standalone thread boundary has no kernel APC carrier, so a genuine
     * alert completion is terminal rather than a successful scheduler wake. */
    opennt_exit_thread(ERROR_OPERATION_ABORTED);
}

static BOOL WINAPI idle_hooked(wow_task_order_thread *thread)
{
    (void)thread;
    /* No foreground-idle hook has been registered for this worker.  This is
     * an observed absent hook, not a substitute idle callback. */
    return FALSE;
}

static void WINAPI idle_hook(wow_task_order_thread *thread)
{
    (void)thread;
    opennt_exit_thread(ERROR_INVALID_STATE);
}

static void WINAPI wake_input_idle(wow_task_order_thread *thread)
{
    if (!thread || !thread->ptdb || !thread->ptdb->pwti ||
            !thread->ptdb->pwti->pIdleEvent ||
            !SetEvent(thread->ptdb->pwti->pIdleEvent))
        opennt_exit_thread(ERROR_INVALID_STATE);
}

static void WINAPI receive(wow_task_order_thread *thread)
{
    wow_user_task_lifecycle_thread *task = thread ? thread->host_context : NULL;
    if (!task || !wow_user_message_bridge_receive(&task->messages, thread))
        opennt_exit_thread(ERROR_CANCELLED);
}

static NTSTATUS WINAPI wait_for_task_or_message(wow_task_order_thread *thread,
    DWORD count, const HANDLE *events, BOOL alertable)
{
    wow_user_task_lifecycle_thread *task = thread ? thread->host_context : NULL;
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    DWORD result;

    if (!task || !events || count == 0 || !binding ||
            binding->runtime != task->owner->runtime) return (NTSTATUS)0xC0000001;
    /* The original caller released its USER domain immediately before this
     * wait.  MsgWait is the finite public substitute for the unavailable
     * kernel queue wake.  It does not dispatch here: the scheduled original
     * xxxUserYield/xxxReceiveMessages path owns that operation. */
    result = MsgWaitForMultipleObjectsEx(count, events, INFINITE, QS_ALLINPUT,
        alertable ? MWMO_ALERTABLE : 0);
    if (result < WAIT_OBJECT_0 + count) return (NTSTATUS)(result - WAIT_OBJECT_0);
    if (result == WAIT_IO_COMPLETION) return (NTSTATUS)0x000000C0;
    if (result == WAIT_OBJECT_0 + count) {
        if (!wow_user_runtime_enter(binding)) return (NTSTATUS)0xC0000001;
        /* A native queue wake is real work for this original task.  Retain
         * taskman's existing event-count/list scheduling; do not construct a
         * private SMS list or assert QS_SENDMESSAGE without an NT4 SMS. */
        if (thread->ptdb && thread->ptdb->nEvents == 0) {
            ++thread->ptdb->nEvents;
            ++thread->ppi->shared->nEvents;
        }
        (void)SetEvent(thread->pEventQueueServer);
        (void)wow_user_runtime_leave(binding);
        return 0;
    }
    return (NTSTATUS)0xC0000001;
}

static wow_task_order_thread *WINAPI find_thread(wow_task_order_thread *thread,
    DWORD id)
{
    wow_user_task_lifecycle_thread *task;
    wow_user_task_lifecycle *owner;
    if (!thread || !(owner = ((wow_user_task_lifecycle_thread *)
            thread->host_context)->owner)) return NULL;
    for (task = owner->threads; task; task = task->next) {
        if (task->thread_id == id) return &task->thread;
    }
    return NULL;
}

static const wow_task_host_ops host_ops = {
    release_lock, acquire_lock, check_death, deliver_apc, idle_hooked,
    idle_hook, wake_input_idle, receive, wait_for_task_or_message, find_thread
};

static wow_task_order_queue *WINAPI allocate_queue(wow_input_desktop *desktop)
{
    (void)desktop;
    return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        sizeof(wow_task_order_queue));
}

static void WINAPI destroy_queue(wow_task_order_queue *queue,
    wow_task_order_thread *thread)
{
    (void)thread;
    if (queue && queue->cThreads == 0) HeapFree(GetProcessHeap(), 0, queue);
}

static BOOL WINAPI journal_hooked(wow_task_order_process *process, BOOL playback)
{
    (void)process;
    (void)playback;
    /* The modern public USER surface exposes no query for NT4's journal
     * record/playback state.  A non-registered hook is false; no hook state
     * or callback is fabricated here. */
    return FALSE;
}

static BOOL WINAPI reattach(wow_task_order_process *process, BOOL journal)
{
    wow_user_task_lifecycle_thread *task;
    if (!process || journal || !(task = current_task(
            (wow_user_task_lifecycle *)process->init_context))) return FALSE;
    RecalcThreadAttachment(&task->thread);
    return task->thread.pqAttach != NULL;
}

static void WINAPI start_cursor(wow_task_order_process *process, DWORD timeout)
{
    (void)process;
    /* The retained product boundary has no CSRSS presentation owner.  This
     * call is intentionally kept as a W1-visible no-presentation boundary;
     * it does not alter task scheduling or synthesize a cursor. */
    (void)timeout;
}

static const wow_task_init_ops init_ops = {
    journal_hooked, reattach, start_cursor, wow_task_profile
};

static BOOL WINAPI cleanup_destroy_class(wow_cleanup_context *context,
    wow_class_lookup_entry **link)
{
    if (!context || !link || !*link) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    /* The original CLS retains the module identity used by the public
     * UnregisterClass boundary.  No guessed current-module value enters
     * task/module cleanup. */
    return wow_class_words_destroy_native(link, (*link)->hModule, TRUE);
}

static void WINAPI cleanup_destroy_object(wow_cleanup_context *context,
    wow_cleanup_handle *entry)
{
    /* No process-owned object producer is enrolled yet.  The selected
     * lifecycle publishes only native windows, which original handtabl.c
     * classifies as thread-owned and therefore never routes here.  A future
     * producer must install its own release operation before publishing a
     * PROCESSOWNED type; silently releasing an opaque object would violate
     * original ownership. */
    UNREFERENCED_PARAMETER(context);
    UNREFERENCED_PARAMETER(entry);
    SetLastError(ERROR_INVALID_STATE);
}

static PVOID WINAPI cleanup_allocate(SIZE_T size)
{
    return HeapAlloc(GetProcessHeap(), 0, size);
}

static void WINAPI cleanup_release(PVOID memory)
{
    if (memory) HeapFree(GetProcessHeap(), 0, memory);
}

BOOL WINAPI wow_user_task_lifecycle_initialize(wow_user_task_lifecycle *owner,
    wow_user_runtime *runtime, const wow_user_task_lifecycle_callbacks *callbacks)
{
    if (!owner || !runtime || !callbacks || owner->initialized || runtime->lifecycle) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    ZeroMemory(owner, sizeof(*owner));
    owner->runtime = runtime;
    owner->callbacks = *callbacks;
    owner->process.shared = &owner->shared;
    owner->process.init = &init_ops;
    owner->process.init_context = owner;
    owner->objects = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        sizeof(*owner->objects));
    if (!owner->objects) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    owner->process.classes = &owner->classes;
    owner->process.objects = owner->objects;
    list_initialize(&owner->desktop.PtiList);
    owner->desktop.allocate_queue = allocate_queue;
    owner->desktop.destroy_queue = destroy_queue;
    owner->desktop.pDeskInfo = &owner->cleanup_desktop;
    /* Original task cleanup replaces a departed task's client WOW procedure
     * with USER's client DefWindowProc.  Public USER32 is the native owner of
     * that exact fallback procedure in this standalone boundary. */
    owner->cleanup_shared.apfnClientA.pfnDefWindowProc = (PROC)DefWindowProcA;
    owner->cleanup_shared.apfnClientW.pfnDefWindowProc = (PROC)DefWindowProcW;
    owner->cleanup.shared = &owner->cleanup_shared;
    owner->cleanup.destroy_class = cleanup_destroy_class;
    owner->cleanup.destroy_object = cleanup_destroy_object;
    owner->cleanup.allocate = cleanup_allocate;
    owner->cleanup.release = cleanup_release;
    runtime->lifecycle = owner;
    owner->initialized = TRUE;
    return TRUE;
}

BOOL WINAPI wow_user_task_lifecycle_register(wow_user_task_lifecycle *owner,
    wow_task_end_callback callback, HANDLE wowexec_event)
{
    BOOL result = FALSE;
    wow_user_runtime_thread *binding;
    if (!owner || !owner->initialized || owner->registered || !callback ||
            !wowexec_event || !(binding = wow_user_runtime_current()) ||
            binding->runtime != owner->runtime || !wow_user_runtime_enter(binding)) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    __try {
        result = xxxRegisterUserHungAppHandlers(callback, wowexec_event,
            &owner->process);
        if (result) owner->registered = TRUE;
    } __finally {
        (void)wow_user_runtime_leave(binding);
    }
    return result;
}

BOOL WINAPI wow_user_task_lifecycle_init(wow_user_task_lifecycle *owner,
    UINT version, LPCSTR app_name, DWORD task_id, DWORD hotkey, DWORD shared_id,
    DWORD x, DWORD y, DWORD width, DWORD height, WORD show)
{
    wow_user_task_lifecycle_thread *task;
    wow_user_runtime_thread *binding;
    WCHAR *wide = NULL;
    UNICODE_STRING name;
    int length;
    NTSTATUS status;
    BOOL result = FALSE;

    if (!owner || !owner->initialized || !owner->registered || !app_name ||
            !(binding = wow_user_runtime_current()) || binding->runtime != owner->runtime ||
            binding->thread || !wow_user_runtime_enter(binding)) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    task = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*task));
    if (!task) {
        (void)wow_user_runtime_leave(binding);
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    length = MultiByteToWideChar(CP_ACP, 0, app_name, -1, NULL, 0);
    if (length <= 1 || !(wide = HeapAlloc(GetProcessHeap(), 0,
            (SIZE_T)length * sizeof(WCHAR))) || !MultiByteToWideChar(CP_ACP, 0,
            app_name, -1, wide, length)) goto done;
    task->owner = owner;
    task->thread_id = GetCurrentThreadId();
    task->queue_event = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (!task->queue_event || !wow_user_message_bridge_initialize(&task->messages))
        goto done;
    task->queue.cThreads = 1;
    task->thread.ppi = &owner->process;
    task->thread.pcti = &task->queue;
    task->thread.pq = &task->queue;
    task->thread.rpdesk = &owner->desktop;
    task->thread.pClientInfo = &task->client;
    task->thread.pEventQueueServer = task->queue_event;
    task->thread.host = &host_ops;
    task->thread.host_context = task;
    /* The recovered class-client body requires the exact WOW32 callbacks
     * captured at registration plus this task's original class list.  A
     * NULL callproc resolver deliberately leaves only CPD classes unsupported;
     * ordinary WOW classes take the original client.c path. */
    task->classes.classes = &owner->classes;
    task->classes.dwExpWinVer = version;
    task->classes.get_version = owner->callbacks.get_version;
    task->classes.callback = owner->callbacks.callback;
    task->classes.thread = &task->thread;
    task->classes.resolve_callproc = NULL;
    list_insert_tail(&owner->desktop.PtiList, &task->thread.PtiLink);
    task->next = owner->threads;
    owner->threads = task;
    if (!wow_user_runtime_set_context(binding, &task->thread, &task->classes)) goto done;
    RtlInitUnicodeString(&name, wide);
    status = xxxInitTask(version, &name, task_id, hotkey, shared_id, x, y,
        width, height, show, &task->thread);
    if (status == STATUS_SUCCESS) {
        result = TRUE;
        task = NULL;
    } else SetLastError(RtlNtStatusToDosError(status));
done:
    if (wide) HeapFree(GetProcessHeap(), 0, wide);
    if (task) {
        wow_user_task_lifecycle_thread **link = &owner->threads;
        while (*link && *link != task) link = &(*link)->next;
        if (*link) *link = task->next;
        if (task->thread.PtiLink.Flink) list_remove(&task->thread.PtiLink);
        if (binding->thread == &task->thread)
            (void)wow_user_runtime_set_context(binding, NULL, NULL);
        wow_user_message_bridge_dispose(&task->messages);
        if (task->queue_event) CloseHandle(task->queue_event);
        HeapFree(GetProcessHeap(), 0, task);
    }
    (void)wow_user_runtime_leave(binding);
    return result;
}

BOOL WINAPI wow_user_task_lifecycle_yield(wow_user_task_lifecycle *owner)
{
    wow_user_task_lifecycle_thread *task = current_task(owner);
    BOOL result = FALSE;
    if (!task || !wow_user_runtime_enter(wow_user_runtime_current())) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    __try { result = xxxUserYield(&task->thread); }
    __finally { (void)wow_user_runtime_leave(wow_user_runtime_current()); }
    return result;
}

VOID WINAPI wow_user_task_lifecycle_directed_yield(wow_user_task_lifecycle *owner,
    DWORD thread_id)
{
    wow_user_task_lifecycle_thread *task = current_task(owner);
    if (!task || !wow_user_runtime_enter(wow_user_runtime_current())) {
        SetLastError(ERROR_INVALID_STATE); return;
    }
    __try { xxxDirectedYield(thread_id, &task->thread); }
    __finally { (void)wow_user_runtime_leave(wow_user_runtime_current()); }
}

BOOL WINAPI wow_user_task_lifecycle_wait(wow_user_task_lifecycle *owner,
    HANDLE wowexec_event)
{
    wow_user_task_lifecycle_thread *task = current_task(owner);
    BOOL result = FALSE;
    if (!task || !wow_user_runtime_enter(wow_user_runtime_current())) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    __try { result = xxxSleepTask(FALSE, wowexec_event, &task->thread); }
    __finally { (void)wow_user_runtime_leave(wow_user_runtime_current()); }
    return result;
}

BOOL WINAPI wow_user_task_lifecycle_wow_cleanup(wow_user_task_lifecycle *owner,
    HANDLE instance, DWORD task_id, PNEMODULESEG selectors, DWORD count)
{
    wow_user_task_lifecycle_thread *task = current_task(owner);
    BOOL result = FALSE;

    if (!task || !wow_user_runtime_enter(wow_user_runtime_current())) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    owner->cleanup.thread = &task->thread;
    __try {
        result = wow_user_cleanup_bound(instance, task_id, selectors, count,
            &owner->cleanup);
    } __finally {
        owner->cleanup.thread = NULL;
        (void)wow_user_runtime_leave(wow_user_runtime_current());
    }
    return result;
}

BOOL WINAPI wow_user_task_lifecycle_cleanup(wow_user_task_lifecycle *owner,
    DWORD task_id)
{
    wow_user_task_lifecycle_thread **link;
    wow_user_task_lifecycle_thread *task;
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    BOOL result = FALSE;
    if (!owner || !binding || binding->runtime != owner->runtime ||
            !wow_user_runtime_enter(binding)) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    for (link = &owner->threads; (task = *link) != NULL; link = &task->next) {
        if (task->thread.ptdb && task->thread.ptdb->hTaskWow == task_id) break;
    }
    if (!task) {
        (void)wow_user_runtime_leave(binding);
        SetLastError(ERROR_NOT_FOUND); return FALSE;
    }
    /* W32DestroyTask invokes this same original cleanup output before it
     * releases the task.  The carrier retains that order for direct worker
     * teardown; a task record has no usable module handle here, and the
     * original task-cleanup branch only requires a non-NULL instance. */
    __try {
        owner->cleanup.thread = &task->thread;
        if (!wow_user_cleanup_bound((HANDLE)(ULONG_PTR)task_id, task_id,
                NULL, 0, &owner->cleanup)) __leave;
        /* Keep the task and its window-owner references alive if the finite
         * native boundary cannot complete original object cleanup. */
        *link = task->next;
        DestroyTask(&owner->process, &task->thread);
        if (binding->thread == &task->thread)
            (void)wow_user_runtime_set_context(binding, NULL, NULL);
        list_remove(&task->thread.PtiLink);
        wow_user_message_bridge_dispose(&task->messages);
        CloseHandle(task->queue_event);
        HeapFree(GetProcessHeap(), 0, task);
        result = TRUE;
    } __finally {
        owner->cleanup.thread = NULL;
        (void)wow_user_runtime_leave(binding);
    }
    return result;
}

void WINAPI wow_user_task_lifecycle_dispose(wow_user_task_lifecycle *owner)
{
    if (!owner || !owner->initialized || owner->threads) return;
    if (owner->process.pwpi) {
        if (owner->process.pwpi->pEventWowExec)
            CloseHandle(owner->process.pwpi->pEventWowExec);
        HeapFree(GetProcessHeap(), 0, owner->process.pwpi);
    }
    if (owner->objects) HeapFree(GetProcessHeap(), 0, owner->objects);
    if (owner->runtime && owner->runtime->lifecycle == owner)
        owner->runtime->lifecycle = NULL;
    ZeroMemory(owner, sizeof(*owner));
}
