#include "bochs.h"
#include "adapter-bop/generic_ud_bridge.h"
#include "adapter-softpc/machine_stage.h"

#include <string.h>

extern "C" int runtime_mantle_generic_ud_bridge_v1(
  const struct runtime_generic_ud_event_v1 *event,
  struct runtime_generic_ud_outcome_v1 *outcome)
{
  (void) event;
  (void) outcome;
  return 0;
}

static int begin_stage(uint64_t memory_bytes)
{
  static const Bit8u halt[] = { 0xf4u };
  struct runtime_machine_stage_v1_request request;

  runtime_machine_stage_v1_request_clear(&request);
  request.guest_memory_bytes = memory_bytes;
  runtime_mechanical_action_v1_clear(&request.startup_action);
  request.startup_action.action_id = 1u;
  request.startup_action.kind = RUNTIME_MECHANICAL_ACTION_V1_WRITE;
  request.startup_action.range_count = 1u;
  request.startup_action.payload_bytes = sizeof(halt);
  request.startup_action.ranges[0].physical_address = 0x700u;
  request.startup_action.ranges[0].byte_count = sizeof(halt);
  memcpy(request.startup_action.payload, halt, sizeof(halt));
  request.preserved_state_address = 0x600u;
  request.preserved_state_bytes = 1u;
  return runtime_machine_stage_v1_request_valid(&request) &&
    runtime_machine_stage_v1_begin(&request) == RUNTIME_MACHINE_STAGE_V1_OK;
}

int main(void)
{
  struct runtime_machine_stage_v1_request request;
  Bit8u written = 0x5au;
  Bit8u observed = 0u;

  runtime_machine_stage_v1_request_clear(&request);
  runtime_mechanical_action_v1_clear(&request.startup_action);
  request.startup_action.action_id = 1u;
  request.startup_action.kind = RUNTIME_MECHANICAL_ACTION_V1_WRITE;
  request.startup_action.range_count = 1u;
  request.startup_action.payload_bytes = 1u;
  request.startup_action.ranges[0].physical_address = 0x700u;
  request.startup_action.ranges[0].byte_count = 1u;
  request.startup_action.payload[0] = 0xf4u;
  request.preserved_state_address = 0x600u;
  request.preserved_state_bytes = 1u;
  if (request.guest_memory_bytes !=
      RUNTIME_MACHINE_STAGE_V1_GUEST_MEMORY_MIN_BYTES ||
      !runtime_machine_stage_v1_request_valid(&request)) return 1;
  request.guest_memory_bytes = UINT64_C(0x1ff000);
  if (runtime_machine_stage_v1_request_valid(&request)) return 2;
  request.guest_memory_bytes = UINT64_C(0x1010000);
  if (runtime_machine_stage_v1_request_valid(&request)) return 3;
  if (!begin_stage(UINT64_C(0x200000))) return 4;
  if (!runtime_mantle_checked_ram_write_v1(UINT64_C(0x100000), &written,
        sizeof(written)) ||
      !runtime_mantle_checked_ram_read_v1(UINT64_C(0x100000), &observed,
        sizeof(observed)) || observed != written) return 5;
  return runtime_machine_stage_v1_reset() == RUNTIME_MACHINE_STAGE_V1_OK ?
    0 : 6;
}
