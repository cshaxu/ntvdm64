#ifndef BX_NTVDM_SEGMENT_ACCESS_OBSERVATION_V1_H
#define BX_NTVDM_SEGMENT_ACCESS_OBSERVATION_V1_H

#include <stdint.h>

#define BX_NTVDM_SEGMENT_ACCESS_OBSERVATION_V1_MAGIC UINT32_C(0x42585331)
#define BX_NTVDM_SEGMENT_ACCESS_OBSERVATION_V1_VERSION UINT32_C(1)
#define BX_NTVDM_SEGMENT_ACCESS_KIND_V1_READ_WORD UINT32_C(1)
#define BX_NTVDM_SEGMENT_ACCESS_BRANCH_V1_DIRECT_LIMIT UINT32_C(1)
#define BX_NTVDM_SEGMENT_ACCESS_BRANCH_V1_READ_CHECK UINT32_C(2)

struct bx_ntvdm_segment_access_observation_v1 {
  uint32_t magic, abi_version, struct_bytes, cpu_id;
  uint32_t access_kind, branch_kind, segment_index, width;
  uint32_t offset, limit_scaled, cache_valid, reserved0;
  uint16_t segment_selector, reserved1;
};

#ifdef __cplusplus
extern "C" {
#endif

int bx_ntvdm_mantle_segment_access_observation_v1(
  const struct bx_ntvdm_segment_access_observation_v1 *event);
void bx_ntvdm_mantle_segment_access_observation_reset(void);
void bx_ntvdm_mantle_segment_access_observation_enable(int enabled);
int bx_ntvdm_mantle_segment_access_observation_copy(
  struct bx_ntvdm_segment_access_observation_v1 *event);

#ifdef __cplusplus
}
#endif

#endif
