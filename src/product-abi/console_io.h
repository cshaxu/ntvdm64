#ifndef NTVDM_CONSOLE_IO_H
#define NTVDM_CONSOLE_IO_H
#include <stdint.h>
#include "console_video.h"

/* Direct worker/frontend protocol; no pointers or native resource identities.
 * Local x86 peers are authenticated by the BaseSrv channel attachment. */
#define CONSOLE_IO_VERSION 10u
#define CONSOLE_IO_DATA_BYTES 16384u
typedef struct console_io_cell {
    uint16_t character,attribute;
} console_io_cell;
typedef struct console_io_input {
    uint32_t type,flags,control,repeat,virtual_key,scan,character,key_down;
    int32_t x,y;
    uint32_t buttons,menu,focus;
} console_io_input;
#define CONSOLE_IO_INPUT_CAPACITY (CONSOLE_IO_DATA_BYTES/sizeof(console_io_input))
enum console_io_operation {
    CONSOLE_IO_WRITE = 1,
    CONSOLE_IO_SCREEN_INFO,
    CONSOLE_IO_CURSOR_POSITION,
    CONSOLE_IO_CURSOR_INFO,
    CONSOLE_IO_FILL_CHARACTER,
    CONSOLE_IO_FILL_ATTRIBUTE,
    CONSOLE_IO_SCROLL,
    CONSOLE_IO_ATTRIBUTE,
    CONSOLE_IO_GET_MODE,
    CONSOLE_IO_SET_MODE,
    CONSOLE_IO_BARRIER,
    CONSOLE_IO_READ_CELLS_A,
    CONSOLE_IO_READ_CELLS_W,
    CONSOLE_IO_WRITE_CELLS_A,
    CONSOLE_IO_WRITE_CELLS_W,
    CONSOLE_IO_BUFFER_SIZE,
    CONSOLE_IO_WINDOW_RECT,
    CONSOLE_IO_READ_INPUT,
    CONSOLE_IO_PEEK_INPUT,
    CONSOLE_IO_GET_CURSOR_INFO,
    CONSOLE_IO_PREPEND_KEYS,
    CONSOLE_IO_CODE_PAGE,
    CONSOLE_IO_WINDOW_QUERY,
    CONSOLE_IO_GET_TITLE_A,
    CONSOLE_IO_SET_TITLE_A,
    CONSOLE_IO_GET_POINTER,
    CONSOLE_IO_SET_POINTER,
    CONSOLE_IO_GET_POINTER_CLIP,
    CONSOLE_IO_SET_POINTER_CLIP,
    CONSOLE_IO_CURRENT_FONT,
    CONSOLE_IO_FONT_SIZE,
    CONSOLE_IO_VIDEO_BEGIN,
    CONSOLE_IO_VIDEO_DATA,
    CONSOLE_IO_VIDEO_TEXT,
    CONSOLE_IO_GET_DISPLAY_MODE,
    CONSOLE_IO_SET_DISPLAY_MODE,
    CONSOLE_IO_KEYBOARD_LAYOUT
};
enum console_io_window_query {
    CONSOLE_WINDOW_ICONIC=1,
    CONSOLE_WINDOW_CLIENT_RECT,
    CONSOLE_WINDOW_CLIENT_TO_SCREEN
};
typedef struct console_io_state {
    int32_t x,y,width,height,left,top,right,bottom,max_width,max_height;
    int32_t clip_left,clip_top,clip_right,clip_bottom;
    uint32_t attribute,character,count,mode,cursor_size,cursor_visible;
    uint32_t input,has_clip;
} console_io_state;
typedef struct console_io_request {
    uint32_t version,generation,sequence,operation,bytes;
    console_io_state state;
    uint8_t data[CONSOLE_IO_DATA_BYTES];
} console_io_request;
typedef struct console_io_reply {
    uint32_t version,generation,sequence,result,error;
    console_io_state state;
    uint32_t bytes;
    uint8_t data[CONSOLE_IO_DATA_BYTES];
} console_io_reply;
#endif
