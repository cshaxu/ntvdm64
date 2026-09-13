#include "precomp.h"

extern ULONG Cpu40GdtShadowAddress;
extern ULONG Cpu40LdtShadowAddress;
extern ULONG DpmiCpu40DescriptorShadowAddress(USHORT Selector);

int main(void)
{
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
    return 0;
}
