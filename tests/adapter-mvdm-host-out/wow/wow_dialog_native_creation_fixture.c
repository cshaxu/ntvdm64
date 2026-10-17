#include "wow_dialog_native_bindings.h"
#include "wow_native_text_descriptor.h"
#include <stdio.h>
#include <string.h>

/* Supplemental real-native dependency test, staged with the S3 group.
 * Hidden, independently authored windows only. Does not prove WOW enrollment,
 * guest callbacks, dialog DS or the original creator/default manager. */
static unsigned failures, nc_creates, creates, destroys;
static BOOL veto;
static void *expected_parameter;
static HWND seen_window;
static unsigned focus_notifications;
static const WCHAR class_name[] = L"T447.OriginalDialog.NativeCreation";
#define CHECK(condition) do { if (!(condition)) { ++failures; \
    printf("FAIL line %d: %s error=%lu\n", __LINE__, #condition, GetLastError()); \
} } while (0)

static LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    if (message == WM_ACTIVATE || message == WM_SETFOCUS ||
            message == WM_KILLFOCUS || message == WM_NCACTIVATE)
        ++focus_notifications;
    if (message == WM_NCCREATE) {
        CREATESTRUCTW *request = (CREATESTRUCTW *)lp;
        ++nc_creates; seen_window = window;
        CHECK(request->lpCreateParams == expected_parameter);
        if (veto) return FALSE;
    }
    if (message == WM_CREATE) ++creates;
    if (message == WM_NCDESTROY) ++destroys;
    return DefWindowProcW(window, message, wp, lp);
}

static void run_creation(ATOM atom, HINSTANCE module, BOOL ansi_creator,
    BOOL ansi_title)
{
    WCHAR wide_text[] = { L'A', 0x4e2d, L'X', L'Y', 0 };
    CHAR narrow_text[] = { 'A', 'B', 'X', 'Y', 0 };
    LARGE_STRING title = {0};
    LARGE_STRING *guarded_title;
    wow_native_text_descriptor converted;
    SYSTEM_INFO system;
    BYTE *pages;
    DWORD old_protection, error;
    WCHAR observed[32] = {0};
    HWND window;
    int parameter = 42;
    nc_creates = creates = destroys = 0; seen_window = NULL;
    expected_parameter = &parameter;
    title.Length = ansi_title ? 2 : 2 * sizeof(WCHAR);
    title.MaximumLength = ansi_title ? sizeof(narrow_text) : sizeof(wide_text);
    title.bAnsi = !!ansi_title;
    title.Buffer = ansi_title ? (PVOID)narrow_text : (PVOID)wide_text;
    GetSystemInfo(&system);
    pages = VirtualAlloc(NULL, system.dwPageSize * 2, MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE);
    CHECK(pages != NULL);
    if (!pages) return;
    if (!VirtualProtect(pages + system.dwPageSize, system.dwPageSize,
            PAGE_NOACCESS, &old_protection)) {
        CHECK(FALSE);
        VirtualFree(pages, 0, MEM_RELEASE);
        return;
    }
    guarded_title = (LARGE_STRING *)(pages + system.dwPageSize - sizeof(title));
    *guarded_title = title;
    __try {
        wow_native_text_capture(&converted, guarded_title);
        CHECK(converted.length == title.Length && converted.buffer_upper == 0);
        CHECK(converted.buffer == (ULONG)(ULONG_PTR)title.Buffer);
        CHECK(converted.maximum_length_and_ansi ==
            (title.MaximumLength | (ansi_title ? 0x80000000u : 0)));
        window = wow_dialog_native_create_window(ansi_creator ? WS_EX_ANSICREATOR : 0,
            atom, guarded_title, WS_OVERLAPPED, 10, 10, 120, 80, NULL, NULL, module,
            &parameter, 0x030a);
        CHECK(!memcmp(guarded_title, &title, sizeof(title)));
    } __finally {
        error = GetLastError();
        CHECK(VirtualFree(pages, 0, MEM_RELEASE));
        SetLastError(error);
    }
    CHECK(window != NULL);
    if (!window) return;
    CHECK(window == seen_window && nc_creates == 1 && creates == 1);
    CHECK(!IsWindowVisible(window));
    CHECK(GetWindowTextW(window, observed, ARRAYSIZE(observed)) == 2);
    CHECK(observed[0] == L'A' && observed[1] == (ansi_title ? L'B' : 0x4e2d));
    CHECK(observed[2] == 0); /* Counted length, not the trailing XY string. */
    CHECK((GetWindowLongW(window, GWL_EXSTYLE) & WS_EX_ANSICREATOR) == 0);
    CHECK(SendMessageW(window, WM_NOTIFYFORMAT, 0, NF_QUERY) ==
        (ansi_creator ? NFR_ANSI : NFR_UNICODE));
    /* Worker-owned thread/hidden window only. The original queue reset
     * clears both identities without the notifications of public setters. */
    SetActiveWindow(window);
    SetFocus(window);
    CHECK(GetActiveWindow() == window && GetFocus() == window);
    focus_notifications = 0;
    CHECK(wow_dialog_native_zap_active_and_focus());
    CHECK(GetActiveWindow() == NULL && GetFocus() == NULL);
    CHECK(focus_notifications == 0);
    CHECK(wow_dialog_native_zap_active_and_focus());
    CHECK(GetActiveWindow() == NULL && GetFocus() == NULL);
    CHECK(focus_notifications == 0);
    CHECK(DestroyWindow(window));
    CHECK(destroys == 1 && !IsWindow(window));
}

int main(void)
{
    HINSTANCE module = GetModuleHandleW(NULL);
    WNDCLASSEXW registration = {sizeof(registration)};
    ATOM atom;
    HWND window;
    registration.lpfnWndProc = window_proc;
    registration.hInstance = module;
    registration.lpszClassName = class_name;
    if (!wow_dialog_native_ready()) {
        printf("native_dialog_dependency_unavailable=%lu\n", GetLastError());
        return 2;
    }
    atom = RegisterClassExW(&registration);
    if (!atom) return 3;
    run_creation(atom, module, FALSE, FALSE);
    run_creation(atom, module, TRUE, FALSE);
    run_creation(atom, module, FALSE, TRUE);
    run_creation(atom, module, TRUE, TRUE);

    nc_creates = creates = destroys = 0;
    expected_parameter = NULL; seen_window = NULL; veto = TRUE;
    window = wow_dialog_native_create_window(0, atom, NULL, WS_OVERLAPPED,
        10, 10, 120, 80, NULL, NULL, module, NULL, 0x030a);
    CHECK(!window && nc_creates == 1 && creates == 0);
    CHECK(seen_window != NULL && !IsWindow(seen_window));
    CHECK(destroys == 1);
    veto = FALSE;

    SetLastError(0);
    CHECK(wow_dialog_native_create_window(0, 0, NULL, 0, 0, 0, 0, 0,
        NULL, NULL, module, NULL, 0x030a) == NULL);
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(UnregisterClassW(MAKEINTATOM(atom), module));
    printf("native_dialog_creation_failures=%u\n", failures);
    return failures != 0;
}
