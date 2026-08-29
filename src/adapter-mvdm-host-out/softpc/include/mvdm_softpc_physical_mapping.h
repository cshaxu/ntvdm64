#ifndef MVDM_SOFTPC_PHYSICAL_MAPPING_H
#define MVDM_SOFTPC_PHYSICAL_MAPPING_H

#include <stdint.h>

/* Source-shaped replacement for the NT4 PhysicalPageREC remap seam.  The
 * original VdmAddVirtualMemory ABI carries a 32-bit HostAddress.  This facade
 * carries a session-local surrogate in that field; native pointer identity is
 * never copied into an MVDM or guest ABI. */
int mvdm_softpc_physical_mapping_publish(void *host_bytes,
    uint32_t byte_count, uint32_t *identifier_out);
int mvdm_softpc_physical_mapping_prepare(uint32_t identifier,
    uint32_t byte_count, uint32_t *alignment_out);
/* Keep the historical external seam spelling and fixed-width argument ABI.
 * The selected SoftPC host calls it after its original SAS reservation; the
 * adapter binds or retires the session-owned physical-page mapping. */
void VdmSetPhysRecStructs(uint32_t host_address, uint32_t intel_address,
    uint32_t byte_count);
void mvdm_softpc_physical_mapping_set(uint32_t identifier,
    uint32_t intel_address, uint32_t byte_count);
/* Source-shaped replacement for the NT4 kernel-VDM EMS page alias services.
 * These page numbers always name guest physical 4 KiB pages; they are not
 * host pointers and therefore do not enter the mapping manager. */
int32_t VdmMapDosMemory(uint32_t dos_intel_page, uint32_t vdm_intel_page,
    uint32_t page_count);
int32_t VdmUnmapDosMemory(uint32_t dos_intel_page, uint32_t page_count);
int mvdm_softpc_physical_mapping_translate(uint32_t intel_address,
    uint32_t *translated_address_out);
int mvdm_softpc_physical_mapping_resolve(uint32_t intel_address,
    uint8_t **host_byte_out);
void mvdm_softpc_physical_mapping_cancel(uint32_t identifier);

#endif
