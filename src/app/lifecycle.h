#ifndef APP_LIFECYCLE_V1_H
#define APP_LIFECYCLE_V1_H

/*
 * CLI-owned lifecycle policy and result classification.  This is deliberately
 * downstream of the copied mantle engine contract: it neither adds a machine
 * cancellation primitive nor carries guest state, Bochs state or a Windows
 * handle across the CLI/engine boundary.
 */

#include <stdint.h>

#include "engine_contract.h"

#define APP_LIFECYCLE_V1_POLICY_MAGIC UINT32_C(0x4e4c5031)
#define APP_LIFECYCLE_V1_AUDIT_MAGIC  UINT32_C(0x4e4c4131)
#define APP_LIFECYCLE_V1_VERSION UINT32_C(1)

enum app_lifecycle_v1_cancellation_request {
    APP_LIFECYCLE_V1_CANCELLATION_NONE = 0u,
    APP_LIFECYCLE_V1_CANCELLATION_REQUESTED
};

enum app_lifecycle_v1_terminal {
    APP_LIFECYCLE_V1_TERMINAL_NONE = 0u,
    APP_LIFECYCLE_V1_TERMINAL_REJECTED_POLICY,
    APP_LIFECYCLE_V1_TERMINAL_REJECTED_ENGINE_REQUEST,
    APP_LIFECYCLE_V1_TERMINAL_REJECTED_COMPOSITION,
    APP_LIFECYCLE_V1_TERMINAL_MACHINE_FAILURE,
    APP_LIFECYCLE_V1_TERMINAL_EXECUTION_BUDGET,
    APP_LIFECYCLE_V1_TERMINAL_CONTROLLED_GUEST_TERMINAL,
    APP_LIFECYCLE_V1_TERMINAL_ORDINARY_GUEST_COMPLETION,
    APP_LIFECYCLE_V1_TERMINAL_HOST_CANCELLATION,
    APP_LIFECYCLE_V1_TERMINAL_INVALID_ENGINE_RESULT
};

enum app_lifecycle_v1_presentation {
    APP_LIFECYCLE_V1_PRESENTATION_NONE = 0u,
    APP_LIFECYCLE_V1_PRESENTATION_REJECTED_POLICY,
    APP_LIFECYCLE_V1_PRESENTATION_REJECTED_ENGINE_REQUEST,
    APP_LIFECYCLE_V1_PRESENTATION_REJECTED_COMPOSITION,
    APP_LIFECYCLE_V1_PRESENTATION_MACHINE_FAILURE,
    APP_LIFECYCLE_V1_PRESENTATION_EXECUTION_BUDGET,
    APP_LIFECYCLE_V1_PRESENTATION_CONTROLLED_GUEST_TERMINAL,
    APP_LIFECYCLE_V1_PRESENTATION_ORDINARY_GUEST_COMPLETION,
    APP_LIFECYCLE_V1_PRESENTATION_HOST_CANCELLATION,
    APP_LIFECYCLE_V1_PRESENTATION_INVALID_ENGINE_RESULT
};

struct app_lifecycle_v1_policy {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t cancellation_request;
    uint64_t instruction_tick_budget;
    uint32_t reserved0;
    uint32_t reserved1;
};

struct app_lifecycle_v1_audit {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t cancellation_request;
    uint32_t engine_terminal_kind;
    uint32_t engine_detail_code;
    uint32_t lifecycle_terminal;
    uint32_t presentation;
    uint32_t reserved0;
};

void app_lifecycle_v1_policy_clear(
    struct app_lifecycle_v1_policy *policy);
int app_lifecycle_v1_policy_valid(
    const struct app_lifecycle_v1_policy *policy);
void app_lifecycle_v1_audit_clear(struct app_lifecycle_v1_audit *audit);
int app_lifecycle_v1_audit_valid(
    const struct app_lifecycle_v1_audit *audit);
int app_lifecycle_v1_classify(
    const struct app_lifecycle_v1_policy *policy,
    const struct runtime_engine_result_v1 *engine_result,
    struct app_lifecycle_v1_audit *audit);

#endif
