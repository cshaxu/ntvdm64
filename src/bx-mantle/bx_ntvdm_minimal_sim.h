/////////////////////////////////////////////////////////////////////////
//
// Minimal original-SIM parameter initializer for the registered
// BX-MACH-023 triple-fault shutdown path.
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_NTVDM_MINIMAL_SIM_H
#define BX_NTVDM_MINIMAL_SIM_H

enum bx_ntvdm_minimal_sim_status {
  BX_NTVDM_MINIMAL_SIM_OK = 0,
  BX_NTVDM_MINIMAL_SIM_ROOT_UNAVAILABLE,
  BX_NTVDM_MINIMAL_SIM_ROOT_NOT_EMPTY,
  BX_NTVDM_MINIMAL_SIM_CPU_NOT_LIST,
  BX_NTVDM_MINIMAL_SIM_CPU_LAYOUT_INVALID,
  BX_NTVDM_MINIMAL_SIM_RESET_PARAM_NOT_BOOL,
  BX_NTVDM_MINIMAL_SIM_RESET_PARAM_NOT_FALSE
};

// Creates only cpu.reset_on_triple_fault=false on an otherwise empty
// original SIM tree.  Existing state is accepted only when it exactly matches
// that invariant; no existing parameter is changed.
bx_ntvdm_minimal_sim_status bx_ntvdm_minimal_sim_initialize(void);

#endif
