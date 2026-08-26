#ifndef RUNTIME_MACHINE_PROFILE_ABI_H
#define RUNTIME_MACHINE_PROFILE_ABI_H

#include <stdint.h>

#include "observation_transaction_abi.h"
#include "startup_snapshot_abi.h"

#define RUNTIME_MACHINE_PROFILE_ABI_MAGIC 0x42584d50u
#define RUNTIME_MACHINE_PROFILE_ABI_VERSION 1u
#define RUNTIME_MACHINE_PROFILE_MAX_OBSERVATIONS 4u

/* Profile-owned range data. The identifier has no guest or device meaning;
 * it only selects one immutable observation declaration. */
typedef struct runtime_machine_observation {
    uint32_t id;
    uint32_t flags;
    runtime_guest_range guest_read;
} runtime_machine_observation;

/* A fixed, validated collection of observation declarations supplied by the
 * machine-profile owner. This ABI contains no Bochs object, memory pointer,
 * firmware value or host path. */
typedef struct runtime_machine_profile {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t flags;
    uint32_t observation_count;
    uint32_t reserved0;
    runtime_machine_observation observations[RUNTIME_MACHINE_PROFILE_MAX_OBSERVATIONS];
    uint32_t trigger_observation_id;
    uint32_t trigger_exception_vector;
    uint32_t trigger_resume_bytes;
    uint8_t trigger_instruction_bytes[2];
    uint8_t trigger_instruction_length;
    uint8_t reserved1;
    uint32_t snapshot_trigger_count;
    uint32_t snapshot_trigger_ids[RUNTIME_MACHINE_PROFILE_MAX_OBSERVATIONS];
} runtime_machine_profile;

#ifdef __cplusplus
extern "C" {
#endif

void runtime_machine_profile_initialize(
    runtime_machine_profile *profile);
int runtime_machine_profile_set_observation(
    runtime_machine_profile *profile, uint32_t id,
    const runtime_guest_range *guest_read);
int runtime_machine_profile_set_neutral_ud2_trigger(
    runtime_machine_profile *profile, uint32_t observation_id);
int runtime_machine_profile_set_neutral_ud2_snapshot_trigger(
    runtime_machine_profile *profile, const uint32_t *observation_ids,
    uint32_t observation_count);
int runtime_machine_profile_valid(
    const runtime_machine_profile *profile, uint64_t aperture_bytes);
int runtime_machine_profile_prepare_observation(
    const runtime_machine_profile *profile, uint32_t id,
    const runtime_exception_event *boundary,
    const runtime_cpu_state *cpu_before,
    uint64_t aperture_bytes,
    runtime_observation_transaction *transaction);
int runtime_machine_profile_prepare_neutral_ud2_trigger(
    const runtime_machine_profile *profile,
    const runtime_exception_event *boundary,
    const runtime_cpu_state *cpu_before,
    const runtime_instruction_window *window,
    uint64_t aperture_bytes,
    runtime_observation_transaction *transaction,
    uint64_t *resume_rip);
int runtime_machine_profile_prepare_neutral_ud2_snapshot_trigger(
    const runtime_machine_profile *profile,
    const runtime_exception_event *boundary,
    const runtime_cpu_state *cpu_before,
    const runtime_instruction_window *window,
    uint64_t aperture_bytes,
    runtime_startup_snapshot_transaction *transaction,
    uint64_t *resume_rip);

#ifdef __cplusplus
}
#endif

#endif
