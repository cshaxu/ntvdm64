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
#define BX_NTVDM_GENERIC_UD_GPR32_COUNT 8u
#define BX_NTVDM_GENERIC_UD_OUTCOME_V2_VERSION 2u
/* Native CPU support mask, copied here as an ABI restriction rather than an
 * invitation to expose a CPU object or arbitrary FLAGS state. */
#define BX_NTVDM_GENERIC_UD_EFLAGS_WRITE_MASK 0x003f7fd5u

enum bx_ntvdm_generic_ud_disposition_v1 {
  BX_NTVDM_GENERIC_UD_PASS_THROUGH = 0u,
  BX_NTVDM_GENERIC_UD_RESUME = 1u,
  BX_NTVDM_GENERIC_UD_STOP = 2u,
  /* Mechanical retained-state suspension.  No provider meaning crosses
   * this record. */
  BX_NTVDM_GENERIC_UD_PENDING = 3u
};

/* A requested architectural execution mode.  This deliberately has no CR0
 * payload: the core retains all native CR0 validation and only changes PE. */
enum bx_ntvdm_generic_ud_context_mode_v2 {
  BX_NTVDM_GENERIC_UD_CONTEXT_UNCHANGED = 0u,
  BX_NTVDM_GENERIC_UD_CONTEXT_REAL = 1u,
  BX_NTVDM_GENERIC_UD_CONTEXT_PROTECTED = 2u
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
  uint32_t segment_write_mask;
  uint16_t segment_values[6];
  uint32_t eflags_write_mask, eflags_values;
};

/* Fixed-width generic context result.  It carries copied architectural
 * scalars only; it has no selector, service, memory, provider, or object
 * identity.  A zero write mask retains the corresponding native state. */
struct bx_ntvdm_generic_ud_outcome_v2 {
  uint32_t abi_version, disposition;
  uint64_t resume_rip;
  uint32_t gpr32_write_mask;
  uint32_t gpr32_values[BX_NTVDM_GENERIC_UD_GPR32_COUNT];
  uint32_t segment_write_mask;
  uint16_t segment_values[6];
  uint32_t eflags_write_mask, eflags_values;
  uint32_t context_mode, reserved0;
};

#ifdef __cplusplus
extern "C" {
#endif

/* The bridge is synchronous; neither argument nor any nested data may be
 * retained. Return zero for a decline. */
int bx_ntvdm_mantle_generic_ud_bridge_v1(
  const struct bx_ntvdm_generic_ud_event_v1 *event,
  struct bx_ntvdm_generic_ud_outcome_v1 *outcome);

/* v2 is tried before v1.  A zero return declines and leaves v1/native
 * exception handling unchanged. */
int bx_ntvdm_mantle_generic_ud_bridge_v2(
  const struct bx_ntvdm_generic_ud_event_v1 *event,
  struct bx_ntvdm_generic_ud_outcome_v2 *outcome);

/* Generic-machine stop observation. The CPU invokes this only after it has
 * accepted a typed STOP outcome; no selector, provider, or guest data is
 * carried here. Finite-run uses it solely to distinguish the stop from its
 * watchdog timer. */
void bx_ntvdm_mantle_generic_ud_stop_observation_reset(void);
void bx_ntvdm_mantle_generic_ud_stop_observation_mark(void);
int bx_ntvdm_mantle_generic_ud_stop_observed(void);
void bx_ntvdm_mantle_generic_ud_pending_observation_reset(void);
void bx_ntvdm_mantle_generic_ud_pending_observation_mark(void);
int bx_ntvdm_mantle_generic_ud_pending_observed(void);

#ifdef __cplusplus
}
#endif

#endif
