/////////////////////////////////////////////////////////////////////////
//
// Default-off, selector-blind fixed-range physical-write observation.
//
/////////////////////////////////////////////////////////////////////////

#ifndef RUNTIME_PHYSICAL_WRITE_OBSERVATION_H
#define RUNTIME_PHYSICAL_WRITE_OBSERVATION_H

#include <stdint.h>

#define RUNTIME_PHYSICAL_WRITE_OBSERVATION_MAGIC 0x42505731u
#define RUNTIME_PHYSICAL_WRITE_OBSERVATION_VERSION 1u
#define RUNTIME_PHYSICAL_WRITE_OBSERVATION_BYTES 8u

struct runtime_physical_write_observation {
  uint32_t magic;
  uint32_t abi_version;
  uint32_t struct_bytes;
  uint32_t valid;
  uint64_t physical_address;
  uint32_t byte_count;
  uint32_t captured_bytes;
  uint64_t sequence;
  uint64_t rip;
  uint64_t cs_base;
  uint16_t cs;
  uint16_t ss;
  uint16_t sp;
  uint16_t reserved0;
  uint8_t bytes[RUNTIME_PHYSICAL_WRITE_OBSERVATION_BYTES];
};

#ifdef __cplusplus
extern "C" {
#endif

int runtime_physical_write_observation_configure(uint64_t physical_address,
  uint64_t byte_count);
void runtime_physical_write_observation_reset(void);
int runtime_physical_write_observation_copy(
  struct runtime_physical_write_observation *value);
void runtime_physical_write_observation_record(uint64_t physical_address,
  uint32_t byte_count, const void *bytes, uint64_t sequence, uint64_t rip,
  uint64_t cs_base, uint16_t cs, uint16_t ss, uint16_t sp);

#ifdef __cplusplus
}
#endif

#endif
