#ifndef RUNTIME_BIOS_SELECTOR_MAP_H
#define RUNTIME_BIOS_SELECTOR_MAP_H

#include <stdint.h>

/* This is a recovered selection record, not a recreated BIOS[] table.  It
 * carries no function pointer and cannot invoke an OpenNT owner. */
typedef enum runtime_bios_owner_v1 {
    RUNTIME_BIOS_OWNER_NONE = 0u,
    RUNTIME_BIOS_OWNER_OPENNT_MS_BOP_0 = 1u,
    RUNTIME_BIOS_OWNER_OPENNT_MS_BOP_4 = 2u
} runtime_bios_owner_v1;

#ifdef __cplusplus
extern "C" {
#endif

/* Returns nonzero only for an explicitly admitted historical selector. */
int runtime_bios_selector_map_v1_lookup(uint8_t selector,
    runtime_bios_owner_v1 *owner);

#ifdef __cplusplus
}
#endif

#endif
