#include "mvdm_wow_task_frame.h"

#include <string.h>

#include "session/session.h"

static __declspec(thread) mvdm_wow_task_frame current_task;
static __declspec(thread) uint32_t current_task_active;

static int mvdm_wow_task_frame_valid(void)
{
    return current_task_active == 1u && session_thread_current() != NULL;
}

int mvdm_wow_task_frame_bind(uint32_t vp_stack, uint32_t vp_callback_stack,
    uint32_t flags, uint16_t task16)
{
    if (session_thread_current() == NULL || current_task_active != 0u ||
        vp_stack == 0u || task16 == 0u ||
        (flags & ~MVDM_WOW_TASK_FRAME_CALLBACK_INITIALIZED) != 0u) return 0;
    memset(&current_task, 0, sizeof(current_task));
    current_task.vp_stack = vp_stack;
    current_task.vp_callback_stack = vp_callback_stack;
    current_task.flags = flags;
    current_task.task16 = task16;
    current_task_active = 1u;
    return 1;
}

int mvdm_wow_task_frame_copy(mvdm_wow_task_frame *frame_out)
{
    if (frame_out == NULL || !mvdm_wow_task_frame_valid()) return 0;
    *frame_out = current_task;
    return 1;
}

int mvdm_wow_task_frame_begin_callback(uint32_t frame_bytes,
    mvdm_wow_task_frame *frame_out)
{
    uint32_t base;

    if (frame_out == NULL || !mvdm_wow_task_frame_valid() ||
        frame_bytes == 0u) return 0;
    /* Preserve the original CallBack16 branch and word alignment. */
    base = (current_task.flags & MVDM_WOW_TASK_FRAME_CALLBACK_INITIALIZED) != 0u ?
        current_task.vp_stack : current_task.vp_callback_stack;
    if (base < frame_bytes) return 0;
    current_task.vp_callback_stack = (base - frame_bytes) & ~1u;
    if ((current_task.flags & MVDM_WOW_TASK_FRAME_CALLBACK_INITIALIZED) == 0u)
        current_task.flags |= MVDM_WOW_TASK_FRAME_CALLBACK_INITIALIZED;
    *frame_out = current_task;
    return 1;
}

int mvdm_wow_task_frame_restore_stack(uint32_t vp_stack)
{
    if (!mvdm_wow_task_frame_valid() || vp_stack == 0u) return 0;
    current_task.vp_stack = vp_stack;
    current_task.vp_callback_stack = vp_stack;
    return 1;
}

void mvdm_wow_task_frame_clear(void)
{
    memset(&current_task, 0, sizeof(current_task));
    current_task_active = 0u;
}
