#ifndef T198_S121_DEM_LIFECYCLE_LEDGER_H
#define T198_S121_DEM_LIFECYCLE_LEDGER_H

#include "adapter-bop/bx_ntvdm_generic_ud_bridge.h"
#include "bx-vdm/bx_ntvdm_boot_namespace_composition_v1.h"

#define T198_S121_DEM_LIFECYCLE_LEDGER_CAPACITY 16u

struct t198_s121_dem_lifecycle_entry_v1 {
    uint8_t service, accepted;
    uint16_t cs;
    uint32_t eip, token, disposition, ax, bp;
    uint32_t namespace_open, namespace_file_index, namespace_generation;
    uint64_t namespace_offset;
};
struct t198_s121_dem_lifecycle_ledger_v1 {
    uint32_t count;
    struct t198_s121_dem_lifecycle_entry_v1 entries[T198_S121_DEM_LIFECYCLE_LEDGER_CAPACITY];
};
void t198_s121_dem_lifecycle_ledger_v1_reset(struct t198_s121_dem_lifecycle_ledger_v1 *ledger);
void t198_s121_dem_lifecycle_ledger_v1_record(struct t198_s121_dem_lifecycle_ledger_v1 *ledger,
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome, int accepted,
    const bx_ntvdm_boot_namespace_diagnostic_v1 *diagnostic);
int t198_s121_dem_lifecycle_ledger_v1_get(const struct t198_s121_dem_lifecycle_ledger_v1 *ledger,
    uint32_t index, struct t198_s121_dem_lifecycle_entry_v1 *entry);
#endif
