#ifndef RUNTIME_SEGMENT_ACCESS_OBSERVATION_V1_H
#define RUNTIME_SEGMENT_ACCESS_OBSERVATION_V1_H

#include <stdint.h>

#define RUNTIME_SEGMENT_ACCESS_OBSERVATION_V1_MAGIC UINT32_C(0x42585331)
#define RUNTIME_SEGMENT_ACCESS_OBSERVATION_V1_VERSION UINT32_C(1)
#define RUNTIME_SEGMENT_ACCESS_KIND_V1_READ_WORD UINT32_C(1)
#define RUNTIME_SEGMENT_ACCESS_BRANCH_V1_DIRECT_LIMIT UINT32_C(1)
#define RUNTIME_SEGMENT_ACCESS_BRANCH_V1_READ_CHECK UINT32_C(2)

struct runtime_segment_access_observation_v1 {
  uint32_t magic, abi_version, struct_bytes, cpu_id;
  uint32_t access_kind, branch_kind, segment_index, width;
  uint32_t offset, limit_scaled, cache_valid, reserved0;
  uint16_t segment_selector, reserved1;
};

#ifdef __cplusplus
extern "C" {
#endif

int runtime_mantle_segment_access_observation_v1(
  const struct runtime_segment_access_observation_v1 *event);
void runtime_mantle_segment_access_observation_reset(void);
void runtime_mantle_segment_access_observation_enable(int enabled);
int runtime_mantle_segment_access_observation_copy(
  struct runtime_segment_access_observation_v1 *event);

#ifdef __cplusplus
}
#endif

#endif
