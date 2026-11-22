#ifndef WOW_EDIT_CONVERSION_BINDINGS_H
#define WOW_EDIT_CONVERSION_BINDINGS_H
#include <windows.h>

typedef LONG (WINAPI *wow_edit_original_proc)(HWND, UINT, WPARAM, LONG);
typedef struct wow_edit_conversion_provider {
    wow_edit_original_proc ansi;
    wow_edit_original_proc unicode;
    /* Original service and CPD values supplied with the original entries. */
    DWORD call_window_function, ansi_to_unicode_options, unicode_to_ansi_options;
} wow_edit_conversion_provider;
BOOL WINAPI wow_edit_conversion_bind(const wow_edit_conversion_provider *);

/* ADAPTER-WOW-060: exact original EDIT CsSendMessage call shape. This is
 * not a generic replacement for USER message sending or its other FNIDs. */
LONG WINAPI wow_edit_CsSendMessage(HWND, UINT, WPARAM, LPARAM,
    DWORD procedure, DWORD function, BOOL ansi);
#endif
