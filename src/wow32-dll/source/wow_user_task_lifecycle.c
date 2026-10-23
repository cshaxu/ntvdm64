#include "wow_user_task_lifecycle.h"

#include "wow_class_words_binding.h"
#include "wow_standard_class_bindings.h"
#include "wow_window_words_binding.h"
#include "wow_user_borrow_scope.h"
#include "wow_window_creation_binding.h"
#include "wow_dialog_state_bindings.h"
#include "wow_user_object_bindings.h"
#include "wow_native_gui_thread.h"
#include "wow_menu_client_bindings.h"
#include "wow_menu_native_bindings.h"
#include "wow_task_profile_bindings.h"
#include "wow_idle_hook_binding.h"
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
    wow_task_order_queue client_queue;
    wow_input_group_plan *input_plan;
    wow_task_client_info client;
    wow_class_client_context classes;
    wow_user_message_bridge messages;
    HANDLE queue_event;
    HHOOK creation_hook;
    HHOOK terminal_observer;
    HHOOK idle_observer;
    DWORD message_wakes;
    DWORD launch_request;
    BOOL start_pending;
    BOOL wait_publication_pending; /* Native wait omitted a guest publication. */
    const void *start_attempt;
} wow_user_task_lifecycle_thread;

static void publish_scheduled_return(wow_user_runtime_thread *binding);

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

HHOOK WINAPI wow_user_idle_hook_for_current(void)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    wow_user_task_lifecycle_thread *task;
    HHOOK result = NULL;
    DWORD error = GetLastError();
    BOOL held;
    if (!binding || !binding->runtime) {
        SetLastError(ERROR_INVALID_STATE);
        return NULL;
    }
    held = binding->exclusive_held;
    if (!held && !wow_user_runtime_enter(binding)) return NULL;
    task = current_task(binding->runtime->lifecycle);
    if (task && task->thread.native_thread_id == GetCurrentThreadId())
        result = task->idle_observer;
    if (!held) (void)wow_user_runtime_leave(binding);
    SetLastError(result ? error : ERROR_INVALID_STATE);
    return result;
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
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    if (!binding || binding->thread != thread || !binding->exclusive_held)
        lifecycle_failure("idle-hook-query-owner", ERROR_INVALID_STATE);
    return wow_user_idle_hook_registered(thread->native_thread_id);
}

static void WINAPI idle_hook(wow_task_order_thread *thread)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    if (!binding || binding->thread != thread || !binding->exclusive_held ||
            thread->ppi->pwpi->ptiScheduled != thread)
        lifecycle_failure("idle-hook-dispatch-owner", ERROR_INVALID_STATE);
    retirement_trace("idle-dispatch", thread->ptdb->hTaskWow, 0);
    wow_user_idle_hook_dispatch();
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
     * kernel queue wake. Actual native sent callbacks must be admitted before
     * the original rescan can wait for their suspended sender to return. */
    /* queue.c xxxSleepThread checks fsWakeBits for pending synchronous
     * sends, not just change bits. GetQueueStatus's high word retains old
     * unread sends; MsgWait alone would wait for another new message. Keep
     * posted/filter/WaitMessage new-input behavior unchanged. */
    if (HIWORD(GetQueueStatus(QS_SENDMESSAGE)) & QS_SENDMESSAGE)
        result = WAIT_OBJECT_0 + count;
    else
        result = MsgWaitForMultipleObjectsEx(count, events, INFINITE, QS_ALLINPUT,
            alertable ? MWMO_ALERTABLE : 0);
    if (result < WAIT_OBJECT_0 + count) return (NTSTATUS)(result - WAIT_OBJECT_0);
    if (result == WAIT_IO_COMPLETION) return (NTSTATUS)0x000000C0;
    if (result == WAIT_OBJECT_0 + count) {
        /* ADAPTER-WOW-093: receive under the existing callback scheduler,
         * not by granting this native wait continuation guest execution.
         * The existing input plan spans exactly the original InitTask call;
         * startup defers receiving until that call/finally releases it. */
        if (!task->input_plan &&
                !wow_user_message_bridge_receive_wait(&task->messages, thread))
            return (NTSTATUS)0xC0000001;
        if (!wow_user_runtime_enter(binding)) return (NTSTATUS)0xC0000001;
        if (!task->input_plan &&
                !wow_user_message_bridge_sync(&task->messages, thread)) {
            (void)wow_user_runtime_leave(binding);
            return (NTSTATUS)0xC0000001;
        }
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
        if (task->thread.native_thread_id == id) return &task->thread;
    }
    return NULL;
}

static void WINAPI client_context(wow_task_order_thread *previous,
    wow_task_order_thread *next)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    wow_user_task_lifecycle *owner = binding && binding->runtime ?
        binding->runtime->lifecycle : NULL;
    DWORD error = GetLastError();
    if (!binding || !binding->exclusive_held || !owner || (!previous && !next) ||
            (previous && !previous->pClientInfo) || (next && !next->pClientInfo) ||
            (previous && previous->ppi != &owner->process) ||
            (next && next->ppi != &owner->process))
        wow_user_native_call_failed("client-context-owner", ERROR_INVALID_STATE);
    /* A quiesced native-thread unbind abandons its frames, not resumes them.
     * The CPU view may already be withdrawn. No inactive scalar snapshot
     * owns a borrow; the next execution owner replaces the view before use. */
    if (previous && previous == binding->thread && !next && binding->terminal_exit) {
        ZeroMemory(previous->pClientInfo->guest_callback,
            sizeof(previous->pClientInfo->guest_callback));
    } else if (mvdm_softpc_wow_page_domain_active()) {
        if (previous == next && next) {
            if (!mvdm_softpc_wow_page_domain_set_expected_windows_version(
                    next->pClientInfo->dwExpWinVer))
                wow_user_native_call_failed("client-context-version", ERROR_INVALID_STATE);
        } else if (!mvdm_softpc_wow_page_domain_exchange_client_context(
                previous ? previous->pClientInfo->guest_callback : NULL,
                next ? next->pClientInfo->dwExpWinVer : 0,
                next ? next->pClientInfo->guest_callback : NULL))
            wow_user_native_call_failed("client-context-transfer", GetLastError());
    }
    SetLastError(error);
}

static const wow_task_host_ops host_ops = {
    release_lock, acquire_lock, check_death, deliver_apc, idle_hooked,
    idle_hook, wake_input_idle, receive, wait_for_task_or_message, find_thread,
    client_context
};

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
    wow_input_group_commit(&((wow_user_task_lifecycle *)process->init_context)->input,
        task->input_plan, &task->thread);
    return TRUE;
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

