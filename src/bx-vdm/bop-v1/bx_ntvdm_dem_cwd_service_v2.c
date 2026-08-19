#include "bx_ntvdm_dem_cwd_service_v2.h"

#include <string.h>

#define BX_NTVDM_DEM_CWD_APERTURE UINT64_C(0x100000)
#define BX_NTVDM_DEM_CDS_BYTES 71u
#define BX_NTVDM_DEM_CDS_TEXT_BYTES 67u
#define BX_NTVDM_DEM_CDS_END_OFFSET 69u
#define BX_NTVDM_DEM_CWD_PATH_BYTES 67u
#define BX_NTVDM_ERROR_INVALID_FUNCTION 1u
#define BX_NTVDM_ERROR_PATH_NOT_FOUND 3u
#define BX_NTVDM_ERROR_ACCESS_DENIED 5u
#define BX_NTVDM_ERROR_INVALID_DRIVE 15u

static int physical(uint16_t segment, uint16_t offset, uint64_t length,
    uint64_t *address)
{
    uint64_t value = (uint64_t)segment * 16u + offset;
    if (address == 0 || value > BX_NTVDM_DEM_CWD_APERTURE - length) return 0;
    *address = value;
    return 1;
}

static int service_valid(uint8_t service)
{
    return service == 0x13u || service == 0x18u || service == 0x1au;
}

static int finish(bx_ntvdm_multi_write_transaction_v1 *transaction,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu, int write_ax, uint16_t ax, int cf)
{
    bx_ntvdm_multi_write_transaction_v1_initialize(transaction, event, cpu);
    if (!bx_ntvdm_cpu_result_v2_resume(&transaction->result,
            event->fault_rip + 4u) ||
        !bx_ntvdm_cpu_result_v2_set_cf(&transaction->result, cf)) return 0;
    return !write_ax || bx_ntvdm_cpu_delta_v1_set_gpr16(
        &transaction->result.cpu_delta, 0u, ax);
}

static int path_from_guest(const uint8_t *bytes, uint64_t byte_count,
    uint8_t *drive_out, wchar_t relative[BX_NTVDM_DEM_CWD_CONTEXT_V1_MAX_RELATIVE])
{
    uint64_t index, output = 0u;
    if (bytes == 0 || drive_out == 0 || relative == 0 || byte_count < 4u ||
        ((bytes[0] < 'A' || bytes[0] > 'Z') &&
         (bytes[0] < 'a' || bytes[0] > 'z')) || bytes[1] != ':' ||
        bytes[2] != '\\') return 0;
    *drive_out = (uint8_t)((bytes[0] >= 'a' ? bytes[0] - 'a' : bytes[0] - 'A'));
    for (index = 3u; index < byte_count; ++index) {
        if (bytes[index] == 0u) {
            relative[output] = L'\0';
            return 1;
        }
        if (bytes[index] > 0x7fu || output + 1u >=
            BX_NTVDM_DEM_CWD_CONTEXT_V1_MAX_RELATIVE) return 0;
        relative[output++] = (wchar_t)bytes[index];
    }
    return 0;
}

static int full_path(uint8_t drive, const wchar_t *relative,
    uint8_t output[BX_NTVDM_DEM_CDS_TEXT_BYTES])
{
    uint32_t index = 0u;
    if (relative == 0 || drive >= 26u) return 0;
    memset(output, 0, BX_NTVDM_DEM_CDS_TEXT_BYTES);
    output[index++] = (uint8_t)('A' + drive);
    output[index++] = ':';
    output[index++] = '\\';
    while (*relative != L'\0') {
        if (*relative > 0x7fu || index + 1u >= BX_NTVDM_DEM_CDS_TEXT_BYTES)
            return 0;
        output[index++] = (uint8_t)*relative++;
    }
    return 1;
}

static int failure_for_context(int result, int default_drive,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu)
{
    if (result == BX_NTVDM_DEM_CWD_V1_READONLY)
        return finish(transaction, event, cpu, 1, BX_NTVDM_ERROR_ACCESS_DENIED, 1);
    if (result == BX_NTVDM_DEM_CWD_V1_NOT_FOUND)
        return finish(transaction, event, cpu, 1, BX_NTVDM_ERROR_PATH_NOT_FOUND, 1);
    return finish(transaction, event, cpu, default_drive,
        default_drive ? BX_NTVDM_ERROR_INVALID_FUNCTION : 0u, 1);
}

int bx_ntvdm_dem_cwd_service_v2_prepare(uint8_t service,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_guest_gather_read_action_v1 *action)
{
    bx_ntvdm_guest_range range;
    uint16_t offset;
    uint64_t length;
    if (!service_valid(service) || event == 0 || cpu == 0 || action == 0 ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu) || event->vector != 6u ||
        cpu->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL) return 0;
    offset = service == 0x18u ? (uint16_t)cpu->edx : (uint16_t)cpu->esi;
    length = service == 0x13u ? BX_NTVDM_DEM_CDS_BYTES :
        BX_NTVDM_DEM_CWD_PATH_BYTES;
    if (!physical(cpu->ds, offset, length, &range.address)) return 0;
    range.length = length;
    return bx_ntvdm_guest_gather_read_action_v1_need_read_resume(action,
        &range, 1u, event->fault_rip + 4u);
}

