#include "adapter-mvdm-host-out/softpc/finite_run.h"
#include "adapter-mvdm-host-out/softpc/first_fault_observation.h"
#include "adapter-mvdm-host-out/softpc/segment_access_observation.h"
#include "adapter-mvdm-host-in/generic_ud_bridge.h"

extern "C" int runtime_machine_generic_ud_bridge(
  const struct runtime_generic_ud_event *,
  struct runtime_generic_ud_outcome *)
{
  return 0;
}

#include <string.h>

int main(void)
{
  struct runtime_finite_run_request request;
  struct runtime_first_fault_observation fault;
  struct runtime_segment_access_observation access;
  runtime_finite_run_status status;

  memset(&access, 0, sizeof(access));
  access.magic = RUNTIME_SEGMENT_ACCESS_OBSERVATION_MAGIC;
  access.abi_version = RUNTIME_SEGMENT_ACCESS_OBSERVATION_VERSION;
  access.struct_bytes = sizeof(access);
  access.access_kind = RUNTIME_SEGMENT_ACCESS_KIND_READ_WORD;
  access.branch_kind = RUNTIME_SEGMENT_ACCESS_BRANCH_DIRECT_LIMIT;
  access.width = 2u;
  runtime_machine_segment_access_observation_enable(0);
  if (runtime_machine_segment_access_observation(&access) ||
      runtime_machine_segment_access_observation_copy(&access)) return 10;

  memset(&request, 0, sizeof(request));
  request.request_version = RUNTIME_FINITE_RUN_REQUEST_VERSION;
  request.entry_bytes[0] = 0xa1u; /* mov ax,[ffffh], a two-byte DS read */
  request.entry_bytes[1] = 0xffu;
  request.entry_bytes[2] = 0xffu;
  request.entry_byte_count = 3u;
  request.entry_physical_address = 0xe000u;
  request.entry_cs = 0x0e00u;
  request.entry_eip = 0u;
  request.instruction_tick_budget = 64u;
  request.ips = 1u;
  request.stop_on_first_fault_fixture = 1;
  runtime_machine_segment_access_observation_enable(1);
  status = runtime_run_finite_bare_bytes(&request);
  if (status != RUNTIME_FINITE_RUN_COMPLETED_FIRST_FAULT_STOP) return 1;
  if (!runtime_machine_first_fault_observation_observed()) return 2;
  memset(&fault, 0, sizeof(fault));
  if (!runtime_machine_first_fault_observation_copy(&fault) ||
      fault.vector != 13u || fault.cs != 0x0e00u || fault.eip != 0u) return 3;
  memset(&access, 0, sizeof(access));
  if (!runtime_machine_segment_access_observation_copy(&access) ||
      access.magic != RUNTIME_SEGMENT_ACCESS_OBSERVATION_MAGIC ||
      access.abi_version != RUNTIME_SEGMENT_ACCESS_OBSERVATION_VERSION ||
      access.struct_bytes != sizeof(access) ||
      access.access_kind != RUNTIME_SEGMENT_ACCESS_KIND_READ_WORD ||
      access.branch_kind != RUNTIME_SEGMENT_ACCESS_BRANCH_DIRECT_LIMIT ||
      access.width != 2u || access.offset != 0xffffu ||
      access.limit_scaled != 0xffffu || access.segment_selector != 0u) return 4;
  if (runtime_machine_segment_access_observation(&access)) return 5;
  runtime_machine_segment_access_observation_enable(0);
  runtime_machine_segment_access_observation_reset();
  runtime_machine_first_fault_observation_reset();
  if (runtime_machine_segment_access_observation_copy(&access) ||
      runtime_machine_first_fault_observation_observed()) return 6;
  return 0;
}
