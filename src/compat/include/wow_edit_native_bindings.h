#ifndef WOW_EDIT_NATIVE_BINDINGS_H
#define WOW_EDIT_NATIVE_BINDINGS_H
#include "wow_native_gui_thread.h"
#include "compat/include/wow_native_controls.h"
struct _LARGE_STRING;
BOOL WINAPI wow_edit_native_NtUserDefSetText(HWND, const struct _LARGE_STRING *);
#endif
