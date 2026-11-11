/* Independently authored host-only diagnostic. No product or guest code.
 * Test whether a pending cooked read retains its original echo buffer after
 * SetConsoleActiveScreenBuffer. This is NOT a product reproducer. */
#include <windows.h>
#include <stdio.h>

typedef struct read_context {
    HANDLE input, entered;
    DWORD count, error;
    WCHAR text[32];
} read_context;

static DWORD WINAPI read_line(void *parameter)
{
    read_context *context = parameter;
    SetEvent(context->entered);
    if (!ReadConsoleW(context->input, context->text, 32, &context->count, NULL))
        context->error = GetLastError();
    return context->error;
}

static BOOL key(HANDLE input, WCHAR character)
{
    INPUT_RECORD records[2] = {0};
    DWORD written;
    records[0].EventType = KEY_EVENT;
    records[0].Event.KeyEvent.bKeyDown = TRUE;
    records[0].Event.KeyEvent.wRepeatCount = 1;
    records[0].Event.KeyEvent.wVirtualKeyCode = character == L'\r' ? VK_RETURN : (WORD)VkKeyScanW(character);
    records[0].Event.KeyEvent.uChar.UnicodeChar = character;
    records[1] = records[0];
    records[1].Event.KeyEvent.bKeyDown = FALSE;
    return WriteConsoleInputW(input, records, 2, &written) && written == 2;
}

static WCHAR cell(HANDLE output)
{
    WCHAR value = 0;
    DWORD count;
    if (!ReadConsoleOutputCharacterW(output, &value, 1, (COORD){0,0}, &count) || count != 1)
        return 0xffff;
    return value;
}

int main(int argc, char **argv)
{
    HANDLE input, first, second, thread;
    read_context context = {0};
    FILE *report;
    DWORD mode, wait;
    WCHAR first_q, second_q, second_r;
    if ((argc != 2 && argc != 3) || fopen_s(&report, argv[1], "w")) return 64;
    FreeConsole();
    if (!AllocConsole()) return 65;
    input = CreateFileW(L"CONIN$", GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    first = CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    second = CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    if (input == INVALID_HANDLE_VALUE || first == INVALID_HANDLE_VALUE || second == INVALID_HANDLE_VALUE) return 66;
    if (!GetConsoleMode(input, &mode) || !SetConsoleMode(input, ENABLE_PROCESSED_INPUT|ENABLE_LINE_INPUT|ENABLE_ECHO_INPUT) ||
        !SetConsoleActiveScreenBuffer(first)) return 67;
    context.input = input;
    context.entered = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (argc == 3 && !SetConsoleActiveScreenBuffer(second)) return 77;
    thread = CreateThread(NULL, 0, read_line, &context, 0, NULL);
    if (!thread || WaitForSingleObject(context.entered, 10000) != WAIT_OBJECT_0) return 68;
    /* entered means the thread has reached the call site, not kernel admission.
     * Record this pacing limitation; the result below observes actual echo. */
    Sleep(250);
    if (!SetConsoleActiveScreenBuffer(second) || !key(input, L'q')) return 69;
    Sleep(250);
    first_q = cell(first); second_q = cell(second);
    if (!key(input, L'\r')) return 70;
    wait = WaitForSingleObject(thread, 10000);
    fprintf(report, "first-read wait=%lu error=%lu count=%lu old-cell=%u active-cell=%u\n",
        wait, context.error, context.count, first_q, second_q); fflush(report);
    if (wait != WAIT_OBJECT_0 || context.error) return 71;
    CloseHandle(thread); ResetEvent(context.entered);
    if (!SetConsoleCursorPosition(second, (COORD){0,0})) return 78;
    context.count = context.error = 0;
    thread = CreateThread(NULL, 0, read_line, &context, 0, NULL);
    if (!thread || WaitForSingleObject(context.entered, 10000) != WAIT_OBJECT_0) return 72;
    Sleep(250);
    if (!key(input, L'r')) return 73;
    Sleep(250); second_r = cell(second);
    if (!key(input, L'\r')) return 74;
    wait = WaitForSingleObject(thread, 10000);
    fprintf(report, "second-read wait=%lu error=%lu count=%lu active-cell=%u\n",
        wait, context.error, context.count, second_r); fclose(report);
    if (wait != WAIT_OBJECT_0) return 75;
    CloseHandle(thread); CloseHandle(context.entered);
    SetConsoleMode(input, mode);
    CloseHandle(input); CloseHandle(first); CloseHandle(second); FreeConsole();
    return context.error ? 76 : 0;
}
