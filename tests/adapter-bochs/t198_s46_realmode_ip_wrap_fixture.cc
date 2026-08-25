/////////////////////////////////////////////////////////////////////////
//
// Neutral real-mode CS:FFFF instruction-fetch witness.  The five-byte
// instruction is split intentionally: its first byte is at CS:FFFF and its
// remaining bytes, followed by HLT, are at CS:0000.  No guest artifact,
// BOP, device, or adapter input participates.
//
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "adapter-softpc/finite_run.h"

#include <stdio.h>
#include <string.h>

int main()
{
  static const Bit8u first_byte[] = { 0x2e };
  static const Bit8u wrapped_bytes[] = { 0x89, 0x1e, 0x18, 0x00, 0xf4 };
  runtime_finite_run_request request;
  runtime_finite_run_terminal_snapshot terminal;
  runtime_finite_run_status status;

  memset(&request, 0, sizeof(request));
  request.request_version = RUNTIME_FINITE_RUN_REQUEST_VERSION;
  memcpy(request.entry_bytes, first_byte, sizeof(first_byte));
  request.entry_byte_count = sizeof(first_byte);
  request.entry_physical_address = 0x1ffff;
  request.entry_cs = 0x1000;
  request.entry_eip = 0xffff;
  request.instruction_tick_budget = 64;
  request.ips = 1000000;
  request.has_preentry_action = 1;
  runtime_mechanical_action_clear(&request.preentry_action);
  request.preentry_action.action_id = 1;
  request.preentry_action.kind = RUNTIME_MECHANICAL_ACTION_WRITE;
  request.preentry_action.range_count = 1;
  request.preentry_action.payload_bytes = sizeof(wrapped_bytes);
  request.preentry_action.ranges[0].physical_address = 0x10000;
  request.preentry_action.ranges[0].byte_count = sizeof(wrapped_bytes);
  memcpy(request.preentry_action.payload, wrapped_bytes, sizeof(wrapped_bytes));
  request.capture_terminal_snapshot = 1;

  status = runtime_run_finite_bare_bytes(&request);
  /* The repaired generic path must decode across the 16-bit offset wrap,
   * advance the split instruction to offset 0004, then execute HLT. The
   * normal pre-execution advance makes the retained post-HLT IP 0005. */
  if (status != RUNTIME_FINITE_RUN_COMPLETED_BUDGET ||
      !runtime_finite_run_terminal_snapshot_get(&terminal)) return 1;
  fprintf(stdout, "t198-s46 terminal=%04x:%08x\n", terminal.cs,
    (unsigned) terminal.eip);
  return terminal.cs == 0x1000 && terminal.eip == 0x0005 ? 0 : 2;
}
