#ifndef RUNTIME_SOFTPC_MEMORY_SIZE_SHIM_H
#define RUNTIME_SOFTPC_MEMORY_SIZE_SHIM_H

#include <stdint.h>

/* SoftPC's bios.h defines MEMORY_VAR as BIOS_VAR_START + 0x13: BDA 0040:0013.
 * Keep this address in bx-vdm, where its BIOS/BOP meaning belongs. */
#define RUNTIME_SOFTPC_MEMORY_VAR 0x413u

/* Minimal compatibility vocabulary for the directly mirrored SoftPC body. */
typedef uint16_t word;

int runtime_softpc_memory_size_begin(uint16_t *result_ax);
int runtime_softpc_memory_size_end(uint16_t *result_ax);

/* DIVERGENCE(SOFTPC-DIV-001): SAS and CPU names are process-global in historical SoftPC and
 * collide with independently mirrored OpenNT modules.  The source body keeps
 * its original spellings; these private preprocessor aliases give this mirror
 * unique modern link symbols without changing its call order or data flow. */
void runtime_softpc_memory_size_sas_loadw(uint32_t address, word *value);
void runtime_softpc_memory_size_setAX(word value);
#define sas_loadw runtime_softpc_memory_size_sas_loadw
#define setAX runtime_softpc_memory_size_setAX

#endif
