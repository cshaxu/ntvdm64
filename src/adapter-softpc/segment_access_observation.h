#ifndef RUNTIME_SEGMENT_ACCESS_OBSERVATION_H
#define RUNTIME_SEGMENT_ACCESS_OBSERVATION_H

#include <stdint.h>

#define RUNTIME_SEGMENT_ACCESS_OBSERVATION_MAGIC UINT32_C(0x42585331)
#define RUNTIME_SEGMENT_ACCESS_OBSERVATION_VERSION UINT32_C(1)
#define RUNTIME_SEGMENT_ACCESS_KIND_READ_WORD UINT32_C(1)
#define RUNTIME_SEGMENT_ACCESS_BRANCH_DIRECT_LIMIT UINT32_C(1)
#define RUNTIME_SEGMENT_ACCESS_BRANCH_READ_CHECK UINT32_C(2)

struct runtime_segment_access_observation {
  uint32_t magic, abi_version, struct_bytes, cpu_id;
  uint32_t access_kind, branch_kind, segment_index, width;
  uint32_t offset, limit_scaled, cache_valid, reserved0;
  uint16_t segment_selector, reserved1;
};

#ifdef __cplusplus
extern "C" {
#endif

int runtime_machine_segment_access_observation(
  const struct runtime_segment_access_observation *event);
void runtime_machine_segment_access_observation_reset(void);
void runtime_machine_segment_access_observation_enable(int enabled);
int runtime_machine_segment_access_observation_copy(
  struct runtime_segment_access_observation *event);

#ifdef __cplusplus
}
#endif

#endif
