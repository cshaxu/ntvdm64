#ifndef WIN32_PROCESS_COMMAND_LINE_H
#define WIN32_PROCESS_COMMAND_LINE_H
#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Return a borrowed pointer into a non-NULL native command line. */
PCWSTR win32_process_next_argument(PCWSTR command);
#ifdef __cplusplus
}
#endif
#endif