static BOOL WINAPI dispose_process(PVOID context)
{
    return wow_user_task_lifecycle_dispose(context);
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
    /* Original InitializeClientPfnArrays binds hModClient to hModUser.
     * GetClassPtr's fallback must not receive NULL: _InnerGetClassPtr treats
     * it as a wildcard and would select an unrelated private module. */
    owner->classes.client_module = GetModuleHandleW(L"user32.dll");
    if (!owner->classes.client_module) return FALSE;
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
    wow_input_group_initialize(&owner->input, &owner->desktop);
    owner->desktop.pDeskInfo = &owner->cleanup_desktop;
    /* Original task cleanup replaces a departed task's client WOW procedure
     * with USER's client DefWindowProc.  Public USER32 is the native owner of
     * that exact fallback procedure in this standalone boundary. */
    owner->cleanup_shared.apfnClientA.pfnDefWindowProc = (PROC)DefWindowProcA;
    owner->cleanup_shared.apfnClientW.pfnDefWindowProc = (PROC)DefWindowProcW;
    owner->cleanup.shared = &owner->cleanup_shared;
    owner->cleanup.destroy_class = cleanup_destroy_class;
    owner->cleanup.destroy_object = wow_menu_client_cleanup;
    owner->cleanup.allocate = cleanup_allocate;
    owner->cleanup.release = cleanup_release;
    /* Original USER process initialization supplies the public control
     * classes before application lookup; original cleanup needs the matching
     * restore table. Native USER owns the actual registrations. Prepare their
     * existing metadata and cleanup binding as one unpublished transaction. */
    if (!wow_standard_classes_prepare(&owner->classes, callbacks->callback) ||
            !wow_standard_classes_bind_cleanup(&owner->cleanup)) {
        DWORD error = GetLastError();
        if (!wow_class_words_forget_native(&owner->classes)) return FALSE;
        HeapFree(GetProcessHeap(), 0, owner->objects);
        owner->objects = NULL;
        owner->process.objects = NULL;
        SetLastError(error);
        return FALSE;
    }
    runtime->lifecycle = owner;
    runtime->retire_thread = retire_current_thread;
    runtime->dispose_process = dispose_process;
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
    DWORD failure_error;
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
    /* Creates the current USER queue without pumping input or creating a
     * hidden window. A task owns its native thread capability until all of
     * this binding's input links have been retired. */
    if (!wow_edit_native_ensure_gui_thread()) goto done;
    task->thread.native_thread_id = GetCurrentThreadId();
    task->thread.native_thread = OpenThread(SYNCHRONIZE | THREAD_QUERY_LIMITED_INFORMATION,
        FALSE, task->thread.native_thread_id);
    if (!task->thread.native_thread) goto done;
    task->queue_event = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (!task->queue_event || !wow_user_message_bridge_initialize(&task->messages))
        goto done;
    task->thread.ppi = &owner->process;
    task->thread.pcti = &task->client_queue;
    task->thread.rpdesk = &owner->desktop;
    task->thread.pClientInfo = &task->client;
    task->thread.pEventQueueServer = task->queue_event;
    task->thread.host = &host_ops;
    task->thread.host_context = task;
    /* The recovered class-client body requires the exact WOW32 callbacks
     * captured at registration plus this task's original class list.  A
     * NULL override selects the class adapter's native address owner; test
     * overrides are not required for production CPD registration. */
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
    /* Native WND/CLS slots may contain a real native subclass, not our fixed
     * gateway. Observe terminal return independently before admitting any
     * window creation, including a subclass swallowing WM_NCDESTROY. */
    task->terminal_observer = SetWindowsHookExA(WH_CALLWNDPROCRET,
        wow_window_terminal_observer, NULL, task->thread.native_thread_id);
    if (!task->terminal_observer) goto done;
    task->creation_hook = SetWindowsHookExA(WH_CBT,
        wow_window_creation_observer, NULL, task->thread.native_thread_id);
    if (!task->creation_hook) goto done;
    /* Install on every task, not just the INT2F registrar: another task may
     * be the last one to wait. The original TD list still owns intent. */
    task->idle_observer = SetWindowsHookExA(WH_FOREGROUNDIDLE,
        WK32ForegroundIdleHook, NULL, task->thread.native_thread_id);
    if (!task->idle_observer) goto done;
    /* Original InitTask initializes CLIENTINFO before acquiring execution.
     * Its handoff publishes this task's guest view; do not overwrite another
     * task's still-current TEB during preparation or a rejected init. */
    /* Prepared native controls share their original class owner just like
     * registered WOW classes. Publish before any admitted task can create a
     * WND, under the existing exclusive runtime lock and active page domain.
     * Never manufacture an observed per-window class for a managed control. */
    if (!wow_standard_classes_publish(&owner->classes)) goto done;
    task->input_plan = wow_input_group_prepare(&owner->input);
    if (!task->input_plan) goto done;
    RtlInitUnicodeString(&name, wide);
    __try {
        status = xxxInitTask(version, &name, task_id, hotkey, shared_id, x, y,
            width, height, show, &task->thread);
    } __finally {
        wow_input_group_release_plan(task->input_plan);
        task->input_plan = NULL;
    }
    if (status == STATUS_SUCCESS) {
        task->launch_request = shared_id;
        /* Separate WOW deliberately passes zero to original USER. Its real
         * launch is identified by the original TD at the registration edge;
         * bootstrap/internal tasks never issue that notification. */
        task->start_pending = shared_id != (DWORD)-1;
        result = TRUE;
        task = NULL;
    } else SetLastError(RtlNtStatusToDosError(status));
done:
    failure_error = GetLastError();
    if (wide) HeapFree(GetProcessHeap(), 0, wide);
    if (task) {
        wow_user_task_lifecycle_thread **link = &owner->threads;
        if (task->idle_observer) UnhookWindowsHookEx(task->idle_observer);
        if (task->creation_hook) UnhookWindowsHookEx(task->creation_hook);
        if (task->terminal_observer) UnhookWindowsHookEx(task->terminal_observer);
        while (*link && *link != task) link = &(*link)->next;
        if (*link) *link = task->next;
        if (task->thread.PtiLink.Flink) list_remove(&task->thread.PtiLink);
        if (binding->thread == &task->thread)
            (void)wow_user_runtime_set_context(binding, NULL, NULL);
        wow_user_message_bridge_dispose(&task->messages);
        if (task->queue_event) CloseHandle(task->queue_event);
        if (task->thread.native_thread) CloseHandle(task->thread.native_thread);
        /* queue.c DeleteThreadInfo owns this final release in original USER.
         * xxxInitTask may allocate the name and then reject TDB allocation;
         * this project carrier must release it on rollback as well. */
        cleanup_release(task->thread.pstrAppName);
        HeapFree(GetProcessHeap(), 0, task);
    }
    (void)wow_user_runtime_leave(binding);
    if (!result) SetLastError(failure_error);
    return result;
}

DWORD WINAPI wow_user_task_lifecycle_notify_start(wow_user_task_lifecycle *owner,
    DWORD task_id, DWORD shared_id, DWORD (WINAPI *notify)(ULONG))
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    wow_user_task_lifecycle_thread *task;
    DWORD error = ERROR_OPERATION_ABORTED;
    char attempt;
    if (!notify || !owner || !binding || binding->runtime != owner->runtime ||
            !wow_user_runtime_enter(binding)) return ERROR_INVALID_STATE;
    task = current_task(owner);
    if (!task || !task->thread.ptdb || task->thread.ptdb->hTaskWow != task_id ||
            task->launch_request != shared_id || !task->start_pending ||
            task->start_attempt) {
        (void)wow_user_runtime_leave(binding);
        return ERROR_INVALID_STATE;
    }
    /* A live call-frame token also distinguishes retirement/reallocation.
     * Never retain/dereference the old task pointer across the RPC. */
    task->start_attempt = &attempt;
    (void)wow_user_runtime_leave(binding);
    __try {
        error = notify(shared_id);
    } __finally {
        binding = wow_user_runtime_current();
        if (binding && binding->runtime == owner->runtime &&
                wow_user_runtime_enter(binding)) {
            task = current_task(owner);
            if (task && task->start_attempt == &attempt) {
                task->start_attempt = NULL;
                if (!error) task->start_pending = FALSE;
            } else error = ERROR_OPERATION_ABORTED;
            (void)wow_user_runtime_leave(binding);
        } else error = ERROR_OPERATION_ABORTED;
    }
    return error;
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
    __try {
        xxxDirectedYield(thread_id, &task->thread);
        publish_scheduled_return(wow_user_runtime_current());
    }
    __finally { (void)wow_user_runtime_leave(wow_user_runtime_current()); }
}

