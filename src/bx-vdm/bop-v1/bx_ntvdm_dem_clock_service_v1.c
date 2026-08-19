#include "bx_ntvdm_dem_clock_service_v1.h"

#include <windows.h>
#include <time.h>

static int selected(const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window)
{
    return event && cpu && window && bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu) && bx_ntvdm_instruction_window_v1_valid(window) &&
        event->vector == 6u && cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        window->valid_bytes >= 4u && window->bytes[0] == 0xc4u &&
        window->bytes[1] == 0xc4u && window->bytes[2] == 0x50u &&
        event->fault_rip <= UINT64_MAX - 4u;
}

static int set_result(const bx_ntvdm_exception_event_v1 *event,
    bx_ntvdm_cpu_result_v2 *result, uint16_t ax)
{
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax);
}

static int set_date_or_time(const bx_ntvdm_dem_profile_consumer_v1 *profile,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    uint8_t service, bx_ntvdm_cpu_result_v2 *result)
{
    SYSTEMTIME local;
    uint32_t policy;
    if (!profile || !bx_ntvdm_dem_profile_consumer_v1_resolve(profile,
            BX_NTVDM_MUTATION_CLASS_V1_HOST_GLOBAL, &policy)) return 0;
    if (policy == BX_NTVDM_MUTATION_POLICY_V1_REJECT_READONLY)
        return set_result(event, result, (uint16_t)((cpu->eax & 0xff00u) | 0xffu));
    if (policy != BX_NTVDM_MUTATION_POLICY_V1_DIRECT_HOST) return 0;
    GetLocalTime(&local);
    if (service == 0x19u) {
        local.wYear = (WORD)(cpu->ecx & 0xffffu);
        local.wMonth = (WORD)((cpu->edx >> 8u) & 0xffu);
        local.wDay = (WORD)(cpu->edx & 0xffu);
    } else {
        local.wHour = (WORD)((cpu->ecx >> 8u) & 0xffu);
        local.wMinute = (WORD)(cpu->ecx & 0xffu);
        local.wSecond = (WORD)((cpu->edx >> 8u) & 0xffu);
        local.wMilliseconds = (WORD)((cpu->edx & 0xffu) * 10u);
    }
    /* demgset.c treats ERROR_PRIVILEGE_NOT_HELD as AL=0, even when Windows
     * denied the actual clock update. */
    return set_result(event, result, (uint16_t)((cpu->eax & 0xff00u) |
        ((SetLocalTime(&local) || GetLastError() == ERROR_PRIVILEGE_NOT_HELD)
            ? 0u : 0xffu)));
}

int bx_ntvdm_dem_clock_service_v1_dispatch_with_profile(
    const bx_ntvdm_dem_profile_consumer_v1 *profile,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    time_t now;
    struct tm local;
    uint16_t ax, cx, dx;

    if (!result || !selected(event, cpu, window)) return 0;
    if (window->bytes[3] == 0x19u || window->bytes[3] == 0x1cu)
        return set_date_or_time(profile, event, cpu, window->bytes[3], result);
    if (window->bytes[3] != 0x14u && window->bytes[3] != 0x15u) return 0;
    now = time(0);
    if (now == (time_t)-1 || localtime_s(&local, &now) != 0) return 0;
    if (!bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u)) return 0;
    if (window->bytes[3] == 0x14u) {
        ax = (uint16_t)((cpu->eax & 0xff00u) | (uint8_t)local.tm_wday);
        cx = (uint16_t)(local.tm_year + 1900);
        dx = (uint16_t)(((uint16_t)(local.tm_mon + 1u) << 8u) | (uint8_t)local.tm_mday);
    } else {
        ax = (uint16_t)cpu->eax;
        cx = (uint16_t)(((uint16_t)local.tm_hour << 8u) | (uint8_t)local.tm_min);
        dx = (uint16_t)((uint16_t)local.tm_sec << 8u);
    }
    return bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u, cx) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 3u, dx);
}
