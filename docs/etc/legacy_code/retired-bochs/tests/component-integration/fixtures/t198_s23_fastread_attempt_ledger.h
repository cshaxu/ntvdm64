#ifndef T198_S23_FASTREAD_ATTEMPT_LEDGER_H
#define T198_S23_FASTREAD_ATTEMPT_LEDGER_H

#include "adapter-mvdm-host-in/generic_ud_bridge.h"

#define T198_S23_FASTREAD_ATTEMPT_LEDGER_CAPACITY 8u

struct t198_s23_fastread_attempt {
    uint16_t cs;
    uint32_t eip, eax, ebp, ecx, edx, eflags;
    uint32_t accepted, disposition, gpr16_write_mask;
    uint64_t resume_rip;
    uint16_t ax;
    uint32_t eflags_write_mask, eflags_values;
};

struct t198_s23_fastread_attempt_ledger {
    uint32_t count;
    struct t198_s23_fastread_attempt entries[T198_S23_FASTREAD_ATTEMPT_LEDGER_CAPACITY];
};

void t198_s23_fastread_attempt_ledger_reset(
    struct t198_s23_fastread_attempt_ledger *ledger);
void t198_s23_fastread_attempt_ledger_record(
    struct t198_s23_fastread_attempt_ledger *ledger,
    const struct runtime_generic_ud_event *event,
    const struct runtime_generic_ud_outcome *outcome, int accepted);
int t198_s23_fastread_attempt_ledger_get(
    const struct t198_s23_fastread_attempt_ledger *ledger, uint32_t index,
    struct t198_s23_fastread_attempt *entry);

#endif
