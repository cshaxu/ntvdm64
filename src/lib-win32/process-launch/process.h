#ifndef LIB_WIN32_PROCESS_LAUNCH_PROCESS_H
#define LIB_WIN32_PROCESS_LAUNCH_PROCESS_H
#include <windows.h>
/* Create a child with standard CreateProcessW parameters and caller-selected flags. The
 * caller owns all returned handles, admission, resume and failure rollback. */
BOOL win32_process_create(PCWSTR application, PWSTR command,
    LPSECURITY_ATTRIBUTES process_attributes, LPSECURITY_ATTRIBUTES thread_attributes,
    BOOL inherit, DWORD flags, LPVOID environment, PCWSTR directory,
    LPSTARTUPINFOW startup, LPPROCESS_INFORMATION child);
#endif
