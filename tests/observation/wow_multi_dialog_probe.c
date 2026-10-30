/* Message-only, PID-bound Win16 menu/dialog timing probe. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#define WOW_PROBE_PREFIX "WOW dialog: "
#include "wow_probe_thread_snapshot.h"

static DWORD owner_pid;
static HWND main_window, dialog_window, startup_error;
static HWND calc_display;
static const WCHAR *needle;
static const WCHAR *main_title_filter;
static UINT command_id;

static void report_process_state(HANDLE process)
{
    DWORD code = 0;
    DWORD wait = WaitForSingleObject(process, 0);
    BOOL known = GetExitCodeProcess(process, &code);
    printf("worker wait=%lu exit-known=%d exit=%08lx\n", wait, known, code);
}

static UINT find_command(HMENU menu)
{
    int i, count = GetMenuItemCount(menu);
    for (i = 0; i < count; ++i) {
        WCHAR text[256] = L"";
        MENUITEMINFOW item = { sizeof(item) };
        UINT nested;
        item.fMask = MIIM_STRING | MIIM_ID | MIIM_SUBMENU;
        item.dwTypeData = text; item.cch = ARRAYSIZE(text)-1;
        if (!GetMenuItemInfoW(menu, i, TRUE, &item)) continue;
        if (item.hSubMenu && (nested = find_command(item.hSubMenu)) != 0) return nested;
        if (!item.hSubMenu && wcsstr(text, needle) && item.wID && item.wID != (UINT)-1)
            return item.wID;
    }
    return 0;
}

static BOOL CALLBACK find_main(HWND window, LPARAM unused)
{
    DWORD pid = 0; HMENU menu;
    (void)unused;
    WCHAR title[256] = L"", cls[64] = L"";
    GetWindowThreadProcessId(window, &pid);
    if (pid != owner_pid || !IsWindowVisible(window)) return TRUE;
    GetClassNameW(window, cls, ARRAYSIZE(cls));
    GetWindowTextW(window, title, ARRAYSIZE(title));
    if (!wcscmp(cls, L"#32770") && wcsstr(title, L"16-bit Windows program")) {
        startup_error = window;
        return FALSE;
    }
    if (main_title_filter && !wcsstr(title, main_title_filter)) return TRUE;
    if (!(menu = GetMenu(window))) return TRUE;
    command_id = find_command(menu);
    if (command_id) { main_window = window; return FALSE; }
    return TRUE;
}

/* A startup failure is distinct from a missing menu item. Report only
 * windows belonging to this isolated worker after the bounded wait. */
static BOOL CALLBACK report_child(HWND window, LPARAM unused)
{
    WCHAR cls[128] = L"", title[512] = L"";
    (void)unused;
    GetClassNameW(window, cls, ARRAYSIZE(cls));
    GetWindowTextW(window, title, ARRAYSIZE(title));
    if (title[0]) printf("child class=%ls text=%ls\n", cls, title);
    return TRUE;
}

static BOOL CALLBACK report_owned_window(HWND window, LPARAM unused)
{
    DWORD pid = 0;
    WCHAR cls[128] = L"", title[256] = L"";
    (void)unused;
    GetWindowThreadProcessId(window, &pid);
    if (pid != owner_pid) return TRUE;
    GetClassNameW(window, cls, ARRAYSIZE(cls));
    GetWindowTextW(window, title, ARRAYSIZE(title));
    printf("owned hwnd=%p visible=%d class=%ls title=%ls menu=%p\n",
        window, IsWindowVisible(window), cls, title, GetMenu(window));
    /* File/load failures may use a different dialog title from the generic
     * startup error. Report their child text for the owned worker too. */
    if (!wcscmp(cls, L"#32770")) EnumChildWindows(window, report_child, 0);
    return TRUE;
}

static BOOL CALLBACK find_dialog(HWND window, LPARAM unused)
{
    DWORD pid = 0; WCHAR cls[64];
    (void)unused;
    GetWindowThreadProcessId(window, &pid);
    if (pid != owner_pid || !IsWindowVisible(window) || window == main_window ||
        !GetClassNameW(window, cls, ARRAYSIZE(cls)) || wcscmp(cls, L"#32770")) return TRUE;
    dialog_window = window;
    return FALSE;
}

static BOOL CALLBACK find_calc_display(HWND window, LPARAM unused)
{
    WCHAR cls[64] = L"", text[128] = L"";
    const WCHAR *value = text;
    (void)unused;
    GetClassNameW(window, cls, ARRAYSIZE(cls));
    GetWindowTextW(window, text, ARRAYSIZE(text));
    while (*value == L' ') ++value;
    if (!wcscmp(cls, L"Static") &&
            (!wcscmp(value, L"0.") || !wcscmp(value, L"0") || !wcscmp(value, L"0,"))) {
        calc_display = window;
        return FALSE;
    }
    return TRUE;
}

