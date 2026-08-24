/////////////////////////////////////////////////////////////////////////
//
// Finite bare-byte execution mechanic owned entirely by the native mantle.
//
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "bx-core/cpu/cpu.h"
#include "bx-core/memory/memory.h"
#include "bx-mantle/pc_system.h"
#include "bx_ntvdm_finite_run.h"
#include "bx_ntvdm_generic_ud_bridge.h"
#include "bx_ntvdm_first_fault_observation_v1.h"
#include "bx-mantle/minimal_machine.h"
#include "bx_ntvdm_a20_capability_v1.h"
#include "bx_ntvdm_port_action_v1.h"
#include "bx_ntvdm_protected_range_action_v1.h"

struct bx_ntvdm_finite_run_stop_state {
  bx_bool fired;
};

static bx_ntvdm_finite_run_terminal_snapshot terminal_snapshot;
static bx_phy_address terminal_capture_physical_address;
static Bit8u terminal_capture_byte_count;

void bx_ntvdm_finite_run_terminal_snapshot_clear(void)
{
  memset(&terminal_snapshot, 0, sizeof(terminal_snapshot));
}

bx_bool bx_ntvdm_finite_run_terminal_snapshot_get(
  bx_ntvdm_finite_run_terminal_snapshot *snapshot)
{
  if (!snapshot || !terminal_snapshot.valid) return 0;
  *snapshot = terminal_snapshot;
  return 1;
}

static void bx_ntvdm_finite_run_stop(void *opaque)
{
  bx_ntvdm_finite_run_stop_state *state =
    (bx_ntvdm_finite_run_stop_state *) opaque;
  state->fired = 1;
  bx_pc_system.kill_bochs_request = 1;
}

/* The finite machine exposes exactly one MiB of ordinary RAM.  Keep this
 * validation beside the native copy calls so every request range is known
 * valid before the first mutable operation. */
static bx_bool bx_ntvdm_finite_run_ordinary_range_is_valid(
  bx_phy_address physical_address, Bit64u byte_count)
{
  return byte_count <= 0x100000 &&
    physical_address <= (bx_phy_address) (0x100000 - byte_count);
}

static void bx_ntvdm_finite_run_adapter_lifecycle_stop(void)
{
  bx_ntvdm_port_action_v1_set_lifecycle_active(0u);
  bx_ntvdm_protected_range_action_v1_set_lifecycle_active(0u);
  bx_ntvdm_a20_capability_v1_set_lifecycle_active(0u);
}

bx_bool bx_ntvdm_finite_run_terminal_snapshot_configure_ordinary_range(
  bx_phy_address physical_address, Bit8u byte_count)
{
  terminal_capture_physical_address = 0;
  terminal_capture_byte_count = 0;
  if (byte_count == 0) return physical_address == 0;
  if (byte_count > BX_NTVDM_FINITE_RUN_TERMINAL_SNAPSHOT_MAX_BYTES ||
      !bx_ntvdm_finite_run_ordinary_range_is_valid(physical_address,
        byte_count)) return 0;
  terminal_capture_physical_address = physical_address;
  terminal_capture_byte_count = byte_count;
  return 1;
}

