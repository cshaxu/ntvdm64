/////////////////////////////////////////////////////////////////////////
//
// S56 scalar real/V86 segment-span compatibility regression.  The disabled
// branch preserves native Bochs #GP; the enabled branch executes ordinary
// real-mode word and dword stores/loads across FFFFh, then reaches a
// fixture-only #UD stop.  No BOP, adapter, guest artifact or provider joins.
//
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "adapter-softpc/finite_run.h"
#include "adapter-softpc/first_fault_observation.h"
#include "adapter-bop/generic_ud_bridge.h"
#include "bx-mantle/minimal_machine.h"
#include "bx-core/cpu/cpu.h"

#include <string.h>

extern "C" int runtime_mantle_generic_ud_bridge_v1(
  const struct runtime_generic_ud_event_v1 *event,
  struct runtime_generic_ud_outcome_v1 *outcome)
{
  if (event == 0 || outcome == 0 || event->vector != 6u) return 0;
  outcome->abi_version = RUNTIME_GENERIC_UD_EVENT_V1_VERSION;
  outcome->disposition = RUNTIME_GENERIC_UD_STOP;
  return 1;
}

int main(void)
{
  static const Bit8u disabled_code[] = { 0xa1, 0xff, 0xff };
  static const Bit8u enabled_code[] = {
    0xb8, 0x34, 0x12,             // mov ax,1234h
    0xa3, 0xff, 0xff,             // mov [ffffh],ax
    0xa1, 0xff, 0xff,             // mov ax,[ffffh]
    0xa3, 0xfc, 0xff,             // mov [fffch],ax
    0x66, 0xb8, 0x78, 0x56, 0x34, 0x12, // mov eax,12345678h
    0x66, 0xa3, 0xfe, 0xff,       // mov [fffeh],eax
    0x66, 0xa1, 0xfe, 0xff,       // mov eax,[fffeh]
    0x66, 0xa3, 0xf8, 0xff,       // mov [fff8h],eax
    0xff, 0x06, 0xff, 0xff,       // inc word [ffffh]: RMW crossing
    0xa1, 0xff, 0xff,             // mov ax,[ffffh]
    0xa3, 0xf4, 0xff,             // mov [fff4h],ax
    0xbe, 0xff, 0xff,             // mov si,ffffh
    0xbf, 0xf0, 0xff,             // mov di,fff0h
    0xb9, 0x01, 0x00,             // mov cx,1
    0xf3, 0xa5,                   // rep movsw: source crosses FFFFh
    0xb8, 0x34, 0x12,             // mov ax,1234h
    0xa3, 0x00, 0x08,             // mov [0800h],ax
    0xb8, 0x78, 0x56,             // mov ax,5678h
    0xa3, 0x02, 0x08,             // mov [0802h],ax
    0xbe, 0x03, 0x08,             // mov si,0803h
    0xbf, 0x13, 0x08,             // mov di,0813h
    0xb9, 0x04, 0x00,             // mov cx,4
    0xfd,                         // std
    0xf3, 0xa4,                   // rep movsb: reverse source/destination movement
    0xfc,                         // cld
    0xa1, 0x10, 0x08,             // mov ax,[0810h]
    0xa3, 0xe0, 0xff,             // mov [ffe0h],ax
    0xa1, 0x12, 0x08,             // mov ax,[0812h]
    0xa3, 0xe2, 0xff,             // mov [ffe2h],ax
    0xbc, 0x01, 0x00,             // mov sp,0001h
    0xb8, 0x9a, 0xbc,             // mov ax,bc9ah
    0x50,                         // push ax: SS:ffffh word crossing
    0x5b,                         // pop bx:  SS:ffffh word crossing
    0x89, 0xd8,                   // mov ax,bx
    0xa3, 0xf6, 0xff,             // mov [fff6h],ax
    0x0f, 0x0b                    // ud2: fixture-only controlled stop
  };
  runtime_finite_run_request request;
  runtime_finite_run_terminal_snapshot terminal;
  runtime_first_fault_observation_v1 fault;
  runtime_finite_run_status status;

  memset(&request, 0, sizeof(request));
  request.request_version = RUNTIME_FINITE_RUN_REQUEST_VERSION;
  memcpy(request.entry_bytes, disabled_code, sizeof(disabled_code));
  request.entry_byte_count = sizeof(disabled_code);
  request.entry_physical_address = 0xe000u;
  request.entry_cs = 0x0e00u;
  request.instruction_tick_budget = 64u;
  request.ips = 1u;
  request.stop_on_first_fault_fixture = 1u;
  status = runtime_run_finite_bare_bytes(&request);
  if (status != RUNTIME_FINITE_RUN_COMPLETED_FIRST_FAULT_STOP ||
      !runtime_mantle_first_fault_observation_observed()) return 1;
  memset(&fault, 0, sizeof(fault));
  if (!runtime_mantle_first_fault_observation_copy(&fault) ||
      fault.vector != 13u || fault.cs != 0x0e00u || fault.eip != 0u) return 2;

  memset(&request, 0, sizeof(request));
  request.request_version = RUNTIME_FINITE_RUN_REQUEST_VERSION;
  memcpy(request.entry_bytes, enabled_code, sizeof(enabled_code));
  request.entry_byte_count = sizeof(enabled_code);
  request.entry_physical_address = 0xe000u;
  request.entry_cs = 0x0e00u;
  request.instruction_tick_budget = 64u;
  request.ips = 1u;
  request.enable_realmode_segment_limit_compatibility = 1u;
  request.capture_terminal_snapshot = 1u;
  if (!runtime_finite_run_terminal_snapshot_configure_ordinary_range(
      0xffe0u, 34u)) return 3;
  status = runtime_run_finite_bare_bytes(&request);
  {
    static const Bit8u expected[] = {
      /* EndInit-equivalent STD/REP MOVSB result at 0810h, copied here. */
      0x34, 0x12, 0x78, 0x56,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00,
      0x57, 0x34, 0x00, 0x00, 0x57, 0x34, 0x9a, 0xbc, 0x78,
      0x56, 0x34, 0x12, 0x34, 0x12, 0x78, 0x9a, 0xbc, 0x12
    };
    unsigned index;
    if (status != RUNTIME_FINITE_RUN_COMPLETED_UD_STOP) return 40;
    if (!runtime_finite_run_terminal_snapshot_get(&terminal)) return 41;
    if (terminal.captured_byte_count != sizeof(expected)) return 42;
    for (index = 0; index < sizeof(expected); ++index) {
      if (terminal.captured_bytes[index] != expected[index]) return 43 + index;
    }
  }

  {
    static const Bit8u fetch_control_entry[] = {
      0x66, 0xea, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
    };
    static const Bit8u fetch_control_target[] = {
      0xb8, 0x78, 0x56, 0xa3, 0xf4, 0xff, 0x0f, 0x0b
    };
    static runtime_mechanical_action_v1 target_write;

    runtime_mechanical_action_v1_clear(&target_write);
    target_write.kind = RUNTIME_MECHANICAL_ACTION_V1_WRITE;
    target_write.action_id = 1u;
    target_write.range_count = 1u;
    target_write.payload_bytes = sizeof(fetch_control_target);
    target_write.ranges[0].physical_address = 0x10000u;
    target_write.ranges[0].byte_count = sizeof(fetch_control_target);
    memcpy(target_write.payload, fetch_control_target,
      sizeof(fetch_control_target));

    memset(&request, 0, sizeof(request));
    request.request_version = RUNTIME_FINITE_RUN_REQUEST_VERSION;
    memcpy(request.entry_bytes, fetch_control_entry,
      sizeof(fetch_control_entry));
    request.entry_byte_count = sizeof(fetch_control_entry);
    request.entry_physical_address = 0xfff0u;
    request.entry_cs = 0u;
    request.entry_eip = 0xfff0u;
    request.instruction_tick_budget = 64u;
    request.ips = 1u;
    request.stop_on_first_fault_fixture = 1u;
    request.has_preentry_action = 1u;
    request.preentry_action = target_write;
    status = runtime_run_finite_bare_bytes(&request);
    if (status != RUNTIME_FINITE_RUN_COMPLETED_FIRST_FAULT_STOP ||
        !runtime_mantle_first_fault_observation_observed()) return 5;
    memset(&fault, 0, sizeof(fault));
    if (!runtime_mantle_first_fault_observation_copy(&fault) ||
        fault.vector != 13u || fault.cs != 0u || fault.eip != 0xfff0u) return 6;

    request.stop_on_first_fault_fixture = 0u;
    request.enable_realmode_segment_limit_compatibility = 1u;
    request.capture_terminal_snapshot = 1u;
    if (!runtime_finite_run_terminal_snapshot_configure_ordinary_range(
        0xfff4u, 2u)) return 7;
    status = runtime_run_finite_bare_bytes(&request);
    if (status != RUNTIME_FINITE_RUN_COMPLETED_UD_STOP ||
        !runtime_finite_run_terminal_snapshot_get(&terminal) ||
        terminal.cs != 0u || terminal.eip != 0x10008u ||
        terminal.captured_byte_count != 2u ||
        terminal.captured_bytes[0] != 0x78u ||
        terminal.captured_bytes[1] != 0x56u) return 8;
  }

  {
    bx_mantle_minimal_machine_c machine;
    if (machine.initialize(0x100000u, 0x100000u) !=
        BX_MANTLE_MINIMAL_MACHINE_OK ||
        !machine.set_realmode_segment_limit_compatibility(1u)) return 5;
    bx_cpu.cpu_mode = BX_MODE_IA32_PROTECTED;
    if (machine.realmode_segment_limit_compatibility_active() ||
        machine.cleanup() != BX_MANTLE_MINIMAL_MACHINE_OK) return 6;
  }

  return 0;
}
