/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    dpmiselr.c

Abstract:

    This is the code for maintaining descriptor data for dpmi32.

Author:

    Dave Hart (davehart) 11-Apr-1993

Notes:

    
Revision History:

    09-Feb-1994 (daveh)
        Moved here from not386.c.
    31-Jul-1995 (neilsa) 
        Merged with x86 source
    12-Dec-1995 (neilsa)
        Wrote VdmAddDescriptorMapping(), GetDescriptorMapping

--*/

#include "precomp.h"
#pragma hdrstop
#include "softpc.h"
#include "malloc.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_termination.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"

#if DBG
USHORT CheckValue=0;
#endif

#ifndef i386
ULONG
GetDescriptorMapping(
    USHORT Sel,
    ULONG LdtBase
    );

typedef struct _DESC_MAPPING {
    USHORT Sel;
    USHORT SelCount;
    ULONG LdtBase;
    ULONG FlatBase;
    struct _DESC_MAPPING* pNext;
} DESC_MAPPING, *PDESC_MAPPING;

PDESC_MAPPING pDescMappingHead = NULL;

#endif // i386



//
// Imported functions from SoftPC world
//
extern VOID EnableEmulatorIretHooks(VOID);
extern VOID DisableEmulatorIretHooks(VOID);

BOOL
DpmiSetX86Descriptor(
    LDT_ENTRY *Descriptors,
    USHORT  registerAX,
    USHORT  registerCX
    );

NTSTATUS
DpmiSetWowLdtEntry(
    ULONG Selector,
    ULONG EntryLow,
    ULONG EntryHigh
    )
/*++

Routine Description:

    Implements the finite effect of the original x86 kernel
    NtSetLdtEntries call reached by WOW_x86 KRNL386 through its INT 2Ah
    fast path.  That kernel service changes the current process LDT only;
    it does not rewrite DOSX's gdtdsc source table or the DOSX GDT.

Arguments:

    Selector, EntryLow, EntryHigh - the three operands supplied by the
        original WOW_x86 INT 2Ah protocol.

Return Value:

    The source-shaped NTSTATUS returned to KRNL386 in EAX.

--*/
{
    ULONG EntryOffset;
    ULONG Base;
    ULONG Limit;
    PLDT_ENTRY Entry;

    /* This is the single-entry subset of base\ntos\ps\x86\psldt.c's
     * NtSetLdtEntries validation.  The worker has a finite LDT carrier, so
     * growing a process LDT and taking the kernel LDT mutex have no analogue;
     * the selector and descriptor contract does. */
    if (Selector & 0xffff0000UL)
        return STATUS_INVALID_LDT_DESCRIPTOR;
    EntryOffset = Selector & ~7UL;
    if (EntryOffset == 0)
        return STATUS_SUCCESS;
    if ((EntryOffset / sizeof(LDT_ENTRY)) >= LDT_SIZE ||
            Cpu40LdtShadowAddress == 0u)
        return STATUS_INVALID_LDT_DESCRIPTOR;

    Base = ((EntryLow & 0xffff0000UL) >> 16) +
        ((EntryHigh & 0x000000ffUL) << 16) +
        (EntryHigh & 0xff000000UL);
    Limit = (EntryLow & 0x0000ffffUL) + (EntryHigh & 0x000f0000UL);
    if (EntryHigh & 0x00800000UL)
        Limit = (Limit << 12) | 0x00000fffUL;

    /* The source kernel accepts a not-present descriptor without a base or
     * limit check.  For a present descriptor, preserve the x86 user-address
     * ceiling that DPMI's original descriptor publisher already enforces. */
    if ((EntryHigh & 0x00008000UL) &&
            (Base > 0x7ffeffffUL || Base > Base + Limit ||
             Base + Limit > 0x7ffeffffUL))
        return STATUS_INVALID_LDT_DESCRIPTOR;

    /* Only application descriptors at DPL 3 are admissible; conforming code
     * and system descriptors retain the original kernel rejection. */
    if (EntryHigh & 0x00007f00UL) {
        if (!(EntryHigh & 0x00001000UL) ||
                (EntryHigh & 0x00001c00UL) == 0x00001c00UL ||
                (EntryHigh & 0x00006000UL) != 0x00006000UL)
            return STATUS_INVALID_LDT_DESCRIPTOR;
    }

    /* CCPU binds its LDTR to the standalone carrier for OpenNT's current
     * process LDT.  Store the words verbatim: KRNL386 already performed the
     * source-side limit adjustment and the kernel service did not re-derive
     * or rewrite them. */
    Entry = (PLDT_ENTRY)(IntelBase + Cpu40LdtShadowAddress) +
        (EntryOffset / sizeof(LDT_ENTRY));
    *(PULONG)Entry = EntryLow;
    *(((PULONG)Entry) + 1) = EntryHigh;
    /* DIVERGENCE: DIV-310. WOW_x86 writes a guest-linear FlatAddressArray
     * before INT 2Ah. Recover the original non-i386 native mapping here;
     * that guest view cannot itself contain this process's native pointers. */
    FlatAddress[EntryOffset / sizeof(LDT_ENTRY)] =
        GetDescriptorMapping((USHORT)Selector, Base);
    if (FlatAddress[EntryOffset / sizeof(LDT_ENTRY)] == Base)
        FlatAddress[EntryOffset / sizeof(LDT_ENTRY)] += (ULONG)IntelBase;
    return STATUS_SUCCESS;
}