BOOL WINAPI wow_user_task_lifecycle_wait(wow_user_task_lifecycle *owner,
    HANDLE wowexec_event)
{
    wow_user_task_lifecycle_thread *task = current_task(owner);
    BOOL result = FALSE;
    DWORD queue_status;
    if (!task || !wow_user_runtime_enter(wow_user_runtime_current())) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    __try {
        /* taskman.c checks WOWEXEC's queue bits before a hardware-only wake.
         * Native queue observers can clear change bits without removing a
         * posted command. Preserve that pending input for WOWEXEC's unfiltered
         * PeekMessage, as queue.c restores examined but unconsumed wake bits.
         * Refresh, not accumulate: removed posts must not keep it runnable.
         * Ordinary WaitMessage/filter semantics remain in their own binding. */
        queue_status = GetQueueStatus(QS_ALLINPUT);
        task->thread.pcti->fsChangeBits = LOWORD(queue_status) |
            (HIWORD(queue_status) & QS_POSTMESSAGE);
        result = xxxSleepTask(FALSE, wowexec_event, &task->thread);
        if (result) publish_scheduled_return(wow_user_runtime_current());
    }
    __finally { (void)wow_user_runtime_leave(wow_user_runtime_current()); }
    return result;
}

/* OpenNT client.c::MsgWaitForMultipleObjectsEx brackets its nonzero wait
 * with WaitForMsgAndEvent(HEVENT_REMOVEME) and DirectedYield(DY_OLDYIELD).
 * Native USER owns handles/queue/result; existing original taskman owns the
 * WOW execution handoff. No private queue or alternate scheduler. */
DWORD WINAPI wow_user_msg_wait(DWORD count, const HANDLE *handles, BOOL all,
    DWORD timeout, DWORD wake_mask)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    DWORD result, error = GetLastError();
    wow_user_native_call call;
    if (!timeout || !binding || !binding->thread || !binding->thread->ptdb)
        return MsgWaitForMultipleObjects(count, handles, all, timeout, wake_mask);
    if (!wow_user_native_call_begin(&call, NULL)) return WAIT_FAILED;
    SetLastError(error);
    __try {
        result = MsgWaitForMultipleObjects(count, handles, all, timeout, wake_mask);
    } __finally {
        error = GetLastError();
        if (!wow_user_native_call_end(&call))
            wow_user_native_call_failed("msg-wait-return", GetLastError());
        SetLastError(error);
    }
    return result;
}

/* ADAPTER-WOW-051: input.c::xxxInternalGetMessage orders posted/input/paint
 * before the WOW yield and timer scan; queue.c::xxxSleepThread blocks through
 * the original task scheduler. Native USER retains queue/filter ownership.
 * No guest pointer or native message is held across a cooperative switch. */
static BOOL sleep_message_task(wow_user_task_lifecycle *owner, BOOL publish)
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
        if (publish) publish_scheduled_return(binding);
    } __finally { (void)wow_user_runtime_leave(binding); }
    return TRUE;
}

static BOOL peek_message_published(LPMSG message, HWND window, UINT first,
    UINT last, UINT flags, BOOL unicode, BOOL *available)
{
    wow_user_native_call call;
    BOOL restored;
    DWORD error = GetLastError();
    /* A queue filter is not a send target. PeekMessage can dispatch sent
     * callbacks even when it returns FALSE, so publish on both outcomes. */
    if (!wow_user_native_call_begin_inline(&call)) return FALSE;
    SetLastError(error);
    __try {
        *available = unicode ? PeekMessageW(message, window, first, last, flags) :
            PeekMessageA(message, window, first, last, flags);
        if (*available) call.returned_count =
            wow_user_queue_window_roots(message, call.returned_payload);
    }
    __finally {
        error = GetLastError();
        restored = wow_user_native_call_end(&call);
        if (restored) SetLastError(error);
    }
    return restored;
}

static BOOL lifecycle_message(wow_user_task_lifecycle *owner,
    LPMSG message, HWND window, UINT first, UINT last, UINT flags, BOOL get,
    BOOL unicode)
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
        BOOL available;
        /* Suppress native WOW yielding: this process owns the admitted
         * original taskman state, not modern USER's historical WOW state. */
        if (!peek_message_published(message, window, first, last,
                (flags & PM_REMOVE) | PM_NOYIELD | early, unicode, &available))
            return get ? -1 : FALSE;
        if (available)
            return get ? message->message != WM_QUIT : TRUE;
        if (!(flags & PM_NOYIELD) && !wow_user_task_lifecycle_yield(owner))
            return get ? -1 : FALSE;
        if (!peek_message_published(message, window, first, last,
                flags | PM_NOYIELD, unicode, &available)) return get ? -1 : FALSE;
        if (available)
            return get ? message->message != WM_QUIT : TRUE;
        if (!get) return FALSE;
        if (!sleep_message_task(owner, FALSE)) return -1;
    }
}

/* Original WOW thunks use ANSI; the recovered USER dialog/control island
 * is Unicode. Both retain exactly the same scheduler/publication sequence.
 * Encoding belongs to native queue retrieval, not a second message codec. */
BOOL WINAPI wow_user_task_lifecycle_message(wow_user_task_lifecycle *owner,
    LPMSG message, HWND window, UINT first, UINT last, UINT flags, BOOL get)
{
    return lifecycle_message(owner, message, window, first, last, flags, get, FALSE);
}

BOOL WINAPI wow_user_task_lifecycle_messageW(wow_user_task_lifecycle *owner,
    LPMSG message, HWND window, UINT first, UINT last, UINT flags, BOOL get)
{
    return lifecycle_message(owner, message, window, first, last, flags, get, TRUE);
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
        if (!sleep_message_task(owner, TRUE)) return FALSE;
    } while (task->message_wakes == wakes);
    return TRUE;
}

static void execution_push(wow_task_order_thread *thread,
    wow_task_execution_scope *scope, BOOL ready)
{
    scope->previous = thread->execution;
    scope->ready = ready;
    scope->active = TRUE;
    thread->execution = scope;
}

static void execution_pop(wow_task_order_thread *thread,
    wow_task_execution_scope *scope)
{
    if (!thread || thread->execution != scope || !scope->active)
        wow_user_native_call_failed("execution-scope-order", ERROR_INVALID_STATE);
    thread->execution = scope->previous;
    scope->previous = NULL;
    scope->active = FALSE;
}

/* A synchronous inline native operation may keep its inherited WOW owner.
 * If another task actually receives a callback, that owner is inside native
 * code, not running guest instructions. Park it through the original bounded
 * HEVENT_REMOVEME branch; persistent eligibility prevents later reselection.
 * No send/reply identity or nominated receiver is inferred here. */
