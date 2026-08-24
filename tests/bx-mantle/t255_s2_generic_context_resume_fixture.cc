#include "bochs.h"
#include "bx-core/cpu/cpu.h"
#include "adapter-softpc/a20_capability.h"
#include "adapter-bop/generic_ud_bridge.h"
#include "adapter-softpc/machine_stage.h"
#include "adapter-softpc/protected_range_action.h"
#include "support/t255_s2_generic_ud_context_fixture_bridge.h"

#include <string.h>

static unsigned v1_calls;
static uint32_t v1_execution_mode;
static uint32_t v1_eax;

extern "C" int runtime_mantle_generic_ud_bridge_v1(
  const struct runtime_generic_ud_event_v1 *event,
  struct runtime_generic_ud_outcome_v1 *outcome)
{
  if (event == 0 || outcome == 0 || event->vector != 6u) return 0;
  ++v1_calls;
  v1_execution_mode = event->execution_mode;
  v1_eax = event->eax;
  outcome->abi_version = RUNTIME_GENERIC_UD_EVENT_V1_VERSION;
  outcome->disposition = RUNTIME_GENERIC_UD_STOP;
  return 1;
}

static int begin_stage(const uint8_t *code, uint32_t code_bytes)
{
  static const uint8_t gdt_image[0x40] = {
    0x17, 0x00, 0x20, 0x08, 0x00, 0x00, /* GDTR at 0800h */
    0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,             /* null descriptor */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0xff, 0xff, 0x00, 0x00, 0x00, 0x9a, 0xcf, 0x00,
    0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xcf, 0x00
  };
  struct runtime_machine_stage_v1_request request;
  struct runtime_machine_stage_v1_entry entry;
  runtime_machine_stage_v1_request_clear(&request);
  runtime_mechanical_action_v1_clear(&request.initial_state_action);
  request.initial_state_action.action_id = 1u;
  request.initial_state_action.kind = RUNTIME_MECHANICAL_ACTION_V1_WRITE;
  request.initial_state_action.range_count = 1u;
  request.initial_state_action.payload_bytes = sizeof(gdt_image);
  request.initial_state_action.ranges[0].physical_address = 0x800u;
  request.initial_state_action.ranges[0].byte_count = sizeof(gdt_image);
  memcpy(request.initial_state_action.payload, gdt_image, sizeof(gdt_image));
  runtime_mechanical_action_v1_clear(&request.startup_action);
  request.startup_action.action_id = 2u;
  request.startup_action.kind = RUNTIME_MECHANICAL_ACTION_V1_WRITE;
  request.startup_action.range_count = 1u;
  request.startup_action.payload_bytes = code_bytes;
  request.startup_action.ranges[0].physical_address = 0x700u;
  request.startup_action.ranges[0].byte_count = code_bytes;
  memcpy(request.startup_action.payload, code, code_bytes);
  request.preserved_state_address = 0x600u;
  request.preserved_state_bytes = 4u;
  if (!runtime_machine_stage_v1_request_valid(&request) ||
      runtime_machine_stage_v1_begin(&request) != RUNTIME_MACHINE_STAGE_V1_OK)
    return 0;
  runtime_machine_stage_v1_entry_clear(&entry);
  entry.cs = 0x70u;
  entry.eip = 0u;
  return runtime_machine_stage_v1_arm_real_mode_entry(&entry) ==
    RUNTIME_MACHINE_STAGE_V1_OK;
}

static int execute_to_stop(void)
{
  struct runtime_machine_stage_v1_execution_request request;
  runtime_machine_stage_v1_execution_request_clear(&request);
  request.ips = 1000000u;
  request.instruction_tick_budget = 1000000u;
  return runtime_machine_stage_v1_execute(&request) ==
    RUNTIME_MACHINE_STAGE_V1_EXECUTION_CONTROLLED_STOP;
}

