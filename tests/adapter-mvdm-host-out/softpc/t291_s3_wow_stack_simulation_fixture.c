#include "mvdm_vdm_stack.h"

#include <string.h>

#include "adapter-bochs/machine_facade.h"

static struct machine_facade_real_mode_frame current_frame;
static uint32_t prepare_calls;
static uint32_t loop_calls;

uint32_t getPE(void)
{
    return 0u;
}

int machine_facade_copy_real_mode_frame(
    struct machine_facade_real_mode_frame *frame)
{
    if (frame == NULL) return MACHINE_FACADE_REAL_MODE_FRAME_REJECTED_INPUT;
    *frame = current_frame;
    return MACHINE_FACADE_REAL_MODE_FRAME_OK;
}

int machine_facade_commit_real_mode_frame(
    const struct machine_facade_real_mode_frame *expected,
    const struct machine_facade_real_mode_frame *candidate)
{
    if (expected == NULL || candidate == NULL ||
        memcmp(expected, &current_frame, sizeof(current_frame)) != 0)
        return MACHINE_FACADE_REAL_MODE_FRAME_REJECTED_STALE;
    current_frame = *candidate;
    return MACHINE_FACADE_REAL_MODE_FRAME_OK;
}

int machine_facade_prepare_cpu_resume(void)
{
    ++prepare_calls;
    return 1;
}

void machine_facade_cpu_loop(void)
{
    ++loop_calls;
}

int main(void)
{
    uint32_t stack;

    memset(&current_frame, 0, sizeof(current_frame));
    current_frame.abi_version = MACHINE_FACADE_REAL_MODE_FRAME_VERSION;
    current_frame.struct_bytes = sizeof(current_frame);
    current_frame.execution_mode = MACHINE_FACADE_EXECUTION_MODE_REAL;
    current_frame.ss = 0x1234u;
    current_frame.sp = 0x0080u;
    if (!mvdm_vdm_stack_copy(&stack) || stack != 0x12340080u ||
        !mvdm_vdm_stack_set(0x5678005au) || current_frame.ss != 0x5678u ||
        current_frame.sp != 0x005au || !mvdm_vdm_stack_copy(&stack) ||
        stack != 0x5678005au) return 1;
    host_simulate();
    return prepare_calls == 1u && loop_calls == 1u ? 0 : 2;
}
