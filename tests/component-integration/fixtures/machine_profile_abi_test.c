#include <stdint.h>
#include <string.h>

#include "machine_profile_abi.h"

int main(void)
{
    runtime_machine_profile profile;
    runtime_guest_range range = { 0x100u, 8u };
    runtime_exception_event event;
    runtime_cpu_state state;
    runtime_instruction_window window;
    runtime_observation_transaction transaction;
    runtime_startup_snapshot_transaction snapshot;
    uint32_t snapshot_ids[2] = { 7u, 8u };
    uint64_t resume_rip = 0u;
    const uint8_t ud2[] = { 0x0fu, 0x0bu };

    runtime_machine_profile_initialize(&profile);
    if (!runtime_machine_profile_set_observation(&profile, 7u, &range) ||
        runtime_machine_profile_set_observation(&profile, 7u, &range) ||
        !runtime_machine_profile_valid(&profile, 0x100000u)) return 1;
    memset(&event, 0, sizeof(event));
    event.magic = RUNTIME_EXCEPTION_ABI_MAGIC;
    event.abi_version = RUNTIME_EXCEPTION_ABI_VERSION;
    event.struct_bytes = sizeof(event);
    event.kind = RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.vector = 6u;
    runtime_cpu_state_initialize(&state, RUNTIME_CPU_EXECUTION_REAL);
    if (!runtime_machine_profile_prepare_observation(&profile, 7u, &event,
        &state, 0x100000u, &transaction) || transaction.guest_read.address !=
        range.address || transaction.guest_read.length != range.length) return 2;
    if (runtime_machine_profile_prepare_observation(&profile, 8u, &event,
        &state, 0x100000u, &transaction)) return 3;
    event.fault_rip = 0x7c00u;
    runtime_instruction_window_capture(&window, ud2, sizeof(ud2));
    if (runtime_machine_profile_set_neutral_ud2_trigger(&profile, 8u) ||
        !runtime_machine_profile_set_neutral_ud2_trigger(&profile, 7u) ||
        !runtime_machine_profile_valid(&profile, 0x100000u) ||
        !runtime_machine_profile_prepare_neutral_ud2_trigger(&profile,
            &event, &state, &window, 0x100000u, &transaction, &resume_rip) ||
        resume_rip != 0x7c02u || transaction.guest_read.address != range.address)
        return 4;
    window.bytes[0] = 0xc4u;
    if (runtime_machine_profile_prepare_neutral_ud2_trigger(&profile,
        &event, &state, &window, 0x100000u, &transaction, &resume_rip)) return 5;
    range.address = 0xfffffu;
    runtime_machine_profile_initialize(&profile);
    runtime_instruction_window_capture(&window, ud2, sizeof(ud2));
    range.address = 0x40u; range.length = 48u;
    if (!runtime_machine_profile_set_observation(&profile, 7u, &range)) return 6;
    range.address = 0x714u; range.length = 4u;
    if (!runtime_machine_profile_set_observation(&profile, 8u, &range) ||
        !runtime_machine_profile_set_neutral_ud2_snapshot_trigger(&profile,
            snapshot_ids, 2u) ||
        runtime_machine_profile_set_neutral_ud2_trigger(&profile, 7u) ||
        !runtime_machine_profile_prepare_neutral_ud2_snapshot_trigger(
            &profile, &event, &state, &window, 0x100000u, &snapshot, &resume_rip) ||
        snapshot.range_count != 2u || snapshot.output_bytes != 52u ||
        snapshot.ranges[0].id != 7u || snapshot.ranges[1].id != 8u ||
        resume_rip != 0x7c02u) return 7;
    range.address = 0xfffffu;
    range.length = 8u;
    runtime_machine_profile_initialize(&profile);
    if (!runtime_machine_profile_set_observation(&profile, 1u, &range) ||
        runtime_machine_profile_valid(&profile, 0x100000u)) return 8;
    runtime_machine_profile_initialize(0);
    return runtime_machine_profile_valid(0, 0u) ? 9 : 0;
}
