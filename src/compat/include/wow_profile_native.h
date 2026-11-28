#ifndef COMPAT_WOW_PROFILE_NATIVE_H
#define COMPAT_WOW_PROFILE_NATIVE_H
#include <windows.h>
#include <winternl.h>
HANDLE WINAPI wow_profile_open_key(PUNICODE_STRING, UINT, ACCESS_MASK, PDWORD);
#endif
