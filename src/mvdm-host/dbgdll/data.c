/*
 *  data.c - Main data of DBG DLL.
 *
 */
#include <precomp.h>
#pragma hdrstop


DWORD IntelMemoryBase;

DWORD VdmDbgTraceFlags = 0;

BOOL  fDebugged = FALSE;

VDMCONTEXT vcContext;
WORD EventFlags;                // flags defined in VDMDBG.H
VDMINTERNALINFO viInfo;
DWORD EventParams[4];
