#include "bochs.h"
#include "cpu/cpu.h"
#include "adapter-softpc/machine_stage.h"
extern "C" {
#include "opennt-bop/ingress/command_native_session.h"
}

#include <string.h>
#include <stdio.h>
#include <windows.h>

static Bit32u g_command_cr_index;

static int action_append(struct runtime_mechanical_action_v1 *action,
  Bit64u physical, const void *bytes, Bit32u byte_count)
{
  Bit32u index = action->range_count;
  if (index >= RUNTIME_MECHANICAL_ACTION_V1_MAX_RANGES || byte_count == 0u ||
      action->payload_bytes > RUNTIME_MECHANICAL_ACTION_V1_MAX_BYTES - byte_count)
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
  static const Bit32u standard_handles[3] = {
    UINT32_MAX, UINT32_MAX, UINT32_MAX
  };
  static const Bit8u preserved[] = { 0x10u, 0x20u, 0x30u, 0x40u };
  char command[MAX_PATH + 16u];
  char environment[MAX_PATH + 32u];
  DWORD system_root_bytes;
  DWORD comspec_bytes;
  Bit32u command_bytes;
  struct runtime_machine_stage_v1_request request;

  /* Direct `54:08` preserves OpenNT's direct-executable path: the guest tail
   * names the public COMSPEC executable, while AH=0 avoids conflating this
   * worker witness with the separately audited shell-expansion branch. */
  comspec_bytes = GetEnvironmentVariableA("COMSPEC", command,
    (DWORD)sizeof(command));
  if (comspec_bytes == 0u || comspec_bytes >= sizeof(command) -
      sizeof(" /c exit 37\r")) return 0;
  memcpy(command + comspec_bytes, " /c exit 37\r", sizeof(" /c exit 37\r"));
  command_bytes = comspec_bytes + (Bit32u)sizeof(" /c exit 37\r");
  g_command_cr_index = command_bytes - 2u;
  system_root_bytes = GetEnvironmentVariableA("SystemRoot", environment +
    sizeof("T236=NATIVE\0SystemRoot=") - 1u,
    (DWORD)(sizeof(environment) - (sizeof("T236=NATIVE\0SystemRoot=") - 1u)));
  if (system_root_bytes == 0u || system_root_bytes >=
      sizeof(environment) - (sizeof("T236=NATIVE\0SystemRoot=") - 1u)) return 0;
  memcpy(environment, "T236=NATIVE\0SystemRoot=", sizeof("T236=NATIVE\0SystemRoot=") - 1u);
  environment[sizeof("T236=NATIVE\0SystemRoot=") - 1u + system_root_bytes] = '\0';
  environment[sizeof("T236=NATIVE\0SystemRoot=") + system_root_bytes] = '\0';

  runtime_machine_stage_v1_request_clear(&request);
  runtime_mechanical_action_v1_clear(&request.initial_state_action);
  request.initial_state_action.action_id = 1u;
  request.initial_state_action.kind = RUNTIME_MECHANICAL_ACTION_V1_WRITE;
  if (!action_append(&request.initial_state_action, 0x714u, preserved,
      sizeof(preserved))) return 0;

  runtime_mechanical_action_v1_clear(&request.startup_action);
  request.startup_action.action_id = 2u;
  request.startup_action.kind = RUNTIME_MECHANICAL_ACTION_V1_WRITE;
  if (!action_append(&request.startup_action, 0x700u, code, sizeof(code)) ||
      !action_append(&request.startup_action, 0x1000u, command, command_bytes) ||
      !action_append(&request.startup_action, 0x2000u, environment,
          (Bit32u)(sizeof("T236=NATIVE\0SystemRoot=") + system_root_bytes + 1u)) ||
      !action_append(&request.startup_action, 0x6000u, standard_handles,
          sizeof(standard_handles))) return 0;
  request.preserved_state_address = 0x714u;
  request.preserved_state_bytes = sizeof(preserved);
  if (!runtime_machine_stage_v1_request_valid(&request) ||
      runtime_machine_stage_v1_begin(&request) != RUNTIME_MACHINE_STAGE_V1_OK)
    return 0;

  /* Test-only direct core state setup.  The product's public mantle contract
   * remains CS:IP-only; this fixture supplies the historical 54:08 input
   * registers so the actual CPU can raise its own #UD. */
  bx_cpu.apply_real_mode_state(0x0002u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 2u,
    0x70u, 0x100u, 0x200u, 0x600u, 0u, 0u);
  return 1;
}

static uint32_t execute_slice(void)
{
  struct runtime_machine_stage_v1_execution_request request;
  runtime_machine_stage_v1_execution_request_clear(&request);
  request.ips = 1000000u;
  request.instruction_tick_budget = 1000000u;
  return runtime_machine_stage_v1_execute(&request);
}

int main()
{
  uint32_t attempt, status = 0u;
  Bit8u observed_command[MAX_PATH + 16u];
  runtime_command_native_session session;
  if (!begin_native_cmdexec_stage() ||
      !runtime_command_native_session_initialize(&session) ||
      !runtime_command_native_session_bind(&session)) return 1;
  if (execute_slice() != RUNTIME_MACHINE_STAGE_V1_EXECUTION_PENDING) return 2;
  for (attempt = 0u; attempt < 100u; ++attempt) {
    Sleep(10u);
    status = execute_slice();
    if (status != RUNTIME_MACHINE_STAGE_V1_EXECUTION_PENDING) break;
  }
  /* cmdExec documents AL as the child return code.  This fixture selects
   * AH=0 (the direct-executable branch), so the untouched high byte remains
   * zero: AX is 0025, not the former AH=1 test input's 0125. */
  if (status != RUNTIME_MACHINE_STAGE_V1_EXECUTION_BUDGET ||
      bx_cpu.get_reg16(0u) != 0x0025u ||
      !bx_mem.copy_from_ordinary_ram(0x1000u, g_command_cr_index + 2u,
        observed_command) || observed_command[g_command_cr_index] != 0u) {
    fprintf(stderr, "native cmdexec32 failure: status=%lu ax=%04x child=%lu command-cr=%02x create=%lu error=%lu env=%lu/%02lx command=\"%s\"\\n",
      (unsigned long)status, (unsigned)bx_cpu.get_reg16(0u),
      (unsigned long)session.direct.local_child_exit_code,
      (unsigned)observed_command[g_command_cr_index],
      (unsigned long)session.direct.create_process_attempted,
      (unsigned long)session.direct.create_process_last_error,
      (unsigned long)session.direct.create_process_environment_bytes,
      (unsigned long)session.direct.create_process_environment_flags,
      session.direct.pending.command);
    return 3;
  }
  runtime_command_native_session_unbind(&session);
  return runtime_machine_stage_v1_reset() == RUNTIME_MACHINE_STAGE_V1_OK ? 0 : 4;
}
