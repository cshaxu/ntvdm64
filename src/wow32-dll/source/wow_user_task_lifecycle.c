#include "wow_user_task_lifecycle.h"

#include "wow_class_words_binding.h"
#include "wow_window_words_binding.h"
#include "wow_task_profile_bindings.h"
#include "wow_user_session_binding.h"
#include "opennt-abi/host-compat/include/thread_start_compat.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"
#include <stdio.h>

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
    DWORD message_wakes;
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

/* Observe the existing retirement edge independently of ModuleUnload and
 * launcher completion. Default-off; preserve the caller's error status. */
static void retirement_trace(const char *stage, DWORD task, DWORD status)
{
    char path[MAX_PATH], line[160];
    DWORD error = GetLastError(), size, written, path_length;
    HANDLE file;
    path_length = GetEnvironmentVariableA("MVDM_WOW_WINDOW_TRACE_PATH", path, sizeof(path));
    if (path_length && path_length < sizeof(path)) {
        file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file != INVALID_HANDLE_VALUE) {
            size = (DWORD)sprintf_s(line, sizeof(line),
                "%lu TaskRetirement tid=%lu task=%08lX stage=%s status=%lu\r\n",
                GetCurrentProcessId(), GetCurrentThreadId(), task, stage, status);
            if (size) (void)WriteFile(file, line, size, &written, NULL);
            CloseHandle(file);
        }
    }
    SetLastError(error);
}

/* Default-off witness for an existing terminal boundary; retain its code. */
static void lifecycle_failure(const char *stage, DWORD code)
{
    char path[MAX_PATH], line[192];
    DWORD error = GetLastError(), size, written;
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    PVOID frames[12];
    USHORT count, index;
    HANDLE file;
    if (GetEnvironmentVariableA("MVDM_WOW_WINDOW_TRACE_PATH", path, sizeof(path))) {
        file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file != INVALID_HANDLE_VALUE) {
            size = (DWORD)sprintf_s(line, sizeof(line),
                "%lu TaskFailure tid=%lu stage=%s code=%lu error=%lu binding=%p held=%d owner=%lu\r\n",
                GetCurrentProcessId(), GetCurrentThreadId(), stage, code, error,
                binding, binding ? binding->exclusive_held : 0,
                binding && binding->runtime ? binding->runtime->owner_thread_id : 0);
            if (size) (void)WriteFile(file, line, size, &written, NULL);
            count = CaptureStackBackTrace(0, 12, frames, NULL);
            for (index = 0; index < count; ++index) {
                HMODULE module = NULL;
                (void)GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)frames[index], &module);
                size = (DWORD)sprintf_s(line, sizeof(line),
                    " TaskFailureFrame address=%p module=%p rva=%08lX\r\n",
                    frames[index], module, (DWORD)((ULONG_PTR)frames[index] - (ULONG_PTR)module));
                if (size) (void)WriteFile(file, line, size, &written, NULL);
            }
            CloseHandle(file);
        }
    }
    SetLastError(error);
    opennt_exit_thread(code);
}

static void WINAPI release_lock(wow_task_order_thread *thread)
{
    wow_user_task_lifecycle_thread *task = thread ? thread->host_context : NULL;
    if (!task || !wow_user_runtime_leave(wow_user_runtime_current()))
        lifecycle_failure("release-lock", ERROR_INVALID_STATE);
}

static void WINAPI acquire_lock(wow_task_order_thread *thread)
{
    wow_user_task_lifecycle_thread *task = thread ? thread->host_context : NULL;
    if (!task || !wow_user_runtime_enter(wow_user_runtime_current()))
        lifecycle_failure("acquire-lock", ERROR_INVALID_STATE);
}

static void WINAPI check_death(wow_task_order_thread *thread)
{
    if (!thread || !wow_user_worker_active())
        lifecycle_failure("check-death", ERROR_PROCESS_ABORTED);
}

static void WINAPI deliver_apc(wow_task_order_thread *thread)
{
    (void)thread;
    /* Original taskman reaches this only after an alertable termination APC.
     * The standalone thread boundary has no kernel APC carrier, so a genuine
     * alert completion is terminal rather than a successful scheduler wake. */
    lifecycle_failure("deliver-apc", ERROR_OPERATION_ABORTED);
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
    lifecycle_failure("idle-hook", ERROR_INVALID_STATE);
}

