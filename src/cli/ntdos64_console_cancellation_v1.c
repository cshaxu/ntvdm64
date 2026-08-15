#include "ntdos64_console_cancellation_v1.h"

static HANDLE ntdos64_console_cancellation_v1_event;

static BOOL WINAPI ntdos64_console_cancellation_v1_handler(DWORD control_type)
{
    if (control_type != CTRL_C_EVENT && control_type != CTRL_BREAK_EVENT) return FALSE;
    if (ntdos64_console_cancellation_v1_event != NULL) SetEvent(
        ntdos64_console_cancellation_v1_event);
    return TRUE;
}

int ntdos64_console_cancellation_v1_begin(HANDLE *event_out)
{
    HANDLE event;
    if (event_out == NULL || ntdos64_console_cancellation_v1_event != NULL) return 0;
    event = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (event == NULL) return 0;
    ntdos64_console_cancellation_v1_event = event;
    if (!SetConsoleCtrlHandler(ntdos64_console_cancellation_v1_handler, TRUE)) {
        ntdos64_console_cancellation_v1_event = NULL;
        CloseHandle(event);
        return 0;
    }
    *event_out = event;
    return 1;
}

void ntdos64_console_cancellation_v1_end(void)
{
    HANDLE event = ntdos64_console_cancellation_v1_event;
    if (event == NULL) return;
    SetConsoleCtrlHandler(ntdos64_console_cancellation_v1_handler, FALSE);
    ntdos64_console_cancellation_v1_event = NULL;
    CloseHandle(event);
}
