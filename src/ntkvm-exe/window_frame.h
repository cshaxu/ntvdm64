#ifndef FRONTEND_WINDOW_FRAME_H
#define FRONTEND_WINDOW_FRAME_H
#include "native_console_host.h"
#include "console_video.h"
#include "lib/kvm-window/frame_interface.h"

enum { FRONTEND_NATIVE_CELL_WIDTH=8, FRONTEND_NATIVE_CELL_HEIGHT=14 };

/* Snapshot ownership stays with the caller for this synchronous conversion.
 * Native raster frames are TEXT for display policy, not DOS graphics.
 * Errors invalidate the destination; no incomplete frame may be published. */
DWORD frontend_window_native_frame(const run16_native_frame_info *,
    const CHAR_INFO *, SIZE_T count, kvm_window_frame *);
DWORD frontend_window_native_frame_pointer(const run16_native_frame_info *,
    const CHAR_INFO *, SIZE_T count, const POINT *, kvm_window_frame *);
DWORD frontend_window_dos_frame(const run16_console_video *, kvm_window_frame *);
#endif
