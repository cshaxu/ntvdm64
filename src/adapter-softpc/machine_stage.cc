#include "adapter-bochs/machine_facade.h"
#include "generic_ud_bridge.h"
#include "first_fault_observation.h"
#include "cancellation_controller.h"
#include "physical_irq.h"
#include "machine_stage.h"
#include "ivt_watch.h"
#include "a20_capability.h"
#include "ordinary_ram_reservation.h"
#include "port_action.h"
#include "protected_range_action.h"
#include "instruction_history.h"

#include <string.h>

#ifndef RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY
#define RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY 0
#endif

#ifndef RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
#define RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE 0
#endif

static uint32_t runtime_machine_stage_v1_terminal_position_enabled;
static struct runtime_machine_stage_v1_terminal_position
  runtime_machine_stage_v1_terminal_position;
static uint32_t runtime_machine_stage_v1_terminal_history_enabled;
static struct runtime_machine_stage_v1_terminal_history
  runtime_machine_stage_v1_terminal_history;
static uint32_t runtime_machine_stage_v1_terminal_cs_transitions_enabled;
static struct runtime_machine_stage_v1_terminal_cs_transitions
  runtime_machine_stage_v1_terminal_cs_transitions;
static uint32_t runtime_machine_stage_v1_terminal_provenance_enabled;
static struct runtime_machine_stage_v1_terminal_provenance
  runtime_machine_stage_v1_terminal_provenance;
static uint32_t runtime_machine_stage_v1_terminal_cs_provenance_enabled;
static struct runtime_machine_stage_v1_terminal_cs_provenance
  runtime_machine_stage_v1_terminal_cs_provenance;

static void runtime_machine_stage_v1_terminal_position_clear(void)
{
  memset(&runtime_machine_stage_v1_terminal_position, 0,
    sizeof(runtime_machine_stage_v1_terminal_position));
  runtime_machine_stage_v1_terminal_position.magic =
    RUNTIME_MACHINE_STAGE_V1_TERMINAL_POSITION_MAGIC;
  runtime_machine_stage_v1_terminal_position.abi_version =
    RUNTIME_MACHINE_STAGE_V1_TERMINAL_POSITION_VERSION;
  runtime_machine_stage_v1_terminal_position.struct_bytes =
    sizeof(runtime_machine_stage_v1_terminal_position);
}

static void runtime_machine_stage_v1_terminal_history_clear(void)
{
  memset(&runtime_machine_stage_v1_terminal_history, 0,
    sizeof(runtime_machine_stage_v1_terminal_history));
  runtime_machine_stage_v1_terminal_history.magic =
    RUNTIME_MACHINE_STAGE_V1_TERMINAL_HISTORY_MAGIC;
  runtime_machine_stage_v1_terminal_history.abi_version =
    RUNTIME_MACHINE_STAGE_V1_TERMINAL_HISTORY_VERSION;
  runtime_machine_stage_v1_terminal_history.struct_bytes =
    sizeof(runtime_machine_stage_v1_terminal_history);
}

static void runtime_machine_stage_v1_terminal_cs_transitions_clear(void)
{
  memset(&runtime_machine_stage_v1_terminal_cs_transitions, 0,
    sizeof(runtime_machine_stage_v1_terminal_cs_transitions));
  runtime_machine_stage_v1_terminal_cs_transitions.magic =
    RUNTIME_MACHINE_STAGE_V1_TERMINAL_CS_TRANSITIONS_MAGIC;
  runtime_machine_stage_v1_terminal_cs_transitions.abi_version =
    RUNTIME_MACHINE_STAGE_V1_TERMINAL_CS_TRANSITIONS_VERSION;
  runtime_machine_stage_v1_terminal_cs_transitions.struct_bytes =
    sizeof(runtime_machine_stage_v1_terminal_cs_transitions);
}

#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY
static void runtime_machine_stage_v1_instruction_history_configure(void)
{
  (void) runtime_mantle_instruction_history_v1_configure(
    (runtime_machine_stage_v1_terminal_history_enabled ||
     runtime_machine_stage_v1_terminal_cs_transitions_enabled) ?
      RUNTIME_INSTRUCTION_HISTORY_V1_CAPACITY_MAX : 0u);
}
#endif

