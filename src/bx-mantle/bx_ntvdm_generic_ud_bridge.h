/////////////////////////////////////////////////////////////////////////
//
// Fixed, synchronous CPU-to-mantle #UD mechanics record.
// No service, guest-memory, or host-capability meaning belongs here.
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_NTVDM_GENERIC_UD_BRIDGE_H
#define BX_NTVDM_GENERIC_UD_BRIDGE_H

#include <stdint.h>

#define BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC 0x42585544u
#define BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION 1u
#define BX_NTVDM_GENERIC_UD_WINDOW_BYTES 15u
#define BX_NTVDM_GENERIC_UD_GPR16_COUNT 8u

enum bx_ntvdm_generic_ud_disposition_v1 {
  BX_NTVDM_GENERIC_UD_PASS_THROUGH = 0u,
  BX_NTVDM_GENERIC_UD_RESUME = 1u,
  BX_NTVDM_GENERIC_UD_STOP = 2u
};

struct bx_ntvdm_generic_ud_event_v1 {
  uint32_t magic, abi_version, struct_bytes, cpu_id;
  uint32_t vector, error_code, execution_mode, reserved0;
  uint64_t fault_rip;
  uint32_t eax, ebx, ecx, edx, esi, edi, ebp, esp, eip, eflags;
  uint16_t cs, ds, es, ss, fs, gs;
  uint32_t window_bytes;
  uint8_t window[BX_NTVDM_GENERIC_UD_WINDOW_BYTES];
  uint8_t reserved1;
};

struct bx_ntvdm_generic_ud_outcome_v1 {
  uint32_t abi_version, disposition;
  uint64_t resume_rip;
  uint32_t gpr16_write_mask;
  uint16_t gpr16_values[BX_NTVDM_GENERIC_UD_GPR16_COUNT];
  uint32_t eflags_write_mask, eflags_values;
};

#ifdef __cplusplus
extern "C" {
#endif

/* The bridge is synchronous; neither argument nor any nested data may be
 * retained. Return zero for a decline. */
int bx_ntvdm_mantle_generic_ud_bridge_v1(
  const struct bx_ntvdm_generic_ud_event_v1 *event,
  struct bx_ntvdm_generic_ud_outcome_v1 *outcome);

#ifdef __cplusplus
}
#endif

#endif
