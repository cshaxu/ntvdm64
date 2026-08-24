#include "dpmi_descriptor_source_shim.h"

#include "adapter-softpc/mechanical_action.h"
#include "adapter-softpc/protected_range_action.h"

#include <string.h>

static int current_cpu(runtime_cpu_state_v1 *cpu)
{
  return runtime_dpmi_startup_session_runtime_copy_cpu(cpu) &&
    cpu->execution_mode == RUNTIME_CPU_EXECUTION_PROTECTED;
}

USHORT runtime_dpmi_getAX(VOID)
{
  runtime_cpu_state_v1 cpu;
  return current_cpu(&cpu) ? (USHORT)cpu.eax : 0u;
}

USHORT runtime_dpmi_getBX(VOID)
{
  runtime_cpu_state_v1 cpu;
  return current_cpu(&cpu) ? (USHORT)cpu.ebx : 0u;
}

USHORT runtime_dpmi_getCX(VOID)
{
  runtime_cpu_state_v1 cpu;
  return current_cpu(&cpu) ? (USHORT)cpu.ecx : 0u;
}

USHORT runtime_dpmi_getES(VOID)
{
  runtime_cpu_state_v1 cpu;
  return current_cpu(&cpu) ? cpu.es : 0u;
}

VOID runtime_dpmi_setAX(USHORT value)
{
  runtime_dpmi_startup_session_runtime_set_ax(value);
}

BOOL runtime_dpmi_descriptor_source_acquire(LDT_ENTRY *descriptors,
  USHORT registerCX)
{
  struct runtime_protected_range_action_v1 action;
  uint32_t bytes = (uint32_t)registerCX * (uint32_t)sizeof(LDT_ENTRY);
  if (descriptors == 0 || registerCX == 0u ||
      registerCX > RUNTIME_DPMI_DESCRIPTOR_MAX_ENTRIES ||
      bytes > RUNTIME_PROTECTED_RANGE_ACTION_V1_MAX_BYTES) return FALSE;
  runtime_protected_range_action_v1_clear(&action);
  action.kind = RUNTIME_PROTECTED_RANGE_ACTION_V1_READ;
  action.segment = 0u; /* BX_SEG_REG_ES, a selector-blind machine register id. */
  action.offset = runtime_dpmi_getBX();
  action.byte_count = bytes;
  if (runtime_mantle_execute_protected_range_action_v1(&action) !=
      RUNTIME_PROTECTED_RANGE_ACTION_V1_OK) return FALSE;
  memcpy(descriptors, action.bytes, bytes);
  return TRUE;
}

BOOL runtime_dpmi_set_x86_descriptor(LDT_ENTRY *descriptors, USHORT registerAX,
  USHORT registerCX)
{
  const struct runtime_dpmi_startup_session *session;
  uint32_t bytes = (uint32_t)registerCX * (uint32_t)sizeof(LDT_ENTRY);
  uint32_t target;
  if (descriptors == 0 || registerCX == 0u ||
      registerAX % sizeof(LDT_ENTRY) != 0u ||
      registerCX > RUNTIME_DPMI_DESCRIPTOR_MAX_ENTRIES ||
      bytes > RUNTIME_PROTECTED_RANGE_ACTION_V1_MAX_BYTES ||
      (uint32_t)registerAX + bytes > UINT32_C(0x10000)) return FALSE;
  session = runtime_dpmi_startup_session_runtime_current();
  if (session == 0 || session->selector_table_linear == 0u ||
      session->selector_table_linear > UINT32_MAX - registerAX) return FALSE;
  target = session->selector_table_linear + registerAX;
  return runtime_mantle_checked_ram_write_v1(target,
    (const uint8_t *)descriptors, bytes) ? TRUE : FALSE;
}
