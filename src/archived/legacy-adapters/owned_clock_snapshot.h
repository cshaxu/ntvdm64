#ifndef OWNED_CLOCK_SNAPSHOT_H
#define OWNED_CLOCK_SNAPSHOT_H

#include <stdint.h>

typedef struct owned_clock_snapshot {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t weekday;
} owned_clock_snapshot;

#endif