static void park_native_owner(wow_task_order_thread *receiver)
{
    wow_task_order_state *state = receiver->ppi->pwpi;
    wow_task_order_thread *thread = state ? state->CSOwningThread : NULL;
    if (!thread || state->nTaskLock) return;
    if (!wow_task_execution_ready(thread))
        (void)xxxSleepTask(FALSE, (HANDLE)-1, thread);
}

static BOOL callback_enter(wow_task_callback_scope *scope, BOOL resource_cleanup)
{
    DWORD error = GetLastError();
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    wow_task_order_thread *thread;
    ZeroMemory(scope, sizeof(*scope));
    if (!binding || !(thread = binding->thread) || !thread->ptdb) return TRUE;
    if (!thread->ppi || !thread->ppi->pwpi) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    if ((thread->TIF_flags & WOW_TASK_TIF_INCLEANUP) &&
            !(resource_cleanup && binding->resource_cleanup_live)) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    scope->held = binding->exclusive_held;
    if (!scope->held && !wow_user_runtime_enter(binding)) return FALSE;
    scope->binding = binding;
    execution_push(thread, &scope->execution, TRUE);
    if (thread->ppi->pwpi->CSOwningThread != thread)
        park_native_owner(thread);
    /* queue.c xxxSleepThread resumes taskman before receiving callbacks.
     * Native USER can instead deliver inside its own SendMessage wait. */
    if (thread->ppi->pwpi->CSOwningThread != thread) {
        /* The callback has actually arrived, even when native SendMessage's
         * wait dispatched it without our queue-wait observer. Original
         * OldYield records runnable work before entering SleepTask. */
        xxxDirectedYield((DWORD)-1, thread);
        scope->resumed = TRUE;
    }
    if (thread->ppi->pwpi->CSOwningThread != thread)
        wow_user_native_call_failed("callback-execution-owner", ERROR_INVALID_STATE);
    /* Once execution was acquired, a failed lock release is not a rejected
     * callback. Returning FALSE would leave an acquired scope to a caller
     * which never entered its finally block. */
    if (!wow_user_runtime_leave(binding))
        wow_user_native_call_failed("release-callback-entry", GetLastError());
    SetLastError(error);
    return TRUE;
}

BOOL WINAPI wow_task_callback_enter(wow_task_callback_scope *scope)
{
    return callback_enter(scope, FALSE);
}

BOOL WINAPI wow_task_resource_cleanup_enter(wow_task_callback_scope *scope)
{
    return callback_enter(scope, TRUE);
}

BOOL WINAPI wow_task_callback_leave(wow_task_callback_scope *scope)
{
    DWORD error = GetLastError();
    wow_user_runtime_thread *binding = scope->binding;
    if (!binding) return TRUE;
    if (wow_user_runtime_current() != binding)
        wow_user_native_call_failed("restore-callback-binding", ERROR_INVALID_STATE);
    if (!wow_user_runtime_enter(binding))
        wow_user_native_call_failed("restore-callback-owner", GetLastError());
    execution_pop(binding->thread, &scope->execution);
    /* Return to the suspended native wait, not to guest execution. */
    if (scope->resumed) (void)xxxSleepTask(FALSE, (HANDLE)-1, binding->thread);
    scope->binding = NULL;
    if (!scope->held && !wow_user_runtime_leave(binding))
        wow_user_native_call_failed("release-callback-owner", GetLastError());
    SetLastError(error);
    return TRUE;
}

static BOOL native_call_begin(wow_user_native_call *call, HWND window,
    BOOL inline_call)
{
    DWORD error = GetLastError();
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    wow_task_order_thread *sender;
    wow_window_words_binding *words;
    wow_cleanup_window *view;
    ZeroMemory(call, sizeof(*call));
    if (!binding) return TRUE;
    sender = binding->thread;
    call->held = binding->exclusive_held;
    /* USER resource/enumeration entry can precede original InitTask. There
     * is no guest execution frame yet, but native callbacks still require
     * the caller's data lock to be released and restored on every exit. */
    if (!sender || !sender->ptdb) {
        if (call->held && !wow_user_runtime_leave(binding)) return FALSE;
        call->binding = binding;
        SetLastError(error);
        return TRUE;
    }
    if (!binding->runtime->lifecycle) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    /* Native resource-only bindings need no WOW scheduler. A real original
     * WOW task may not silently lose its registered process state. */
    if ((sender->TIF_flags & WOW_TASK_TIF_16BIT) &&
            (!sender->ppi || !sender->ppi->pwpi)) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    if (!call->held && !wow_user_runtime_enter(binding)) return FALSE;
    call->binding = binding;
    call->execution_held = sender->ppi && sender->ppi->pwpi &&
        sender->ppi->pwpi->CSOwningThread == sender;
    /* ADAPTER-WOW-082: the explicit native operand is not background
     * desktop observation. Retain its known full identity for return-time
     * publication even if it belongs to another process. Invalid operands
     * keep their native failure path; never enroll or revive one here. */
    if (window) {
        HWND full = wow_user_window_native_handle(window);
        if (full && IsWindow(full)) call->target_window = full;
    }
    /* Preserve the original direct same-task path, including short HWND
     * carriers. This is only an inline execution optimization, never the
     * identity of a message received during the native operation. */
    words = wow_window_words_acquire(window);
    view = words ? wow_window_words_cleanup_value(words) : NULL;
    inline_call = inline_call || (view && view->thread == sender);
    if (words) wow_window_words_release(words);
    execution_push(sender, &call->execution, FALSE);
    /* Actual native waits/modal calls are excluded persistently. Task lock
     * is not overridden; actual same-thread callbacks inherit execution. */
    if (call->execution_held && !inline_call && !sender->ppi->pwpi->nTaskLock)
        (void)xxxSleepTask(FALSE, (HANDLE)-1, sender);
    if (!wow_user_runtime_leave(binding))
        wow_user_native_call_failed("release-native-handoff", GetLastError());
    /* Owner lookup is bookkeeping, not the native operation. In particular
     * a NULL target must not replace a parser's pending error with the
     * window lookup's ERROR_INVALID_WINDOW_HANDLE during menu rollback. */
    SetLastError(error);
    return TRUE;
}

BOOL WINAPI wow_user_native_call_begin(wow_user_native_call *call, HWND window)
{
    return native_call_begin(call, window, FALSE);
}

BOOL WINAPI wow_user_native_call_begin_inline(wow_user_native_call *call)
{
    return native_call_begin(call, NULL, TRUE);
}

LRESULT WINAPI wow_native_CallNextHookEx(HHOOK hook, int code, WPARAM wp, LPARAM lp)
{
    wow_user_native_call call;
    LRESULT result;
    DWORD error = GetLastError();
    if (!wow_user_native_call_begin_inline(&call)) return 0;
    __try {
        SetLastError(error);
        result = CallNextHookEx(hook, code, wp, lp);
    } __finally {
        error = GetLastError();
        if (!wow_user_native_call_end(&call))
            wow_user_native_call_failed("next-hook-return", GetLastError());
        SetLastError(error);
    }
    return result;
}

