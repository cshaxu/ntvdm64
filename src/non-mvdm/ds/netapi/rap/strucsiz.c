/*++

Copyright (c) 1991 Microsoft Corporation

Module Name:

    StrucSiz.c

Abstract:

    This module contains Remote Admin Protocol (RAP) routines.  These routines
    are shared between XactSrv and RpcXlate.

--*/

#include <windef.h>
#include <lmcons.h>
#include <rap.h>

DWORD
RapStructureSize (
    IN LPDESC Descriptor,
    IN RAP_TRANSMISSION_MODE TransmissionMode,
    IN BOOL Native
    )
{
    DWORD structureSize;

    RapExamineDescriptor(
                Descriptor,
                NULL,
                &structureSize,
                NULL,
                NULL,
                NULL,
                NULL,
                TransmissionMode,
                Native
                );

    return structureSize;
}
