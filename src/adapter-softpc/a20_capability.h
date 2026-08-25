#ifndef RUNTIME_A20_CAPABILITY_H
#define RUNTIME_A20_CAPABILITY_H

#include <stdint.h>

enum runtime_a20_capability_operation {
  RUNTIME_A20_CAPABILITY_QUERY = 1u,
  RUNTIME_A20_CAPABILITY_SET = 2u
};

enum runtime_a20_capability_status {
  RUNTIME_A20_CAPABILITY_OK = 0u,
  RUNTIME_A20_CAPABILITY_REJECTED_LIFECYCLE,
  RUNTIME_A20_CAPABILITY_REJECTED_VERSION,
  RUNTIME_A20_CAPABILITY_REJECTED_OPERATION,
  RUNTIME_A20_CAPABILITY_REJECTED_VALUE
};

#define RUNTIME_A20_CAPABILITY_VERSION 1u

struct runtime_a20_capability_request {
  uint32_t version;
  uint32_t operation;
  uint32_t requested_enabled;
};

struct runtime_a20_capability_result {
  uint32_t status;
  uint32_t enabled;
};

#ifdef __cplusplus
extern "C" {
#endif
/* Minimal-machine lifecycle owner only; fixed C linkage permits the adapter
 * to call the same typed record without importing a C++ object. */
void runtime_a20_capability_set_lifecycle_active(uint32_t active);
void runtime_a20_capability_dispatch(
  const struct runtime_a20_capability_request *request,
  struct runtime_a20_capability_result *result);
#ifdef __cplusplus
}
#endif

#endif
