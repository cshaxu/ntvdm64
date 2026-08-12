#ifndef BX_NTVDM_SESSION_RESOURCE_ABI_H
#define BX_NTVDM_SESSION_RESOURCE_ABI_H

#include <stdint.h>

#define BX_NTVDM_SESSION_RESOURCE_ABI_MAGIC 0x42585253u
#define BX_NTVDM_SESSION_RESOURCE_ABI_VERSION 1u

typedef enum bx_ntvdm_session_resource_kind_v1 {
    BX_NTVDM_SESSION_RESOURCE_NONE = 0u,
    BX_NTVDM_SESSION_RESOURCE_NTIO = 1u,
    BX_NTVDM_SESSION_RESOURCE_NTDOS = 2u,
    BX_NTVDM_SESSION_RESOURCE_COMMAND = 3u
} bx_ntvdm_session_resource_kind_v1;

/* Identity only. Payload ownership remains in the CLI/adapter same-island
 * staging layer and is never encoded as an address here. */
typedef struct bx_ntvdm_session_resource_v1 {
    uint32_t kind;
    uint32_t flags;
    uint64_t byte_count;
    uint8_t sha256[32];
} bx_ntvdm_session_resource_v1;

typedef struct bx_ntvdm_session_resources_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t flags;
    bx_ntvdm_session_resource_v1 resources[3];
} bx_ntvdm_session_resources_v1;

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_session_resources_v1_initialize(
    bx_ntvdm_session_resources_v1 *resources);
int bx_ntvdm_session_resources_v1_valid(
    const bx_ntvdm_session_resources_v1 *resources);

#ifdef __cplusplus
}
#endif

#endif
