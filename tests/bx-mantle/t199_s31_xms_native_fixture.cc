#include "bochs.h"
#include "bx-mantle/bx_ntvdm_minimal_machine.h"
#include "bx-vdm/bx_ntvdm_xms_package_session_v1.h"

static int call(bx_ntvdm_xms_package_session_v1 *session, unsigned service,
  unsigned ax, unsigned bx, unsigned dx, bx_ntvdm_cpu_result_v2 *result)
{
  Bit8u bytes[4] = { 0xc4, 0xc4, 0x52, (Bit8u)service };
  bx_ntvdm_instruction_window_v1 window;
  bx_ntvdm_bop_ingress_v1 ingress;
  bx_ntvdm_bop_provider_selection_v1 selection;
  bx_ntvdm_exception_event_v1 event = {};
  bx_ntvdm_cpu_state_v1 cpu;
  bx_ntvdm_instruction_window_v1_capture(&window, bytes, 4);
  if (!bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) ||
      !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection)) return 0;
  event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
  event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
  event.struct_bytes = sizeof(event);
  event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
  event.cpu_id = 1; event.vector = 6; event.fault_rip = 0x100;
  bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
  cpu.eax = ax; cpu.ebx = bx; cpu.edx = dx;
  return bx_ntvdm_xms_package_session_v1_dispatch(session, &ingress,
    &selection, &event, &cpu, &window, result);
}

int main()
{
  bx_ntvdm_minimal_machine_c machine;
  bx_ntvdm_xms_package_session_v1 session;
  bx_ntvdm_cpu_result_v2 result;
  if (machine.initialize(0x400000, 0x400000) != BX_NTVDM_MINIMAL_MACHINE_OK) return 1;
  if (!bx_ntvdm_xms_package_session_v1_initialize(&session)) return 2;
  if (!call(&session, 0, 0, 0, 0, &result) || result.cpu_delta.gpr16_values[0] != 1) return 3;
  if (!call(&session, 0, 2, 0, 0, &result) || result.cpu_delta.gpr16_values[0] != 0) return 4;
  if (!call(&session, 2, 0, 0, 64, &result) || result.cpu_delta.gpr16_values[0] != 1088) return 5;
  if (!call(&session, 11, 1088, 96, 64, &result) || result.cpu_delta.gpr16_values[1] != 1152) return 6;
  if (!call(&session, 3, 1152, 0, 96, &result) || result.cpu_delta.gpr16_values[0] != 1) return 7;
  if (!call(&session, 5, 0, 0, 0, &result) || result.cpu_delta.gpr16_values[0] != 3072) return 8;
  if (!call(&session, 1, 0, 0, 0, &result) || result.disposition != BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH) return 9;
  return machine.cleanup() == BX_NTVDM_MINIMAL_MACHINE_OK ? 0 : 10;
}
