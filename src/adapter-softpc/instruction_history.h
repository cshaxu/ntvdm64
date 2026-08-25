/////////////////////////////////////////////////////////////////////////
//
// Default-off, fixed-width native instruction-position history mechanic.
// Its data is copied mechanically and has no instruction, service, or profile
// interpretation.
//
/////////////////////////////////////////////////////////////////////////

#ifndef RUNTIME_INSTRUCTION_HISTORY_H
#define RUNTIME_INSTRUCTION_HISTORY_H

#include <stdint.h>

#define RUNTIME_INSTRUCTION_HISTORY_VERSION 1u
#define RUNTIME_INSTRUCTION_HISTORY_CAPACITY_MAX 16u
#define RUNTIME_INSTRUCTION_HISTORY_CS_TRANSITION_CAPACITY_MAX 16u
#define RUNTIME_INSTRUCTION_HISTORY_PREDECESSOR_BYTES 15u
#define RUNTIME_INSTRUCTION_HISTORY_SUCCESSOR_BYTES 15u
#define RUNTIME_INSTRUCTION_HISTORY_STACK_BYTES 10u

struct runtime_instruction_history_record {
  uint32_t version;
  uint32_t cpu_id;
  uint64_t sequence;
  uint64_t rip;
  uint16_t cs;
  uint16_t ss;
  uint16_t sp;
  uint16_t bp;
  uint16_t reserved0;
};

struct runtime_instruction_history_transition {
  struct runtime_instruction_history_record previous;
  struct runtime_instruction_history_record current;
};

struct runtime_instruction_history_transition_history {
  uint32_t count;
  uint32_t reserved0;
  struct runtime_instruction_history_transition
    transitions[RUNTIME_INSTRUCTION_HISTORY_CS_TRANSITION_CAPACITY_MAX];
};

struct runtime_instruction_history_provenance {
  struct runtime_instruction_history_transition transition;
  uint8_t predecessor_bytes[RUNTIME_INSTRUCTION_HISTORY_PREDECESSOR_BYTES];
  uint8_t successor_bytes[RUNTIME_INSTRUCTION_HISTORY_SUCCESSOR_BYTES];
  uint8_t stack_bytes[RUNTIME_INSTRUCTION_HISTORY_STACK_BYTES];
  uint8_t predecessor_valid;
  uint8_t successor_valid;
  uint8_t stack_valid;
  uint8_t reserved0;
};

#ifdef __cplusplus
extern "C" {
#endif

int runtime_machine_instruction_history_configure(uint32_t capacity);
void runtime_machine_instruction_history_reset(void);
void runtime_machine_instruction_history_record(
  const struct runtime_instruction_history_record *record);
uint32_t runtime_machine_instruction_history_count(void);
int runtime_machine_instruction_history_get(uint32_t index,
  struct runtime_instruction_history_record *record);
int runtime_machine_instruction_history_get_latest_cs_transition(
  struct runtime_instruction_history_transition *transition);
uint32_t runtime_machine_instruction_history_cs_transition_count(void);
int runtime_machine_instruction_history_get_cs_transition(uint32_t index,
  struct runtime_instruction_history_transition *transition);
int runtime_machine_instruction_history_get_latest_cs_provenance(
  struct runtime_instruction_history_provenance *provenance);

#ifdef __cplusplus
}
#endif

#endif
