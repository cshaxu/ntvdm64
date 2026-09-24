#include "precomp.h"

extern ULONG Cpu40GdtShadowAddress;
extern ULONG Cpu40LdtShadowAddress;
extern ULONG IntelBase;
extern ULONG DpmiCpu40DescriptorShadowAddress(USHORT Selector);
extern BOOL DpmiCpu40GetDosxIdtDescriptor(PULONG Base, PUSHORT Limit);
extern NTSTATUS DpmiSetWowLdtEntry(ULONG Selector, ULONG EntryLow,
    ULONG EntryHigh);

static LDT_ENTRY gdt[32];
static LDT_ENTRY ldt[LDT_SIZE];

static void set_dosx_idt(ULONG base, ULONG limit)
{
    LDT_ENTRY *entry = &gdt[0x88u >> 3];

    RtlZeroMemory(entry, sizeof(*entry));
    entry->BaseLow = (USHORT)base;
    entry->HighWord.Bytes.BaseMid = (UCHAR)(base >> 16);
    entry->HighWord.Bytes.BaseHi = (UCHAR)(base >> 24);
    entry->LimitLow = (USHORT)limit;
    entry->HighWord.Bits.LimitHi = (UCHAR)(limit >> 16);
    entry->HighWord.Bits.Pres = 1u;
    entry->HighWord.Bits.Sys = 1u;
    entry->HighWord.Bits.Type = 2u;
}

int main(void)
{
    ULONG base;
    USHORT limit;
    ULONG ldt_base = 0x00123000u;
    ULONG ldt_low = (ldt_base << 16) | 0x0000ffffu;
    ULONG ldt_high = 0x0000f212u;
    ULONG ldt_index = 0x83b0u >> 3;

    /* DOSX's original 20h-byte VdmPmStackInfo prefix is shared with the
     * host VDM_DPMIINFO carrier; its extra reflector is outside that prefix.
     * Test the actual compiled ABI, not a duplicate fixture structure. */
    if (sizeof(VDM_DPMIINFO) != 36u ||
        FIELD_OFFSET(VDM_DPMIINFO, LockCount) != 0u ||
        FIELD_OFFSET(VDM_DPMIINFO, Flags) != 2u ||
        FIELD_OFFSET(VDM_DPMIINFO, SsSelector) != 4u ||
        FIELD_OFFSET(VDM_DPMIINFO, SaveSsSelector) != 6u ||
        FIELD_OFFSET(VDM_DPMIINFO, SaveEsp) != 8u ||
        FIELD_OFFSET(VDM_DPMIINFO, SaveEip) != 12u ||
        FIELD_OFFSET(VDM_DPMIINFO, DosxIntIret) != 16u ||
        FIELD_OFFSET(VDM_DPMIINFO, DosxIntIretD) != 20u ||
        FIELD_OFFSET(VDM_DPMIINFO, DosxFaultIret) != 24u ||
        FIELD_OFFSET(VDM_DPMIINFO, DosxFaultIretD) != 28u ||
        FIELD_OFFSET(VDM_DPMIINFO, DosxRmReflector) != 32u) return 12;

    Cpu40GdtShadowAddress = 0x00100000u;
    Cpu40LdtShadowAddress = 0x00200000u;

    if (DpmiCpu40DescriptorShadowAddress(0x0040u) !=
        Cpu40GdtShadowAddress) return 1;
    if (DpmiCpu40DescriptorShadowAddress(0x0004u) !=
        Cpu40LdtShadowAddress) return 2;
    if (DpmiCpu40DescriptorShadowAddress(0x000cu) !=
        Cpu40LdtShadowAddress) return 3;
    if (DpmiCpu40DescriptorShadowAddress(0u) != Cpu40GdtShadowAddress)
        return 4;

    /* A fresh worker has no published GDT and cannot inherit an IDTR. */
    IntelBase = (ULONG)gdt - 0x1000u;
    Cpu40GdtShadowAddress = 0u;
    if (DpmiCpu40GetDosxIdtDescriptor(&base, &limit)) return 5;

    set_dosx_idt(0x00123000u, 0x7ffu);
    if (DpmiCpu40GetDosxIdtDescriptor(&base, &limit)) return 6;

    /* Publish the final descriptor. It is the sole authority at 53:01. */
    Cpu40GdtShadowAddress = 0x1000u;
    if (!DpmiCpu40GetDosxIdtDescriptor(&base, &limit)) return 7;
    if (base != 0x00123000u || limit != 0x7ffu) return 8;

    /* A later descriptor publication replaces the source; no first-address
     * latch may survive it. */
    set_dosx_idt(0x00456000u, 0x7ffu);
    if (!DpmiCpu40GetDosxIdtDescriptor(&base, &limit)) return 9;
    if (base != 0x00456000u || limit != 0x7ffu) return 10;

    set_dosx_idt(0x00456000u, 0x7feu);
    if (DpmiCpu40GetDosxIdtDescriptor(&base, &limit)) return 11;

    /* WOW_x86 KRNL386 bypasses BOP 53:00 for one LDT entry: it supplies
     * selector 83B7h and two descriptor words to the kernel's INT 2Ah
     * NtSetLdtEntries service.  Exercise the standalone finite carrier with
     * that exact selector form, including its TI/RPL normalization. */
    RtlZeroMemory(ldt, sizeof(ldt));
    Cpu40LdtShadowAddress = 0x3000u;
    IntelBase = (ULONG)ldt - Cpu40LdtShadowAddress;
    if (!NT_SUCCESS(DpmiSetWowLdtEntry(0x83b7u, ldt_low, ldt_high)))
        return 13;
    if (*(PULONG)&ldt[ldt_index] != ldt_low ||
            *(((PULONG)&ldt[ldt_index]) + 1) != ldt_high)
        return 14;
    if (FlatAddress[ldt_index] != (ULONG)IntelBase + ldt_base)
        return 18;

    /* Selector zero means that the optional entry was not supplied. */
    ldt[0].LimitLow = 0x55aau;
    if (!NT_SUCCESS(DpmiSetWowLdtEntry(0u, 0u, 0u)) ||
            ldt[0].LimitLow != 0x55aau)
        return 15;

    /* Preserve the source NT kernel's precise invalid-descriptor outcome. */
    if (DpmiSetWowLdtEntry(0x00010000u, ldt_low, ldt_high) !=
            STATUS_INVALID_LDT_DESCRIPTOR)
        return 16;
    if (DpmiSetWowLdtEntry(0x83b7u, ldt_low, 0x0000e002u) !=
            STATUS_INVALID_LDT_DESCRIPTOR)
        return 17;
    if (FlatAddress[ldt_index] != (ULONG)IntelBase + ldt_base)
        return 19;
    /* Original DIB aliases must survive the fast publication path; changing
     * the descriptor base must retire that alias through the original owner. */
    if (!VdmAddDescriptorMapping(0x83b7u, 1u, ldt_base, 0xdead0000u) ||
            !NT_SUCCESS(DpmiSetWowLdtEntry(0x83b7u, ldt_low, ldt_high)) ||
            FlatAddress[ldt_index] != 0xdead0000u)
        return 20;
    if (!NT_SUCCESS(DpmiSetWowLdtEntry(0x83b7u, ldt_low, ldt_high + 1u)) ||
            FlatAddress[ldt_index] != (ULONG)IntelBase + ldt_base + 0x10000u)
        return 21;
    return 0;
}
