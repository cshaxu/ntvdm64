#include "bx_ntvdm_bios_selector_map.h"

typedef struct bx_ntvdm_bios_selector_mapping_v1 {
    uint8_t selector;
    bx_ntvdm_bios_owner_v1 owner;
} bx_ntvdm_bios_selector_mapping_v1;

/* Recovered from OpenNT's historical BIOS[] initializer.  Deliberately do
 * not expand this into the historical 256-entry function-pointer table. */
static const bx_ntvdm_bios_selector_mapping_v1 bx_ntvdm_bios_selector_map[] = {
    { 0x50u, BX_NTVDM_BIOS_OWNER_OPENNT_MS_BOP_0 },
    { 0x54u, BX_NTVDM_BIOS_OWNER_OPENNT_MS_BOP_4 }
};

int bx_ntvdm_bios_selector_map_v1_lookup(uint8_t selector,
    bx_ntvdm_bios_owner_v1 *owner)
{
    unsigned int index;

    if (owner == 0) return 0;
    *owner = BX_NTVDM_BIOS_OWNER_NONE;
    for (index = 0u; index < sizeof(bx_ntvdm_bios_selector_map) /
        sizeof(bx_ntvdm_bios_selector_map[0]); ++index) {
        if (bx_ntvdm_bios_selector_map[index].selector == selector) {
            *owner = bx_ntvdm_bios_selector_map[index].owner;
            return 1;
        }
    }
    return 0;
}
