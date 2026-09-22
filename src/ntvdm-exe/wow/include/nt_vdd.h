/* ADAPTER-WOW-024: retain the byte-exact OpenNT ABI carrier and qualify only
 * the three worker-owned cdecl imports used by the /Gz WOW32 file provider.
 * These macros are declaration-local, not call-site aliases or wrappers. */
#ifndef WOW_WORKER_NT_VDD_ABI_H
#define WOW_WORKER_NT_VDD_ABI_H
#define VDDAllocateDosHandle __cdecl VDDAllocateDosHandle
#define VDDAssociateNtHandle __cdecl VDDAssociateNtHandle
#define VDDRetrieveNtHandle __cdecl VDDRetrieveNtHandle
#include "opennt-abi/source/public/sdk/inc/nt_vdd.h"
#undef VDDAllocateDosHandle
#undef VDDAssociateNtHandle
#undef VDDRetrieveNtHandle
#endif
