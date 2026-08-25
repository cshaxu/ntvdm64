#ifndef RUNTIME_INTERRUPT_RETURN_OBSERVATION_H
#define RUNTIME_INTERRUPT_RETURN_OBSERVATION_H

#include <stdint.h>

#define RUNTIME_INTERRUPT_RETURN_OBSERVATION_VERSION 1u
#define RUNTIME_INTERRUPT_RETURN_OBSERVATION_CAPACITY_MAX 64u

struct runtime_interrupt_return_observation_record {
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

int runtime_machine_interrupt_return_observation_configure(uint32_t capacity);
void runtime_machine_interrupt_return_observation_reset(void);
void runtime_machine_interrupt_return_observation_record(
  const struct runtime_interrupt_return_observation_record *record);
uint32_t runtime_machine_interrupt_return_observation_count(void);
int runtime_machine_interrupt_return_observation_get(uint32_t index,
  struct runtime_interrupt_return_observation_record *record);

#ifdef __cplusplus
}
#endif

#endif