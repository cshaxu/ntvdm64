#include "precomp.h"

extern ULONG Cpu40GdtShadowAddress;
extern ULONG Cpu40LdtShadowAddress;
extern ULONG IntelBase;
extern ULONG DpmiCpu40DescriptorShadowAddress(USHORT Selector);
extern BOOL DpmiCpu40GetDosxIdtDescriptor(PULONG Base, PUSHORT Limit);

static LDT_ENTRY gdt[32];

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
    return 0;
}
