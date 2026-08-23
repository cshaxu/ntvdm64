#include "bochs.h"
#include "bx-core/cpu/cpu.h"
#include "bx-core/memory/memory.h"
#include "bx-mantle/pc_system.h"
#include "bx_ntvdm_generic_ud_bridge.h"
#include "bx_ntvdm_first_fault_observation_v1.h"
#include "bx_ntvdm_cancellation_controller_v1.h"
#include "bx_ntvdm_physical_irq_v1.h"
#include "bx_ntvdm_machine_stage_v1.h"
#include "bx_ntvdm_ivt_watch_v1.h"
#include "bx_ntvdm_minimal_machine.h"
#include "bx_ntvdm_instruction_history.h"

#include <string.h>

#ifndef BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY
#define BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY 0
#endif

#ifndef BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
#define BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE 0
#endif

static bx_ntvdm_minimal_machine_c *bx_ntvdm_machine_stage_machine;
static uint32_t bx_ntvdm_machine_stage_v1_terminal_position_enabled;
static struct bx_ntvdm_machine_stage_v1_terminal_position
  bx_ntvdm_machine_stage_v1_terminal_position;
static uint32_t bx_ntvdm_machine_stage_v1_terminal_history_enabled;
static struct bx_ntvdm_machine_stage_v1_terminal_history
  bx_ntvdm_machine_stage_v1_terminal_history;
static uint32_t bx_ntvdm_machine_stage_v1_terminal_cs_transitions_enabled;
static struct bx_ntvdm_machine_stage_v1_terminal_cs_transitions
  bx_ntvdm_machine_stage_v1_terminal_cs_transitions;
static uint32_t bx_ntvdm_machine_stage_v1_terminal_provenance_enabled;
static struct bx_ntvdm_machine_stage_v1_terminal_provenance
  bx_ntvdm_machine_stage_v1_terminal_provenance;
static uint32_t bx_ntvdm_machine_stage_v1_terminal_cs_provenance_enabled;
static struct bx_ntvdm_machine_stage_v1_terminal_cs_provenance
  bx_ntvdm_machine_stage_v1_terminal_cs_provenance;

static void bx_ntvdm_machine_stage_v1_terminal_position_clear(void)
{
  memset(&bx_ntvdm_machine_stage_v1_terminal_position, 0,
    sizeof(bx_ntvdm_machine_stage_v1_terminal_position));
  bx_ntvdm_machine_stage_v1_terminal_position.magic =
    BX_NTVDM_MACHINE_STAGE_V1_TERMINAL_POSITION_MAGIC;
  bx_ntvdm_machine_stage_v1_terminal_position.abi_version =
    BX_NTVDM_MACHINE_STAGE_V1_TERMINAL_POSITION_VERSION;
  bx_ntvdm_machine_stage_v1_terminal_position.struct_bytes =
    sizeof(bx_ntvdm_machine_stage_v1_terminal_position);
}

static void bx_ntvdm_machine_stage_v1_terminal_history_clear(void)
{
  memset(&bx_ntvdm_machine_stage_v1_terminal_history, 0,
    sizeof(bx_ntvdm_machine_stage_v1_terminal_history));
  bx_ntvdm_machine_stage_v1_terminal_history.magic =
    BX_NTVDM_MACHINE_STAGE_V1_TERMINAL_HISTORY_MAGIC;
  bx_ntvdm_machine_stage_v1_terminal_history.abi_version =
    BX_NTVDM_MACHINE_STAGE_V1_TERMINAL_HISTORY_VERSION;
  bx_ntvdm_machine_stage_v1_terminal_history.struct_bytes =
    sizeof(bx_ntvdm_machine_stage_v1_terminal_history);
}

