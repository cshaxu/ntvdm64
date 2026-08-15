#ifndef NTDOS64_CONSOLE_CANCELLATION_V1_H
#define NTDOS64_CONSOLE_CANCELLATION_V1_H

/* CLI-private console control lifetime.  The event never crosses into the
 * engine: it is observed only by the CLI orchestration path. */

#include <windows.h>

int ntdos64_console_cancellation_v1_begin(HANDLE *event_out);
void ntdos64_console_cancellation_v1_end(void);

#endif