static void WINAPI wake_input_idle(wow_task_order_thread *thread)
{
    if (!thread || !thread->ptdb)
        lifecycle_failure("wake-input-idle", ERROR_INVALID_STATE);
    /* input.c::WakeInputIdle: the selected taskman shared-WOW caller clears
     * FIRSTIDLE even when no WaitForInputIdle record exists (WOWEXEC). The
     * absence of a waiter is normal, not a reason to terminate the thread.
     * A present record retains this binding's existing native-event owner. */
    thread->TIF_flags &= ~WOW_TASK_TIF_FIRSTIDLE;
    if (thread->ptdb->pwti && (!thread->ptdb->pwti->pIdleEvent ||
            !SetEvent(thread->ptdb->pwti->pIdleEvent)))
        lifecycle_failure("wake-input-idle", ERROR_INVALID_STATE);
}

static void WINAPI receive(wow_task_order_thread *thread)
{
    wow_user_task_lifecycle_thread *task = thread ? thread->host_context : NULL;
    if (!task || !wow_user_message_bridge_receive(&task->messages, thread))
        lifecycle_failure("receive", ERROR_CANCELLED);
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
        ++task->message_wakes;
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

static BOOL WINAPI retire_current_thread(PVOID context);

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
    runtime->retire_thread = retire_current_thread;
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
    /* USER16 directly reads this counterpart of queue.c's THREADINFO and
     * CLIENTINFO value. Publish it before the original init can return to
     * guest code; undo it if that initialization rejects the task. */
    if (!mvdm_softpc_wow_page_domain_set_expected_windows_version(version)) goto done;
    RtlInitUnicodeString(&name, wide);
    status = xxxInitTask(version, &name, task_id, hotkey, shared_id, x, y,
        width, height, show, &task->thread);
    if (status == STATUS_SUCCESS) {
        result = TRUE;
        task = NULL;
    } else SetLastError(RtlNtStatusToDosError(status));
done:
    if (!result)
        (void)mvdm_softpc_wow_page_domain_set_expected_windows_version(0u);
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

/* ADAPTER-WOW-051: input.c::xxxInternalGetMessage orders posted/input/paint
 * before the WOW yield and timer scan; queue.c::xxxSleepThread blocks through
 * the original task scheduler. Native USER retains queue/filter ownership.
 * No guest pointer or native message is held across a cooperative switch. */
static BOOL sleep_message_task(wow_user_task_lifecycle *owner)
{
    wow_user_task_lifecycle_thread *task = current_task(owner);
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    if (!task || !wow_user_runtime_enter(binding)) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    __try {
        wake_input_idle(&task->thread);
        (void)xxxSleepTask(TRUE, NULL, &task->thread);
    } __finally { (void)wow_user_runtime_leave(binding); }
    return TRUE;
}

BOOL WINAPI wow_user_task_lifecycle_message(wow_user_task_lifecycle *owner,
    LPMSG message, HWND window, UINT first, UINT last, UINT flags, BOOL get)
{
    UINT early = (QS_POSTMESSAGE | QS_HOTKEY | QS_INPUT | QS_PAINT |
        QS_SENDMESSAGE) << 16;
    if (!current_task(owner)) {
        SetLastError(ERROR_INVALID_STATE);
        return get ? -1 : FALSE;
    }
    if (!message || (window && window != (HWND)-1 && !IsWindow(window))) {
        SetLastError(message ? ERROR_INVALID_WINDOW_HANDLE : ERROR_INVALID_PARAMETER);
        return get ? -1 : FALSE;
    }
    for (;;) {
        /* Suppress native WOW yielding: this process owns the admitted
         * original taskman state, not modern USER's historical WOW state. */
        if (PeekMessageA(message, window, first, last,
                (flags & PM_REMOVE) | PM_NOYIELD | early))
            return get ? message->message != WM_QUIT : TRUE;
        if (!(flags & PM_NOYIELD) && !wow_user_task_lifecycle_yield(owner))
            return get ? -1 : FALSE;
        if (PeekMessageA(message, window, first, last, flags | PM_NOYIELD))
            return get ? message->message != WM_QUIT : TRUE;
        if (!get) return FALSE;
        if (!sleep_message_task(owner)) return -1;
    }
}

BOOL WINAPI wow_user_task_lifecycle_wait_message(wow_user_task_lifecycle *owner)
{
    wow_user_task_lifecycle_thread *task = current_task(owner);
    DWORD wakes;
    if (!task) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    /* WaitMessage waits for new input, including already queued but not yet
     * examined input. GetQueueStatus supplies that native change-bit contract;
     * scheduler-only wakes must not become spurious successful message wakes. */
    if (LOWORD(GetQueueStatus(QS_ALLINPUT))) return TRUE;
    wakes = task->message_wakes;
    /* ReceiveMessages can examine the queue before taskman returns. Retain
     * the native wait's wake observation, not a second queue or event count. */
    do {
        if (!sleep_message_task(owner)) return FALSE;
    } while (task->message_wakes == wakes);
    return TRUE;
}

BOOL WINAPI wow_user_native_call_begin(wow_user_native_call *call, HWND window)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    wow_task_order_thread *sender, *receiver;
    wow_window_words_binding *words;
    wow_user_task_lifecycle_thread *target;
    wow_cleanup_window *view;
    ZeroMemory(call, sizeof(*call));
    /* sendmsg.c keeps its same-thread direct branch outside inter-send.
     * Use the already-published WND owner, including original short HWNDs;
     * GetWindowThreadProcessId does not accept that historical carrier. */
    if (!binding || !(sender = binding->thread) || !sender->ptdb) return TRUE;
    call->held = binding->exclusive_held;
    if (!call->held && !wow_user_runtime_enter(binding)) return FALSE;
    words = wow_window_words_acquire(window);
    view = words ? wow_window_words_cleanup_value(words) : NULL;
    receiver = view ? view->thread : NULL;
    for (target = ((wow_user_task_lifecycle *)binding->runtime->lifecycle)->threads;
            target && &target->thread != receiver; target = target->next) {}
    if (words) wow_window_words_release(words);
    if (!target || receiver == sender || !receiver->ptdb || sender->ppi->pwpi->nTaskLock) {
        /* ssend.c MAKECALL drops USER data protection, not WOW execution
         * ownership. Only a caller-held lock needs restoring after callout. */
        if (call->held) call->binding = binding;
        return wow_user_runtime_leave(binding);
    }
    call->binding = binding;
    call->receiver_id = target->thread_id;
    call->message.ptiSender = sender;
    call->message.ptiReceiver = receiver;
    call->previous = sender->psmsSent;
    sender->psmsSent = &call->message;
    DirectedScheduleTask(sender, receiver, TRUE, &call->message);
    (void)xxxSleepTask(FALSE, (HANDLE)-1, sender);
    return wow_user_runtime_leave(binding);
}

