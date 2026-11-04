/* Modern capability binding for BaseCheckVDM's original CONSOLE_HANDLE
 * predicate (OpenNT public/internal/base/inc/conroute.h).  Retain the
 * original low-bit pseudo-handle identity as well as recognizing a modern
 * Console object at the adapter resource boundary. */
#include <windows.h>
BOOL OpenNtBaseIsConsoleHandle(HANDLE handle)
{
    enum { opennt_console_handle_signature = 0x00000003u };
    DWORD mode, saved=GetLastError();
    BOOL result=((ULONG_PTR)handle & opennt_console_handle_signature) ==
        opennt_console_handle_signature || GetConsoleMode(handle,&mode);
    SetLastError(saved);
    return result;
}
