#include <stdio.h>

#include "core/machine/machine_interface.h"

typedef struct ntdos64_core_probe {
    unsigned int transition_calls;
} ntdos64_core_probe;

static void ntdos64_core_probe_transition(void *owner,
    const core_machine_undefined_instruction_input *input,
    core_machine_undefined_instruction_response *out_response)
{
    ntdos64_core_probe *probe = (ntdos64_core_probe *)owner;

    if (probe == NULL || input == NULL || out_response == NULL) return;
    ++probe->transition_calls;
    out_response->outcome = CORE_MACHINE_UNDEFINED_INSTRUCTION_STOP;
}

int main(void)
{
    static const uint8_t transition[] = { 0xd6u };
    static const uint8_t program[] = { 0xd6u, 0xf4u };
    const core_machine_config config = {
        .memory_bytes = CORE_MACHINE_MINIMUM_MEMORY_BYTES,
        .cpu_profile = CORE_MACHINE_CPU_PROFILE_8086,
        .fpu_profile = CORE_MACHINE_FPU_PROFILE_NONE,
        .ticks_per_instruction = 1u
    };
    const core_machine_entry_plan_preload preload = {
        .physical = 0u,
        .bytes = program,
        .byte_count = sizeof(program)
    };
    const core_machine_entry_plan plan = {
        .state = {
            .cs = 0u, .ds = 0u, .es = 0u, .ss = 0u,
            .ip = 0u, .sp = 0x1000u, .eflags = 0x00000200u
        },
        .entry_physical = 0u,
        .entry_route = CORE_MACHINE_MEMORY_ROUTE_ORDINARY_RAM,
        .preloads = &preload,
        .preload_count = 1u
    };
    const core_machine_run_budget budget = { .instructions = 8u, .ticks = 0u };
    core_machine_run_result result;
    ntdos64_core_probe probe = { 0u };
    core_machine *machine = NULL;
    type_status status;

    status = core_machine_create(&config, &machine);
    if (status == TYPE_STATUS_OK) {
        status = core_machine_register_undefined_instruction_transition(machine,
            transition, sizeof(transition), ntdos64_core_probe_transition, &probe);
    }
    if (status == TYPE_STATUS_OK) status = core_machine_freeze_execution_providers(machine);
    if (status == TYPE_STATUS_OK) status = core_machine_reset(machine);
    if (status == TYPE_STATUS_OK) status = core_machine_apply_entry_plan(machine, &plan);
    if (status == TYPE_STATUS_OK) status = core_machine_run(machine, budget, &result);

    if (status != TYPE_STATUS_OK || probe.transition_calls != 1u ||
        result.reason != CORE_MACHINE_STOP_REQUESTED) {
        fprintf(stderr, "ntdos64 core-machine probe: FAIL status=%d calls=%u reason=%d\n",
            (int)status, probe.transition_calls, (int)result.reason);
        core_machine_destroy(machine);
        return 1;
    }

    core_machine_destroy(machine);
    puts("ntdos64 core-machine probe: OK");
    return 0;
}