static int configure_context(uint32_t expected_mode, uint32_t target_mode,
  uint64_t resume_rip, uint32_t eax)
{
  struct t255_s2_generic_ud_context_fixture fixture;
  t255_s2_generic_ud_context_fixture_clear(&fixture);
  fixture.expected_execution_mode = expected_mode;
  fixture.outcome.abi_version = RUNTIME_GENERIC_UD_OUTCOME_V2_VERSION;
  fixture.outcome.disposition = RUNTIME_GENERIC_UD_RESUME;
  fixture.outcome.resume_rip = resume_rip;
  fixture.outcome.gpr32_write_mask = 1u;
  fixture.outcome.gpr32_values[0] = eax;
  fixture.outcome.segment_write_mask = 0x3fu;
  fixture.outcome.segment_values[0] = target_mode ==
    RUNTIME_GENERIC_UD_CONTEXT_PROTECTED ? 0x10u : 0u;
  fixture.outcome.segment_values[1] = target_mode ==
    RUNTIME_GENERIC_UD_CONTEXT_PROTECTED ? 0x08u : 0u;
  fixture.outcome.segment_values[2] = target_mode ==
    RUNTIME_GENERIC_UD_CONTEXT_PROTECTED ? 0x10u : 0u;
  fixture.outcome.segment_values[3] = fixture.outcome.segment_values[0];
  fixture.outcome.segment_values[4] = fixture.outcome.segment_values[0];
  fixture.outcome.segment_values[5] = fixture.outcome.segment_values[0];
  fixture.outcome.eflags_write_mask = 1u;
  fixture.outcome.eflags_values = 1u;
  fixture.outcome.context_mode = target_mode;
  return t255_s2_generic_ud_context_fixture_configure(&fixture);
}

static int set_a20(uint32_t enabled)
{
  struct runtime_a20_capability_request_v1 request;
  struct runtime_a20_capability_result_v1 result;
  request.version = RUNTIME_A20_CAPABILITY_V1_VERSION;
  request.operation = RUNTIME_A20_CAPABILITY_SET;
  request.requested_enabled = enabled;
  runtime_a20_capability_v1_dispatch(&request, &result);
  return result.status == RUNTIME_A20_CAPABILITY_OK && result.enabled == enabled;
}

static int test_real_to_protected(void)
{
  static const uint8_t code[] = { 0x0f, 0x01, 0x16, 0x00, 0x08, 0x0f, 0x0b };
  static const uint8_t target[] = { 0x0f, 0x0b };
  static const uint8_t source[] = { 0x12, 0x34, 0x56, 0x78 };
  static const uint8_t replacement[] = { 0x87, 0x65, 0x43, 0x21 };
  uint8_t observed[sizeof(replacement)];
  uint8_t edge_before[2] = { 0x31, 0x42 };
  uint8_t edge_after[2];
  struct runtime_protected_range_action_v1 action;
  if (!begin_stage(code, sizeof(code)) ||
      !runtime_mantle_checked_ram_write_v1(0x900u, target, sizeof(target)) ||
      !runtime_mantle_checked_ram_write_v1(0xa00u, source, sizeof(source)) ||
      !runtime_mantle_checked_ram_write_v1(0x9fffeu, edge_before,
        sizeof(edge_before)) ||
      !configure_context(1u, RUNTIME_GENERIC_UD_CONTEXT_PROTECTED,
        0x900u, 0x11223344u)) return 0;
  runtime_protected_range_action_v1_clear(&action);
  action.kind = RUNTIME_PROTECTED_RANGE_ACTION_V1_READ;
  action.segment = 3u; action.offset = 0xa00u; action.byte_count = sizeof(source);
  if (runtime_mantle_execute_protected_range_action_v1(&action) !=
      RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_MODE) return 0;
  v1_calls = 0u;
  if (!execute_to_stop() || v1_calls != 1u || v1_execution_mode != 2u ||
      v1_eax != 0x11223344u) return 0;
  runtime_protected_range_action_v1_clear(&action);
  action.kind = RUNTIME_PROTECTED_RANGE_ACTION_V1_READ;
  action.segment = 3u; action.offset = 0xa00u; action.byte_count = sizeof(source);
  if (runtime_mantle_execute_protected_range_action_v1(&action) !=
      RUNTIME_PROTECTED_RANGE_ACTION_V1_OK || memcmp(action.bytes, source, sizeof(source))) return 0;
  runtime_protected_range_action_v1_clear(&action);
  action.kind = RUNTIME_PROTECTED_RANGE_ACTION_V1_WRITE;
  action.segment = 3u; action.offset = 0xa10u; action.byte_count = sizeof(replacement);
  memcpy(action.bytes, replacement, sizeof(replacement));
  if (runtime_mantle_execute_protected_range_action_v1(&action) !=
      RUNTIME_PROTECTED_RANGE_ACTION_V1_OK ||
      !runtime_mantle_checked_ram_read_v1(0xa10u, observed, sizeof(observed)) ||
      memcmp(observed, replacement, sizeof(observed))) return 0;
  runtime_protected_range_action_v1_clear(&action);
  action.kind = RUNTIME_PROTECTED_RANGE_ACTION_V1_WRITE;
  action.segment = 1u; action.offset = 0xa10u; action.byte_count = 1u;
  if (runtime_mantle_execute_protected_range_action_v1(&action) !=
      RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_ACCESS) return 0;
  runtime_protected_range_action_v1_clear(&action);
  action.kind = RUNTIME_PROTECTED_RANGE_ACTION_V1_READ;
  action.segment = 6u; action.offset = 0xa00u; action.byte_count = 1u;
  if (runtime_mantle_execute_protected_range_action_v1(&action) !=
      RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_INPUT) return 0;
  runtime_protected_range_action_v1_clear(&action);
  action.kind = RUNTIME_PROTECTED_RANGE_ACTION_V1_READ;
  action.segment = 3u; action.offset = 0x100000u; action.byte_count = 1u;
  if (!set_a20(0u) || runtime_mantle_execute_protected_range_action_v1(&action) !=
      RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_MEMORY || !set_a20(1u))
    return 0;
  runtime_protected_range_action_v1_clear(&action);
  action.kind = RUNTIME_PROTECTED_RANGE_ACTION_V1_READ;
  action.segment = 3u; action.offset = 0xa0000u; action.byte_count = 1u;
  if (runtime_mantle_execute_protected_range_action_v1(&action) !=
      RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_MEMORY) return 0;
  runtime_protected_range_action_v1_clear(&action);
  action.kind = RUNTIME_PROTECTED_RANGE_ACTION_V1_WRITE;
  action.segment = 3u; action.offset = 0x9fffeu; action.byte_count = sizeof(replacement);
  memcpy(action.bytes, replacement, sizeof(replacement));
  if (runtime_mantle_execute_protected_range_action_v1(&action) !=
      RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_MEMORY ||
      !runtime_mantle_checked_ram_read_v1(0x9fffeu, edge_after,
        sizeof(edge_after)) || memcmp(edge_after, edge_before, sizeof(edge_before)))
    return 0;
  runtime_protected_range_action_v1_clear(&action);
  action.kind = RUNTIME_PROTECTED_RANGE_ACTION_V1_READ;
  action.segment = 3u; action.offset = 0xa00u; action.byte_count = 1u;
  bx_cpu.SetCR0(bx_cpu.read_CR0() | 0x80000000u);
  if (runtime_mantle_execute_protected_range_action_v1(&action) !=
      RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_MODE) return 0;
  bx_cpu.SetCR0(bx_cpu.read_CR0() & ~0x80000000u);
  return runtime_machine_stage_v1_reset() == RUNTIME_MACHINE_STAGE_V1_OK;
}

