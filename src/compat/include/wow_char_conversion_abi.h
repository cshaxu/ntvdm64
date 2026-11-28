#ifndef _OPENNT_HOST_NTUSER_CHARTRANAPI_H_
#define _OPENNT_HOST_NTUSER_CHARTRANAPI_H_

/*
 * Standalone bindings for the selected user-mode slice of OpenNT USER's
 * chartran.c.  The original bodies retain conversion, allocation and error
 * semantics; this header names only the unavailable product services.
 */

#include <windows.h>

USHORT WINAPI OpenNtUserThreadCodePage(VOID);
PVOID WINAPI OpenNtUserRtlAllocMem(SIZE_T bytes);
VOID WINAPI OpenNtUserRtlFreeMem(PVOID value);

// XP user.h counted conversion constants; shared by conversion consumers.
#define USER_AWCONV_COUNTSTRING (-1)
#define USER_AWCONV_COUNTSTRINGSZ (-2)

// XP allows the standalone thread-code-page provider before its fallback.
#define THREAD_CODEPAGE() OpenNtUserThreadCodePage()
/* Native USER owns the same system-wide DBCS capability as other control bindings. */
#undef IS_DBCS_ENABLED
#define IS_DBCS_ENABLED() GetSystemMetrics(SM_DBCSENABLED)

#define UserRtlAllocMem OpenNtUserRtlAllocMem
#define UserRtlFreeMem OpenNtUserRtlFreeMem

#endif
