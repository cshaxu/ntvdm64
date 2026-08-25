#include "machine_profile_abi.h"

#include <string.h>

void runtime_machine_profile_initialize(
    runtime_machine_profile *profile)
{
    if (profile == 0) return;
    memset(profile, 0, sizeof(*profile));
    profile->magic = RUNTIME_MACHINE_PROFILE_ABI_MAGIC;
    profile->abi_version = RUNTIME_MACHINE_PROFILE_ABI_VERSION;
    profile->struct_bytes = sizeof(*profile);
}

int runtime_machine_profile_set_observation(
    runtime_machine_profile *profile, uint32_t id,
    const runtime_guest_range *guest_read)
{
    uint32_t index;
    if (profile == 0 || id == 0u || guest_read == 0 ||
        guest_read->length == 0u ||
        profile->magic != RUNTIME_MACHINE_PROFILE_ABI_MAGIC ||
        profile->abi_version != RUNTIME_MACHINE_PROFILE_ABI_VERSION ||
        profile->struct_bytes != sizeof(*profile) || profile->flags != 0u ||
        profile->reserved0 != 0u) return 0;
    for (index = 0u; index < profile->observation_count; ++index) {
        if (profile->observations[index].id == id) return 0;
    }
    if (profile->observation_count >= RUNTIME_MACHINE_PROFILE_MAX_OBSERVATIONS)
        return 0;
    index = profile->observation_count++;
    profile->observations[index].id = id;
    profile->observations[index].flags = 0u;
    profile->observations[index].guest_read = *guest_read;
    return 1;
}

int runtime_machine_profile_set_neutral_ud2_trigger(
    runtime_machine_profile *profile, uint32_t observation_id)
{
    uint32_t index;
    if (profile == 0 || observation_id == 0u ||
        profile->magic != RUNTIME_MACHINE_PROFILE_ABI_MAGIC ||
        profile->abi_version != RUNTIME_MACHINE_PROFILE_ABI_VERSION ||
        profile->struct_bytes != sizeof(*profile) || profile->flags != 0u ||
        profile->reserved0 != 0u || profile->trigger_observation_id != 0u ||
        profile->snapshot_trigger_count != 0u)
        return 0;
    for (index = 0u; index < profile->observation_count; ++index) {
        if (profile->observations[index].id == observation_id) {
            profile->trigger_observation_id = observation_id;
            profile->trigger_exception_vector = 6u;
            profile->trigger_resume_bytes = 2u;
            profile->trigger_instruction_bytes[0] = 0x0fu;
            profile->trigger_instruction_bytes[1] = 0x0bu;
            profile->trigger_instruction_length = 2u;
            return 1;
        }
    }
    return 0;
}

int runtime_machine_profile_set_neutral_ud2_snapshot_trigger(
    runtime_machine_profile *profile, const uint32_t *observation_ids,
    uint32_t observation_count)
{
    uint32_t index;
    if (profile == 0 || observation_ids == 0 || observation_count == 0u ||
        observation_count > RUNTIME_MACHINE_PROFILE_MAX_OBSERVATIONS ||
        profile->magic != RUNTIME_MACHINE_PROFILE_ABI_MAGIC ||
        profile->abi_version != RUNTIME_MACHINE_PROFILE_ABI_VERSION ||
        profile->struct_bytes != sizeof(*profile) || profile->flags != 0u ||
        profile->reserved0 != 0u || profile->trigger_observation_id != 0u ||
        profile->snapshot_trigger_count != 0u) return 0;
    for (index = 0u; index < observation_count; ++index) {
        uint32_t other;
        uint32_t found = 0u;
        if (observation_ids[index] == 0u) return 0;
        for (other = 0u; other < index; ++other) {
            if (observation_ids[other] == observation_ids[index]) return 0;
        }
        for (other = 0u; other < profile->observation_count; ++other) {
            if (profile->observations[other].id == observation_ids[index]) { found = 1u; break; }
        }
        if (found == 0u) return 0;
    }
    profile->snapshot_trigger_count = observation_count;
    memcpy(profile->snapshot_trigger_ids, observation_ids,
        observation_count * sizeof(observation_ids[0]));
    profile->trigger_exception_vector = 6u;
    profile->trigger_resume_bytes = 2u;
    profile->trigger_instruction_bytes[0] = 0x0fu;
    profile->trigger_instruction_bytes[1] = 0x0bu;
    profile->trigger_instruction_length = 2u;
    return 1;
}

