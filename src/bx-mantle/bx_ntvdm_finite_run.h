/////////////////////////////////////////////////////////////////////////
//
// Finite bare-byte execution mechanic owned entirely by the native mantle.
// This is a C++-private fixture interface, not a cross-component or CLI ABI.
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_NTVDM_FINITE_RUN_H
#define BX_NTVDM_FINITE_RUN_H

#include "bochs.h"

enum bx_ntvdm_finite_run_status {
  BX_NTVDM_FINITE_RUN_COMPLETED_BUDGET = 0,
  BX_NTVDM_FINITE_RUN_REJECTED_INPUT,
  BX_NTVDM_FINITE_RUN_MACHINE_ERROR,
  BX_NTVDM_FINITE_RUN_UNEXPECTED_LOOP_RETURN,
  BX_NTVDM_FINITE_RUN_COMPLETED_UD_STOP,
  BX_NTVDM_FINITE_RUN_ENTRY_BYTES_MISMATCH
};

#define BX_NTVDM_FINITE_RUN_REQUEST_VERSION 2
#define BX_NTVDM_FINITE_RUN_MAX_ENTRY_BYTES 65536

struct bx_ntvdm_finite_run_terminal_snapshot {
  Bit8u valid;
  Bit8u reserved0;
  Bit16u cs;
  Bit32u eip;
};

struct bx_ntvdm_finite_run_request {
  Bit32u request_version;
  Bit8u entry_bytes[BX_NTVDM_FINITE_RUN_MAX_ENTRY_BYTES];
  Bit32u entry_byte_count;
  bx_phy_address entry_physical_address;
  Bit16u entry_cs;
  Bit32u entry_eip;
  Bit64u instruction_tick_budget;
  Bit32u ips;
  bx_bool stop_on_ud_fixture;
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
bx_ntvdm_finite_run_status bx_ntvdm_run_finite_bare_bytes(
  const bx_ntvdm_finite_run_request *request);

/* This private mantle query copies no more than the state already held by
 * CPU0 after a finite loop returns.  It does not expose a CPU object. */
void bx_ntvdm_finite_run_terminal_snapshot_clear(void);
bx_bool bx_ntvdm_finite_run_terminal_snapshot_get(
  bx_ntvdm_finite_run_terminal_snapshot *snapshot);

#endif
