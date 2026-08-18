#ifndef BX_NTVDM_INTERRUPT_RETURN_OBSERVATION_V1_H
#define BX_NTVDM_INTERRUPT_RETURN_OBSERVATION_V1_H

#include <stdint.h>

#define BX_NTVDM_INTERRUPT_RETURN_OBSERVATION_V1_VERSION 1u
#define BX_NTVDM_INTERRUPT_RETURN_OBSERVATION_V1_CAPACITY_MAX 64u

struct bx_ntvdm_interrupt_return_observation_v1_record {
  uint32_t version;
  uint32_t cpu_id;
  uint64_t sequence;
  uint64_t rip;
  uint32_t eflags;
  uint32_t sp;
  uint16_t cs;
  uint16_t ss;
  uint16_t ax;
  uint16_t bx;
  uint16_t cx;
  uint16_t dx;
  uint16_t ds;
  uint16_t es;
  uint8_t execution_mode;
  uint8_t operand_width;
  uint16_t reserved0;
};

#ifdef __cplusplus
extern "C" {
#endif

int bx_ntvdm_mantle_interrupt_return_observation_v1_configure(uint32_t capacity);
void bx_ntvdm_mantle_interrupt_return_observation_v1_reset(void);
void bx_ntvdm_mantle_interrupt_return_observation_v1_record(
  const struct bx_ntvdm_interrupt_return_observation_v1_record *record);
uint32_t bx_ntvdm_mantle_interrupt_return_observation_v1_count(void);
int bx_ntvdm_mantle_interrupt_return_observation_v1_get(uint32_t index,
  struct bx_ntvdm_interrupt_return_observation_v1_record *record);

#ifdef __cplusplus
}
#endif

#endif