/////////////////////////////////////////////////////////////////////////
//
// Default-off, selector-blind fixed-range physical-write observation.
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_NTVDM_PHYSICAL_WRITE_OBSERVATION_V1_H
#define BX_NTVDM_PHYSICAL_WRITE_OBSERVATION_V1_H

#include <stdint.h>

#define BX_NTVDM_PHYSICAL_WRITE_OBSERVATION_V1_MAGIC 0x42505731u
#define BX_NTVDM_PHYSICAL_WRITE_OBSERVATION_V1_VERSION 1u
#define BX_NTVDM_PHYSICAL_WRITE_OBSERVATION_V1_BYTES 8u

struct bx_ntvdm_physical_write_observation_v1 {
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
  uint8_t bytes[BX_NTVDM_PHYSICAL_WRITE_OBSERVATION_V1_BYTES];
};

#ifdef __cplusplus
extern "C" {
#endif

int bx_ntvdm_physical_write_observation_v1_configure(uint64_t physical_address,
  uint64_t byte_count);
void bx_ntvdm_physical_write_observation_v1_reset(void);
int bx_ntvdm_physical_write_observation_v1_copy(
  struct bx_ntvdm_physical_write_observation_v1 *value);
void bx_ntvdm_physical_write_observation_v1_record(uint64_t physical_address,
  uint32_t byte_count, const void *bytes, uint64_t sequence, uint64_t rip,
  uint64_t cs_base, uint16_t cs, uint16_t ss, uint16_t sp);

#ifdef __cplusplus
}
#endif

#endif
