#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma comment(linker, "/EXPORT:MouseInputHook=_MouseInputHook@12")
LRESULT CALLBACK MouseInputHook(int code, WPARAM w, LPARAM l) {return CallNextHookEx(NULL,code,w,l);}
