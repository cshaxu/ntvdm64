#include "dpmi_descriptor_source_shim.h"

#include "adapter-softpc/bx_ntvdm_mechanical_action_v1.h"
#include "adapter-softpc/bx_ntvdm_protected_range_action_v1.h"

#include <string.h>

static int current_cpu(bx_ntvdm_cpu_state_v1 *cpu)
{
  return bx_ntvdm_dpmi_startup_session_runtime_copy_cpu(cpu) &&
    cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_PROTECTED;
}

USHORT bx_ntvdm_dpmi_getAX(VOID)
{
  bx_ntvdm_cpu_state_v1 cpu;
  return current_cpu(&cpu) ? (USHORT)cpu.eax : 0u;
}

USHORT bx_ntvdm_dpmi_getBX(VOID)
{
  bx_ntvdm_cpu_state_v1 cpu;
  return current_cpu(&cpu) ? (USHORT)cpu.ebx : 0u;
}

USHORT bx_ntvdm_dpmi_getCX(VOID)
{
  bx_ntvdm_cpu_state_v1 cpu;
  return current_cpu(&cpu) ? (USHORT)cpu.ecx : 0u;
}

USHORT bx_ntvdm_dpmi_getES(VOID)
{
  bx_ntvdm_cpu_state_v1 cpu;
  return current_cpu(&cpu) ? cpu.es : 0u;
}

VOID bx_ntvdm_dpmi_setAX(USHORT value)
{
  bx_ntvdm_dpmi_startup_session_runtime_set_ax(value);
}

BOOL bx_ntvdm_dpmi_descriptor_source_acquire(LDT_ENTRY *descriptors,
  USHORT registerCX)
{
  struct bx_ntvdm_protected_range_action_v1 action;
  uint32_t bytes = (uint32_t)registerCX * (uint32_t)sizeof(LDT_ENTRY);
  if (descriptors == 0 || registerCX == 0u ||
      registerCX > BX_NTVDM_DPMI_DESCRIPTOR_MAX_ENTRIES ||
      bytes > BX_NTVDM_PROTECTED_RANGE_ACTION_V1_MAX_BYTES) return FALSE;
  bx_ntvdm_protected_range_action_v1_clear(&action);
  action.kind = BX_NTVDM_PROTECTED_RANGE_ACTION_V1_READ;
  action.segment = 0u; /* BX_SEG_REG_ES, a selector-blind machine register id. */
  action.offset = bx_ntvdm_dpmi_getBX();
  action.byte_count = bytes;
  if (bx_ntvdm_mantle_execute_protected_range_action_v1(&action) !=
      BX_NTVDM_PROTECTED_RANGE_ACTION_V1_OK) return FALSE;
  memcpy(descriptors, action.bytes, bytes);
  return TRUE;
}

BOOL bx_ntvdm_dpmi_set_x86_descriptor(LDT_ENTRY *descriptors, USHORT registerAX,
  USHORT registerCX)
{
  const struct bx_ntvdm_dpmi_startup_session *session;
  uint32_t bytes = (uint32_t)registerCX * (uint32_t)sizeof(LDT_ENTRY);
  uint32_t target;
  if (descriptors == 0 || registerCX == 0u ||
      registerAX % sizeof(LDT_ENTRY) != 0u ||
      registerCX > BX_NTVDM_DPMI_DESCRIPTOR_MAX_ENTRIES ||
      bytes > BX_NTVDM_PROTECTED_RANGE_ACTION_V1_MAX_BYTES ||
      (uint32_t)registerAX + bytes > UINT32_C(0x10000)) return FALSE;
  session = bx_ntvdm_dpmi_startup_session_runtime_current();
  if (session == 0 || session->selector_table_linear == 0u ||
      session->selector_table_linear > UINT32_MAX - registerAX) return FALSE;
  target = session->selector_table_linear + registerAX;
  return bx_ntvdm_mantle_checked_ram_write_v1(target,
    (const uint8_t *)descriptors, bytes) ? TRUE : FALSE;
}