static void bx_ntvdm_machine_stage_v1_terminal_cs_transitions_clear(void)
{
  memset(&bx_ntvdm_machine_stage_v1_terminal_cs_transitions, 0,
    sizeof(bx_ntvdm_machine_stage_v1_terminal_cs_transitions));
  bx_ntvdm_machine_stage_v1_terminal_cs_transitions.magic =
    BX_NTVDM_MACHINE_STAGE_V1_TERMINAL_CS_TRANSITIONS_MAGIC;
  bx_ntvdm_machine_stage_v1_terminal_cs_transitions.abi_version =
    BX_NTVDM_MACHINE_STAGE_V1_TERMINAL_CS_TRANSITIONS_VERSION;
  bx_ntvdm_machine_stage_v1_terminal_cs_transitions.struct_bytes =
    sizeof(bx_ntvdm_machine_stage_v1_terminal_cs_transitions);
}

#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY
static void bx_ntvdm_machine_stage_v1_instruction_history_configure(void)
{
  (void) bx_ntvdm_mantle_instruction_history_v1_configure(
    (bx_ntvdm_machine_stage_v1_terminal_history_enabled ||
     bx_ntvdm_machine_stage_v1_terminal_cs_transitions_enabled) ?
      BX_NTVDM_INSTRUCTION_HISTORY_V1_CAPACITY_MAX : 0u);
}
#endif

static void bx_ntvdm_machine_stage_v1_terminal_provenance_clear(void)
{
  memset(&bx_ntvdm_machine_stage_v1_terminal_provenance, 0,
    sizeof(bx_ntvdm_machine_stage_v1_terminal_provenance));
  bx_ntvdm_machine_stage_v1_terminal_provenance.magic =
    BX_NTVDM_MACHINE_STAGE_V1_TERMINAL_PROVENANCE_MAGIC;
  bx_ntvdm_machine_stage_v1_terminal_provenance.abi_version =
    BX_NTVDM_MACHINE_STAGE_V1_TERMINAL_PROVENANCE_VERSION;
  bx_ntvdm_machine_stage_v1_terminal_provenance.struct_bytes =
    sizeof(bx_ntvdm_machine_stage_v1_terminal_provenance);
}

static void bx_ntvdm_machine_stage_v1_terminal_cs_provenance_clear(void)
{
  memset(&bx_ntvdm_machine_stage_v1_terminal_cs_provenance, 0,
    sizeof(bx_ntvdm_machine_stage_v1_terminal_cs_provenance));
  bx_ntvdm_machine_stage_v1_terminal_cs_provenance.magic =
    BX_NTVDM_MACHINE_STAGE_V1_TERMINAL_CS_PROVENANCE_MAGIC;
  bx_ntvdm_machine_stage_v1_terminal_cs_provenance.abi_version =
    BX_NTVDM_MACHINE_STAGE_V1_TERMINAL_CS_PROVENANCE_VERSION;
  bx_ntvdm_machine_stage_v1_terminal_cs_provenance.struct_bytes =
    sizeof(bx_ntvdm_machine_stage_v1_terminal_cs_provenance);
}

#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
static bx_bool bx_ntvdm_machine_stage_v1_real_address(uint16_t segment,
  uint16_t offset, uint32_t bytes, bx_phy_address *address)
{
  Bit32u value;
  if (address == 0 || bytes == 0u) return 0;
  value = ((Bit32u) segment << 4) + offset;
  if (value > 0x100000u - bytes) return 0;
  *address = value;
  return 1;
}

static void bx_ntvdm_machine_stage_v1_terminal_provenance_capture(void)
{
  bx_phy_address instruction_address;
  bx_phy_address stack_address;
  struct bx_ntvdm_machine_stage_v1_terminal_provenance *value =
    &bx_ntvdm_machine_stage_v1_terminal_provenance;

  value->cs = bx_cpu.sregs[BX_SEG_REG_CS].selector.value;
  value->ss = bx_cpu.sregs[BX_SEG_REG_SS].selector.value;
  value->sp = bx_cpu.get_reg16(BX_16BIT_REG_SP);
  value->eip = bx_cpu.get_eip();
  if (bx_ntvdm_machine_stage_v1_real_address(value->cs,
      (uint16_t) value->eip, BX_NTVDM_INSTRUCTION_HISTORY_V1_PREDECESSOR_BYTES,
      &instruction_address) && bx_mem.copy_from_ordinary_ram(instruction_address,
      BX_NTVDM_INSTRUCTION_HISTORY_V1_PREDECESSOR_BYTES,
      value->instruction_bytes)) value->instruction_valid = 1u;
  if (bx_ntvdm_machine_stage_v1_real_address(value->ss, value->sp,
      BX_NTVDM_INSTRUCTION_HISTORY_V1_STACK_BYTES, &stack_address) &&
      bx_mem.copy_from_ordinary_ram(stack_address,
      BX_NTVDM_INSTRUCTION_HISTORY_V1_STACK_BYTES,
      value->stack_bytes)) value->stack_valid = 1u;
  value->valid = 1u;
}
#endif

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
  bx_cpu.async_event = 1;
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
  bx_cpu.async_event = 1;
}

