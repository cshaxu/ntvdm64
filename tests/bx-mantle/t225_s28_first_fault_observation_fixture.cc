#include "adapter-softpc/finite_run.h"
#include "adapter-softpc/first_fault_observation.h"
#include "adapter-bop/generic_ud_bridge.h"

extern "C" int runtime_mantle_generic_ud_bridge_v1(
  const struct runtime_generic_ud_event_v1 *,
  struct runtime_generic_ud_outcome_v1 *)
{
  return 0;
}

#include <string.h>

int main(void)
{
  struct runtime_finite_run_request request;
  struct runtime_first_fault_observation_v1 event;
  runtime_finite_run_status status;

  memset(&event, 0, sizeof(event));
  event.magic = RUNTIME_FIRST_FAULT_OBSERVATION_V1_MAGIC;
  event.abi_version = RUNTIME_FIRST_FAULT_OBSERVATION_V1_VERSION;
  event.struct_bytes = sizeof(event);
  event.vector = 0u;
  runtime_mantle_first_fault_observation_enable(0);
  if (runtime_mantle_first_fault_observation_v1(&event) ||
      runtime_mantle_first_fault_observation_observed()) return 6;

  memset(&request, 0, sizeof(request));
  request.request_version = RUNTIME_FINITE_RUN_REQUEST_VERSION;
  request.entry_bytes[0] = 0x31u; /* xor ax, ax */
  request.entry_bytes[1] = 0xc0u;
  request.entry_bytes[2] = 0xf7u; /* div ax: test-owned #DE, never #UD */
  request.entry_bytes[3] = 0xf0u;
  request.entry_byte_count = 4u;
  request.entry_physical_address = 0xe000u;
  request.entry_cs = 0x0e00u;
  request.entry_eip = 0u;
  request.instruction_tick_budget = 64u;
  request.ips = 1u;
  request.stop_on_first_fault_fixture = 1;
  status = runtime_run_finite_bare_bytes(&request);
  if (status != RUNTIME_FINITE_RUN_COMPLETED_FIRST_FAULT_STOP) return 1;
  if (!runtime_mantle_first_fault_observation_observed()) return 2;
  memset(&event, 0, sizeof(event));
  if (!runtime_mantle_first_fault_observation_copy(&event)) return 3;
  if (event.magic != RUNTIME_FIRST_FAULT_OBSERVATION_V1_MAGIC ||
      event.abi_version != RUNTIME_FIRST_FAULT_OBSERVATION_V1_VERSION ||
      event.struct_bytes != sizeof(event) || event.vector != 0u ||
      event.cs != 0x0e00u || event.eax != 0u || event.eip != 2u) return 4;
  runtime_mantle_first_fault_observation_reset();
  if (runtime_mantle_first_fault_observation_observed() ||
      runtime_mantle_first_fault_observation_copy(&event)) return 5;
  return 0;
}
