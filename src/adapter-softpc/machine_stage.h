/////////////////////////////////////////////////////////////////////////
//
// Process-local minimal-machine preentry stage.  This C ABI transports only
// copied mechanical actions; Bochs object lifetime remains private to mantle.
//
/////////////////////////////////////////////////////////////////////////

#ifndef RUNTIME_MACHINE_STAGE_V1_H
#define RUNTIME_MACHINE_STAGE_V1_H

#include <stdint.h>

#include "mechanical_action.h"
#include "instruction_history.h"

#define RUNTIME_MACHINE_STAGE_V1_MAGIC UINT32_C(0x42584d53)
#define RUNTIME_MACHINE_STAGE_V1_VERSION UINT32_C(1)
#define RUNTIME_MACHINE_STAGE_V1_GUEST_MEMORY_MIN_BYTES UINT64_C(0x100000)
#define RUNTIME_MACHINE_STAGE_V1_GUEST_MEMORY_MAX_BYTES UINT64_C(0x1000000)
#define RUNTIME_MACHINE_STAGE_V1_GUEST_MEMORY_GRANULARITY UINT64_C(0x10000)

enum runtime_machine_stage_v1_status {
  RUNTIME_MACHINE_STAGE_V1_OK = 0u,
  RUNTIME_MACHINE_STAGE_V1_REJECTED_INPUT,
  RUNTIME_MACHINE_STAGE_V1_REJECTED_ACTIVE,
  RUNTIME_MACHINE_STAGE_V1_MACHINE_FAILURE,
  RUNTIME_MACHINE_STAGE_V1_ACTION_FAILURE,
  RUNTIME_MACHINE_STAGE_V1_PRESERVE_FAILURE,
  RUNTIME_MACHINE_STAGE_V1_CLEANUP_FAILURE,
  RUNTIME_MACHINE_STAGE_V1_REJECTED_INACTIVE,
  RUNTIME_MACHINE_STAGE_V1_REJECTED_ENTRY
};

#define RUNTIME_MACHINE_STAGE_V1_ENTRY_MAGIC UINT32_C(0x42584d45)
#define RUNTIME_MACHINE_STAGE_V1_EXECUTION_MAGIC UINT32_C(0x42584d58)

#define RUNTIME_MACHINE_STAGE_V1_TERMINAL_POSITION_MAGIC UINT32_C(0x42584d54)
#define RUNTIME_MACHINE_STAGE_V1_TERMINAL_POSITION_VERSION UINT32_C(1)

/* Default-off, copied finite-terminal position. It exposes no Bochs object,
 * guest byte, selector/service identity, or register state beyond CS:IP. */
struct runtime_machine_stage_v1_terminal_position {
  uint32_t magic;
  uint32_t abi_version;
  uint32_t struct_bytes;
  uint32_t valid;
  uint16_t cs;
  uint16_t reserved0;
  uint32_t eip;
};

#define RUNTIME_MACHINE_STAGE_V1_TERMINAL_HISTORY_MAGIC UINT32_C(0x42584d48)
#define RUNTIME_MACHINE_STAGE_V1_TERMINAL_HISTORY_VERSION UINT32_C(1)

/* Default-off scalar history copied only after an existing watchdog budget
 * return. It carries no bytes, decoded instruction, BOP identity, or object. */
struct runtime_machine_stage_v1_terminal_history {
  uint32_t magic;
  uint32_t abi_version;
  uint32_t struct_bytes;
  uint32_t valid;
  uint32_t count;
  uint32_t reserved0;
  struct runtime_instruction_history_record_v1
    records[RUNTIME_INSTRUCTION_HISTORY_V1_CAPACITY_MAX];
};

#define RUNTIME_MACHINE_STAGE_V1_TERMINAL_CS_TRANSITIONS_MAGIC UINT32_C(0x42584354)
#define RUNTIME_MACHINE_STAGE_V1_TERMINAL_CS_TRANSITIONS_VERSION UINT32_C(1)

