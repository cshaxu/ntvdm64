#ifndef BX_NTVDM_BOP_SHIM_SOFTPC_EMM_UNAVAILABLE_SHIM_H
#define BX_NTVDM_BOP_SHIM_SOFTPC_EMM_UNAVAILABLE_SHIM_H

#include <stdint.h>

typedef uint16_t word;

/*
 * Compatibility vocabulary for the directly mirrored `emm_init` body.
 * Historical SoftPC obtains LIM configuration, allocation state and the
 * segment68/offset68 globals from its product shell.  The admitted profile
 * deliberately has no EMS device or PIF configuration, so this shim exposes
 * the exact no-pages result only.  It carries copied register values and no
 * guest pointer, Bochs object, or persistent host handle.
 */
int bx_ntvdm_softpc_emm_unavailable_begin(uint16_t ds, uint16_t dx,
    uint16_t *result_bx);
int bx_ntvdm_softpc_emm_unavailable_end(uint16_t *result_bx);
word bx_ntvdm_softpc_emm_get_ds(void);
word bx_ntvdm_softpc_emm_get_dx(void);
short bx_ntvdm_softpc_emm_get_total_pages(void);
void bx_ntvdm_softpc_emm_set_bx(word value);
void bx_ntvdm_softpc_emm_dma_lim_setup(void);

#define getDS bx_ntvdm_softpc_emm_get_ds
#define getDX bx_ntvdm_softpc_emm_get_dx
#define get_total_pages bx_ntvdm_softpc_emm_get_total_pages
#define setBX bx_ntvdm_softpc_emm_set_bx
#define dma_lim_setup bx_ntvdm_softpc_emm_dma_lim_setup

#endif