static void runtime_machine_stage_v1_terminal_provenance_clear(void)
{
  memset(&runtime_machine_stage_v1_terminal_provenance, 0,
    sizeof(runtime_machine_stage_v1_terminal_provenance));
  runtime_machine_stage_v1_terminal_provenance.magic =
    RUNTIME_MACHINE_STAGE_V1_TERMINAL_PROVENANCE_MAGIC;
  runtime_machine_stage_v1_terminal_provenance.abi_version =
    RUNTIME_MACHINE_STAGE_V1_TERMINAL_PROVENANCE_VERSION;
  runtime_machine_stage_v1_terminal_provenance.struct_bytes =
    sizeof(runtime_machine_stage_v1_terminal_provenance);
}

static void runtime_machine_stage_v1_terminal_cs_provenance_clear(void)
{
  memset(&runtime_machine_stage_v1_terminal_cs_provenance, 0,
    sizeof(runtime_machine_stage_v1_terminal_cs_provenance));
  runtime_machine_stage_v1_terminal_cs_provenance.magic =
    RUNTIME_MACHINE_STAGE_V1_TERMINAL_CS_PROVENANCE_MAGIC;
  runtime_machine_stage_v1_terminal_cs_provenance.abi_version =
    RUNTIME_MACHINE_STAGE_V1_TERMINAL_CS_PROVENANCE_VERSION;
  runtime_machine_stage_v1_terminal_cs_provenance.struct_bytes =
    sizeof(runtime_machine_stage_v1_terminal_cs_provenance);
}

#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
static int runtime_machine_stage_v1_real_address(uint16_t segment,
  uint16_t offset, uint32_t bytes, uint64_t *address)
{
  uint32_t value;
  if (address == 0 || bytes == 0u) return 0;
  value = ((uint32_t) segment << 4) + offset;
  if (value > 0x100000u - bytes) return 0;
  *address = value;
  return 1;
}

static void runtime_machine_stage_v1_terminal_provenance_capture(void)
{
  uint64_t instruction_address;
  uint64_t stack_address;
  struct runtime_machine_stage_v1_terminal_provenance *value =
    &runtime_machine_stage_v1_terminal_provenance;

  if (!machine_facade_v1_copy_real_mode_state(&value->cs, &value->ss,
      &value->sp, &value->eip)) return;
  if (runtime_machine_stage_v1_real_address(value->cs,
      (uint16_t) value->eip, RUNTIME_INSTRUCTION_HISTORY_V1_PREDECESSOR_BYTES,
      &instruction_address) && machine_facade_v1_memory_read(instruction_address,
      RUNTIME_INSTRUCTION_HISTORY_V1_PREDECESSOR_BYTES,
      value->instruction_bytes)) value->instruction_valid = 1u;
  if (runtime_machine_stage_v1_real_address(value->ss, value->sp,
      RUNTIME_INSTRUCTION_HISTORY_V1_STACK_BYTES, &stack_address) &&
      machine_facade_v1_memory_read(stack_address,
      RUNTIME_INSTRUCTION_HISTORY_V1_STACK_BYTES,
      value->stack_bytes)) value->stack_valid = 1u;
  value->valid = 1u;
}
#endif

struct runtime_machine_stage_v1_stop_state {
  uint32_t watchdog_fired;
  uint32_t cancellation_fired;
};

static const Bit64u runtime_machine_stage_v1_cancellation_poll_ticks = 1024u;

static void runtime_machine_stage_v1_stop(void *opaque)
{
  runtime_machine_stage_v1_stop_state *state =
    (runtime_machine_stage_v1_stop_state *) opaque;
  state->watchdog_fired = 1;
  machine_facade_v1_request_cpu_stop();
}

