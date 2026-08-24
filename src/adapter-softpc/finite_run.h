/////////////////////////////////////////////////////////////////////////
//
// Finite bare-byte execution mechanic owned entirely by the native mantle.
// This is a C++-private fixture interface, not a cross-component or CLI ABI.
//
/////////////////////////////////////////////////////////////////////////

#ifndef RUNTIME_FINITE_RUN_H
#define RUNTIME_FINITE_RUN_H

#include "bochs.h"
#include "mechanical_action.h"

enum runtime_finite_run_status {
  RUNTIME_FINITE_RUN_COMPLETED_BUDGET = 0,
  RUNTIME_FINITE_RUN_REJECTED_INPUT,
  RUNTIME_FINITE_RUN_MACHINE_ERROR,
  RUNTIME_FINITE_RUN_UNEXPECTED_LOOP_RETURN,
  RUNTIME_FINITE_RUN_COMPLETED_UD_STOP,
  RUNTIME_FINITE_RUN_COMPLETED_FIRST_FAULT_STOP
};

#define RUNTIME_FINITE_RUN_REQUEST_VERSION 4
#define RUNTIME_FINITE_RUN_MAX_ENTRY_BYTES 65536
#define RUNTIME_FINITE_RUN_TERMINAL_SNAPSHOT_MAX_BYTES 64

struct runtime_finite_run_terminal_snapshot {
  Bit8u valid;
  Bit8u captured_byte_count;
  Bit16u cs;
  Bit32u eip;
  bx_phy_address captured_physical_address;
  Bit8u captured_bytes[RUNTIME_FINITE_RUN_TERMINAL_SNAPSHOT_MAX_BYTES];
};

struct runtime_finite_run_request {
  Bit32u request_version;
  Bit8u entry_bytes[RUNTIME_FINITE_RUN_MAX_ENTRY_BYTES];
  Bit32u entry_byte_count;
  bx_phy_address entry_physical_address;
  Bit16u entry_cs;
  Bit32u entry_eip;
  Bit64u instruction_tick_budget;
  Bit32u ips;
  bx_bool stop_on_first_fault_fixture;
  /* Test-only mantle selection of the default-off CPU compatibility gate. */
  bx_bool enable_realmode_segment_limit_compatibility;
  /* Optional fixed copied ordinary-RAM action executed before any preserved
   * range is captured or entry bytes are placed.  Its content remains opaque
   * to this private mantle request. */
  bx_bool has_preentry_action;
  struct runtime_mechanical_action_v1 preentry_action;
  /* Optional opaque range captured before entry_bytes are published and
   * restored immediately afterwards.  This is a machine-only preservation
   * mechanic; its address and bytes have no guest-service meaning here. */
  bx_phy_address preserve_physical_address;
  Bit32u preserve_byte_count;
  /* Default-off terminal observation for fixture diagnostics only. */
  bx_bool capture_terminal_snapshot;
};

// The request carries copied fixed-width data only; the byte sequence is
// opaque to mantle.
runtime_finite_run_status runtime_run_finite_bare_bytes(
  const runtime_finite_run_request *request);

/* This private mantle query copies no more than the state already held by
 * CPU0 after a finite loop returns.  It does not expose a CPU object. */
void runtime_finite_run_terminal_snapshot_clear(void);
bx_bool runtime_finite_run_terminal_snapshot_get(
  runtime_finite_run_terminal_snapshot *snapshot);

/* One-shot, default-off fixture observation of opaque ordinary RAM after the
 * finite loop returns. It accepts no guest pointer and carries no guest or
 * service vocabulary. A zero length disables it; every nonzero range must
 * fit ordinary RAM and the fixed private capture bound. */
bx_bool runtime_finite_run_terminal_snapshot_configure_ordinary_range(
  bx_phy_address physical_address, Bit8u byte_count);

#endif
