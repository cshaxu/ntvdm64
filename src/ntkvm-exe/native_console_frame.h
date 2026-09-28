#ifndef NTKVM_NATIVE_CONSOLE_FRAME_H
#define NTKVM_NATIVE_CONSOLE_FRAME_H
#include "native_console_capture.h"
#include "native_launch.h"

/* Local presentation projection, not a helper protocol or canonical terminal
 * storage. Native terminal state remains Unicode/width/RGB in native_terminal. */
typedef struct run16_native_frame_info {
    CONSOLE_SCREEN_BUFFER_INFOEX screen;
    CONSOLE_CURSOR_INFO cursor;
    DWORD output_mode,input_codepage,output_codepage;
} run16_native_frame_info;
typedef struct run16_native_geometry {
    COORD size;
    SMALL_RECT window;
    CONSOLE_FONT_INFOEX font;
} run16_native_geometry;
#endif