static void runtime_machine_stage_v1_cancellation_poll(void *opaque)
{
  runtime_machine_stage_v1_stop_state *state =
    (runtime_machine_stage_v1_stop_state *) opaque;
#if defined(RUNTIME_CANCELLATION_TESTING)
  runtime_cancellation_controller_v1_test_poll_mark();
#endif
  if (runtime_cancellation_controller_v1_requested_reason() ==
      RUNTIME_CANCELLATION_V1_NONE) return;
  state->cancellation_fired = 1;
  machine_facade_v1_request_cpu_stop();
}

static int runtime_machine_stage_preserved_range_valid(
  uint64_t address, uint64_t bytes)
{
  return bytes != 0 && bytes <= 64u && address <= 0x100000u - bytes;
}

static int runtime_machine_stage_reservation_valid(uint64_t capacity,
  uint64_t base, uint64_t bytes)
{
  return (base == 0u && bytes == 0u) ||
    (base >= 0x100000u && bytes != 0u && base % 0x10000u == 0u &&
     bytes % 0x10000u == 0u && base + bytes > base && base + bytes <= capacity);
}

/* Timer callbacks execute from the native CPU timing path.  This consumes
 * only selector-blind physical-line publications made by another host thread;
 * the existing PIC remains the sole owner of masking, cascade and INTR. */
static void runtime_machine_stage_v1_physical_irq_poll(void *opaque)
{
  (void)opaque;
  (void)runtime_mantle_drain_posted_physical_irqs_v1();
}

/* Seed the fixed PC BIOS conventional-memory datum before optional external
 * bytes.  This finite stage owns 640 KiB below A0000 and publishes the
 * little-endian size as machine lifecycle state. */
static int runtime_machine_stage_seed_conventional_memory(void)
{
  static const uint8_t conventional_kib[] = { 0x80u, 0x02u };
  return machine_facade_v1_memory_write(0x413u, sizeof(conventional_kib),
    conventional_kib);
}

/* The pre-entry state action is optional.  A caller which has no external
 * bytes to seed supplies the all-clear record from request_clear; this stage
 * still preserves its declared ordinary-RAM range before the startup action.
 * The distinction is mechanical only: it carries no guest or service policy. */
static bx_bool runtime_machine_stage_optional_action_valid(
  const struct runtime_mechanical_action_v1 *action)
{
  return action != 0 &&
    (action->action_id == 0u ? action->magic == 0u :
      runtime_mechanical_action_v1_valid(action));
}

extern "C" void runtime_machine_stage_v1_request_clear(
  struct runtime_machine_stage_v1_request *request)
{
  if (request == 0) return;
  memset(request, 0, sizeof(*request));
  request->magic = RUNTIME_MACHINE_STAGE_V1_MAGIC;
  request->abi_version = RUNTIME_MACHINE_STAGE_V1_VERSION;
  request->struct_bytes = sizeof(*request);
  request->guest_memory_bytes = RUNTIME_MACHINE_STAGE_V1_GUEST_MEMORY_MIN_BYTES;
}

extern "C" int runtime_machine_stage_v1_request_valid(
  const struct runtime_machine_stage_v1_request *request)
{
  return request != 0 && request->magic == RUNTIME_MACHINE_STAGE_V1_MAGIC &&
    request->abi_version == RUNTIME_MACHINE_STAGE_V1_VERSION &&
    request->struct_bytes == sizeof(*request) && request->reserved0 == 0u &&
    request->guest_memory_bytes >= RUNTIME_MACHINE_STAGE_V1_GUEST_MEMORY_MIN_BYTES &&
    request->guest_memory_bytes <= RUNTIME_MACHINE_STAGE_V1_GUEST_MEMORY_MAX_BYTES &&
    request->guest_memory_bytes % RUNTIME_MACHINE_STAGE_V1_GUEST_MEMORY_GRANULARITY == 0u &&
    runtime_machine_stage_reservation_valid(request->guest_memory_bytes,
      request->reserved_memory_base, request->reserved_memory_bytes) &&
    runtime_machine_stage_optional_action_valid(&request->initial_state_action) &&
    runtime_mechanical_action_v1_valid(&request->startup_action) &&
    request->ivt_watch_enabled <= 1u &&
    (request->ivt_watch_enabled != 0u || request->ivt_watch_vector == 0u) &&
    request->ivt_watch_vector <= 0xffu &&
    runtime_machine_stage_preserved_range_valid(
      request->preserved_state_address, request->preserved_state_bytes);
}

