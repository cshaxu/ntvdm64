#ifndef BX_NTVDM_A20_CAPABILITY_V1_H
#define BX_NTVDM_A20_CAPABILITY_V1_H

#include <stdint.h>

enum bx_ntvdm_a20_capability_operation_v1 {
  BX_NTVDM_A20_CAPABILITY_QUERY = 1u,
  BX_NTVDM_A20_CAPABILITY_SET = 2u
};

enum bx_ntvdm_a20_capability_status_v1 {
  BX_NTVDM_A20_CAPABILITY_OK = 0u,
  BX_NTVDM_A20_CAPABILITY_REJECTED_LIFECYCLE,
  BX_NTVDM_A20_CAPABILITY_REJECTED_VERSION,
  BX_NTVDM_A20_CAPABILITY_REJECTED_OPERATION,
  BX_NTVDM_A20_CAPABILITY_REJECTED_VALUE
};

#define BX_NTVDM_A20_CAPABILITY_V1_VERSION 1u

struct bx_ntvdm_a20_capability_request_v1 {
  uint32_t version;
  uint32_t operation;
  uint32_t requested_enabled;
};

struct bx_ntvdm_a20_capability_result_v1 {
  uint32_t status;
  uint32_t enabled;
};

/* Minimal-machine lifecycle owner only; this is not a cross-component ABI. */
void bx_ntvdm_a20_capability_v1_set_lifecycle_active(uint32_t active);
void bx_ntvdm_a20_capability_v1_dispatch(
  const struct bx_ntvdm_a20_capability_request_v1 *request,
  struct bx_ntvdm_a20_capability_result_v1 *result);

#endif
