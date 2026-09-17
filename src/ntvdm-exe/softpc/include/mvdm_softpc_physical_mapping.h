#ifndef MVDM_SOFTPC_PHYSICAL_MAPPING_H
#define MVDM_SOFTPC_PHYSICAL_MAPPING_H

#include <stdint.h>

/* Source-shaped replacement for the NT4 PhysicalPageREC remap seam. The
 * original Win32/x86 ABI carries its process-local HostAddress directly. */
int mvdm_softpc_physical_mapping_initialize(void *normal_base, uint32_t size);
void mvdm_softpc_physical_mapping_release(void);
/* Keep the historical external seam spelling and fixed-width argument ABI.
 * The selected SoftPC host calls it after its original SAS reservation; the
 * adapter binds or retires the session-owned physical-page mapping. */
void VdmSetPhysRecStructs(uint32_t host_address, uint32_t intel_address,
    uint32_t byte_count);
/* Source-shaped replacement for the NT4 kernel-VDM EMS page alias services.
 * These page numbers always name guest physical 4 KiB pages; they are not
 * host pointers and therefore do not use an identity service. */
int32_t VdmMapDosMemory(uint32_t dos_intel_page, uint32_t vdm_intel_page,
    uint32_t page_count);
int32_t VdmUnmapDosMemory(uint32_t dos_intel_page, uint32_t page_count);
int mvdm_softpc_physical_mapping_translate(uint32_t intel_address,
    uint32_t *translated_address_out);
int mvdm_softpc_physical_mapping_resolve(uint32_t intel_address,
    uint8_t **host_byte_out);
/* The original nt_emm fast path can use a single native pointer only when
 * no byte in its span enters a session-owned EMS alias. */
int mvdm_softpc_physical_mapping_span_is_aliased(uint32_t intel_address,
    uint32_t byte_count);


#endif
