/////////////////////////////////////////////////////////////////////////
//
// Finite bare-byte execution mechanic owned entirely by the native mantle.
//
/////////////////////////////////////////////////////////////////////////

#include "adapter-bochs/machine_facade.h"
#include "finite_run.h"
#include "generic_ud_bridge.h"
#include "first_fault_observation.h"
#include "a20_capability.h"
#include "port_action.h"
#include "protected_range_action.h"

struct runtime_finite_run_stop_state {
  uint32_t fired;
};

static runtime_finite_run_terminal_snapshot terminal_snapshot;
static uint64_t terminal_capture_physical_address;
static uint8_t terminal_capture_byte_count;

void runtime_finite_run_terminal_snapshot_clear(void)
{
  memset(&terminal_snapshot, 0, sizeof(terminal_snapshot));
}

int runtime_finite_run_terminal_snapshot_get(
  runtime_finite_run_terminal_snapshot *snapshot)
{
  if (!snapshot || !terminal_snapshot.valid) return 0;
  *snapshot = terminal_snapshot;
  return 1;
}

static void runtime_finite_run_stop(void *opaque)
{
  runtime_finite_run_stop_state *state =
    (runtime_finite_run_stop_state *) opaque;
  state->fired = 1;
  machine_facade_v1_request_cpu_stop();
}

/* The finite machine exposes exactly one MiB of ordinary RAM.  Keep this
 * validation beside the native copy calls so every request range is known
 * valid before the first mutable operation. */
static int runtime_finite_run_ordinary_range_is_valid(
  uint64_t physical_address, uint64_t byte_count)
{
  return byte_count <= 0x100000 &&
    physical_address <= UINT64_C(0x100000) - byte_count;
}

static void runtime_finite_run_adapter_lifecycle_stop(void)
{
  runtime_port_action_v1_set_lifecycle_active(0u);
  runtime_protected_range_action_v1_set_lifecycle_active(0u);
  runtime_a20_capability_v1_set_lifecycle_active(0u);
}

int runtime_finite_run_terminal_snapshot_configure_ordinary_range(
  uint64_t physical_address, uint8_t byte_count)
{
  terminal_capture_physical_address = 0;
  terminal_capture_byte_count = 0;
  if (byte_count == 0) return physical_address == 0;
  if (byte_count > RUNTIME_FINITE_RUN_TERMINAL_SNAPSHOT_MAX_BYTES ||
      !runtime_finite_run_ordinary_range_is_valid(physical_address,
        byte_count)) return 0;
  terminal_capture_physical_address = physical_address;
  terminal_capture_byte_count = byte_count;
  return 1;
}

