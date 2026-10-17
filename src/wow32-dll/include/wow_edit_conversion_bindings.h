#ifndef WOW_EDIT_CONVERSION_BINDINGS_H
#define WOW_EDIT_CONVERSION_BINDINGS_H
#include <windows.h>

/* ADAPTER-WOW-060: exact original EDIT CsSendMessage call shape. This is
 * not a generic replacement for USER message sending or its other FNIDs. */
LONG WINAPI wow_edit_CsSendMessage(HWND, UINT, WPARAM, LPARAM,
    DWORD procedure, DWORD function, BOOL ansi);
#endif
