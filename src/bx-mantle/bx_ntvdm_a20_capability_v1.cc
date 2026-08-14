#include "bochs.h"
#include "bx-mantle/pc_system.h"
#include "bx_ntvdm_a20_capability_v1.h"

static bx_bool bx_ntvdm_a20_capability_lifecycle_active = 0;

void bx_ntvdm_a20_capability_v1_set_lifecycle_active(uint32_t active)
{
  bx_ntvdm_a20_capability_lifecycle_active = active == 1u;
}

void bx_ntvdm_a20_capability_v1_dispatch(
  const struct bx_ntvdm_a20_capability_request_v1 *request,
  struct bx_ntvdm_a20_capability_result_v1 *result)
{
  if (result == 0) return;
  result->status = BX_NTVDM_A20_CAPABILITY_REJECTED_LIFECYCLE;
  result->enabled = 0u;
  if (!bx_ntvdm_a20_capability_lifecycle_active) return;
  result->enabled = bx_pc_system.get_enable_a20() ? 1u : 0u;
  if (request == 0 || request->version != BX_NTVDM_A20_CAPABILITY_V1_VERSION) {
    result->status = BX_NTVDM_A20_CAPABILITY_REJECTED_VERSION;
    return;
  }
  if (request->operation == BX_NTVDM_A20_CAPABILITY_QUERY) {
    result->status = BX_NTVDM_A20_CAPABILITY_OK;
    return;
  }
  if (request->operation != BX_NTVDM_A20_CAPABILITY_SET) {
    result->status = BX_NTVDM_A20_CAPABILITY_REJECTED_OPERATION;
    return;
  }
  if (request->requested_enabled > 1u) {
    result->status = BX_NTVDM_A20_CAPABILITY_REJECTED_VALUE;
    return;
  }
  bx_pc_system.set_enable_a20(request->requested_enabled ? 1 : 0);
  result->enabled = bx_pc_system.get_enable_a20() ? 1u : 0u;
  result->status = BX_NTVDM_A20_CAPABILITY_OK;
}