void WINAPI wow_user_native_call_result(wow_user_native_call *call,
    UINT message, WPARAM wp, LPARAM lp, LRESULT result)
{
    call->returned_count = wow_user_message_return_roots(message, wp, lp,
        result, call->returned_payload);
    /* wmsg16.c returns a previous menu, or NEXTMENU's output pair. Copy
     * identities now, not native structure pointers across task handoff. */
    if (message == WM_MDISETMENU) call->returned_menu = (HMENU)result;
    else if (message == WM_NEXTMENU && lp)
        call->returned_menu = ((const MDINEXTMENU *)lp)->hmenuNext;
    else if (message == WM_DRAWITEM && lp && ((const DRAWITEMSTRUCT *)lp)->CtlType == ODT_MENU)
        call->returned_menu = (HMENU)((const DRAWITEMSTRUCT *)lp)->hwndItem;
    else if (message == WM_DELETEITEM && lp && ((const DELETEITEMSTRUCT *)lp)->CtlType == ODT_MENU)
        call->returned_menu = (HMENU)((const DELETEITEMSTRUCT *)lp)->hwndItem;
    else if (message == WM_COMPAREITEM && lp && ((const COMPAREITEMSTRUCT *)lp)->CtlType == ODT_MENU)
        call->returned_menu = (HMENU)((const COMPAREITEMSTRUCT *)lp)->hwndItem;
}

static BOOL publish_call_windows(const wow_user_native_call *call)
{
    HWND roots[6];
    ULONG count = call->returned_count;
    if (count > 4) { SetLastError(ERROR_INVALID_DATA); return FALSE; }
    CopyMemory(roots, call->returned_payload, count * sizeof(*roots));
    roots[count++] = call->returned_window;
    roots[count++] = call->target_window;
    return wow_user_window_refresh_payload(NULL, roots, count) &&
        (!call->returned_menu || !IsMenu(call->returned_menu) ||
            wow_menu_native_refresh_menu(call->returned_menu));
}

static BOOL publish_guest_return(wow_user_runtime_thread *binding,
    const wow_user_native_call *call)
{
    wow_user_task_lifecycle_thread *task = current_task(binding->runtime->lifecycle);
    BOOL published = (call ? publish_call_windows(call) : wow_user_window_refresh_graph(NULL)) &&
        wow_user_borrow_client_graph(NULL, GetDesktopWindow()) &&
        wow_menu_client_refresh_all(binding->thread->ppi);
    /* An ordinary native return may have already discharged a prior wait.
     * Do not clear on partial failure or unwind. This is a call-boundary
     * obligation, not an independent HWND/graph dirty-state authority. */
    if (published && task) task->wait_publication_pending = FALSE;
    return published;
}

static void publish_scheduled_return(wow_user_runtime_thread *binding)
{
    DWORD saved = GetLastError();
    wow_task_order_thread *thread = binding->thread;
    wow_user_task_lifecycle_thread *task = current_task(binding->runtime->lifecycle);
    /* Original HEVENT_REMOVEME can return to a native continuation instead.
     * This helper publishes an acquired owner; it never grants execution. */
    if (task && task->wait_publication_pending &&
            thread && thread->ppi && thread->ppi->pwpi &&
            thread->ppi->pwpi->CSOwningThread == thread &&
            !publish_guest_return(binding, NULL))
        wow_user_native_call_failed("publish-scheduler-return", GetLastError());
    SetLastError(saved);
}

static BOOL native_call_end(wow_user_native_call *call, BOOL scheduler_wait)
{
    wow_user_runtime_thread *binding = call->binding;
    wow_task_order_thread *sender;
    BOOL published;
    DWORD error;
    if (!binding) return TRUE;
    if (wow_user_runtime_current() != binding)
        wow_user_native_call_failed("restore-native-binding", ERROR_INVALID_STATE);
    if (!call->execution.active) {
        if (binding->exclusive_held || (call->held && !wow_user_runtime_enter(binding)))
            wow_user_native_call_failed("restore-resource-owner", ERROR_INVALID_STATE);
        call->binding = NULL;
        return TRUE;
    }
    if (!wow_user_runtime_enter(binding))
        wow_user_native_call_failed("restore-native-owner", GetLastError());
    sender = binding->thread;
    if (!sender || sender->execution != &call->execution)
        wow_user_native_call_failed("native-return-scope", ERROR_INVALID_STATE);
    /* Native completion makes this caller ready. It proves no receiver or
     * private SMS relation. Reacquire only execution owned on entry. */
    call->execution.ready = TRUE;
    if (call->execution_held && sender->ppi->pwpi->CSOwningThread != sender)
        (void)xxxDirectedYield((DWORD)-1, sender);
    if (call->execution_held && sender->ppi->pwpi->CSOwningThread != sender)
        wow_user_native_call_failed("native-return-execution", ERROR_INVALID_STATE);
    execution_pop(sender, &call->execution);
    call->binding = NULL;
    if (scheduler_wait && !call->execution_held) {
        wow_user_task_lifecycle_thread *task = current_task(binding->runtime->lifecycle);
        if (!task)
            wow_user_native_call_failed("native-wait-task", ERROR_INVALID_STATE);
        task->wait_publication_pending = TRUE;
        published = TRUE;
    } else published = publish_guest_return(binding, call);
    error = GetLastError();
    if (!published) wow_user_native_call_failed("publish-native-return", error);
    if (!call->held && !wow_user_runtime_leave(binding))
        wow_user_native_call_failed("release-native-owner", GetLastError());
    SetLastError(error);
    return published;
}

BOOL WINAPI wow_user_native_call_end(wow_user_native_call *call)
{
    return native_call_end(call, FALSE);
}

BOOL WINAPI wow_user_native_call_end_wait(wow_user_native_call *call)
{
    return native_call_end(call, TRUE);
}

/* A native operation may already have consumed a MSG or mutated USER. It
 * cannot be replayed, reported as an ordinary false result or followed by
 * unsafe guest execution. Fail the worker, not merely the current WOW task.
 * Use the product's existing fail-fast convention for broken ownership. */
__declspec(noreturn) void WINAPI wow_user_native_call_failed(const char *stage, DWORD error)
{
    EXCEPTION_RECORD record = {0};
    retirement_trace(stage, 0, error);
    record.ExceptionCode = 0xc0000420u; /* STATUS_ASSERTION_FAILURE */
    record.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
    record.NumberParameters = 1;
    record.ExceptionInformation[0] = error;
    RaiseFailFastException(&record, NULL, 0);
    __assume(0);
}

/* ADAPTER-WOW-051: candidate target-aware native call edge. Same-thread
 * calls retain their direct execution. Native transport/result is unchanged. */
#define WOW_NATIVE_CALL_AFTER(type, name, params, args, failed, target, after) \
type WINAPI wow_native_##name params \
{ \
    wow_user_native_call call; \
    type result; BOOL restored; DWORD error = GetLastError(); \
    if (!wow_user_native_call_begin(&call, target)) return failed; \
    SetLastError(error); \
    __try { result = name args; after; } \
    __finally { \
        error = GetLastError(); \
        restored = wow_user_native_call_end(&call); \
        if (restored) SetLastError(error); \
    } \
    return restored ? result : failed; \
}
#define WOW_NATIVE_CALL_RESULT(type, name, params, args, failed, target, window_result) \
    WOW_NATIVE_CALL_AFTER(type, name, params, args, failed, target, call.returned_window = (window_result))
