#include "lifecycle_v1.h"

#include <string.h>

static int cancellation_request_valid(uint32_t value)
{
    return value == NTDOS64_LIFECYCLE_V1_CANCELLATION_NONE ||
        value == NTDOS64_LIFECYCLE_V1_CANCELLATION_REQUESTED;
}

static int terminal_valid(uint32_t value)
{
    return value <= NTDOS64_LIFECYCLE_V1_TERMINAL_INVALID_ENGINE_RESULT;
}

static int presentation_valid(uint32_t value)
{
    return value <= NTDOS64_LIFECYCLE_V1_PRESENTATION_INVALID_ENGINE_RESULT;
}

void ntdos64_lifecycle_v1_policy_clear(struct ntdos64_lifecycle_v1_policy *policy)
{
    if (policy == 0) return;
    memset(policy, 0, sizeof(*policy));
    policy->magic = NTDOS64_LIFECYCLE_V1_POLICY_MAGIC;
    policy->abi_version = NTDOS64_LIFECYCLE_V1_VERSION;
    policy->struct_bytes = (uint32_t) sizeof(*policy);
}

int ntdos64_lifecycle_v1_policy_valid(
    const struct ntdos64_lifecycle_v1_policy *policy)
{
    return policy != 0 && policy->magic == NTDOS64_LIFECYCLE_V1_POLICY_MAGIC &&
        policy->abi_version == NTDOS64_LIFECYCLE_V1_VERSION &&
        policy->struct_bytes == sizeof(*policy) &&
        cancellation_request_valid(policy->cancellation_request) &&
        policy->instruction_tick_budget != 0u && policy->reserved0 == 0u &&
        policy->reserved1 == 0u;
}

void ntdos64_lifecycle_v1_audit_clear(struct ntdos64_lifecycle_v1_audit *audit)
{
    if (audit == 0) return;
    memset(audit, 0, sizeof(*audit));
    audit->magic = NTDOS64_LIFECYCLE_V1_AUDIT_MAGIC;
    audit->abi_version = NTDOS64_LIFECYCLE_V1_VERSION;
    audit->struct_bytes = (uint32_t) sizeof(*audit);
}

int ntdos64_lifecycle_v1_audit_valid(const struct ntdos64_lifecycle_v1_audit *audit)
{
    return audit != 0 && audit->magic == NTDOS64_LIFECYCLE_V1_AUDIT_MAGIC &&
        audit->abi_version == NTDOS64_LIFECYCLE_V1_VERSION &&
        audit->struct_bytes == sizeof(*audit) &&
        cancellation_request_valid(audit->cancellation_request) &&
        terminal_valid(audit->lifecycle_terminal) &&
        presentation_valid(audit->presentation) && audit->reserved0 == 0u;
}

static void classify_engine_terminal(uint32_t engine_terminal,
    uint32_t *lifecycle_terminal, uint32_t *presentation)
{
    switch (engine_terminal) {
    case BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_REQUEST:
        *lifecycle_terminal = NTDOS64_LIFECYCLE_V1_TERMINAL_REJECTED_ENGINE_REQUEST;
        *presentation = NTDOS64_LIFECYCLE_V1_PRESENTATION_REJECTED_ENGINE_REQUEST;
        break;
    case BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION:
        *lifecycle_terminal = NTDOS64_LIFECYCLE_V1_TERMINAL_REJECTED_COMPOSITION;
        *presentation = NTDOS64_LIFECYCLE_V1_PRESENTATION_REJECTED_COMPOSITION;
        break;
    case BX_NTVDM_ENGINE_TERMINAL_V1_MACHINE_FAILURE:
        *lifecycle_terminal = NTDOS64_LIFECYCLE_V1_TERMINAL_MACHINE_FAILURE;
        *presentation = NTDOS64_LIFECYCLE_V1_PRESENTATION_MACHINE_FAILURE;
        break;
    case BX_NTVDM_ENGINE_TERMINAL_V1_EXECUTION_BUDGET:
        *lifecycle_terminal = NTDOS64_LIFECYCLE_V1_TERMINAL_EXECUTION_BUDGET;
        *presentation = NTDOS64_LIFECYCLE_V1_PRESENTATION_EXECUTION_BUDGET;
        break;
    case BX_NTVDM_ENGINE_TERMINAL_V1_CONTROLLED_GUEST_TERMINAL:
        *lifecycle_terminal = NTDOS64_LIFECYCLE_V1_TERMINAL_CONTROLLED_GUEST_TERMINAL;
        *presentation = NTDOS64_LIFECYCLE_V1_PRESENTATION_CONTROLLED_GUEST_TERMINAL;
        break;
    case BX_NTVDM_ENGINE_TERMINAL_V1_ORDINARY_GUEST_COMPLETION:
        *lifecycle_terminal = NTDOS64_LIFECYCLE_V1_TERMINAL_ORDINARY_GUEST_COMPLETION;
        *presentation = NTDOS64_LIFECYCLE_V1_PRESENTATION_ORDINARY_GUEST_COMPLETION;
        break;
    case BX_NTVDM_ENGINE_TERMINAL_V1_HOST_CANCELLATION:
        *lifecycle_terminal = NTDOS64_LIFECYCLE_V1_TERMINAL_HOST_CANCELLATION;
        *presentation = NTDOS64_LIFECYCLE_V1_PRESENTATION_HOST_CANCELLATION;
        break;
    default:
        *lifecycle_terminal = NTDOS64_LIFECYCLE_V1_TERMINAL_INVALID_ENGINE_RESULT;
        *presentation = NTDOS64_LIFECYCLE_V1_PRESENTATION_INVALID_ENGINE_RESULT;
        break;
    }
}

int ntdos64_lifecycle_v1_classify(
    const struct ntdos64_lifecycle_v1_policy *policy,
    const struct bx_ntvdm_engine_result_v1 *engine_result,
    struct ntdos64_lifecycle_v1_audit *audit)
{
    if (audit == 0) return 0;
    ntdos64_lifecycle_v1_audit_clear(audit);
    if (!ntdos64_lifecycle_v1_policy_valid(policy)) {
        audit->lifecycle_terminal = NTDOS64_LIFECYCLE_V1_TERMINAL_REJECTED_POLICY;
        audit->presentation = NTDOS64_LIFECYCLE_V1_PRESENTATION_REJECTED_POLICY;
        return 0;
    }
    audit->cancellation_request = policy->cancellation_request;
    if (!bx_ntvdm_engine_result_v1_valid(engine_result)) {
        audit->lifecycle_terminal = NTDOS64_LIFECYCLE_V1_TERMINAL_INVALID_ENGINE_RESULT;
        audit->presentation = NTDOS64_LIFECYCLE_V1_PRESENTATION_INVALID_ENGINE_RESULT;
        return 0;
    }
    audit->engine_terminal_kind = engine_result->terminal_kind;
    audit->engine_detail_code = engine_result->detail_code;
    classify_engine_terminal(engine_result->terminal_kind,
        &audit->lifecycle_terminal, &audit->presentation);
    return audit->lifecycle_terminal != NTDOS64_LIFECYCLE_V1_TERMINAL_INVALID_ENGINE_RESULT;
}
