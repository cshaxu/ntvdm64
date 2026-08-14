#include "bx_ntvdm_dem_computer_name_service_v1.h"

int bx_ntvdm_dem_computer_name_service_v1_prepare(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD])
{
    uint64_t address;
    if (!event || !cpu || !window || !transaction || !payload ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu) ||
        !bx_ntvdm_instruction_window_v1_valid(window) || event->vector != 6u ||
        cpu->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        event->fault_rip > UINT64_MAX - 4u || window->valid_bytes < 4u ||
        window->bytes[0] != 0xc4u || window->bytes[1] != 0xc4u ||
        window->bytes[2] != 0x50u || window->bytes[3] != 0x41u) return 0;
    address = ((uint64_t)cpu->ds << 4) + (uint16_t)cpu->edx;
    if (address >= UINT64_C(0x100000)) return 0;
    bx_ntvdm_multi_write_transaction_v1_initialize(transaction, event, cpu);
    payload[0] = 0u;
    return bx_ntvdm_multi_write_v1_add(&transaction->writes, address, 1u, 0u) &&
        bx_ntvdm_cpu_result_v2_resume(&transaction->result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&transaction->result.cpu_delta, 2u,
            (uint16_t)(cpu->ecx & 0x00ffu)) &&
        bx_ntvdm_multi_write_transaction_v1_preflight(transaction,
            UINT64_C(0x100000), transaction->writes.payload_bytes);
}
