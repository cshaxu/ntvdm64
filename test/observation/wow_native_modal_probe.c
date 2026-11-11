/* Test-only native peer; operates only the pinned worker's two probe dialogs
 * on its observer-owned, unswitched desktop. No product helper HWND/hook. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
typedef struct modal_search { DWORD worker; PCWSTR text; HWND window, observed; } modal_search;
static int complete(HDESK desktop, HDESK original, int code)
{
    if (!SetThreadDesktop(original) && !code) code = 11;
    if (!CloseDesktop(desktop) && !code) code = 9;
    return code;
}
static BOOL CALLBACK find_dialog(HWND window, LPARAM parameter)
{
    modal_search *search = (modal_search *)parameter;
    WCHAR title[128], name[32];
    DWORD process = 0;
    if (!GetWindowThreadProcessId(window, &process) || process != search->worker ||
            !GetClassNameW(window, name, ARRAYSIZE(name)) || wcscmp(name, L"#32770") ||
            !GetWindowTextW(window, title, ARRAYSIZE(title)) || !wcsstr(title, search->text)) return TRUE;
    if (search->observed != window) {
        HWND button = GetDlgItem(window, IDOK);
        printf("MODAL_FOUND hwnd=%p visible=%u enabled=%u button=%p id=%d button_enabled=%u\n",
            window, IsWindowVisible(window), IsWindowEnabled(window), button,
            GetDlgCtrlID(button), IsWindowEnabled(button));
        fflush(stdout);
        search->observed = window;
    }
    if (!IsWindowVisible(window) || !IsWindowEnabled(window)) return TRUE;
    search->window = window;
    return FALSE;
}
int wmain(int argc, WCHAR **argv)
{
    HDESK desktop, original = GetThreadDesktop(GetCurrentThreadId());
    modal_search search;
    unsigned phase;
    DWORD previous_thread = 0;
    WCHAR *end;
    if (argc != 3) return 2;
    search.worker = wcstoul(argv[1], &end, 10);
    if (*end || !search.worker) return 3;
    desktop = OpenDesktopW(argv[2], 0, FALSE,
        DESKTOP_ENUMERATE | DESKTOP_READOBJECTS | DESKTOP_WRITEOBJECTS);
    if (!desktop) return 4;
    /* Match the established gameplay probe's interaction contract. Merely
     * enumerating a foreign desktop is a read-only observation, not input
     * ownership. Joining never switches the user's physical input desktop. */
    if (!SetThreadDesktop(desktop)) { CloseDesktop(desktop); return 4; }
    for (phase = 0; phase < 2; ++phase) {
        ULONGLONG deadline = GetTickCount64() + 20000;
        DWORD thread, process = 0;
        DWORD_PTR response = 0;
        HWND button;
        search.window = search.observed = NULL;
        search.text = phase ? L"T447 Modal scope" : L"T447 MessageBox scope";
        do {
            if (!EnumDesktopWindows(desktop, find_dialog, (LPARAM)&search) && !search.window) {
                return complete(desktop, original, 5);
            }
            if (search.window) break;
            Sleep(25);
        } while (GetTickCount64() < deadline);
        if (!search.window) return complete(desktop, original, 6);
        thread = GetWindowThreadProcessId(search.window, &process);
        button = GetDlgItem(search.window, IDOK);
        printf("MODAL_READY phase=%u pid=%lu tid=%lu hwnd=%p visible=%u enabled=%u button=%p button_enabled=%u\n",
            phase, process, thread, search.window, IsWindowVisible(search.window),
            IsWindowEnabled(search.window), button, IsWindowEnabled(button));
        fflush(stdout);
        /* Cached text and child creation alone do not prove modal init has
         * finished. Require a real responsive window before issuing input. */
        if (!SendMessageTimeoutW(search.window, WM_NULL, 0, 0,
                SMTO_BLOCK | SMTO_ABORTIFHUNG, 3000, &response)) {
            printf("MODAL_READY_FAILED phase=%u error=%lu\n", phase, GetLastError());
            return complete(desktop, original, 10);
        }
        if (!thread || process != search.worker ||
                (previous_thread && previous_thread != thread) ||
                /* Do not assume a private/direct IDOK child or internal
                 * WM_COMMAND handling in modern MessageBox. Exercise the
                 * public window-close path; the native control establishes
                 * the exact modal result, and the guest must retain it. */
                !PostMessageW(search.window, WM_CLOSE, 0, 0)) {
            return complete(desktop, original, 7);
        }
        previous_thread = thread;
        deadline = GetTickCount64() + 5000;
        while (IsWindow(search.window) && GetTickCount64() < deadline) Sleep(25);
        if (IsWindow(search.window)) return complete(desktop, original, 8);
        printf("MODAL_NATIVE phase=%u pid=%lu tid=%lu real_dialog=1 closed=1\n", phase, process, thread);
        fflush(stdout);
    }
    puts("PASS actual MessageBox and ShellAbout dialogs closed in one verified worker thread");
    return complete(desktop, original, 0);
}
