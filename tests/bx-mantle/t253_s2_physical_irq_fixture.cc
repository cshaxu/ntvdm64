#include "bochs.h"
#include "iodev/iodev.h"
#include "adapter-bop/generic_ud_bridge.h"
#include "adapter-softpc/machine_stage.h"
#include "adapter-softpc/physical_irq.h"
#include "adapter-softpc/port_action.h"
#include "bx-mantle/pc_system.h"

#include <string.h>

extern "C" int runtime_mantle_generic_ud_bridge_v1(
  const struct runtime_generic_ud_event_v1 *,
  struct runtime_generic_ud_outcome_v1 *)
{ return 0; }

static int write8(uint16_t port, uint8_t value)
{
  struct runtime_port_action_v1 action;
  runtime_port_action_v1_initialize(&action);
  action.kind = RUNTIME_PORT_ACTION_V1_WRITE8;
  action.port = port;
  action.value = value;
  return runtime_mantle_execute_port_action_v1(&action);
}

static int begin_stage(void)
{
  struct runtime_machine_stage_v1_request request;
  const uint8_t state[1] = { 0 };
  runtime_machine_stage_v1_request_clear(&request);
  runtime_mechanical_action_v1_clear(&request.initial_state_action);
  request.initial_state_action.action_id = 1u;
  request.initial_state_action.kind = RUNTIME_MECHANICAL_ACTION_V1_WRITE;
  request.initial_state_action.range_count = 1u;
  request.initial_state_action.payload_bytes = sizeof(state);
  request.initial_state_action.ranges[0].physical_address = 0x700u;
  request.initial_state_action.ranges[0].byte_count = sizeof(state);
  memcpy(request.initial_state_action.payload, state, sizeof(state));
  runtime_mechanical_action_v1_clear(&request.startup_action);
  request.startup_action.action_id = 2u;
  request.startup_action.kind = RUNTIME_MECHANICAL_ACTION_V1_WRITE;
  request.startup_action.range_count = 1u;
  request.startup_action.payload_bytes = sizeof(state);
  request.startup_action.ranges[0].physical_address = 0x701u;
  request.startup_action.ranges[0].byte_count = sizeof(state);
  memcpy(request.startup_action.payload, state, sizeof(state));
  request.preserved_state_address = 0x700u;
  request.preserved_state_bytes = sizeof(state);
  return runtime_machine_stage_v1_request_valid(&request) &&
    runtime_machine_stage_v1_begin(&request) == RUNTIME_MACHINE_STAGE_V1_OK;
}

int main(void)
{
  if (runtime_mantle_request_physical_irq_v1(14u)) return 1;
  if (runtime_mantle_post_physical_irq_v1(14u)) return 12;
  if (runtime_mantle_request_physical_irq_v1(16u)) return 2;
  if (!begin_stage()) return 3;
  if (bx_devices.pluginPicDevice == &bx_devices.stubPic) return 4;
  if (runtime_mantle_post_physical_irq_v1(16u)) return 13;
  /* Default slave mask keeps the valid line pending, not spuriously injected. */
  if (!runtime_mantle_post_physical_irq_v1(14u)) return 5;
  if (runtime_mantle_drain_posted_physical_irqs_v1() != 1u) return 14;
  if (bx_pc_system.IAC() == 0x76u) return 6;
  /* Unmask slave IRQ6: the retained request now acknowledges as vector 76h. */
  if (!write8(0xa1u, 0xbfu)) return 7;
  if (bx_pc_system.IAC() != 0x76u) return 8;
  if (runtime_mantle_post_physical_irq_v1(16u)) return 9;
  if (runtime_machine_stage_v1_reset() != RUNTIME_MACHINE_STAGE_V1_OK) return 10;
  if (runtime_mantle_drain_posted_physical_irqs_v1() != 0u) return 15;
  return runtime_mantle_request_physical_irq_v1(14u) ? 11 : 0;
}
