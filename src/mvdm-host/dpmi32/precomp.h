/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    precomp.h

Abstract:

    This is the private include file for the 32 bit dpmi and protected mode
    support

--*/

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <vdm.h>
#include <dpmi.h>
#include <vint.h>
#include <mvdm.h>
#include <dpmi32.h>
#include <dbgsvc.h>
#include <nt_vdd.h>

#ifdef CCPU
#define getMODE() (((getMSW() & MSW_PE) && !(getEFLAGS() & EFLAGS_VM_MASK)) ? VDM_PM : VDM_V86)
#ifndef DECLARE_LocalVdmContext
#define DECLARE_LocalVdmContext
#endif
extern ULONG Cpu40GdtShadowAddress, Cpu40LdtShadowAddress;
VOID DpmiCpu40BindDescriptorTables(VOID);
BOOL DpmiCpu40GetDosxIdtDescriptor(PULONG base, PUSHORT limit);
#endif
