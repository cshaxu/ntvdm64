#include "bochs.h"
#include "bx-core/cpu/cpu.h"
#include "adapter-softpc/bx_ntvdm_machine_stage_v1.h"
#include "bx-mantle/pc_system.h"
#include "adapter-softpc/bx_ntvdm_instruction_history.h"
#include "adapter-bop/bx_ntvdm_generic_ud_bridge.h"

#include <string.h>

#ifndef BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
#define BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE 0
#endif

static int stop_on_ud;
static int untracked_stop_fired;

static void untracked_stop(void *)
{
  untracked_stop_fired = 1;
  bx_pc_system.kill_bochs_request = 1;
  bx_cpu.async_event = 1;
}

extern "C" int bx_ntvdm_mantle_generic_ud_bridge_v1(
  const struct bx_ntvdm_generic_ud_event_v1 *event,
  struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
  if (!stop_on_ud || event == 0 || outcome == 0 || event->vector != 6u)
    return 0;
  memset(outcome, 0, sizeof(*outcome));
  outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
  outcome->disposition = BX_NTVDM_GENERIC_UD_STOP;
  bx_ntvdm_mantle_generic_ud_stop_observation_mark();
  return 1;
}

static void write_action(struct bx_ntvdm_mechanical_action_v1 *action,
  uint32_t id, uint64_t address, const Bit8u *bytes, uint32_t count)
{
  bx_ntvdm_mechanical_action_v1_clear(action);
  action->action_id = id;
  action->kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
  action->range_count = 1u;
  action->payload_bytes = count;
  action->ranges[0].physical_address = address;
  action->ranges[0].byte_count = count;
  memcpy(action->payload, bytes, count);
}

static int prepare(struct bx_ntvdm_machine_stage_v1_request *request,
  const Bit8u *entry_bytes, uint32_t entry_byte_count)
{
  static const Bit8u preserved[] = { 0x10u, 0x20u, 0x30u, 0x40u };
  struct bx_ntvdm_machine_stage_v1_entry entry;
  bx_ntvdm_machine_stage_v1_request_clear(request);
  write_action(&request->initial_state_action, 1u, 0x714u, preserved,
    sizeof(preserved));
  write_action(&request->startup_action, 2u, 0x700u, entry_bytes,
    entry_byte_count);
  request->preserved_state_address = 0x714u;
  request->preserved_state_bytes = sizeof(preserved);
  if (!bx_ntvdm_machine_stage_v1_request_valid(request) ||
      bx_ntvdm_machine_stage_v1_begin(request) != BX_NTVDM_MACHINE_STAGE_V1_OK)
    return 0;
  bx_ntvdm_machine_stage_v1_entry_clear(&entry);
  entry.cs = 0x70u;
  return bx_ntvdm_machine_stage_v1_arm_real_mode_entry(&entry) ==
    BX_NTVDM_MACHINE_STAGE_V1_OK;
}

