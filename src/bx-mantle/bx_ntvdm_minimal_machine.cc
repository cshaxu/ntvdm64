/////////////////////////////////////////////////////////////////////////
//
// Minimal native-machine composition for the registered BX-MACH-026
// reset-window fixture boundary.
//
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "iodev/iodev.h"
#include "pc_system.h"
#include "bx_ntvdm_minimal_sim.h"
#include "bx_ntvdm_minimal_machine.h"

static logfunctions bx_ntvdm_minimal_machine_log;
logfunctions *pluginlog = &bx_ntvdm_minimal_machine_log;

bx_pc_system_c bx_pc_system;
bx_debug_t bx_dbg;
BOCHSAPI BX_CPU_C bx_cpu;
BOCHSAPI BX_MEM_C bx_mem;
bx_bool bx_user_quit = 0;

bx_ntvdm_minimal_machine_c::bx_ntvdm_minimal_machine_c()
{
  attempted = 0;
  memory_owned = 0;
  port_space_owned = 0;
}

bx_ntvdm_minimal_machine_status
bx_ntvdm_minimal_machine_c::initialize(Bit64u guest, Bit64u host)
{
  if (attempted) {
    return BX_NTVDM_MINIMAL_MACHINE_ALREADY_ATTEMPTED;
  }
  attempted = 1;

  SAFE_GET_IOFUNC();
  SAFE_GET_GENLOG();

  if (bx_ntvdm_minimal_sim_initialize() != BX_NTVDM_MINIMAL_SIM_OK) {
    return BX_NTVDM_MINIMAL_MACHINE_SIM_FAILED;
  }

  if (!bx_mem.init_memory_without_sim(guest, host)) {
    return BX_NTVDM_MINIMAL_MACHINE_MEMORY_FAILED;
  }
  memory_owned = 1;

  if (!bx_devices.init_empty_port_space()) {
    bx_mem.cleanup_memory();
    memory_owned = 0;
    return BX_NTVDM_MINIMAL_MACHINE_PORT_SPACE_FAILED;
  }
  port_space_owned = 1;

  bx_cpu.initialize();
  bx_cpu.reset(BX_RESET_HARDWARE);

  return BX_NTVDM_MINIMAL_MACHINE_OK;
}

bx_ntvdm_minimal_machine_status bx_ntvdm_minimal_machine_c::cleanup(void)
{
  if (port_space_owned) {
    if (!bx_devices.cleanup_empty_port_space()) {
      return BX_NTVDM_MINIMAL_MACHINE_PORT_SPACE_CLEANUP_FAILED;
    }
    port_space_owned = 0;
  }

  if (memory_owned) {
    bx_mem.cleanup_memory();
    memory_owned = 0;
  }

  return BX_NTVDM_MINIMAL_MACHINE_OK;
}
