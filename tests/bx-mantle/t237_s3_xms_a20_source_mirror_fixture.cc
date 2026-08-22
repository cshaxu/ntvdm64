#include "bochs.h"
#include "bx-vdm/bx_ntvdm_cpu_state_abi.h"
#include "bx-mantle/bx_ntvdm_a20_capability_v1.h"
#include "bx-mantle/bx_ntvdm_generic_ud_bridge.h"
#include "bx-mantle/bx_ntvdm_minimal_machine.h"

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

static int invoke_a20(unsigned operation, unsigned expected_ax)
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
  event.window[3] = 0u;
  event.eax = operation;
  memset(&outcome, 0, sizeof(outcome));
  return bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) &&
    outcome.disposition == BX_NTVDM_GENERIC_UD_RESUME &&
    outcome.resume_rip == UINT64_C(0x2a04) &&
    outcome.gpr16_values[0] == expected_ax;
}

int main()
{
  bx_ntvdm_minimal_machine_c machine;
  if (machine.initialize(16u * 1024u * 1024u, 16u * 1024u * 1024u) !=
      BX_NTVDM_MINIMAL_MACHINE_OK) return 1;
  if (!bx_ntvdm_xms_v2_runtime_session_bind(8192u)) return 2;

  /* Original xmsa20.c: 0 means enable 20-bit wrapping (A20 off), 1 means
   * disable wrapping (A20 on), and 2 queries the resulting source state. */
  if (!a20_enabled(1u)) return 3;
  if (!invoke_a20(0u, 1u) || !a20_enabled(0u)) return 4;
  if (!invoke_a20(2u, 0u) || !a20_enabled(0u)) return 5;
  if (!invoke_a20(1u, 1u) || !a20_enabled(1u)) return 6;
  if (!invoke_a20(2u, 1u) || !a20_enabled(1u)) return 7;

  bx_ntvdm_xms_v2_runtime_session_reset();
  return machine.cleanup() == BX_NTVDM_MINIMAL_MACHINE_OK ? 0 : 8;
}
