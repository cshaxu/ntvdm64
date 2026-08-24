#ifndef RUNTIME_SESSION_RESOURCE_ABI_H
#define RUNTIME_SESSION_RESOURCE_ABI_H

#include <stdint.h>

#define RUNTIME_SESSION_RESOURCE_ABI_MAGIC 0x42585253u
#define RUNTIME_SESSION_RESOURCE_ABI_VERSION 1u

typedef enum runtime_session_resource_kind_v1 {
    RUNTIME_SESSION_RESOURCE_NONE = 0u,
    RUNTIME_SESSION_RESOURCE_NTIO = 1u,
    RUNTIME_SESSION_RESOURCE_NTDOS = 2u,
    RUNTIME_SESSION_RESOURCE_COMMAND = 3u
} runtime_session_resource_kind_v1;

/* Identity only. Payload ownership remains in the CLI/adapter same-island
 * staging layer and is never encoded as an address here. */
typedef struct runtime_session_resource_v1 {
    uint32_t kind;
    uint32_t flags;
    uint64_t byte_count;
    uint8_t sha256[32];
} runtime_session_resource_v1;

typedef struct runtime_session_resources_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t flags;
    runtime_session_resource_v1 resources[3];
} runtime_session_resources_v1;

#ifdef __cplusplus
extern "C" {
#endif

void runtime_session_resources_v1_initialize(
    runtime_session_resources_v1 *resources);
int runtime_session_resources_v1_valid(
    const runtime_session_resources_v1 *resources);

#ifdef __cplusplus
}
#endif

#endif
