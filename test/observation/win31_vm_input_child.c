/* Isolated S6 native child receipt witness. No injected or synthetic input.
 * The diagnostic deadline bounds a missing guest producer, not product I/O. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <share.h>

static FILE *report;
static HANDLE finished;

static DWORD WINAPI deadline(void *unused)
{
    DWORD wait;
    (void)unused;
    wait = WaitForSingleObject(finished, 30000);
    if (wait == WAIT_OBJECT_0) return 0;
    fprintf(report, "deadline wait=%lu error=%lu\n", wait, GetLastError());
    fflush(report);
    ExitProcess(ERROR_TIMEOUT);
    return 0;
}

int main(int argc, char **argv)
{
    HANDLE input = GetStdHandle(STD_INPUT_HANDLE), timer;
    DWORD mode, count, error = 0;
    INPUT_RECORD record;
    if (argc != 2 || !(report = _fsopen(argv[1], "w", _SH_DENYWR))) return 64;
    fprintf(report, "started pid=%lu\n", GetCurrentProcessId());
    fflush(report);
    if (!GetConsoleMode(input, &mode) || !SetConsoleMode(input, 0)) {
        fprintf(report, "mode-error=%lu\n", GetLastError());
        fclose(report);
        return 65;
    }
    finished = CreateEventW(NULL, TRUE, FALSE, NULL);
    timer = finished ? CreateThread(NULL, 0, deadline, NULL, 0, NULL) : NULL;
    if (!timer) { SetConsoleMode(input, mode); fclose(report); return 66; }
    fputs("waiting-real-input\n", report);
    fflush(report);
    for (;;) {
        if (!ReadConsoleInputW(input, &record, 1, &count) || count != 1) {
            error = GetLastError();
            if (!error) error = ERROR_INVALID_DATA;
            fprintf(report, "read-error=%lu\n", error);
            break;
        }
        if (record.EventType == KEY_EVENT) {
            const KEY_EVENT_RECORD *key = &record.Event.KeyEvent;
            fprintf(report, "key down=%u repeat=%u vk=%04x scan=%04x char=%04x modifiers=%08lx\n",
                key->bKeyDown, key->wRepeatCount, key->wVirtualKeyCode,
                key->wVirtualScanCode, key->uChar.UnicodeChar, key->dwControlKeyState);
            fflush(report);
            if (!key->bKeyDown && key->wVirtualKeyCode == VK_RETURN) break;
        }
    }
    SetEvent(finished);
    WaitForSingleObject(timer, INFINITE);
    if (!SetConsoleMode(input, mode) && !error) error = GetLastError();
    fprintf(report, "complete error=%lu\n", error);
    CloseHandle(timer);
    CloseHandle(finished);
    fclose(report);
    return (int)error;
}
