#include "bx_ntvdm_bop_selector_probe.h"

int bx_ntvdm_bop_selector_probe_v1(
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_bios_owner_v1 *owner)
{
    if (owner == 0) return 0;
    *owner = BX_NTVDM_BIOS_OWNER_NONE;
    if (!bx_ntvdm_instruction_window_v1_valid(window) ||
        window->valid_bytes < 3u || window->bytes[0] != 0xc4u ||
        window->bytes[1] != 0xc4u) return 0;
    return bx_ntvdm_bios_selector_map_v1_lookup(window->bytes[2], owner);
}
