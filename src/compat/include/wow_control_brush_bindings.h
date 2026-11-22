#ifndef WOW_CONTROL_BRUSH_BINDINGS_H
#define WOW_CONTROL_BRUSH_BINDINGS_H
#include <windows.h>
struct tagWND;

typedef HBRUSH (WINAPI *wow_control_brush_provider)(struct tagWND *, HDC, UINT);
BOOL WINAPI wow_control_brush_bind(wow_control_brush_provider);
HBRUSH WINAPI wow_control_brush(HWND, HDC, UINT);
VOID WINAPI wow_control_brush_retain(HWND, struct tagWND *);
HBRUSH WINAPI wow_control_brush_color(HWND, HWND, HDC, UINT);
HBRUSH WINAPI wow_edit_NtUserGetControlColor(HWND, HWND, HDC, UINT);

#ifdef WOW_ORIGINAL_CONTROL_BRUSH
#include "wow_user_client_view_layout.h"
#include "wow_user_borrow_scope.h"
/* The outer service owns an exception-safe borrow scope. Original ThreadLock
 * retains the exact selected recipient in that scope, not an NT4 server lock.
 * Release is deferred to the service's finally, including native exceptions.
 * No original code uses tlpwndSend after this sole ThreadUnlock. */
#define TL wow_user_borrow_scope
#define CheckLock(window) ((void)0)
#define ThreadLock(window, lock) ((void)(lock), wow_control_brush_retain((window)->head.h, (window)))
#define ThreadUnlock(lock) ((void)(lock))
#define xxxGetControlColor(recipient, control, dc, message) \
    wow_control_brush_color((recipient)->head.h, (control)->head.h, (dc), (message))
#endif
#endif
