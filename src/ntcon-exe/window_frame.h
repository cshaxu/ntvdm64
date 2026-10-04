#ifndef FRONTEND_WINDOW_FRAME_H
#define FRONTEND_WINDOW_FRAME_H
#include "console_video.h"
#include "lib/kvm-window/frame_interface.h"

/* Decode copied TEXT_FRAME or DIB data, independently of worker type.
 * TEXT_CONFIGURATION updates metadata; it is not a standalone Window frame. */
DWORD frontend_window_decode_frame(const frontend_video *, kvm_window_frame *);
#endif
