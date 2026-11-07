/* Diagnostic-only production keyboard boundary; no input or device mutation. */
#define PendingKeyboardHistory original_pending_keyboard_history
#include "mvdm-host/softpc.new/base/keymouse/keyba.c"
#undef PendingKeyboardHistory
#include <stdio.h>
unsigned PendingKeyboardHistory(void)
{
    unsigned count = original_pending_keyboard_history();
    DWORD error = GetLastError();
    char line[160];
    DWORD written;
    HANDLE file = CreateFileA(TEST_KEYBOARD_TRACE_PATH, FILE_APPEND_DATA,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
    int length = sprintf_s(line, sizeof(line),
        "%lu %u %u %u %u %u %u\n", GetCurrentProcessId(), count,
        (unsigned)(unsigned char)output_contents, (unsigned)output_full,
        (unsigned)kbd_output_data_read, (unsigned)kbd_output_irq_acknowledged,
        nt_keyboard_history.output);
    if (file == INVALID_HANDLE_VALUE) ExitProcess(GetLastError());
    if (!WriteFile(file, line, length, &written, NULL) || written != (DWORD)length)
        ExitProcess(ERROR_WRITE_FAULT);
    CloseHandle(file);
    SetLastError(error);
    return count;
}
