#ifndef NTVDM_CONSOLE_CLIENT_H
#define NTVDM_CONSOLE_CLIENT_H
#include <windows.h>
#include "ntvdm-exe/session/session.h"
#include "compat/include/console_video.h"
#include "compat/include/console_graphics.h"
#include "compat/include/console_services.h"
DWORD ntvdm_console_client_begin(session *);
void ntvdm_console_client_end(session *);
HANDLE WINAPI MvdmCreateFileA(LPCSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE);
HANDLE WINAPI MvdmCreateFileW(LPCWSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE);
DWORD WINAPI MvdmGetFileType(HANDLE);
BOOL WINAPI GetConsoleKeyboardLayoutNameA(LPSTR);
BOOL ntvdm_console_inherit_launch_capabilities(session *,HANDLE *,HANDLE *);
BOOL WINAPI MvdmWriteConsoleA(HANDLE,const VOID *,DWORD,LPDWORD,LPVOID);
#endif
