#ifndef RUNTIME_ENGINE_CONTRACT_V1_H
#define RUNTIME_ENGINE_CONTRACT_V1_H

/*
 * Public copied-data contract between the CLI-owned native-engine executable
 * and the bx-vdm composition entry.  This header intentionally depends only
 * on fixed-width C types: it exposes no Bochs object, guest locator, provider
 * identity, host handle, or retained caller pointer.  adapter-bochs accepts only
 * already-assembled selector-blind machine-stage records from that entry.
 */

#include <stdint.h>

#define RUNTIME_ENGINE_REQUEST_V1_MAGIC UINT32_C(0x42584551)
#define RUNTIME_ENGINE_RESULT_V1_MAGIC  UINT32_C(0x42584552)
#define RUNTIME_ENGINE_CONTRACT_V1_VERSION UINT32_C(5)
#define RUNTIME_ENGINE_V1_MAX_DESCRIPTOR_CHARS UINT32_C(260)
#define RUNTIME_ENGINE_V1_MAX_LAUNCH_CHARS UINT32_C(256)

/* Fixed composition-selection values.  The engine carries only this copied
 * value; bx-vdm owns its profile interpretation and all host capability. */
enum runtime_engine_mutation_mode_v1 {
    RUNTIME_ENGINE_MUTATION_MODE_V1_DIRECT = 1u,
    RUNTIME_ENGINE_MUTATION_MODE_V1_READONLY = 2u
};

enum runtime_engine_terminal_kind_v1 {
    RUNTIME_ENGINE_TERMINAL_V1_NONE = 0u,
    RUNTIME_ENGINE_TERMINAL_V1_REJECTED_REQUEST,
    RUNTIME_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION,
    RUNTIME_ENGINE_TERMINAL_V1_MACHINE_FAILURE,
    RUNTIME_ENGINE_TERMINAL_V1_EXECUTION_BUDGET,
    RUNTIME_ENGINE_TERMINAL_V1_CONTROLLED_GUEST_TERMINAL,
    RUNTIME_ENGINE_TERMINAL_V1_ORDINARY_GUEST_COMPLETION,
    RUNTIME_ENGINE_TERMINAL_V1_HOST_CANCELLATION
};

struct runtime_engine_request_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t mutation_mode;
    uint32_t reserved0;
    uint32_t profile_descriptor_chars;
    uint32_t root_descriptor_chars;
    uint32_t launch_descriptor_chars;
    uint64_t instruction_tick_budget;
    uint64_t guest_memory_bytes;
    uint64_t reserved_memory_base;
    uint64_t reserved_memory_bytes;
    uint16_t profile_descriptor[RUNTIME_ENGINE_V1_MAX_DESCRIPTOR_CHARS + 1u];
    uint16_t root_descriptor[RUNTIME_ENGINE_V1_MAX_DESCRIPTOR_CHARS + 1u];
    uint16_t launch_descriptor[RUNTIME_ENGINE_V1_MAX_LAUNCH_CHARS + 1u];
};

struct runtime_engine_result_v1 {
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

void runtime_engine_request_v1_clear(struct runtime_engine_request_v1 *request);
int runtime_engine_request_v1_valid(const struct runtime_engine_request_v1 *request);
void runtime_engine_result_v1_clear(struct runtime_engine_result_v1 *result);
int runtime_engine_result_v1_set(struct runtime_engine_result_v1 *result,
    uint32_t terminal_kind, uint32_t detail_code);
int runtime_engine_result_v1_valid(const struct runtime_engine_result_v1 *result);
/* Requests cancellation of the one active process-local engine run.  The
 * fixed reason contains no host handle, callback, guest state or selector. */
int runtime_engine_request_cancellation_v1(uint32_t reason);
int runtime_engine_run_v1(const struct runtime_engine_request_v1 *request,
    struct runtime_engine_result_v1 *result);

#ifdef __cplusplus
}
#endif

#endif
