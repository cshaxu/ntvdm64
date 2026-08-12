#include "bx_ntvdm_cmd_current_dir_service.h"

#define BX_NTVDM_CMD_CURRENT_DIR_ROOT_BYTES 4u

static int bx_ntvdm_cmd_current_dir_physical(uint16_t segment, uint16_t offset,
    uint64_t *address)
{
    uint64_t value = ((uint64_t)segment << 4) + offset;
    if (address == 0 || value > UINT64_C(0x100000) -
        BX_NTVDM_CMD_CURRENT_DIR_ROOT_BYTES) return 0;
    *address = value;
    return 1;
}

int bx_ntvdm_cmd_current_dir_service_v1_prepare(uint32_t available_mask,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD])
{
    uint8_t drive;
    uint64_t address;
    if (event == 0 || cpu_before == 0 || window == 0 || transaction == 0 ||
        payload == 0 || !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window) || event->vector != 6u ||
        cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        window->valid_bytes < 4u || window->bytes[0] != 0xc4u ||
        window->bytes[1] != 0xc4u || window->bytes[2] != 0x54u ||
        window->bytes[3] != 0x04u || event->fault_rip > UINT64_MAX - 4u)
        return 0;
    drive = (uint8_t)cpu_before->eax;
    if (drive >= 26u) return 0;
    bx_ntvdm_multi_write_transaction_v1_initialize(transaction, event, cpu_before);
    if ((available_mask & (UINT32_C(1) << drive)) == 0u) {
        return bx_ntvdm_cpu_result_v2_resume(&transaction->result,
                event->fault_rip + 4u) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&transaction->result.cpu_delta,
                0u, 0u) && bx_ntvdm_cpu_result_v2_set_cf(&transaction->result, 1) &&
            bx_ntvdm_cpu_result_v2_valid(&transaction->result);
    }
    if (!bx_ntvdm_cmd_current_dir_physical(cpu_before->ds,
            (uint16_t)cpu_before->esi, &address)) return 0;
    payload[0] = (uint8_t)('A' + drive);
    payload[1] = ':';
    payload[2] = '\\';
    payload[3] = '\0';
    if (!bx_ntvdm_multi_write_v1_add(&transaction->writes, address,
            BX_NTVDM_CMD_CURRENT_DIR_ROOT_BYTES, 0u) ||
        !bx_ntvdm_cpu_result_v2_resume(&transaction->result,
            event->fault_rip + 4u) ||
        !bx_ntvdm_cpu_result_v2_set_cf(&transaction->result, 0)) return 0;
    return bx_ntvdm_multi_write_transaction_v1_preflight(transaction,
        UINT64_C(0x100000), BX_NTVDM_CMD_CURRENT_DIR_ROOT_BYTES);
}
