#ifndef _OPENNT_HOST_NTUSER_CHARTRANAPI_H_
#define _OPENNT_HOST_NTUSER_CHARTRANAPI_H_

/*
 * Standalone bindings for the selected user-mode slice of OpenNT USER's
 * chartran.c.  The original bodies retain conversion, allocation and error
 * semantics; this header names only the unavailable product services.
 */

#include <nt.h>
#include "opennt-abi/source/public/sdk/inc/ntrtl_wow32.h"

USHORT WINAPI OpenNtUserNlsAnsiCodePage(VOID);
PVOID WINAPI OpenNtUserRtlAllocMem(SIZE_T bytes);
VOID WINAPI OpenNtUserRtlFreeMem(PVOID value);

#define NlsAnsiCodePage OpenNtUserNlsAnsiCodePage()
#define UserRtlAllocMem OpenNtUserRtlAllocMem
#define UserRtlFreeMem OpenNtUserRtlFreeMem

#endif
