#include "machine_composition.h"

#if RUNTIME_ENABLE_MACHINE_COMPOSITION
extern void unexpected_int(void);
extern void illegal_op_int(void);
#endif

#if defined(_MSC_VER)
#define RUNTIME_MACHINE_TLS __declspec(thread)
#else
#define RUNTIME_MACHINE_TLS _Thread_local
#endif

static RUNTIME_MACHINE_TLS const runtime_machine_mechanics_v1
    *runtime_machine_active_mechanics;
static RUNTIME_MACHINE_TLS int runtime_machine_failed;

void runtime_machine_mechanics_v1_initialize(
    runtime_machine_mechanics_v1 *mechanics)
{
    if (mechanics == 0) return;
    mechanics->magic = RUNTIME_MACHINE_MECHANICS_V1_MAGIC;
    mechanics->abi_version = RUNTIME_MACHINE_MECHANICS_V1_VERSION;
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

int runtime_machine_mechanics_v1_valid(
    const runtime_machine_mechanics_v1 *mechanics)
{
    return mechanics != 0 && mechanics->magic == RUNTIME_MACHINE_MECHANICS_V1_MAGIC &&
        mechanics->abi_version == RUNTIME_MACHINE_MECHANICS_V1_VERSION &&
        mechanics->struct_bytes == sizeof(*mechanics) && mechanics->read8 != 0 &&
        mechanics->write8 != 0 && mechanics->store8 != 0 &&
        mechanics->reserved0 == 0u;
}

void inb(uint16_t port, uint8_t *value)
{
    if (value == 0 || runtime_machine_active_mechanics == 0 ||
        !runtime_machine_active_mechanics->read8(
            runtime_machine_active_mechanics->opaque, port, value)) {
        if (value != 0) *value = 0xffu;
        runtime_machine_failed = 1;
    }
}

void outb(uint16_t port, uint8_t value)
{
    if (runtime_machine_active_mechanics == 0 ||
        !runtime_machine_active_mechanics->write8(
            runtime_machine_active_mechanics->opaque, port, value))
        runtime_machine_failed = 1;
}

void c_sas_store(uint32_t address, uint8_t value)
{
    if (runtime_machine_active_mechanics == 0 ||
        !runtime_machine_active_mechanics->store8(
            runtime_machine_active_mechanics->opaque, address, value))
        runtime_machine_failed = 1;
}

uint32_t c_effective_addr(uint16_t segment, uint32_t offset)
{
    return ((uint32_t)segment << 4) + offset;
}

uint32_t c_getESP(void)
{
    if (runtime_machine_active_mechanics == 0) {
        runtime_machine_failed = 1;
        return 0u;
    }
    return runtime_machine_active_mechanics->esp;
}

uint16_t c_getSS(void)
{
    if (runtime_machine_active_mechanics == 0) {
        runtime_machine_failed = 1;
        return 0u;
    }
    return runtime_machine_active_mechanics->ss;
}

uint8_t c_sas_hw_at(uint32_t address)
{
    uint8_t value = 0u;
    if (runtime_machine_active_mechanics == 0 ||
        !runtime_machine_active_mechanics->load8(
            runtime_machine_active_mechanics->opaque, address, &value))
        runtime_machine_failed = 1;
    return value;
}

short host_error(int error_number, int options, char *message)
{
    if (runtime_machine_active_mechanics == 0 ||
        runtime_machine_active_mechanics->report == 0)
        runtime_machine_failed = 1;
    else
        runtime_machine_active_mechanics->report(
            runtime_machine_active_mechanics->opaque,
            (uint32_t)error_number, (uint32_t)options, message);
    return 2;
}

#if RUNTIME_ENABLE_MACHINE_COMPOSITION
static int runtime_machine_composition_v2_execute(
    const runtime_machine_mechanics_v1 *mechanics, void (*handler)(void))
{
    if (!runtime_machine_mechanics_v1_valid(mechanics) || handler == 0 ||
        runtime_machine_active_mechanics != 0) return 0;
    runtime_machine_active_mechanics = mechanics;
    runtime_machine_failed = 0;
    handler();
    runtime_machine_active_mechanics = 0;
    return !runtime_machine_failed;
}
#endif

int runtime_machine_composition_v2_dispatch(
    const runtime_exception_event_v1 *event,
    const runtime_cpu_state_v1 *cpu_before,
    const runtime_instruction_window_v1 *window,
    const runtime_machine_mechanics_v1 *mechanics,
    runtime_cpu_result_v2 *result)
{
    if (result == 0 || event == 0 || cpu_before == 0 || window == 0 ||
        !runtime_exception_event_v1_valid(event) ||
        !runtime_cpu_state_v1_valid(cpu_before) ||
        !runtime_instruction_window_v1_valid(window) ||
        !runtime_machine_mechanics_v1_valid(mechanics)) return 0;
    runtime_cpu_result_v2_pass_through(result);
#if RUNTIME_ENABLE_MACHINE_COMPOSITION
    if (event->kind != RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION ||
        event->vector != 6u || window->valid_bytes < 3u ||
        window->bytes[0] != 0xc4u || window->bytes[1] != 0xc4u) return 1;
    if (event->fault_rip > UINT64_MAX - 3u) return runtime_cpu_result_v2_stop(result);
    if (window->bytes[2] == 0x02u) {
        if (!runtime_machine_composition_v2_execute(mechanics, unexpected_int))
            return runtime_cpu_result_v2_stop(result);
    } else if (window->bytes[2] == 0x06u) {
        if (mechanics->load8 == 0 || mechanics->report == 0 ||
            mechanics->execution_mode != RUNTIME_CPU_EXECUTION_REAL ||
            !runtime_machine_composition_v2_execute(mechanics, illegal_op_int))
            return runtime_cpu_result_v2_stop(result);
    } else return 1;
    return runtime_cpu_result_v2_resume(result, event->fault_rip + 3u);
#else
    (void)event; (void)cpu_before; (void)window; (void)mechanics;
    return 1;
#endif
}
