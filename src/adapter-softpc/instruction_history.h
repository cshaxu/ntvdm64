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

#define RUNTIME_INSTRUCTION_HISTORY_V1_VERSION 1u
#define RUNTIME_INSTRUCTION_HISTORY_V1_CAPACITY_MAX 16u
#define RUNTIME_INSTRUCTION_HISTORY_V1_CS_TRANSITION_CAPACITY_MAX 16u
#define RUNTIME_INSTRUCTION_HISTORY_V1_PREDECESSOR_BYTES 15u
#define RUNTIME_INSTRUCTION_HISTORY_V1_SUCCESSOR_BYTES 15u
#define RUNTIME_INSTRUCTION_HISTORY_V1_STACK_BYTES 10u

struct runtime_instruction_history_record_v1 {
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

struct runtime_instruction_history_transition_v1 {
  struct runtime_instruction_history_record_v1 previous;
  struct runtime_instruction_history_record_v1 current;
};

struct runtime_instruction_history_transition_history_v1 {
  uint32_t count;
  uint32_t reserved0;
  struct runtime_instruction_history_transition_v1
    transitions[RUNTIME_INSTRUCTION_HISTORY_V1_CS_TRANSITION_CAPACITY_MAX];
};

struct runtime_instruction_history_provenance_v1 {
  struct runtime_instruction_history_transition_v1 transition;
  uint8_t predecessor_bytes[RUNTIME_INSTRUCTION_HISTORY_V1_PREDECESSOR_BYTES];
  uint8_t successor_bytes[RUNTIME_INSTRUCTION_HISTORY_V1_SUCCESSOR_BYTES];
  uint8_t stack_bytes[RUNTIME_INSTRUCTION_HISTORY_V1_STACK_BYTES];
  uint8_t predecessor_valid;
  uint8_t successor_valid;
  uint8_t stack_valid;
  uint8_t reserved0;
};

#ifdef __cplusplus
extern "C" {
#endif

int runtime_mantle_instruction_history_v1_configure(uint32_t capacity);
void runtime_mantle_instruction_history_v1_reset(void);
void runtime_mantle_instruction_history_v1_record(
  const struct runtime_instruction_history_record_v1 *record);
uint32_t runtime_mantle_instruction_history_v1_count(void);
int runtime_mantle_instruction_history_v1_get(uint32_t index,
  struct runtime_instruction_history_record_v1 *record);
int runtime_mantle_instruction_history_v1_get_latest_cs_transition(
  struct runtime_instruction_history_transition_v1 *transition);
uint32_t runtime_mantle_instruction_history_v1_cs_transition_count(void);
int runtime_mantle_instruction_history_v1_get_cs_transition(uint32_t index,
  struct runtime_instruction_history_transition_v1 *transition);
int runtime_mantle_instruction_history_v1_get_latest_cs_provenance(
  struct runtime_instruction_history_provenance_v1 *provenance);

#ifdef __cplusplus
}
#endif

#endif