static bx_bool bx_ntvdm_machine_stage_preserved_range_valid(
  Bit64u address, Bit64u bytes)
{
  return bytes != 0 && bytes <= 64u && address <= 0x100000u - bytes;
}

/* Timer callbacks execute from the native CPU timing path.  This consumes
 * only selector-blind physical-line publications made by another host thread;
 * the existing PIC remains the sole owner of masking, cascade and INTR. */
static void bx_ntvdm_machine_stage_v1_physical_irq_poll(void *opaque)
{
  (void)opaque;
  (void)bx_ntvdm_mantle_drain_posted_physical_irqs_v1();
}

/* Seed the fixed PC BIOS conventional-memory datum before optional external
 * bytes.  This finite stage owns 640 KiB below A0000 and publishes the
 * little-endian size as machine lifecycle state. */
static bx_bool bx_ntvdm_machine_stage_seed_conventional_memory(void)
{
  static const Bit8u conventional_kib[] = { 0x80u, 0x02u };
  return bx_mem.copy_to_ordinary_ram(0x413u, sizeof(conventional_kib),
    conventional_kib);
}

/* The pre-entry state action is optional.  A caller which has no external
 * bytes to seed supplies the all-clear record from request_clear; this stage
 * still preserves its declared ordinary-RAM range before the startup action.
 * The distinction is mechanical only: it carries no guest or service policy. */
