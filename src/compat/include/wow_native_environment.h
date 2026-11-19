#ifndef COMPAT_WOW_NATIVE_ENVIRONMENT_H
#define COMPAT_WOW_NATIVE_ENVIRONMENT_H
#include <windows.h>
BOOL WINAPI wow_edit_native_ensure_gui_thread(void);
PVOID WINAPI wow_edit_native_highest_user_address(void);
#endif
