/////////////////////////////////////////////////////////////////////////
//
// Fixed synchronous ordinary-RAM action owned by the native Bochs mantle.
// It carries only physical ranges and copied bytes: no higher-layer meaning
// is represented here.
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_NTVDM_MECHANICAL_ACTION_V1_H
#define BX_NTVDM_MECHANICAL_ACTION_V1_H

#include <stdint.h>

#define BX_NTVDM_MECHANICAL_ACTION_V1_MAGIC 0x42584d41u
#define BX_NTVDM_MECHANICAL_ACTION_V1_VERSION 1u
#define BX_NTVDM_MECHANICAL_ACTION_V1_MAX_RANGES 64u
#define BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES 65535u

enum bx_ntvdm_mechanical_action_v1_kind {
  BX_NTVDM_MECHANICAL_ACTION_V1_NONE = 0u,
  BX_NTVDM_MECHANICAL_ACTION_V1_READ = 1u,
  BX_NTVDM_MECHANICAL_ACTION_V1_WRITE = 2u
};

struct bx_ntvdm_mechanical_action_v1_range {
  uint64_t physical_address;
  uint32_t byte_count;
  uint32_t payload_offset;
};

struct bx_ntvdm_mechanical_action_v1 {
  uint32_t magic, abi_version, struct_bytes, kind;
  uint32_t action_id, range_count;
  uint32_t payload_bytes, reserved0;
  struct bx_ntvdm_mechanical_action_v1_range
    ranges[BX_NTVDM_MECHANICAL_ACTION_V1_MAX_RANGES];
  uint8_t payload[BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES];
};

#ifdef __cplusplus
extern "C" {
#endif

/* Records are borrowed only for this call.  A successful read copies bytes
 * into payload; a successful write consumes its copied payload. */
void bx_ntvdm_mechanical_action_v1_clear(
  struct bx_ntvdm_mechanical_action_v1 *action);
int bx_ntvdm_mechanical_action_v1_valid(
  const struct bx_ntvdm_mechanical_action_v1 *action);
int bx_ntvdm_mantle_execute_mechanical_action_v1(
  struct bx_ntvdm_mechanical_action_v1 *action);

#ifdef __cplusplus
}
#endif

#endif
