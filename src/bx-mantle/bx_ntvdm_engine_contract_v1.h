#ifndef BX_NTVDM_ENGINE_CONTRACT_V1_H
#define BX_NTVDM_ENGINE_CONTRACT_V1_H

/*
 * Public copied-data contract between the CLI-owned native-engine executable
 * and the mantle-owned engine entry.  This header intentionally depends only
 * on fixed-width C types: it exposes no Bochs object, guest locator, provider
 * identity, host handle, or retained caller pointer.
 */

#include <stdint.h>

#define BX_NTVDM_ENGINE_REQUEST_V1_MAGIC UINT32_C(0x42584551)
#define BX_NTVDM_ENGINE_RESULT_V1_MAGIC  UINT32_C(0x42584552)
#define BX_NTVDM_ENGINE_CONTRACT_V1_VERSION UINT32_C(1)
#define BX_NTVDM_ENGINE_V1_MAX_DESCRIPTOR_CHARS UINT32_C(260)
#define BX_NTVDM_ENGINE_V1_MAX_LAUNCH_CHARS UINT32_C(256)

enum bx_ntvdm_engine_terminal_kind_v1 {
    BX_NTVDM_ENGINE_TERMINAL_V1_NONE = 0u,
    BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_REQUEST,
    BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION,
    BX_NTVDM_ENGINE_TERMINAL_V1_MACHINE_FAILURE,
    BX_NTVDM_ENGINE_TERMINAL_V1_EXECUTION_BUDGET,
    BX_NTVDM_ENGINE_TERMINAL_V1_CONTROLLED_GUEST_TERMINAL,
    BX_NTVDM_ENGINE_TERMINAL_V1_ORDINARY_GUEST_COMPLETION,
    BX_NTVDM_ENGINE_TERMINAL_V1_HOST_CANCELLATION
};

struct bx_ntvdm_engine_request_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t admitted_drive_mask;
    uint32_t excluded_drive_mask;
    uint32_t profile_descriptor_chars;
    uint32_t root_descriptor_chars;
    uint32_t launch_descriptor_chars;
    uint64_t instruction_tick_budget;
    uint16_t profile_descriptor[BX_NTVDM_ENGINE_V1_MAX_DESCRIPTOR_CHARS + 1u];
    uint16_t root_descriptor[BX_NTVDM_ENGINE_V1_MAX_DESCRIPTOR_CHARS + 1u];
    uint16_t launch_descriptor[BX_NTVDM_ENGINE_V1_MAX_LAUNCH_CHARS + 1u];
};

struct bx_ntvdm_engine_result_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t terminal_kind;
    uint32_t detail_code;
    uint32_t reserved0;
};

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_engine_request_v1_clear(struct bx_ntvdm_engine_request_v1 *request);
int bx_ntvdm_engine_request_v1_valid(const struct bx_ntvdm_engine_request_v1 *request);
void bx_ntvdm_engine_result_v1_clear(struct bx_ntvdm_engine_result_v1 *result);
int bx_ntvdm_engine_result_v1_set(struct bx_ntvdm_engine_result_v1 *result,
    uint32_t terminal_kind, uint32_t detail_code);
int bx_ntvdm_engine_result_v1_valid(const struct bx_ntvdm_engine_result_v1 *result);
int bx_ntvdm_engine_run_v1(const struct bx_ntvdm_engine_request_v1 *request,
    struct bx_ntvdm_engine_result_v1 *result);

#ifdef __cplusplus
}
#endif

#endif
