#ifndef COMPAT_CONSOLE_ENDPOINT_H
#define COMPAT_CONSOLE_ENDPOINT_H

#include "console_transport.h"
#include "input_readiness.h"

/* A single ordered request stream and lock, shared with the product owner. */
typedef struct compat_console_endpoint {
    ntcon_worker_client channel;
    CRITICAL_SECTION lock;
    console_io_request request;
    HANDLE input_identity, output_identity;
    BYTE request_tile[CONSOLE_IO_TILE_BYTES];
    CRITICAL_SECTION palette_lock;
    PALETTEENTRY text_palette[16];
    BOOL text_palette_valid;
    struct worker_base_publication *publisher;
    struct ntvdm_console_graphics *graphics;
    compat_input_readiness *input_readiness;
} compat_console_endpoint;

compat_console_endpoint *compat_console_output_endpoint(HANDLE output);
compat_console_endpoint *compat_console_input_endpoint(HANDLE input);
DWORD compat_console_send_video(compat_console_endpoint *endpoint, const console_video_description *description,
    const void *pixels);
DWORD compat_console_send_video_copy(void *endpoint, const void *copy, SIZE_T bytes);

#endif
