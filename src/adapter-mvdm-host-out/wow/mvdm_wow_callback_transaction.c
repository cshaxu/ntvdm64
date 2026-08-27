#include "mvdm_wow_callback_transaction.h"

#include <string.h>

#include "adapter-mvdm-host-out/monitor/include/mvdm_wow_task_frame.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_wow_pointer_scope.h"
#include "session/session.h"

static uint16_t mvdm_wow_read_u16(const uint8_t *bytes)
{
    return (uint16_t)((uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8));
}

int mvdm_wow_callback_transaction_invoke(uint32_t return_id,
    uint32_t procedure, const uint8_t *parameters, uint16_t parameter_bytes,
    uint32_t *return_value_out)
{
    mvdm_wow_task_frame task;
    mvdm_wow_callback_transaction transaction;
    session *owner = session_thread_current();
    uint8_t *source_frame;
    mvdm_wow_callback_guest_frame *callback_frame;
    uint16_t local_bp;
    int32_t dispatch_result;
    int result = 0;

    if (return_value_out != NULL) *return_value_out = 0u;
    if (owner == NULL || return_value_out == NULL || parameter_bytes >
        MVDM_WOW_CALLBACK_PARAMETER_MAXIMUM ||
        (parameter_bytes != 0u && parameters == NULL) ||
        !mvdm_wow_task_frame_copy(&task) ||
        !mvdm_wow_pointer_scope_begin()) return 0;
    source_frame = (uint8_t *)mvdm_wow_pointer_scope_acquire(
        task.vp_stack, 6u, MVDM_WOW_POINTER_ACCESS_READ);
    if (source_frame == NULL || mvdm_wow_read_u16(source_frame) != task.task16)
        goto done;
    local_bp = mvdm_wow_read_u16(source_frame + 4u);
    if (
        !mvdm_wow_pointer_scope_release(source_frame, 0) ||
        !mvdm_wow_task_frame_begin_callback(
            (uint32_t)sizeof(mvdm_wow_callback_guest_frame), &task)) goto done;
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

    callback_frame = (mvdm_wow_callback_guest_frame *)
        mvdm_wow_pointer_scope_acquire(task.vp_callback_stack,
            (uint32_t)sizeof(*callback_frame), MVDM_WOW_POINTER_ACCESS_WRITE);
    if (callback_frame == NULL) goto restore;
    callback_frame->w_tdb = task.task16;
    callback_frame->w_return_id = (uint16_t)return_id;
    callback_frame->w_local_bp = local_bp;
    if (parameter_bytes != 0u)
        memcpy(callback_frame->parm16, parameters, parameter_bytes);
    callback_frame->vpfn_proc = procedure;
    callback_frame->vp_stack = task.vp_stack;
    callback_frame->w_ax = (uint16_t)(task.vp_stack >> 16);
    if (return_id == 56u && parameter_bytes >= 2u)
        callback_frame->w_gen_use1 = mvdm_wow_read_u16(parameters);
    transaction.guest_frame = *callback_frame;
    if (!mvdm_wow_pointer_scope_release(callback_frame, 1)) goto restore;

    /* The app-owned dispatch is the sole synchronous machine-resume owner.
     * This adapter supplies only a copied source-shaped callback record. */
    dispatch_result = session_dispatch_control(owner,
        MVDM_WOW_CALLBACK_CONTROL_OPERATION, &transaction, -1);
    if (dispatch_result != 0) goto restore;
    callback_frame = (mvdm_wow_callback_guest_frame *)
        mvdm_wow_pointer_scope_acquire(task.vp_callback_stack,
            (uint32_t)sizeof(*callback_frame), MVDM_WOW_POINTER_ACCESS_WRITE);
    if (callback_frame == NULL) goto restore;
    callback_frame->w_ax = transaction.ax;
    callback_frame->w_dx = transaction.dx;
    if (!mvdm_wow_pointer_scope_release(callback_frame, 1)) goto restore;
    callback_frame = (mvdm_wow_callback_guest_frame *)
        mvdm_wow_pointer_scope_acquire(task.vp_callback_stack,
            (uint32_t)sizeof(*callback_frame), MVDM_WOW_POINTER_ACCESS_READ);
    if (callback_frame == NULL) goto restore;
    transaction.ax = callback_frame->w_ax;
    transaction.dx = callback_frame->w_dx;
    if (!mvdm_wow_pointer_scope_release(callback_frame, 0)) goto restore;
    *return_value_out = (uint32_t)transaction.ax |
        ((uint32_t)transaction.dx << 16);
    result = 1;

restore:
    if (!mvdm_wow_task_frame_restore_stack(transaction.original_stack)) result = 0;
done:
    if (!mvdm_wow_pointer_scope_end()) result = 0;
    return result;
}
