#include "wow_user_runtime.h"
#include "wow_window_creation_binding.h"
#include "wow_dialog_creation_binding.h"
#include "wow_class_query_bindings.h"
#include "wow_class_lookup_bindings.h"
#include "wow_private_user_compat.h"
#include "wow_bitmap_bindings.h"
#include "wow_task_order_bindings.h"
#include "wow_user_task_lifecycle.h"
#include "wow_user_object_bindings.h"
#include "wow_user_borrow_scope.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"

#include <stdio.h>

VOID WINAPI FreeDDEData(HANDLE, BOOL, BOOL);

/* ADAPTER-WOW-056: UserRegisterWowHandlers is one original all-or-nothing
 * ABI transaction.  Modern USER32 exposes its historical name but rejects
 * registration, so the provider owns this narrow transport seam.  All 21
 * WOW-to-USER callbacks are retained verbatim for original owners introduced
 * by later packets.  Every 20 USER-to-WOW output slots is non-null: a slot
 * with a selected source-backed carrier is wired immediately; an otherwise
 * unimplemented slot fails explicitly and is labelled below.  No placeholder
 * reports successful work or invents USER policy. */
static PFNWOWHANDLERSIN wow_input_handlers;
static wow_user_task_lifecycle wow_lifecycle;

static LRESULT call_previous_window_proc(WNDPROC procedure, HWND window,
    UINT message, WPARAM wp, LPARAM lp, BOOL unicode)
{
    wow_user_borrow_scope scope;
    LRESULT result;
    /* Original clmsg.c::CallWindowProcAorW resolves CPD first, then
     * usercli.h::CALLPROC_WOWCHECK forwards tagged WOW targets with NULL WW.
     * Modern USER owns native/CPD translation but cannot interpret this
     * worker's WOW target. Keep the original registered callback as owner. */
    if (HIWORD(procedure) == WNDPROC_HANDLE ||
            !((DWORD)(ULONG_PTR)procedure & WNDPROC_WOW))
        return unicode ? CallWindowProcW(procedure, window, message, wp, lp) :
            CallWindowProcA(procedure, window, message, wp, lp);
    if (!wow_input_handlers.pfnWowWndProcEx) {
        SetLastError(ERROR_INVALID_STATE);
        return 0;
    }
    if (!wow_user_borrow_enter(&scope)) return 0;
    __try {
        result = wow_input_handlers.pfnWowWndProcEx(window, message, wp, lp,
            (DWORD)(ULONG_PTR)procedure, NULL);
    } __finally {
        wow_user_borrow_leave(&scope);
    }
    return result;
}

LRESULT WINAPI wow_user_call_window_procA(WNDPROC procedure, HWND window,
    UINT message, WPARAM wp, LPARAM lp)
{
    return call_previous_window_proc(procedure, window, message, wp, lp, FALSE);
}

LRESULT WINAPI wow_user_call_window_procW(WNDPROC procedure, HWND window,
    UINT message, WPARAM wp, LPARAM lp)
{
    return call_previous_window_proc(procedure, window, message, wp, lp, TRUE);
}

/* The registration seam is normally silent.  An opt-in host-only witness is
 * retained because the immutable USER16 workload has no textual way to show
 * whether it reached the all-or-nothing registration call. */
static void registration_trace(const char *stage, DWORD result)
{
    char path[MAX_PATH];
    char line[96];
    HANDLE file;
    DWORD bytes;
    DWORD written;

    if (!GetEnvironmentVariableA("MVDM_WOW_REGISTRATION_TRACE_PATH", path,
            sizeof(path)))
        return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ |
        FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return;
    bytes = (DWORD)sprintf_s(line, sizeof(line),
        "%lu %s result=%08lX\r\n", (unsigned long)GetCurrentProcessId(),
        stage, (unsigned long)result);
    if (bytes)
        (void)WriteFile(file, line, bytes, &written, NULL);
    CloseHandle(file);
}

static void registration_trace_class(LPCSTR name, ATOM result)
{
    char path[MAX_PATH];
    char line[192];
    HANDLE file;
    DWORD bytes;
    DWORD written;

    if (!GetEnvironmentVariableA("MVDM_WOW_REGISTRATION_TRACE_PATH", path,
            sizeof(path))) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ |
        FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    bytes = (DWORD)sprintf_s(line, sizeof(line),
        "%lu RegisterClassName name=%s atom=%04X\r\n",
        (unsigned long)GetCurrentProcessId(), name ? name : "<null>",
        (unsigned)result);
    if (bytes) (void)WriteFile(file, line, bytes, &written, NULL);
    CloseHandle(file);
}

