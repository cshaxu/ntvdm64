#ifndef MVDM_COMMAND_GUEST_STATE_H
#define MVDM_COMMAND_GUEST_STATE_H

#include <stdint.h>

#include "mvdm_guest_location.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Retains the original COMMAND SCS/DOS-data locations as real-mode numeric
 * addresses.  No host pointer leaves a call; all dereferences use the
 * thread-bound session guest-memory mapping manager through a short lease.
 */
int mvdm_command_guest_state_set_scs(uint16_t segment, uint16_t offset,
    uint32_t scs_bytes, uint32_t to_sync_offset);
int mvdm_command_guest_state_acquire_scs(uint32_t access,
    mvdm_guest_location_lease *lease_out);
int mvdm_command_guest_state_release_scs(mvdm_guest_location_lease *lease,
    int commit);
int mvdm_command_guest_state_scs_field(uint32_t byte_offset,
    uint16_t *segment_out, uint16_t *offset_out);
int mvdm_command_guest_state_set_to_sync(uint8_t value);

#ifdef __cplusplus
}
#endif

#endif