int main()
{
  static const Bit8u loop[] = { 0xebu, 0xfeu };
  static const Bit8u ud2[] = { 0x0fu, 0x0bu };
  struct bx_ntvdm_machine_stage_v1_request request;
  struct bx_ntvdm_machine_stage_v1_execution_request execution;
  struct bx_ntvdm_machine_stage_v1_terminal_position position;
  struct bx_ntvdm_machine_stage_v1_terminal_history history;
  struct bx_ntvdm_machine_stage_v1_terminal_cs_transitions transitions;
  bx_ntvdm_machine_stage_v1_execution_request_clear(&execution);
  execution.ips = 1000000u;
  execution.instruction_tick_budget = 16u;
  bx_ntvdm_machine_stage_v1_terminal_position_observation_enable(0u);
  bx_ntvdm_machine_stage_v1_terminal_history_observation_enable(0u);
  bx_ntvdm_machine_stage_v1_terminal_cs_transitions_observation_enable(0u);
  if (!prepare(&request, loop, sizeof(loop))) return 1;
  if (bx_ntvdm_machine_stage_v1_execute(&execution) !=
      BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_BUDGET) return 2;
  if (bx_ntvdm_machine_stage_v1_terminal_position_observation_copy(&position)) return 3;
  if (bx_ntvdm_machine_stage_v1_terminal_history_observation_copy(&history)) return 4;
  if (bx_ntvdm_machine_stage_v1_terminal_cs_transitions_observation_copy(&transitions)) return 5;
  if (bx_ntvdm_machine_stage_v1_reset() != BX_NTVDM_MACHINE_STAGE_V1_OK) return 6;

  bx_ntvdm_machine_stage_v1_terminal_position_observation_enable(1u);
  bx_ntvdm_machine_stage_v1_terminal_history_observation_enable(1u);
  bx_ntvdm_machine_stage_v1_terminal_cs_transitions_observation_enable(1u);
  if (!prepare(&request, loop, sizeof(loop)) ||
      bx_ntvdm_machine_stage_v1_execute(&execution) !=
        BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_BUDGET ||
      !bx_ntvdm_machine_stage_v1_terminal_position_observation_copy(&position) ||
      position.magic != BX_NTVDM_MACHINE_STAGE_V1_TERMINAL_POSITION_MAGIC ||
      position.abi_version != BX_NTVDM_MACHINE_STAGE_V1_TERMINAL_POSITION_VERSION ||
      position.struct_bytes != sizeof(position) || position.valid != 1u ||
      position.cs != 0x70u || position.eip > 1u ||
      bx_ntvdm_machine_stage_v1_reset() != BX_NTVDM_MACHINE_STAGE_V1_OK) return 2;

#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY
  if (!bx_ntvdm_machine_stage_v1_terminal_history_observation_copy(&history) ||
      history.magic != BX_NTVDM_MACHINE_STAGE_V1_TERMINAL_HISTORY_MAGIC ||
      history.abi_version != BX_NTVDM_MACHINE_STAGE_V1_TERMINAL_HISTORY_VERSION ||
      history.struct_bytes != sizeof(history) || history.valid != 1u ||
      history.count == 0u ||
      history.count > BX_NTVDM_INSTRUCTION_HISTORY_V1_CAPACITY_MAX ||
      !bx_ntvdm_machine_stage_v1_terminal_cs_transitions_observation_copy(&transitions) ||
      transitions.magic != BX_NTVDM_MACHINE_STAGE_V1_TERMINAL_CS_TRANSITIONS_MAGIC ||
      transitions.abi_version != BX_NTVDM_MACHINE_STAGE_V1_TERMINAL_CS_TRANSITIONS_VERSION ||
      transitions.struct_bytes != sizeof(transitions) || transitions.valid != 1u ||
      transitions.value.count > BX_NTVDM_INSTRUCTION_HISTORY_V1_CS_TRANSITION_CAPACITY_MAX) return 5;
#else
  if (bx_ntvdm_machine_stage_v1_terminal_history_observation_copy(&history) ||
      bx_ntvdm_machine_stage_v1_terminal_cs_transitions_observation_copy(&transitions)) return 5;
#endif

  stop_on_ud = 1;
  if (!prepare(&request, ud2, sizeof(ud2)) ||
      bx_ntvdm_machine_stage_v1_execute(&execution) !=
        BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_CONTROLLED_STOP ||
      bx_ntvdm_machine_stage_v1_terminal_position_observation_copy(&position) ||
      bx_ntvdm_machine_stage_v1_terminal_history_observation_copy(&history) ||
      bx_ntvdm_machine_stage_v1_terminal_cs_transitions_observation_copy(&transitions) ||
      bx_ntvdm_machine_stage_v1_reset() != BX_NTVDM_MACHINE_STAGE_V1_OK) return 3;
  stop_on_ud = 0;

  /* This timer is intentionally outside the stage stop state.  It exercises
   * the existing cpu_loop unexpected-return classification and proves that
   * terminal-position observation copies only the final mechanical fact. */
  {
    int untracked_stop_timer;
    untracked_stop_fired = 0;
    if (!prepare(&request, loop, sizeof(loop)) ||
        (untracked_stop_timer = bx_pc_system.register_timer_ticks(
            &untracked_stop_fired, untracked_stop, 1u, 0, 1,
            "fixture-untracked-stop")) <= 0 ||
        bx_ntvdm_machine_stage_v1_execute(&execution) !=
          BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_UNEXPECTED_LOOP_RETURN ||
        !untracked_stop_fired ||
        !bx_ntvdm_machine_stage_v1_terminal_position_observation_copy(&position) ||
        position.valid != 1u || position.cs != 0x70u || position.eip > 1u) return 9;
    bx_pc_system.deactivate_timer((unsigned)untracked_stop_timer);
    bx_pc_system.unregisterTimer((unsigned)untracked_stop_timer);
    if (bx_ntvdm_machine_stage_v1_reset() != BX_NTVDM_MACHINE_STAGE_V1_OK) return 10;
  }

  bx_ntvdm_machine_stage_v1_terminal_position_observation_enable(0u);
  bx_ntvdm_machine_stage_v1_terminal_history_observation_enable(0u);
  bx_ntvdm_machine_stage_v1_terminal_cs_transitions_observation_enable(0u);
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY && !BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
  {
    struct bx_ntvdm_instruction_history_record_v1 record;
    struct bx_ntvdm_instruction_history_transition_v1 transition;
    uint32_t index;
    memset(&record, 0, sizeof(record));
    record.version = BX_NTVDM_INSTRUCTION_HISTORY_V1_VERSION;
    if (!bx_ntvdm_mantle_instruction_history_v1_configure(
        BX_NTVDM_INSTRUCTION_HISTORY_V1_CAPACITY_MAX)) return 6;
    for (index = 0u; index < BX_NTVDM_INSTRUCTION_HISTORY_V1_CS_TRANSITION_CAPACITY_MAX + 4u; ++index) {
      record.sequence = index;
      record.rip = index;
      record.cs = (uint16_t)(0x100u + index);
      bx_ntvdm_mantle_instruction_history_v1_record(&record);
    }
    if (bx_ntvdm_mantle_instruction_history_v1_cs_transition_count() !=
        BX_NTVDM_INSTRUCTION_HISTORY_V1_CS_TRANSITION_CAPACITY_MAX ||
        !bx_ntvdm_mantle_instruction_history_v1_get_cs_transition(0u, &transition) ||
        transition.previous.sequence != 3u || transition.current.sequence != 4u ||
        !bx_ntvdm_mantle_instruction_history_v1_get_cs_transition(
          BX_NTVDM_INSTRUCTION_HISTORY_V1_CS_TRANSITION_CAPACITY_MAX - 1u, &transition) ||
        transition.previous.sequence != 18u || transition.current.sequence != 19u) return 7;
    bx_ntvdm_mantle_instruction_history_v1_reset();
    if (bx_ntvdm_mantle_instruction_history_v1_cs_transition_count() != 0u ||
        bx_ntvdm_mantle_instruction_history_v1_get_cs_transition(0u, &transition)) return 8;
  }
#endif
  return bx_ntvdm_machine_stage_v1_terminal_position_observation_copy(&position) ||
    bx_ntvdm_machine_stage_v1_terminal_history_observation_copy(&history) ||
    bx_ntvdm_machine_stage_v1_terminal_cs_transitions_observation_copy(&transitions) ? 4 : 0;
}
