#include "protected_range_action.h"
#include "adapter-bochs/machine_facade.h"

#include <string.h>

static uint32_t runtime_protected_range_action_v1_lifecycle_active;

extern "C" void runtime_protected_range_action_v1_clear(
  struct runtime_protected_range_action_v1 *action)
{
  if (action == 0) return;
  memset(action, 0, sizeof(*action));
  action->magic = RUNTIME_PROTECTED_RANGE_ACTION_V1_MAGIC;
  action->abi_version = RUNTIME_PROTECTED_RANGE_ACTION_V1_VERSION;
  action->struct_bytes = sizeof(*action);
  action->status = RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_INPUT;
}

extern "C" int runtime_protected_range_action_v1_valid(
  const struct runtime_protected_range_action_v1 *action)
{
  return action != 0 &&
    action->magic == RUNTIME_PROTECTED_RANGE_ACTION_V1_MAGIC &&
    action->abi_version == RUNTIME_PROTECTED_RANGE_ACTION_V1_VERSION &&
    action->struct_bytes == sizeof(*action) &&
    (action->kind == RUNTIME_PROTECTED_RANGE_ACTION_V1_READ ||
     action->kind == RUNTIME_PROTECTED_RANGE_ACTION_V1_WRITE) &&
    action->segment < 6u && action->byte_count != 0u &&
    action->byte_count <= RUNTIME_PROTECTED_RANGE_ACTION_V1_MAX_BYTES;
}

extern "C" void runtime_protected_range_action_v1_set_lifecycle_active(
  uint32_t active)
{
  runtime_protected_range_action_v1_lifecycle_active = active == 1u ? 1u : 0u;
}

extern "C" uint32_t runtime_mantle_execute_protected_range_action_v1(
  struct runtime_protected_range_action_v1 *action)
{
  int status;

  if (action == 0 || !runtime_protected_range_action_v1_valid(action))
    return RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_INPUT;

  action->status = RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_LIFECYCLE;
  if (!runtime_protected_range_action_v1_lifecycle_active) return action->status;

  status = machine_facade_v1_execute_protected_range(action->kind,
    action->segment, action->offset, action->byte_count, action->bytes);
  if (status == MACHINE_FACADE_V1_PROTECTED_RANGE_OK)
    action->status = RUNTIME_PROTECTED_RANGE_ACTION_V1_OK;
  else if (status == MACHINE_FACADE_V1_PROTECTED_RANGE_REJECTED_ACCESS)
    action->status = RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_ACCESS;
  else if (status == MACHINE_FACADE_V1_PROTECTED_RANGE_REJECTED_MEMORY)
    action->status = RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_MEMORY;
  else
    action->status = RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_MODE;
  return action->status;
}
