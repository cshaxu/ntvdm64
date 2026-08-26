#include "frame_transaction.h"

#include "adapter-softpc/mechanical_action.h"

#include <string.h>

static int event_valid(const struct runtime_generic_ud_event *event)
{
    return event != NULL && event->magic == RUNTIME_GENERIC_UD_EVENT_MAGIC &&
        event->abi_version == RUNTIME_GENERIC_UD_EVENT_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->window_bytes <= RUNTIME_GENERIC_UD_WINDOW_BYTES;
}

static void copy_cpu(const struct runtime_generic_ud_event *source,
    runtime_cpu_state *target)
{
    runtime_cpu_state_initialize(target, source->execution_mode);
    target->eax = source->eax; target->ebx = source->ebx; target->ecx = source->ecx;
    target->edx = source->edx; target->esi = source->esi; target->edi = source->edi;
    target->ebp = source->ebp; target->esp = source->esp; target->eip = source->eip;
    target->eflags = source->eflags; target->cs = source->cs; target->ds = source->ds;
    target->es = source->es; target->ss = source->ss; target->fs = source->fs;
    target->gs = source->gs;
}

int runtime_bop_frame_transaction_valid(
    const runtime_bop_frame_transaction *transaction)
{
    return transaction != NULL &&
        transaction->magic == RUNTIME_BOP_FRAME_TRANSACTION_MAGIC &&
        transaction->abi_version == RUNTIME_BOP_FRAME_TRANSACTION_VERSION &&
        transaction->struct_bytes == sizeof(*transaction) &&
        transaction->reserved0 == 0u &&
        runtime_exception_event_valid(&transaction->boundary) &&
        runtime_cpu_state_valid(&transaction->cpu) &&
        runtime_cpu_result_valid(&transaction->result);
}

int runtime_bop_frame_transaction_begin(
    const struct runtime_generic_ud_event *event,
    runtime_bop_frame_transaction *transaction)
{
    if (!event_valid(event) || transaction == NULL) return 0;
    memset(transaction, 0, sizeof(*transaction));
    transaction->magic = RUNTIME_BOP_FRAME_TRANSACTION_MAGIC;
    transaction->abi_version = RUNTIME_BOP_FRAME_TRANSACTION_VERSION;
    transaction->struct_bytes = sizeof(*transaction);
    transaction->boundary.magic = RUNTIME_EXCEPTION_ABI_MAGIC;
    transaction->boundary.abi_version = RUNTIME_EXCEPTION_ABI_VERSION;
    transaction->boundary.struct_bytes = sizeof(transaction->boundary);
    transaction->boundary.kind = RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION;
    transaction->boundary.cpu_id = event->cpu_id;
    transaction->boundary.vector = event->vector;
    transaction->boundary.error_code = event->error_code;
    transaction->boundary.fault_rip = event->fault_rip;
    copy_cpu(event, &transaction->cpu);
    runtime_cpu_result_pass_through(&transaction->result);
    return runtime_bop_frame_transaction_valid(transaction);
}

int runtime_bop_frame_transaction_guest_read(void *context,
    uint32_t address, uint8_t *bytes, uint32_t byte_count)
{
    return runtime_bop_frame_transaction_valid(
        (const runtime_bop_frame_transaction *)context) &&
        runtime_machine_checked_ram_read(address, bytes, byte_count);
}

int runtime_bop_frame_transaction_guest_write(void *context,
    uint32_t address, const uint8_t *bytes, uint32_t byte_count)
{
    return runtime_bop_frame_transaction_valid(
        (const runtime_bop_frame_transaction *)context) &&
        runtime_machine_checked_ram_write(address, bytes, byte_count);
}

int runtime_bop_frame_transaction_complete(
    const runtime_bop_frame_transaction *transaction,
    struct runtime_generic_ud_outcome *outcome)
{
    const runtime_cpu_result *result;
    if (!runtime_bop_frame_transaction_valid(transaction) || outcome == NULL)
        return 0;
    result = &transaction->result;
    if (result->disposition == RUNTIME_CPU_RESULT_PASS_THROUGH) return 0;
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    outcome->disposition = result->disposition == RUNTIME_CPU_RESULT_RESUME ?
        RUNTIME_GENERIC_UD_RESUME :
        (result->disposition == RUNTIME_CPU_RESULT_PENDING ?
            RUNTIME_GENERIC_UD_PENDING : RUNTIME_GENERIC_UD_STOP);
    outcome->resume_rip = result->resume_rip;
    outcome->gpr16_write_mask = result->cpu_delta.gpr16_write_mask;
    memcpy(outcome->gpr16_values, result->cpu_delta.gpr16_values,
        sizeof(outcome->gpr16_values));
    outcome->segment_write_mask = result->cpu_delta.segment_write_mask;
    memcpy(outcome->segment_values, result->cpu_delta.segment_values,
        sizeof(outcome->segment_values));
    outcome->eflags_write_mask = result->eflags_write_mask;
    outcome->eflags_values = result->eflags_values;
    return 1;
}