/* Default-off chronological scalar CS transition ring. It is copied only
 * after an existing watchdog budget return and carries no guest bytes,
 * decoded instruction, service identity, or machine object. */
struct runtime_machine_stage_v1_terminal_cs_transitions {
  uint32_t magic;
  uint32_t abi_version;
  uint32_t struct_bytes;
  uint32_t valid;
  struct runtime_instruction_history_transition_history_v1 value;
};

#define RUNTIME_MACHINE_STAGE_V1_TERMINAL_PROVENANCE_MAGIC UINT32_C(0x42585056)
#define RUNTIME_MACHINE_STAGE_V1_TERMINAL_PROVENANCE_VERSION UINT32_C(1)

/* Explicit-provenance diagnostic only. It copies fixed ordinary-RAM windows
 * after an already-returned watchdog budget outcome; it neither decodes the
 * bytes nor affects CPU, service, or stop semantics. */
struct runtime_machine_stage_v1_terminal_provenance {
  uint32_t magic;
  uint32_t abi_version;
  uint32_t struct_bytes;
  uint32_t valid;
  uint16_t cs;
  uint16_t ss;
  uint16_t sp;
  uint16_t reserved0;
  uint32_t eip;
  uint8_t instruction_bytes[RUNTIME_INSTRUCTION_HISTORY_V1_PREDECESSOR_BYTES];
  uint8_t stack_bytes[RUNTIME_INSTRUCTION_HISTORY_V1_STACK_BYTES];
  uint8_t instruction_valid;
  uint8_t stack_valid;
  uint16_t reserved1;
};
#define RUNTIME_MACHINE_STAGE_V1_TERMINAL_CS_PROVENANCE_MAGIC UINT32_C(0x42584350)
#define RUNTIME_MACHINE_STAGE_V1_TERMINAL_CS_PROVENANCE_VERSION UINT32_C(1)

/* An already captured BX-MANTLE-075 record, copied only after the existing
 * watchdog return in an explicit provenance diagnostic graph. */
struct runtime_machine_stage_v1_terminal_cs_provenance {
  uint32_t magic;
  uint32_t abi_version;
  uint32_t struct_bytes;
  uint32_t valid;
  struct runtime_instruction_history_provenance_v1 value;
};

/* A real-mode control-transfer delta.  It deliberately excludes general
 * registers, flags, descriptor caches, and any machine object. */
struct runtime_machine_stage_v1_entry {
  uint32_t magic;
  uint32_t abi_version;
  uint32_t struct_bytes;
  uint32_t reserved0;
  uint16_t cs;
  uint16_t reserved1;
  uint32_t eip;
};

enum runtime_machine_stage_v1_execution_status {
  RUNTIME_MACHINE_STAGE_V1_EXECUTION_BUDGET = 0u,
  RUNTIME_MACHINE_STAGE_V1_EXECUTION_CONTROLLED_STOP,
  RUNTIME_MACHINE_STAGE_V1_EXECUTION_FIRST_FAULT_STOP,
  RUNTIME_MACHINE_STAGE_V1_EXECUTION_REJECTED_INACTIVE,
  RUNTIME_MACHINE_STAGE_V1_EXECUTION_REJECTED_INPUT,
  RUNTIME_MACHINE_STAGE_V1_EXECUTION_TIMER_FAILURE,
  RUNTIME_MACHINE_STAGE_V1_EXECUTION_UNEXPECTED_LOOP_RETURN,
  RUNTIME_MACHINE_STAGE_V1_EXECUTION_HOST_CANCELLATION
  ,RUNTIME_MACHINE_STAGE_V1_EXECUTION_PENDING
};

struct runtime_machine_stage_v1_execution_request {
  uint32_t magic;
  uint32_t abi_version;
  uint32_t struct_bytes;
  uint32_t ips;
  uint64_t instruction_tick_budget;
};

