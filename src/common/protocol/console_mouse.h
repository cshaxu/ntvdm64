#ifndef NTVDM_CONSOLE_MOUSE_H
#define NTVDM_CONSOLE_MOUSE_H
#include <stdint.h>
#include "compat/include/mvdm_mouse_record.h"

/* Worker-neutral Window sample: relative content pixels, source geometry and
 * Windows modifier bits. Console input remains ordinary absolute MOUSE_EVENT.
 * Workers interpret this copied record locally; never write it to Windows. */
#define CONSOLE_INPUT_FRAME_MOUSE 0x8003u
typedef struct console_frame_mouse_input {
    int32_t dx,dy;
    uint16_t width,height,control;
    uint8_t buttons,action;
} console_frame_mouse_input;
typedef char console_frame_mouse_payload_size[(sizeof(console_frame_mouse_input)==16) ? 1 : -1];
static __inline int console_frame_mouse_input_valid(const console_frame_mouse_input *input)
{
    console_mouse_input geometry;
    if(!input || (input->control&~0x1ffu))return 0;
    geometry.dx=input->dx;geometry.dy=input->dy;
    geometry.width=input->width;geometry.height=input->height;
    geometry.buttons=input->buttons;geometry.action=input->action;
    return console_mouse_input_valid(&geometry);
}
#endif