VOID
DpmiSetDescriptorEntry(
    VOID
    )
/*++

Routine Description:

    This function is stolen from i386\dpmi386.c and brain-damaged to
    only maintain the FlatAddress array.

Arguments:

    None

Return Value:

    None.

--*/

{
    LDT_ENTRY UNALIGNED *Descriptors;
    USHORT i;
    ULONG Base;
    ULONG Limit;
    USHORT registerCX;
    USHORT registerAX;

    registerAX = getAX();
    mvdm_softpc_report_dpmi_set_descriptor_entry(registerAX, getBX(), getCX());
    if (registerAX % 8){
        return;
    }

    Descriptors = (PLDT_ENTRY)Sim32GetVDMPointer(((getES() << 16) | getBX()),
        0,
        (UCHAR) (getMSW() & MSW_PE));

    mvdm_softpc_report_dpmi_set_descriptor(getES(), getBX(), registerAX,
        getCX(), (ULONG)Descriptors, (ULONG)(getMSW() & MSW_PE));


    registerCX =  getCX();
    for (i = 0; i < registerCX; i++) {

        // form Base and Limit values

        Base = Descriptors[i].BaseLow | (Descriptors[i].HighWord.Bytes.BaseMid << 16) |
               (Descriptors[i].HighWord.Bytes.BaseHi << 24);

        Limit = Descriptors[i].LimitLow | (Descriptors[i].HighWord.Bits.LimitHi << 16);
        Limit = (Limit << (12 * Descriptors[i].HighWord.Bits.Granularity)) +
            Descriptors[i].HighWord.Bits.Granularity * 0xFFF;

        //
        // Do NOT remove the following code.  There are several apps that
        // choose arbitrarily high limits for theirs selectors.  This works
        // under windows 3.1, but NT won't allow us to do that.
        // The following code fixes the limits for such selectors.
        // Note: if the base is > 0x7FFEFFFF, the selector set will fail
        //

        if ((Limit > 0x7FFEFFFF) || (Base + Limit > 0x7FFEFFFF)) {
            Limit = 0x7FFEFFFF - (Base + 0xFFF);
            if (!Descriptors[i].HighWord.Bits.Granularity) {
                Descriptors[i].LimitLow = (USHORT)(Limit & 0x0000FFFF);
                Descriptors[i].HighWord.Bits.LimitHi =
                    (Limit & 0x000f0000) >> 16;
            } else {
                Descriptors[i].LimitLow = (USHORT)((Limit >> 12) & 0xFFFF);
                Descriptors[i].HighWord.Bits.LimitHi =
                    ((Limit >> 12) & 0x000f0000) >> 16;
            }
        }

        if ((registerAX >> 3) != 0) {
#if defined(CPU_40_STYLE)
            /* DIVERGENCE: DIV-310. Publish the guest-linear value before
             * the original non-i386 native-address conversion below. */
            mvdm_softpc_wow_page_domain_update_flat_address((USHORT)
                (registerAX + i * sizeof(LDT_ENTRY)), Base);
#endif
#ifndef i386
            {
                ULONG BaseOrig = Base;
                Base = GetDescriptorMapping(registerAX+i*8, Base);
                if (BaseOrig == Base) {
                    Base += (ULONG)IntelBase;
                }
            }
#endif

            FlatAddress[(registerAX >> 3) + i] = Base;
#if DBG
            SelectorLimit[(registerAX >> 3) + i] = Limit;
#endif
#if defined(CPU_40_STYLE)
            ULONG Selector = registerAX + i * sizeof(LDT_ENTRY);
            ULONG GdtAddress = DpmiCpu40DescriptorShadowAddress(
                (USHORT)(Selector & ~4u));

            /* The original i386 provider publishes every 53:00 record to
             * the process LDT via NtSetLdtEntries/ProcessLdtInformation.
             * A TI=0 record is also the DOSX GDT image that protected-mode
             * selector loads consume.  CCPU needs distinct storage for
             * those two architectural domains, but the original publication
             * reaches both; TI=1 records update only their LDT domain. */
            if (Cpu40LdtShadowAddress != 0)
                ((PLDT_ENTRY)(IntelBase + Cpu40LdtShadowAddress))[
                    Selector >> 3] = Descriptors[i];
            if (!(Selector & 4u) && GdtAddress != 0)
                ((PLDT_ENTRY)(IntelBase + GdtAddress))[Selector >> 3] =
                    Descriptors[i];
            mvdm_softpc_report_cpu40_descriptor_publish((USHORT)Selector,
                *(PULONG)(Descriptors + i), *((PULONG)(Descriptors + i) + 1),
                GdtAddress != 0 ? *(PULONG)((PLDT_ENTRY)(IntelBase +
                    GdtAddress) + (Selector >> 3)) : 0,
                GdtAddress != 0 ? *((PULONG)((PLDT_ENTRY)(IntelBase +
                    GdtAddress) + (Selector >> 3)) + 1) : 0,
                Cpu40LdtShadowAddress != 0 ? *(PULONG)((PLDT_ENTRY)(IntelBase +
                    Cpu40LdtShadowAddress) + (Selector >> 3)) : 0,
                Cpu40LdtShadowAddress != 0 ? *((PULONG)((PLDT_ENTRY)(IntelBase +
                    Cpu40LdtShadowAddress) + (Selector >> 3)) + 1) : 0);
            /* WOW's original Get-LDT service returns this GDT-resident
             * alias.  Record only its already-published descriptor triplet
             * when explicitly requested, so the KRNL386 free-selector scan
             * can be compared to the descriptor it actually loads. */
            if (Selector == 0x0130u && Cpu40GdtShadowAddress != 0 &&
                    Cpu40LdtShadowAddress != 0) {
                PLDT_ENTRY Source = Descriptors + i;
                PLDT_ENTRY Gdt = (PLDT_ENTRY)(IntelBase +
                    Cpu40GdtShadowAddress) + (Selector >> 3);
                PLDT_ENTRY Ldt = (PLDT_ENTRY)(IntelBase +
                    Cpu40LdtShadowAddress) + (Selector >> 3);
                mvdm_softpc_report_dpmi_descriptor((USHORT)Selector,
                    *(PULONG)Source, *((PULONG)Source + 1), *(PULONG)Gdt,
                    *((PULONG)Gdt + 1), *(PULONG)Ldt, *((PULONG)Ldt + 1));
            }
#endif
        }
    }

#ifdef i386
    if (!DpmiSetX86Descriptor(Descriptors, registerAX, registerCX)) {
        return;
    }
#endif

    setAX(0);
    
#ifndef i386
#if DBG
    //
    // debugbug
    //
    if (registerAX == CheckValue) {
        force_yoda();    
    }
#endif
#endif // not i386
}


