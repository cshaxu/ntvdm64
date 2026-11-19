#ifndef COMPAT_WOW_DIALOG_SLOTS_H
#define COMPAT_WOW_DIALOG_SLOTS_H
#include <windows.h>
/* Public HWND slots; the caller owns window and task lifetime. */
LONG WINAPI wow_dialog_host_get(HWND, int, BOOL);
LONG WINAPI wow_dialog_host_set(HWND, int, LONG, BOOL);
#endif