static void registration_trace_context(const wow_class_lookup_context *classes)
{
    char path[MAX_PATH];
    char line[96];
    HANDLE file;
    DWORD bytes;
    DWORD written;
    if (!GetEnvironmentVariableA("MVDM_WOW_REGISTRATION_TRACE_PATH", path,
            sizeof(path))) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ |
        FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    bytes = (DWORD)sprintf_s(line, sizeof(line),
        "%lu RegisterClassContext value=%08lX private=%08lX public=%08lX\r\n",
        (unsigned long)GetCurrentProcessId(), (unsigned long)(ULONG_PTR)classes,
        (unsigned long)(ULONG_PTR)(classes ? classes->pclsPrivateList : NULL),
        (unsigned long)(ULONG_PTR)(classes ? classes->pclsPublicList : NULL));
    if (bytes) (void)WriteFile(file, line, bytes, &written, NULL);
    CloseHandle(file);
}

static void unsupported_void(void)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}

static BOOL ensure_lifecycle(void)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    wow_user_task_lifecycle_callbacks callbacks;

    if (!binding || !binding->runtime) {
        unsupported_void();
        return FALSE;
    }
    if (wow_lifecycle.initialized)
        return wow_lifecycle.runtime == binding->runtime;
    callbacks.get_version = wow_input_handlers.pfnGetExpWinVer;
    callbacks.callback = wow_input_handlers.pfnWowWndProcEx;
    return wow_user_task_lifecycle_initialize(&wow_lifecycle, binding->runtime,
        &callbacks);
}

static BOOL WINAPI registered_init_task(UINT version, LPCSTR app_name,
    DWORD task_id, DWORD hotkey, DWORD shared_id, DWORD x, DWORD y,
    DWORD width, DWORD height, WORD show)
{
    BOOL result = ensure_lifecycle() && wow_user_task_lifecycle_init(&wow_lifecycle,
        version, app_name, task_id, hotkey, shared_id, x, y, width, height,
        show);
    registration_trace("InitTask", result);
    return result;
}

static HWND WINAPI registered_create_window_ex(DWORD ex_style,
    LPCTSTR class_name, LPCTSTR window_name, DWORD style, int x, int y,
    int width, int height, HWND parent, HMENU menu, HANDLE instance,
    LPVOID param, DWORD flags, LPDWORD wow_words)
{
    HWND result = wow_user_create_window_ex(ex_style, class_name, window_name,
        style, x, y, width, height, parent, menu, instance, param, flags,
        wow_words);
    registration_trace("CreateWindowEx", (DWORD)(ULONG_PTR)result);
    return result;
}

static ATOM WINAPI registered_register_class(PVOID a, LPDWORD b)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    if (!binding || !binding->classes) {
        unsupported_void(); return 0;
    }
    {
        ATOM result;
        registration_trace_context(binding->classes->classes);
        result = wow_class_client_register(binding->classes, (WNDCLASSA *)a, b);
        registration_trace_context(binding->classes->classes);
        registration_trace("RegisterClass", result);
        registration_trace_class(((WNDCLASSA *)a)->lpszClassName, result);
        return result;
    }
}

static BOOL WINAPI registered_hung_handlers(PFNW32ET callback, HANDLE event)
{
    BOOL result = ensure_lifecycle() && wow_user_task_lifecycle_register(&wow_lifecycle,
        callback, event);
    registration_trace("RegisterHung", result);
    return result;
}

static BOOL WINAPI registered_cleanup(HANDLE instance, DWORD task,
    PNEMODULESEG selectors, DWORD count)
{
    BOOL result = ensure_lifecycle() && wow_user_task_lifecycle_exit(
        &wow_lifecycle, instance, task, selectors, count);
    registration_trace("WOWCleanup", result);
    return result;
}

