#ifndef COMPAT_GUEST_LOCATION_H
#define COMPAT_GUEST_LOCATION_H
#include <stdint.h>

/* Numeric real-mode address only; no session or borrowed mapping escapes here. */
typedef struct mvdm_guest_location {
    uint16_t segment;
    uint16_t offset;
    uint32_t valid;
} mvdm_guest_location;

typedef struct compat_guest_location_services {
    int (*copy_to_guest)(const mvdm_guest_location *, const uint8_t *, uint32_t);
} compat_guest_location_services;

int compat_guest_location_install(const compat_guest_location_services *services);
int mvdm_guest_location_set_real_mode(mvdm_guest_location *location,
    uint16_t segment, uint16_t offset);
int mvdm_guest_location_copy_to_guest(const mvdm_guest_location *location,
    const uint8_t *bytes, uint32_t byte_count);
#endif
