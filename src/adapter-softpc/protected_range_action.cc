#include "bochs.h"
#include "bx-core/cpu/cpu.h"
#include "bx-core/memory/memory.h"
#include "bx-mantle/pc_system.h"
#include "protected_range_action.h"

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
  bx_address linear;
  bx_bool access_ok;
  bx_bool copy_ok;

  if (action == 0 || !runtime_protected_range_action_v1_valid(action))
    return RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_INPUT;

  action->status = RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_LIFECYCLE;
  if (!runtime_protected_range_action_v1_lifecycle_active) return action->status;

  action->status = RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_MODE;
  if (!bx_cpu.protected_mode() || (bx_cpu.read_CR0() & 0x80000000u) != 0u)
    return action->status;

  access_ok = action->kind == RUNTIME_PROTECTED_RANGE_ACTION_V1_READ ?
    bx_cpu.read_virtual_checks(&bx_cpu.sregs[action->segment], action->offset,
      action->byte_count) :
    bx_cpu.write_virtual_checks(&bx_cpu.sregs[action->segment], action->offset,
      action->byte_count);
  if (!access_ok) {
    action->status = RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_ACCESS;
    return action->status;
  }

  linear = bx_cpu.get_laddr32(action->segment, action->offset);
  // A20 is part of the native physical-address domain.  This copied action
  // does not emulate wrapping: any span that would address the disabled A20
  // region is declined before ordinary-RAM validation or byte transfer.
  if (!bx_pc_system.get_enable_a20() &&
      ((((Bit64u) linear & 0x100000u) != 0u) ||
       (Bit64u) linear + action->byte_count > 0x100000u)) {
    action->status = RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_MEMORY;
    return action->status;
  }

  copy_ok = action->kind == RUNTIME_PROTECTED_RANGE_ACTION_V1_READ ?
    bx_mem.copy_from_ordinary_ram((bx_phy_address) linear, action->byte_count,
      action->bytes) :
    bx_mem.copy_to_ordinary_ram((bx_phy_address) linear, action->byte_count,
      action->bytes);
  if (!copy_ok) {
    action->status = RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_MEMORY;
    return action->status;
  }

  action->status = RUNTIME_PROTECTED_RANGE_ACTION_V1_OK;
  return action->status;
}
