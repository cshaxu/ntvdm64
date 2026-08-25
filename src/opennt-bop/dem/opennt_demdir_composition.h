#ifndef APP_OPENNT_DEMDIR_COMPOSITION_H
#define APP_OPENNT_DEMDIR_COMPOSITION_H

/* Compatibility surface for directly imported OpenNT demdir.c.  It reuses
 * the demfile/demhndl mechanical ABI and adds only the original directory
 * owner's OEM host calls plus its packed CDS guest layout. */

#include "opennt-bop/dem/opennt_demfile_compat.h"
#include "adapter-win32/include/opennt-api/opennt_dem_oem_facade.h"

#pragma pack(push, 1)
typedef struct CURDIR_LIST {
    CHAR CurDir_Text[67];
    USHORT CurDir_Flags;
    USHORT CurDir_End;
} CDS, *PCDS;
#pragma pack(pop)

enum runtime_demdir_service {
    RUNTIME_DEMDIR_CREATE = 0x04u,
    RUNTIME_DEMDIR_DELETE = 0x06u,
    RUNTIME_DEMDIR_QUERY_CURRENT = 0x13u,
    RUNTIME_DEMDIR_SET_CURRENT = 0x18u
};

BOOL SetEnvironmentVariableOem(LPSTR, LPSTR);
int runtime_demdir_invoke(runtime_demhndl_call *call);

#endif
