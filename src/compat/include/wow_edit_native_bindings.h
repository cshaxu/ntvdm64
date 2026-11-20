#ifndef WOW_EDIT_NATIVE_BINDINGS_H
#define WOW_EDIT_NATIVE_BINDINGS_H
#include <nt.h>
#include "wow_native_gui_thread.h"
#define WOW_ORIGINAL_LARGE_STRING_LAYOUT
#include "compat/include/opennt_user_multipass.h"
#undef WOW_ORIGINAL_LARGE_STRING_LAYOUT
#include "non-mvdm/public/internal/windows/inc/wingdip.h"

#include "compat/include/wow_native_controls.h"
BOOL WINAPI wow_edit_native_NtUserDefSetText(HWND, const LARGE_STRING *);
#endif
