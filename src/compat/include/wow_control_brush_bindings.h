#ifndef WOW_CONTROL_BRUSH_BINDINGS_H
#define WOW_CONTROL_BRUSH_BINDINGS_H
#include "wow_user_client_view_layout.h"
#include "wow_user_borrow_scope.h"

HBRUSH WINAPI wow_control_brush(HWND, HDC, UINT);
VOID WINAPI wow_control_brush_retain(PWND);
HBRUSH WINAPI wow_control_brush_color(PWND, PWND, HDC, UINT);

#ifdef WOW_ORIGINAL_CONTROL_BRUSH
/* The outer service owns an exception-safe borrow scope. Original ThreadLock
 * retains the exact selected recipient in that scope, not an NT4 server lock.
 * Release is deferred to the service's finally, including native exceptions.
 * No original code uses tlpwndSend after this sole ThreadUnlock. */
#define TL wow_user_borrow_scope
#define CheckLock(window) ((void)0)
#define ThreadLock(window, lock) ((void)(lock), wow_control_brush_retain(window))
#define ThreadUnlock(lock) ((void)(lock))
#define xxxGetControlColor wow_control_brush_color
#endif
#endif
