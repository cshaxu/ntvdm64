#include "lifecycle_v1.h"

int main(void)
{
    struct ntdos64_lifecycle_v1_policy policy;
    struct ntdos64_lifecycle_v1_audit audit;
    struct bx_ntvdm_engine_result_v1 engine;
    uint32_t kind;

    ntdos64_lifecycle_v1_policy_clear(&policy);
    policy.instruction_tick_budget = UINT64_C(1000);
    if (!ntdos64_lifecycle_v1_policy_valid(&policy)) return 1;
    policy.cancellation_request = 2u;
    if (ntdos64_lifecycle_v1_policy_valid(&policy)) return 2;
    policy.cancellation_request = NTDOS64_LIFECYCLE_V1_CANCELLATION_REQUESTED;
    if (!ntdos64_lifecycle_v1_policy_valid(&policy)) return 3;
    policy.reserved0 = 1u;
    if (ntdos64_lifecycle_v1_policy_valid(&policy)) return 4;
    policy.reserved0 = 0u;

    bx_ntvdm_engine_result_v1_clear(&engine);
    for (kind = BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_REQUEST;
         kind <= BX_NTVDM_ENGINE_TERMINAL_V1_HOST_CANCELLATION; ++kind) {
        if (!bx_ntvdm_engine_result_v1_set(&engine, kind, kind + 10u) ||
            !ntdos64_lifecycle_v1_classify(&policy, &engine, &audit) ||
            !ntdos64_lifecycle_v1_audit_valid(&audit) ||
            audit.cancellation_request != NTDOS64_LIFECYCLE_V1_CANCELLATION_REQUESTED ||
            audit.engine_terminal_kind != kind || audit.engine_detail_code != kind + 10u)
            return (int)(20u + kind);
        if (kind == BX_NTVDM_ENGINE_TERMINAL_V1_CONTROLLED_GUEST_TERMINAL &&
            audit.lifecycle_terminal != NTDOS64_LIFECYCLE_V1_TERMINAL_CONTROLLED_GUEST_TERMINAL)
            return 40;
        if (kind == BX_NTVDM_ENGINE_TERMINAL_V1_ORDINARY_GUEST_COMPLETION &&
            audit.lifecycle_terminal != NTDOS64_LIFECYCLE_V1_TERMINAL_ORDINARY_GUEST_COMPLETION)
            return 41;
        if (kind == BX_NTVDM_ENGINE_TERMINAL_V1_HOST_CANCELLATION &&
            audit.lifecycle_terminal != NTDOS64_LIFECYCLE_V1_TERMINAL_HOST_CANCELLATION)
            return 42;
    }
    if (!bx_ntvdm_engine_result_v1_set(&engine,
        BX_NTVDM_ENGINE_TERMINAL_V1_CONTROLLED_GUEST_TERMINAL, 0u) ||
        !ntdos64_lifecycle_v1_classify(&policy, &engine, &audit) ||
        audit.lifecycle_terminal == NTDOS64_LIFECYCLE_V1_TERMINAL_HOST_CANCELLATION)
        return 43;
    engine.reserved0 = 1u;
    if (ntdos64_lifecycle_v1_classify(&policy, &engine, &audit) ||
        audit.lifecycle_terminal != NTDOS64_LIFECYCLE_V1_TERMINAL_INVALID_ENGINE_RESULT ||
        !ntdos64_lifecycle_v1_audit_valid(&audit)) return 44;
    engine.reserved0 = 0u;
    policy.instruction_tick_budget = 0u;
    if (ntdos64_lifecycle_v1_classify(&policy, &engine, &audit) ||
        audit.lifecycle_terminal != NTDOS64_LIFECYCLE_V1_TERMINAL_REJECTED_POLICY ||
        !ntdos64_lifecycle_v1_audit_valid(&audit)) return 45;
    return 0;
}