extern "C" uint32_t runtime_machine_stage_v1_begin(
  const struct runtime_machine_stage_v1_request *request)
{
  uint8_t preserved[64];
  struct runtime_mechanical_action_v1 initial_state_action;
  struct runtime_mechanical_action_v1 startup_action;

  if (!runtime_machine_stage_v1_request_valid(request))
    return RUNTIME_MACHINE_STAGE_V1_REJECTED_INPUT;
  if (machine_facade_v1_machine_active())
    return RUNTIME_MACHINE_STAGE_V1_REJECTED_ACTIVE;
  if (!machine_facade_v1_machine_begin(request->guest_memory_bytes,
      request->guest_memory_bytes)) {
    return RUNTIME_MACHINE_STAGE_V1_MACHINE_FAILURE;
  }
  if (!machine_facade_v1_set_realmode_segment_limit_compatibility(1u)) {
    runtime_machine_stage_v1_reset();
    return RUNTIME_MACHINE_STAGE_V1_MACHINE_FAILURE;
  }
  if (!runtime_machine_stage_seed_conventional_memory()) {
    runtime_machine_stage_v1_reset();
    return RUNTIME_MACHINE_STAGE_V1_MACHINE_FAILURE;
  }
  if (runtime_ordinary_ram_reservation_v1_configure(
      request->reserved_memory_base, request->reserved_memory_bytes) !=
      RUNTIME_ORDINARY_RAM_RESERVATION_V1_OK) {
    runtime_machine_stage_v1_reset();
    return RUNTIME_MACHINE_STAGE_V1_MACHINE_FAILURE;
  }
  runtime_ordinary_ram_reservation_v1_set_lifecycle_active(1u);
  runtime_a20_capability_v1_set_lifecycle_active(1u);
  runtime_protected_range_action_v1_set_lifecycle_active(1u);
  runtime_port_action_v1_set_lifecycle_active(1u);

  initial_state_action = request->initial_state_action;
  startup_action = request->startup_action;
  if (initial_state_action.action_id != 0u &&
      !runtime_mantle_execute_mechanical_action_v1(&initial_state_action)) {
    runtime_machine_stage_v1_reset();
    return RUNTIME_MACHINE_STAGE_V1_ACTION_FAILURE;
  }
  if (!machine_facade_v1_memory_read(request->preserved_state_address,
      request->preserved_state_bytes, preserved)) {
    runtime_machine_stage_v1_reset();
    return RUNTIME_MACHINE_STAGE_V1_PRESERVE_FAILURE;
  }
  if (!runtime_mantle_execute_mechanical_action_v1(&startup_action)) {
    runtime_machine_stage_v1_reset();
    return RUNTIME_MACHINE_STAGE_V1_ACTION_FAILURE;
  }
  if (!machine_facade_v1_memory_write(request->preserved_state_address,
      request->preserved_state_bytes, preserved)) {
    runtime_machine_stage_v1_reset();
    return RUNTIME_MACHINE_STAGE_V1_PRESERVE_FAILURE;
  }
  if (request->ivt_watch_enabled != 0u &&
      !runtime_ivt_watch_v1_initialize(request->ivt_watch_vector)) {
    runtime_machine_stage_v1_reset();
    return RUNTIME_MACHINE_STAGE_V1_ACTION_FAILURE;
  }
  return RUNTIME_MACHINE_STAGE_V1_OK;
}

