#include "bios_selector_map.h"

typedef struct runtime_bios_selector_mapping {
    uint8_t selector;
    runtime_bios_owner owner;
} runtime_bios_selector_mapping;

/* Recovered from OpenNT's historical BIOS[] initializer.  Deliberately do
 * not expand this into the historical 256-entry function-pointer table. */
static const runtime_bios_selector_mapping runtime_bios_selector_map[] = {
    { 0x50u, RUNTIME_BIOS_OWNER_OPENNT_MS_BOP_0 },
    { 0x54u, RUNTIME_BIOS_OWNER_OPENNT_MS_BOP_4 }
};

int runtime_bios_selector_map_lookup(uint8_t selector,
    runtime_bios_owner *owner)
{
    unsigned int index;

    if (owner == 0) return 0;
    *owner = RUNTIME_BIOS_OWNER_NONE;
    for (index = 0u; index < sizeof(runtime_bios_selector_map) /
        sizeof(runtime_bios_selector_map[0]); ++index) {
        if (runtime_bios_selector_map[index].selector == selector) {
            *owner = runtime_bios_selector_map[index].owner;
            return 1;
        }
    }
    return 0;
}
