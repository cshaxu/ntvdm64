#ifndef FRONTEND_WINDOW_FRAME_H
#define FRONTEND_WINDOW_FRAME_H
#include "console_video.h"
#include "lib/kvm-window/frame_interface.h"

enum { FRONTEND_NATIVE_CELL_WIDTH=8, FRONTEND_NATIVE_CELL_HEIGHT=14 };

/* Both workers use this same copied-frame decoder. */
DWORD frontend_window_dos_frame(const run16_console_video *, kvm_window_frame *);
DWORD frontend_window_pointer(kvm_window_frame *,const POINT *);
#endif
