#ifndef RUNTIME_GUEST_RANGE_H
#define RUNTIME_GUEST_RANGE_H

#include <stdint.h>

typedef struct runtime_guest_range {
    uint64_t address;
    uint64_t length;
} runtime_guest_range;

int runtime_guest_range_within(uint64_t aperture_bytes,
    const runtime_guest_range *range);

#endif
