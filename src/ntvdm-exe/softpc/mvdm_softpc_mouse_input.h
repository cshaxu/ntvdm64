#ifndef MVDM_SOFTPC_MOUSE_INPUT_H
#define MVDM_SOFTPC_MOUSE_INPUT_H
#include <stdint.h>

/* Relative-input carrier only. The caller holds original host_ica_lock;
 * original nt_mouse.c owns coordinates, guest callbacks and IRQ scheduling.
 * Recovered from reference T423 presentation/mouse_delivery.c, with button
 * transitions delayed until the final piece of a split motion sample. */
#define MVDM_MOUSE_INPUT_CAPACITY 256u
typedef struct mvdm_mouse_input_sample {
    int32_t dx,dy;
    uint32_t buttons;
    uint32_t action;
} mvdm_mouse_input_sample;
typedef struct mvdm_mouse_input {
    mvdm_mouse_input_sample samples[MVDM_MOUSE_INPUT_CAPACITY];
    uint32_t head,count,delivered_buttons;
} mvdm_mouse_input;
/* Movement pressure follows the original 16-record threshold: add relative
 * counters at the same-button tail, never across button/ENTER/LEAVE edges.
 * Reject before mutation; the transport owner must surface backpressure. */
int mvdm_mouse_input_push(mvdm_mouse_input *,const mvdm_mouse_input_sample *);
int mvdm_mouse_input_take(mvdm_mouse_input *,mvdm_mouse_input_sample *);
/* Only original IRQ cancellation/disposal may discard accepted samples. */
void mvdm_mouse_input_clear(mvdm_mouse_input *);
#endif