bx_ntvdm_finite_run_status bx_ntvdm_run_finite_bare_bytes(
  const bx_ntvdm_finite_run_request *request)
{
  bx_mantle_minimal_machine_c machine;
  bx_ntvdm_finite_run_stop_state stop_state;
  Bit8u entry_probe[2];
  Bit8u preserved[64];
  bx_phy_address terminal_capture_address;
  Bit8u terminal_capture_count;
  int stop_timer;

  if (request == 0 || request->request_version != BX_NTVDM_FINITE_RUN_REQUEST_VERSION ||
      request->entry_bytes == 0 ||
      request->entry_byte_count == 0 ||
      request->entry_byte_count > BX_NTVDM_FINITE_RUN_MAX_ENTRY_BYTES ||
      request->instruction_tick_budget == 0 ||
      request->ips == 0 || request->preserve_byte_count > sizeof(preserved) ||
      (request->has_preentry_action &&
       !bx_ntvdm_mechanical_action_v1_valid(&request->preentry_action)) ||
      (request->preserve_byte_count != 0 &&
       !bx_ntvdm_finite_run_ordinary_range_is_valid(
         request->preserve_physical_address, request->preserve_byte_count)) ||
      !bx_ntvdm_finite_run_ordinary_range_is_valid(
        request->entry_physical_address, request->entry_byte_count)) {
    return BX_NTVDM_FINITE_RUN_REJECTED_INPUT;
  }

  bx_ntvdm_finite_run_terminal_snapshot_clear();
  terminal_capture_address = terminal_capture_physical_address;
  terminal_capture_count = terminal_capture_byte_count;
  terminal_capture_physical_address = 0;
  terminal_capture_byte_count = 0;

  if (machine.initialize(0x100000, 0x100000) != BX_MANTLE_MINIMAL_MACHINE_OK) {
    return BX_NTVDM_FINITE_RUN_MACHINE_ERROR;
  }
  bx_ntvdm_a20_capability_v1_set_lifecycle_active(1u);
  bx_ntvdm_protected_range_action_v1_set_lifecycle_active(1u);
  bx_ntvdm_port_action_v1_set_lifecycle_active(1u);

  if (!machine.set_realmode_segment_limit_compatibility(
      request->enable_realmode_segment_limit_compatibility)) {
    bx_ntvdm_finite_run_adapter_lifecycle_stop();
    machine.cleanup();
    return BX_NTVDM_FINITE_RUN_MACHINE_ERROR;
  }

  if (request->has_preentry_action) {
    struct bx_ntvdm_mechanical_action_v1 action = request->preentry_action;
    if (!bx_ntvdm_mantle_execute_mechanical_action_v1(&action)) {
      bx_ntvdm_finite_run_adapter_lifecycle_stop();
      machine.cleanup();
      return BX_NTVDM_FINITE_RUN_REJECTED_INPUT;
    }
  }

  if (request->preserve_byte_count != 0 &&
      !bx_mem.copy_from_ordinary_ram(request->preserve_physical_address,
        request->preserve_byte_count, preserved)) {
    bx_ntvdm_finite_run_adapter_lifecycle_stop();
    machine.cleanup();
    return BX_NTVDM_FINITE_RUN_REJECTED_INPUT;
  }
  if (!bx_mem.copy_to_ordinary_ram(request->entry_physical_address,
      request->entry_byte_count, request->entry_bytes)) {
    bx_ntvdm_finite_run_adapter_lifecycle_stop();
    machine.cleanup();
    return BX_NTVDM_FINITE_RUN_REJECTED_INPUT;
  }
  if (request->preserve_byte_count != 0 &&
      !bx_mem.copy_to_ordinary_ram(request->preserve_physical_address,
        request->preserve_byte_count, preserved)) {
    bx_ntvdm_finite_run_adapter_lifecycle_stop();
    machine.cleanup();
    return BX_NTVDM_FINITE_RUN_MACHINE_ERROR;
  }
  if (request->stop_on_ud_fixture && request->entry_byte_count >= sizeof(entry_probe) &&
      (!bx_mem.copy_from_ordinary_ram(request->entry_physical_address,
        sizeof(entry_probe), entry_probe) ||
       memcmp(entry_probe, request->entry_bytes, sizeof(entry_probe)) != 0)) {
    bx_ntvdm_finite_run_adapter_lifecycle_stop();
    machine.cleanup();
    return BX_NTVDM_FINITE_RUN_ENTRY_BYTES_MISMATCH;
  }

  bx_pc_system.initialize(request->ips);
  bx_cpu.apply_real_mode_entry(request->entry_cs, request->entry_eip);
  stop_state.fired = 0;
  stop_timer = bx_pc_system.register_timer_ticks(&stop_state,
    bx_ntvdm_finite_run_stop, request->instruction_tick_budget, 0,
    1, "finite-run-stop");
  if (stop_timer <= 0) {
    bx_ntvdm_finite_run_adapter_lifecycle_stop();
    machine.cleanup();
    return BX_NTVDM_FINITE_RUN_MACHINE_ERROR;
  }

  bx_ntvdm_mantle_generic_ud_fixture_stop(request->stop_on_ud_fixture);
  bx_ntvdm_mantle_first_fault_observation_fixture_stop(request->stop_on_first_fault_fixture);
  bx_ntvdm_mantle_generic_ud_stop_observation_reset();
  bx_cpu.cpu_loop();
  if (request->capture_terminal_snapshot) {
    terminal_snapshot.cs = bx_cpu.sregs[BX_SEG_REG_CS].selector.value;
    terminal_snapshot.eip = bx_cpu.get_eip();
    if (terminal_capture_count != 0) {
      if (!bx_mem.copy_from_ordinary_ram(terminal_capture_address,
          terminal_capture_count, terminal_snapshot.captured_bytes)) {
        bx_ntvdm_finite_run_adapter_lifecycle_stop();
        machine.cleanup();
        return BX_NTVDM_FINITE_RUN_MACHINE_ERROR;
      }
      terminal_snapshot.captured_physical_address = terminal_capture_address;
      terminal_snapshot.captured_byte_count = terminal_capture_count;
    }
    terminal_snapshot.valid = 1;
  }
  bx_ntvdm_mantle_generic_ud_fixture_stop(0);
  bx_ntvdm_mantle_first_fault_observation_fixture_stop(0);
  /* A bridge STOP may return before the finite watchdog fires.  The native
   * timer contract requires explicit deactivation before unregistration. */
  bx_pc_system.deactivate_timer((unsigned) stop_timer);
  bx_pc_system.unregisterTimer((unsigned) stop_timer);
  bx_ntvdm_finite_run_adapter_lifecycle_stop();
  if (machine.cleanup() != BX_MANTLE_MINIMAL_MACHINE_OK) {
    return BX_NTVDM_FINITE_RUN_MACHINE_ERROR;
  }
  if (bx_ntvdm_mantle_generic_ud_stop_observed()) {
    return BX_NTVDM_FINITE_RUN_COMPLETED_UD_STOP;
  }
  if (bx_ntvdm_mantle_first_fault_observation_observed()) {
    return BX_NTVDM_FINITE_RUN_COMPLETED_FIRST_FAULT_STOP;
  }
  return stop_state.fired ? BX_NTVDM_FINITE_RUN_COMPLETED_BUDGET :
    BX_NTVDM_FINITE_RUN_UNEXPECTED_LOOP_RETURN;
}