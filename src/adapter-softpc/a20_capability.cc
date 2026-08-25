#include "a20_capability.h"
#include "adapter-bochs/machine_facade.h"

static uint32_t runtime_a20_capability_lifecycle_active;

void runtime_a20_capability_set_lifecycle_active(uint32_t active)
{
  runtime_a20_capability_lifecycle_active = active == 1u ? 1u : 0u;
}

void runtime_a20_capability_dispatch(
  const struct runtime_a20_capability_request *request,
  struct runtime_a20_capability_result *result)
{
  if (result == 0) return;
  result->status = RUNTIME_A20_CAPABILITY_REJECTED_LIFECYCLE;
  result->enabled = 0u;
  if (!runtime_a20_capability_lifecycle_active) return;
  if (!machine_facade_get_a20(&result->enabled)) return;
  if (request == 0 || request->version != RUNTIME_A20_CAPABILITY_VERSION) {
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
  if (!machine_facade_set_a20(request->requested_enabled) ||
      !machine_facade_get_a20(&result->enabled)) return;
  result->status = RUNTIME_A20_CAPABILITY_OK;
}
