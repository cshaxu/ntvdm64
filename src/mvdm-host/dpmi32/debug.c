/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    debug.c

Abstract:

    This module contains debugging routines for dpmi.

Revision History:

    Neil Sandlin (neilsa) Nov. 1, 95 - wrote it

--*/

#include "precomp.h"
#pragma hdrstop
#include "softpc.h"


#if DBG
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
