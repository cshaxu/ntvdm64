#include "adapter-bochs/machine_lifecycle.h"

#include "adapter-bochs/machine_facade.h"

namespace {

struct stop_state {
    uint32_t fired;
};

void stop_on_budget(void *opaque)
{
    stop_state *state = static_cast<stop_state *>(opaque);
    state->fired = 1;
    machine_facade_request_cpu_stop();
}

int configuration_valid(
    const adapter_bochs_machine_lifecycle_configuration *configuration)
{
    return configuration != 0 &&
        configuration->version == ADAPTER_BOCHS_MACHINE_LIFECYCLE_VERSION &&
        configuration->ips != 0 && configuration->guest_memory_bytes != 0 &&
        configuration->host_memory_bytes != 0;
}

int load_valid(const adapter_bochs_machine_lifecycle_load *load)
{
    return load != 0 &&
        load->version == ADAPTER_BOCHS_MACHINE_LIFECYCLE_VERSION &&
        load->byte_count != 0 &&
        load->byte_count <= ADAPTER_BOCHS_MACHINE_LIFECYCLE_MAX_BYTES &&
        load->physical_address <= UINT64_C(0x100000) &&
        load->byte_count <= UINT64_C(0x100000) - load->physical_address;
}

}  // namespace

extern "C" enum adapter_bochs_machine_lifecycle_status
adapter_bochs_machine_lifecycle_create(
    const struct adapter_bochs_machine_lifecycle_configuration *configuration)
{
    if (!configuration_valid(configuration))
        return ADAPTER_BOCHS_MACHINE_LIFECYCLE_REJECTED_INPUT;
    if (machine_facade_machine_active())
        return ADAPTER_BOCHS_MACHINE_LIFECYCLE_REJECTED_ACTIVE;
    if (!machine_facade_machine_begin(configuration->guest_memory_bytes,
        configuration->host_memory_bytes))
        return ADAPTER_BOCHS_MACHINE_LIFECYCLE_MACHINE_FAILURE;
    machine_facade_initialize_timing(configuration->ips);
    return ADAPTER_BOCHS_MACHINE_LIFECYCLE_OK;
}

extern "C" enum adapter_bochs_machine_lifecycle_status
adapter_bochs_machine_lifecycle_load_realmode(
    const struct adapter_bochs_machine_lifecycle_load *load)
{
    if (!machine_facade_machine_active())
        return ADAPTER_BOCHS_MACHINE_LIFECYCLE_REJECTED_INACTIVE;
    if (!load_valid(load))
        return ADAPTER_BOCHS_MACHINE_LIFECYCLE_REJECTED_INPUT;
    if (!machine_facade_memory_write(load->physical_address, load->byte_count,
        load->bytes))
        return ADAPTER_BOCHS_MACHINE_LIFECYCLE_MACHINE_FAILURE;
    machine_facade_apply_real_mode_entry(load->cs, load->eip);
    return ADAPTER_BOCHS_MACHINE_LIFECYCLE_OK;
}

extern "C" enum adapter_bochs_machine_lifecycle_status
adapter_bochs_machine_lifecycle_run_budget(uint64_t tick_budget)
{
    stop_state state;
    uint32_t timer_id;

    if (!machine_facade_machine_active())
        return ADAPTER_BOCHS_MACHINE_LIFECYCLE_REJECTED_INACTIVE;
    if (tick_budget == 0)
        return ADAPTER_BOCHS_MACHINE_LIFECYCLE_REJECTED_INPUT;
    if (!machine_facade_prepare_cpu_resume())
        return ADAPTER_BOCHS_MACHINE_LIFECYCLE_MACHINE_FAILURE;

    state.fired = 0;
    if (!machine_facade_register_timer(&state, stop_on_budget, tick_budget,
        0, 1, &timer_id))
        return ADAPTER_BOCHS_MACHINE_LIFECYCLE_MACHINE_FAILURE;
    machine_facade_cpu_loop();
    machine_facade_deactivate_timer(timer_id);
    machine_facade_unregister_timer(timer_id);
    return state.fired ? ADAPTER_BOCHS_MACHINE_LIFECYCLE_BUDGET :
        ADAPTER_BOCHS_MACHINE_LIFECYCLE_UNEXPECTED_RETURN;
}

extern "C" enum adapter_bochs_machine_lifecycle_status
adapter_bochs_machine_lifecycle_destroy(void)
{
    if (!machine_facade_machine_active())
        return ADAPTER_BOCHS_MACHINE_LIFECYCLE_REJECTED_INACTIVE;
    return machine_facade_machine_cleanup() ?
        ADAPTER_BOCHS_MACHINE_LIFECYCLE_OK :
        ADAPTER_BOCHS_MACHINE_LIFECYCLE_MACHINE_FAILURE;
}

extern "C" int adapter_bochs_machine_lifecycle_active(void)
{
    return machine_facade_machine_active();
}
