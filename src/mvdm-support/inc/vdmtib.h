
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
/* DIVERGENCE(MVDM-SUPPORT-DIV-008): the original build used an absolute NT
 * source-tree include.  The identical selected VDM_TIB declaration is mirrored
 * by the product's OpenNT ABI include root, so retain the original name and
 * layout through normal include resolution rather than copying a declaration. */
#include <vdm.h>


extern VDM_TIB VdmTib;
#endif