struct runtime_machine_stage_v1_request {
  uint32_t magic;
  uint32_t abi_version;
  uint32_t struct_bytes;
  uint32_t reserved0;
  uint64_t guest_memory_bytes;
  uint64_t reserved_memory_base;
  uint64_t reserved_memory_bytes;
  struct runtime_mechanical_action_v1 initial_state_action;
  struct runtime_mechanical_action_v1 startup_action;
  uint64_t preserved_state_address;
  uint64_t preserved_state_bytes;
  uint32_t ivt_watch_enabled;
  uint32_t ivt_watch_vector;
};

#ifdef __cplusplus
extern "C" {
#endif

void runtime_machine_stage_v1_request_clear(
  struct runtime_machine_stage_v1_request *request);
int runtime_machine_stage_v1_request_valid(
  const struct runtime_machine_stage_v1_request *request);
/* Begins one process-local machine stage and retains only the initialized
 * mantle machine.  Inputs are consumed during the call and never retained. */
uint32_t runtime_machine_stage_v1_begin(
  const struct runtime_machine_stage_v1_request *request);
/* Releases the retained machine; no Bochs object crosses this ABI. */
uint32_t runtime_machine_stage_v1_reset(void);
int runtime_machine_stage_v1_active(void);
void runtime_machine_stage_v1_entry_clear(
  struct runtime_machine_stage_v1_entry *entry);
int runtime_machine_stage_v1_entry_valid(
  const struct runtime_machine_stage_v1_entry *entry);
/* Applies only the checked real-mode CS:IP delta to an active stage. */
uint32_t runtime_machine_stage_v1_arm_real_mode_entry(
  const struct runtime_machine_stage_v1_entry *entry);
/* Copies the currently armed CS:IP without exposing the native CPU object. */
uint32_t runtime_machine_stage_v1_copy_real_mode_entry(
  struct runtime_machine_stage_v1_entry *entry);
void runtime_machine_stage_v1_execution_request_clear(
  struct runtime_machine_stage_v1_execution_request *request);
int runtime_machine_stage_v1_execution_request_valid(
  const struct runtime_machine_stage_v1_execution_request *request);
/* Executes an already armed active stage until a finite watchdog or accepted
 * generic STOP outcome returns control. It never owns a BOP/provider policy. */
uint32_t runtime_machine_stage_v1_execute(
  const struct runtime_machine_stage_v1_execution_request *request);

/* Enables one default-off copied CS:IP capture only for a watchdog budget
 * return. The observation never changes machine execution. */
void runtime_machine_stage_v1_terminal_position_observation_enable(
  uint32_t enabled);
int runtime_machine_stage_v1_terminal_position_observation_copy(
  struct runtime_machine_stage_v1_terminal_position *position);

/* Requires the separately compiled instruction-history diagnostic graph.
 * Default graphs retain no history and this copy returns unavailable. */
void runtime_machine_stage_v1_terminal_history_observation_enable(
  uint32_t enabled);
int runtime_machine_stage_v1_terminal_history_observation_copy(
  struct runtime_machine_stage_v1_terminal_history *history);

/* Requires the separately compiled scalar instruction-history diagnostic
 * graph. Default graphs retain no transition ring and this copy is unavailable. */
void runtime_machine_stage_v1_terminal_cs_transitions_observation_enable(
  uint32_t enabled);
int runtime_machine_stage_v1_terminal_cs_transitions_observation_copy(
  struct runtime_machine_stage_v1_terminal_cs_transitions *transitions);

/* Requires the separately compiled explicit-provenance diagnostic graph.
 * Default and scalar-history graphs retain no terminal RAM window. */
void runtime_machine_stage_v1_terminal_provenance_observation_enable(
  uint32_t enabled);
int runtime_machine_stage_v1_terminal_provenance_observation_copy(
  struct runtime_machine_stage_v1_terminal_provenance *provenance);
void runtime_machine_stage_v1_terminal_cs_provenance_observation_enable(
  uint32_t enabled);
int runtime_machine_stage_v1_terminal_cs_provenance_observation_copy(
  struct runtime_machine_stage_v1_terminal_cs_provenance *provenance);

#ifdef __cplusplus
}
#endif

#endif
