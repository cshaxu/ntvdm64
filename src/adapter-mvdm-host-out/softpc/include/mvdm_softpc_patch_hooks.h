#ifndef MVDM_SOFTPC_PATCH_HOOKS_H
#define MVDM_SOFTPC_PATCH_HOOKS_H

#include <stdint.h>

/* DEFERRED EXPERIMENT MVDM-SOFTPC-PATCH-002:
 * This declaration surface intentionally has no active production consumer.
 * See the disabled source body for the required CPU-frame/PhysicalPageREC/BOP
 * recovery preconditions. */
#if 0
/*
 * Same-shaped backing for the four reached NTVDMx64 fmstubs imports.
 *
 * These are not guest ABI declarations: their uint32_t values remain original
 * SoftPC numeric inputs.  Any native address that they need is confined to a
 * session-owned mapping or a bounded guest-memory lease.
 */
void mvdm_softpc_patch_clear_instance_data(void);
void mvdm_softpc_patch_set_physical_records(uint32_t host_identity,
    uint32_t intel_address, uint32_t byte_count);
void mvdm_softpc_patch_fast_bop(uint32_t immediate);
uint32_t mvdm_softpc_patch_virtualise_instruction(uint32_t eip_in_rom,
    uint32_t byte_count, uint32_t linear_address_or_port, uint32_t data_in);
uint8_t *mvdm_softpc_patch_touch(uint32_t address, uint32_t byte_count);
#endif

#endif
