/////////////////////////////////////////////////////////////////////////
//
// Default-off, fixed-width first non-UD exception observation owned by the
// native mantle.  It is a private diagnostic mechanic, not an adapter ABI.
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_NTVDM_FIRST_FAULT_OBSERVATION_V1_H
#define BX_NTVDM_FIRST_FAULT_OBSERVATION_V1_H

#include <stdint.h>

#define BX_NTVDM_FIRST_FAULT_OBSERVATION_V1_MAGIC UINT32_C(0x42584631)
#define BX_NTVDM_FIRST_FAULT_OBSERVATION_V1_VERSION UINT32_C(1)

struct bx_ntvdm_first_fault_observation_v1 {
  uint32_t magic, abi_version, struct_bytes, cpu_id;
  uint32_t vector, error_code, execution_mode, reserved0;
  uint64_t fault_rip;
  uint32_t eax, ebx, ecx, edx, esi, edi, ebp, esp, eip, eflags;
  uint16_t cs, ds, es, ss, fs, gs;
};

#ifdef __cplusplus
extern "C" {
#endif

/* Returns nonzero only when the private first-fault diagnostic accepted this
 * copied non-UD event and requests the pre-existing mechanical stop transfer. */
int bx_ntvdm_mantle_first_fault_observation_v1(
  const struct bx_ntvdm_first_fault_observation_v1 *event);
void bx_ntvdm_mantle_first_fault_observation_reset(void);
void bx_ntvdm_mantle_first_fault_observation_enable(int enabled);
int bx_ntvdm_mantle_first_fault_observation_enabled(void);
void bx_ntvdm_mantle_first_fault_observation_fixture_stop(int enabled);
int bx_ntvdm_mantle_first_fault_observation_observed(void);
int bx_ntvdm_mantle_first_fault_observation_copy(
  struct bx_ntvdm_first_fault_observation_v1 *event);

#ifdef __cplusplus
}
#endif

#endif