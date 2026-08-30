#ifndef MVDM_WOW_POINTER_SCOPE_H
#define MVDM_WOW_POINTER_SCOPE_H

#include <stdint.h>

#define MVDM_WOW_POINTER_ACCESS_READ 1u
#define MVDM_WOW_POINTER_ACCESS_WRITE 2u

/* Source-shaped bounded replacement for the original WOW32 GETVDMPTR,
 * FLUSHVDMPTR and FREEVDMPTR macro trio. A returned pointer is only a
 * synchronous bounce lease and cannot be stored or passed to a worker. */
int mvdm_wow_pointer_scope_begin(void);
void *mvdm_wow_pointer_scope_acquire(uint32_t vp, uint32_t byte_count,
    uint32_t access);
int mvdm_wow_pointer_scope_release(void *bytes, int flush);
int mvdm_wow_pointer_scope_end(void);

#endif
