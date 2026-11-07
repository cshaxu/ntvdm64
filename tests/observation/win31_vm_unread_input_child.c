/* Diagnostic native child: deliberately leave records in its own Console.
 * Unlike win31_vm_input_child, this proves handback, not physical input.
 * Usage: child report [z|sequence|repeat|function]. No case injects Enter. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <share.h>
#include <string.h>

static void pair(INPUT_RECORD *records, WORD vk, WORD scan, WCHAR character, WORD repeat)
{
    ZeroMemory(records, 2 * sizeof(*records));
    records[0].EventType = KEY_EVENT;
    records[0].Event.KeyEvent.bKeyDown = TRUE;
    records[0].Event.KeyEvent.wRepeatCount = repeat;
    records[0].Event.KeyEvent.wVirtualKeyCode = vk;
    records[0].Event.KeyEvent.wVirtualScanCode = scan;
    records[0].Event.KeyEvent.uChar.UnicodeChar = character;
    records[1] = records[0];
    records[1].Event.KeyEvent.bKeyDown = FALSE;
    records[1].Event.KeyEvent.wRepeatCount = 1;
    records[1].Event.KeyEvent.uChar.UnicodeChar = 0;
}

int main(int argc, char **argv)
{
    INPUT_RECORD records[40] = {0};
    HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
    FILE *report;
    const char *mode = argc >= 3 ? argv[2] : "z";
    char role[2] = {0}, round[2] = {0}, report_path[MAX_PATH];
    int vm = argc == 4 && !strcmp(argv[3], "--vm");
    DWORD before = 0, after = 0, written = 0, error = 0, count = 2, i;
    if (argc < 2 || argc > 4 || (argc == 4 && !vm)) return 64;
    if (vm && (GetEnvironmentVariableA("ROLE", role, sizeof(role)) != 1 ||
        GetEnvironmentVariableA("ROUND", round, sizeof(round)) != 1 ||
        (role[0] != 'A' && role[0] != 'B') || (round[0] != '0' && round[0] != '1')))
        return 64;
    pair(records, 'Z', 0x2c, L'z', 1);
    if (!strcmp(mode, "sequence")) {
        /* Twenty ordered characters exceed the normal 15-word BIOS ring. */
        count = 40;
        for (i = 0; i < 20; ++i)
            pair(records + 2 * i, i & 1 ? 'B' : 'A', i & 1 ? 0x30 : 0x1e,
                i & 1 ? L'b' : L'a', 1);
        if (vm && role[0] == 'B')
            for (i = 0; i < count; ++i) {
                records[i].Event.KeyEvent.wVirtualKeyCode += 2;
                records[i].Event.KeyEvent.wVirtualScanCode = i & 2 ? 0x20 : 0x2e;
                if (records[i].Event.KeyEvent.uChar.UnicodeChar)
                    records[i].Event.KeyEvent.uChar.UnicodeChar += 2;
            }
    } else if (!strcmp(mode, "repeat")) {
        pair(records, 'Z', 0x2c, L'z', 20);
    } else if (!strcmp(mode, "function")) {
        pair(records, VK_F1, 0x3b, 0, 1);
    } else if (!strcmp(mode, "extended")) {
        pair(records, VK_F11, 0x57, 0, 1);
    } else if (strcmp(mode, "z")) return 64;
    if (vm) {
        if (sprintf_s(report_path, sizeof(report_path), "%s-%c-%c.txt", argv[1], role[0], round[0]) < 0)
            return 64;
    } else if (strcpy_s(report_path, sizeof(report_path), argv[1])) return 64;
    if (!(report = _fsopen(report_path, "w", _SH_DENYWR))) return 64;
    if (!GetNumberOfConsoleInputEvents(input, &before)) error = GetLastError();
    if (!error) {
        if (!WriteConsoleInputW(input, records, count, &written)) error = GetLastError();
        else if (written != count) error = ERROR_WRITE_FAULT;
    }
    if (!error && !GetNumberOfConsoleInputEvents(input, &after)) error = GetLastError();
    fprintf(report, "pid=%lu synthetic-unread=%s before=%lu requested=%lu written=%lu after=%lu error=%lu\n",
        GetCurrentProcessId(), mode, before, count, written, after, error);
    for (i = 0; i < count; ++i) {
        const KEY_EVENT_RECORD *key = &records[i].Event.KeyEvent;
        fprintf(report, "record=%lu down=%d repeat=%u vk=%04x scan=%04x unicode=%04x\n",
            i, key->bKeyDown, key->wRepeatCount, key->wVirtualKeyCode,
            key->wVirtualScanCode, key->uChar.UnicodeChar);
    }
    if (fclose(report) && !error) error = ERROR_WRITE_FAULT;
    return error ? (int)error : vm ? 37 + role[0] - 'A' : 0;
}
