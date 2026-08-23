#ifndef BX_NTVDM_BOP_SHIM_SOFTPC_MOUSE_VECTOR_SHIM_H
#define BX_NTVDM_BOP_SHIM_SOFTPC_MOUSE_VECTOR_SHIM_H

#include <stdint.h>

typedef uint16_t bx_ntvdm_softpc_mouse_word;

/*
 * Fixed-width compatibility seam for the selected headless C8 handoff.
 * It carries copied CS:BX and performs only checked ordinary-RAM transfers;
 * no SAS pointer, Bochs object, host input object, or mouse device crosses
 * this boundary.
 */
int bx_ntvdm_softpc_mouse_vector_begin(uint16_t cs, uint16_t bx);
int bx_ntvdm_softpc_mouse_vector_end(void);
int bx_ntvdm_softpc_mouse_vector_load_table_word(uint16_t byte_offset,
    bx_ntvdm_softpc_mouse_word *value);
int bx_ntvdm_softpc_mouse_vector_store_int33(
    bx_ntvdm_softpc_mouse_word offset,
    bx_ntvdm_softpc_mouse_word segment);

#endif
