/////////////////////////////////////////////////////////////////////////
//
// Finite bare-byte execution mechanic owned entirely by the native mantle.
// This is a C++-private fixture interface, not a cross-component or CLI ABI.
//
/////////////////////////////////////////////////////////////////////////

#ifndef RUNTIME_FINITE_RUN_H
#define RUNTIME_FINITE_RUN_H

#include <stdint.h>
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
  uint8_t valid;
  uint8_t captured_byte_count;
  uint16_t cs;
  uint32_t eip;
  uint64_t captured_physical_address;
  uint8_t captured_bytes[RUNTIME_FINITE_RUN_TERMINAL_SNAPSHOT_MAX_BYTES];
};

struct runtime_finite_run_request {
  uint32_t request_version;
  uint8_t entry_bytes[RUNTIME_FINITE_RUN_MAX_ENTRY_BYTES];
  uint32_t entry_byte_count;
  uint64_t entry_physical_address;
  uint16_t entry_cs;
  uint32_t entry_eip;
  uint64_t instruction_tick_budget;
  uint32_t ips;
  uint32_t stop_on_first_fault_fixture;
  /* Test-only mantle selection of the default-off CPU compatibility gate. */
  uint32_t enable_realmode_segment_limit_compatibility;
  /* Optional fixed copied ordinary-RAM action executed before any preserved
   * range is captured or entry bytes are placed.  Its content remains opaque
   * to this private mantle request. */
  uint32_t has_preentry_action;
  struct runtime_mechanical_action_v1 preentry_action;
  /* Optional opaque range captured before entry_bytes are published and
   * restored immediately afterwards.  This is a machine-only preservation
   * mechanic; its address and bytes have no guest-service meaning here. */
  uint64_t preserve_physical_address;
  uint32_t preserve_byte_count;
  /* Default-off terminal observation for fixture diagnostics only. */
  uint32_t capture_terminal_snapshot;
};

// The request carries copied fixed-width data only; the byte sequence is
// opaque to mantle.
runtime_finite_run_status runtime_run_finite_bare_bytes(
  const runtime_finite_run_request *request);

/* This private mantle query copies no more than the state already held by
 * CPU0 after a finite loop returns.  It does not expose a CPU object. */
void runtime_finite_run_terminal_snapshot_clear(void);
int runtime_finite_run_terminal_snapshot_get(
  runtime_finite_run_terminal_snapshot *snapshot);

/* One-shot, default-off fixture observation of opaque ordinary RAM after the
 * finite loop returns. It accepts no guest pointer and carries no guest or
 * service vocabulary. A zero length disables it; every nonzero range must
 * fit ordinary RAM and the fixed private capture bound. */
int runtime_finite_run_terminal_snapshot_configure_ordinary_range(
  uint64_t physical_address, uint8_t byte_count);

#endif
