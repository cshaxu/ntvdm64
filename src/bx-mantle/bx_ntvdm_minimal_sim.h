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
  BX_NTVDM_MINIMAL_SIM_RESET_PARAM_NOT_FALSE,
  BX_NTVDM_MINIMAL_SIM_PROFILE_CONFIGURATION_UNSUPPORTED,
  BX_NTVDM_MINIMAL_SIM_ALREADY_BOUND
};

// Creates the finite CPU5/Pentium MMX bare-machine parameter tree using
// original Bochs parameter types. The caller must compile with CPU level 5
// and x86-64 support disabled; this function never starts product SIM setup.
bx_ntvdm_minimal_sim_status bx_ntvdm_minimal_sim_initialize(void);

#endif