extern "C" uint32_t runtime_machine_stage_v1_reset(void)
{
  runtime_mantle_clear_posted_physical_irqs_v1();
  runtime_ivt_watch_v1_reset();
  runtime_ordinary_ram_reservation_v1_set_lifecycle_active(0u);
  runtime_a20_capability_v1_set_lifecycle_active(0u);
  runtime_protected_range_action_v1_set_lifecycle_active(0u);
  runtime_port_action_v1_set_lifecycle_active(0u);
  if (!machine_facade_v1_machine_active()) return RUNTIME_MACHINE_STAGE_V1_OK;
  if (!machine_facade_v1_machine_cleanup()) {
    return RUNTIME_MACHINE_STAGE_V1_CLEANUP_FAILURE;
  }
  return RUNTIME_MACHINE_STAGE_V1_OK;
}

extern "C" int runtime_machine_stage_v1_active(void)
{
  return machine_facade_v1_machine_active();
}

extern "C" void runtime_machine_stage_v1_terminal_position_observation_enable(
  uint32_t enabled)
{
  runtime_machine_stage_v1_terminal_position_enabled = enabled == 1u;
  runtime_machine_stage_v1_terminal_position_clear();
}

extern "C" int runtime_machine_stage_v1_terminal_position_observation_copy(
  struct runtime_machine_stage_v1_terminal_position *position)
{
  if (position == 0 || !runtime_machine_stage_v1_terminal_position_enabled ||
      !runtime_machine_stage_v1_terminal_position.valid) return 0;
  *position = runtime_machine_stage_v1_terminal_position;
  return 1;
}

extern "C" void runtime_machine_stage_v1_terminal_history_observation_enable(
  uint32_t enabled)
{
  runtime_machine_stage_v1_terminal_history_enabled = enabled == 1u;
  runtime_machine_stage_v1_terminal_history_clear();
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY
  runtime_machine_stage_v1_instruction_history_configure();
#else
  (void) enabled;
#endif
}

extern "C" int runtime_machine_stage_v1_terminal_history_observation_copy(
  struct runtime_machine_stage_v1_terminal_history *history)
{
  if (history == 0 || !runtime_machine_stage_v1_terminal_history_enabled ||
      !runtime_machine_stage_v1_terminal_history.valid) return 0;
  *history = runtime_machine_stage_v1_terminal_history;
  return 1;
}

extern "C" void runtime_machine_stage_v1_terminal_cs_transitions_observation_enable(
  uint32_t enabled)
{
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY
  runtime_machine_stage_v1_terminal_cs_transitions_enabled = enabled == 1u;
  runtime_machine_stage_v1_instruction_history_configure();
#else
  (void) enabled;
  runtime_machine_stage_v1_terminal_cs_transitions_enabled = 0u;
#endif
  runtime_machine_stage_v1_terminal_cs_transitions_clear();
}

extern "C" int runtime_machine_stage_v1_terminal_cs_transitions_observation_copy(
  struct runtime_machine_stage_v1_terminal_cs_transitions *transitions)
{
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY
  if (transitions == 0 ||
      !runtime_machine_stage_v1_terminal_cs_transitions_enabled ||
      !runtime_machine_stage_v1_terminal_cs_transitions.valid) return 0;
  *transitions = runtime_machine_stage_v1_terminal_cs_transitions;
  return 1;
#else
  (void) transitions;
  return 0;
#endif
}

extern "C" void runtime_machine_stage_v1_terminal_provenance_observation_enable(
  uint32_t enabled)
{
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
  runtime_machine_stage_v1_terminal_provenance_enabled = enabled == 1u;
#else
  (void) enabled;
  runtime_machine_stage_v1_terminal_provenance_enabled = 0u;
#endif
  runtime_machine_stage_v1_terminal_provenance_clear();
  runtime_machine_stage_v1_terminal_cs_provenance_clear();
}

extern "C" int runtime_machine_stage_v1_terminal_provenance_observation_copy(
  struct runtime_machine_stage_v1_terminal_provenance *provenance)
{
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
  if (provenance == 0 || !runtime_machine_stage_v1_terminal_provenance_enabled ||
      !runtime_machine_stage_v1_terminal_provenance.valid) return 0;
  *provenance = runtime_machine_stage_v1_terminal_provenance;
  return 1;
#else
  (void) provenance;
  return 0;
#endif
}

