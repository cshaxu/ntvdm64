#ifndef COMPAT_WOW_SHARED_CLOCK_H
#define COMPAT_WOW_SHARED_CLOCK_H

#include <stdint.h>

#define COMPAT_WOW_CLOCK_PAGE_BYTES 4096u

typedef struct compat_wow_shared_clock {
    volatile uint32_t *page;
    void *stop;
    void *thread;
} compat_wow_shared_clock;

/* Zero-initialized, caller-owned resource. Map page after allocate, before
 * start. Stop/join before removing that mapping; dispose only after unmap.
 * Failed allocation/start uses the same stop/unmap/dispose sequence. */
int compat_wow_shared_clock_allocate(compat_wow_shared_clock *clock);
int compat_wow_shared_clock_start(compat_wow_shared_clock *clock);
void compat_wow_shared_clock_stop(compat_wow_shared_clock *clock);
void compat_wow_shared_clock_dispose(compat_wow_shared_clock *clock);

#endif
