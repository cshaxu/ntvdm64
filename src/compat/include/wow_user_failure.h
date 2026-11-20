#ifndef COMPAT_WOW_USER_FAILURE_H
#define COMPAT_WOW_USER_FAILURE_H
#include <windows.h>

/* Host diagnostic and terminal invariant outcome; no task or scheduler access. */
void WINAPI wow_user_retirement_trace(const char *, DWORD, DWORD);
__declspec(noreturn) void WINAPI wow_user_native_call_failed(const char *, DWORD);
#endif
