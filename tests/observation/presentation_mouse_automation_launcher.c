/*
 * No-argument GUI launcher for the bounded P6 presentation-input test.
 * It creates a hidden, real Console through the existing observer, which
 * owns all product input. This launcher does not send guest input or use a
 * private UI protocol; its only purpose is to make the observer startable
 * through the normal Windows application launcher for automated UI testing.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command,
                   int show)
{
    char line[2048];
    STARTUPINFOA startup;
    PROCESS_INFORMATION child;
    const char *root = "O:\\repos.hobby\\ntvdm64";
    const char *runtime = "O:\\ntvdm64";
    const char *report = "O:\\ntvdm64\\logs\\t404-mouse-auto-observer.txt";
    const char *mouse_report = "O:\\ntvdm64\\logs\\t404-mouse-auto-p6.txt";

    (void)instance;
    (void)previous;
    (void)command;
    (void)show;
    (void)DeleteFileA(report);
    (void)DeleteFileA(mouse_report);
    if (!SetEnvironmentVariableA("MVDM_CONSOLE_PRESENTATION_REPORT_PATH",
        mouse_report)) return 1;
    if (wsprintfA(line,
        "\"%s\\build\\M0-T404\\S3\\r006-presentation-observer\\console-startup-observer.exe\" "
        "\"%s\\ntvdm32.exe\" \"%s\" \"%s\" -f -o --command EDIT.COM "
        "--observe-presentation-toggle --observation-timeout-ms 30000",
        root, runtime, runtime, report) >= (int)sizeof(line)) return 2;
    ZeroMemory(&startup, sizeof(startup));
    ZeroMemory(&child, sizeof(child));
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESHOWWINDOW;
    startup.wShowWindow = SW_HIDE;
    if (!CreateProcessA(NULL, line, NULL, NULL, FALSE, CREATE_NEW_CONSOLE,
        NULL, runtime, &startup, &child)) return 3;
    CloseHandle(child.hThread);
    WaitForSingleObject(child.hProcess, INFINITE);
    CloseHandle(child.hProcess);
    return 0;
}
