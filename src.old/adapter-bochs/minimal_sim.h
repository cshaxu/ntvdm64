/////////////////////////////////////////////////////////////////////////
//
// Minimal original-SIM parameter initializer for the registered
// BX-MACH-023 triple-fault shutdown path.
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_MACHINE_MINIMAL_SIM_H
#define BX_MACHINE_MINIMAL_SIM_H

enum bx_machine_minimal_sim_status {
  BX_MACHINE_MINIMAL_SIM_OK = 0,
  BX_MACHINE_MINIMAL_SIM_ROOT_UNAVAILABLE,
  BX_MACHINE_MINIMAL_SIM_ROOT_NOT_EMPTY,
  BX_MACHINE_MINIMAL_SIM_CPU_NOT_LIST,
  BX_MACHINE_MINIMAL_SIM_CPU_LAYOUT_INVALID,
  BX_MACHINE_MINIMAL_SIM_RESET_PARAM_NOT_BOOL,
  BX_MACHINE_MINIMAL_SIM_RESET_PARAM_NOT_FALSE,
  BX_MACHINE_MINIMAL_SIM_PROFILE_CONFIGURATION_UNSUPPORTED,
  BX_MACHINE_MINIMAL_SIM_ALREADY_BOUND
};

// Creates the finite CPU5/Pentium MMX bare-machine parameter tree using
// original Bochs parameter types. The caller must compile with CPU level 5
// and x86-64 support disabled; this function never starts product SIM setup.
bx_machine_minimal_sim_status bx_machine_minimal_sim_initialize(void);

#endif