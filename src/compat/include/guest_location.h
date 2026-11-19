#ifndef COMPAT_GUEST_LOCATION_H
#define COMPAT_GUEST_LOCATION_H
#include "guest_memory_types.h"

/* Numeric real-mode address; mapping lifetime stays with the bound lease owner. */
typedef struct mvdm_guest_location {
    uint16_t segment;
    uint16_t offset;
    uint32_t valid;
} mvdm_guest_location;

typedef struct mvdm_guest_location_lease {
    guest_memory_lease *lease;
    uint8_t *bytes;
} mvdm_guest_location_lease;

#ifdef __cplusplus
extern "C" {
#endif
int mvdm_guest_location_set_real_mode(mvdm_guest_location *location,
    uint16_t segment, uint16_t offset);
int mvdm_guest_location_copy_to_guest(const mvdm_guest_location *location,
    const uint8_t *bytes, uint32_t byte_count);
int mvdm_guest_location_acquire(const mvdm_guest_location *location,
    uint32_t byte_count, uint32_t access, mvdm_guest_location_lease *lease_out);
int mvdm_guest_location_release(mvdm_guest_location_lease *lease, int commit);
int mvdm_guest_location_from_far_value(mvdm_guest_location *location,
    uint32_t far_value);
int mvdm_guest_location_read_far(mvdm_guest_location const *location,
    mvdm_guest_location *far_location_out);
int mvdm_guest_location_acquire_far(mvdm_guest_location const *location,
    uint32_t byte_count, uint32_t access,
    mvdm_guest_location_lease *lease_out);
int mvdm_guest_location_read_u16(mvdm_guest_location const *location,
    uint16_t *value_out);
int mvdm_guest_location_copy_c_string(mvdm_guest_location const *location,
    uint8_t *bytes_out, uint32_t capacity, uint32_t *byte_count_out);
/* Synchronous VMM alias access: base is the actual VM's CB_High_Linear,
 * never a native pointer. Ordinary real-mode access uses base zero. */
int mvdm_guest_location_acquire_vm(mvdm_guest_location const *location,
    uint32_t linear_base, uint32_t byte_count, uint32_t access,
    mvdm_guest_location_lease *lease_out);

#ifdef __cplusplus
}
#endif
#endif
