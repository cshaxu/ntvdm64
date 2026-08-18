/////////////////////////////////////////////////////////////////////////
//
// Default-off, fixed-width native instruction-position history mechanic.
// Its data is copied mechanically and has no instruction, service, or profile
// interpretation.
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_NTVDM_INSTRUCTION_HISTORY_H
#define BX_NTVDM_INSTRUCTION_HISTORY_H

#include <stdint.h>

#define BX_NTVDM_INSTRUCTION_HISTORY_V1_VERSION 1u
#define BX_NTVDM_INSTRUCTION_HISTORY_V1_CAPACITY_MAX 16u
#define BX_NTVDM_INSTRUCTION_HISTORY_V1_PREDECESSOR_BYTES 15u
#define BX_NTVDM_INSTRUCTION_HISTORY_V1_SUCCESSOR_BYTES 15u
#define BX_NTVDM_INSTRUCTION_HISTORY_V1_STACK_BYTES 10u

struct bx_ntvdm_instruction_history_record_v1 {
  uint32_t version;
  uint32_t cpu_id;
  uint64_t sequence;
  uint64_t rip;
  uint16_t cs;
  uint16_t ss;
  uint16_t sp;
  uint16_t reserved0;
};

struct bx_ntvdm_instruction_history_transition_v1 {
  struct bx_ntvdm_instruction_history_record_v1 previous;
  struct bx_ntvdm_instruction_history_record_v1 current;
};

struct bx_ntvdm_instruction_history_provenance_v1 {
  struct bx_ntvdm_instruction_history_transition_v1 transition;
  uint8_t predecessor_bytes[BX_NTVDM_INSTRUCTION_HISTORY_V1_PREDECESSOR_BYTES];
  uint8_t successor_bytes[BX_NTVDM_INSTRUCTION_HISTORY_V1_SUCCESSOR_BYTES];
  uint8_t stack_bytes[BX_NTVDM_INSTRUCTION_HISTORY_V1_STACK_BYTES];
  uint8_t predecessor_valid;
  uint8_t successor_valid;
  uint8_t stack_valid;
  uint8_t reserved0;
};

#ifdef __cplusplus
extern "C" {
#endif

int bx_ntvdm_mantle_instruction_history_v1_configure(uint32_t capacity);
void bx_ntvdm_mantle_instruction_history_v1_reset(void);
void bx_ntvdm_mantle_instruction_history_v1_record(
  const struct bx_ntvdm_instruction_history_record_v1 *record);
uint32_t bx_ntvdm_mantle_instruction_history_v1_count(void);
int bx_ntvdm_mantle_instruction_history_v1_get(uint32_t index,
  struct bx_ntvdm_instruction_history_record_v1 *record);
int bx_ntvdm_mantle_instruction_history_v1_get_latest_cs_transition(
  struct bx_ntvdm_instruction_history_transition_v1 *transition);
int bx_ntvdm_mantle_instruction_history_v1_get_latest_cs_provenance(
  struct bx_ntvdm_instruction_history_provenance_v1 *provenance);

#ifdef __cplusplus
}
#endif

#endif
