#include "bx_ntvdm_machine_engine_v1.h"

#include "bx_ntvdm_cancellation_controller_v1.h"

#include <string.h>

static int cancelled(uint32_t *reason)
{
    uint32_t value = bx_ntvdm_cancellation_controller_v1_requested_reason();
    if (value == BX_NTVDM_CANCELLATION_V1_NONE) return 0;
    if (reason != 0) *reason = value;
    return 1;
}

int bx_ntvdm_machine_engine_v1_run(
    const struct bx_ntvdm_machine_stage_v1_request *stage,
    const struct bx_ntvdm_machine_stage_v1_entry *entry,
    const struct bx_ntvdm_machine_stage_v1_execution_request *execution,
    struct bx_ntvdm_machine_engine_v1_result *result)
{
    if (stage == 0 || entry == 0 || execution == 0 || result == 0) return 0;
    memset(result, 0, sizeof(*result));
    result->begin_status = BX_NTVDM_MACHINE_STAGE_V1_REJECTED_INPUT;
    result->entry_status = BX_NTVDM_MACHINE_STAGE_V1_REJECTED_ENTRY;
    result->execution_status = BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_REJECTED_INACTIVE;
    result->reset_status = BX_NTVDM_MACHINE_STAGE_V1_OK;
    if (!bx_ntvdm_cancellation_controller_v1_activate()) return 0;
    if (cancelled(&result->cancellation_reason)) {
        result->execution_status = BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_HOST_CANCELLATION;
        bx_ntvdm_cancellation_controller_v1_deactivate();
        return 1;
    }
    result->begin_status = bx_ntvdm_machine_stage_v1_begin(stage);
    if (result->begin_status == BX_NTVDM_MACHINE_STAGE_V1_OK)
        result->entry_status = bx_ntvdm_machine_stage_v1_arm_real_mode_entry(entry);
    if (result->entry_status == BX_NTVDM_MACHINE_STAGE_V1_OK) {
        result->execution_status = cancelled(&result->cancellation_reason) ?
            BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_HOST_CANCELLATION :
            bx_ntvdm_machine_stage_v1_execute(execution);
    }
    result->reset_status = bx_ntvdm_machine_stage_v1_reset();
    bx_ntvdm_cancellation_controller_v1_deactivate();
    return 1;
}