extern "C" void runtime_machine_stage_v1_terminal_cs_provenance_observation_enable(
  uint32_t enabled)
{
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
  runtime_machine_stage_v1_terminal_cs_provenance_enabled = enabled == 1u;
#else
  (void) enabled;
  runtime_machine_stage_v1_terminal_cs_provenance_enabled = 0u;
#endif
  runtime_machine_stage_v1_terminal_cs_provenance_clear();
}

extern "C" int runtime_machine_stage_v1_terminal_cs_provenance_observation_copy(
  struct runtime_machine_stage_v1_terminal_cs_provenance *provenance)
{
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
  if (provenance == 0 ||
      !runtime_machine_stage_v1_terminal_cs_provenance_enabled ||
      !runtime_machine_stage_v1_terminal_cs_provenance.valid) return 0;
  *provenance = runtime_machine_stage_v1_terminal_cs_provenance;
  return 1;
#else
  (void) provenance;
  return 0;
#endif
}

extern "C" void runtime_machine_stage_v1_entry_clear(
  struct runtime_machine_stage_v1_entry *entry)
{
  if (entry == 0) return;
  memset(entry, 0, sizeof(*entry));
  entry->magic = RUNTIME_MACHINE_STAGE_V1_ENTRY_MAGIC;
  entry->abi_version = RUNTIME_MACHINE_STAGE_V1_VERSION;
  entry->struct_bytes = sizeof(*entry);
}

extern "C" int runtime_machine_stage_v1_entry_valid(
  const struct runtime_machine_stage_v1_entry *entry)
{
  return entry != 0 && entry->magic == RUNTIME_MACHINE_STAGE_V1_ENTRY_MAGIC &&
    entry->abi_version == RUNTIME_MACHINE_STAGE_V1_VERSION &&
    entry->struct_bytes == sizeof(*entry) && entry->reserved0 == 0u &&
    entry->reserved1 == 0u && entry->eip <= 0xffffu;
}

extern "C" uint32_t runtime_machine_stage_v1_arm_real_mode_entry(
  const struct runtime_machine_stage_v1_entry *entry)
{
  if (!machine_facade_v1_machine_active())
    return RUNTIME_MACHINE_STAGE_V1_REJECTED_INACTIVE;
  if (!runtime_machine_stage_v1_entry_valid(entry))
    return RUNTIME_MACHINE_STAGE_V1_REJECTED_ENTRY;
  machine_facade_v1_apply_real_mode_entry(entry->cs, entry->eip);
  return RUNTIME_MACHINE_STAGE_V1_OK;
}

extern "C" uint32_t runtime_machine_stage_v1_copy_real_mode_entry(
  struct runtime_machine_stage_v1_entry *entry)
{
  if (!machine_facade_v1_machine_active() || entry == 0)
    return RUNTIME_MACHINE_STAGE_V1_REJECTED_INACTIVE;
  runtime_machine_stage_v1_entry_clear(entry);
  if (!machine_facade_v1_copy_real_mode_entry(&entry->cs, &entry->eip))
    return RUNTIME_MACHINE_STAGE_V1_MACHINE_FAILURE;
  return RUNTIME_MACHINE_STAGE_V1_OK;
}

extern "C" void runtime_machine_stage_v1_execution_request_clear(
  struct runtime_machine_stage_v1_execution_request *request)
{
  if (request == 0) return;
  memset(request, 0, sizeof(*request));
  request->magic = RUNTIME_MACHINE_STAGE_V1_EXECUTION_MAGIC;
  request->abi_version = RUNTIME_MACHINE_STAGE_V1_VERSION;
  request->struct_bytes = sizeof(*request);
}

extern "C" int runtime_machine_stage_v1_execution_request_valid(
  const struct runtime_machine_stage_v1_execution_request *request)
{
  return request != 0 &&
    request->magic == RUNTIME_MACHINE_STAGE_V1_EXECUTION_MAGIC &&
    request->abi_version == RUNTIME_MACHINE_STAGE_V1_VERSION &&
    request->struct_bytes == sizeof(*request) && request->ips != 0u &&
    request->instruction_tick_budget != 0u;
}