int runtime_machine_profile_valid(
    const runtime_machine_profile *profile, uint64_t aperture_bytes)
{
    uint32_t index;
    uint32_t prior;
    if (profile == 0 || profile->magic != RUNTIME_MACHINE_PROFILE_ABI_MAGIC ||
        profile->abi_version != RUNTIME_MACHINE_PROFILE_ABI_VERSION ||
        profile->struct_bytes != sizeof(*profile) || profile->flags != 0u ||
        profile->reserved0 != 0u || profile->observation_count == 0u ||
        profile->observation_count > RUNTIME_MACHINE_PROFILE_MAX_OBSERVATIONS)
        return 0;
    for (index = 0u; index < profile->observation_count; ++index) {
        const runtime_machine_observation *observation =
            &profile->observations[index];
        if (observation->id == 0u || observation->flags != 0u ||
            observation->guest_read.length == 0u ||
            !runtime_guest_range_within(aperture_bytes,
                &observation->guest_read)) return 0;
        for (prior = 0u; prior < index; ++prior) {
            if (profile->observations[prior].id == observation->id) return 0;
        }
    }
    if (profile->trigger_observation_id != 0u || profile->snapshot_trigger_count != 0u) {
        int found = 0;
        if (profile->trigger_exception_vector != 6u ||
            profile->trigger_resume_bytes != 2u ||
            profile->trigger_instruction_length != 2u ||
            profile->trigger_instruction_bytes[0] != 0x0fu ||
            profile->trigger_instruction_bytes[1] != 0x0bu ||
            profile->reserved1 != 0u) return 0;
        if (profile->trigger_observation_id != 0u &&
            profile->snapshot_trigger_count != 0u) return 0;
        if (profile->trigger_observation_id != 0u) {
            for (index = 0u; index < profile->observation_count; ++index) {
                if (profile->observations[index].id == profile->trigger_observation_id) {
                    found = 1;
                    break;
                }
            }
            if (!found) return 0;
        } else {
            if (profile->snapshot_trigger_count >
                RUNTIME_MACHINE_PROFILE_MAX_OBSERVATIONS) return 0;
            for (index = 0u; index < profile->snapshot_trigger_count; ++index) {
                uint32_t other;
                found = 0;
                if (profile->snapshot_trigger_ids[index] == 0u) return 0;
                for (other = 0u; other < index; ++other) {
                    if (profile->snapshot_trigger_ids[other] ==
                        profile->snapshot_trigger_ids[index]) return 0;
                }
                for (other = 0u; other < profile->observation_count; ++other) {
                    if (profile->observations[other].id ==
                        profile->snapshot_trigger_ids[index]) { found = 1; break; }
                }
                if (!found) return 0;
            }
        }
    } else if (profile->trigger_exception_vector != 0u ||
        profile->trigger_resume_bytes != 0u ||
        profile->trigger_instruction_length != 0u || profile->reserved1 != 0u ||
        profile->trigger_instruction_bytes[0] != 0u ||
        profile->trigger_instruction_bytes[1] != 0u) return 0;
    else {
        for (index = 0u; index < RUNTIME_MACHINE_PROFILE_MAX_OBSERVATIONS;
            ++index) {
            if (profile->snapshot_trigger_ids[index] != 0u) return 0;
        }
    }
    return 1;
}

