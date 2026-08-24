#include "bochs.h"
#include "adapter-bochs/pc_system.h"
#include "a20_capability.h"

static bx_bool runtime_a20_capability_lifecycle_active = 0;

void runtime_a20_capability_v1_set_lifecycle_active(uint32_t active)
{
  runtime_a20_capability_lifecycle_active = active == 1u;
}

void runtime_a20_capability_v1_dispatch(
  const struct runtime_a20_capability_request_v1 *request,
  struct runtime_a20_capability_result_v1 *result)
{
  if (result == 0) return;
  result->status = RUNTIME_A20_CAPABILITY_REJECTED_LIFECYCLE;
  result->enabled = 0u;
  if (!runtime_a20_capability_lifecycle_active) return;
  result->enabled = bx_pc_system.get_enable_a20() ? 1u : 0u;
  if (request == 0 || request->version != RUNTIME_A20_CAPABILITY_V1_VERSION) {
    result->status = RUNTIME_A20_CAPABILITY_REJECTED_VERSION;
    return;
  }
  if (request->operation == RUNTIME_A20_CAPABILITY_QUERY) {
    result->status = RUNTIME_A20_CAPABILITY_OK;
    return;
  }
  if (request->operation != RUNTIME_A20_CAPABILITY_SET) {
    result->status = RUNTIME_A20_CAPABILITY_REJECTED_OPERATION;
    return;
  }
  if (request->requested_enabled > 1u) {
    result->status = RUNTIME_A20_CAPABILITY_REJECTED_VALUE;
    return;
  }
  bx_pc_system.set_enable_a20(request->requested_enabled ? 1 : 0);
  result->enabled = bx_pc_system.get_enable_a20() ? 1u : 0u;
  result->status = RUNTIME_A20_CAPABILITY_OK;
}
