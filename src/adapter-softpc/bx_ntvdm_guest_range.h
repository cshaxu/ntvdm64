#ifndef BX_NTVDM_GUEST_RANGE_H
#define BX_NTVDM_GUEST_RANGE_H

#include <stdint.h>

typedef struct bx_ntvdm_guest_range {
    uint64_t address;
    uint64_t length;
} bx_ntvdm_guest_range;

int bx_ntvdm_guest_range_within(uint64_t aperture_bytes,
    const bx_ntvdm_guest_range *range);

#endif
