#include "bochs.h"
#include "bx-vdm/bx_ntvdm_cpu_state_abi.h"
#include "bx-mantle/bx_ntvdm_a20_capability_v1.h"
#include "bx-mantle/bx_ntvdm_generic_ud_bridge.h"
#include "bx-mantle/bx_ntvdm_machine_stage_v1.h"

extern "C" {
#include "bop/xms_v2_runtime_session.h"
}

#include <string.h>

static int a20_enabled(unsigned expected)
{
  bx_ntvdm_a20_capability_request_v1 request = {
    BX_NTVDM_A20_CAPABILITY_V1_VERSION, BX_NTVDM_A20_CAPABILITY_QUERY, 0 };
  bx_ntvdm_a20_capability_result_v1 result;
  bx_ntvdm_a20_capability_v1_dispatch(&request, &result);
  return result.status == BX_NTVDM_A20_CAPABILITY_OK &&
    result.enabled == expected;
}

static int invoke_xms(unsigned service, unsigned ax, unsigned bx,
  unsigned expected_gpr16_mask, unsigned expected_ax)
{
  bx_ntvdm_generic_ud_event_v1 event;
  bx_ntvdm_generic_ud_outcome_v1 outcome;
  memset(&event, 0, sizeof(event));
  event.magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
  event.abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
  event.struct_bytes = sizeof(event);
  event.vector = 6u;
  event.execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
  event.fault_rip = UINT64_C(0x2a00);
  event.window_bytes = 4u;
  event.window[0] = 0xc4u;
  event.window[1] = 0xc4u;
  event.window[2] = 0x52u;
  event.window[3] = service;
  event.eax = ax;
  event.ebx = bx;
  memset(&outcome, 0, sizeof(outcome));
  return !(bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) &&
    outcome.disposition == BX_NTVDM_GENERIC_UD_RESUME &&
    outcome.resume_rip == UINT64_C(0x2a04) &&
    outcome.gpr16_write_mask == expected_gpr16_mask &&
    outcome.gpr16_values[0] == expected_ax);
}

int main()
{
  bx_ntvdm_machine_stage_v1_request machine_request;
  uint8_t zero = 0u;
  bx_ntvdm_machine_stage_v1_request_clear(&machine_request);
  bx_ntvdm_mechanical_action_v1_clear(&machine_request.initial_state_action);
  machine_request.initial_state_action.action_id = 1u;
  machine_request.initial_state_action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
  machine_request.initial_state_action.range_count = 1u;
  machine_request.initial_state_action.payload_bytes = 1u;
  machine_request.initial_state_action.ranges[0].physical_address = 0x2010u;
  machine_request.initial_state_action.ranges[0].byte_count = 1u;
  machine_request.initial_state_action.payload[0] = zero;
  bx_ntvdm_mechanical_action_v1_clear(&machine_request.startup_action);
  machine_request.startup_action.action_id = 2u;
  machine_request.startup_action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
  machine_request.startup_action.range_count = 1u;
  machine_request.startup_action.payload_bytes = 1u;
  machine_request.startup_action.ranges[0].physical_address = 0x2000u;
  machine_request.startup_action.ranges[0].byte_count = 1u;
  machine_request.startup_action.payload[0] = 0xf4u;
  machine_request.preserved_state_address = 0x2010u;
  machine_request.preserved_state_bytes = 1u;
  if (!bx_ntvdm_machine_stage_v1_request_valid(&machine_request) ||
      bx_ntvdm_machine_stage_v1_begin(&machine_request) != BX_NTVDM_MACHINE_STAGE_V1_OK)
    return 1;
  if (!bx_ntvdm_xms_v2_runtime_session_bind(8192u)) return 2;

  /* 52:06 remains in its original xmsInitUMB body.  Its AX:BX target is
   * retained as a checked real-mode address, then xmsa20.c writes the state
   * byte through the session RAM seam rather than through a host pointer. */
  /* `xmsInitUMB` itself has no AX result.  Its original observable output is
   * the A20 transition and the saved HIMEM byte, so a register write here
   * would be an adapter invention. */
  {
    int status = invoke_xms(6u, 0x0200u, 0x0010u, 0u, 0u);
    if (status != 0) return 20 + status;
  }
  {
    Bit8u state = 0xffu;
    if (!bx_mem.copy_from_ordinary_ram(0x2010u, 1u, &state) || state != 0u)
      return 10;
  }

  /* Original xmsa20.c: 0 means enable 20-bit wrapping (A20 off), 1 means
   * disable wrapping (A20 on), and 2 queries the resulting source state. */
  if (!a20_enabled(0u)) return 3;
  /* The original query branch writes AX and clears BL. */
  if (invoke_xms(0u, 2u, 0u, 1u | 8u, 0u) != 0 || !a20_enabled(0u)) return 4;
  if (invoke_xms(0u, 1u, 0u, 1u, 1u) != 0 || !a20_enabled(1u)) return 5;
  {
    Bit8u state = 0xffu;
    if (!bx_mem.copy_from_ordinary_ram(0x2010u, 1u, &state) || state != 1u)
      return 11;
  }
  if (invoke_xms(0u, 0u, 0u, 1u, 1u) != 0 || !a20_enabled(0u)) return 6;
  {
    Bit8u state = 0xffu;
    if (!bx_mem.copy_from_ordinary_ram(0x2010u, 1u, &state) || state != 0u)
      return 12;
  }
  if (invoke_xms(0u, 2u, 0u, 1u | 8u, 0u) != 0 || !a20_enabled(0u)) return 7;

  bx_ntvdm_xms_v2_runtime_session_reset();
  return bx_ntvdm_machine_stage_v1_reset() == BX_NTVDM_MACHINE_STAGE_V1_OK ? 0 : 8;
}
