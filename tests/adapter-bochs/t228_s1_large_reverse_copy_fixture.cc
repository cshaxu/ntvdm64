#include "bochs.h"
#include "adapter-mvdm-host-out/softpc/finite_run.h"
#include "adapter-mvdm-host-in/generic_ud_bridge.h"

#include <string.h>

extern "C" int runtime_machine_generic_ud_bridge(
  const struct runtime_generic_ud_event *event,
  struct runtime_generic_ud_outcome *outcome)
{
  if (event == 0 || outcome == 0 || event->vector != 6u) return 0;
  memset(outcome, 0, sizeof(*outcome));
  outcome->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
  outcome->disposition = RUNTIME_GENERIC_UD_STOP;
  return 1;
}

int main(void)
{
  static const Bit8u code[] = {
    0x0eu, 0x1fu,                         /* push cs; pop ds */
    0xb8u, 0x00u, 0x30u, 0x8eu, 0xc0u,     /* mov ax,3000h; mov es,ax */
    0xbeu, 0xcfu, 0xc5u,                   /* mov si,c5cfh */
    0xbfu, 0xcfu, 0xc5u,                   /* mov di,c5cfh */
    0xb9u, 0xd0u, 0xc5u,                   /* mov cx,c5d0h */
    0xfdu, 0xf3u, 0xa4u, 0xfcu,            /* std; rep movsb; cld */
    0x26u, 0xffu, 0x1eu, 0xf0u, 0x95u,     /* call far [es:95f0h] */
    0x0fu, 0x0bu                          /* fixture-only ud2 */
  };
  static struct runtime_finite_run_request request;
  static const Bit8u expected[] = {
    0xa5u, 0xa5u, 0xa5u, 0xa5u, 0xa5u, 0xa5u, 0xa5u, 0xa5u,
    0x93u, 0x01u, 0x41u, 0x00u,
    0xa5u, 0xa5u, 0xa5u, 0xa5u, 0xa5u, 0xa5u, 0xa5u, 0xa5u
  };
  struct runtime_finite_run_terminal_snapshot terminal;
  struct runtime_mechanical_action action;
  unsigned index;

  memset(&request, 0, sizeof(request));
  request.request_version = RUNTIME_FINITE_RUN_REQUEST_VERSION;
  memset(request.entry_bytes, 0xa5, 0xc5d0u);
  memcpy(request.entry_bytes, code, sizeof(code));
  request.entry_bytes[0x95f0u] = 0x93u;
  request.entry_bytes[0x95f1u] = 0x01u;
  request.entry_bytes[0x95f2u] = 0x41u;
  request.entry_bytes[0x95f3u] = 0x00u;
  request.entry_byte_count = 0xc5d0u;
  request.entry_physical_address = 0xe000u;
  request.entry_cs = 0x0e00u;
  request.instruction_tick_budget = 64u;
  request.ips = 1u;
  request.enable_realmode_segment_limit_compatibility = 1u;
  runtime_mechanical_action_clear(&action);
  action.kind = RUNTIME_MECHANICAL_ACTION_WRITE;
  action.action_id = 1u;
  action.range_count = 1u;
  action.payload_bytes = 1u;
  action.ranges[0].physical_address = 0x5a3u;
  action.ranges[0].byte_count = 1u;
  action.payload[0] = 0xcbu;               /* retf */
  request.has_preentry_action = 1u;
  request.preentry_action = action;
  request.capture_terminal_snapshot = 1u;
  if (!runtime_finite_run_terminal_snapshot_configure_ordinary_range(
      0x395e8u, sizeof(expected))) return 1;
  if (runtime_run_finite_bare_bytes(&request) !=
      RUNTIME_FINITE_RUN_COMPLETED_UD_STOP) return 2;
  if (!runtime_finite_run_terminal_snapshot_get(&terminal) ||
      terminal.captured_byte_count != sizeof(expected) ||
      terminal.captured_physical_address != 0x395e8u) return 3;
  for (index = 0u; index < sizeof(expected); ++index)
    if (terminal.captured_bytes[index] != expected[index]) return 4 + (int)index;
  return 0;
}
