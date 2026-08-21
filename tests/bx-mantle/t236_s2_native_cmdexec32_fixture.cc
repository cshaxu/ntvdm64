#include "bochs.h"
#include "cpu/cpu.h"
#include "bx-mantle/bx_ntvdm_machine_stage_v1.h"
extern "C" {
#include "bop/shim/command_native_session_shim.h"
}

#include <string.h>
#include <windows.h>

static int action_append(struct bx_ntvdm_mechanical_action_v1 *action,
  Bit64u physical, const void *bytes, Bit32u byte_count)
{
  Bit32u index = action->range_count;
  if (index >= BX_NTVDM_MECHANICAL_ACTION_V1_MAX_RANGES || byte_count == 0u ||
      action->payload_bytes > BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES - byte_count)
    return 0;
  action->ranges[index].physical_address = physical;
  action->ranges[index].byte_count = byte_count;
  action->ranges[index].payload_offset = action->payload_bytes;
  memcpy(action->payload + action->payload_bytes, bytes, byte_count);
  action->payload_bytes += byte_count;
  action->range_count++;
  return 1;
}

static int begin_native_cmdexec_stage(void)
{
  static const Bit8u code[] = { 0xc4u, 0xc4u, 0x54u, 0x08u, 0xf4u };
  static const char command[] = "exit 37\r";
  static const char environment[] = "T236=NATIVE\0\0";
  static const Bit32u standard_handles[3] = {
    UINT32_MAX, UINT32_MAX, UINT32_MAX
  };
  static const Bit8u preserved[] = { 0x10u, 0x20u, 0x30u, 0x40u };
  struct bx_ntvdm_machine_stage_v1_request request;

  bx_ntvdm_machine_stage_v1_request_clear(&request);
  bx_ntvdm_mechanical_action_v1_clear(&request.initial_state_action);
  request.initial_state_action.action_id = 1u;
  request.initial_state_action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
  if (!action_append(&request.initial_state_action, 0x714u, preserved,
      sizeof(preserved))) return 0;

  bx_ntvdm_mechanical_action_v1_clear(&request.startup_action);
  request.startup_action.action_id = 2u;
  request.startup_action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
  if (!action_append(&request.startup_action, 0x700u, code, sizeof(code)) ||
      !action_append(&request.startup_action, 0x1000u, command, sizeof(command)) ||
      !action_append(&request.startup_action, 0x2000u, environment,
          sizeof(environment)) ||
      !action_append(&request.startup_action, 0x6000u, standard_handles,
          sizeof(standard_handles))) return 0;
  request.preserved_state_address = 0x714u;
  request.preserved_state_bytes = sizeof(preserved);
  if (!bx_ntvdm_machine_stage_v1_request_valid(&request) ||
      bx_ntvdm_machine_stage_v1_begin(&request) != BX_NTVDM_MACHINE_STAGE_V1_OK)
    return 0;

  /* Test-only direct core state setup.  The product's public mantle contract
   * remains CS:IP-only; this fixture supplies the historical 54:08 input
   * registers so the actual CPU can raise its own #UD. */
  bx_cpu.apply_real_mode_state(0x0102u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 2u,
    0x70u, 0x100u, 0x200u, 0x600u, 0u, 0u);
  return 1;
}

static uint32_t execute_slice(void)
{
  struct bx_ntvdm_machine_stage_v1_execution_request request;
  bx_ntvdm_machine_stage_v1_execution_request_clear(&request);
  request.ips = 1000000u;
  request.instruction_tick_budget = 1000000u;
  return bx_ntvdm_machine_stage_v1_execute(&request);
}

int main()
{
  uint32_t attempt, status = 0u;
  Bit8u observed_command[sizeof("exit 37\r")];
  bx_ntvdm_command_native_session session;
  if (!begin_native_cmdexec_stage() ||
      !bx_ntvdm_command_native_session_initialize(&session) ||
      !bx_ntvdm_command_native_session_bind(&session)) return 1;
  if (execute_slice() != BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_PENDING) return 2;
  for (attempt = 0u; attempt < 100u; ++attempt) {
    Sleep(10u);
    status = execute_slice();
    if (status != BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_PENDING) break;
  }
  if (status != BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_BUDGET ||
      bx_cpu.get_reg16(0u) != 0x0125u ||
      !bx_mem.copy_from_ordinary_ram(0x1000u, sizeof(observed_command),
        observed_command) || observed_command[7] != 0u) return 3;
  bx_ntvdm_command_native_session_unbind(&session);
  return bx_ntvdm_machine_stage_v1_reset() == BX_NTVDM_MACHINE_STAGE_V1_OK ? 0 : 4;
}
