#include "lifecycle.h"

#include <string.h>

static int cancellation_request_valid(uint32_t value)
{
    return value == APP_LIFECYCLE_V1_CANCELLATION_NONE ||
        value == APP_LIFECYCLE_V1_CANCELLATION_REQUESTED;
}

static int terminal_valid(uint32_t value)
{
    return value <= APP_LIFECYCLE_V1_TERMINAL_INVALID_ENGINE_RESULT;
}

static int presentation_valid(uint32_t value)
{
    return value <= APP_LIFECYCLE_V1_PRESENTATION_INVALID_ENGINE_RESULT;
}

void app_lifecycle_v1_policy_clear(struct app_lifecycle_v1_policy *policy)
{
    if (policy == 0) return;
    memset(policy, 0, sizeof(*policy));
    policy->magic = APP_LIFECYCLE_V1_POLICY_MAGIC;
    policy->abi_version = APP_LIFECYCLE_V1_VERSION;
    policy->struct_bytes = (uint32_t) sizeof(*policy);
}

int app_lifecycle_v1_policy_valid(
    const struct app_lifecycle_v1_policy *policy)
{
    return policy != 0 && policy->magic == APP_LIFECYCLE_V1_POLICY_MAGIC &&
        policy->abi_version == APP_LIFECYCLE_V1_VERSION &&
        policy->struct_bytes == sizeof(*policy) &&
        cancellation_request_valid(policy->cancellation_request) &&
        policy->instruction_tick_budget != 0u && policy->reserved0 == 0u &&
        policy->reserved1 == 0u;
}

void app_lifecycle_v1_audit_clear(struct app_lifecycle_v1_audit *audit)
{
    if (audit == 0) return;
    memset(audit, 0, sizeof(*audit));
    audit->magic = APP_LIFECYCLE_V1_AUDIT_MAGIC;
    audit->abi_version = APP_LIFECYCLE_V1_VERSION;
    audit->struct_bytes = (uint32_t) sizeof(*audit);
}

int app_lifecycle_v1_audit_valid(const struct app_lifecycle_v1_audit *audit)
{
    return audit != 0 && audit->magic == APP_LIFECYCLE_V1_AUDIT_MAGIC &&
        audit->abi_version == APP_LIFECYCLE_V1_VERSION &&
        audit->struct_bytes == sizeof(*audit) &&
        cancellation_request_valid(audit->cancellation_request) &&
        terminal_valid(audit->lifecycle_terminal) &&
        presentation_valid(audit->presentation) && audit->reserved0 == 0u;
}

static void classify_engine_terminal(uint32_t engine_terminal,
    uint32_t *lifecycle_terminal, uint32_t *presentation)
{
    switch (engine_terminal) {
    case RUNTIME_ENGINE_TERMINAL_V1_REJECTED_REQUEST:
        *lifecycle_terminal = APP_LIFECYCLE_V1_TERMINAL_REJECTED_ENGINE_REQUEST;
        *presentation = APP_LIFECYCLE_V1_PRESENTATION_REJECTED_ENGINE_REQUEST;
        break;
    case RUNTIME_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION:
        *lifecycle_terminal = APP_LIFECYCLE_V1_TERMINAL_REJECTED_COMPOSITION;
        *presentation = APP_LIFECYCLE_V1_PRESENTATION_REJECTED_COMPOSITION;
        break;
    case RUNTIME_ENGINE_TERMINAL_V1_MACHINE_FAILURE:
        *lifecycle_terminal = APP_LIFECYCLE_V1_TERMINAL_MACHINE_FAILURE;
        *presentation = APP_LIFECYCLE_V1_PRESENTATION_MACHINE_FAILURE;
        break;
    case RUNTIME_ENGINE_TERMINAL_V1_EXECUTION_BUDGET:
        *lifecycle_terminal = APP_LIFECYCLE_V1_TERMINAL_EXECUTION_BUDGET;
        *presentation = APP_LIFECYCLE_V1_PRESENTATION_EXECUTION_BUDGET;
        break;
    case RUNTIME_ENGINE_TERMINAL_V1_CONTROLLED_GUEST_TERMINAL:
        *lifecycle_terminal = APP_LIFECYCLE_V1_TERMINAL_CONTROLLED_GUEST_TERMINAL;
        *presentation = APP_LIFECYCLE_V1_PRESENTATION_CONTROLLED_GUEST_TERMINAL;
        break;
    case RUNTIME_ENGINE_TERMINAL_V1_ORDINARY_GUEST_COMPLETION:
        *lifecycle_terminal = APP_LIFECYCLE_V1_TERMINAL_ORDINARY_GUEST_COMPLETION;
        *presentation = APP_LIFECYCLE_V1_PRESENTATION_ORDINARY_GUEST_COMPLETION;
        break;
    case RUNTIME_ENGINE_TERMINAL_V1_HOST_CANCELLATION:
        *lifecycle_terminal = APP_LIFECYCLE_V1_TERMINAL_HOST_CANCELLATION;
        *presentation = APP_LIFECYCLE_V1_PRESENTATION_HOST_CANCELLATION;
        break;
    default:
        *lifecycle_terminal = APP_LIFECYCLE_V1_TERMINAL_INVALID_ENGINE_RESULT;
        *presentation = APP_LIFECYCLE_V1_PRESENTATION_INVALID_ENGINE_RESULT;
        break;
    }
}

int app_lifecycle_v1_classify(
    const struct app_lifecycle_v1_policy *policy,
    const struct runtime_engine_result_v1 *engine_result,
    struct app_lifecycle_v1_audit *audit)
{
    if (audit == 0) return 0;
    app_lifecycle_v1_audit_clear(audit);
    if (!app_lifecycle_v1_policy_valid(policy)) {
        audit->lifecycle_terminal = APP_LIFECYCLE_V1_TERMINAL_REJECTED_POLICY;
        audit->presentation = APP_LIFECYCLE_V1_PRESENTATION_REJECTED_POLICY;
        return 0;
    }
    audit->cancellation_request = policy->cancellation_request;
    if (!runtime_engine_result_v1_valid(engine_result)) {
        audit->lifecycle_terminal = APP_LIFECYCLE_V1_TERMINAL_INVALID_ENGINE_RESULT;
        audit->presentation = APP_LIFECYCLE_V1_PRESENTATION_INVALID_ENGINE_RESULT;
        return 0;
    }
    audit->engine_terminal_kind = engine_result->terminal_kind;
    audit->engine_detail_code = engine_result->detail_code;
    classify_engine_terminal(engine_result->terminal_kind,
        &audit->lifecycle_terminal, &audit->presentation);
    return audit->lifecycle_terminal != APP_LIFECYCLE_V1_TERMINAL_INVALID_ENGINE_RESULT;
}
