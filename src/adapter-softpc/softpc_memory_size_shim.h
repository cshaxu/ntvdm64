#ifndef BX_NTVDM_SOFTPC_MEMORY_SIZE_SHIM_H
#define BX_NTVDM_SOFTPC_MEMORY_SIZE_SHIM_H

#include <stdint.h>

/* SoftPC's bios.h defines MEMORY_VAR as BIOS_VAR_START + 0x13: BDA 0040:0013.
 * Keep this address in bx-vdm, where its BIOS/BOP meaning belongs. */
#define BX_NTVDM_SOFTPC_MEMORY_VAR 0x413u

/* Minimal compatibility vocabulary for the directly mirrored SoftPC body. */
typedef uint16_t word;

int bx_ntvdm_softpc_memory_size_begin(uint16_t *result_ax);
int bx_ntvdm_softpc_memory_size_end(uint16_t *result_ax);

/* DIVERGENCE: SAS and CPU names are process-global in historical SoftPC and
 * collide with independently mirrored OpenNT modules.  The source body keeps
 * its original spellings; these private preprocessor aliases give this mirror
 * unique modern link symbols without changing its call order or data flow. */
void bx_ntvdm_softpc_memory_size_sas_loadw(uint32_t address, word *value);
void bx_ntvdm_softpc_memory_size_setAX(word value);
#define sas_loadw bx_ntvdm_softpc_memory_size_sas_loadw
#define setAX bx_ntvdm_softpc_memory_size_setAX

#endif
