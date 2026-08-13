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
#include "bx_ntvdm_minimal_machine.h"

struct bx_ntvdm_finite_run_stop_state {
  bx_bool fired;
};

static void bx_ntvdm_finite_run_stop(void *opaque)
{
  bx_ntvdm_finite_run_stop_state *state =
    (bx_ntvdm_finite_run_stop_state *) opaque;
  state->fired = 1;
  bx_pc_system.kill_bochs_request = 1;
}

bx_ntvdm_finite_run_status bx_ntvdm_run_finite_bare_bytes(
  const bx_ntvdm_finite_run_request *request)
{
  bx_ntvdm_minimal_machine_c machine;
  bx_ntvdm_finite_run_stop_state stop_state;
  int stop_timer;

  if (request == 0 || request->entry_bytes == 0 ||
      request->entry_byte_count == 0 || request->instruction_tick_budget == 0 ||
      request->ips == 0) {
    return BX_NTVDM_FINITE_RUN_REJECTED_INPUT;
  }

  if (machine.initialize(0x100000, 0x100000) != BX_NTVDM_MINIMAL_MACHINE_OK) {
    return BX_NTVDM_FINITE_RUN_MACHINE_ERROR;
  }

  if (!bx_mem.copy_to_ordinary_ram(request->entry_physical_address,
      request->entry_byte_count, request->entry_bytes)) {
    machine.cleanup();
    return BX_NTVDM_FINITE_RUN_REJECTED_INPUT;
  }

  bx_pc_system.initialize(request->ips);
  bx_cpu.apply_real_mode_entry(request->entry_cs, request->entry_eip);
  stop_state.fired = 0;
  stop_timer = bx_pc_system.register_timer_ticks(&stop_state,
    bx_ntvdm_finite_run_stop, request->instruction_tick_budget, 0,
    1, "finite-run-stop");
  if (stop_timer <= 0) {
    machine.cleanup();
    return BX_NTVDM_FINITE_RUN_MACHINE_ERROR;
  }

  bx_ntvdm_mantle_generic_ud_fixture_stop(request->stop_on_ud_fixture);
  bx_cpu.cpu_loop();
  bx_ntvdm_mantle_generic_ud_fixture_stop(0);
  bx_pc_system.unregisterTimer((unsigned) stop_timer);
  if (machine.cleanup() != BX_NTVDM_MINIMAL_MACHINE_OK) {
    return BX_NTVDM_FINITE_RUN_MACHINE_ERROR;
  }
  if (request->stop_on_ud_fixture &&
      bx_ntvdm_mantle_generic_ud_fixture_stop_observed()) {
    return BX_NTVDM_FINITE_RUN_COMPLETED_UD_STOP;
  }
  return stop_state.fired ? BX_NTVDM_FINITE_RUN_COMPLETED_BUDGET :
    BX_NTVDM_FINITE_RUN_UNEXPECTED_LOOP_RETURN;
}