int runtime_machine_profile_prepare_neutral_ud2_snapshot_trigger(
    const runtime_machine_profile *profile,
    const runtime_exception_event *boundary,
    const runtime_cpu_state *cpu_before,
    const runtime_instruction_window *window,
    uint64_t aperture_bytes,
    runtime_startup_snapshot_transaction *transaction,
    uint64_t *resume_rip)
{
    runtime_startup_snapshot_range ranges[
        RUNTIME_MACHINE_PROFILE_MAX_OBSERVATIONS];
    uint32_t index;
    if (profile == 0 || window == 0 || transaction == 0 || resume_rip == 0 ||
        !runtime_machine_profile_valid(profile, aperture_bytes) ||
        profile->snapshot_trigger_count == 0u || boundary == 0 ||
        boundary->vector != profile->trigger_exception_vector ||
        !runtime_instruction_window_valid(window) ||
        window->valid_bytes < profile->trigger_instruction_length ||
        memcmp(window->bytes, profile->trigger_instruction_bytes,
            profile->trigger_instruction_length) != 0 ||
        boundary->fault_rip > UINT64_MAX - profile->trigger_resume_bytes)
        return 0;
    for (index = 0u; index < profile->snapshot_trigger_count; ++index) {
        uint32_t observation_index;
        for (observation_index = 0u;
            observation_index < profile->observation_count; ++observation_index) {
            if (profile->observations[observation_index].id ==
                profile->snapshot_trigger_ids[index]) break;
        }
        if (observation_index == profile->observation_count) return 0;
        ranges[index].id = profile->observations[observation_index].id;
        ranges[index].flags = 0u;
        ranges[index].guest_read = profile->observations[observation_index].guest_read;
    }
    runtime_startup_snapshot_transaction_initialize(transaction, boundary,
        cpu_before, ranges, profile->snapshot_trigger_count);
    if (!runtime_startup_snapshot_transaction_preflight(transaction,
        aperture_bytes, transaction->output_bytes)) return 0;
    *resume_rip = boundary->fault_rip + profile->trigger_resume_bytes;
    return 1;
}

int runtime_machine_profile_prepare_observation(
    const runtime_machine_profile *profile, uint32_t id,
    const runtime_exception_event *boundary,
    const runtime_cpu_state *cpu_before,
    uint64_t aperture_bytes,
    runtime_observation_transaction *transaction)
{
    uint32_t index;
    if (transaction == 0 || id == 0u ||
        !runtime_machine_profile_valid(profile, aperture_bytes)) return 0;
    for (index = 0u; index < profile->observation_count; ++index) {
        if (profile->observations[index].id == id) {
            runtime_observation_transaction_initialize(transaction, boundary,
                cpu_before, &profile->observations[index].guest_read);
            return runtime_observation_transaction_preflight(transaction,
                aperture_bytes, profile->observations[index].guest_read.length);
        }
    }
    return 0;
}

int runtime_machine_profile_prepare_neutral_ud2_trigger(
    const runtime_machine_profile *profile,
    const runtime_exception_event *boundary,
    const runtime_cpu_state *cpu_before,
    const runtime_instruction_window *window,
    uint64_t aperture_bytes,
    runtime_observation_transaction *transaction,
    uint64_t *resume_rip)
{
    if (profile == 0 || window == 0 || transaction == 0 || resume_rip == 0 ||
        !runtime_machine_profile_valid(profile, aperture_bytes) ||
        profile->trigger_observation_id == 0u || boundary == 0 ||
        boundary->vector != profile->trigger_exception_vector ||
        !runtime_instruction_window_valid(window) ||
        window->valid_bytes < profile->trigger_instruction_length ||
        memcmp(window->bytes, profile->trigger_instruction_bytes,
            profile->trigger_instruction_length) != 0 ||
        boundary->fault_rip > UINT64_MAX - profile->trigger_resume_bytes)
        return 0;
    if (!runtime_machine_profile_prepare_observation(profile,
            profile->trigger_observation_id, boundary, cpu_before,
            aperture_bytes, transaction)) return 0;
    *resume_rip = boundary->fault_rip + profile->trigger_resume_bytes;
    return 1;
}
