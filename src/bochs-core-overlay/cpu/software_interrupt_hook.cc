/////////////////////////////////////////////////////////////////////////
// DIVERGENCE(BX-CORE-DIV-003): private copied software-interrupt observer.
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#include "bochs-core-overlay/cpu/opaque_callback_private.h"

#ifndef RUNTIME_ENABLE_MACHINE_SOFTWARE_INTERRUPT_OBSERVATION
#define RUNTIME_ENABLE_MACHINE_SOFTWARE_INTERRUPT_OBSERVATION 0
#endif

struct bochs_core_overlay_software_interrupt {
  Bit32u version, cpu_id; Bit64u sequence, rip; Bit32u eflags;
  Bit16u cs, ss, sp, ax, bx, cx, dx, ds, es;
  Bit8u vector, execution_mode; Bit16u reserved0;
  Bit32u opaque_tag;
};

void BX_CPU_C::overlay_observe_software_interrupt(unsigned vector)
{
#if RUNTIME_ENABLE_MACHINE_SOFTWARE_INTERRUPT_OBSERVATION
  if (real_mode() || v8086_mode()) {
    bochs_core_overlay_software_interrupt record;
    memset(&record, 0, sizeof(record));
    record.version=1u; record.cpu_id=bx_cpuid; record.sequence=icount; record.rip=RIP;
    record.eflags=read_eflags(); record.cs=sregs[BX_SEG_REG_CS].selector.value;
    record.ss=sregs[BX_SEG_REG_SS].selector.value; record.sp=SP;
    record.ax=AX; record.bx=BX; record.cx=CX; record.dx=DX;
    record.ds=sregs[BX_SEG_REG_DS].selector.value; record.es=sregs[BX_SEG_REG_ES].selector.value;
    record.vector=(Bit8u)vector; record.execution_mode=real_mode()?1u:3u;
    record.opaque_tag=0x42585349u;
    (void)bochs_core_overlay_opaque_callback_invoke(&record,sizeof(record),0,0);
  }
#else
  (void)vector;
#endif
}
