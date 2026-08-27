#include "bochs.h"
#include "adapter-mvdm-host-out/softpc/cancellation_controller.h"
#include "adapter-mvdm-host-in/generic_ud_bridge.h"
#include "adapter-mvdm-host-out/softpc/machine_stage.h"

#include <string.h>
#include <windows.h>

static int make_request(struct runtime_machine_stage_request *request,
  const Bit8u *bytes, Bit32u byte_count)
{
  runtime_machine_stage_request_clear(request);
  runtime_mechanical_action_clear(&request->initial_state_action);
  request->initial_state_action.action_id = 1u;
  request->initial_state_action.kind = RUNTIME_MECHANICAL_ACTION_WRITE;
  request->initial_state_action.range_count = 1u;
  request->initial_state_action.payload_bytes = 4u;
  request->initial_state_action.ranges[0].physical_address = 0x714u;
  request->initial_state_action.ranges[0].byte_count = 4u;
  memcpy(request->initial_state_action.payload, "\x10\x20\x30\x40", 4u);
  runtime_mechanical_action_clear(&request->startup_action);
  request->startup_action.action_id = 2u;
  request->startup_action.kind = RUNTIME_MECHANICAL_ACTION_WRITE;
  request->startup_action.range_count = 1u;
  request->startup_action.payload_bytes = byte_count;
  request->startup_action.ranges[0].physical_address = 0x700u;
  request->startup_action.ranges[0].byte_count = byte_count;
  memcpy(request->startup_action.payload, bytes, byte_count);
  request->preserved_state_address = 0x714u;
  request->preserved_state_bytes = 4u;
  return runtime_machine_stage_request_valid(request);
}

static int begin_halted_stage(const Bit8u *bytes, Bit32u byte_count)
{
  struct runtime_machine_stage_request request;
  struct runtime_machine_stage_entry entry;
  if (!make_request(&request, bytes, byte_count) ||
      runtime_machine_stage_begin(&request) != RUNTIME_MACHINE_STAGE_OK)
    return 0;
  runtime_machine_stage_entry_clear(&entry);
  entry.cs = 0x70u;
  return runtime_machine_stage_arm_real_mode_entry(&entry) ==
    RUNTIME_MACHINE_STAGE_OK;
}

static uint32_t execute(uint64_t budget)
{
  struct runtime_machine_stage_execution_request request;
  runtime_machine_stage_execution_request_clear(&request);
  request.ips = 1000000u;
  request.instruction_tick_budget = budget;
  return runtime_machine_stage_execute(&request);
}

struct cancellation_thread_state {
  HANDLE stop;
  volatile LONG requested;
};

static DWORD WINAPI request_after_first_poll(void *opaque)
{
  cancellation_thread_state *state = (cancellation_thread_state *) opaque;
  while (runtime_cancellation_controller_test_poll_count() == 0u) {
    if (WaitForSingleObject(state->stop, 0u) == WAIT_OBJECT_0) return 0u;
    SwitchToThread();
  }
  if (runtime_cancellation_controller_request(
      RUNTIME_CANCELLATION_USER_REQUEST))
    InterlockedExchange(&state->requested, 1);
  return 0u;
}

extern "C" int runtime_machine_generic_ud_bridge(
  const struct runtime_generic_ud_event *event,
  struct runtime_generic_ud_outcome *outcome)
{
  if (event == 0 || outcome == 0 || event->vector != 6u) return 0;
  outcome->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
  outcome->disposition = RUNTIME_GENERIC_UD_STOP;
  return 1;
}

int main()
{
  static const Bit8u hlt[] = { 0xf4u };
  static const Bit8u ud2[] = { 0x0fu, 0x0bu };

  if (runtime_cancellation_controller_request(0u) ||
      runtime_cancellation_controller_requested_reason() != 0u ||
      !runtime_cancellation_controller_activate() ||
      runtime_cancellation_controller_activate() ||
      runtime_cancellation_controller_request(2u) ||
      !runtime_cancellation_controller_request(
        RUNTIME_CANCELLATION_USER_REQUEST) ||
      runtime_cancellation_controller_request(
        RUNTIME_CANCELLATION_USER_REQUEST)) return 1;
  if (!begin_halted_stage(hlt, sizeof(hlt)) ||
      execute(UINT64_C(100000000)) !=
        RUNTIME_MACHINE_STAGE_EXECUTION_HOST_CANCELLATION ||
      runtime_machine_stage_reset() != RUNTIME_MACHINE_STAGE_OK)
    return 2;
  runtime_cancellation_controller_deactivate();
  if (runtime_cancellation_controller_requested_reason() != 0u ||
      runtime_cancellation_controller_request(
        RUNTIME_CANCELLATION_USER_REQUEST)) return 3;

  {
    cancellation_thread_state state;
    HANDLE thread;
    DWORD wait;
    state.stop = CreateEventW(0, TRUE, FALSE, 0);
    state.requested = 0;
    if (state.stop == 0 || !runtime_cancellation_controller_activate() ||
        !begin_halted_stage(hlt, sizeof(hlt))) return 4;
    thread = CreateThread(0, 0u, request_after_first_poll, &state, 0u, 0);
    if (thread == 0 || execute(UINT64_C(100000000)) !=
        RUNTIME_MACHINE_STAGE_EXECUTION_HOST_CANCELLATION) {
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
        runtime_machine_stage_reset() != RUNTIME_MACHINE_STAGE_OK)
      return 6;
    runtime_cancellation_controller_deactivate();
  }

  if (!runtime_cancellation_controller_activate() ||
      !begin_halted_stage(ud2, sizeof(ud2))) return 7;
  if (execute(UINT64_C(100000000)) !=
      RUNTIME_MACHINE_STAGE_EXECUTION_CONTROLLED_STOP) return 8;
  if (runtime_machine_stage_reset() != RUNTIME_MACHINE_STAGE_OK)
    return 9;
  runtime_cancellation_controller_deactivate();

  if (!runtime_cancellation_controller_activate() ||
      !begin_halted_stage(hlt, sizeof(hlt)) ||
      execute(UINT64_C(1024)) != RUNTIME_MACHINE_STAGE_EXECUTION_BUDGET ||
      runtime_machine_stage_reset() != RUNTIME_MACHINE_STAGE_OK)
    return 10;
  runtime_cancellation_controller_deactivate();
  return 0;
}