runtime_finite_run_status runtime_run_finite_bare_bytes(
  const runtime_finite_run_request *request)
{
  runtime_finite_run_stop_state stop_state;
  uint8_t preserved[64];
  uint64_t terminal_capture_address;
  uint8_t terminal_capture_count;
  uint32_t stop_timer;

  if (request == 0 || request->request_version != RUNTIME_FINITE_RUN_REQUEST_VERSION ||
      request->entry_bytes == 0 ||
      request->entry_byte_count == 0 ||
      request->entry_byte_count > RUNTIME_FINITE_RUN_MAX_ENTRY_BYTES ||
      request->instruction_tick_budget == 0 ||
      request->ips == 0 || request->preserve_byte_count > sizeof(preserved) ||
      (request->has_preentry_action &&
       !runtime_mechanical_action_v1_valid(&request->preentry_action)) ||
      (request->preserve_byte_count != 0 &&
       !runtime_finite_run_ordinary_range_is_valid(
         request->preserve_physical_address, request->preserve_byte_count)) ||
      !runtime_finite_run_ordinary_range_is_valid(
        request->entry_physical_address, request->entry_byte_count)) {
    return RUNTIME_FINITE_RUN_REJECTED_INPUT;
  }

  runtime_finite_run_terminal_snapshot_clear();
  terminal_capture_address = terminal_capture_physical_address;
  terminal_capture_count = terminal_capture_byte_count;
  terminal_capture_physical_address = 0;
  terminal_capture_byte_count = 0;

  if (!machine_facade_v1_machine_begin(0x100000u, 0x100000u)) {
    return RUNTIME_FINITE_RUN_MACHINE_ERROR;
  }
  runtime_a20_capability_v1_set_lifecycle_active(1u);
  runtime_protected_range_action_v1_set_lifecycle_active(1u);
  runtime_port_action_v1_set_lifecycle_active(1u);

  if (!machine_facade_v1_set_realmode_segment_limit_compatibility(
      request->enable_realmode_segment_limit_compatibility)) {
    runtime_finite_run_adapter_lifecycle_stop();
    machine_facade_v1_machine_cleanup();
    return RUNTIME_FINITE_RUN_MACHINE_ERROR;
  }

  if (request->has_preentry_action) {
    struct runtime_mechanical_action_v1 action = request->preentry_action;
    if (!runtime_mantle_execute_mechanical_action_v1(&action)) {
      runtime_finite_run_adapter_lifecycle_stop();
      machine_facade_v1_machine_cleanup();
      return RUNTIME_FINITE_RUN_REJECTED_INPUT;
    }
  }

  if (request->preserve_byte_count != 0 &&
      !machine_facade_v1_memory_read(request->preserve_physical_address,
        request->preserve_byte_count, preserved)) {
    runtime_finite_run_adapter_lifecycle_stop();
    machine_facade_v1_machine_cleanup();
    return RUNTIME_FINITE_RUN_REJECTED_INPUT;
  }
  if (!machine_facade_v1_memory_write(request->entry_physical_address,
      request->entry_byte_count, request->entry_bytes)) {
    runtime_finite_run_adapter_lifecycle_stop();
      machine_facade_v1_machine_cleanup();
    return RUNTIME_FINITE_RUN_REJECTED_INPUT;
  }
  if (request->preserve_byte_count != 0 &&
      !machine_facade_v1_memory_write(request->preserve_physical_address,
        request->preserve_byte_count, preserved)) {
    runtime_finite_run_adapter_lifecycle_stop();
    machine_facade_v1_machine_cleanup();
    return RUNTIME_FINITE_RUN_MACHINE_ERROR;
  }
  machine_facade_v1_initialize_timing(request->ips);
  machine_facade_v1_apply_real_mode_entry(request->entry_cs, request->entry_eip);
  stop_state.fired = 0;
  if (!machine_facade_v1_register_timer(&stop_state, runtime_finite_run_stop,
      request->instruction_tick_budget, 0u, 1u, &stop_timer)) {
    runtime_finite_run_adapter_lifecycle_stop();
    machine_facade_v1_machine_cleanup();
    return RUNTIME_FINITE_RUN_MACHINE_ERROR;
  }

  runtime_mantle_first_fault_observation_fixture_stop(request->stop_on_first_fault_fixture);
  runtime_mantle_generic_ud_stop_observation_reset();
  machine_facade_v1_cpu_loop();
  if (request->capture_terminal_snapshot) {
    if (!machine_facade_v1_copy_real_mode_entry(&terminal_snapshot.cs,
        &terminal_snapshot.eip)) {
      runtime_finite_run_adapter_lifecycle_stop();
      machine_facade_v1_machine_cleanup();
      return RUNTIME_FINITE_RUN_MACHINE_ERROR;
    }
    if (terminal_capture_count != 0) {
      if (!machine_facade_v1_memory_read(terminal_capture_address,
          terminal_capture_count, terminal_snapshot.captured_bytes)) {
        runtime_finite_run_adapter_lifecycle_stop();
        machine_facade_v1_machine_cleanup();
        return RUNTIME_FINITE_RUN_MACHINE_ERROR;
      }
      terminal_snapshot.captured_physical_address = terminal_capture_address;
      terminal_snapshot.captured_byte_count = terminal_capture_count;
    }
    terminal_snapshot.valid = 1;
  }
  runtime_mantle_first_fault_observation_fixture_stop(0);
  /* A bridge STOP may return before the finite watchdog fires.  The native
   * timer contract requires explicit deactivation before unregistration. */
  machine_facade_v1_deactivate_timer((uint32_t)stop_timer);
  machine_facade_v1_unregister_timer((uint32_t)stop_timer);
  runtime_finite_run_adapter_lifecycle_stop();
  if (!machine_facade_v1_machine_cleanup()) {
    return RUNTIME_FINITE_RUN_MACHINE_ERROR;
  }
  if (runtime_mantle_generic_ud_stop_observed()) {
    return RUNTIME_FINITE_RUN_COMPLETED_UD_STOP;
  }
  if (runtime_mantle_first_fault_observation_observed()) {
    return RUNTIME_FINITE_RUN_COMPLETED_FIRST_FAULT_STOP;
  }
  return stop_state.fired ? RUNTIME_FINITE_RUN_COMPLETED_BUDGET :
    RUNTIME_FINITE_RUN_UNEXPECTED_LOOP_RETURN;
}
