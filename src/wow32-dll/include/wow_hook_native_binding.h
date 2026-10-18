#ifndef WOW_HOOK_NATIVE_BINDING_H
#define WOW_HOOK_NATIVE_BINDING_H
#include <windows.h>
HHOOK WINAPI wow_native_SetWindowsHookExA(int, HOOKPROC, HINSTANCE, DWORD);
#endif