#ifndef i386

BOOL
VdmAddDescriptorMapping(
    USHORT SelectorStart,
    USHORT SelectorCount,
    ULONG LdtBase,
    ULONG Flat
    )
/*++

Routine Description:

    This function was added to support the DIB.DRV implementation on RISC.
    When an app uses DIB.DRV, then the situation arises where the Intel
    linear base address + the flat address of the start of the Intel address
    space does NOT equal the flat address of the memory. This happens when
    the VdmAddVirtualMemory() api is used to set up an additional layer of
    indirection for memory addressing in the emulator.

    But there is more to the story. When app wants to use CreateDIBSection
    via WinG we also need to map selectors, thus this routine should not
    depend upon DpmiSetDesctriptorEntry being called afterwards. Thus, we go
    and zap the flat address table with the new address. 

Arguments:

    SelectorStart, Count - range of selectors involved in the mapping
    LdtBase              - Intel base of start of range
    Flat                 - True flat address base to be used for these selectors

Return Value:

    This function returns TRUE on success, or FALSE for failure (out of mem)

--*/

{
    PDESC_MAPPING pdm;
    USHORT i;

    if ((pdm = (PDESC_MAPPING) malloc(sizeof (DESC_MAPPING))) == NULL)
                return FALSE;

    pdm->Sel         = SelectorStart &= ~7;
    pdm->SelCount    = SelectorCount;
    pdm->LdtBase     = LdtBase;
    pdm->FlatBase    = Flat;
    pdm->pNext       = pDescMappingHead;
    pDescMappingHead = pdm;

    // this code does what essentially desctribed in comment above
    for (i = 0; i < SelectorCount; ++i) {
        FlatAddress[(SelectorStart >> 3) + i] = Flat + 65536 * i;
    }

    return TRUE;
}

