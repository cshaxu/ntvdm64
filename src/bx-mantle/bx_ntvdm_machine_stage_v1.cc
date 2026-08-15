#include "bochs.h"
#include "bx-core/cpu/cpu.h"
#include "bx-core/memory/memory.h"
#include "bx-mantle/pc_system.h"
#include "bx_ntvdm_generic_ud_bridge.h"
#include "bx_ntvdm_cancellation_controller_v1.h"
#include "bx_ntvdm_machine_stage_v1.h"
#include "bx_ntvdm_minimal_machine.h"

#include <string.h>

static bx_ntvdm_minimal_machine_c *bx_ntvdm_machine_stage_machine;

struct bx_ntvdm_machine_stage_v1_stop_state {
  bx_bool watchdog_fired;
  bx_bool cancellation_fired;
};

static const Bit64u bx_ntvdm_machine_stage_v1_cancellation_poll_ticks = 1024u;

static void bx_ntvdm_machine_stage_v1_stop(void *opaque)
{
  bx_ntvdm_machine_stage_v1_stop_state *state =
    (bx_ntvdm_machine_stage_v1_stop_state *) opaque;
  state->watchdog_fired = 1;
  bx_pc_system.kill_bochs_request = 1;
}

static void bx_ntvdm_machine_stage_v1_cancellation_poll(void *opaque)
{
  bx_ntvdm_machine_stage_v1_stop_state *state =
    (bx_ntvdm_machine_stage_v1_stop_state *) opaque;
#if defined(BX_NTVDM_CANCELLATION_TESTING)
  bx_ntvdm_cancellation_controller_v1_test_poll_mark();
#endif
  if (bx_ntvdm_cancellation_controller_v1_requested_reason() ==
      BX_NTVDM_CANCELLATION_V1_NONE) return;
  state->cancellation_fired = 1;
  bx_pc_system.kill_bochs_request = 1;
}

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

extern "C" void bx_ntvdm_machine_stage_v1_execution_request_clear(
  struct bx_ntvdm_machine_stage_v1_execution_request *request)
{
  if (request == 0) return;
  memset(request, 0, sizeof(*request));
  request->magic = BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_MAGIC;
  request->abi_version = BX_NTVDM_MACHINE_STAGE_V1_VERSION;
  request->struct_bytes = sizeof(*request);
}

extern "C" int bx_ntvdm_machine_stage_v1_execution_request_valid(
  const struct bx_ntvdm_machine_stage_v1_execution_request *request)
{
  return request != 0 &&
    request->magic == BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_MAGIC &&
    request->abi_version == BX_NTVDM_MACHINE_STAGE_V1_VERSION &&
    request->struct_bytes == sizeof(*request) && request->ips != 0u &&
    request->instruction_tick_budget != 0u;
}

extern "C" uint32_t bx_ntvdm_machine_stage_v1_execute(
  const struct bx_ntvdm_machine_stage_v1_execution_request *request)
{
  bx_ntvdm_machine_stage_v1_stop_state stop_state;
  int stop_timer, cancellation_timer;

  if (bx_ntvdm_machine_stage_machine == 0)
    return BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_REJECTED_INACTIVE;
  if (!bx_ntvdm_machine_stage_v1_execution_request_valid(request))
    return BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_REJECTED_INPUT;
  if (bx_ntvdm_cancellation_controller_v1_requested_reason() !=
      BX_NTVDM_CANCELLATION_V1_NONE)
    return BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_HOST_CANCELLATION;
  bx_pc_system.initialize(request->ips);
  stop_state.watchdog_fired = 0;
  stop_state.cancellation_fired = 0;
  stop_timer = bx_pc_system.register_timer_ticks(&stop_state,
    bx_ntvdm_machine_stage_v1_stop, request->instruction_tick_budget, 0, 1,
    "machine-stage-stop");
  if (stop_timer <= 0)
    return BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_TIMER_FAILURE;
  cancellation_timer = bx_pc_system.register_timer_ticks(&stop_state,
    bx_ntvdm_machine_stage_v1_cancellation_poll,
    bx_ntvdm_machine_stage_v1_cancellation_poll_ticks, 1, 1,
    "machine-stage-cancel");
  if (cancellation_timer <= 0) {
    bx_pc_system.deactivate_timer((unsigned) stop_timer);
    bx_pc_system.unregisterTimer((unsigned) stop_timer);
    return BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_TIMER_FAILURE;
  }
  bx_ntvdm_mantle_generic_ud_stop_observation_reset();
  bx_cpu.cpu_loop();
  bx_pc_system.deactivate_timer((unsigned) cancellation_timer);
  bx_pc_system.unregisterTimer((unsigned) cancellation_timer);
  bx_pc_system.deactivate_timer((unsigned) stop_timer);
  bx_pc_system.unregisterTimer((unsigned) stop_timer);
  if (bx_ntvdm_mantle_generic_ud_stop_observed())
    return BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_CONTROLLED_STOP;
  if (stop_state.cancellation_fired)
    return BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_HOST_CANCELLATION;
  return stop_state.watchdog_fired ? BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_BUDGET :
    BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_UNEXPECTED_LOOP_RETURN;
}