static DWORD WINAPI registered_full_handle(WORD handle)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    HWND window;

    if (!binding || !binding->thread || !binding->thread->ppi ||
            !binding->thread->ppi->objects) {
        unsupported_void();
        return 0;
    }
    window = wow_user_window_full_handle(binding->thread->ppi->objects, handle);
    registration_trace("GetFullUserHandle", (DWORD)(ULONG_PTR)window);
    return (DWORD)(ULONG_PTR)window;
}

static VOID WINAPI registered_directed_yield(DWORD thread_id)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    if (!binding || !binding->thread) { unsupported_void(); return; }
    wow_user_task_lifecycle_directed_yield(&wow_lifecycle, thread_id);
    registration_trace("DirectedYield", thread_id);
}

static BOOL WINAPI registered_yield_task(void)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    BOOL result;
    if (!binding || !binding->thread) { unsupported_void(); return FALSE; }
    result = wow_user_task_lifecycle_yield(&wow_lifecycle);
    registration_trace("YieldTask", result);
    return result;
}

static BOOL WINAPI registered_wait_for_message(HANDLE event)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    BOOL result;
    if (!binding || !binding->thread) { unsupported_void(); return FALSE; }
    result = wow_user_task_lifecycle_wait(&wow_lifecycle, event);
    registration_trace("WaitForMessage", result);
    return result;
}

static HBITMAP WINAPI registered_load_bitmap(HINSTANCE module, LPCSTR name,
    LPBYTE bits, DWORD bytes)
{
    if (!wow_bitmap_current) { unsupported_void(); return NULL; }
    return WOWLoadBitmapA(module, name, bits, bytes);
}

static HWND WINAPI registered_find_window(LPCSTR class_name, LPCSTR window_name)
{
    return FindWindowA(class_name, window_name);
}

static VOID WINAPI registered_fill_window(HWND parent, HWND window, HDC dc,
    HANDLE brush)
{
    (void)wow_private_user_fill_window(parent, window, dc, (HBRUSH)brush);
}

DWORD WINAPI UserRegisterWowHandlers(APFNWOWHANDLERSIN input,
    APFNWOWHANDLERSOUT output)
{
    DWORD shared;
    if (!input || !output) {
        SetLastError(ERROR_INVALID_PARAMETER);
        registration_trace("UserRegisterWowHandlers", 0);
        return 0;
    }

    shared = (DWORD)mvdm_softpc_wow_page_domain_guest_shared_info();
    if (!shared || !mvdm_softpc_wow_page_domain_client_desktop_ready()) {
        SetLastError(ERROR_INVALID_STATE);
        registration_trace("UserRegisterWowHandlers", 0);
        return 0;
    }

    wow_input_handlers = *input;
    ZeroMemory(output, sizeof(*output));
    /* Pinned USER.EXE publishes WOW 0400 with WOWDBG layout (init.c).
     * The shared view uses that checked layout, including 16-byte handles. */
    output->dwBldInfo = 0x84000000u;
    output->pfnCsCreateWindowEx = registered_create_window_ex;
    output->pfnDirectedYield = registered_directed_yield;
    output->pfnFreeDDEData = FreeDDEData;
    output->pfnGetClassWOWWords = GetClassWOWWords;
    output->pfnInitTask = registered_init_task;
    output->pfnRegisterClassWOWA = registered_register_class;
    output->pfnRegisterUserHungAppHandlers = registered_hung_handlers;
    output->pfnServerCreateDialog = wow_user_server_create_dialog;
    output->pfnServerLoadCreateCursorIcon =
        wow_private_user_load_create_cursor_icon;
    output->pfnServerLoadCreateMenu = wow_private_user_load_create_menu;
    output->pfnWOWCleanup = registered_cleanup;
    output->pfnWOWFindWindow = registered_find_window;
    output->pfnWOWGetIdFromDirectory = wow_private_user_get_id_from_directory;
    output->pfnWOWLoadBitmapA = registered_load_bitmap;
    output->pfnWowWaitForMsgAndEvent = registered_wait_for_message;
    output->pfnYieldTask = registered_yield_task;
    output->pfnGetFullUserHandle = registered_full_handle;
    output->pfnGetMenuIndex = wow_private_user_get_menu_index;
    output->pfnWowGetDefWindowProcBits =
        wow_private_user_get_def_window_proc_bits;
    output->pfnFillWindow = registered_fill_window;
    registration_trace("UserRegisterWowHandlers", shared);
    return shared;
}
