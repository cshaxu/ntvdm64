/////////////////////////////////////////////////////////////////////////
// DIVERGENCE(BX-CORE-DIV-002): private copied segment-access observer.
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#include "bochs-core-overlay/cpu/opaque_callback_private.h"

#ifndef RUNTIME_ENABLE_MACHINE_SEGMENT_ACCESS_OBSERVER
#if defined(RUNTIME_ENABLE_MACHINE_SEGMENT_ACCESS_OBSERVER)
#define RUNTIME_ENABLE_MACHINE_SEGMENT_ACCESS_OBSERVER RUNTIME_ENABLE_MACHINE_SEGMENT_ACCESS_OBSERVER
#else
#define RUNTIME_ENABLE_MACHINE_SEGMENT_ACCESS_OBSERVER 0
#endif
#endif

enum { BOCHS_CORE_OVERLAY_SEGMENT_ACCESS_MAGIC = 0x42585331u };

struct bochs_core_overlay_segment_access {
  Bit32u magic, abi_version, struct_bytes, cpu_id;
  Bit32u access_kind, branch_kind, segment_index, width;
  Bit32u offset, limit_scaled, cache_valid, reserved0;
  Bit16u segment_selector, reserved1;
};

void BX_CPU_C::overlay_observe_segment_access(unsigned segment_index,
  const bx_segment_reg_t *segment, Bit32u offset, Bit32u branch_kind)
{
#if RUNTIME_ENABLE_MACHINE_SEGMENT_ACCESS_OBSERVER
  bochs_core_overlay_segment_access event;
  memset(&event, 0, sizeof(event));
  event.magic = BOCHS_CORE_OVERLAY_SEGMENT_ACCESS_MAGIC;
  event.abi_version = 1u;
  event.struct_bytes = sizeof(event);
  event.cpu_id = bx_cpuid;
  event.access_kind = 1u;
  event.branch_kind = branch_kind;
  event.segment_index = segment_index;
  event.width = 2u;
  event.offset = offset;
  event.limit_scaled = segment->cache.u.segment.limit_scaled;
  event.cache_valid = segment->cache.valid;
  event.segment_selector = segment->selector.value;
  (void)bochs_core_overlay_opaque_callback_invoke(&event, sizeof(event), 0, 0);
#else
  (void)segment_index; (void)segment; (void)offset; (void)branch_kind;
#endif
}