extern "C" uint32_t runtime_machine_stage_v1_execute(
  const struct runtime_machine_stage_v1_execution_request *request)
{
  runtime_machine_stage_v1_stop_state stop_state;
  uint32_t stop_timer, cancellation_timer, physical_irq_timer;

  if (!machine_facade_v1_machine_active())
    return RUNTIME_MACHINE_STAGE_V1_EXECUTION_REJECTED_INACTIVE;
  if (!runtime_machine_stage_v1_execution_request_valid(request))
    return RUNTIME_MACHINE_STAGE_V1_EXECUTION_REJECTED_INPUT;
  if (runtime_cancellation_controller_v1_requested_reason() !=
      RUNTIME_CANCELLATION_V1_NONE)
    return RUNTIME_MACHINE_STAGE_V1_EXECUTION_HOST_CANCELLATION;
  machine_facade_v1_initialize_timing(request->ips);
  if (!machine_facade_v1_compose_headless_8042()) {
    runtime_machine_stage_v1_reset();
    return RUNTIME_MACHINE_STAGE_V1_MACHINE_FAILURE;
  }
  stop_state.watchdog_fired = 0;
  stop_state.cancellation_fired = 0;
  runtime_mantle_clear_posted_physical_irqs_v1();
  if (!machine_facade_v1_register_timer(&stop_state,
      runtime_machine_stage_v1_stop, request->instruction_tick_budget, 0u,
      1u, &stop_timer))
    return RUNTIME_MACHINE_STAGE_V1_EXECUTION_TIMER_FAILURE;
  if (!machine_facade_v1_register_timer(&stop_state,
      runtime_machine_stage_v1_cancellation_poll,
      runtime_machine_stage_v1_cancellation_poll_ticks, 1u, 1u,
      &cancellation_timer)) {
    machine_facade_v1_deactivate_timer(stop_timer);
    machine_facade_v1_unregister_timer(stop_timer);
    return RUNTIME_MACHINE_STAGE_V1_EXECUTION_TIMER_FAILURE;
  }
  if (!machine_facade_v1_register_timer(&stop_state,
      runtime_machine_stage_v1_physical_irq_poll,
      runtime_machine_stage_v1_cancellation_poll_ticks, 1u, 1u,
      &physical_irq_timer)) {
    machine_facade_v1_deactivate_timer(cancellation_timer);
    machine_facade_v1_unregister_timer(cancellation_timer);
    machine_facade_v1_deactivate_timer(stop_timer);
    machine_facade_v1_unregister_timer(stop_timer);
    return RUNTIME_MACHINE_STAGE_V1_EXECUTION_TIMER_FAILURE;
  }
  runtime_mantle_generic_ud_stop_observation_reset();
  runtime_mantle_generic_ud_pending_observation_reset();
  runtime_mantle_first_fault_observation_reset();
  runtime_machine_stage_v1_terminal_position_clear();
  runtime_machine_stage_v1_terminal_history_clear();
  runtime_machine_stage_v1_terminal_cs_transitions_clear();
  runtime_machine_stage_v1_terminal_provenance_clear();
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY
  runtime_machine_stage_v1_instruction_history_configure();
#endif
  machine_facade_v1_cpu_loop();
  machine_facade_v1_deactivate_timer(physical_irq_timer);
  machine_facade_v1_unregister_timer(physical_irq_timer);
  machine_facade_v1_deactivate_timer(cancellation_timer);
  machine_facade_v1_unregister_timer(cancellation_timer);
  machine_facade_v1_deactivate_timer(stop_timer);
  machine_facade_v1_unregister_timer(stop_timer);
  /* A terminal position is a selector-blind CPU fact.  Capture it for either
   * finite watchdog expiry or the otherwise-classified cpu_loop return; this
   * observation never changes the return disposition or CPU state. */
  if (runtime_machine_stage_v1_terminal_position_enabled &&
      !stop_state.cancellation_fired &&
      !runtime_mantle_first_fault_observation_observed() &&
      !runtime_mantle_generic_ud_stop_observed() &&
      !runtime_mantle_generic_ud_pending_observed()) {
    if (!machine_facade_v1_copy_real_mode_entry(
        &runtime_machine_stage_v1_terminal_position.cs,
        &runtime_machine_stage_v1_terminal_position.eip))
      return RUNTIME_MACHINE_STAGE_V1_MACHINE_FAILURE;
    runtime_machine_stage_v1_terminal_position.valid = 1u;
  }
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY
  if (runtime_machine_stage_v1_terminal_history_enabled &&
      stop_state.watchdog_fired && !stop_state.cancellation_fired &&
      !runtime_mantle_first_fault_observation_observed() &&
      !runtime_mantle_generic_ud_stop_observed()) {
    uint32_t count = runtime_mantle_instruction_history_v1_count();
    uint32_t index;
    if (count > RUNTIME_INSTRUCTION_HISTORY_V1_CAPACITY_MAX)
      count = RUNTIME_INSTRUCTION_HISTORY_V1_CAPACITY_MAX;
    for (index = 0u; index < count; ++index) {
      if (!runtime_mantle_instruction_history_v1_get(index,
          &runtime_machine_stage_v1_terminal_history.records[index])) {
        count = 0u;
        break;
      }
    }
    runtime_machine_stage_v1_terminal_history.count = count;
    runtime_machine_stage_v1_terminal_history.valid = 1u;
  }
  if (runtime_machine_stage_v1_terminal_cs_transitions_enabled &&
      stop_state.watchdog_fired && !stop_state.cancellation_fired &&
      !runtime_mantle_first_fault_observation_observed() &&
      !runtime_mantle_generic_ud_stop_observed()) {
    uint32_t count = runtime_mantle_instruction_history_v1_cs_transition_count();
    uint32_t index;
    if (count > RUNTIME_INSTRUCTION_HISTORY_V1_CS_TRANSITION_CAPACITY_MAX)
      count = RUNTIME_INSTRUCTION_HISTORY_V1_CS_TRANSITION_CAPACITY_MAX;
    for (index = 0u; index < count; ++index) {
      if (!runtime_mantle_instruction_history_v1_get_cs_transition(index,
          &runtime_machine_stage_v1_terminal_cs_transitions.value.transitions[index])) {
        count = 0u;
        break;
      }
    }
    runtime_machine_stage_v1_terminal_cs_transitions.value.count = count;
    runtime_machine_stage_v1_terminal_cs_transitions.valid = 1u;
  }
#endif
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
  if (runtime_machine_stage_v1_terminal_provenance_enabled &&
      stop_state.watchdog_fired && !stop_state.cancellation_fired &&
      !runtime_mantle_first_fault_observation_observed() &&
      !runtime_mantle_generic_ud_stop_observed())
    runtime_machine_stage_v1_terminal_provenance_capture();
  if (runtime_machine_stage_v1_terminal_cs_provenance_enabled &&
      runtime_mantle_instruction_history_v1_get_latest_cs_provenance(
        &runtime_machine_stage_v1_terminal_cs_provenance.value))
    runtime_machine_stage_v1_terminal_cs_provenance.valid = 1u;
#endif
  if (runtime_mantle_first_fault_observation_observed())
    return RUNTIME_MACHINE_STAGE_V1_EXECUTION_FIRST_FAULT_STOP;
  if (runtime_mantle_generic_ud_stop_observed())
    return RUNTIME_MACHINE_STAGE_V1_EXECUTION_CONTROLLED_STOP;
  if (runtime_mantle_generic_ud_pending_observed())
    return RUNTIME_MACHINE_STAGE_V1_EXECUTION_PENDING;
  if (stop_state.cancellation_fired)
    return RUNTIME_MACHINE_STAGE_V1_EXECUTION_HOST_CANCELLATION;
  return stop_state.watchdog_fired ? RUNTIME_MACHINE_STAGE_V1_EXECUTION_BUDGET :
    RUNTIME_MACHINE_STAGE_V1_EXECUTION_UNEXPECTED_LOOP_RETURN;
}
