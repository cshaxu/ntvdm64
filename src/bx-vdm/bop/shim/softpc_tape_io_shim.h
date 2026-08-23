#ifndef BX_NTVDM_SOFTPC_TAPE_IO_SHIM_H
#define BX_NTVDM_SOFTPC_TAPE_IO_SHIM_H

#include <stdint.h>

/* Minimal compatibility vocabulary for the directly mirrored non-PM branch
 * of SoftPC's tape_io.c. */
typedef uint16_t half_word;

#define INT15_EMS_DETERMINE 0x88u

int bx_ntvdm_softpc_tape_io_begin(uint16_t input_ax, uint16_t *result_ax);
int bx_ntvdm_softpc_tape_io_end(uint16_t *result_ax);

/* DIVERGENCE: historical CPU register macros are process-global.  The mirror
 * keeps its original call spellings while these aliases bind them to a
 * synchronous bx-vdm call record with no retained guest pointer or host
 * handle. */
half_word bx_ntvdm_softpc_tape_io_getAH(void);
void bx_ntvdm_softpc_tape_io_setAX(half_word value);
#define getAH bx_ntvdm_softpc_tape_io_getAH
#define setAX bx_ntvdm_softpc_tape_io_setAX

#endif
