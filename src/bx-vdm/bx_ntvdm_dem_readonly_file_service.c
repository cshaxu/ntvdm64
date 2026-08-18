#include "bx_ntvdm_dem_readonly_file_service.h"

#include <string.h>

#define BX_NTVDM_DEM_READONLY_FILE_APERTURE UINT64_C(0x100000)
#define BX_NTVDM_DEM_OPEN_PATH_BYTES 128u
#define BX_NTVDM_ERROR_ACCESS_DENIED 5u
#define BX_NTVDM_ERROR_INVALID_HANDLE 6u
#define BX_NTVDM_ERROR_INVALID_PARAMETER 87u

static int matches(const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu, const bx_ntvdm_instruction_window_v1 *window,
    uint8_t service)
{
    return event != 0 && cpu != 0 && window != 0 && bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu) && bx_ntvdm_instruction_window_v1_valid(window) &&
        event->vector == 6u && cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        window->valid_bytes >= 4u && window->bytes[0] == 0xc4u && window->bytes[1] == 0xc4u &&
        window->bytes[2] == 0x50u && window->bytes[3] == service && event->fault_rip <= UINT64_MAX - 4u;
}

static int physical(uint16_t segment, uint16_t offset, uint64_t length, uint64_t *address)
{
    uint64_t value = ((uint64_t)segment << 4) + offset;
    if (address == 0 || value >= BX_NTVDM_DEM_READONLY_FILE_APERTURE || length >
        BX_NTVDM_DEM_READONLY_FILE_APERTURE - value) return 0;
    *address = value;
    return 1;
}

static int result(const bx_ntvdm_exception_event_v1 *event, bx_ntvdm_cpu_result_v2 *value,
    uint16_t ax, uint16_t bx, uint16_t cx, uint16_t dx, uint16_t bp, int carry)
{
    return bx_ntvdm_cpu_result_v2_resume(value, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&value->cpu_delta, 0u, ax) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&value->cpu_delta, 1u, cx) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&value->cpu_delta, 2u, dx) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&value->cpu_delta, 3u, bx) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&value->cpu_delta, 5u, bp) &&
        bx_ntvdm_cpu_result_v2_set_cf(value, carry);
}

/* OpenNT demClose documents only CF on success and AX on failure.  Its
 * successful close path must not manufacture a register result. */
static int close_result(const bx_ntvdm_exception_event_v1 *event,
    bx_ntvdm_cpu_result_v2 *value, uint16_t ax, int carry)
{
    return bx_ntvdm_cpu_result_v2_resume(value, event->fault_rip + 4u) &&
        (!carry || bx_ntvdm_cpu_delta_v1_set_gpr16(&value->cpu_delta, 0u, ax)) &&
        bx_ntvdm_cpu_result_v2_set_cf(value, carry);
}

/* OpenNT demRead changes AX and CF only on both success and failure.  The
 * NTDOS caller keeps BP/BX across the BOP and later pops its saved frame. */
static int read_result(const bx_ntvdm_exception_event_v1 *event,
    bx_ntvdm_cpu_result_v2 *value, uint16_t ax, int carry)
{
    return bx_ntvdm_cpu_result_v2_resume(value, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&value->cpu_delta, 0u, ax) &&
        bx_ntvdm_cpu_result_v2_set_cf(value, carry);
}

/* OpenNT dem.h defines GETHANDLE(AX, BP): AX is the high word and BP the
 * low word.  NTDOS stores that exact layout in sf_NTHandle before FASTREAD. */
static uint32_t token(const bx_ntvdm_cpu_state_v1 *cpu) { return ((cpu->eax & 0xffffu) << 16) | (cpu->ebp & 0xffffu); }

int bx_ntvdm_dem_readonly_file_v1_prepare_open(const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu, const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_read_action_v1 *action)
{
    uint64_t address;
    if (action == 0 || !matches(event, cpu, window, 0x12u) ||
        !physical(cpu->ds, (uint16_t)cpu->esi, BX_NTVDM_DEM_OPEN_PATH_BYTES, &address)) return 0;
    return bx_ntvdm_guest_read_action_v1_need_read(action, address, BX_NTVDM_DEM_OPEN_PATH_BYTES);
}

