/////////////////////////////////////////////////////////////////////////
//
// Process-local minimal-machine preentry stage.  This C ABI transports only
// copied mechanical actions; Bochs object lifetime remains private to mantle.
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_NTVDM_MACHINE_STAGE_V1_H
#define BX_NTVDM_MACHINE_STAGE_V1_H

#include <stdint.h>

#include "bx_ntvdm_mechanical_action_v1.h"

#define BX_NTVDM_MACHINE_STAGE_V1_MAGIC UINT32_C(0x42584d53)
#define BX_NTVDM_MACHINE_STAGE_V1_VERSION UINT32_C(1)

enum bx_ntvdm_machine_stage_v1_status {
  BX_NTVDM_MACHINE_STAGE_V1_OK = 0u,
  BX_NTVDM_MACHINE_STAGE_V1_REJECTED_INPUT,
  BX_NTVDM_MACHINE_STAGE_V1_REJECTED_ACTIVE,
  BX_NTVDM_MACHINE_STAGE_V1_MACHINE_FAILURE,
  BX_NTVDM_MACHINE_STAGE_V1_ACTION_FAILURE,
  BX_NTVDM_MACHINE_STAGE_V1_PRESERVE_FAILURE,
  BX_NTVDM_MACHINE_STAGE_V1_CLEANUP_FAILURE
};

struct bx_ntvdm_machine_stage_v1_request {
  uint32_t magic;
  uint32_t abi_version;
  uint32_t struct_bytes;
  uint32_t reserved0;
  struct bx_ntvdm_mechanical_action_v1 initial_state_action;
  struct bx_ntvdm_mechanical_action_v1 startup_action;
  uint64_t preserved_state_address;
  uint64_t preserved_state_bytes;
};

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_machine_stage_v1_request_clear(
  struct bx_ntvdm_machine_stage_v1_request *request);
int bx_ntvdm_machine_stage_v1_request_valid(
  const struct bx_ntvdm_machine_stage_v1_request *request);
/* Begins one process-local machine stage and retains only the initialized
 * mantle machine.  Inputs are consumed during the call and never retained. */
uint32_t bx_ntvdm_machine_stage_v1_begin(
  const struct bx_ntvdm_machine_stage_v1_request *request);
/* Releases the retained machine; no Bochs object crosses this ABI. */
uint32_t bx_ntvdm_machine_stage_v1_reset(void);
int bx_ntvdm_machine_stage_v1_active(void);

#ifdef __cplusplus
}
#endif

#endif
