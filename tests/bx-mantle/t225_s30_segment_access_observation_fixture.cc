#include "adapter-softpc/bx_ntvdm_finite_run.h"
#include "adapter-softpc/bx_ntvdm_first_fault_observation_v1.h"
#include "adapter-softpc/bx_ntvdm_segment_access_observation_v1.h"
#include "adapter-softpc/bx_ntvdm_generic_ud_bridge.h"

extern "C" int bx_ntvdm_mantle_generic_ud_bridge_v1(
  const struct bx_ntvdm_generic_ud_event_v1 *,
  struct bx_ntvdm_generic_ud_outcome_v1 *)
{
  return 0;
}

#include <string.h>

int main(void)
{
  struct bx_ntvdm_finite_run_request request;
  struct bx_ntvdm_first_fault_observation_v1 fault;
  struct bx_ntvdm_segment_access_observation_v1 access;
  bx_ntvdm_finite_run_status status;

  memset(&access, 0, sizeof(access));
  access.magic = BX_NTVDM_SEGMENT_ACCESS_OBSERVATION_V1_MAGIC;
  access.abi_version = BX_NTVDM_SEGMENT_ACCESS_OBSERVATION_V1_VERSION;
  access.struct_bytes = sizeof(access);
  access.access_kind = BX_NTVDM_SEGMENT_ACCESS_KIND_V1_READ_WORD;
  access.branch_kind = BX_NTVDM_SEGMENT_ACCESS_BRANCH_V1_DIRECT_LIMIT;
  access.width = 2u;
  bx_ntvdm_mantle_segment_access_observation_enable(0);
  if (bx_ntvdm_mantle_segment_access_observation_v1(&access) ||
      bx_ntvdm_mantle_segment_access_observation_copy(&access)) return 10;

  memset(&request, 0, sizeof(request));
  request.request_version = BX_NTVDM_FINITE_RUN_REQUEST_VERSION;
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
  bx_ntvdm_mantle_segment_access_observation_enable(1);
  status = bx_ntvdm_run_finite_bare_bytes(&request);
  if (status != BX_NTVDM_FINITE_RUN_COMPLETED_FIRST_FAULT_STOP) return 1;
  if (!bx_ntvdm_mantle_first_fault_observation_observed()) return 2;
  memset(&fault, 0, sizeof(fault));
  if (!bx_ntvdm_mantle_first_fault_observation_copy(&fault) ||
      fault.vector != 13u || fault.cs != 0x0e00u || fault.eip != 0u) return 3;
  memset(&access, 0, sizeof(access));
  if (!bx_ntvdm_mantle_segment_access_observation_copy(&access) ||
      access.magic != BX_NTVDM_SEGMENT_ACCESS_OBSERVATION_V1_MAGIC ||
      access.abi_version != BX_NTVDM_SEGMENT_ACCESS_OBSERVATION_V1_VERSION ||
      access.struct_bytes != sizeof(access) ||
      access.access_kind != BX_NTVDM_SEGMENT_ACCESS_KIND_V1_READ_WORD ||
      access.branch_kind != BX_NTVDM_SEGMENT_ACCESS_BRANCH_V1_DIRECT_LIMIT ||
      access.width != 2u || access.offset != 0xffffu ||
      access.limit_scaled != 0xffffu || access.segment_selector != 0u) return 4;
  if (bx_ntvdm_mantle_segment_access_observation_v1(&access)) return 5;
  bx_ntvdm_mantle_segment_access_observation_enable(0);
  bx_ntvdm_mantle_segment_access_observation_reset();
  bx_ntvdm_mantle_first_fault_observation_reset();
  if (bx_ntvdm_mantle_segment_access_observation_copy(&access) ||
      bx_ntvdm_mantle_first_fault_observation_observed()) return 6;
  return 0;
}
