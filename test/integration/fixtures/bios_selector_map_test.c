#include "bios_selector_map.h"

int main(void)
{
    runtime_bios_owner owner = RUNTIME_BIOS_OWNER_NONE;

    if (!runtime_bios_selector_map_lookup(0x50u, &owner) ||
        owner != RUNTIME_BIOS_OWNER_OPENNT_MS_BOP_0) return 1;
    if (!runtime_bios_selector_map_lookup(0x54u, &owner) ||
        owner != RUNTIME_BIOS_OWNER_OPENNT_MS_BOP_4) return 2;
    if (runtime_bios_selector_map_lookup(0x4fu, &owner) ||
        owner != RUNTIME_BIOS_OWNER_NONE) return 3;
    if (runtime_bios_selector_map_lookup(0x51u, &owner) ||
        owner != RUNTIME_BIOS_OWNER_NONE) return 4;
    return runtime_bios_selector_map_lookup(0x50u, 0) ? 5 : 0;
}
