#include "bx_ntvdm_bios_selector_map.h"

int main(void)
{
    bx_ntvdm_bios_owner_v1 owner = BX_NTVDM_BIOS_OWNER_NONE;

    if (!bx_ntvdm_bios_selector_map_v1_lookup(0x50u, &owner) ||
        owner != BX_NTVDM_BIOS_OWNER_OPENNT_MS_BOP_0) return 1;
    if (!bx_ntvdm_bios_selector_map_v1_lookup(0x54u, &owner) ||
        owner != BX_NTVDM_BIOS_OWNER_OPENNT_MS_BOP_4) return 2;
    if (bx_ntvdm_bios_selector_map_v1_lookup(0x4fu, &owner) ||
        owner != BX_NTVDM_BIOS_OWNER_NONE) return 3;
    if (bx_ntvdm_bios_selector_map_v1_lookup(0x51u, &owner) ||
        owner != BX_NTVDM_BIOS_OWNER_NONE) return 4;
    return bx_ntvdm_bios_selector_map_v1_lookup(0x50u, 0) ? 5 : 0;
}
