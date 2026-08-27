#include "bochs.h"
#include "adapter-bochs/machine_facade.h"
#include "adapter-mvdm-host-in/bop_ingress.h"
#include "adapter-mvdm-host-in/generic_ud_bridge.h"
#include "adapter-softpc/machine_stage.h"

#include <string.h>

static int fixture_pending;
static int fixture_stop;

static int fixture_route(
  const struct runtime_generic_ud_event *event,
  struct runtime_generic_ud_outcome *outcome, void *context)
{
  (void)context;
  if (event == 0 || outcome == 0 || event->vector != 6u) return 0;
  outcome->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
  if (fixture_pending) {
    outcome->disposition = RUNTIME_GENERIC_UD_PENDING;
    return 1;
  }
  if (fixture_stop) {
    outcome->disposition = RUNTIME_GENERIC_UD_STOP;
    return 1;
  }
  return 0;
}

static int begin_ud_stage(void)
{
  static const Bit8u ud2[] = { 0x0fu, 0x0bu };
  struct runtime_machine_stage_request request;
  struct runtime_machine_stage_entry entry;
  runtime_machine_stage_request_clear(&request);
  runtime_mechanical_action_clear(&request.initial_state_action);
  request.initial_state_action.action_id = 1u;
  request.initial_state_action.kind = RUNTIME_MECHANICAL_ACTION_WRITE;
  request.initial_state_action.range_count = 1u;
  request.initial_state_action.payload_bytes = 4u;
  request.initial_state_action.ranges[0].physical_address = 0x714u;
  request.initial_state_action.ranges[0].byte_count = 4u;
  memcpy(request.initial_state_action.payload, "\x10\x20\x30\x40", 4u);
  runtime_mechanical_action_clear(&request.startup_action);
  request.startup_action.action_id = 2u;
  request.startup_action.kind = RUNTIME_MECHANICAL_ACTION_WRITE;
  request.startup_action.range_count = 1u;
  request.startup_action.payload_bytes = sizeof(ud2);
  request.startup_action.ranges[0].physical_address = 0x700u;
  request.startup_action.ranges[0].byte_count = sizeof(ud2);
  memcpy(request.startup_action.payload, ud2, sizeof(ud2));
  request.preserved_state_address = 0x714u;
  request.preserved_state_bytes = 4u;
  if (!runtime_machine_stage_request_valid(&request) ||
      runtime_machine_stage_begin(&request) != RUNTIME_MACHINE_STAGE_OK)
    return 0;
  runtime_machine_stage_entry_clear(&entry);
  entry.cs = 0x70u;
  return runtime_machine_stage_arm_real_mode_entry(&entry) ==
    RUNTIME_MACHINE_STAGE_OK;
}

static uint32_t execute(void)
{
  struct runtime_machine_stage_execution_request request;
  runtime_machine_stage_execution_request_clear(&request);
  request.ips = 1000000u;
  request.instruction_tick_budget = 1000000u;
  return runtime_machine_stage_execute(&request);
}

int main()
{
  if (!runtime_bop_ingress_bind(fixture_route, 0) ||
      !machine_facade_bind_opaque_callback(
        runtime_bop_ingress_opaque_callback, 0) || !begin_ud_stage()) return 1;
  fixture_pending = 1;
  if (execute() != RUNTIME_MACHINE_STAGE_EXECUTION_PENDING ||
      !runtime_machine_stage_active()) return 2;
  /* The exact unadvanced #UD is re-entered on the next controlled execute.
   * Switching only this selector-blind fixture outcome proves resume without
   * teaching core/machine about a provider. */
  fixture_pending = 0;
  fixture_stop = 1;
  if (execute() != RUNTIME_MACHINE_STAGE_EXECUTION_CONTROLLED_STOP) return 3;
  fixture_stop = 0;
  machine_facade_unbind_opaque_callback();
  runtime_bop_ingress_unbind();
  return runtime_machine_stage_reset() == RUNTIME_MACHINE_STAGE_OK ? 0 : 4;
}
