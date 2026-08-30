#include "mvdm_vdm_stack.h"

#include "mvdm_command_registers.h"

#include "adapter-bochs/machine_facade.h"
#include "adapter-bochs/machine_lifecycle.h"
#include "session/session.h"

int mvdm_vdm_stack_copy(uint32_t *vp_stack_out)
{
    struct machine_facade_real_mode_frame frame;

    if (vp_stack_out == NULL || getPE() != 0u ||
        machine_facade_copy_real_mode_frame(&frame) !=
            MACHINE_FACADE_REAL_MODE_FRAME_OK) return 0;
    *vp_stack_out = ((uint32_t)frame.ss << 16) | frame.sp;
    return 1;
}

int mvdm_vdm_stack_set(uint32_t vp_stack)
{
    struct machine_facade_real_mode_frame expected;
    struct machine_facade_real_mode_frame candidate;

    /* The original SETVDMSTACK macro is a 16:16 monitor operation.  Keep the
     * exact numeric split, but commit both fields atomically through the
     * selector-blind machine record instead of mutating Bochs fields. */
    if (getPE() != 0u ||
        machine_facade_copy_real_mode_frame(&expected) !=
            MACHINE_FACADE_REAL_MODE_FRAME_OK) return 0;
    candidate = expected;
    candidate.ss = (uint16_t)(vp_stack >> 16);
    candidate.sp = (uint16_t)vp_stack;
    return machine_facade_commit_real_mode_frame(&expected, &candidate) ==
        MACHINE_FACADE_REAL_MODE_FRAME_OK;
}

void host_simulate(void)
{
    session *instance = session_thread_current();
    struct adapter_bochs_machine_resume_request request;
    struct adapter_bochs_machine_resume_outcome outcome;

    /* This preserves the reached historical call shape.  adapter-bochs owns
     * the actual CPU loop and its typed stop condition; no MVDM meaning is
     * introduced into the machine layer.  The session retains only the copied
     * mechanical result, never a Bochs object or service outcome. */
    if (instance == NULL) return;
    if (session_machine_backend(instance) != SESSION_MACHINE_BACKEND_BOCHS) {
        /* S3 deliberately has no runnable original SoftPC composition.  The
         * selected SoftPC branch must not execute the Bochs facade as an
         * implicit fallback; S4 owns its source-shaped create/run binding. */
        session_record_mechanical_resume_status(instance,
            SESSION_MECHANICAL_STATUS_BACKEND_UNAVAILABLE);
        return;
    }
    request.version = ADAPTER_BOCHS_MACHINE_RESUME_VERSION;
    request.struct_bytes = sizeof(request);
    request.tick_budget = session_mechanical_resume_budget(instance);
    if (!adapter_bochs_machine_lifecycle_resume(&request, &outcome)) {
        session_record_mechanical_resume_status(instance,
            ADAPTER_BOCHS_MACHINE_RESUME_REJECTED_INPUT);
        return;
    }
    session_record_mechanical_resume_status(instance, outcome.status);
}
