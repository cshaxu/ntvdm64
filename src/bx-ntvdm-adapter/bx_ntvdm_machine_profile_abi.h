#ifndef BX_NTVDM_MACHINE_PROFILE_ABI_H
#define BX_NTVDM_MACHINE_PROFILE_ABI_H

#include <stdint.h>

#include "bx_ntvdm_observation_transaction_abi.h"
#include "bx_ntvdm_startup_snapshot_abi.h"

#define BX_NTVDM_MACHINE_PROFILE_ABI_MAGIC 0x42584d50u
#define BX_NTVDM_MACHINE_PROFILE_ABI_VERSION 1u
#define BX_NTVDM_MACHINE_PROFILE_V1_MAX_OBSERVATIONS 4u

/* Profile-owned range data. The identifier has no guest or device meaning;
 * it only selects one immutable observation declaration. */
typedef struct bx_ntvdm_machine_observation_v1 {
    uint32_t id;
    uint32_t flags;
    bx_ntvdm_guest_range guest_read;
} bx_ntvdm_machine_observation_v1;

/* A fixed, validated collection of observation declarations supplied by the
 * machine-profile owner. This ABI contains no Bochs object, memory pointer,
 * firmware value or host path. */
typedef struct bx_ntvdm_machine_profile_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t flags;
    uint32_t observation_count;
    uint32_t reserved0;
    bx_ntvdm_machine_observation_v1 observations[BX_NTVDM_MACHINE_PROFILE_V1_MAX_OBSERVATIONS];
    uint32_t trigger_observation_id;
    uint32_t trigger_exception_vector;
    uint32_t trigger_resume_bytes;
    uint8_t trigger_instruction_bytes[2];
    uint8_t trigger_instruction_length;
    uint8_t reserved1;
    uint32_t snapshot_trigger_count;
    uint32_t snapshot_trigger_ids[BX_NTVDM_MACHINE_PROFILE_V1_MAX_OBSERVATIONS];
} bx_ntvdm_machine_profile_v1;

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_machine_profile_v1_initialize(
    bx_ntvdm_machine_profile_v1 *profile);
int bx_ntvdm_machine_profile_v1_set_observation(
    bx_ntvdm_machine_profile_v1 *profile, uint32_t id,
    const bx_ntvdm_guest_range *guest_read);
int bx_ntvdm_machine_profile_v1_set_neutral_ud2_trigger(
    bx_ntvdm_machine_profile_v1 *profile, uint32_t observation_id);
int bx_ntvdm_machine_profile_v1_set_neutral_ud2_snapshot_trigger(
    bx_ntvdm_machine_profile_v1 *profile, const uint32_t *observation_ids,
    uint32_t observation_count);
int bx_ntvdm_machine_profile_v1_valid(
    const bx_ntvdm_machine_profile_v1 *profile, uint64_t aperture_bytes);
int bx_ntvdm_machine_profile_v1_prepare_observation(
    const bx_ntvdm_machine_profile_v1 *profile, uint32_t id,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    uint64_t aperture_bytes,
    bx_ntvdm_observation_transaction_v1 *transaction);
int bx_ntvdm_machine_profile_v1_prepare_neutral_ud2_trigger(
    const bx_ntvdm_machine_profile_v1 *profile,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    uint64_t aperture_bytes,
    bx_ntvdm_observation_transaction_v1 *transaction,
    uint64_t *resume_rip);
int bx_ntvdm_machine_profile_v1_prepare_neutral_ud2_snapshot_trigger(
    const bx_ntvdm_machine_profile_v1 *profile,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    uint64_t aperture_bytes,
    bx_ntvdm_startup_snapshot_transaction_v1 *transaction,
    uint64_t *resume_rip);

#ifdef __cplusplus
}
#endif

#endif
