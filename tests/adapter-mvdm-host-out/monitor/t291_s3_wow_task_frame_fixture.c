#include "mvdm_wow_task_frame.h"

#include <string.h>

#include "session/session.h"

static int read_memory(void *context, uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    (void)context;
    (void)address;
    memset(bytes, 0, byte_count);
    return 1;
}

static int write_memory(void *context, uint32_t address, uint8_t const *bytes,
    uint32_t byte_count)
{
    (void)context;
    (void)address;
    (void)bytes;
    (void)byte_count;
    return 1;
}

int main(void)
{
    session instance;
    mvdm_wow_task_frame frame;

    session_initialize(&instance, 291u);
    if (!session_activate(&instance) ||
        !session_guest_memory_begin(&instance, NULL, read_memory, write_memory) ||
        !session_thread_bind(&instance)) return 1;
    if (mvdm_wow_task_frame_copy(&frame) ||
        !mvdm_wow_task_frame_bind(0x12340080u, 0x12340060u, 0u, 0x0042u) ||
        mvdm_wow_task_frame_bind(0x12340080u, 0x12340060u, 0u, 0x0043u) ||
        !mvdm_wow_task_frame_begin_callback(0x20u, &frame) ||
        frame.vp_callback_stack != 0x12340040u ||
        (frame.flags & MVDM_WOW_TASK_FRAME_CALLBACK_INITIALIZED) == 0u ||
        !mvdm_wow_task_frame_begin_callback(0x20u, &frame) ||
        frame.vp_callback_stack != 0x12340060u ||
        !mvdm_wow_task_frame_restore_stack(0x12340080u) ||
        !mvdm_wow_task_frame_copy(&frame) ||
        frame.vp_stack != 0x12340080u || frame.vp_callback_stack != 0x12340080u ||
        frame.task16 != 0x0042u) return 2;
    mvdm_wow_task_frame_clear();
    if (mvdm_wow_task_frame_copy(&frame)) return 3;
    if (!session_thread_unbind(&instance)) return 4;
    session_guest_memory_end(&instance);
    return session_dispose(&instance) ? 0 : 5;
}
