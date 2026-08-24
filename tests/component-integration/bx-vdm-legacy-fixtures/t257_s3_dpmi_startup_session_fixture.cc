#include "bochs.h"
#include "adapter-bop/generic_ud_bridge.h"
#include "adapter-softpc/machine_stage.h"
#include "opennt-bop/dpmi/dpmi_startup_session_shim.h"
#include "opennt-bop/dpmi/dpmi_startup_source_shim.h"

#include <string.h>

extern "C" int runtime_mantle_generic_ud_bridge_v1(
  const struct runtime_generic_ud_event_v1 *event,
  struct runtime_generic_ud_outcome_v1 *outcome)
{
  (void)event;
  (void)outcome;
  return 0;
}

static void put16(uint8_t *bytes, uint32_t offset, uint16_t value)
{
  bytes[offset] = (uint8_t)value;
  bytes[offset + 1u] = (uint8_t)(value >> 8);
}

static void put32(uint8_t *bytes, uint32_t offset, uint32_t value)
{
  bytes[offset] = (uint8_t)value;
  bytes[offset + 1u] = (uint8_t)(value >> 8);
  bytes[offset + 2u] = (uint8_t)(value >> 16);
  bytes[offset + 3u] = (uint8_t)(value >> 24);
}

static int begin_stage(void)
{
  static const Bit8u halt[] = { 0xf4u };
  struct runtime_machine_stage_v1_request request;
  runtime_machine_stage_v1_request_clear(&request);
  request.guest_memory_bytes = UINT64_C(0x200000);
  runtime_mechanical_action_v1_clear(&request.startup_action);
  request.startup_action.action_id = 1u;
  request.startup_action.kind = RUNTIME_MECHANICAL_ACTION_V1_WRITE;
  request.startup_action.range_count = 1u;
  request.startup_action.payload_bytes = sizeof(halt);
  request.startup_action.ranges[0].physical_address = 0x700u;
  request.startup_action.ranges[0].byte_count = sizeof(halt);
  memcpy(request.startup_action.payload, halt, sizeof(halt));
  request.preserved_state_address = 0x600u;
  request.preserved_state_bytes = 1u;
  return runtime_machine_stage_v1_request_valid(&request) &&
    runtime_machine_stage_v1_begin(&request) == RUNTIME_MACHINE_STAGE_V1_OK;
}

int main(void)
{
  const struct runtime_dpmi_startup_session *session;
  runtime_cpu_state_v1 cpu;
  uint8_t shared[RUNTIME_DPMI_STARTUP_SESSION_SHARED_DATA_BYTES];
  const uint32_t address = 0x2800u;

  if (!begin_stage()) return 1;
  memset(shared, 0, sizeof(shared));
  put16(shared, 0u, 0x1234u);
  put32(shared, 2u, 0x00112233u);
  put32(shared, 6u, 0x00445566u);
  put32(shared, 10u, 0x00778899u);
  put16(shared, 14u, 0x0102u);
  put32(shared, 16u, 0x03040506u);
  put16(shared, 20u, 0x0708u);
  put32(shared, 22u, 0x090a0b0cu);
  put16(shared, 26u, 0x0d0eu);
  put16(shared, 28u, 0x0f10u);
  put32(shared, 30u, 0x11121314u);
  put32(shared, 34u, 0x15161718u);
  put32(shared, 38u, 0x191a1b1cu);
  put32(shared, 42u, 0x1d1e1f20u);
  put32(shared, 46u, 0x21222324u);
  put32(shared, 50u, 0x25262728u);
  put32(shared, 54u, 0x292a2b2cu);
  if (!runtime_mantle_checked_ram_write_v1(address, shared, sizeof(shared))) return 2;
  runtime_dpmi_startup_session_runtime_reset();
  if (!runtime_dpmi_startup_session_runtime_stage_dosx(address)) return 3;
  /* This invokes the imported OpenNT dpmi32.c body, not the seam directly. */
  DpmiInitDosx();
  session = runtime_dpmi_startup_session_runtime_current();
  if (session == 0 || session->dosx_stack_segment != 0x1234u ||
      session->small_xlat_buffer_linear != 0x00112233u ||
      session->dosx_stack_frame_size != 0x0102u ||
      session->dosx_iretd != 0x292a2b2cu) return 4;
  if (!runtime_dpmi_startup_session_runtime_stage_selector_table(0x3000u)) return 5;
  DpmiPassTableAddress();
  if (session->selector_table_linear != 0x3000u) return 6;
  runtime_cpu_state_v1_initialize(&cpu, RUNTIME_CPU_EXECUTION_PROTECTED);
  cpu.eax = 1u;
  if (!runtime_dpmi_startup_session_runtime_stage_app(&cpu, 0x3300u)) return 7;
  DpmiInitApp();
  if (session->current_app_flags != 1u || session->current_dta_linear != 0x3300u)
    return 8;
  runtime_cpu_state_v1_initialize(&cpu, RUNTIME_CPU_EXECUTION_REAL);
  if (!runtime_dpmi_startup_session_runtime_stage_app(&cpu, 0x3300u)) return 9;
  DpmiInitApp();
  if (session->current_app_flags != 1u) return 10;
  if (runtime_machine_stage_v1_reset() != RUNTIME_MACHINE_STAGE_V1_OK) return 11;
  return 0;
}