static int test_protected_to_real(void)
{
  static const uint8_t code[] = {
    0x0f, 0x01, 0x16, 0x00, 0x08, /* lgdt [0800h] */
    0x66, 0xb8, 0x01, 0x00, 0x00, 0x00,
    0x0f, 0x22, 0xc0,             /* mov cr0,eax */
    0xea, 0x00, 0x09, 0x08, 0x00  /* far jmp 0008:0900 */
  };
  static const uint8_t target[] = { 0x0f, 0x0b };
  if (!begin_stage(code, sizeof(code)) ||
      !runtime_mantle_checked_ram_write_v1(0x900u, target, sizeof(target)) ||
      !runtime_mantle_checked_ram_write_v1(0xa00u, target, sizeof(target)) ||
      !configure_context(2u, RUNTIME_GENERIC_UD_CONTEXT_REAL,
        0xa00u, 0x55667788u)) return 0;
  v1_calls = 0u;
  if (!execute_to_stop() || v1_calls != 1u || v1_execution_mode != 1u ||
      v1_eax != 0x55667788u) return 0;
  return runtime_machine_stage_v1_reset() == RUNTIME_MACHINE_STAGE_V1_OK;
}

static int test_invalid_context_rejected(void)
{
  struct t255_s2_generic_ud_context_fixture fixture;
  struct runtime_protected_range_action_v1 action;
  t255_s2_generic_ud_context_fixture_clear(&fixture);
  fixture.expected_execution_mode = 1u;
  fixture.outcome.abi_version = RUNTIME_GENERIC_UD_OUTCOME_V2_VERSION;
  fixture.outcome.disposition = RUNTIME_GENERIC_UD_RESUME;
  fixture.outcome.context_mode = 3u;
  if (t255_s2_generic_ud_context_fixture_configure(&fixture)) return 0;
  runtime_protected_range_action_v1_clear(&action);
  action.kind = RUNTIME_PROTECTED_RANGE_ACTION_V1_READ;
  action.segment = 3u; action.offset = 0u; action.byte_count = 1u;
  return runtime_mantle_execute_protected_range_action_v1(&action) ==
    RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_LIFECYCLE;
}

int main(void)
{
  if (!test_invalid_context_rejected()) return 1;
  if (!test_real_to_protected()) return 2;
  if (!test_protected_to_real()) return 3;
  return 0;
}
