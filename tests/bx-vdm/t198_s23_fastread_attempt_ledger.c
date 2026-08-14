#include "t198_s23_fastread_attempt_ledger.h"

#include <string.h>

void t198_s23_fastread_attempt_ledger_v1_reset(
    struct t198_s23_fastread_attempt_ledger_v1 *ledger)
{
    if (ledger != 0) memset(ledger, 0, sizeof(*ledger));
}

void t198_s23_fastread_attempt_ledger_v1_record(
    struct t198_s23_fastread_attempt_ledger_v1 *ledger,
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome, int accepted)
{
    struct t198_s23_fastread_attempt_v1 *entry;
    if (ledger == 0 || event == 0 || ledger->count >= T198_S23_FASTREAD_ATTEMPT_LEDGER_CAPACITY ||
        event->window_bytes < 4u || event->window[0] != 0xc4u || event->window[1] != 0xc4u ||
        event->window[2] != 0x50u || event->window[3] != 0x42u) return;
    entry = &ledger->entries[ledger->count++];
    entry->cs = event->cs; entry->eip = event->eip; entry->eax = event->eax;
    entry->ebp = event->ebp; entry->ecx = event->ecx; entry->edx = event->edx;
    entry->eflags = event->eflags; entry->accepted = accepted ? 1u : 0u;
    if (accepted && outcome != 0) {
        entry->disposition = outcome->disposition; entry->resume_rip = outcome->resume_rip;
        entry->gpr16_write_mask = outcome->gpr16_write_mask;
        entry->ax = outcome->gpr16_values[0];
        entry->eflags_write_mask = outcome->eflags_write_mask;
        entry->eflags_values = outcome->eflags_values;
    }
}

int t198_s23_fastread_attempt_ledger_v1_get(
    const struct t198_s23_fastread_attempt_ledger_v1 *ledger, uint32_t index,
    struct t198_s23_fastread_attempt_v1 *entry)
{
    if (ledger == 0 || entry == 0 || index >= ledger->count) return 0;
    *entry = ledger->entries[index]; return 1;
}
