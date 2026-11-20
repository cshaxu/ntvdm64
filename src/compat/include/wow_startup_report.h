#ifndef COMPAT_WOW_STARTUP_REPORT_H
#define COMPAT_WOW_STARTUP_REPORT_H

#include <windows.h>

/* Internal worker/DLL ABI. Bind is requested only after local USER commits
 * the accepted startup receipt for this exact shared task. */
DWORD WINAPI NtvdmReportWowStartup(ULONG request, USHORT task, BOOL bind);
BOOL WINAPI wow_current_separate_launch(void);

#endif
