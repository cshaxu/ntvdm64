/////////////////////////////////////////////////////////////////////////
// DIVERGENCE(BX-CORE-DIV-003): private copied instruction-history observer.
/////////////////////////////////////////////////////////////////////////
#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#include "bochs-core-overlay/cpu/opaque_callback_private.h"

struct bochs_core_overlay_instruction_history_v1 {
  Bit32u version, cpu_id; Bit64u sequence, rip;
  Bit16u cs, ss, sp, bp, reserved0; Bit32u opaque_tag;
};
void BX_CPU_C::overlay_observe_instruction_history(void)
{
#if defined(BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY) && BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY
  bochs_core_overlay_instruction_history_v1 record;
  memset(&record, 0, sizeof(record));
  record.version=1u; record.cpu_id=bx_cpuid; record.sequence=icount; record.rip=prev_rip;
  record.cs=sregs[BX_SEG_REG_CS].selector.value; record.ss=sregs[BX_SEG_REG_SS].selector.value;
  record.sp=SP; record.bp=BP; record.opaque_tag=0x42584948u;
  (void)bochs_core_overlay_opaque_callback_v1_invoke(&record,sizeof(record),0,0);
#endif
}
