#ifndef APP_CONSOLE_CANCELLATION_H
#define APP_CONSOLE_CANCELLATION_H

/* CLI-private console control lifetime.  The event never crosses into the
 * engine: it is observed only by the CLI orchestration path. */

#include <windows.h>

int app_console_cancellation_begin(HANDLE *event_out);
void app_console_cancellation_end(void);

#endif