#define WOW_NATIVE_CALL_TARGET(type, name, params, args, failed, target) \
    WOW_NATIVE_CALL_RESULT(type, name, params, args, failed, target, NULL)
#define WOW_NATIVE_CALL(type, name, params, args, failed) \
    WOW_NATIVE_CALL_TARGET(type, name, params, args, failed, w)
WOW_NATIVE_CALL(BOOL, BringWindowToTop, (HWND w), (w), FALSE)
WOW_NATIVE_CALL(BOOL, SetWindowPos, (HWND w, HWND after, int x, int y, int cx, int cy, UINT flags), (w, after, x, y, cx, cy, flags), FALSE)
WOW_NATIVE_CALL(BOOL, MoveWindow, (HWND w, int x, int y, int cx, int cy, BOOL paint), (w, x, y, cx, cy, paint), FALSE)
WOW_NATIVE_CALL(BOOL, ShowWindow, (HWND w, int show), (w, show), FALSE)
WOW_NATIVE_CALL(BOOL, EnableWindow, (HWND w, BOOL enable), (w, enable), FALSE)
WOW_NATIVE_CALL_RESULT(HWND, SetActiveWindow, (HWND w), (w), NULL, w, result)
WOW_NATIVE_CALL(BOOL, SetForegroundWindow, (HWND w), (w), FALSE)
WOW_NATIVE_CALL(BOOL, SetWindowContextHelpId, (HWND w, DWORD id), (w, id), FALSE)
WOW_NATIVE_CALL_TARGET(BOOL, SetCursorPos, (int x, int y), (x, y), FALSE, NULL)
WOW_NATIVE_CALL_TARGET(int, ShowCursor, (BOOL show), (show), 0, NULL)
/* Original ListBox keeps selection/WM_BEGINDRAG policy. Native USER owns
 * capture and the modal input wait; callbacks use this shared boundary. */
WOW_NATIVE_CALL(BOOL, DragDetect, (HWND w, POINT point), (w, point), FALSE)
WOW_NATIVE_CALL_RESULT(HWND, SetFocus, (HWND w), (w), NULL, w, result)
/* capture.c sends WM_CAPTURECHANGED to the previous capture, not w. */
WOW_NATIVE_CALL_RESULT(HWND, SetCapture, (HWND w), (w), NULL, GetCapture(), result)
WOW_NATIVE_CALL_TARGET(BOOL, ReleaseCapture, (void), (), FALSE, GetCapture())
WOW_NATIVE_CALL(BOOL, DestroyWindow, (HWND w), (w), FALSE)
WOW_NATIVE_CALL(BOOL, EndDialog, (HWND w, INT_PTR result_value), (w, result_value), FALSE)
WOW_NATIVE_CALL_RESULT(HWND, SetParent, (HWND w, HWND parent), (w, parent), NULL, w, result)
WOW_NATIVE_CALL(BOOL, SetWindowPlacement, (HWND w, const WINDOWPLACEMENT *placement), (w, placement), FALSE)
WOW_NATIVE_CALL_RESULT(LONG, SetWindowLongA, (HWND w, int index, LONG value),
    (w, index, value), 0, w, index == GWL_HWNDPARENT ? (HWND)result : NULL)
WOW_NATIVE_CALL_TARGET(BOOL, EndDeferWindowPos, (HDWP positions), (positions), FALSE, NULL)
/* Original WOW thunks retain conversion and return policy. These public
 * USER calls can mutate children, frame geometry or disabled state without
 * passing through another redirected thunk. Use the same return owner. */
WOW_NATIVE_CALL(UINT, ArrangeIconicWindows, (HWND w), (w), 0)
WOW_NATIVE_CALL(BOOL, CloseWindow, (HWND w), (w), FALSE)
WOW_NATIVE_CALL(BOOL, OpenIcon, (HWND w), (w), FALSE)
WOW_NATIVE_CALL(BOOL, ShowOwnedPopups, (HWND w, BOOL show), (w, show), FALSE)
WOW_NATIVE_CALL(BOOL, ScrollWindow, (HWND w, int dx, int dy, const RECT *area, const RECT *clip), (w, dx, dy, area, clip), FALSE)
WOW_NATIVE_CALL(int, ScrollWindowEx, (HWND w, int dx, int dy, const RECT *area, const RECT *clip, HRGN update, LPRECT bounds, UINT flags), (w, dx, dy, area, clip, update, bounds, flags), ERROR)
WOW_NATIVE_CALL(BOOL, ShowScrollBar, (HWND w, int bar, BOOL show), (w, bar, show), FALSE)
WOW_NATIVE_CALL(BOOL, SetScrollRange, (HWND w, int bar, int minimum, int maximum, BOOL redraw), (w, bar, minimum, maximum, redraw), FALSE)
WOW_NATIVE_CALL(int, SetScrollPos, (HWND w, int bar, int position, BOOL redraw), (w, bar, position, redraw), 0)
WOW_NATIVE_CALL(BOOL, EnableScrollBar, (HWND w, UINT bar, UINT arrows), (w, bar, arrows), FALSE)
/* Original EDIT's NtUser calls can synchronously paint, notify or reenter.
 * Keep these in the same native-call family, not a second control scheduler.
 * No returned HDC/HBITMAP/HCURSOR is a returned window identity. */
WOW_NATIVE_CALL(HDC, BeginPaint, (HWND w, LPPAINTSTRUCT paint), (w, paint), NULL)
WOW_NATIVE_CALL(BOOL, EndPaint, (HWND w, const PAINTSTRUCT *paint), (w, paint), FALSE)
WOW_NATIVE_CALL(int, SetScrollInfo, (HWND w, int bar, LPCSCROLLINFO info, BOOL redraw), (w, bar, info, redraw), 0)
WOW_NATIVE_CALL(BOOL, CreateCaret, (HWND w, HBITMAP bitmap, int width, int height), (w, bitmap, width, height), FALSE)
WOW_NATIVE_CALL_TARGET(BOOL, DestroyCaret, (void), (), FALSE, NULL)
WOW_NATIVE_CALL_TARGET(BOOL, SetCaretPos, (int x, int y), (x, y), FALSE, NULL)
WOW_NATIVE_CALL(BOOL, HideCaret, (HWND w), (w), FALSE)
WOW_NATIVE_CALL(BOOL, ShowCaret, (HWND w), (w), FALSE)
WOW_NATIVE_CALL(BOOL, InvalidateRect, (HWND w, const RECT *area, BOOL erase), (w, area, erase), FALSE)
/* ADAPTER-WOW-061: original EDIT/combo/list/modal code also enters USER
 * through public paint APIs. UpdateWindow and RDW_UPDATENOW can synchronously
 * call application procedures; invalidation-only calls keep the same owner.
 * Do not implement a separate control paint pump or bypass return publication. */
WOW_NATIVE_CALL(BOOL, UpdateWindow, (HWND w), (w), FALSE)
WOW_NATIVE_CALL(BOOL, RedrawWindow, (HWND w, const RECT *area, HRGN region, UINT flags), (w, area, region, flags), FALSE)
WOW_NATIVE_CALL_TARGET(HCURSOR, SetCursor, (HCURSOR cursor), (cursor), NULL, NULL)
WOW_NATIVE_CALL(DWORD, SetClassLongA, (HWND w, int index, LONG value), (w, index, value), 0)
WOW_NATIVE_CALL(int, GetWindowTextA, (HWND w, LPSTR text, int count), (w, text, count), 0)
/* Original mnemonic lookup requests Unicode text and may reenter a control.
 * Dialog message filters likewise execute application Hooks. Both use the
 * existing native-call ownership/return boundary, not direct USER calls. */
