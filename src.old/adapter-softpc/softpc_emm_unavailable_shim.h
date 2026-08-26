#ifndef RUNTIME_BOP_SHIM_SOFTPC_EMM_UNAVAILABLE_SHIM_H
#define RUNTIME_BOP_SHIM_SOFTPC_EMM_UNAVAILABLE_SHIM_H

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
int runtime_softpc_emm_unavailable_begin(uint16_t ds, uint16_t dx,
    uint16_t *result_bx);
int runtime_softpc_emm_unavailable_end(uint16_t *result_bx);
word runtime_softpc_emm_get_ds(void);
word runtime_softpc_emm_get_dx(void);
short runtime_softpc_emm_get_total_pages(void);
void runtime_softpc_emm_set_bx(word value);
void runtime_softpc_emm_dma_lim_setup(void);

#define getDS runtime_softpc_emm_get_ds
#define getDX runtime_softpc_emm_get_dx
#define get_total_pages runtime_softpc_emm_get_total_pages
#define setBX runtime_softpc_emm_set_bx
#define dma_lim_setup runtime_softpc_emm_dma_lim_setup

#endif
