#ifndef WOW_CONTROL_DRAW_STATE_H
#define WOW_CONTROL_DRAW_STATE_H
#include <windows.h>
#include "wow_button_resources.h"

/* OPENNT-HOST-116: resources only. The original ClientDrawState owns effects. */
typedef struct wow_control_draw_context {
    HDC gray_dc, bits_dc;
    HGDIOBJ original_gray_bitmap;
    HBITMAP owned_gray_bitmap;
    int gray_width, gray_height;
    CRITICAL_SECTION lock;
    BOOL lock_ready;
    wow_button_resources atlas;
} wow_control_draw_context;
extern __declspec(thread) wow_control_draw_context *wow_control_draw_current;
BOOL WINAPI wow_control_draw_state(HDC, HBRUSH, DRAWSTATEPROC,
    LPARAM, WPARAM, int, int, int, int, UINT);
BOOL WINAPI wow_control_icon_size(HICON, UINT, int *, int *);
BOOL WINAPI wow_control_atlas_blit(HDC, int, int, int, int, int, int, DWORD);
HGDIOBJ WINAPI wow_control_draw_select(HDC, HGDIOBJ);
#endif
