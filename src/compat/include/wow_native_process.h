#ifndef WOW_NATIVE_PROCESS_H
#define WOW_NATIVE_PROCESS_H
#include <windows.h>
DWORD WINAPI wow_user_msg_wait(DWORD,const HANDLE *,BOOL,DWORD,DWORD);
/* Definition-only cleanup interposition retains the original callers.
 * PTD is opaque at this forced compile boundary; preserve its /Gz ABI. */
VOID W32FreeTask(PVOID);
/* OpenNT USER's MsgWait leaves/reenters its WOW scheduler. Modern USER
 * cannot see this worker's taskman state; bind that same call boundary. */
#define MsgWaitForMultipleObjects wow_user_msg_wait
#endif
