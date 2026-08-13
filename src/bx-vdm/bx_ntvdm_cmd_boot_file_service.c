#include "bx_ntvdm_cmd_boot_file_service.h"

#include <string.h>

#define BX_NTVDM_CMD_BOOT_FILE_APERTURE UINT64_C(0x100000)
#define BX_NTVDM_CMD_BOOT_FILE_MAX_PATH 64u

static int bx_ntvdm_cmd_boot_file_physical(uint16_t segment, uint16_t offset,
    uint64_t *address)
{
    uint64_t value = ((uint64_t)segment << 4) + offset;
    if (address == 0 || value > BX_NTVDM_CMD_BOOT_FILE_APERTURE -
        BX_NTVDM_CMD_BOOT_FILE_MAX_PATH) return 0;
    *address = value;
    return 1;
}

int bx_ntvdm_cmd_boot_file_service_v1_prepare_diagnostic(
    const bx_ntvdm_readonly_namespace_v1 *namespace_value,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD],
    bx_ntvdm_cmd_boot_file_prepare_diagnostic_v1 *diagnostic)
{
    uint32_t file_index;
    const wchar_t *guest_path;
    uint64_t address;
    size_t index, bytes;

    if (diagnostic == 0 || namespace_value == 0 || event == 0 || cpu_before == 0 ||
        window == 0 || transaction == 0 || payload == 0) return 0;
    *diagnostic = BX_NTVDM_CMD_BOOT_FILE_PREPARE_DIAGNOSTIC_V1_ARGUMENT;
    if (!bx_ntvdm_exception_event_v1_valid(event)) {
        *diagnostic = BX_NTVDM_CMD_BOOT_FILE_PREPARE_DIAGNOSTIC_V1_EVENT;
        return 0;
    }
    if (!bx_ntvdm_cpu_state_v1_valid(cpu_before)) {
        *diagnostic = BX_NTVDM_CMD_BOOT_FILE_PREPARE_DIAGNOSTIC_V1_CPU;
        return 0;
    }
    if (!bx_ntvdm_instruction_window_v1_valid(window)) {
        *diagnostic = BX_NTVDM_CMD_BOOT_FILE_PREPARE_DIAGNOSTIC_V1_WINDOW;
        return 0;
    }
    if (event->vector != 6u || cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        window->valid_bytes < 4u || window->bytes[0] != 0xc4u ||
        window->bytes[1] != 0xc4u || window->bytes[2] != 0x54u ||
        (window->bytes[3] != 0x0cu && window->bytes[3] != 0x0du) ||
        event->fault_rip > UINT64_MAX - 4u) {
        *diagnostic = BX_NTVDM_CMD_BOOT_FILE_PREPARE_DIAGNOSTIC_V1_CONTRACT;
        return 0;
    }
    if (namespace_value->drive_index >= 26u || !bx_ntvdm_cmd_boot_file_physical(
            cpu_before->ds, (uint16_t)cpu_before->edx, &address)) {
        *diagnostic = BX_NTVDM_CMD_BOOT_FILE_PREPARE_DIAGNOSTIC_V1_NAMESPACE;
        return 0;
    }

    file_index = window->bytes[3] == 0x0cu ? 1u : 2u;
    guest_path = namespace_value->files[file_index].path;
    if (guest_path[0] != L'\\') {
        *diagnostic = BX_NTVDM_CMD_BOOT_FILE_PREPARE_DIAGNOSTIC_V1_PATH;
        return 0;
    }
    bytes = wcslen(guest_path) + 3u; /* drive, colon, path, NUL */
    if (bytes > BX_NTVDM_CMD_BOOT_FILE_MAX_PATH) {
        *diagnostic = BX_NTVDM_CMD_BOOT_FILE_PREPARE_DIAGNOSTIC_V1_PATH;
        return 0;
    }
    payload[0] = (uint8_t)('A' + namespace_value->drive_index);
    payload[1] = ':';
    for (index = 0u; index <= wcslen(guest_path); ++index) {
        if (guest_path[index] > 0x7fu) {
            *diagnostic = BX_NTVDM_CMD_BOOT_FILE_PREPARE_DIAGNOSTIC_V1_PATH;
            return 0;
        }
        payload[index + 2u] = (uint8_t)guest_path[index];
    }
    bx_ntvdm_multi_write_transaction_v1_initialize(transaction, event, cpu_before);
    if (!bx_ntvdm_multi_write_v1_add(&transaction->writes, address, bytes, 0u) ||
        !bx_ntvdm_cpu_result_v2_resume(&transaction->result, event->fault_rip + 4u) ||
        !bx_ntvdm_multi_write_transaction_v1_preflight(transaction,
            BX_NTVDM_CMD_BOOT_FILE_APERTURE, bytes)) {
        *diagnostic = BX_NTVDM_CMD_BOOT_FILE_PREPARE_DIAGNOSTIC_V1_TRANSACTION;
        return 0;
    }
    *diagnostic = BX_NTVDM_CMD_BOOT_FILE_PREPARE_DIAGNOSTIC_V1_ACCEPTED;
    return 1;
}

int bx_ntvdm_cmd_boot_file_service_v1_prepare(
    const bx_ntvdm_readonly_namespace_v1 *namespace_value,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD])
{
    bx_ntvdm_cmd_boot_file_prepare_diagnostic_v1 diagnostic;
    return bx_ntvdm_cmd_boot_file_service_v1_prepare_diagnostic(namespace_value, event,
        cpu_before, window, transaction, payload, &diagnostic);
}