static bx_bool bx_ntvdm_machine_stage_optional_action_valid(
  const struct bx_ntvdm_mechanical_action_v1 *action)
{
  return action != 0 &&
    (action->action_id == 0u ? action->magic == 0u :
      bx_ntvdm_mechanical_action_v1_valid(action));
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
    bx_ntvdm_machine_stage_optional_action_valid(&request->initial_state_action) &&
    bx_ntvdm_mechanical_action_v1_valid(&request->startup_action) &&
    request->ivt_watch_enabled <= 1u &&
    (request->ivt_watch_enabled != 0u || request->ivt_watch_vector == 0u) &&
    request->ivt_watch_vector <= 0xffu &&
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
  if (!bx_ntvdm_machine_stage_machine->
      set_realmode_segment_limit_compatibility(1u)) {
    bx_ntvdm_machine_stage_v1_reset();
    return BX_NTVDM_MACHINE_STAGE_V1_MACHINE_FAILURE;
  }
  if (!bx_ntvdm_machine_stage_seed_conventional_memory()) {
    bx_ntvdm_machine_stage_v1_reset();
    return BX_NTVDM_MACHINE_STAGE_V1_MACHINE_FAILURE;
  }

  initial_state_action = request->initial_state_action;
  startup_action = request->startup_action;
  if (initial_state_action.action_id != 0u &&
      !bx_ntvdm_mantle_execute_mechanical_action_v1(&initial_state_action)) {
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
  if (request->ivt_watch_enabled != 0u &&
      !bx_ntvdm_ivt_watch_v1_initialize(request->ivt_watch_vector)) {
    bx_ntvdm_machine_stage_v1_reset();
    return BX_NTVDM_MACHINE_STAGE_V1_ACTION_FAILURE;
  }
  return BX_NTVDM_MACHINE_STAGE_V1_OK;
}

extern "C" uint32_t bx_ntvdm_machine_stage_v1_reset(void)
{
  bx_ntvdm_minimal_machine_c *machine = bx_ntvdm_machine_stage_machine;
  bx_ntvdm_machine_stage_machine = 0;
  bx_ntvdm_mantle_clear_posted_physical_irqs_v1();
  bx_ntvdm_ivt_watch_v1_reset();
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

extern "C" void bx_ntvdm_machine_stage_v1_terminal_position_observation_enable(
  uint32_t enabled)
{
  bx_ntvdm_machine_stage_v1_terminal_position_enabled = enabled == 1u;
  bx_ntvdm_machine_stage_v1_terminal_position_clear();
}

extern "C" int bx_ntvdm_machine_stage_v1_terminal_position_observation_copy(
  struct bx_ntvdm_machine_stage_v1_terminal_position *position)
{
  if (position == 0 || !bx_ntvdm_machine_stage_v1_terminal_position_enabled ||
      !bx_ntvdm_machine_stage_v1_terminal_position.valid) return 0;
  *position = bx_ntvdm_machine_stage_v1_terminal_position;
  return 1;
}

extern "C" void bx_ntvdm_machine_stage_v1_terminal_history_observation_enable(
  uint32_t enabled)
{
  bx_ntvdm_machine_stage_v1_terminal_history_enabled = enabled == 1u;
  bx_ntvdm_machine_stage_v1_terminal_history_clear();
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY
  bx_ntvdm_machine_stage_v1_instruction_history_configure();
#else
  (void) enabled;
#endif
}

extern "C" int bx_ntvdm_machine_stage_v1_terminal_history_observation_copy(
  struct bx_ntvdm_machine_stage_v1_terminal_history *history)
{
  if (history == 0 || !bx_ntvdm_machine_stage_v1_terminal_history_enabled ||
      !bx_ntvdm_machine_stage_v1_terminal_history.valid) return 0;
  *history = bx_ntvdm_machine_stage_v1_terminal_history;
  return 1;
}

extern "C" void bx_ntvdm_machine_stage_v1_terminal_cs_transitions_observation_enable(
  uint32_t enabled)
{
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY
  bx_ntvdm_machine_stage_v1_terminal_cs_transitions_enabled = enabled == 1u;
  bx_ntvdm_machine_stage_v1_instruction_history_configure();
#else
  (void) enabled;
  bx_ntvdm_machine_stage_v1_terminal_cs_transitions_enabled = 0u;
#endif
  bx_ntvdm_machine_stage_v1_terminal_cs_transitions_clear();
}

extern "C" int bx_ntvdm_machine_stage_v1_terminal_cs_transitions_observation_copy(
  struct bx_ntvdm_machine_stage_v1_terminal_cs_transitions *transitions)
{
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY
  if (transitions == 0 ||
      !bx_ntvdm_machine_stage_v1_terminal_cs_transitions_enabled ||
      !bx_ntvdm_machine_stage_v1_terminal_cs_transitions.valid) return 0;
  *transitions = bx_ntvdm_machine_stage_v1_terminal_cs_transitions;
  return 1;
#else
  (void) transitions;
  return 0;
#endif
}

extern "C" void bx_ntvdm_machine_stage_v1_terminal_provenance_observation_enable(
  uint32_t enabled)
{
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
  bx_ntvdm_machine_stage_v1_terminal_provenance_enabled = enabled == 1u;
#else
  (void) enabled;
  bx_ntvdm_machine_stage_v1_terminal_provenance_enabled = 0u;
#endif
  bx_ntvdm_machine_stage_v1_terminal_provenance_clear();
  bx_ntvdm_machine_stage_v1_terminal_cs_provenance_clear();
}

extern "C" int bx_ntvdm_machine_stage_v1_terminal_provenance_observation_copy(
  struct bx_ntvdm_machine_stage_v1_terminal_provenance *provenance)
{
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
  if (provenance == 0 || !bx_ntvdm_machine_stage_v1_terminal_provenance_enabled ||
      !bx_ntvdm_machine_stage_v1_terminal_provenance.valid) return 0;
  *provenance = bx_ntvdm_machine_stage_v1_terminal_provenance;
  return 1;
#else
  (void) provenance;
  return 0;
#endif
}

extern "C" void bx_ntvdm_machine_stage_v1_terminal_cs_provenance_observation_enable(
  uint32_t enabled)
{
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
  bx_ntvdm_machine_stage_v1_terminal_cs_provenance_enabled = enabled == 1u;
#else
  (void) enabled;
  bx_ntvdm_machine_stage_v1_terminal_cs_provenance_enabled = 0u;
#endif
  bx_ntvdm_machine_stage_v1_terminal_cs_provenance_clear();
}

extern "C" int bx_ntvdm_machine_stage_v1_terminal_cs_provenance_observation_copy(
  struct bx_ntvdm_machine_stage_v1_terminal_cs_provenance *provenance)
{
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
  if (provenance == 0 ||
      !bx_ntvdm_machine_stage_v1_terminal_cs_provenance_enabled ||
      !bx_ntvdm_machine_stage_v1_terminal_cs_provenance.valid) return 0;
  *provenance = bx_ntvdm_machine_stage_v1_terminal_cs_provenance;
  return 1;
#else
  (void) provenance;
  return 0;
#endif
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
  int stop_timer, cancellation_timer, physical_irq_timer;

  if (bx_ntvdm_machine_stage_machine == 0)
    return BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_REJECTED_INACTIVE;
  if (!bx_ntvdm_machine_stage_v1_execution_request_valid(request))
    return BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_REJECTED_INPUT;
  if (bx_ntvdm_cancellation_controller_v1_requested_reason() !=
      BX_NTVDM_CANCELLATION_V1_NONE)
    return BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_HOST_CANCELLATION;
  bx_pc_system.initialize(request->ips);
  if (bx_ntvdm_machine_stage_machine->compose_headless_8042() !=
      BX_NTVDM_MINIMAL_MACHINE_OK) {
    bx_ntvdm_machine_stage_v1_reset();
    return BX_NTVDM_MACHINE_STAGE_V1_MACHINE_FAILURE;
  }
  stop_state.watchdog_fired = 0;
  stop_state.cancellation_fired = 0;
  bx_ntvdm_mantle_clear_posted_physical_irqs_v1();
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
  physical_irq_timer = bx_pc_system.register_timer_ticks(&stop_state,
    bx_ntvdm_machine_stage_v1_physical_irq_poll,
    bx_ntvdm_machine_stage_v1_cancellation_poll_ticks, 1, 1,
    "machine-stage-physical-irq");
  if (physical_irq_timer <= 0) {
    bx_pc_system.deactivate_timer((unsigned) cancellation_timer);
    bx_pc_system.unregisterTimer((unsigned) cancellation_timer);
    bx_pc_system.deactivate_timer((unsigned) stop_timer);
    bx_pc_system.unregisterTimer((unsigned) stop_timer);
    return BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_TIMER_FAILURE;
  }
  bx_ntvdm_mantle_generic_ud_stop_observation_reset();
  bx_ntvdm_mantle_generic_ud_pending_observation_reset();
  bx_ntvdm_mantle_first_fault_observation_reset();
  bx_ntvdm_machine_stage_v1_terminal_position_clear();
  bx_ntvdm_machine_stage_v1_terminal_history_clear();
  bx_ntvdm_machine_stage_v1_terminal_cs_transitions_clear();
  bx_ntvdm_machine_stage_v1_terminal_provenance_clear();
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY
  bx_ntvdm_machine_stage_v1_instruction_history_configure();
#endif
  bx_cpu.cpu_loop();
  bx_pc_system.deactivate_timer((unsigned) physical_irq_timer);
  bx_pc_system.unregisterTimer((unsigned) physical_irq_timer);
  bx_pc_system.deactivate_timer((unsigned) cancellation_timer);
  bx_pc_system.unregisterTimer((unsigned) cancellation_timer);
  bx_pc_system.deactivate_timer((unsigned) stop_timer);
  bx_pc_system.unregisterTimer((unsigned) stop_timer);
  /* A terminal position is a selector-blind CPU fact.  Capture it for either
   * finite watchdog expiry or the otherwise-classified cpu_loop return; this
   * observation never changes the return disposition or CPU state. */
  if (bx_ntvdm_machine_stage_v1_terminal_position_enabled &&
      !stop_state.cancellation_fired &&
      !bx_ntvdm_mantle_first_fault_observation_observed() &&
      !bx_ntvdm_mantle_generic_ud_stop_observed() &&
      !bx_ntvdm_mantle_generic_ud_pending_observed()) {
    bx_ntvdm_machine_stage_v1_terminal_position.cs =
      bx_cpu.sregs[BX_SEG_REG_CS].selector.value;
    bx_ntvdm_machine_stage_v1_terminal_position.eip = bx_cpu.get_eip();
    bx_ntvdm_machine_stage_v1_terminal_position.valid = 1u;
  }
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY
  if (bx_ntvdm_machine_stage_v1_terminal_history_enabled &&
      stop_state.watchdog_fired && !stop_state.cancellation_fired &&
      !bx_ntvdm_mantle_first_fault_observation_observed() &&
      !bx_ntvdm_mantle_generic_ud_stop_observed()) {
    uint32_t count = bx_ntvdm_mantle_instruction_history_v1_count();
    uint32_t index;
    if (count > BX_NTVDM_INSTRUCTION_HISTORY_V1_CAPACITY_MAX)
      count = BX_NTVDM_INSTRUCTION_HISTORY_V1_CAPACITY_MAX;
    for (index = 0u; index < count; ++index) {
      if (!bx_ntvdm_mantle_instruction_history_v1_get(index,
          &bx_ntvdm_machine_stage_v1_terminal_history.records[index])) {
        count = 0u;
        break;
      }
    }
    bx_ntvdm_machine_stage_v1_terminal_history.count = count;
    bx_ntvdm_machine_stage_v1_terminal_history.valid = 1u;
  }
  if (bx_ntvdm_machine_stage_v1_terminal_cs_transitions_enabled &&
      stop_state.watchdog_fired && !stop_state.cancellation_fired &&
      !bx_ntvdm_mantle_first_fault_observation_observed() &&
      !bx_ntvdm_mantle_generic_ud_stop_observed()) {
    uint32_t count = bx_ntvdm_mantle_instruction_history_v1_cs_transition_count();
    uint32_t index;
    if (count > BX_NTVDM_INSTRUCTION_HISTORY_V1_CS_TRANSITION_CAPACITY_MAX)
      count = BX_NTVDM_INSTRUCTION_HISTORY_V1_CS_TRANSITION_CAPACITY_MAX;
    for (index = 0u; index < count; ++index) {
      if (!bx_ntvdm_mantle_instruction_history_v1_get_cs_transition(index,
          &bx_ntvdm_machine_stage_v1_terminal_cs_transitions.value.transitions[index])) {
        count = 0u;
        break;
      }
    }
    bx_ntvdm_machine_stage_v1_terminal_cs_transitions.value.count = count;
    bx_ntvdm_machine_stage_v1_terminal_cs_transitions.valid = 1u;
  }
#endif
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
  if (bx_ntvdm_machine_stage_v1_terminal_provenance_enabled &&
      stop_state.watchdog_fired && !stop_state.cancellation_fired &&
      !bx_ntvdm_mantle_first_fault_observation_observed() &&
      !bx_ntvdm_mantle_generic_ud_stop_observed())
    bx_ntvdm_machine_stage_v1_terminal_provenance_capture();
  if (bx_ntvdm_machine_stage_v1_terminal_cs_provenance_enabled &&
      bx_ntvdm_mantle_instruction_history_v1_get_latest_cs_provenance(
        &bx_ntvdm_machine_stage_v1_terminal_cs_provenance.value))
    bx_ntvdm_machine_stage_v1_terminal_cs_provenance.valid = 1u;
#endif
  if (bx_ntvdm_mantle_first_fault_observation_observed())
    return BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_FIRST_FAULT_STOP;
  if (bx_ntvdm_mantle_generic_ud_stop_observed())
    return BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_CONTROLLED_STOP;
  if (bx_ntvdm_mantle_generic_ud_pending_observed())
    return BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_PENDING;
  if (stop_state.cancellation_fired)
    return BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_HOST_CANCELLATION;
  return stop_state.watchdog_fired ? BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_BUDGET :
    BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_UNEXPECTED_LOOP_RETURN;
}
