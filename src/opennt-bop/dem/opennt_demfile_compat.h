#ifndef RUNTIME_BOP_SHIM_DEMFILE_SHIM_H
#define RUNTIME_BOP_SHIM_DEMFILE_SHIM_H

/* Compatibility boundary for the directly imported OpenNT files
 * src/opennt/base/mvdm/dos/dem/demfile.c and demlabel.c.  demfile shares the
 * historical CCPU/SAS register and checked guest-memory contract with
 * demhndl, so it reuses that neutral call record rather than growing a second
 * synthetic CPU interface. */

#include "opennt_dem_ccpu_sas_facade.h"
#include "adapter-win32/include/opennt-api/opennt_dem_file_oem_facade.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ACCESS_MASK 0x0007u
#define OPEN_FOR_READ 0x0000u
#define OPEN_FOR_WRITE 0x0001u
#define SHARING_MASK 0x0070u
#define SHARING_COMPAT 0x0000u
#define SHARING_DENY_BOTH 0x0010u
#define SHARING_DENY_WRITE 0x0020u
#define SHARING_DENY_READ 0x0030u
#define DOS_ATTR_MASK 0x0037u
#define ATTR_VOLUME_ID 0x0008u
#define DRIVEBYTE 0u
#define LABELOFF 3u
#define MSG_EAS 0u
#define IS_ASCII_PATH_SEPARATOR(value) ((value) == '\\' || (value) == '/')

typedef struct _DOSSFT { WORD SFT_Ref_Count; WORD SFT_Mode; WORD SFT_Attr;
    WORD SFT_Flags; ULONG SFT_Devptr; ULONG SFT_NTHandle; } DOSSFT, *PDOSSFT;
typedef struct _DOSSF { WORD SFCount; ULONG SFLink; DOSSFT SFTable[1]; } DOSSF, *PDOSSF;
typedef struct _DOSPDB { ULONG PDB_JFN_Pointer; WORD PDB_JFN_Length; } DOSPDB, *PDOSPDB;
extern PUSHORT pusCurrentPDB;

enum runtime_demfile_service {
    RUNTIME_DEMFILE_CHMOD = 0x01u,
    RUNTIME_DEMFILE_CREATE = 0x03u,
    RUNTIME_DEMFILE_DELETE = 0x05u,
    RUNTIME_DEMFILE_OPEN = 0x12u,
    RUNTIME_DEMFILE_RENAME = 0x17u,
    RUNTIME_DEMFILE_CREATE_NEW = 0x22u,
    RUNTIME_DEMFILE_CHECK_PATH = 0x44u
};

USHORT demCreateLabel(BYTE, LPSTR);
/* The original C89 translation unit calls these before their K&R-style
 * definitions.  MSVC's C11 mode needs declarations, not reordered bodies. */
VOID demCreateCommon(ULONG);
BOOL IsCdRomFile(PSTR);
HANDLE VDDRetrieveNtHandle(ULONG, SHORT, PVOID *, PBYTE *);

#pragma warning(disable:4057 4101 4131 4244 4305 4311 4312 4554 4996)

#endif
