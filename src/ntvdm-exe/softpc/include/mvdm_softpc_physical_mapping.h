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

/* Temporary scalar-only observation. EMS request/result slots correspond
 * to service numbers 40h..5fh. Not a mapping or dispatch interface. */
enum mvdm_mapping_observation_slot {
    MVDM_MAPPING_CONFIG, MVDM_MAPPING_EMS_INIT,
    MVDM_MAPPING_DPMI, MVDM_MAPPING_WOW,
    MVDM_MAPPING_MAIN_RETURN, MVDM_MAPPING_TERMINATION,
    MVDM_MAPPING_EXCEPTION, MVDM_MAPPING_APP_RETURN,
    MVDM_MAPPING_WOW_FAILURE, MVDM_MAPPING_WOW_READY,
    MVDM_MAPPING_EMS_REQUEST = 32, MVDM_MAPPING_EMS_RESULT = 64,
    MVDM_MAPPING_EMS_LEASE_LOAD = 72,
    MVDM_MAPPING_EMS_LEASE_LOAD_RESULT,
    MVDM_MAPPING_EMS_LEASE_STORE,
    MVDM_MAPPING_EMS_LEASE_STORE_RESULT,
    MVDM_MAPPING_EMS_LEASE_MOVE,
    MVDM_MAPPING_EMS_LEASE_MOVE_RESULT
};
void mvdm_softpc_mapping_observe(unsigned slot, const char *event,
    uint32_t a, uint32_t b, uint32_t c);

#endif
