#include "bochs.h"
#include "bx-core/cpu/cpu.h"
#include "bx-core/memory/memory.h"
#include "bx_ntvdm_machine_stage_v1.h"
#include "bx_ntvdm_minimal_machine.h"

#include <string.h>

static bx_ntvdm_minimal_machine_c *bx_ntvdm_machine_stage_machine;

static bx_bool bx_ntvdm_machine_stage_preserved_range_valid(
  Bit64u address, Bit64u bytes)
{
  return bytes != 0 && bytes <= 64u && address <= 0x100000u - bytes;
}

extern "C" void bx_ntvdm_machine_stage_v1_request_clear(
  struct bx_ntvdm_machine_stage_v1_request *request)
{
  if (request == 0) return;
  memset(request, 0, sizeof(*request));
  request->magic = BX_NTVDM_MACHINE_STAGE_V1_MAGIC;
  request->abi_version = BX_NTVDM_MACHINE_STAGE_V1_VERSION;
  request->struct_bytes = sizeof(*request);
}

extern "C" int bx_ntvdm_machine_stage_v1_request_valid(
  const struct bx_ntvdm_machine_stage_v1_request *request)
{
  return request != 0 && request->magic == BX_NTVDM_MACHINE_STAGE_V1_MAGIC &&
    request->abi_version == BX_NTVDM_MACHINE_STAGE_V1_VERSION &&
    request->struct_bytes == sizeof(*request) && request->reserved0 == 0u &&
    bx_ntvdm_mechanical_action_v1_valid(&request->initial_state_action) &&
    bx_ntvdm_mechanical_action_v1_valid(&request->startup_action) &&
    bx_ntvdm_machine_stage_preserved_range_valid(
      request->preserved_state_address, request->preserved_state_bytes);
}

extern "C" uint32_t bx_ntvdm_machine_stage_v1_begin(
  const struct bx_ntvdm_machine_stage_v1_request *request)
{
  Bit8u preserved[64];
  struct bx_ntvdm_mechanical_action_v1 initial_state_action;
  struct bx_ntvdm_mechanical_action_v1 startup_action;

  if (!bx_ntvdm_machine_stage_v1_request_valid(request))
    return BX_NTVDM_MACHINE_STAGE_V1_REJECTED_INPUT;
  if (bx_ntvdm_machine_stage_machine != 0)
    return BX_NTVDM_MACHINE_STAGE_V1_REJECTED_ACTIVE;
  bx_ntvdm_machine_stage_machine = new bx_ntvdm_minimal_machine_c;
  if (bx_ntvdm_machine_stage_machine == 0 ||
      bx_ntvdm_machine_stage_machine->initialize(0x100000u, 0x100000u) !=
        BX_NTVDM_MINIMAL_MACHINE_OK) {
    delete bx_ntvdm_machine_stage_machine;
    bx_ntvdm_machine_stage_machine = 0;
    return BX_NTVDM_MACHINE_STAGE_V1_MACHINE_FAILURE;
  }

  initial_state_action = request->initial_state_action;
  startup_action = request->startup_action;
  if (!bx_ntvdm_mantle_execute_mechanical_action_v1(&initial_state_action)) {
    bx_ntvdm_machine_stage_v1_reset();
    return BX_NTVDM_MACHINE_STAGE_V1_ACTION_FAILURE;
  }
  if (!bx_mem.copy_from_ordinary_ram(request->preserved_state_address,
      request->preserved_state_bytes, preserved)) {
    bx_ntvdm_machine_stage_v1_reset();
    return BX_NTVDM_MACHINE_STAGE_V1_PRESERVE_FAILURE;
  }
  if (!bx_ntvdm_mantle_execute_mechanical_action_v1(&startup_action)) {
    bx_ntvdm_machine_stage_v1_reset();
    return BX_NTVDM_MACHINE_STAGE_V1_ACTION_FAILURE;
  }
  if (!bx_mem.copy_to_ordinary_ram(request->preserved_state_address,
      request->preserved_state_bytes, preserved)) {
    bx_ntvdm_machine_stage_v1_reset();
    return BX_NTVDM_MACHINE_STAGE_V1_PRESERVE_FAILURE;
  }
  return BX_NTVDM_MACHINE_STAGE_V1_OK;
}

extern "C" uint32_t bx_ntvdm_machine_stage_v1_reset(void)
{
  bx_ntvdm_minimal_machine_c *machine = bx_ntvdm_machine_stage_machine;
  bx_ntvdm_machine_stage_machine = 0;
  if (machine == 0) return BX_NTVDM_MACHINE_STAGE_V1_OK;
  if (machine->cleanup() != BX_NTVDM_MINIMAL_MACHINE_OK) {
    delete machine;
    return BX_NTVDM_MACHINE_STAGE_V1_CLEANUP_FAILURE;
  }
  delete machine;
  return BX_NTVDM_MACHINE_STAGE_V1_OK;
}

extern "C" int bx_ntvdm_machine_stage_v1_active(void)
{
  return bx_ntvdm_machine_stage_machine != 0;
}

extern "C" void bx_ntvdm_machine_stage_v1_entry_clear(
  struct bx_ntvdm_machine_stage_v1_entry *entry)
{
  if (entry == 0) return;
  memset(entry, 0, sizeof(*entry));
  entry->magic = BX_NTVDM_MACHINE_STAGE_V1_ENTRY_MAGIC;
  entry->abi_version = BX_NTVDM_MACHINE_STAGE_V1_VERSION;
  entry->struct_bytes = sizeof(*entry);
}

extern "C" int bx_ntvdm_machine_stage_v1_entry_valid(
  const struct bx_ntvdm_machine_stage_v1_entry *entry)
{
  return entry != 0 && entry->magic == BX_NTVDM_MACHINE_STAGE_V1_ENTRY_MAGIC &&
    entry->abi_version == BX_NTVDM_MACHINE_STAGE_V1_VERSION &&
    entry->struct_bytes == sizeof(*entry) && entry->reserved0 == 0u &&
    entry->reserved1 == 0u && entry->eip <= 0xffffu;
}

extern "C" uint32_t bx_ntvdm_machine_stage_v1_arm_real_mode_entry(
  const struct bx_ntvdm_machine_stage_v1_entry *entry)
{
  if (bx_ntvdm_machine_stage_machine == 0)
    return BX_NTVDM_MACHINE_STAGE_V1_REJECTED_INACTIVE;
  if (!bx_ntvdm_machine_stage_v1_entry_valid(entry))
    return BX_NTVDM_MACHINE_STAGE_V1_REJECTED_ENTRY;
  bx_cpu.apply_real_mode_entry(entry->cs, entry->eip);
  return BX_NTVDM_MACHINE_STAGE_V1_OK;
}

extern "C" uint32_t bx_ntvdm_machine_stage_v1_copy_real_mode_entry(
  struct bx_ntvdm_machine_stage_v1_entry *entry)
{
  if (bx_ntvdm_machine_stage_machine == 0 || entry == 0)
    return BX_NTVDM_MACHINE_STAGE_V1_REJECTED_INACTIVE;
  bx_ntvdm_machine_stage_v1_entry_clear(entry);
  entry->cs = bx_cpu.sregs[BX_SEG_REG_CS].selector.value;
  entry->eip = bx_cpu.get_eip();
  return BX_NTVDM_MACHINE_STAGE_V1_OK;
}