WOW_NATIVE_CALL(int, GetWindowTextW, (HWND w, LPWSTR text, int count), (w, text, count), 0)
/* ADAPTER-WOW-103: preserve the original ANSI entry, not an A-to-W rewrite. */
WOW_NATIVE_CALL_TARGET(BOOL, CallMsgFilterA, (LPMSG message, int code),
    (message, code), FALSE, message ? message->hwnd : NULL)
WOW_NATIVE_CALL_TARGET(BOOL, CallMsgFilterW, (LPMSG message, int code),
    (message, code), FALSE, message ? message->hwnd : NULL)
WOW_NATIVE_CALL(int, GetWindowTextLengthA, (HWND w), (w), 0)
WOW_NATIVE_CALL(BOOL, SetWindowTextA, (HWND w, LPCSTR text), (w, text), FALSE)
WOW_NATIVE_CALL(BOOL, SetWindowTextW, (HWND w, LPCWSTR text), (w, text), FALSE)
/* lboxctl3.c supplies the dialog and item ID, but WM_SETTEXT calls the
 * selected child. Target that child for the existing execution handoff;
 * keep native USER's lookup, failure and text semantics unchanged. */
WOW_NATIVE_CALL_TARGET(BOOL, SetDlgItemTextW, (HWND w, int id, LPCWSTR text), (w, id, text), FALSE, GetDlgItem(w, id))
#define WOW_NATIVE_MESSAGE(name, params, args) \
    WOW_NATIVE_CALL_AFTER(LRESULT, name, params, args, 0, w, \
        wow_user_native_call_result(&call, message, wp, lp, result))
WOW_NATIVE_MESSAGE(SendMessageA, (HWND w, UINT message, WPARAM wp, LPARAM lp), (w, message, wp, lp))
/* Recovered original controls select A/W themselves. The Unicode branch
 * needs the same execution-return owner, not an ANSI conversion detour. */
WOW_NATIVE_MESSAGE(SendMessageW, (HWND w, UINT message, WPARAM wp, LPARAM lp), (w, message, wp, lp))
WOW_NATIVE_CALL_AFTER(LRESULT, SendMessageTimeoutA, (HWND w, UINT message, WPARAM wp, LPARAM lp, UINT flags, UINT timeout, PDWORD_PTR value), (w, message, wp, lp, flags, timeout, value), 0, w,
    if (result && value) wow_user_native_call_result(&call, message, wp, lp, (LRESULT)*value))
WOW_NATIVE_MESSAGE(DefWindowProcA, (HWND w, UINT message, WPARAM wp, LPARAM lp), (w, message, wp, lp))
WOW_NATIVE_MESSAGE(DefWindowProcW, (HWND w, UINT message, WPARAM wp, LPARAM lp), (w, message, wp, lp))
LRESULT WINAPI wow_native_DefDlgProcA(HWND w, UINT message, WPARAM wp, LPARAM lp)
{
    wow_user_native_call call;
    LONG result;
    BOOL restored;
    DWORD error = GetLastError();
    if (wow_dialog_default_message(w, message, wp, lp, &result)) return result;
    /* Untouched native dialogs keep modern USER ownership. Recovered state
     * must never fall through here after a handled zero/error result. */
    if (!wow_user_native_call_begin(&call, w)) return 0;
    SetLastError(error);
    __try {
        result = DefDlgProcA(w, message, wp, lp);
        wow_user_native_call_result(&call, message, wp, lp, result);
    } __finally {
        error = GetLastError();
        restored = wow_user_native_call_end(&call);
        if (restored) SetLastError(error);
    }
    return restored ? result : 0;
}
WOW_NATIVE_MESSAGE(DefMDIChildProcA, (HWND w, UINT message, WPARAM wp, LPARAM lp), (w, message, wp, lp))
WOW_NATIVE_MESSAGE(DefFrameProcA, (HWND w, HWND client, UINT message, WPARAM wp, LPARAM lp), (w, client, message, wp, lp))
#undef WOW_NATIVE_MESSAGE
WOW_NATIVE_CALL_TARGET(LRESULT, DispatchMessageA, (const MSG *message), (message), 0, message ? message->hwnd : NULL)
WOW_NATIVE_CALL_TARGET(LRESULT, DispatchMessageW, (const MSG *message), (message), 0, message ? message->hwnd : NULL)
#undef WOW_NATIVE_CALL
#undef WOW_NATIVE_CALL_TARGET
#undef WOW_NATIVE_CALL_RESULT
#undef WOW_NATIVE_CALL_AFTER

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
        if (task->idle_observer && !UnhookWindowsHookEx(task->idle_observer))
            __leave;
        task->idle_observer = NULL;
        if (task->creation_hook && !UnhookWindowsHookEx(task->creation_hook))
            __leave;
        task->creation_hook = NULL;
        if (task->terminal_observer && !UnhookWindowsHookEx(task->terminal_observer))
            __leave;
        task->terminal_observer = NULL;
        /* DestroyTask releases only this task's execution view. Never clear
         * another live task's compatibility version during retirement. */
        /* Keep the task and its window-owner references alive if the finite
         * native boundary cannot complete original object cleanup. */
        /* All refusing cleanup edges have completed. A terminal thread may
         * abandon its native frames; withdraw the borrowed top only now,
         * immediately before removing the original task under this lock. */
        task->thread.execution = NULL;
        wow_input_group_retire(&owner->input, &task->thread);
        *link = task->next;
        DestroyTask(&owner->process, &task->thread);
        if (binding->thread == &task->thread)
            (void)wow_user_runtime_set_context(binding, NULL, NULL);
        list_remove(&task->thread.PtiLink);
        wow_user_message_bridge_dispose(&task->messages);
        CloseHandle(task->queue_event);
        CloseHandle(task->thread.native_thread);
        /* DestroyTask releases the TDB, not THREADINFO's application name.
         * Retain DeleteThreadInfo's separate final-release responsibility. */
        cleanup_release(task->thread.pstrAppName);
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
            task->thread.ptdb->hTaskWow != task_id))) {
        retirement_trace("cleanup-task-identity", task_id, ERROR_INVALID_STATE);
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    /* wuser.c::ModuleUnload calls the task branch and then task==0 module
     * cleanup while guest code still runs. SG_EXIT precedes GlobalFreeAll;
     * W32FreeTask is too late for modeless dialogs' guest resource pointers.
     * Native destruction may call back: do not hold the USER lock here.
     * Module-only unload must not destroy the current task's windows.
     * Neither branch destroys THREADINFO. */
    if (task_id && !wow_user_task_prepare_exit()) return FALSE;
    if (!wow_user_runtime_enter(binding)) return FALSE;
    __try {
        owner->cleanup.thread = &task->thread;
        result = wow_user_cleanup_bound(instance, task_id, selectors, count,
            &owner->cleanup);
        if (!result) retirement_trace("cleanup-original-boundary", task_id, GetLastError());
    } __finally {
        owner->cleanup.thread = NULL;
        (void)wow_user_runtime_leave(binding);
    }
    return result;
}

