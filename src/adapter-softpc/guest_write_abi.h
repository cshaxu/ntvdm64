#ifndef RUNTIME_GUEST_WRITE_ABI_H
#define RUNTIME_GUEST_WRITE_ABI_H

#include <stdint.h>

#define RUNTIME_GUEST_WRITE_ABI_MAGIC 0x42584757u
#define RUNTIME_GUEST_WRITE_ABI_VERSION 1u

/* A descriptor for one proposed physical guest-memory write.  Payload bytes
 * are a separate copied transport region indexed by payload_offset; this ABI
 * never contains a guest, host, or Bochs pointer. */
typedef struct runtime_guest_write_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t flags;
    uint64_t guest_physical_address;
    uint64_t byte_count;
    uint64_t payload_offset;
} runtime_guest_write_v1;

#ifdef __cplusplus
extern "C" {
#endif

void runtime_guest_write_v1_initialize(runtime_guest_write_v1 *write,
    uint64_t guest_physical_address, uint64_t byte_count,
    uint64_t payload_offset);
int runtime_guest_write_v1_preflight(const runtime_guest_write_v1 *write,
    uint64_t aperture_bytes, uint64_t payload_bytes);

#ifdef __cplusplus
}
#endif

#endif
