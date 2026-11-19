#ifndef COMPAT_GUEST_LOCATION_H
#define COMPAT_GUEST_LOCATION_H
#include "guest_memory_types.h"

/* Numeric real-mode address; mapping lifetime stays with the installed provider. */
typedef struct mvdm_guest_location {
    uint16_t segment;
    uint16_t offset;
    uint32_t valid;
} mvdm_guest_location;

typedef struct mvdm_guest_location_lease {
    guest_memory_lease *lease;
    uint8_t *bytes;
} mvdm_guest_location_lease;

typedef struct compat_guest_location_services {
    int (*copy_to_guest)(const mvdm_guest_location *, const uint8_t *, uint32_t);
    int (*acquire)(const mvdm_guest_location *, uint32_t, uint32_t,
        mvdm_guest_location_lease *);
    int (*release)(mvdm_guest_location_lease *, int);
} compat_guest_location_services;

#ifdef __cplusplus
extern "C" {
#endif
int compat_guest_location_install(const compat_guest_location_services *services);
int mvdm_guest_location_set_real_mode(mvdm_guest_location *location,
    uint16_t segment, uint16_t offset);
int mvdm_guest_location_copy_to_guest(const mvdm_guest_location *location,
    const uint8_t *bytes, uint32_t byte_count);
int mvdm_guest_location_acquire(const mvdm_guest_location *location,
    uint32_t byte_count, uint32_t access, mvdm_guest_location_lease *lease_out);
int mvdm_guest_location_release(mvdm_guest_location_lease *lease, int commit);
#ifdef __cplusplus
}
#endif
#endif
