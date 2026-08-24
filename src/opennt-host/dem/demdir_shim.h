#ifndef BX_NTVDM_BOP_SHIM_DEMDIR_SHIM_H
#define BX_NTVDM_BOP_SHIM_DEMDIR_SHIM_H

/* Compatibility surface for directly imported OpenNT demdir.c.  It reuses
 * the demfile/demhndl mechanical ABI and adds only the original directory
 * owner's OEM host calls plus its packed CDS guest layout. */

#include "demfile_shim.h"

#pragma pack(push, 1)
typedef struct CURDIR_LIST {
    CHAR CurDir_Text[67];
    USHORT CurDir_Flags;
    USHORT CurDir_End;
} CDS, *PCDS;
#pragma pack(pop)

enum bx_ntvdm_demdir_service {
    BX_NTVDM_DEMDIR_CREATE = 0x04u,
    BX_NTVDM_DEMDIR_DELETE = 0x06u,
    BX_NTVDM_DEMDIR_QUERY_CURRENT = 0x13u,
    BX_NTVDM_DEMDIR_SET_CURRENT = 0x18u
};

BOOL CreateDirectoryOem(LPSTR, LPSECURITY_ATTRIBUTES);
BOOL RemoveDirectoryOem(LPSTR);
BOOL SetCurrentDirectoryOem(LPSTR);
BOOL SetEnvironmentVariableOem(LPSTR, LPSTR);
int bx_ntvdm_demdir_invoke(bx_ntvdm_demhndl_call *call);

#endif
