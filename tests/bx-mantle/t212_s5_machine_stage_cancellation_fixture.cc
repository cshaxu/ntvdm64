#include "bochs.h"
#include "adapter-softpc/bx_ntvdm_cancellation_controller_v1.h"
#include "adapter-bop/bx_ntvdm_generic_ud_bridge.h"
#include "adapter-softpc/bx_ntvdm_machine_stage_v1.h"

#include <string.h>
#include <windows.h>

static int make_request(struct bx_ntvdm_machine_stage_v1_request *request,
  const Bit8u *bytes, Bit32u byte_count)
{
  bx_ntvdm_machine_stage_v1_request_clear(request);
  bx_ntvdm_mechanical_action_v1_clear(&request->initial_state_action);
  request->initial_state_action.action_id = 1u;
  request->initial_state_action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
  request->initial_state_action.range_count = 1u;
  request->initial_state_action.payload_bytes = 4u;
  request->initial_state_action.ranges[0].physical_address = 0x714u;
  request->initial_state_action.ranges[0].byte_count = 4u;
  memcpy(request->initial_state_action.payload, "\x10\x20\x30\x40", 4u);
  bx_ntvdm_mechanical_action_v1_clear(&request->startup_action);
  request->startup_action.action_id = 2u;
  request->startup_action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
  request->startup_action.range_count = 1u;
  request->startup_action.payload_bytes = byte_count;
  request->startup_action.ranges[0].physical_address = 0x700u;
  request->startup_action.ranges[0].byte_count = byte_count;
  memcpy(request->startup_action.payload, bytes, byte_count);
  request->preserved_state_address = 0x714u;
  request->preserved_state_bytes = 4u;
  return bx_ntvdm_machine_stage_v1_request_valid(request);
}

static int begin_halted_stage(const Bit8u *bytes, Bit32u byte_count)
{
  struct bx_ntvdm_machine_stage_v1_request request;
  struct bx_ntvdm_machine_stage_v1_entry entry;
  if (!make_request(&request, bytes, byte_count) ||
      bx_ntvdm_machine_stage_v1_begin(&request) != BX_NTVDM_MACHINE_STAGE_V1_OK)
    return 0;
  bx_ntvdm_machine_stage_v1_entry_clear(&entry);
  entry.cs = 0x70u;
  return bx_ntvdm_machine_stage_v1_arm_real_mode_entry(&entry) ==
    BX_NTVDM_MACHINE_STAGE_V1_OK;
}

static uint32_t execute(uint64_t budget)
{
  struct bx_ntvdm_machine_stage_v1_execution_request request;
  bx_ntvdm_machine_stage_v1_execution_request_clear(&request);
  request.ips = 1000000u;
  request.instruction_tick_budget = budget;
  return bx_ntvdm_machine_stage_v1_execute(&request);
}

struct cancellation_thread_state {
  HANDLE stop;
  volatile LONG requested;
};

static DWORD WINAPI request_after_first_poll(void *opaque)
{
  cancellation_thread_state *state = (cancellation_thread_state *) opaque;
  while (bx_ntvdm_cancellation_controller_v1_test_poll_count() == 0u) {
    if (WaitForSingleObject(state->stop, 0u) == WAIT_OBJECT_0) return 0u;
    SwitchToThread();
  }
  if (bx_ntvdm_cancellation_controller_v1_request(
      BX_NTVDM_CANCELLATION_V1_USER_REQUEST))
    InterlockedExchange(&state->requested, 1);
  return 0u;
}

int main()
{
  static const Bit8u hlt[] = { 0xf4u };
  static const Bit8u ud2[] = { 0x0fu, 0x0bu };

  if (bx_ntvdm_cancellation_controller_v1_request(0u) ||
      bx_ntvdm_cancellation_controller_v1_requested_reason() != 0u ||
      !bx_ntvdm_cancellation_controller_v1_activate() ||
      bx_ntvdm_cancellation_controller_v1_activate() ||
      bx_ntvdm_cancellation_controller_v1_request(2u) ||
      !bx_ntvdm_cancellation_controller_v1_request(
        BX_NTVDM_CANCELLATION_V1_USER_REQUEST) ||
      bx_ntvdm_cancellation_controller_v1_request(
        BX_NTVDM_CANCELLATION_V1_USER_REQUEST)) return 1;
  if (!begin_halted_stage(hlt, sizeof(hlt)) ||
      execute(UINT64_C(100000000)) !=
        BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_HOST_CANCELLATION ||
      bx_ntvdm_machine_stage_v1_reset() != BX_NTVDM_MACHINE_STAGE_V1_OK)
    return 2;
  bx_ntvdm_cancellation_controller_v1_deactivate();
  if (bx_ntvdm_cancellation_controller_v1_requested_reason() != 0u ||
      bx_ntvdm_cancellation_controller_v1_request(
        BX_NTVDM_CANCELLATION_V1_USER_REQUEST)) return 3;

  {
    cancellation_thread_state state;
    HANDLE thread;
    DWORD wait;
    state.stop = CreateEventW(0, TRUE, FALSE, 0);
    state.requested = 0;
    if (state.stop == 0 || !bx_ntvdm_cancellation_controller_v1_activate() ||
        !begin_halted_stage(hlt, sizeof(hlt))) return 4;
    thread = CreateThread(0, 0u, request_after_first_poll, &state, 0u, 0);
    if (thread == 0 || execute(UINT64_C(100000000)) !=
        BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_HOST_CANCELLATION) {
      SetEvent(state.stop);
      if (thread != 0) { WaitForSingleObject(thread, INFINITE); CloseHandle(thread); }
      CloseHandle(state.stop);
      return 5;
    }
    SetEvent(state.stop);
    wait = WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
    CloseHandle(state.stop);
    if (wait != WAIT_OBJECT_0 ||
        InterlockedCompareExchange(&state.requested, 0, 0) != 1 ||
        bx_ntvdm_machine_stage_v1_reset() != BX_NTVDM_MACHINE_STAGE_V1_OK)
      return 6;
    bx_ntvdm_cancellation_controller_v1_deactivate();
  }

  if (!bx_ntvdm_cancellation_controller_v1_activate() ||
      !begin_halted_stage(ud2, sizeof(ud2))) return 7;
  bx_ntvdm_mantle_generic_ud_fixture_stop(1);
  if (execute(UINT64_C(100000000)) !=
      BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_CONTROLLED_STOP) return 8;
  bx_ntvdm_mantle_generic_ud_fixture_stop(0);
  if (bx_ntvdm_machine_stage_v1_reset() != BX_NTVDM_MACHINE_STAGE_V1_OK)
    return 9;
  bx_ntvdm_cancellation_controller_v1_deactivate();

  if (!bx_ntvdm_cancellation_controller_v1_activate() ||
      !begin_halted_stage(hlt, sizeof(hlt)) ||
      execute(UINT64_C(1024)) != BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_BUDGET ||
      bx_ntvdm_machine_stage_v1_reset() != BX_NTVDM_MACHINE_STAGE_V1_OK)
    return 10;
  bx_ntvdm_cancellation_controller_v1_deactivate();
  return 0;
}