int bx_ntvdm_dem_readonly_file_v1_complete_open(bx_ntvdm_readonly_namespace_v1 *space,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_guest_read_action_v1 *action, const uint8_t *path_bytes,
    uint64_t path_byte_count, bx_ntvdm_cpu_result_v2 *out)
{
    wchar_t path[64]; uint32_t index, handle; uint64_t size, expected;
    if (space == 0 || out == 0) return 0;
    /* `matches` needs the original window; completion instead revalidates the
       copied boundary/action, which binds it to the preceding prepare. */
    if (!bx_ntvdm_exception_event_v1_valid(event) || !bx_ntvdm_cpu_state_v1_valid(cpu) ||
        action == 0 || !bx_ntvdm_guest_read_action_v1_valid(action) ||
        action->disposition != BX_NTVDM_GUEST_READ_ACTION_V1_NEED_READ || path_bytes == 0 ||
        path_byte_count != BX_NTVDM_DEM_OPEN_PATH_BYTES || (cpu->eax & 0xffu) != 0u ||
        (cpu->ebx & 0xffu) != 0u || event->fault_rip > UINT64_MAX - 4u) return 0;
    if (!physical(cpu->ds, (uint16_t)cpu->esi, BX_NTVDM_DEM_OPEN_PATH_BYTES, &expected) ||
        action->guest_read.address != expected || action->guest_read.length != expected - expected + BX_NTVDM_DEM_OPEN_PATH_BYTES)
        return 0;
    for (index = 0u; index + 1u < path_byte_count && path_bytes[index] != 0u; ++index) {
        if (path_bytes[index] < 0x20u || path_bytes[index] > 0x7eu) return result(event, out, BX_NTVDM_ERROR_INVALID_PARAMETER, 0, 0, 0, 0, 1);
        path[index] = (wchar_t)path_bytes[index];
    }
    if (index == 0u || index + 1u >= path_byte_count || path_bytes[index] != 0u ||
        index < 4u || path[1] != L':' || path[2] != L'\\')
        return result(event, out, BX_NTVDM_ERROR_INVALID_PARAMETER, 0, 0, 0, 0, 1);
    path[index] = L'\0';
    if (path[0] < L'A' || path[0] > L'Z' || !bx_ntvdm_readonly_namespace_v1_open(space,
        (uint32_t)(path[0] - L'A'), path + 2u, &handle, &size))
        return result(event, out, BX_NTVDM_ERROR_ACCESS_DENIED, 0, 0, 0, 0, 1);
    return result(event, out, (uint16_t)(handle >> 16), (uint16_t)(size >> 16),
        (uint16_t)size, 0u, (uint16_t)handle, 0);
}

int bx_ntvdm_dem_readonly_file_v1_seek(bx_ntvdm_readonly_namespace_v1 *space,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, bx_ntvdm_cpu_result_v2 *out)
{
    int64_t displacement; uint64_t position, base, current_offset, current_size;
    if (space == 0 || out == 0 || !matches(event, cpu, window, 0x00u)) return 0;
    if ((cpu->ebx & 0xffu) > 2u) return 1; /* source caller rejects this; retain listener path */
    if (!bx_ntvdm_readonly_namespace_v1_position(space, token(cpu), &current_offset,
            &current_size))
        return result(event, out, BX_NTVDM_ERROR_INVALID_HANDLE, 0, 0, 0, 0, 1);
    displacement = (int32_t)(((cpu->ecx & 0xffffu) << 16) | (cpu->edx & 0xffffu));
    base = (cpu->ebx & 0xffu) == 0u ? 0u :
        ((cpu->ebx & 0xffu) == 1u ? current_offset : current_size);
    if ((displacement < 0 && (uint64_t)(-displacement) > base) ||
        (displacement > 0 && (uint64_t)displacement > UINT64_MAX - base))
        return result(event, out, BX_NTVDM_ERROR_INVALID_PARAMETER, 0, 0, 0, 0, 1);
    position = displacement < 0 ? base - (uint64_t)(-displacement) : base + (uint64_t)displacement;
    if (!bx_ntvdm_readonly_namespace_v1_seek(space, token(cpu), position))
        return result(event, out, BX_NTVDM_ERROR_INVALID_HANDLE, 0, 0, 0, 0, 1);
    return result(event, out, (uint16_t)position, 0, 0, (uint16_t)(position >> 16), 0, 0);
}

