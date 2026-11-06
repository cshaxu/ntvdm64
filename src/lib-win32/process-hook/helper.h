#ifndef PROCESS_HOOK_HELPER_H
#define PROCESS_HOOK_HELPER_H
#include <windows.h>
typedef BOOL (WINAPI *process_hook_create_w)(LPCWSTR,LPWSTR,LPSECURITY_ATTRIBUTES,
    LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCWSTR,LPSTARTUPINFOW,LPPROCESS_INFORMATION);
typedef DWORD (*process_hook_helper_apply)(HANDLE,const void *,DWORD,PCWSTR,void *);
/* Payload is 1..65536 bytes and must have even length for the following WCHAR
 * path. Odd lengths fail before allocation or helper creation. Child is borrowed;
 * its creator must abort the uncommitted child on installation failure. */
DWORD process_hook_install_helper(HANDLE,PCWSTR,const void *,DWORD,process_hook_create_w);
DWORD process_hook_run_helper(process_hook_helper_apply,void *);
#endif
