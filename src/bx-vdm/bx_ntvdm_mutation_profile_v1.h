#ifndef BX_NTVDM_MUTATION_PROFILE_V1_H
#define BX_NTVDM_MUTATION_PROFILE_V1_H

#include <stdint.h>

#define BX_NTVDM_MUTATION_PROFILE_V1_MAGIC 0x42584d55u
#define BX_NTVDM_MUTATION_PROFILE_V1_VERSION 1u
#define BX_NTVDM_MUTATION_PROFILE_V1_MAX_OWNERS 8u

enum bx_ntvdm_mutation_mode_v1 {
    BX_NTVDM_MUTATION_MODE_V1_DIRECT = 1u,
    BX_NTVDM_MUTATION_MODE_V1_READONLY = 2u,
    BX_NTVDM_MUTATION_MODE_V1_OVERLAY = 3u,
    BX_NTVDM_MUTATION_MODE_V1_VIRTUAL = 4u
};

enum bx_ntvdm_mutation_class_v1 {
    BX_NTVDM_MUTATION_CLASS_V1_NONE_MECHANICAL = 0u,
    BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT = 1u,
    BX_NTVDM_MUTATION_CLASS_V1_FILE_METADATA = 2u,
    BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT = 3u,
    BX_NTVDM_MUTATION_CLASS_V1_HOST_GLOBAL = 4u,
    BX_NTVDM_MUTATION_CLASS_V1_DEVICE_RAW_MEDIA = 5u,
    BX_NTVDM_MUTATION_CLASS_V1_IPC_NETWORK = 6u
};

/* IDs name package ownership only. They carry no BOP selector/service and
 * cannot make an endpoint-specific profile choice. */
enum bx_ntvdm_mutation_owner_v1 {
    BX_NTVDM_MUTATION_OWNER_V1_DEM = 1u,
    BX_NTVDM_MUTATION_OWNER_V1_COMMAND = 2u,
    BX_NTVDM_MUTATION_OWNER_V1_REDIR = 3u,
    BX_NTVDM_MUTATION_OWNER_V1_DEVICE = 4u,
    BX_NTVDM_MUTATION_OWNER_V1_ENGINE = 5u
};

typedef struct bx_ntvdm_mutation_owner_registration_v1 {
    uint32_t owner_id;
    uint32_t mutation_class;
    uint32_t permitted_mode_mask;
    uint32_t reserved0;
} bx_ntvdm_mutation_owner_registration_v1;

typedef struct bx_ntvdm_mutation_profile_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t mode;
    uint32_t owner_count;
    uint32_t reserved0;
    bx_ntvdm_mutation_owner_registration_v1
        owners[BX_NTVDM_MUTATION_PROFILE_V1_MAX_OWNERS];
} bx_ntvdm_mutation_profile_v1;

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_mutation_profile_v1_initialize(
    bx_ntvdm_mutation_profile_v1 *profile, uint32_t mode);
int bx_ntvdm_mutation_profile_v1_register_owner(
    bx_ntvdm_mutation_profile_v1 *profile,
    const bx_ntvdm_mutation_owner_registration_v1 *registration);
int bx_ntvdm_mutation_profile_v1_valid(
    const bx_ntvdm_mutation_profile_v1 *profile);
int bx_ntvdm_mutation_profile_v1_authorize(
    const bx_ntvdm_mutation_profile_v1 *profile, uint32_t owner_id,
    uint32_t mutation_class, uint32_t *mode_out);

#ifdef __cplusplus
}
#endif

#endif
