/* Test-only UI-side driver. Never activates a window or reads the owner desktop. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

static BOOL visible(HANDLE output, PCWSTR marker)
{
    CONSOLE_SCREEN_BUFFER_INFO info; WCHAR row[1025]; DWORD read; SHORT y;
    if (!GetConsoleScreenBufferInfo(output, &info) || info.dwSize.X > 1024) return FALSE;
    for (y = info.srWindow.Top; y <= info.srWindow.Bottom; ++y) {
        COORD start = {0, y};
        if (!ReadConsoleOutputCharacterW(output, row, info.dwSize.X, start, &read)) return FALSE;
        row[read] = 0;
        if (wcsstr(row, marker)) return TRUE;
    }
    return FALSE;
}

static BOOL await_text(HANDLE output, PCWSTR marker)
{
    ULONGLONG deadline = GetTickCount64() + 10000;
    do { if (visible(output, marker)) return TRUE; Sleep(10); } while (GetTickCount64() < deadline);
    return FALSE;
}

static BOOL send_text(HANDLE input, PCWSTR text)
{
    for (; *text; ++text) {
        INPUT_RECORD records[2] = {0}; DWORD written;
        records[0].EventType = records[1].EventType = KEY_EVENT;
        records[0].Event.KeyEvent.bKeyDown = TRUE;
        records[0].Event.KeyEvent.wRepeatCount = records[1].Event.KeyEvent.wRepeatCount = 1;
        records[0].Event.KeyEvent.uChar.UnicodeChar = records[1].Event.KeyEvent.uChar.UnicodeChar = *text;
        records[0].Event.KeyEvent.wVirtualKeyCode = records[1].Event.KeyEvent.wVirtualKeyCode =
            *text == L'\r' ? VK_RETURN : LOBYTE(VkKeyScanW(*text));
        records[0].Event.KeyEvent.wVirtualScanCode = records[1].Event.KeyEvent.wVirtualScanCode =
            (WORD)MapVirtualKeyW(records[0].Event.KeyEvent.wVirtualKeyCode, MAPVK_VK_TO_VSC);
        if (!WriteConsoleInputW(input, records, 2, &written) || written != 2) return FALSE;
    }
    return TRUE;
}

static int selftest(PCWSTR report, BOOL missing)
{
    WCHAR image[MAX_PATH], command[2048]; char text[64] = {0}; DWORD bytes, result;
    STARTUPINFOW startup = {sizeof(startup)}; PROCESS_INFORMATION child = {0};
    if (!GetModuleFileNameW(NULL, image, ARRAYSIZE(image))) return 90;
    swprintf_s(command, ARRAYSIZE(command),
        L"\"%ls\" %lu %ls ACK-SELF W \"%ls\" \"chain\r\"",
        image, GetCurrentProcessId(), missing ? L"ABSENT-READY" : L"READY-SELF", report);
    if (!CreateProcessW(image, command, NULL, NULL, FALSE, CREATE_NO_WINDOW,
        NULL, NULL, &startup, &child)) return 91;
    CloseHandle(child.hThread);
    puts("READY-SELF"); fflush(stdout);
    if (missing) {
        BOOL rejected = WaitForSingleObject(child.hProcess, 12000) == WAIT_OBJECT_0 &&
            GetExitCodeProcess(child.hProcess, &result) && result == 1;
        CloseHandle(child.hProcess);
        if (!rejected) return 94;
        puts("PASS MISSING-PROMPT-REJECTED"); return 0;
    }
    if (!ReadFile(GetStdHandle(STD_INPUT_HANDLE), text, sizeof(text)-1, &bytes, NULL) ||
        bytes < 6 || memcmp(text, "chain\r", 6)) { CloseHandle(child.hProcess); return 92; }
    puts("ACK-SELF"); fflush(stdout);
    if (WaitForSingleObject(child.hProcess, 12000) != WAIT_OBJECT_0 ||
        !GetExitCodeProcess(child.hProcess, &result) || result) { CloseHandle(child.hProcess); return 93; }
    CloseHandle(child.hProcess); puts("PASS INPUT-DRIVER-SELFTEST"); return 0;
}

int wmain(int argc, WCHAR **argv)
{
    WCHAR desktop[128]; DWORD needed; HANDLE input, output; FILE *report = NULL;
    int result = 1; BOOL dos;
    if (!GetUserObjectInformationW(GetThreadDesktop(GetCurrentThreadId()),
        UOI_NAME, desktop, sizeof(desktop), &needed) ||
        wcsncmp(desktop, L"NTVDMConsoleTest-", 17)) return 80;
    if (argc == 3 && !wcscmp(argv[1], L"--selftest")) return selftest(argv[2], FALSE);
    if (argc == 3 && !wcscmp(argv[1], L"--selftest-missing")) return selftest(argv[2], TRUE);
    if (argc != 7) return 80;
    dos = !wcscmp(argv[4], L"D");
    FreeConsole();
    if (!AttachConsole(wcstoul(argv[1], NULL, 10))) return 81;
    input = CreateFileW(L"CONIN$", GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    output = CreateFileW(L"CONOUT$", GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (_wfopen_s(&report, argv[5], L"w")) goto done;
    if (!await_text(output, argv[2])) { fputs("FAIL no visible ready marker\n", report); goto done; }
    fprintf(report, "READY %ls\n", argv[2]); fflush(report);
    if (!send_text(input, dos ? L"z" : argv[6])) { fputs("FAIL input delivery\n", report); goto done; }
    if (!await_text(output, argv[3])) { fputs("FAIL no visible acknowledgement\n", report); goto done; }
    fprintf(report, "ACK %ls\nPASS visible-output-and-input\n", argv[3]); fflush(report);
    /* Second DOS PAUSE keeps the acknowledgement on-screen until observed. */
    if (dos && !send_text(input, L"z")) { fputs("FAIL final DOS release\n", report); goto done; }
    result = 0;
done:
    if (report) fclose(report);
    if (input != INVALID_HANDLE_VALUE) CloseHandle(input);
    if (output != INVALID_HANDLE_VALUE) CloseHandle(output);
    FreeConsole(); return result;
}
