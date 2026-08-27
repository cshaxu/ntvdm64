#include "mvdm_protected_frame_transaction.h"

#include <string.h>

#include "adapter-bochs/machine_facade.h"

struct protected_frame_transaction {
    int active;
    struct machine_facade_protected_frame expected;
    struct machine_facade_protected_frame candidate;
};

static _Thread_local struct protected_frame_transaction current_transaction;

int mvdm_protected_frame_transaction_begin(void)
{
    if (current_transaction.active ||
        machine_facade_copy_protected_frame(&current_transaction.expected) !=
            MACHINE_FACADE_PROTECTED_FRAME_OK) {
        return 0;
    }
    current_transaction.candidate = current_transaction.expected;
    current_transaction.active = 1;
    return 1;
}

int mvdm_protected_frame_transaction_commit(void)
{
    int result;
    if (!current_transaction.active) return 0;
    result = machine_facade_commit_same_cpl_protected_frame(
        &current_transaction.expected, &current_transaction.candidate) ==
        MACHINE_FACADE_PROTECTED_FRAME_OK;
    memset(&current_transaction, 0, sizeof(current_transaction));
    return result;
}

void mvdm_protected_frame_transaction_abort(void)
{
    memset(&current_transaction, 0, sizeof(current_transaction));
}

int mvdm_protected_frame_transaction_active(void)
{
    return current_transaction.active;
}

uint32_t mvdm_protected_frame_transaction_eip(void) { return current_transaction.candidate.eip; }
uint32_t mvdm_protected_frame_transaction_esp(void) { return current_transaction.candidate.esp; }
uint16_t mvdm_protected_frame_transaction_cs(void) { return current_transaction.candidate.cs; }
uint16_t mvdm_protected_frame_transaction_ss(void) { return current_transaction.candidate.ss; }
uint32_t mvdm_protected_frame_transaction_eflags(void) { return current_transaction.candidate.eflags; }
void mvdm_protected_frame_transaction_set_eip(uint32_t value) { current_transaction.candidate.eip = value; }
void mvdm_protected_frame_transaction_set_esp(uint32_t value) { current_transaction.candidate.esp = value; }
void mvdm_protected_frame_transaction_set_cs(uint16_t value) { current_transaction.candidate.cs = value; }
void mvdm_protected_frame_transaction_set_ss(uint16_t value) { current_transaction.candidate.ss = value; }
void mvdm_protected_frame_transaction_set_eflags(uint32_t value) { current_transaction.candidate.eflags = value; }
