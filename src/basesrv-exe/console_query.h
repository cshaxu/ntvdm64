#ifndef APP_CONSOLE_QUERY_H
#define APP_CONSOLE_QUERY_H
#include <windows.h>
/* Caller owns/pins authenticated process handles for the whole call and
 * revalidates registration generations before using returned membership.
 * This operation never opens a process by PID. Output unchanged on failure. */
DWORD app_console_query(const WCHAR *helper,HANDLE caller,const HANDLE *candidates,
    DWORD count,HANDLE cancel,DWORD timeout,BYTE *members);
#endif
