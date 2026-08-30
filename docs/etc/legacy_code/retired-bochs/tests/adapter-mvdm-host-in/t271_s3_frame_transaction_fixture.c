#include "adapter-mvdm-host-in/frame_transaction.h"

#include <string.h>

int main(void)
{
    struct runtime_generic_ud_event event;
    struct runtime_generic_ud_outcome outcome;
    runtime_bop_frame_transaction frame;
    memset(&event, 0, sizeof(event));
    event.magic = RUNTIME_GENERIC_UD_EVENT_MAGIC;
    event.abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    event.struct_bytes = sizeof(event);
    event.vector = 6u;
    event.cpu_id = 2u;
    event.error_code = 3u;
    event.execution_mode = RUNTIME_CPU_EXECUTION_REAL;
    event.fault_rip = 0x12340000u;
    event.eax = 0x11223344u;
    event.ecx = 0x55667788u;
    event.cs = 0x70u;
    event.ds = 0x71u;
    event.window_bytes = 4u;
    if (!runtime_bop_frame_transaction_begin(&event, &frame) ||
        frame.boundary.cpu_id != 2u || frame.boundary.error_code != 3u ||
        frame.boundary.fault_rip != event.fault_rip ||
        frame.cpu.eax != event.eax || frame.cpu.ecx != event.ecx ||
        frame.cpu.cs != event.cs || frame.cpu.ds != event.ds) return 1;
    if (runtime_bop_frame_transaction_complete(&frame, &outcome)) return 2;
    if (!runtime_cpu_result_resume(&frame.result, event.fault_rip + 4u) ||
        !runtime_cpu_delta_set_gpr16(&frame.result.cpu_delta, 0u, 0x55aau) ||
        !runtime_cpu_result_set_cf(&frame.result, 1) ||
        !runtime_bop_frame_transaction_complete(&frame, &outcome)) return 3;
    if (outcome.abi_version != RUNTIME_GENERIC_UD_EVENT_VERSION ||
        outcome.disposition != RUNTIME_GENERIC_UD_RESUME ||
        outcome.resume_rip != event.fault_rip + 4u ||
        outcome.gpr16_write_mask != 1u || outcome.gpr16_values[0] != 0x55aau ||
        outcome.eflags_write_mask != RUNTIME_CPU_RESULT_EFLAGS_CF ||
        outcome.eflags_values != RUNTIME_CPU_RESULT_EFLAGS_CF) return 4;
    event.vector = 13u;
    return runtime_bop_frame_transaction_begin(&event, &frame) ? 5 : 0;
}
