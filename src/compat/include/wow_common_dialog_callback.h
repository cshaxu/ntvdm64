#ifndef WOW_COMMON_DIALOG_CALLBACK_H
#define WOW_COMMON_DIALOG_CALLBACK_H
#include <windows.h>

typedef UINT (WINAPI *wow_common_dialog_proc)(HWND, UINT, WPARAM, LPARAM);
typedef struct wow_common_dialog_callbacks {
    wow_common_dialog_proc dialog;
    wow_common_dialog_proc print_setup;
    wow_common_dialog_proc find_replace;
} wow_common_dialog_callbacks;

/* Immutable same-image provider; bind before original DLL process attach.
 * The provider and its code remain alive through original detach cleanup. */
BOOL WINAPI wow_common_dialog_bind(const wow_common_dialog_callbacks *provider);
#endif
