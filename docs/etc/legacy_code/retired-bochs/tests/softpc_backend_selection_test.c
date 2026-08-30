#include "adapter-bochs/machine_facade.h"
#include "adapter-bochs/machine_lifecycle.h"
#include "mvdm_command_registers.h"
#include "mvdm_vdm_stack.h"
#include "session/session.h"

static uint32_t resume_calls;

uint32_t getPE(void)
{
    return 1u;
}

int machine_facade_copy_real_mode_frame(struct machine_facade_real_mode_frame *frame)
{
    (void)frame;
    return MACHINE_FACADE_REAL_MODE_FRAME_REJECTED_MODE;
}

int machine_facade_commit_real_mode_frame(
    const struct machine_facade_real_mode_frame *expected,
    const struct machine_facade_real_mode_frame *candidate)
{
    (void)expected;
    (void)candidate;
    return MACHINE_FACADE_REAL_MODE_FRAME_REJECTED_MODE;
}

int adapter_bochs_machine_lifecycle_resume(
    const struct adapter_bochs_machine_resume_request *request,
    struct adapter_bochs_machine_resume_outcome *outcome)
{
    if (request == 0 || outcome == 0) return 0;
    ++resume_calls;
    outcome->version = ADAPTER_BOCHS_MACHINE_RESUME_VERSION;
    outcome->struct_bytes = sizeof(*outcome);
    outcome->status = ADAPTER_BOCHS_MACHINE_RESUME_BUDGET;
    outcome->reserved0 = 0u;
    return 1;
}

int main(void)
{
    session bochs;
    session softpc;

    session_initialize(&bochs, 501u);
    if (!session_select_machine_backend(&bochs, SESSION_MACHINE_BACKEND_BOCHS) ||
        !session_activate(&bochs) || !session_thread_bind(&bochs) ||
        !session_set_mechanical_resume_budget(&bochs, 7u))
        return 1;
    host_simulate();
    if (resume_calls != 1u || session_mechanical_resume_status(&bochs) !=
            ADAPTER_BOCHS_MACHINE_RESUME_BUDGET ||
        !session_thread_unbind(&bochs) || !session_dispose(&bochs))
        return 2;

    session_initialize(&softpc, 502u);
    if (!session_select_machine_backend(&softpc, SESSION_MACHINE_BACKEND_SOFTPC) ||
        !session_activate(&softpc) || !session_thread_bind(&softpc))
        return 3;
    host_simulate();
    if (resume_calls != 1u || session_mechanical_resume_status(&softpc) !=
            SESSION_MECHANICAL_STATUS_BACKEND_UNAVAILABLE ||
        !session_thread_unbind(&softpc) || !session_dispose(&softpc))
        return 4;
    return 0;
}
