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
  BX_NTVDM_FINITE_RUN_UNEXPECTED_LOOP_RETURN
};

struct bx_ntvdm_finite_run_request {
  const Bit8u *entry_bytes;
  Bit64u entry_byte_count;
  bx_phy_address entry_physical_address;
  Bit16u entry_cs;
  Bit32u entry_eip;
  Bit64u instruction_tick_budget;
  Bit32u ips;
  bx_bool stop_on_ud_fixture;
};

// The request is copied/consumed synchronously.  No pointer or guest state
// crosses a component boundary, and the byte sequence is opaque to mantle.
bx_ntvdm_finite_run_status bx_ntvdm_run_finite_bare_bytes(
  const bx_ntvdm_finite_run_request *request);

#endif
