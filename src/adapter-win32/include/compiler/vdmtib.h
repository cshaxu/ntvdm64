
/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    vdmtib.h

Abstract:

    This include file contains definitions, externs, and
    includes necessary to access 16 bit registers in
    the VdmTib

Author:

    Dave Hastings (daveh) 12-Oct-1993

Revision History:

--*/

#ifndef _VDMTIB_H_
#define _VDMTIB_H_


//
// Definition of the VdmTib
//
/* Research staging: the original absolute build-tree path resolves to the
 * exact OpenNT vdm.h copy recorded in this staging include directory. */
#include <vdm.h>


extern VDM_TIB VdmTib;
#endif
