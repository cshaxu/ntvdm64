#ifndef APP_OPENNT_DEMSRCH_FCB_COMPOSITION_H
#define APP_OPENNT_DEMSRCH_FCB_COMPOSITION_H

/* Project-owned source-body selection for imported OpenNT demsrch/demfcb. */
#include "opennt-bop/dem/opennt_demsrch_fcb_compat.h"
#include "adapter-win32/include/opennt-api/opennt_dem_search_facade.h"

int runtime_demsrch_fcb_invoke(runtime_demhndl_call *call);
void VDDTerminateUserHook(USHORT pdb);
void HostTerminatePDB(USHORT pdb);

#endif
