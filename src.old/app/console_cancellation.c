#include "console_cancellation.h"

static HANDLE app_console_cancellation_event;

static BOOL WINAPI app_console_cancellation_handler(DWORD control_type)
{
    if (control_type != CTRL_C_EVENT && control_type != CTRL_BREAK_EVENT) return FALSE;
    if (app_console_cancellation_event != NULL) SetEvent(
        app_console_cancellation_event);
    return TRUE;
}

int app_console_cancellation_begin(HANDLE *event_out)
{
    HANDLE event;
    if (event_out == NULL || app_console_cancellation_event != NULL) return 0;
    event = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (event == NULL) return 0;
    app_console_cancellation_event = event;
    if (!SetConsoleCtrlHandler(app_console_cancellation_handler, TRUE)) {
        app_console_cancellation_event = NULL;
        CloseHandle(event);
        return 0;
    }
    *event_out = event;
    return 1;
}

void app_console_cancellation_end(void)
{
    HANDLE event = app_console_cancellation_event;
    if (event == NULL) return;
    SetConsoleCtrlHandler(app_console_cancellation_handler, FALSE);
    app_console_cancellation_event = NULL;
    CloseHandle(event);
}
