#include "mvdm_vdm_stack.h"

#include "mvdm_command_registers.h"

#include "adapter-bochs/machine_facade.h"

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
    /* This preserves the reached historical call shape.  adapter-bochs owns
     * the actual CPU loop and its typed stop condition; no MVDM meaning is
     * introduced into the machine layer. */
    if (machine_facade_prepare_cpu_resume()) machine_facade_cpu_loop();
}
