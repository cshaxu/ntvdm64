#ifndef RUN16_NATIVE_CONSOLE_HOST_H
#define RUN16_NATIVE_CONSOLE_HOST_H
#include "native_console_capture.h"
#include <stdint.h>

/* Private root/helper channel, not BaseSrv scheduling or the DOS I/O ABI.
 * The root supplies connected pipe capabilities to its exact created helper.
 * Raw handle values in LAUNCH replies are helper-local and must be duplicated
 * through that retained helper process before RELEASE drops the export copy.
 * CreateProcess success hands off execution; no launcher-managed suspend phase. */
#define RUN16_NATIVE_HOST_VERSION 8u
#define RUN16_NATIVE_HOST_CELLS 4096u
#define RUN16_NATIVE_HOST_INPUTS 256u
enum run16_native_host_operation {
    RUN16_NATIVE_LAUNCH=1, RUN16_NATIVE_RELEASE, RUN16_NATIVE_FRAME_BEGIN,
    RUN16_NATIVE_FRAME_READ, RUN16_NATIVE_FRAME_END, RUN16_NATIVE_INPUT,
    RUN16_NATIVE_STOP, RUN16_NATIVE_SCREEN_APPLY, RUN16_NATIVE_CELLS_WRITE,
    RUN16_NATIVE_INPUT_READ, RUN16_NATIVE_CONTROL, RUN16_NATIVE_GEOMETRY,
    RUN16_NATIVE_MEMBERS
};
typedef struct run16_native_host_request {
    uint32_t version,operation,bytes,offset,count;
} run16_native_host_request;
typedef struct run16_native_host_reply {
    uint32_t version,status,bytes,count;
    uint64_t process,thread;
    SMALL_RECT region;
} run16_native_host_reply;
typedef struct run16_native_frame_info {
    CONSOLE_SCREEN_BUFFER_INFOEX screen;
    CONSOLE_CURSOR_INFO cursor;
    DWORD output_mode,input_codepage,output_codepage;
} run16_native_frame_info;

/* Root-to-hidden seeding only. Font geometry limits the hidden Console's
 * viewport too; it must not substitute its default for the visible owner's.
 * Captured native output never changes the user's visible Console font. */
typedef struct run16_native_screen_seed {
    run16_native_frame_info frame;
    CONSOLE_FONT_INFOEX font;
} run16_native_screen_seed;

typedef struct run16_native_geometry {
    COORD size;
    SMALL_RECT window;
    CONSOLE_FONT_INFOEX font;
} run16_native_geometry;

#include "native_launch.h"

DWORD run16_native_console_host(void);
#endif
