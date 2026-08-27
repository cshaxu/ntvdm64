#ifndef MVDM_XMS_MEMORY_H
#define MVDM_XMS_MEMORY_H

#include <stdint.h>

/* Same-shaped replacements for the selected x86 xmsmem86.c exports.  Their
 * arguments remain original XMS linear numeric addresses, never host pointers. */
int32_t xmsCommitBlock(uint32_t base_address, uint32_t byte_count);
int32_t xmsDecommitBlock(uint32_t base_address, uint32_t byte_count);
void xmsMoveMemory(uint32_t destination, uint32_t source, uint32_t byte_count);

/* Adapter-private result form used only by a matching mvdm-host overlay. */
int mvdm_xms_memory_copy(uint32_t destination, uint32_t source,
    uint32_t byte_count);

#endif