int main(int argc, char **argv)
{
    HANDLE process; HDESK desktop; DWORD_PTR reply; ULONGLONG until, start, visible;
    unsigned round, rounds = 1;
    if (argc != 4 && argc != 5) return 2;
    if (argc == 5) {
        char *end;
        rounds = strtoul(argv[4], &end, 10);
        if (*end || rounds < 1 || rounds > 32) return 2;
    }
    owner_pid = strtoul(argv[1], NULL, 10);
    if (!freopen(argv[3], "w", stdout)) return 2;
    setvbuf(stdout, NULL, _IONBF, 0);
    if (!strcmp(argv[2], "notepad")) needle = L"Find";
    else if (!strcmp(argv[2], "progman")) needle = L"New";
    else if (!strcmp(argv[2], "winmine")) needle = L"Custom";
    else if (!strcmp(argv[2], "calc") || !strcmp(argv[2], "calc-arithmetic")) {
        needle = L"About"; main_title_filter = L"Calculator";
    }
    else if (!strcmp(argv[2], "write")) { needle = L"Find"; main_title_filter = L"Write"; }
    else return 2;
    process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, owner_pid);
    desktop = GetThreadDesktop(GetCurrentThreadId());
    if (!process || !desktop) return 3;
    until = GetTickCount64()+60000;
    do { EnumDesktopWindows(desktop, find_main, 0); if (main_window || startup_error) break; Sleep(25); }
    while (GetTickCount64()<until && WaitForSingleObject(process,0)==WAIT_TIMEOUT);
    if (!main_window) {
        puts("FAIL no matching owned application window/menu");
        EnumDesktopWindows(desktop, report_owned_window, 0);
        printf("worker-live=%d\n", WaitForSingleObject(process, 0) == WAIT_TIMEOUT);
        return 4;
    }
    printf("main=%p command=%u style=%08lx exstyle=%08lx zoomed=%d\n",
        main_window, command_id, (unsigned long)GetWindowLongW(main_window, GWL_STYLE),
        (unsigned long)GetWindowLongW(main_window, GWL_EXSTYLE), IsZoomed(main_window));
    {
        RECT window_rect, client_rect;
        POINT client_origin = {0, 0};
        WCHAR title[256] = L"";
        if (GetWindowRect(main_window, &window_rect) &&
                GetClientRect(main_window, &client_rect) &&
                ClientToScreen(main_window, &client_origin)) {
            GetWindowTextW(main_window, title, ARRAYSIZE(title));
            printf("main-title=%ls window=%ld,%ld,%ld,%ld client-origin=%ld,%ld client=%ld,%ld\n",
                title, window_rect.left, window_rect.top, window_rect.right, window_rect.bottom,
                client_origin.x, client_origin.y, client_rect.right, client_rect.bottom);
        }
    }
    if (!strcmp(argv[2], "calc-arithmetic")) {
        /* IDs come from the verified guest image's NE accelerator resource. */
        const UINT commands[] = {49, 90, 50, 111}; /* 1 / 2 = */
        WCHAR text[128] = L"";
        const WCHAR *value = text;
        unsigned i;
        EnumChildWindows(main_window, find_calc_display, 0);
        if (!calc_display) { puts("FAIL no calculator display with initial zero"); return 20; }
        for (i = 0; i < ARRAYSIZE(commands); ++i) {
            if (!SendMessageTimeoutW(main_window, WM_COMMAND, commands[i], 0,
                    SMTO_ABORTIFHUNG, 10000, &reply)) return 21;
        }
        GetWindowTextW(calc_display, text, ARRAYSIZE(text));
        printf("calculator-result=%ls\n", text);
        while (*value == L' ') ++value;
        if (wcscmp(value, L"0.5") && wcscmp(value, L".5")) return 22;
        puts("PASS calculator 1/2 = 0.5");
    }
    for (round = 1; round <= rounds; ++round) {
        printf("round=%u\n", round);
        printf("command-state enabled=%d menu-state=%08x thread=%lu\n",
            IsWindowEnabled(main_window),
            GetMenuState(GetMenu(main_window), command_id, MF_BYCOMMAND),
            GetWindowThreadProcessId(main_window, NULL));
        start=GetTickCount64();
        if (!PostMessageW(main_window, WM_COMMAND, command_id, 0)) return 5;
        until=GetTickCount64()+30000;
        do { dialog_window=NULL; EnumDesktopWindows(desktop, find_dialog, 0); if(dialog_window) break; Sleep(25); }
        while (GetTickCount64()<until && WaitForSingleObject(process,0)==WAIT_TIMEOUT);
        if (!dialog_window) {
            puts("FAIL dialog not observed");
            report_process_state(process);
            EnumDesktopWindows(desktop, report_owned_window, 0);
            report_stalled_worker(owner_pid);
            return 6;
        }
        { WCHAR title[256] = L""; GetWindowTextW(dialog_window, title, ARRAYSIZE(title));
          printf("dialog hwnd=%p title=%ls\n", dialog_window, title); }
        visible=GetTickCount64(); printf("dialog-visible-ms=%llu\n",(unsigned long long)(visible-start));
        if (!PostMessageW(dialog_window,WM_CLOSE,0,0)) return 7;
        until=GetTickCount64()+30000;
        do { dialog_window=NULL; EnumDesktopWindows(desktop,find_dialog,0); if(!dialog_window) break; Sleep(25); }
        while(GetTickCount64()<until && WaitForSingleObject(process,0)==WAIT_TIMEOUT);
        if(dialog_window || WaitForSingleObject(process,0)!=WAIT_TIMEOUT ||
           !SendMessageTimeoutW(main_window,WM_NULL,0,0,SMTO_ABORTIFHUNG,10000,&reply)) {
            puts("FAIL close or parent recovery");
            report_process_state(process);
            report_stalled_worker(owner_pid);
            return 8;
        }
        printf("close-recovery-ms=%llu\n",(unsigned long long)(GetTickCount64()-visible));
        puts("PASS menu dialog and parent recovery");
    }
    if (!PostMessageW(main_window,WM_CLOSE,0,0)) return 9;
    CloseHandle(process); return 0;
}
