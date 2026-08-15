#include "bx_ntvdm_dem_open_observation_v1.h"

#include <string.h>

static uint32_t enabled;
static bx_ntvdm_dem_open_observation_v1 record;

void bx_ntvdm_dem_open_observation_v1_enable(uint32_t value)
{
    enabled = value == 1u;
    memset(&record, 0, sizeof(record));
}

int bx_ntvdm_dem_open_observation_v1_copy(bx_ntvdm_dem_open_observation_v1 *out)
{
    if (out == 0 || !enabled || !record.captured) return 0;
    *out = record;
    return 1;
}

void bx_ntvdm_dem_open_observation_v1_consider(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome, int accepted)
{
    if (!enabled || event == 0 || outcome == 0 || event->window_bytes < 4u ||
        event->window[0] != 0xc4u || event->window[1] != 0xc4u ||
        event->window[2] != 0x50u || event->window[3] != 0x12u) return;
    ++record.seen_open_count;
    if (record.seen_open_count != 2u) return;
    record.captured = 1u;
    record.cs = event->cs; record.ds = event->ds; record.eip = event->eip;
    record.eax = event->eax; record.ebx = event->ebx; record.esi = event->esi;
    record.accepted = accepted ? 1u : 0u; record.disposition = outcome->disposition;
    record.resume_rip = outcome->resume_rip;
    record.gpr16_write_mask = outcome->gpr16_write_mask;
    record.eflags_write_mask = outcome->eflags_write_mask;
    record.eflags_values = outcome->eflags_values;
    memcpy(record.gpr16_values, outcome->gpr16_values, sizeof(record.gpr16_values));
}