BOOL WINAPI wow_user_native_call_end(wow_user_native_call *call)
{
    wow_user_runtime_thread *binding = call->binding;
    wow_task_order_thread *sender, *receiver;
    if (!binding) return TRUE;
    if (wow_user_runtime_current() != binding || !wow_user_runtime_enter(binding))
        return FALSE;
    if (!call->receiver_id) {
        call->binding = NULL;
        return TRUE; /* Restored the caller-held data lock; no task handoff. */
    }
    sender = binding->thread;
    /* Resolve again under the owner lock: the native operation may have
     * destroyed its target. Never dereference a retained receiver pointer. */
    receiver = find_thread(sender, call->receiver_id);
    if (receiver && receiver->ptdb) {
        DirectedScheduleTask(receiver, sender, FALSE, &call->message);
        (void)xxxSleepTask(TRUE, NULL, sender);
    } else {
        (void)xxxDirectedYield((DWORD)-1, sender);
    }
    sender->psmsSent = call->previous;
    call->binding = NULL;
    return call->held || wow_user_runtime_leave(binding);
}

/* ADAPTER-WOW-051: candidate target-aware native call edge. Same-thread
 * calls retain their direct execution. Native transport/result is unchanged. */
#define WOW_NATIVE_CALL(type, name, params, args, failed) \
type WINAPI wow_native_##name params \
{ \
    wow_user_native_call call; \
    type result; BOOL restored; DWORD error = GetLastError(); \
    if (!wow_user_native_call_begin(&call, w)) return failed; \
    SetLastError(error); \
    __try { result = name args; } \
    __finally { \
        error = GetLastError(); \
        restored = wow_user_native_call_end(&call); \
        if (restored) SetLastError(error); \
    } \
    return restored ? result : failed; \
}
WOW_NATIVE_CALL(BOOL, BringWindowToTop, (HWND w), (w), FALSE)
WOW_NATIVE_CALL(BOOL, SetWindowPos, (HWND w, HWND after, int x, int y, int cx, int cy, UINT flags), (w, after, x, y, cx, cy, flags), FALSE)
WOW_NATIVE_CALL(BOOL, MoveWindow, (HWND w, int x, int y, int cx, int cy, BOOL paint), (w, x, y, cx, cy, paint), FALSE)
WOW_NATIVE_CALL(BOOL, ShowWindow, (HWND w, int show), (w, show), FALSE)
WOW_NATIVE_CALL(BOOL, EnableWindow, (HWND w, BOOL enable), (w, enable), FALSE)
WOW_NATIVE_CALL(HWND, SetActiveWindow, (HWND w), (w), NULL)
WOW_NATIVE_CALL(HWND, SetFocus, (HWND w), (w), NULL)
WOW_NATIVE_CALL(BOOL, DestroyWindow, (HWND w), (w), FALSE)
WOW_NATIVE_CALL(HWND, SetParent, (HWND w, HWND parent), (w, parent), NULL)
WOW_NATIVE_CALL(BOOL, SetWindowPlacement, (HWND w, const WINDOWPLACEMENT *placement), (w, placement), FALSE)
WOW_NATIVE_CALL(int, GetWindowTextA, (HWND w, LPSTR text, int count), (w, text, count), 0)
WOW_NATIVE_CALL(int, GetWindowTextLengthA, (HWND w), (w), 0)
WOW_NATIVE_CALL(BOOL, SetWindowTextA, (HWND w, LPCSTR text), (w, text), FALSE)
WOW_NATIVE_CALL(LRESULT, SendMessageA, (HWND w, UINT message, WPARAM wp, LPARAM lp), (w, message, wp, lp), 0)
WOW_NATIVE_CALL(LRESULT, SendMessageTimeoutA, (HWND w, UINT message, WPARAM wp, LPARAM lp, UINT flags, UINT timeout, PDWORD_PTR value), (w, message, wp, lp, flags, timeout, value), 0)
#undef WOW_NATIVE_CALL

