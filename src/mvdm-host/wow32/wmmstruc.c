/*++
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, Microsoft Corporation
 *
 *  WMMSTRUC.C
 *
 *
 *  MultiMedia Structure copying functions (modelled after WSTRUC.C by jeffpar)
 *
 *  For input structures, there are GETxxxx16 macros;  for output structures
 *  there are PUTxxxx16 macros. Most or all of these macros will simply call
 *  the corresponding function below.
 *
 *
 *  WOW32 16-bit MultiMedia structure conversion support
 *
 *  History:
 *  Created  13-Feb-1992 by Mike Tricker (miketri)
 *  Changed  16-Jul-1992 by Mike Tricker (miketri) Sorted out the Caps structure copies
 *  Changed  08-Oct-1992 by StephenE Made the thunks safe on MIPS
 *
 *  Basically doing a GETVDMPTR of a null pointer is bad on MIPS, so is
 *  trying to get a pointer to zero bytes.  On Intel these GETXXX macros
 *  don't really do anything.
 *
--*/

#include "precomp.h"
#pragma hdrstop

#if 0
#error DIVERGENCE MVDM-HOST-DIV-495: original inactive legacy multimedia branch excluded; see mirror register.
#endif
