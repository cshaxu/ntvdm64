#ifndef BX_NTVDM_BOP_SHIM_DEMGSET_SHIM_H
#define BX_NTVDM_BOP_SHIM_DEMGSET_SHIM_H

/*
 * Compatibility boundary for the directly imported OpenNT file
 * src/opennt/base/mvdm/dos/dem/demgset.c.  This starts as an intentionally
 * narrow include/ABI bridge; source-build diagnostics identify each required
 * historical type or host seam before it is admitted here.
 */

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#include <windows.h>
#include <winternl.h>
#include <winioctl.h>
#include "../../../../opennt/local/compat/compiler/opennt_dem_ntdecl_compat.h"
#include "demdir_shim.h"
/* Direct source mirror: shared exact DASD/IOCTL layout and constants. */
#include "../opennt/dem/demdasd.h"

#define MSG_DEFAULT_DRIVE 4u
#define DOS_VOLUME_NAME_SIZE 11u
#define NT_VOLUME_NAME_SIZE 255u
#define FILESYS_NAME_SIZE 8u
/* demdasd.h above owns the exact historical DASD constants, including
 * DOS_DIR_ENTRY_LENGTH_SHIFT_COUNT and NON_REMOVABLE. */
#define ASSERT(expression) ((void)0)
#define STOREDWORD(member, value) ((member) = (ULONG)(value))

#pragma pack(push, 1)
typedef struct _VOLINFO {
    USHORT usInfoLevel;
    ULONG ulSerialNumber;
    CHAR VolumeID[DOS_VOLUME_NAME_SIZE];
    CHAR FileSystemType[FILESYS_NAME_SIZE];
} VOLINFO, *PVOLINFO;

typedef struct _DOSWOWDATA {
    DWORD lpCDSCount;
    DWORD lpCDSFixedTable;
    DWORD lpCDSBuffer;
    DWORD lpCurDrv;
    DWORD lpCurPDB;
    DWORD lpDrvErr;
    DWORD lpExterrLocus;
    DWORD lpSCS_ToSync;
    DWORD lpSftAddr;
} DOSWOWDATA, *PDOSWOWDATA;

#pragma pack(pop)

/* Exact OpenNT demdata.c type: this points to a 32-bit guest FAR address,
 * not to a host pointer. */
extern ULONG UNALIGNED *pulDTALocation;
typedef bx_ntvdm_demhndl_extended_error *PDEMEXTERR;

UINT GetDriveTypeOem(LPCSTR root);
BOOL GetComputerNameOem(LPSTR name, LPDWORD chars);
BOOL GetDiskFreeSpaceOem(LPCSTR root, LPDWORD sectors_per_cluster,
    LPDWORD bytes_per_sector, LPDWORD free_clusters,
    LPDWORD total_clusters);
PBDS demGetBDS(BYTE drive);
BOOL demGetBPB(PBDS bds);
BOOL demGetDiskFreeSpace(BYTE drive, WORD *bytes_per_sector,
    WORD *sectors_per_cluster, WORD *total_clusters, WORD *free_clusters);
BOOL GetMediaId(CHAR drive, PVOLINFO info);
typedef BOOL (*bx_ntvdm_demgset_clock_writer)(const SYSTEMTIME *time);
BOOL bx_ntvdm_demgset_set_local_time(const SYSTEMTIME *time);
void bx_ntvdm_demgset_set_clock_writer(bx_ntvdm_demgset_clock_writer writer);
int bx_ntvdm_demgset_invoke(bx_ntvdm_demhndl_call *call);
int bx_ntvdm_demgset_register_dta(USHORT ds, USHORT dta_offset,
    USHORT pdb_offset, USHORT error_offset, USHORT wow_offset);

#pragma warning(disable:4057 4101 4131 4244 4305 4311 4312 4554 4706 4996)

#endif
