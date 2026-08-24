#ifndef NTDOS64_OPENNT_BOP_VDMREDIR_VRMSLOT_COMPAT_H
#define NTDOS64_OPENNT_BOP_VDMREDIR_VRMSLOT_COMPAT_H

/*
 * Compatibility include for directly retained VDMREDIR mailslot bodies.
 * OpenNT's vdmredir.h supplied these CCPU register macros from the NT4
 * product shell.  The source-shaped adapter-softpc facade retains the same
 * call form against a bounded copied call frame.
 */
#include <windows.h>

#include "adapter-softpc/opennt_ccpu_sas_facade.h"

#define SET_ERROR(err) { setAX((WORD)(err)); setCF(1); }

void VrPeekMailslot(void);

#endif
