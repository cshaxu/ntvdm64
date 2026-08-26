#include "adapter-bochs/bounded_run.h"

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

int request_valid(const adapter_bochs_bounded_run_request *request)
{
    return request != 0 &&
        request->version == ADAPTER_BOCHS_BOUNDED_RUN_VERSION &&
        request->ips != 0 && request->tick_budget != 0 &&
        request->byte_count != 0 &&
        request->byte_count <= ADAPTER_BOCHS_BOUNDED_RUN_MAX_BYTES &&
        request->physical_address <= UINT64_C(0x100000) &&
        request->byte_count <= UINT64_C(0x100000) - request->physical_address;
}

}  // namespace

extern "C" enum adapter_bochs_bounded_run_status adapter_bochs_bounded_run(
    const struct adapter_bochs_bounded_run_request *request)
{
    stop_state state;
    uint32_t timer_id;

    if (!request_valid(request))
        return ADAPTER_BOCHS_BOUNDED_RUN_REJECTED_INPUT;
    if (!machine_facade_machine_begin(UINT64_C(0x100000), UINT64_C(0x100000)))
        return ADAPTER_BOCHS_BOUNDED_RUN_MACHINE_FAILURE;
    if (!machine_facade_memory_write(request->physical_address,
        request->byte_count, request->bytes)) {
        machine_facade_machine_cleanup();
        return ADAPTER_BOCHS_BOUNDED_RUN_MACHINE_FAILURE;
    }

    machine_facade_initialize_timing(request->ips);
    machine_facade_apply_real_mode_entry(request->cs, request->eip);
    state.fired = 0;
    if (!machine_facade_register_timer(&state, stop_on_budget,
        request->tick_budget, 0, 1, &timer_id)) {
        machine_facade_machine_cleanup();
        return ADAPTER_BOCHS_BOUNDED_RUN_MACHINE_FAILURE;
    }

    machine_facade_cpu_loop();
    machine_facade_deactivate_timer(timer_id);
    machine_facade_unregister_timer(timer_id);
    if (!machine_facade_machine_cleanup())
        return ADAPTER_BOCHS_BOUNDED_RUN_MACHINE_FAILURE;
    return state.fired ? ADAPTER_BOCHS_BOUNDED_RUN_BUDGET :
        ADAPTER_BOCHS_BOUNDED_RUN_UNEXPECTED_RETURN;
}
