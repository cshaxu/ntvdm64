/////////////////////////////////////////////////////////////////////////
//
// Minimal native-machine composition for the registered BX-MACH-026
// reset-window fixture boundary.
//
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "bochs-core/cpu/cpu.h"
#include "bochs-core/memory/memory.h"
#include "iodev/iodev.h"
#include "adapter-bochs/pic.h"
#include "adapter-bochs/pc_system.h"
#include "adapter-bochs/minimal_sim.h"
#include "adapter-bochs/minimal_machine.h"
#include "adapter-bochs/headless_8042.h"

static logfunctions bx_mantle_minimal_machine_log;
logfunctions *pluginlog = &bx_mantle_minimal_machine_log;

bx_pc_system_c bx_pc_system;
bx_debug_t bx_dbg;
BOCHSAPI BX_CPU_C bx_cpu;
BOCHSAPI BX_MEM_C bx_mem;
bx_bool bx_user_quit = 0;

bx_mantle_minimal_machine_c::bx_mantle_minimal_machine_c()
{
  attempted = 0;
  memory_owned = 0;
  port_space_owned = 0;
  pic_owned = 0;
  keyboard_owned = 0;
  cpu_initialized = 0;
  pic = NULL;
}

bx_mantle_minimal_machine_status
bx_mantle_minimal_machine_c::initialize(Bit64u guest, Bit64u host)
{
  if (attempted) {
    return BX_MANTLE_MINIMAL_MACHINE_ALREADY_ATTEMPTED;
  }
  attempted = 1;

  SAFE_GET_IOFUNC();
  SAFE_GET_GENLOG();

  if (bx_mantle_minimal_sim_initialize() != BX_MANTLE_MINIMAL_SIM_OK) {
    return BX_MANTLE_MINIMAL_MACHINE_SIM_FAILED;
  }

  if (!bx_mem.init_memory_without_sim(guest, host)) {
    return BX_MANTLE_MINIMAL_MACHINE_MEMORY_FAILED;
  }
  memory_owned = 1;

  if (!bx_devices.init_empty_port_space()) {
    bx_mem.cleanup_memory();
    memory_owned = 0;
    return BX_MANTLE_MINIMAL_MACHINE_PORT_SPACE_FAILED;
  }
  port_space_owned = 1;

  // BX-MANTLE-082-BEGIN
  // Fixed native 8259 assembly: no plugin registry, device discovery or
  // state-registration path is entered.
  pic = bx_mantle_pic_create();
  if (pic == NULL) {
    bx_devices.cleanup_empty_port_space();
    port_space_owned = 0;
    bx_mem.cleanup_memory();
    memory_owned = 0;
    return BX_MANTLE_MINIMAL_MACHINE_PIC_FAILED;
  }
  bx_devices.pluginPicDevice = pic;
  pic->init();
  pic_owned = 1;
  // BX-MANTLE-082-END

  bx_cpu.initialize();
  cpu_initialized = 1;
  bx_cpu.set_realmode_segment_limit_compatibility(0);
  bx_pc_system.set_enable_a20(1);
  bx_cpu.reset(BX_RESET_HARDWARE);
  return BX_MANTLE_MINIMAL_MACHINE_OK;
}

bx_mantle_minimal_machine_status
bx_mantle_minimal_machine_c::compose_headless_8042(void)
{
  if (keyboard_owned)
    return BX_MANTLE_MINIMAL_MACHINE_OK;
  if (!port_space_owned || !pic_owned)
    return BX_MANTLE_MINIMAL_MACHINE_KEYBOARD_FAILED;
  if (!bx_mantle_headless_8042_create_v1())
    return BX_MANTLE_MINIMAL_MACHINE_KEYBOARD_FAILED;
  keyboard_owned = 1;
  return BX_MANTLE_MINIMAL_MACHINE_OK;
}

bx_bool bx_mantle_minimal_machine_c::set_realmode_segment_limit_compatibility(
  bx_bool enabled)
{
  if (!cpu_initialized) return 0;
  bx_cpu.set_realmode_segment_limit_compatibility(enabled);
  return 1;
}

bx_bool bx_mantle_minimal_machine_c::realmode_segment_limit_compatibility_active(void)
{
  return cpu_initialized && bx_cpu.realmode_segment_limit_compatibility_active();
}

bx_mantle_minimal_machine_status bx_mantle_minimal_machine_c::cleanup(void)
{
  if (cpu_initialized) {
    bx_cpu.set_realmode_segment_limit_compatibility(0);
    cpu_initialized = 0;
  }
  if (pic_owned) {
    if (!pic->fini()) {
      return BX_MANTLE_MINIMAL_MACHINE_PIC_CLEANUP_FAILED;
    }
    bx_devices.pluginPicDevice = &bx_devices.stubPic;
    bx_mantle_pic_destroy(pic);
    pic = NULL;
    pic_owned = 0;
  }
  if (keyboard_owned) {
    if (!bx_mantle_headless_8042_destroy_v1())
      return BX_MANTLE_MINIMAL_MACHINE_KEYBOARD_CLEANUP_FAILED;
    keyboard_owned = 0;
  }

  if (port_space_owned) {
    if (!bx_devices.cleanup_empty_port_space()) {
      return BX_MANTLE_MINIMAL_MACHINE_PORT_SPACE_CLEANUP_FAILED;
    }
    port_space_owned = 0;
  }

  if (memory_owned) {
    bx_mem.cleanup_memory();
    memory_owned = 0;
  }
  return BX_MANTLE_MINIMAL_MACHINE_OK;
}
