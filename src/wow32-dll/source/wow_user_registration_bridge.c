#include "wow_user_runtime.h"
#include "wow_window_creation_binding.h"
#include "wow_dialog_creation_binding.h"
#include "wow_class_query_bindings.h"
#include "wow_private_user_compat.h"
#include "wow_bitmap_bindings.h"
#include "wow_task_order_bindings.h"
#include "wow_user_task_lifecycle.h"
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

static void unsupported_void(void)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}

static HWND WINAPI unsupported_create_window(DWORD a, LPCTSTR b, LPCTSTR c,
    DWORD d, int e, int f, int g, int h, HWND i, HMENU j, HANDLE k, LPVOID l,
    DWORD m, LPDWORD n)
{
    UNREFERENCED_PARAMETER(a); UNREFERENCED_PARAMETER(b);
    UNREFERENCED_PARAMETER(c); UNREFERENCED_PARAMETER(d);
    UNREFERENCED_PARAMETER(e); UNREFERENCED_PARAMETER(f);
    UNREFERENCED_PARAMETER(g); UNREFERENCED_PARAMETER(h);
    UNREFERENCED_PARAMETER(i); UNREFERENCED_PARAMETER(j);
    UNREFERENCED_PARAMETER(k); UNREFERENCED_PARAMETER(l);
    UNREFERENCED_PARAMETER(m); UNREFERENCED_PARAMETER(n);
    unsupported_void(); return NULL;
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

static ATOM WINAPI registered_register_class(PVOID a, LPDWORD b)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    if (!binding || !binding->classes) {
        unsupported_void(); return 0;
    }
    return wow_class_client_register(binding->classes, (WNDCLASSA *)a, b);
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
    BOOL result = ensure_lifecycle() && wow_user_task_lifecycle_wow_cleanup(
        &wow_lifecycle, instance, task, selectors, count);
    registration_trace("WOWCleanup", result);
    return result;
}

static DWORD WINAPI unsupported_full_handle(WORD handle)
{
    UNREFERENCED_PARAMETER(handle);
    unsupported_void(); return 0;
}

static VOID WINAPI registered_directed_yield(DWORD thread_id)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    if (!binding || !binding->thread) { unsupported_void(); return; }
    xxxDirectedYield(thread_id, binding->thread);
}

static BOOL WINAPI registered_yield_task(void)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    if (!binding || !binding->thread) { unsupported_void(); return FALSE; }
    return xxxUserYield(binding->thread);
}

static BOOL WINAPI registered_wait_for_message(HANDLE event)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    if (!binding || !binding->thread) { unsupported_void(); return FALSE; }
    return xxxSleepTask(FALSE, event, binding->thread);
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
    if (!input || !output) {
        SetLastError(ERROR_INVALID_PARAMETER);
        registration_trace("UserRegisterWowHandlers", 0);
        return 0;
    }

    wow_input_handlers = *input;
    ZeroMemory(output, sizeof(*output));
    /* Pinned USER.EXE publishes WOW 0400 with WOWDBG layout (init.c).
     * The shared view uses that checked layout, including 16-byte handles. */
    output->dwBldInfo = 0x84000000u;
    output->pfnCsCreateWindowEx = wow_user_create_window_ex;
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
    output->pfnGetFullUserHandle = unsupported_full_handle;
    output->pfnGetMenuIndex = wow_private_user_get_menu_index;
    output->pfnWowGetDefWindowProcBits =
        wow_private_user_get_def_window_proc_bits;
    output->pfnFillWindow = registered_fill_window;
    {
        DWORD shared = (DWORD)mvdm_softpc_wow_page_domain_guest_shared_info();
        registration_trace("UserRegisterWowHandlers", shared);
        return shared;
    }
}
