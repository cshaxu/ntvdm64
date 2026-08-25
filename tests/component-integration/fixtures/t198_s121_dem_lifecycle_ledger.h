#ifndef T198_S121_DEM_LIFECYCLE_LEDGER_H
#define T198_S121_DEM_LIFECYCLE_LEDGER_H

#include "adapter-bop/generic_ud_bridge.h"
#include "bx-vdm/runtime_boot_namespace_composition.h"

#define T198_S121_DEM_LIFECYCLE_LEDGER_CAPACITY 16u

struct t198_s121_dem_lifecycle_entry {
    uint8_t service, accepted;
    uint16_t cs;
    uint32_t eip, token, disposition, ax, bp;
    uint32_t namespace_open, namespace_file_index, namespace_generation;
    uint64_t namespace_offset;
};
struct t198_s121_dem_lifecycle_ledger {
    uint32_t count;
    struct t198_s121_dem_lifecycle_entry entries[T198_S121_DEM_LIFECYCLE_LEDGER_CAPACITY];
};
void t198_s121_dem_lifecycle_ledger_reset(struct t198_s121_dem_lifecycle_ledger *ledger);
void t198_s121_dem_lifecycle_ledger_record(struct t198_s121_dem_lifecycle_ledger *ledger,
    const struct runtime_generic_ud_event *event,
    const struct runtime_generic_ud_outcome *outcome, int accepted,
    const runtime_boot_namespace_diagnostic *diagnostic);
int t198_s121_dem_lifecycle_ledger_get(const struct t198_s121_dem_lifecycle_ledger *ledger,
    uint32_t index, struct t198_s121_dem_lifecycle_entry *entry);
#endif
