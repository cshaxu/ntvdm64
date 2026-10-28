#ifndef NTVDM_SOFTPC_V7_POINTER_H
#define NTVDM_SOFTPC_V7_POINTER_H
#include <windows.h>
#include <stdint.h>

/* State belongs to one graphics surface, and must be zeroed on replacement.
 * The caller owns locking and must provide a fully reconstructed body. */
typedef struct mvdm_v7_pointer {
    BYTE background[32u * 32u];
    int left, top, right, bottom;
    BOOL visible;
} mvdm_v7_pointer;
BOOL mvdm_v7_pointer_clear(mvdm_v7_pointer *, BYTE *, unsigned, unsigned,
    unsigned, SMALL_RECT *);
BOOL mvdm_v7_pointer_paint(mvdm_v7_pointer *, BYTE *, unsigned, unsigned,
    unsigned, const BYTE *, size_t, unsigned, unsigned, SMALL_RECT *);
#endif