static int bx_ntvdm_dem_readonly_file_v1_read_service(bx_ntvdm_readonly_namespace_v1 *space,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, uint8_t service, uint8_t *payload, uint32_t capacity,
    bx_ntvdm_bulk_result_transaction_v1 *transaction,
    bx_ntvdm_cpu_result_v2 *direct_result)
{
    uint32_t count, requested = cpu == 0 ? 0u : (cpu->ecx & 0xffffu); uint64_t address;
    bx_ntvdm_cpu_result_v2 out;
    if (direct_result != 0) bx_ntvdm_cpu_result_v2_pass_through(direct_result);
    if (space == 0 || payload == 0 || transaction == 0 || direct_result == 0 ||
        !matches(event, cpu, window, service)) return 0;
    memset(transaction, 0, sizeof(*transaction));
    if (requested > capacity || !physical(cpu->ds, (uint16_t)cpu->edx, requested, &address))
        return read_result(event, direct_result, BX_NTVDM_ERROR_INVALID_PARAMETER, 1);
    if ((cpu->eflags & 0x40u) == 0u && !bx_ntvdm_readonly_namespace_v1_seek(space, token(cpu),
        ((uint64_t)(cpu->ebx & 0xffffu) << 16) | (cpu->esi & 0xffffu)))
        return read_result(event, direct_result, BX_NTVDM_ERROR_INVALID_HANDLE, 1);
    if (!bx_ntvdm_readonly_namespace_v1_read(space, token(cpu), payload, requested, &count))
        return read_result(event, direct_result, BX_NTVDM_ERROR_INVALID_HANDLE, 1);
    if (!read_result(event, &out, (uint16_t)count, 0)) return 0;
    bx_ntvdm_bulk_result_transaction_v1_initialize(transaction, event, cpu);
    if (count == 0u) address = 0u;
    if (!bx_ntvdm_bulk_result_transaction_v1_prepare(transaction, address, count, &out)) return 0;
    *direct_result = out;
    return 1;
}

int bx_ntvdm_dem_readonly_file_v1_read(bx_ntvdm_readonly_namespace_v1 *space,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, uint8_t *payload, uint32_t capacity,
    bx_ntvdm_bulk_result_transaction_v1 *transaction,
    bx_ntvdm_cpu_result_v2 *direct_result)
{
    return bx_ntvdm_dem_readonly_file_v1_read_service(space, event, cpu, window,
        0x16u, payload, capacity, transaction, direct_result);
}

/* Source-derived x86 fast-I/O composition.  The caller ABI is the same
 * copied handle/position/count/destination contract as demRead, but the
 * selector remains 50:42 so the original FastOrSlow continuation owns the
 * SFT update. */
int bx_ntvdm_dem_readonly_file_v1_fast_read(bx_ntvdm_readonly_namespace_v1 *space,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, uint8_t *payload,
    uint32_t capacity, bx_ntvdm_bulk_result_transaction_v1 *transaction,
    bx_ntvdm_cpu_result_v2 *direct_result)
{
    return bx_ntvdm_dem_readonly_file_v1_read_service(space, event, cpu, window,
        0x42u, payload, capacity, transaction, direct_result);
}

int bx_ntvdm_dem_readonly_file_v1_close(bx_ntvdm_readonly_namespace_v1 *space,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, bx_ntvdm_cpu_result_v2 *out)
{
    if (space == 0 || out == 0 || !matches(event, cpu, window, 0x02u)) return 0;
    if (token(cpu) == 0u) return close_result(event, out, 0u, 0);
    if (!bx_ntvdm_readonly_namespace_v1_close(space, token(cpu)))
        return close_result(event, out, BX_NTVDM_ERROR_INVALID_HANDLE, 1);
    return close_result(event, out, 0u, 0);
}

int bx_ntvdm_dem_readonly_file_v1_file_times(const bx_ntvdm_readonly_namespace_v1 *space,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, bx_ntvdm_cpu_result_v2 *out)
{
    uint16_t dos_time, dos_date;
    if (!space || !out || !matches(event, cpu, window, 0x08u)) return 0;
    if ((cpu->ebx & 0xffu) == 0u) {
        if (!bx_ntvdm_readonly_namespace_v1_file_times(space, token(cpu), &dos_time, &dos_date))
            return close_result(event, out, BX_NTVDM_ERROR_INVALID_HANDLE, 1);
        return bx_ntvdm_cpu_result_v2_resume(out, event->fault_rip + 4u) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&out->cpu_delta, 1u, dos_time) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&out->cpu_delta, 2u, dos_date) &&
            bx_ntvdm_cpu_result_v2_set_cf(out, 0);
    }
    if ((cpu->ebx & 0xffu) == 1u)
        return close_result(event, out, BX_NTVDM_ERROR_ACCESS_DENIED, 1);
    return close_result(event, out, BX_NTVDM_ERROR_INVALID_PARAMETER, 1);
}