static BOOL destroy_terminal_windows(wow_user_task_lifecycle *owner,
    wow_user_task_lifecycle_thread *task, wow_user_runtime_thread *binding)
{
    ULONG index;
    BOOL prepared = FALSE;
    if (!wow_user_runtime_enter(binding)) return FALSE;
    __try {
        /* queue.c marks cleanup before PatchThreadWindows. Prepare all
         * targets before any native destruction can notify another window. */
        task->thread.TIF_flags |= WOW_TASK_TIF_INCLEANUP;
        if (!wow_window_words_collect_retired(owner->objects)) __leave;
        for (index = 0; index <= owner->objects->last_handle; ++index) {
            wow_cleanup_handle *entry = &owner->objects->entries[index];
            wow_cleanup_window *view;
            if (entry->bType != TYPE_WINDOW || entry->pOwner != &task->thread)
                continue;
            view = entry->phead; /* Existing table owns this storage. */
            if (!view || !wow_window_words_prepare_exit(
                    owner->objects->windows[index], view->storage)) __leave;
        }
        prepared = TRUE;
    } __finally {
        if (!wow_user_runtime_leave(binding))
            wow_user_native_call_failed("release-terminal-preparation", GetLastError());
    }
    if (!prepared) return FALSE;
    for (index = 0; index <= owner->objects->last_handle; ++index) {
        HWND window = NULL;
        wow_window_words_binding *borrow = NULL;
        BOOL destroyed;
        if (!wow_user_runtime_enter(binding)) return FALSE;
        __try {
            wow_cleanup_handle *entry = &owner->objects->entries[index];
            if (entry->bType == TYPE_WINDOW && entry->pOwner == &task->thread) {
                window = owner->objects->windows[index];
                borrow = wow_window_words_acquire(window);
            }
        } __finally {
            if (!wow_user_runtime_leave(binding))
                wow_user_native_call_failed("release-terminal-window", GetLastError());
        }
        if (!window) continue; /* Parent destruction may have consumed it. */
        if (!borrow) return FALSE;
        /* Native USER owns child ordering/control teardown. Never hold the
         * domain lock across it; the retained storage survives reentrancy. */
        /* Retain the established native-call execution handoff too: native
         * destruction may notify a sibling task on another native thread. */
        __try { destroyed = wow_native_DestroyWindow(window); }
        __finally { wow_window_words_release(borrow); }
        if (!destroyed) return FALSE;
    }
    if (!wow_user_runtime_enter(binding)) return FALSE;
    __try {
        prepared = wow_window_words_collect_retired(owner->objects);
        /* The final native-window borrower was released after DestroyWindow's
         * return publication. Drain its now-unreferenced observed CLS too;
         * there may be no later callback scope on this exiting thread. */
        if (mvdm_softpc_wow_page_domain_active() &&
                !mvdm_softpc_wow_page_domain_collect_observed_classes())
            prepared = FALSE;
    }
    __finally {
        if (!wow_user_runtime_leave(binding))
            wow_user_native_call_failed("release-terminal-collection", GetLastError());
    }
    return prepared;
}

BOOL WINAPI wow_user_task_prepare_exit(void)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    wow_user_task_lifecycle *owner;
    wow_user_task_lifecycle_thread *task;
    BOOL result = FALSE;
    /* Failed task startup may exit before USER publishes any task carrier. */
    if (!binding || !binding->thread) return TRUE;
    owner = binding->runtime ? binding->runtime->lifecycle : NULL;
    task = owner ? current_task(owner) : NULL;
    if (!task) { SetLastError(ERROR_INVALID_STATE); return FALSE; }
    if (binding->resource_cleanup_live) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    binding->resource_cleanup_live = TRUE;
    __try { result = destroy_terminal_windows(owner, task, binding); }
    __finally { binding->resource_cleanup_live = FALSE; }
    return result;
}

static BOOL WINAPI retire_current_thread(PVOID context)
{
    wow_user_task_lifecycle *owner = context;
    wow_user_task_lifecycle_thread *task = current_task(owner);
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    ULONG index;
    if (!task) { SetLastError(ERROR_INVALID_STATE); return FALSE; }
    if (!wow_user_runtime_enter(binding)) return FALSE;
    if (task->thread.execution && !binding->terminal_exit) {
        (void)wow_user_runtime_leave(binding);
        SetLastError(ERROR_BUSY);
        return FALSE;
    }
    if (!wow_user_runtime_leave(binding))
        wow_user_native_call_failed("retire-execution-scope", GetLastError());
    retirement_trace("enter", task->thread.ptdb->hTaskWow, 0);
    /* Window destruction belongs before original resource teardown, not
     * here. Late finalization retains/refuses any surviving object owner. */
    if (binding && binding->terminal_exit)
        task->thread.TIF_flags |= WOW_TASK_TIF_INCLEANUP;
    /* Retryable detach rejects live windows without changing them. Terminal
     * cleanup must also prove its destruction pass removed every owner. */
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

BOOL WINAPI wow_user_task_lifecycle_dispose(wow_user_task_lifecycle *owner)
{
    wow_class_lookup_entry *entry;
    if (!owner || !owner->initialized || !owner->runtime ||
            owner->runtime->lifecycle != owner) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    if (owner->threads || owner->runtime->owner_thread_id || owner->input.links) {
        SetLastError(ERROR_BUSY); return FALSE;
    }
    /* Refuse application classes/live control associations before releasing
     * any borrowed menu or class storage. A rejected last-owner detach must
     * retain the standard classes needed by a subsequent cleanup retry. */
    if (owner->classes.pclsPrivateList || owner->classes.system_classes) {
        SetLastError(ERROR_BUSY); return FALSE;
    }
    for (entry = owner->classes.pclsPublicList; entry; entry = entry->next) {
        if (!wow_class_words_native_registration((wow_class_words_binding *)entry) ||
                entry->cWndReferenceCount) {
            SetLastError(ERROR_BUSY); return FALSE;
        }
    }
    /* Original DestroyProcessInfo runs after thread/object cleanup. This
     * owner releases only its finite carrier, never live USER objects or a
     * class registration. Their original cleanup remains the prerequisite. */
    if (!wow_menu_client_forget_borrowed(&owner->process)) return FALSE;
    if (!wow_class_words_forget_native(&owner->classes)) return FALSE;
    if (owner->classes.pclsPrivateList || owner->classes.pclsPublicList ||
            owner->classes.system_classes) {
        SetLastError(ERROR_BUSY); return FALSE;
    }
    if (owner->process.pwpi) {
        if (owner->process.pwpi->pEventWowExec &&
                !CloseHandle(owner->process.pwpi->pEventWowExec)) return FALSE;
        HeapFree(GetProcessHeap(), 0, owner->process.pwpi);
    }
    if (owner->objects) HeapFree(GetProcessHeap(), 0, owner->objects);
    if (owner->runtime && owner->runtime->lifecycle == owner) {
        owner->runtime->retire_thread = NULL;
        owner->runtime->dispose_process = NULL;
        owner->runtime->lifecycle = NULL;
    }
    ZeroMemory(owner, sizeof(*owner));
    return TRUE;
}
