/////////////////////////////////////////////////////////////////////////
//
// Default-off, fixed-width native instruction-position history mechanic.
// Its data has no instruction-byte, guest-memory, or service interpretation.
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_NTVDM_INSTRUCTION_HISTORY_H
#define BX_NTVDM_INSTRUCTION_HISTORY_H

#include <stdint.h>

#define BX_NTVDM_INSTRUCTION_HISTORY_V1_VERSION 1u
#define BX_NTVDM_INSTRUCTION_HISTORY_V1_CAPACITY_MAX 16u

struct bx_ntvdm_instruction_history_record_v1 {
  uint32_t version;
  uint32_t cpu_id;
  uint64_t sequence;
  uint64_t rip;
  uint16_t cs;
  uint16_t reserved0;
};

struct bx_ntvdm_instruction_history_transition_v1 {
  struct bx_ntvdm_instruction_history_record_v1 previous;
  struct bx_ntvdm_instruction_history_record_v1 current;
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

#ifdef __cplusplus
}
#endif

#endif
