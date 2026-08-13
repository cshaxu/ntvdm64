#include "bx_ntvdm_machine_composition_v2.h"

#if BX_NTVDM_ENABLE_MACHINE_COMPOSITION
extern void unexpected_int(void);
extern void illegal_op_int(void);
#endif

#if defined(_MSC_VER)
#define BX_NTVDM_MACHINE_TLS __declspec(thread)
#else
#define BX_NTVDM_MACHINE_TLS _Thread_local
#endif

static BX_NTVDM_MACHINE_TLS const bx_ntvdm_machine_mechanics_v1
    *bx_ntvdm_machine_active_mechanics;
static BX_NTVDM_MACHINE_TLS int bx_ntvdm_machine_failed;

void bx_ntvdm_machine_mechanics_v1_initialize(
    bx_ntvdm_machine_mechanics_v1 *mechanics)
{
    if (mechanics == 0) return;
    mechanics->magic = BX_NTVDM_MACHINE_MECHANICS_V1_MAGIC;
    mechanics->abi_version = BX_NTVDM_MACHINE_MECHANICS_V1_VERSION;
    mechanics->struct_bytes = sizeof(*mechanics);
    mechanics->opaque = 0;
    mechanics->read8 = 0;
    mechanics->write8 = 0;
    mechanics->store8 = 0;
    mechanics->load8 = 0;
    mechanics->report = 0;
    mechanics->execution_mode = 0u;
    mechanics->esp = 0u;
    mechanics->ss = 0u;
    mechanics->reserved0 = 0u;
}

int bx_ntvdm_machine_mechanics_v1_valid(
    const bx_ntvdm_machine_mechanics_v1 *mechanics)
{
    return mechanics != 0 && mechanics->magic == BX_NTVDM_MACHINE_MECHANICS_V1_MAGIC &&
        mechanics->abi_version == BX_NTVDM_MACHINE_MECHANICS_V1_VERSION &&
        mechanics->struct_bytes == sizeof(*mechanics) && mechanics->read8 != 0 &&
        mechanics->write8 != 0 && mechanics->store8 != 0 &&
        mechanics->reserved0 == 0u;
}

void inb(uint16_t port, uint8_t *value)
{
    if (value == 0 || bx_ntvdm_machine_active_mechanics == 0 ||
        !bx_ntvdm_machine_active_mechanics->read8(
            bx_ntvdm_machine_active_mechanics->opaque, port, value)) {
        if (value != 0) *value = 0xffu;
        bx_ntvdm_machine_failed = 1;
    }
}

void outb(uint16_t port, uint8_t value)
{
    if (bx_ntvdm_machine_active_mechanics == 0 ||
        !bx_ntvdm_machine_active_mechanics->write8(
            bx_ntvdm_machine_active_mechanics->opaque, port, value))
        bx_ntvdm_machine_failed = 1;
}

void c_sas_store(uint32_t address, uint8_t value)
{
    if (bx_ntvdm_machine_active_mechanics == 0 ||
        !bx_ntvdm_machine_active_mechanics->store8(
            bx_ntvdm_machine_active_mechanics->opaque, address, value))
        bx_ntvdm_machine_failed = 1;
}

uint32_t c_effective_addr(uint16_t segment, uint32_t offset)
{
    return ((uint32_t)segment << 4) + offset;
}

uint32_t c_getESP(void)
{
    if (bx_ntvdm_machine_active_mechanics == 0) {
        bx_ntvdm_machine_failed = 1;
        return 0u;
    }
    return bx_ntvdm_machine_active_mechanics->esp;
}

uint16_t c_getSS(void)
{
    if (bx_ntvdm_machine_active_mechanics == 0) {
        bx_ntvdm_machine_failed = 1;
        return 0u;
    }
    return bx_ntvdm_machine_active_mechanics->ss;
}

uint8_t c_sas_hw_at(uint32_t address)
{
    uint8_t value = 0u;
    if (bx_ntvdm_machine_active_mechanics == 0 ||
        !bx_ntvdm_machine_active_mechanics->load8(
            bx_ntvdm_machine_active_mechanics->opaque, address, &value))
        bx_ntvdm_machine_failed = 1;
    return value;
}

short host_error(int error_number, int options, char *message)
{
    if (bx_ntvdm_machine_active_mechanics == 0 ||
        bx_ntvdm_machine_active_mechanics->report == 0)
        bx_ntvdm_machine_failed = 1;
    else
        bx_ntvdm_machine_active_mechanics->report(
            bx_ntvdm_machine_active_mechanics->opaque,
            (uint32_t)error_number, (uint32_t)options, message);
    return 2;
}

#if BX_NTVDM_ENABLE_MACHINE_COMPOSITION
static int bx_ntvdm_machine_composition_v2_execute(
    const bx_ntvdm_machine_mechanics_v1 *mechanics, void (*handler)(void))
{
    if (!bx_ntvdm_machine_mechanics_v1_valid(mechanics) || handler == 0 ||
        bx_ntvdm_machine_active_mechanics != 0) return 0;
    bx_ntvdm_machine_active_mechanics = mechanics;
    bx_ntvdm_machine_failed = 0;
    handler();
    bx_ntvdm_machine_active_mechanics = 0;
    return !bx_ntvdm_machine_failed;
}
#endif

int bx_ntvdm_machine_composition_v2_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    const bx_ntvdm_machine_mechanics_v1 *mechanics,
    bx_ntvdm_cpu_result_v2 *result)
{
    if (result == 0 || event == 0 || cpu_before == 0 || window == 0 ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window) ||
        !bx_ntvdm_machine_mechanics_v1_valid(mechanics)) return 0;
    bx_ntvdm_cpu_result_v2_pass_through(result);
#if BX_NTVDM_ENABLE_MACHINE_COMPOSITION
    if (event->kind != BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION ||
        event->vector != 6u || window->valid_bytes < 3u ||
        window->bytes[0] != 0xc4u || window->bytes[1] != 0xc4u) return 1;
    if (event->fault_rip > UINT64_MAX - 3u) return bx_ntvdm_cpu_result_v2_stop(result);
    if (window->bytes[2] == 0x02u) {
        if (!bx_ntvdm_machine_composition_v2_execute(mechanics, unexpected_int))
            return bx_ntvdm_cpu_result_v2_stop(result);
    } else if (window->bytes[2] == 0x06u) {
        if (mechanics->load8 == 0 || mechanics->report == 0 ||
            mechanics->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
            !bx_ntvdm_machine_composition_v2_execute(mechanics, illegal_op_int))
            return bx_ntvdm_cpu_result_v2_stop(result);
    } else return 1;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 3u);
#else
    (void)event; (void)cpu_before; (void)window; (void)mechanics;
    return 1;
#endif
}
