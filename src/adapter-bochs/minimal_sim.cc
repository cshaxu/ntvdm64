/////////////////////////////////////////////////////////////////////////
//
// Finite native SIM parameter provider for the CPU5/Pentium MMX
// bare-machine contract.  This intentionally does not start Bochs product
// configuration, GUI, plugin, device, or firmware composition.
//
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "param_names.h"
#include "adapter-bochs/minimal_sim.h"

// These globals are consumed by the original parameter-tree implementation.
// They normally live in gui/siminterface.cc, which is deliberately not part of
// the minimal lifecycle.
bx_simulator_interface_c *SIM = NULL;
logfunctions *siminterface_log = NULL;
bx_list_c *root_param = NULL;

// CPU5/Pentium-MMX has the original legacy local-APIC identity shape.  The
// historical product derives these globals from its broader CPUID parameter
// tree; the finite mantle has no xAPIC-capable model to select.
Bit32u apic_id_mask = 0x0f;
bx_bool simulate_xapic = 0;

static bx_param_c *runtime_find_param(const char *from, bx_param_c *base)
{
  const char *separator = strchr(from, '.');
  size_t length = separator ? (size_t) (separator - from) : strlen(from);
  char component[128];

  if (length == 0 || length >= sizeof(component) || base == NULL ||
      base->get_type() != BXT_LIST) {
    return NULL;
  }

  memcpy(component, from, length);
  component[length] = 0;
  bx_param_c *child = ((bx_list_c *) base)->get_by_name(component);
  if (child == NULL || separator == NULL) {
    return child;
  }
  return runtime_find_param(separator + 1, child);
}

class bx_mantle_minimal_sim_c : public bx_simulator_interface_c {
public:
  bx_mantle_minimal_sim_c() { is_sim_thread_func = NULL; }

  virtual bx_param_c *get_param(const char *pname, bx_param_c *base = NULL)
  {
    if (base == NULL) base = root_param;
    if (pname == NULL || base == NULL) return NULL;
    if (pname[0] == '.' && pname[1] == 0) return base;
    return runtime_find_param(pname, base);
  }

  virtual bx_param_num_c *get_param_num(const char *pname, bx_param_c *base = NULL)
  {
    bx_param_c *param = get_param(pname, base);
    if (param == NULL) return NULL;
    Bit8u type = param->get_type();
    return (type == BXT_PARAM_NUM || type == BXT_PARAM_BOOL || type == BXT_PARAM_ENUM)
      ? (bx_param_num_c *) param : NULL;
  }

  virtual bx_param_string_c *get_param_string(const char *pname, bx_param_c *base = NULL)
  {
    bx_param_c *param = get_param(pname, base);
    return (param != NULL && param->get_type() == BXT_PARAM_STRING)
      ? (bx_param_string_c *) param : NULL;
  }

  virtual bx_param_bool_c *get_param_bool(const char *pname, bx_param_c *base = NULL)
  {
    bx_param_c *param = get_param(pname, base);
    return (param != NULL && param->get_type() == BXT_PARAM_BOOL)
      ? (bx_param_bool_c *) param : NULL;
  }

  virtual bx_param_enum_c *get_param_enum(const char *pname, bx_param_c *base = NULL)
  {
    bx_param_c *param = get_param(pname, base);
    return (param != NULL && param->get_type() == BXT_PARAM_ENUM)
      ? (bx_param_enum_c *) param : NULL;
  }

  virtual bx_list_c *get_bochs_root() { return root_param; }
};

static bx_mantle_minimal_sim_c *bx_mantle_minimal_sim = NULL;

bx_mantle_minimal_sim_status bx_mantle_minimal_sim_initialize(void)
{
#if BX_CPU_LEVEL != 5 || BX_SUPPORT_X86_64
  return BX_MANTLE_MINIMAL_SIM_PROFILE_CONFIGURATION_UNSUPPORTED;
#else
  if (SIM != NULL && SIM != bx_mantle_minimal_sim) {
    return BX_MANTLE_MINIMAL_SIM_ALREADY_BOUND;
  }

  if (SIM != NULL) {
    bx_param_bool_c *reset = SIM->get_param_bool(BXPN_RESET_ON_TRIPLE_FAULT);
    return (reset != NULL && reset->get() == 0)
      ? BX_MANTLE_MINIMAL_SIM_OK
      : BX_MANTLE_MINIMAL_SIM_RESET_PARAM_NOT_FALSE;
  }

  siminterface_log = new logfunctions();
  siminterface_log->put("minimal_sim", "CTRL");
  bx_mantle_minimal_sim = new bx_mantle_minimal_sim_c();
  SIM = bx_mantle_minimal_sim;
  root_param = new bx_list_c(NULL, "bochs", "minimal Bochs parameter root");

  bx_list_c *cpu = new bx_list_c(root_param, "cpu", "CPU Options");
  static const char *cpu_names[] = {
#define bx_define_cpudb(model) #model,
#include "bochs-core/cpudb.h"
    NULL
  };
#undef bx_define_cpudb

  new bx_param_enum_c(cpu, "model", "CPU configuration",
      "Fixed CPU5/Pentium MMX bare-machine configuration", cpu_names,
      bx_cpudb_pentium_mmx, 0);
  new bx_param_bool_c(cpu, "reset_on_triple_fault",
      "Enable CPU reset on triple fault",
      "The bare machine preserves the original non-reset triple-fault path", 0);
  new bx_param_bool_c(cpu, "ignore_bad_msrs", "Ignore bad MSRs",
      "Do not ignore invalid MSR accesses", 0);
  new bx_param_string_c(cpu, "msrs", "Configurable MSR definitions",
      "No configurable MSR file is supplied by the bare machine", "");

  bx_param_bool_c *reset = SIM->get_param_bool(BXPN_RESET_ON_TRIPLE_FAULT);
  if (root_param->get_size() != 1 || cpu->get_size() != 4) {
    return BX_MANTLE_MINIMAL_SIM_CPU_LAYOUT_INVALID;
  }
  if (reset == NULL || reset->get() != 0) {
    return BX_MANTLE_MINIMAL_SIM_RESET_PARAM_NOT_FALSE;
  }
  return BX_MANTLE_MINIMAL_SIM_OK;
#endif
}
