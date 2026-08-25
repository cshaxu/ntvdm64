/////////////////////////////////////////////////////////////////////////
// DIVERGENCE(BX-CORE-DIV-003): private copied physical-write observer.
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#include "bochs-core-overlay/cpu/opaque_callback_private.h"

struct bochs_core_overlay_physical_write_v1 {
  Bit32u magic, abi_version, struct_bytes, valid;
  Bit64u physical_address;
  Bit32u byte_count, captured_bytes;
  Bit64u sequence, rip, cs_base;
  Bit16u cs, ss, sp, reserved0;
  Bit8u bytes[8];
  Bit32u opaque_tag;
};

void BX_CPU_C::overlay_observe_physical_write(Bit64u address, unsigned count,
  const void *bytes)
{
#if defined(BX_NTVDM_ENABLE_MANTLE_PHYSICAL_WRITE_OBSERVATION) && BX_NTVDM_ENABLE_MANTLE_PHYSICAL_WRITE_OBSERVATION
  bochs_core_overlay_physical_write_v1 event;
  unsigned copied = count > 8u ? 8u : count;
  memset(&event, 0, sizeof(event));
  event.magic=0x42505731u; event.abi_version=1u; event.struct_bytes=sizeof(event)-sizeof(event.opaque_tag);
  event.valid=1u; event.physical_address=address; event.byte_count=count; event.captured_bytes=copied;
  event.sequence=icount; event.rip=prev_rip;
  event.cs_base=sregs[BX_SEG_REG_CS].cache.u.segment.base;
  event.cs=sregs[BX_SEG_REG_CS].selector.value; event.ss=sregs[BX_SEG_REG_SS].selector.value;
  event.sp=get_reg16(BX_16BIT_REG_SP); event.opaque_tag=0x42585057u;
  if (bytes != 0 && copied != 0) memcpy(event.bytes, bytes, copied);
  (void)bochs_core_overlay_opaque_callback_v1_invoke(&event,sizeof(event),0,0);
#else
  (void)address; (void)count; (void)bytes;
#endif
}
