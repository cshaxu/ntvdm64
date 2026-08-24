#ifndef T198_S23_FASTREAD_ATTEMPT_LEDGER_H
#define T198_S23_FASTREAD_ATTEMPT_LEDGER_H

#include "adapter-bop/generic_ud_bridge.h"

#define T198_S23_FASTREAD_ATTEMPT_LEDGER_CAPACITY 8u

struct t198_s23_fastread_attempt_v1 {
    uint16_t cs;
    uint32_t eip, eax, ebp, ecx, edx, eflags;
    uint32_t accepted, disposition, gpr16_write_mask;
    uint64_t resume_rip;
    uint16_t ax;
    uint32_t eflags_write_mask, eflags_values;
};

struct t198_s23_fastread_attempt_ledger_v1 {
    uint32_t count;
    struct t198_s23_fastread_attempt_v1 entries[T198_S23_FASTREAD_ATTEMPT_LEDGER_CAPACITY];
};

void t198_s23_fastread_attempt_ledger_v1_reset(
    struct t198_s23_fastread_attempt_ledger_v1 *ledger);
void t198_s23_fastread_attempt_ledger_v1_record(
    struct t198_s23_fastread_attempt_ledger_v1 *ledger,
    const struct runtime_generic_ud_event_v1 *event,
    const struct runtime_generic_ud_outcome_v1 *outcome, int accepted);
int t198_s23_fastread_attempt_ledger_v1_get(
    const struct t198_s23_fastread_attempt_ledger_v1 *ledger, uint32_t index,
    struct t198_s23_fastread_attempt_v1 *entry);

#endif
