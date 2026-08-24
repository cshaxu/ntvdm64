#ifndef NTDOS64_OPENNT_DEMSRCH_FCB_COMPOSITION_H
#define NTDOS64_OPENNT_DEMSRCH_FCB_COMPOSITION_H

/* Project-owned source-body selection for imported OpenNT demsrch/demfcb. */
#include "opennt-bop/dem/opennt_demsrch_fcb_compat.h"
#include "adapter-win32/include/legacy-api/opennt_dem_search_facade.h"

int bx_ntvdm_demsrch_fcb_invoke(bx_ntvdm_demhndl_call *call);
void VDDTerminateUserHook(USHORT pdb);
void HostTerminatePDB(USHORT pdb);

#endif
