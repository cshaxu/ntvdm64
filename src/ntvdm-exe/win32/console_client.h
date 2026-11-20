#ifndef NTVDM_CONSOLE_CLIENT_H
#define NTVDM_CONSOLE_CLIENT_H
#include <windows.h>
#include "ntvdm-exe/session/session.h"
#include "compat/include/console_video.h"
#include "compat/include/console_graphics.h"
#include "compat/include/console_services.h"
DWORD ntvdm_console_client_begin(session *);
void ntvdm_console_client_end(session *);
/* 0 unrelated, 1 input, 2 output; local identities, never UI resources. */
DWORD ntvdm_console_handle_kind(HANDLE);
HANDLE WINAPI MvdmCreateFileA(LPCSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE);
HANDLE WINAPI MvdmCreateFileW(LPCWSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE);
DWORD WINAPI MvdmGetFileType(HANDLE);
BOOL ntvdm_console_set_active(BOOL);
BOOL WINAPI GetConsoleKeyboardLayoutNameA(LPSTR);
BOOL ntvdm_console_inherit_launch_capabilities(HANDLE *,HANDLE *);
BOOL WINAPI MvdmWriteConsoleA(HANDLE,const VOID *,DWORD,LPDWORD,LPVOID);
BOOL ntvdm_console_read_input_tagged(HANDLE,PINPUT_RECORD,uint64_t *,DWORD,LPDWORD,BOOL,const uint64_t *,DWORD);
#endif
