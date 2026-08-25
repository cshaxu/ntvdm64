/////////////////////////////////////////////////////////////////////////
// DIVERGENCE(BX-CORE-DIV-004): private copied IRET observation.
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#include "bochs-core-overlay/cpu/opaque_callback_private.h"

#ifndef RUNTIME_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
#if defined(BX_NTVDM_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION)
#define RUNTIME_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION BX_NTVDM_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
#else
#define RUNTIME_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION 0
#endif
#endif

enum { BOCHS_CORE_OVERLAY_INTERRUPT_RETURN_VERSION = 1u,
  BOCHS_CORE_OVERLAY_INTERRUPT_RETURN_TAG = 0x42584952u };

struct bochs_core_overlay_interrupt_return_v1 {
  Bit32u version, cpu_id;
  Bit64u sequence, rip;
  Bit32u eflags, sp;
  Bit16u cs, ss, ax, bx, cx, dx, ds, es;
  Bit8u execution_mode, operand_width;
  Bit16u reserved0;
  Bit32u opaque_tag;
};

void BX_CPU_C::overlay_observe_interrupt_return(unsigned width)
{
#if RUNTIME_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
  if (real_mode() || v8086_mode()) {
    bochs_core_overlay_interrupt_return_v1 record;
    memset(&record, 0, sizeof(record));
    record.version = BOCHS_CORE_OVERLAY_INTERRUPT_RETURN_VERSION;
    record.cpu_id = bx_cpuid;
    record.sequence = icount;
    record.rip = RIP;
    record.eflags = read_eflags();
    record.sp = ESP;
    record.cs = sregs[BX_SEG_REG_CS].selector.value;
    record.ss = sregs[BX_SEG_REG_SS].selector.value;
    record.ax = AX; record.bx = BX; record.cx = CX; record.dx = DX;
    record.ds = sregs[BX_SEG_REG_DS].selector.value;
    record.es = sregs[BX_SEG_REG_ES].selector.value;
    record.execution_mode = real_mode() ? 1u : 3u;
    record.operand_width = (Bit8u)width;
    record.opaque_tag = BOCHS_CORE_OVERLAY_INTERRUPT_RETURN_TAG;
    (void)bochs_core_overlay_opaque_callback_v1_invoke(&record, sizeof(record), 0, 0);
  }
#else
  (void)width;
#endif
}
