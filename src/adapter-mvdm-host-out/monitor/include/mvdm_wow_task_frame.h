#ifndef MVDM_WOW_TASK_FRAME_H
#define MVDM_WOW_TASK_FRAME_H

#include <stdint.h>

/* Copied, source-shaped subset of the original current WOW TD. It contains
 * only callback-reached numeric fields; it never carries the original TD's
 * native thread, window, GDI or process pointers. */
#define MVDM_WOW_TASK_FRAME_CALLBACK_INITIALIZED 0x00000001u

typedef struct mvdm_wow_task_frame {
    uint32_t vp_stack;
    uint32_t vp_callback_stack;
    uint32_t flags;
    uint16_t task16;
    uint16_t reserved0;
} mvdm_wow_task_frame;

int mvdm_wow_task_frame_bind(uint32_t vp_stack, uint32_t vp_callback_stack,
    uint32_t flags, uint16_t task16);
int mvdm_wow_task_frame_copy(mvdm_wow_task_frame *frame_out);
int mvdm_wow_task_frame_begin_callback(uint32_t frame_bytes,
    mvdm_wow_task_frame *frame_out);
int mvdm_wow_task_frame_restore_stack(uint32_t vp_stack);
void mvdm_wow_task_frame_clear(void);

#endif
