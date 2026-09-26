/* Build the same finite child as CUI and GUI. Its completion witness is
 * written before exit 37; no product broker/worker or user desktop is needed. */
#include <windows.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    FILE *report;
    HWND window = NULL;
    if (argc != 2 || fopen_s(&report, argv[1], "w")) return 64;
#ifdef NATIVE_GUI_FIXTURE
    window = CreateWindowExA(0, "STATIC", "run16 lifecycle fixture", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 240, 100, NULL, NULL, GetModuleHandle(NULL), NULL);
    if (!window) { fclose(report); return 65; }
    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);
    fputs("gui-window-created=1\n", report);
#else
    DWORD written;
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE), alternate;
    CONSOLE_SCREEN_BUFFER_INFO info = {0};
    WCHAR observed[4] = {0};
    const WCHAR unicode[] = { 0x4e2d, 0x6587 };
    if (!WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), "NATIVE-CUI-37\r\n", 15, &written, NULL) ||
        written != 15) { fclose(report); return 66; }
    fputs("cui-output-written=1\n", report);
    /* Verify inherited native Console APIs, including a separate active
     * buffer. This fixture is launched only on the supervisor's private
     * desktop; no caller/user screen buffer is activated. */
    alternate = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    if (alternate == INVALID_HANDLE_VALUE) { fclose(report); return 68; }
    if (!SetConsoleActiveScreenBuffer(alternate) ||
        !WriteConsoleOutputCharacterW(alternate, unicode, 2, (COORD){0,0}, &written) || written != 2 ||
        /* Two full-width characters occupy four screen cells. */
        !ReadConsoleOutputCharacterW(alternate, observed, 4, (COORD){0,0}, &written) || written != 2 ||
        observed[0] != unicode[0] || observed[1] != unicode[1] ||
        !GetConsoleScreenBufferInfo(alternate, &info) ||
        !SetConsoleCursorPosition(alternate, (COORD){3,2}) ||
        !GetConsoleScreenBufferInfo(alternate, &info) ||
        info.dwCursorPosition.X != 3 || info.dwCursorPosition.Y != 2) {
        fprintf(report, "buffer-check-error=%lu count=%lu unicode=%04x,%04x size=%d,%d cursor=%d,%d\n",
            GetLastError(), written, observed[0], observed[1], info.dwSize.X, info.dwSize.Y,
            info.dwCursorPosition.X, info.dwCursorPosition.Y);
        SetConsoleActiveScreenBuffer(output);
        CloseHandle(alternate); fclose(report); return 68;
    }
    if (!SetConsoleActiveScreenBuffer(output)) {
        CloseHandle(alternate); fclose(report); return 68;
    }
    CloseHandle(alternate);
    fputs("cui-unicode-buffer-cursor-restored=1\n", report);
#endif
    Sleep(200);
    if (window && !DestroyWindow(window)) { fclose(report); return 67; }
    fputs("child-complete=37\n", report);
    fclose(report);
    return 37;
}