int bx_ntvdm_dem_cwd_service_v2_complete(
    bx_ntvdm_dem_cwd_context_v1 *context,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t service,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_guest_gather_read_action_v1 *action,
    const uint8_t *bytes, uint64_t byte_count,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD])
{
    uint16_t offset;
    uint64_t address, expected;
    uint8_t drive;
    wchar_t relative[BX_NTVDM_DEM_CWD_CONTEXT_V1_MAX_RELATIVE];
    int result;
    if (!service_valid(service) || context == 0 || event == 0 || cpu == 0 ||
        action == 0 || bytes == 0 || transaction == 0 || payload == 0 ||
        !bx_ntvdm_dem_cwd_context_v1_valid(context) ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu) ||
        !bx_ntvdm_guest_gather_read_action_v1_valid(action)) return 0;
    offset = service == 0x18u ? (uint16_t)cpu->edx : (uint16_t)cpu->esi;
    expected = service == 0x13u ? BX_NTVDM_DEM_CDS_BYTES :
        BX_NTVDM_DEM_CWD_PATH_BYTES;
    if (!physical(cpu->ds, offset, expected, &address) || action->range_count != 1u ||
        action->total_bytes != expected || action->ranges[0].address != address ||
        action->ranges[0].length != expected || byte_count != expected) return 0;
    if (service == 0x13u) {
        drive = (uint8_t)cpu->eax;
        if (drive >= 26u || (host_namespace != 0 &&
            !bx_ntvdm_host_namespace_v1_directory_exists(host_namespace, drive, L"")))
            return finish(transaction, event, cpu, 1, BX_NTVDM_ERROR_INVALID_DRIVE, 1);
        result = bx_ntvdm_dem_cwd_context_v1_reconcile_direct(context,
            host_namespace, drive);
        if (result != BX_NTVDM_DEM_CWD_V1_OK)
            return finish(transaction, event, cpu, 1, BX_NTVDM_ERROR_INVALID_DRIVE, 1);
        if (bx_ntvdm_dem_cwd_context_v1_query(context, drive, relative,
                BX_NTVDM_DEM_CWD_CONTEXT_V1_MAX_RELATIVE) != BX_NTVDM_DEM_CWD_V1_OK ||
            !full_path(drive, relative, payload))
            return finish(transaction, event, cpu, 1, BX_NTVDM_ERROR_INVALID_DRIVE, 1);
        if (!finish(transaction, event, cpu, 0, 0u, 0)) return 0;
        if (memcmp(bytes, payload, BX_NTVDM_DEM_CDS_TEXT_BYTES) == 0 &&
            bytes[BX_NTVDM_DEM_CDS_END_OFFSET] == 2u &&
            bytes[BX_NTVDM_DEM_CDS_END_OFFSET + 1u] == 0u) return 1;
        payload[BX_NTVDM_DEM_CDS_TEXT_BYTES] = 2u;
        payload[BX_NTVDM_DEM_CDS_TEXT_BYTES + 1u] = 0u;
        return bx_ntvdm_multi_write_v1_add(&transaction->writes, address,
            BX_NTVDM_DEM_CDS_TEXT_BYTES, 0u) &&
            bx_ntvdm_multi_write_v1_add(&transaction->writes,
                address + BX_NTVDM_DEM_CDS_END_OFFSET, 2u,
                BX_NTVDM_DEM_CDS_TEXT_BYTES) &&
            bx_ntvdm_multi_write_transaction_v1_preflight(transaction,
                BX_NTVDM_DEM_CWD_APERTURE,
                BX_NTVDM_DEM_CDS_TEXT_BYTES + 2u);
    }
    if (!path_from_guest(bytes, byte_count, &drive, relative))
        return finish(transaction, event, cpu, service == 0x1au,
            BX_NTVDM_ERROR_INVALID_FUNCTION, 1);
    if (service == 0x1au && drive != (uint8_t)cpu->edx)
        return finish(transaction, event, cpu, 1, BX_NTVDM_ERROR_INVALID_FUNCTION, 1);
    result = bx_ntvdm_dem_cwd_context_v1_set(context, host_namespace, drive,
        relative);
    if (result != BX_NTVDM_DEM_CWD_V1_OK)
        return failure_for_context(result, service == 0x1au, transaction, event, cpu);
    if (service == 0x1au) {
        result = bx_ntvdm_dem_cwd_context_v1_set_default_drive(context,
            host_namespace, drive);
        if (result != BX_NTVDM_DEM_CWD_V1_OK)
            return failure_for_context(result, 1, transaction, event, cpu);
    }
    return finish(transaction, event, cpu, 0, 0u, 0);
}