static BOOL retire_task(wow_user_task_lifecycle *owner, DWORD task_id,
    HANDLE instance, PNEMODULESEG selectors, DWORD count)
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
    /* Native-thread retirement is distinct from pfnWOWCleanup. Keep the
     * carrier alive if the finite original cleanup binding rejects release. */
    __try {
        owner->cleanup.thread = &task->thread;
        if (!wow_user_cleanup_bound(instance, task_id, selectors, count,
                &owner->cleanup)) __leave;
        /* The original USER cleanup has now completed.  Its page-domain
         * counterpart may already have been withdrawn during worker teardown;
         * that cannot retain a dead provider task/queue record. */
        (void)mvdm_softpc_wow_page_domain_set_expected_windows_version(0u);
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

BOOL WINAPI wow_user_task_lifecycle_exit(wow_user_task_lifecycle *owner,
    HANDLE instance, DWORD task_id, PNEMODULESEG selectors, DWORD count)
{
    wow_user_task_lifecycle_thread *task = current_task(owner);
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    BOOL result = FALSE;
    if (!task || (task_id && (!task->thread.ptdb ||
            task->thread.ptdb->hTaskWow != task_id)) ||
            !wow_user_runtime_enter(binding)) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    /* wuser.c::ModuleUnload calls the task branch and then task==0 module
     * cleanup while guest code still runs. Neither call destroys THREADINFO. */
    __try {
        owner->cleanup.thread = &task->thread;
        result = wow_user_cleanup_bound(instance, task_id, selectors, count,
            &owner->cleanup);
    } __finally {
        owner->cleanup.thread = NULL;
        (void)wow_user_runtime_leave(binding);
    }
    return result;
}

static BOOL WINAPI retire_current_thread(PVOID context)
{
    wow_user_task_lifecycle *owner = context;
    wow_user_task_lifecycle_thread *task = current_task(owner);
    ULONG index;
    if (!task) { SetLastError(ERROR_INVALID_STATE); return FALSE; }
    retirement_trace("enter", task->thread.ptdb->hTaskWow, 0);
    /* Do not free a THREADINFO carrier still referenced by an enrolled
     * window. Abnormal native-window teardown needs its own retirement edge. */
    for (index = 0; index <= owner->objects->last_handle; ++index) {
        wow_cleanup_handle *entry = &owner->objects->entries[index];
        if (entry->bType == TYPE_WINDOW && entry->pOwner == &task->thread) {
            retirement_trace("live-window", task->thread.ptdb->hTaskWow, ERROR_BUSY);
            SetLastError(ERROR_BUSY); return FALSE;
        }
    }
    {
        DWORD task_id = task->thread.ptdb->hTaskWow;
        BOOL result = retire_task(owner, task_id, NULL, NULL, 0);
        retirement_trace(result ? "complete" : "failed", task_id,
            result ? ERROR_SUCCESS : GetLastError());
        return result;
    }
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
    if (owner->runtime && owner->runtime->lifecycle == owner) {
        owner->runtime->retire_thread = NULL;
        owner->runtime->lifecycle = NULL;
    }
    ZeroMemory(owner, sizeof(*owner));
}
