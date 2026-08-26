#include "bochs.h"
#include "adapter-softpc/cpu_state_abi.h"
#include "adapter-softpc/a20_capability.h"
#include "adapter-bop/generic_ud_bridge.h"
#include "app/bop/route.h"
#include "adapter-softpc/machine_stage.h"
#include "adapter-softpc/ivt_watch.h"

extern "C" {
#include "app/xms_session_binding.h"
}

#include <string.h>

static int a20_enabled(unsigned expected)
{
  runtime_a20_capability_request request = {
    RUNTIME_A20_CAPABILITY_VERSION, RUNTIME_A20_CAPABILITY_QUERY, 0 };
  runtime_a20_capability_result result;
  runtime_a20_capability_dispatch(&request, &result);
  return result.status == RUNTIME_A20_CAPABILITY_OK &&
    result.enabled == expected;
}

static int invoke_xms(unsigned service, unsigned ax, unsigned bx,
  unsigned expected_gpr16_mask, unsigned expected_ax)
{
  runtime_generic_ud_event event;
  runtime_generic_ud_outcome outcome;
  memset(&event, 0, sizeof(event));
  event.magic = RUNTIME_GENERIC_UD_EVENT_MAGIC;
  event.abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
  event.struct_bytes = sizeof(event);
  event.vector = 6u;
  event.execution_mode = RUNTIME_CPU_EXECUTION_REAL;
  event.fault_rip = UINT64_C(0x2a00);
  event.window_bytes = 4u;
  event.window[0] = 0xc4u;
  event.window[1] = 0xc4u;
  event.window[2] = 0x52u;
  event.window[3] = service;
  event.eax = ax;
  event.ebx = bx;
  memset(&outcome, 0, sizeof(outcome));
  return !(runtime_machine_generic_ud_bridge(&event, &outcome) &&
    outcome.disposition == RUNTIME_GENERIC_UD_RESUME &&
    outcome.resume_rip == UINT64_C(0x2a04) &&
    outcome.gpr16_write_mask == expected_gpr16_mask &&
    outcome.gpr16_values[0] == expected_ax);
}

int main()
{
    if (!runtime_bop_ingress_bind(runtime_opennt_bop_route_dispatch, 0)) return 90;
  runtime_machine_stage_request machine_request;
  uint8_t zero = 0u;
  runtime_machine_stage_request_clear(&machine_request);
  runtime_mechanical_action_clear(&machine_request.initial_state_action);
  machine_request.initial_state_action.action_id = 1u;
  machine_request.initial_state_action.kind = RUNTIME_MECHANICAL_ACTION_WRITE;
  machine_request.initial_state_action.range_count = 1u;
  machine_request.initial_state_action.payload_bytes = 1u;
  machine_request.initial_state_action.ranges[0].physical_address = 0x2010u;
  machine_request.initial_state_action.ranges[0].byte_count = 1u;
  machine_request.initial_state_action.payload[0] = zero;
  runtime_mechanical_action_clear(&machine_request.startup_action);
  machine_request.startup_action.action_id = 2u;
  machine_request.startup_action.kind = RUNTIME_MECHANICAL_ACTION_WRITE;
  machine_request.startup_action.range_count = 1u;
  machine_request.startup_action.payload_bytes = 1u;
  machine_request.startup_action.ranges[0].physical_address = 0x2000u;
  machine_request.startup_action.ranges[0].byte_count = 1u;
  machine_request.startup_action.payload[0] = 0xf4u;
  machine_request.preserved_state_address = 0x2010u;
  machine_request.preserved_state_bytes = 1u;
  machine_request.ivt_watch_enabled = 1u;
  machine_request.ivt_watch_vector = 0x15u;
  if (!runtime_machine_stage_request_valid(&machine_request) ||
      runtime_machine_stage_begin(&machine_request) != RUNTIME_MACHINE_STAGE_OK)
    return 1;
  if (!app_xms_session_bind(8192u)) return 2;

  /* The imported xmsmisc.c body calls the direct keybd_io.c fragment before
   * setting CX.  With the sampled IVT15 pair still 0000:0000, the first call
   * takes the original match/update path; the second observes the stale IVT
   * and takes the original clear path. */
  if (invoke_xms(9u, 0x4567u, 0u, 2u, 0u) != 0) return 13;
  {
    uint16_t offset = 0u, segment = 0u;
    if (!runtime_ivt_watch_copy_expected(0x15u, &offset, &segment) ||
        offset != 0x4567u || segment != 0u) return 14;
  }
  if (invoke_xms(9u, 0x1234u, 0u, 2u, 0u) != 0) return 15;
  {
    uint16_t offset = 1u, segment = 1u;
    if (!runtime_ivt_watch_copy_expected(0x15u, &offset, &segment) ||
        offset != 0u || segment != 0u) return 16;
  }

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

  app_xms_session_reset();
  return runtime_machine_stage_reset() == RUNTIME_MACHINE_STAGE_OK ? 0 : 8;
}
