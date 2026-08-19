#ifndef BX_NTVDM_STARTUP_CONFIGURATION_PROVIDER_V1_H
#define BX_NTVDM_STARTUP_CONFIGURATION_PROVIDER_V1_H

#include <stdint.h>

#include "bx_ntvdm_startup_configuration_policy_v1.h"

#define BX_NTVDM_STARTUP_CONFIGURATION_PROVIDER_V1_MAGIC 0x42584346u
#define BX_NTVDM_STARTUP_CONFIGURATION_PROVIDER_V1_VERSION 2u
#define BX_NTVDM_STARTUP_CONFIGURATION_INPUT_V1_BYTES 4096u
#define BX_NTVDM_STARTUP_CONFIGURATION_IMAGE_V1_BYTES 12288u
#define BX_NTVDM_STARTUP_CONFIGURATION_MERGE_V1_BYTES 4096u
#define BX_NTVDM_STARTUP_CONFIGURATION_SYSTEM_ROOT_V1_BYTES 260u

enum bx_ntvdm_startup_configuration_result_v1 {
    BX_NTVDM_STARTUP_CONFIGURATION_RESULT_V1_READY = 1u,
    BX_NTVDM_STARTUP_CONFIGURATION_RESULT_V1_SOURCE_INVALID = 2u,
    BX_NTVDM_STARTUP_CONFIGURATION_RESULT_V1_BOUNDS = 3u,
    BX_NTVDM_STARTUP_CONFIGURATION_RESULT_V1_CONSOLE_DEFERRED = 4u
};

/* OpenNT normally supplies its historical System32 command.com through a
 * generated CONFIG.SYS SHELL= line. A CLI profile states whether that named
 * host capability was present at installation time. */
enum bx_ntvdm_startup_configuration_shell_capability_v1 {
    BX_NTVDM_STARTUP_CONFIGURATION_SHELL_V1_ORIGINAL_HOST = 1u,
    BX_NTVDM_STARTUP_CONFIGURATION_SHELL_V1_DECLARED_GUEST = 2u
};

/* All inputs are copied at CLI/provider admission.  The byte arrays are OEM
 * source data, not host paths, handles, guest addresses, or BOP operands. */
typedef struct bx_ntvdm_startup_configuration_input_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    bx_ntvdm_startup_configuration_policy_v1 policy;
    uint32_t system_root_bytes;
    uint32_t country_id;
    uint32_t oem_code_page;
    uint32_t config_bytes;
    uint32_t autoexec_bytes;
    uint32_t shell_capability;
    uint8_t system_root[BX_NTVDM_STARTUP_CONFIGURATION_SYSTEM_ROOT_V1_BYTES];
    uint8_t config[BX_NTVDM_STARTUP_CONFIGURATION_INPUT_V1_BYTES];
    uint8_t autoexec[BX_NTVDM_STARTUP_CONFIGURATION_INPUT_V1_BYTES];
} bx_ntvdm_startup_configuration_input_v1;

typedef struct bx_ntvdm_startup_configuration_provider_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t result;
    uint32_t config_image_bytes;
    uint32_t autoexec_image_bytes;
    uint32_t merge_bytes;
    uint32_t reserved0;
    uint8_t config_image[BX_NTVDM_STARTUP_CONFIGURATION_IMAGE_V1_BYTES];
    uint8_t autoexec_image[BX_NTVDM_STARTUP_CONFIGURATION_IMAGE_V1_BYTES];
    uint8_t merge[BX_NTVDM_STARTUP_CONFIGURATION_MERGE_V1_BYTES];
} bx_ntvdm_startup_configuration_provider_v1;

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_startup_configuration_provider_v1_clear(
    bx_ntvdm_startup_configuration_provider_v1 *provider);
int bx_ntvdm_startup_configuration_input_v1_valid(
    const bx_ntvdm_startup_configuration_input_v1 *input);
int bx_ntvdm_startup_configuration_provider_v1_valid(
    const bx_ntvdm_startup_configuration_provider_v1 *provider);
int bx_ntvdm_startup_configuration_provider_v1_build(
    bx_ntvdm_startup_configuration_provider_v1 *provider,
    const bx_ntvdm_startup_configuration_input_v1 *input);

#ifdef __cplusplus
}
#endif

#endif
