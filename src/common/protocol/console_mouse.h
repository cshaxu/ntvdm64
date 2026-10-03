#ifndef NTVDM_CONSOLE_MOUSE_H
#define NTVDM_CONSOLE_MOUSE_H
#include <stdint.h>

/* Private copied input between authenticated frontend and DOS worker. This
 * tag is never a Windows Console INPUT_RECORD event for a native process. */
#define CONSOLE_INPUT_RELATIVE_MOUSE 0x8001u
/* Backend-owned logical pointer. Unlike the DOS geometry notification above,
 * this carries modifiers, not a frontend-selected coordinate extent. Never
 * deliver this private record to a native Console application. */
#define CONSOLE_INPUT_POINTER 0x8002u
typedef struct console_pointer_input {
    int32_t dx,dy;
    uint32_t control;
    uint16_t buttons,action;
} console_pointer_input;
typedef char console_pointer_payload_size[(sizeof(console_pointer_input)==16) ? 1 : -1];
enum console_mouse_action {
    CONSOLE_MOUSE_ENTER=1,
    CONSOLE_MOUSE_MOVE,
    CONSOLE_MOUSE_LEAVE,
    /* Native text pointer coordinates in content pixels, not a delta. */
    CONSOLE_MOUSE_POSITION
};
typedef struct console_mouse_input {
    int32_t dx,dy;
    uint16_t width,height;
    uint16_t buttons,action;
} console_mouse_input;
typedef char console_mouse_payload_size[(sizeof(console_mouse_input)==16) ? 1 : -1];

static __inline int console_mouse_input_valid(const console_mouse_input *input)
{
    if(!input || input->buttons>3)return 0;
    if(input->action==CONSOLE_MOUSE_LEAVE)
        return !input->dx && !input->dy && !input->buttons && !input->width && !input->height;
    if(!input->width || !input->height)return 0;
    if(input->action==CONSOLE_MOUSE_ENTER)
        return !input->dx && !input->dy && !input->buttons;
    return input->action==CONSOLE_MOUSE_MOVE;
}
#endif
