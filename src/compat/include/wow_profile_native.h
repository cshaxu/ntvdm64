#ifndef COMPAT_WOW_PROFILE_NATIVE_H
#define COMPAT_WOW_PROFILE_NATIVE_H
#include <windows.h>
HANDLE WINAPI wow_profile_open_key(UINT, ACCESS_MASK);
#endif
