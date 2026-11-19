/* WOW file-provider ABI, ADAPTER-WOW-024.
 * OpenNT 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c,
 * public/sdk/inc/nt_vdd.h:209-212: original names and parameter/return types.
 * The worker exports cdecl; the WOW translation unit defaults to /Gz.
 * Only that caller selects this declaration directory. */
#ifndef WOW_WORKER_NT_VDD_ABI_H
#define WOW_WORKER_NT_VDD_ABI_H
#include <windows.h>
SHORT __cdecl VDDAllocateDosHandle(ULONG pPDB, PVOID* ppSFT, PVOID* ppJFT);
VOID __cdecl VDDAssociateNtHandle(PVOID pSFT, HANDLE h32File, WORD wAccess);
HANDLE __cdecl VDDRetrieveNtHandle(ULONG pPDB, SHORT hFile, PVOID* ppSFT, PVOID* ppJFT);
#endif
