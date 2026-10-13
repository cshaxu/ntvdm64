#ifndef WOW_NATIVE_PROCESS_H
#define WOW_NATIVE_PROCESS_H
#include <windows.h>
BOOL WINAPI wow_native_create_process_a(LPCSTR,LPSTR,LPSECURITY_ATTRIBUTES,
    LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCSTR,LPSTARTUPINFOA,LPPROCESS_INFORMATION);
DWORD WINAPI wow_user_msg_wait(DWORD,const HANDLE *,BOOL,DWORD,DWORD);
#ifdef WOW_NATIVE_PROCESS_BINDING
/* Forced only into original wkman.c, preserving WK32LoadModule32's existing
 * command construction, show state, wait and error mapping. */
#undef CreateProcess
#define CreateProcess wow_native_create_process_a
/* OpenNT USER's MsgWait leaves/reenters its WOW scheduler. Modern USER
 * cannot see this worker's taskman state; bind that same call boundary. */
#define MsgWaitForMultipleObjects wow_user_msg_wait
#endif
#endif
