#ifndef APP_OPENNT_DEMFILE_COMPOSITION_H
#define APP_OPENNT_DEMFILE_COMPOSITION_H

/* Project-owned composition for imported OpenNT demfile.c/demlabel.c.  The
 * body retains OpenNT service semantics; this surface selects that body and
 * binds it to the existing scoped CCPU/SAS-compatible call record. */
#include "opennt-bop/dem/opennt_demfile_compat.h"
#include "adapter-win32/include/legacy-api/opennt_dem_file_oem_facade.h"

int runtime_demfile_invoke(runtime_demhndl_call *call);
BOOL runtime_demfile_publish_handle(HANDLE file);
void *Sim32GetVDMPointer(ULONG address, ULONG bytes, int protect);

#endif
