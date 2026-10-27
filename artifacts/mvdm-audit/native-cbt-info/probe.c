#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

static unsigned samples, info_failures, invalid_windows, class_failures, thread_failures;
static LRESULT CALLBACK observe_create(int code, WPARAM wp, LPARAM lp)
{
    if (code == HCBT_CREATEWND) {
        HWND window = (HWND)wp;
        WINDOWINFO info = { sizeof(info) };
        DWORD incoming = GetLastError(), error, process = 0;
        BOOL valid = IsWindow(window), have_info;
        ATOM atom = (ATOM)GetClassLongA(window, GCW_ATOM);
        DWORD thread = GetWindowThreadProcessId(window, &process);
        SetLastError(0);
        have_info = GetWindowInfo(window, &info);
        error = GetLastError();
        ++samples;
        if (!valid) ++invalid_windows;
        if (!atom) ++class_failures;
        if (!thread || process != GetCurrentProcessId()) ++thread_failures;
        if (!have_info) {
            ++info_failures;
            if (info_failures <= 8)
                printf("HCBT GetWindowInfo failure hwnd=%p error=%lu valid=%d atom=%04x thread=%lu\n",
                    window, error, valid, atom, thread);
        }
        SetLastError(incoming);
    }
    return CallNextHookEx(NULL, code, wp, lp);
}

static int child(const char *report)
{
    static const char *classes[] = { "BUTTON", "STATIC", "EDIT", "COMBOBOX", "LISTBOX", "SCROLLBAR" };
    unsigned round, index, created = 0;
    HHOOK hook;
    if (!freopen(report, "w", stdout)) return 2;
    setvbuf(stdout, NULL, _IONBF, 0);
    hook = SetWindowsHookExA(WH_CBT, observe_create, NULL, GetCurrentThreadId());
    if (!hook) { printf("hook failure=%lu\n", GetLastError()); return 3; }
    for (round = 0; round != 500; ++round) {
        HWND dialog = CreateWindowExA(0, "#32770", "Native creation metadata", WS_POPUP|WS_CAPTION|WS_SYSMENU,
            0, 0, 300, 200, NULL, NULL, GetModuleHandle(NULL), NULL);
        if (!dialog) { printf("dialog creation failure=%lu\n", GetLastError()); break; }
        ++created;
        for (index = 0; index != ARRAYSIZE(classes); ++index) {
            HWND control = CreateWindowExA(0, classes[index], "sample", WS_CHILD,
                0, 0, 80, 24, dialog, (HMENU)(ULONG_PTR)(100+index), GetModuleHandle(NULL), NULL);
            if (!control) { printf("control creation failure class=%s error=%lu\n", classes[index], GetLastError()); break; }
            ++created;
        }
        if (!DestroyWindow(dialog)) { puts("destroy failure"); break; }
        if (index != ARRAYSIZE(classes)) break;
    }
    UnhookWindowsHookEx(hook);
    printf("NATIVE_CBT_INFO rounds=%u created=%u samples=%u info_failures=%u invalid_windows=%u class_failures=%u thread_failures=%u\n",
        round, created, samples, info_failures, invalid_windows, class_failures, thread_failures);
    return round == 500 && samples >= 3500 && !info_failures && !invalid_windows && !class_failures && !thread_failures ? 0 : 4;
}

int main(int argc, char **argv)
{
    char self[MAX_PATH], name[96], command[3*MAX_PATH+96];
    STARTUPINFOA startup = { sizeof(startup) };
    PROCESS_INFORMATION process = {0};
    HDESK desktop;
    DWORD result = 5;
    if (argc == 3 && !strcmp(argv[1], "child")) return child(argv[2]);
    if (argc != 2 || !GetModuleFileNameA(NULL, self, sizeof(self))) return 2;
    sprintf_s(name, sizeof(name), "NTVDMCbtInfo-%lu", GetCurrentProcessId());
    desktop = CreateDesktopA(name, NULL, NULL, 0, GENERIC_ALL, NULL);
    if (!desktop) { printf("private desktop failure=%lu\n", GetLastError()); return 5; }
    sprintf_s(command, sizeof(command), "\"%s\" child \"%s\"", self, argv[1]);
    startup.lpDesktop = name; startup.dwFlags = STARTF_USESHOWWINDOW; startup.wShowWindow = SW_HIDE;
    if (CreateProcessA(self, command, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startup, &process)) {
        CloseHandle(process.hThread);
        if (WaitForSingleObject(process.hProcess, 60000) == WAIT_OBJECT_0)
            GetExitCodeProcess(process.hProcess, &result);
        else { TerminateProcess(process.hProcess, ERROR_PROCESS_ABORTED); WaitForSingleObject(process.hProcess, 5000); }
        CloseHandle(process.hProcess);
    }
    CloseDesktop(desktop);
    return (int)result;
}
