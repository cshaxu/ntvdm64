/////////////////////////////////////////////////////////////////////////
//
// Finite no-product companions required by original Bochs types retained in
// the native mantle.  They deliberately provide no GUI, device, plugin or
// interactive configuration lifecycle.
//
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "bochs-core/cpu/cpu.h"
#include "bochs-core/gui/paramtree.h"
#include "adapter-bochs/pc_system.h"

#define LOG_THIS bx_pc_system.

bx_gui_c *bx_gui = NULL;

int bx_atexit(void)
{
  // The minimal mantle owns no product resource that requires the historical
  // `main.cc` teardown sequence.  logfunctions::fatal exits immediately
  // after this return.
  return 0;
}

void bx_param_num_c::text_print(FILE *) {}
int bx_param_num_c::text_ask(FILE *, FILE *) { return -1; }
void bx_param_bool_c::text_print(FILE *) {}
int bx_param_bool_c::text_ask(FILE *, FILE *) { return -1; }
void bx_param_enum_c::text_print(FILE *) {}
int bx_param_enum_c::text_ask(FILE *, FILE *) { return -1; }
void bx_param_string_c::text_print(FILE *) {}
int bx_param_string_c::text_ask(FILE *, FILE *) { return -1; }
void bx_list_c::text_print(FILE *) {}
int bx_list_c::text_ask(FILE *, FILE *) { return -1; }

int bx_pc_system_c::Reset(unsigned type)
{
  BX_INFO(("minimal pc_system reset (%s)",
    type == BX_RESET_HARDWARE ? "HARDWARE" : "SOFTWARE"));

  set_enable_a20(1);
  for (int i = 0; i < BX_SMP_PROCESSORS; i++) {
    BX_CPU(i)->reset(type);
  }

  // No device lifecycle has been admitted.  The finite SIM keeps the only
  // historical triple-fault caller on the non-reset path.
  return type == BX_RESET_HARDWARE ? -1 : 0;
}

void bx_pc_system_c::exit(void)
{
  // Keep original timer baseline but do not enter full device/GUI teardown.
  numTimers = 1 + BX_SUPPORT_APIC;
}
