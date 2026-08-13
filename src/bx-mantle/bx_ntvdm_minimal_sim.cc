/////////////////////////////////////////////////////////////////////////
//
// Minimal original-SIM parameter initializer for the registered
// BX-MACH-023 triple-fault shutdown path.
//
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "bx_ntvdm_minimal_sim.h"

bx_ntvdm_minimal_sim_status bx_ntvdm_minimal_sim_initialize(void)
{
  bx_init_siminterface();

  bx_param_c *root_param = SIM->get_param(".");
  if (root_param == NULL || root_param->get_type() != BXT_LIST) {
    return BX_NTVDM_MINIMAL_SIM_ROOT_UNAVAILABLE;
  }

  bx_list_c *root = (bx_list_c *) root_param;
  bx_param_c *cpu_param = root->get_by_name("cpu");
  if (cpu_param == NULL) {
    if (root->get_size() != 0) {
      return BX_NTVDM_MINIMAL_SIM_ROOT_NOT_EMPTY;
    }

    bx_list_c *cpu = new bx_list_c(root, "cpu", "CPU Options");
    new bx_param_bool_c(cpu,
      "reset_on_triple_fault", "Enable CPU reset on triple fault",
      "Enable CPU reset if triple fault occured (highly recommended)", 0);
    return BX_NTVDM_MINIMAL_SIM_OK;
  }

  if (cpu_param->get_type() != BXT_LIST) {
    return BX_NTVDM_MINIMAL_SIM_CPU_NOT_LIST;
  }
  if (root->get_size() != 1) {
    return BX_NTVDM_MINIMAL_SIM_ROOT_NOT_EMPTY;
  }

  bx_list_c *cpu = (bx_list_c *) cpu_param;
  bx_param_c *reset_param = cpu->get_by_name("reset_on_triple_fault");
  if (reset_param == NULL || cpu->get_size() != 1) {
    return BX_NTVDM_MINIMAL_SIM_CPU_LAYOUT_INVALID;
  }
  if (reset_param->get_type() != BXT_PARAM_BOOL) {
    return BX_NTVDM_MINIMAL_SIM_RESET_PARAM_NOT_BOOL;
  }
  if (((bx_param_bool_c *) reset_param)->get() != 0) {
    return BX_NTVDM_MINIMAL_SIM_RESET_PARAM_NOT_FALSE;
  }

  return BX_NTVDM_MINIMAL_SIM_OK;
}
