#include "machine_engine.h"

#include "cancellation_controller.h"

#include <string.h>

static int cancelled(uint32_t *reason)
{
    uint32_t value = runtime_cancellation_controller_requested_reason();
    if (value == RUNTIME_CANCELLATION_NONE) return 0;
    if (reason != 0) *reason = value;
    return 1;
}

int runtime_machine_engine_run(
    const struct runtime_machine_stage_request *stage,
    const struct runtime_machine_stage_entry *entry,
    const struct runtime_machine_stage_execution_request *execution,
    struct runtime_machine_engine_result *result)
{
    if (stage == 0 || entry == 0 || execution == 0 || result == 0) return 0;
    memset(result, 0, sizeof(*result));
    result->begin_status = RUNTIME_MACHINE_STAGE_REJECTED_INPUT;
    result->entry_status = RUNTIME_MACHINE_STAGE_REJECTED_ENTRY;
    result->execution_status = RUNTIME_MACHINE_STAGE_EXECUTION_REJECTED_INACTIVE;
    result->reset_status = RUNTIME_MACHINE_STAGE_OK;
    if (!runtime_cancellation_controller_activate()) return 0;
    if (cancelled(&result->cancellation_reason)) {
        result->execution_status = RUNTIME_MACHINE_STAGE_EXECUTION_HOST_CANCELLATION;
        runtime_cancellation_controller_deactivate();
        return 1;
    }
    result->begin_status = runtime_machine_stage_begin(stage);
    if (result->begin_status == RUNTIME_MACHINE_STAGE_OK)
        result->entry_status = runtime_machine_stage_arm_real_mode_entry(entry);
    if (result->entry_status == RUNTIME_MACHINE_STAGE_OK) {
        result->execution_status = cancelled(&result->cancellation_reason) ?
            RUNTIME_MACHINE_STAGE_EXECUTION_HOST_CANCELLATION :
            runtime_machine_stage_execute(execution);
    }
    result->reset_status = runtime_machine_stage_reset();
    runtime_cancellation_controller_deactivate();
    return 1;
}
