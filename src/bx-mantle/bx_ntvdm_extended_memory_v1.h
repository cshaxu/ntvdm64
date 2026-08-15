#ifndef BX_NTVDM_EXTENDED_MEMORY_V1_H
#define BX_NTVDM_EXTENDED_MEMORY_V1_H

#include <stdint.h>

#define BX_NTVDM_EXTENDED_MEMORY_V1_VERSION 1u

enum bx_ntvdm_extended_memory_operation_v1 {
  BX_NTVDM_EXTMEM_QUERY = 1u,
  BX_NTVDM_EXTMEM_ALLOCATE = 2u,
  BX_NTVDM_EXTMEM_FREE = 3u,
  BX_NTVDM_EXTMEM_REALLOCATE = 4u,
  BX_NTVDM_EXTMEM_MOVE = 5u,
  BX_NTVDM_EXTMEM_QUERY_FREE = 6u
};

enum bx_ntvdm_extended_memory_status_v1 {
  BX_NTVDM_EXTMEM_OK = 0u,
  BX_NTVDM_EXTMEM_REJECTED_LIFECYCLE,
  BX_NTVDM_EXTMEM_REJECTED_VERSION,
  BX_NTVDM_EXTMEM_REJECTED_OPERATION,
  BX_NTVDM_EXTMEM_REJECTED_REQUEST,
  BX_NTVDM_EXTMEM_REJECTED_HANDLE,
  BX_NTVDM_EXTMEM_UNAVAILABLE,
  BX_NTVDM_EXTMEM_REJECTED_RANGE
};

struct bx_ntvdm_extended_memory_request_v1 {
  uint32_t version;
  uint32_t operation;
  uint32_t handle;
  uint32_t kib;
  uint32_t source_handle;
  uint32_t destination_handle;
  uint64_t source_offset;
  uint64_t destination_offset;
  uint64_t byte_count;
};

struct bx_ntvdm_extended_memory_result_v1 {
  uint32_t status;
  uint32_t handle;
  uint32_t kib;
  uint32_t free_kib;
  uint32_t largest_free_kib;
  uint64_t physical_address;
};

#ifdef __cplusplus
extern "C" {
#endif
/* Minimal-machine lifecycle owner only; fixed C linkage permits an external
 * client to call the same typed record without importing a C++ object. */
void bx_ntvdm_extended_memory_v1_set_lifecycle_active(uint32_t active);
void bx_ntvdm_extended_memory_v1_dispatch(
  const struct bx_ntvdm_extended_memory_request_v1 *request,
  struct bx_ntvdm_extended_memory_result_v1 *result);
#ifdef __cplusplus
}
#endif

#endif
