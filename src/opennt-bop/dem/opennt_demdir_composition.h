#ifndef NTDOS64_OPENNT_DEMDIR_COMPOSITION_H
#define NTDOS64_OPENNT_DEMDIR_COMPOSITION_H

/* Compatibility surface for directly imported OpenNT demdir.c.  It reuses
 * the demfile/demhndl mechanical ABI and adds only the original directory
 * owner's OEM host calls plus its packed CDS guest layout. */

#include "opennt-bop/dem/opennt_demfile_compat.h"
#include "adapter-win32/include/legacy-api/opennt_dem_oem_facade.h"

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

BOOL SetEnvironmentVariableOem(LPSTR, LPSTR);
int bx_ntvdm_demdir_invoke(bx_ntvdm_demhndl_call *call);

#endif
