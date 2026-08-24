#ifndef NTDOS64_LIFECYCLE_V1_H
#define NTDOS64_LIFECYCLE_V1_H

/*
 * CLI-owned lifecycle policy and result classification.  This is deliberately
 * downstream of the copied mantle engine contract: it neither adds a machine
 * cancellation primitive nor carries guest state, Bochs state or a Windows
 * handle across the CLI/engine boundary.
 */

#include <stdint.h>

#include "engine_contract_v1.h"

#define NTDOS64_LIFECYCLE_V1_POLICY_MAGIC UINT32_C(0x4e4c5031)
#define NTDOS64_LIFECYCLE_V1_AUDIT_MAGIC  UINT32_C(0x4e4c4131)
#define NTDOS64_LIFECYCLE_V1_VERSION UINT32_C(1)

enum ntdos64_lifecycle_v1_cancellation_request {
    NTDOS64_LIFECYCLE_V1_CANCELLATION_NONE = 0u,
    NTDOS64_LIFECYCLE_V1_CANCELLATION_REQUESTED
};

enum ntdos64_lifecycle_v1_terminal {
    NTDOS64_LIFECYCLE_V1_TERMINAL_NONE = 0u,
    NTDOS64_LIFECYCLE_V1_TERMINAL_REJECTED_POLICY,
    NTDOS64_LIFECYCLE_V1_TERMINAL_REJECTED_ENGINE_REQUEST,
    NTDOS64_LIFECYCLE_V1_TERMINAL_REJECTED_COMPOSITION,
    NTDOS64_LIFECYCLE_V1_TERMINAL_MACHINE_FAILURE,
    NTDOS64_LIFECYCLE_V1_TERMINAL_EXECUTION_BUDGET,
    NTDOS64_LIFECYCLE_V1_TERMINAL_CONTROLLED_GUEST_TERMINAL,
    NTDOS64_LIFECYCLE_V1_TERMINAL_ORDINARY_GUEST_COMPLETION,
    NTDOS64_LIFECYCLE_V1_TERMINAL_HOST_CANCELLATION,
    NTDOS64_LIFECYCLE_V1_TERMINAL_INVALID_ENGINE_RESULT
};

enum ntdos64_lifecycle_v1_presentation {
    NTDOS64_LIFECYCLE_V1_PRESENTATION_NONE = 0u,
    NTDOS64_LIFECYCLE_V1_PRESENTATION_REJECTED_POLICY,
    NTDOS64_LIFECYCLE_V1_PRESENTATION_REJECTED_ENGINE_REQUEST,
    NTDOS64_LIFECYCLE_V1_PRESENTATION_REJECTED_COMPOSITION,
    NTDOS64_LIFECYCLE_V1_PRESENTATION_MACHINE_FAILURE,
    NTDOS64_LIFECYCLE_V1_PRESENTATION_EXECUTION_BUDGET,
    NTDOS64_LIFECYCLE_V1_PRESENTATION_CONTROLLED_GUEST_TERMINAL,
    NTDOS64_LIFECYCLE_V1_PRESENTATION_ORDINARY_GUEST_COMPLETION,
    NTDOS64_LIFECYCLE_V1_PRESENTATION_HOST_CANCELLATION,
    NTDOS64_LIFECYCLE_V1_PRESENTATION_INVALID_ENGINE_RESULT
};

struct ntdos64_lifecycle_v1_policy {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t cancellation_request;
    uint64_t instruction_tick_budget;
    uint32_t reserved0;
    uint32_t reserved1;
};

struct ntdos64_lifecycle_v1_audit {
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

void ntdos64_lifecycle_v1_policy_clear(
    struct ntdos64_lifecycle_v1_policy *policy);
int ntdos64_lifecycle_v1_policy_valid(
    const struct ntdos64_lifecycle_v1_policy *policy);
void ntdos64_lifecycle_v1_audit_clear(struct ntdos64_lifecycle_v1_audit *audit);
int ntdos64_lifecycle_v1_audit_valid(
    const struct ntdos64_lifecycle_v1_audit *audit);
int ntdos64_lifecycle_v1_classify(
    const struct ntdos64_lifecycle_v1_policy *policy,
    const struct bx_ntvdm_engine_result_v1 *engine_result,
    struct ntdos64_lifecycle_v1_audit *audit);

#endif