ULONG
GetDescriptorMapping(
    USHORT sel,
    ULONG LdtBase
    )
/*++

Routine Description:


Arguments:

    sel     - the selector for which the base should be returned
    LdtBase - the base for this selector as is set currently in the LDT

Return Value:

    The true flat address for the specified selector.

--*/
{
    PDESC_MAPPING pdm, pdmprev;
    ULONG Base = LdtBase;

    sel &= ~7;                      // and off lower 3 bits
    pdm = pDescMappingHead;

    while (pdm) {

        if ((sel >= pdm->Sel) && (sel < (pdm->Sel + pdm->SelCount*8))) {
            //
            // We found a mapping for this selector. Now check to see if
            // the ldt base still matches the base when the mapping was
            // created.
            //
            if (LdtBase == (pdm->LdtBase + 65536*((sel-pdm->Sel)/8))) {
                //
                // The mapping appears still valid. Return the remapped address
                //
                return (pdm->FlatBase + 65536*((sel-pdm->Sel)/8));

            } else {
                //
                // The ldt base doesn't match the mapping, so the mapping
                // must be obselete. Free the mapping here.
                //
                if (pdm == pDescMappingHead) {
                    //
                    // mapping is the first in the list
                    //
                    pDescMappingHead = pdm->pNext;

                } else {
                    pdmprev->pNext = pdm->pNext;
                }
                free(pdm);
            }

            break;
        }
        pdmprev = pdm;
        pdm = pdm->pNext;

    }

    return Base;
}

#endif i386
