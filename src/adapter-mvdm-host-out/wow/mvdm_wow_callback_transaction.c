#include "mvdm_wow_callback_transaction.h"

#include <string.h>

#include "adapter-mvdm-host-out/monitor/include/mvdm_wow_task_frame.h"
#include "session/session.h"

int mvdm_wow_callback_transaction_invoke(uint32_t return_id,
    uint32_t procedure, const uint8_t *parameters, uint16_t parameter_bytes,
    uint32_t *return_value_out)
{
    mvdm_wow_task_frame task;
    mvdm_wow_callback_transaction transaction;
    session *owner = session_thread_current();
    int32_t dispatch_result;

    if (return_value_out != NULL) *return_value_out = 0u;
    if (owner == NULL || return_value_out == NULL || parameter_bytes >
        MVDM_WOW_CALLBACK_PARAMETER_MAXIMUM ||
        (parameter_bytes != 0u && parameters == NULL) ||
        !mvdm_wow_task_frame_begin_callback(
            (uint32_t)sizeof(mvdm_wow_callback_transaction), &task)) return 0;
    memset(&transaction, 0, sizeof(transaction));
    transaction.struct_bytes = (uint32_t)sizeof(transaction);
    transaction.return_id = return_id;
    transaction.procedure = procedure;
    transaction.callback_stack = task.vp_callback_stack;
    transaction.original_stack = task.vp_stack;
    transaction.task16 = task.task16;
    transaction.parameter_bytes = parameter_bytes;
    if (parameter_bytes != 0u)
        memcpy(transaction.parameters, parameters, parameter_bytes);

    /* The app-owned dispatch is the sole synchronous machine-resume owner.
     * This adapter supplies only a copied source-shaped callback record. */
    dispatch_result = session_dispatch_control(owner,
        MVDM_WOW_CALLBACK_CONTROL_OPERATION, &transaction, -1);
    if (dispatch_result != 0 ||
        !mvdm_wow_task_frame_restore_stack(transaction.original_stack)) return 0;
    *return_value_out = (uint32_t)transaction.ax |
        ((uint32_t)transaction.dx << 16);
    return 1;
}
