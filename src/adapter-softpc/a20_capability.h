#ifndef RUNTIME_A20_CAPABILITY_V1_H
#define RUNTIME_A20_CAPABILITY_V1_H

#include <stdint.h>

enum runtime_a20_capability_operation_v1 {
  RUNTIME_A20_CAPABILITY_QUERY = 1u,
  RUNTIME_A20_CAPABILITY_SET = 2u
};

enum runtime_a20_capability_status_v1 {
  RUNTIME_A20_CAPABILITY_OK = 0u,
  RUNTIME_A20_CAPABILITY_REJECTED_LIFECYCLE,
  RUNTIME_A20_CAPABILITY_REJECTED_VERSION,
  RUNTIME_A20_CAPABILITY_REJECTED_OPERATION,
  RUNTIME_A20_CAPABILITY_REJECTED_VALUE
};

#define RUNTIME_A20_CAPABILITY_V1_VERSION 1u

struct runtime_a20_capability_request_v1 {
  uint32_t version;
  uint32_t operation;
  uint32_t requested_enabled;
};

struct runtime_a20_capability_result_v1 {
  uint32_t status;
  uint32_t enabled;
};

#ifdef __cplusplus
extern "C" {
#endif
/* Minimal-machine lifecycle owner only; fixed C linkage permits the adapter
 * to call the same typed record without importing a C++ object. */
void runtime_a20_capability_v1_set_lifecycle_active(uint32_t active);
void runtime_a20_capability_v1_dispatch(
  const struct runtime_a20_capability_request_v1 *request,
  struct runtime_a20_capability_result_v1 *result);
#ifdef __cplusplus
}
#endif

#endif
