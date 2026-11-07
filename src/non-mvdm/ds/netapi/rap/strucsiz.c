/*++

Copyright (c) 1991 Microsoft Corporation

Module Name:

    StrucSiz.c

Abstract:

    This module contains Remote Admin Protocol (RAP) routines.  These routines
    are shared between XactSrv and RpcXlate.

--*/
#line 37

#include <windef.h>
#include <lmcons.h>
#include <rap.h>
#line 46

DWORD
RapStructureSize (
    IN LPDESC Descriptor,
    IN RAP_TRANSMISSION_MODE TransmissionMode,
    IN BOOL Native
    )
#line 77
{
    DWORD structureSize;

    RapExamineDescriptor(
                Descriptor,
                NULL,
                &structureSize,
                NULL,
                NULL,
                NULL,
                NULL,  // don't need to know structure alignment
                TransmissionMode,
                Native
                );

    return structureSize;
} // RapStructureSize
#line 93
