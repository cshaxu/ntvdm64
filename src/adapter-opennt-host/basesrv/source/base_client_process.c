/* Modern capability binding for BaseCheckVDM's original CONSOLE_HANDLE
 * predicate (OpenNT public/internal/base/inc/conroute.h). No launch policy. */
#include <windows.h>
BOOL OpenNtBaseIsConsoleHandle(HANDLE handle)
{
    DWORD mode, saved=GetLastError();
    BOOL result=GetConsoleMode(handle,&mode);
    SetLastError(saved);
    return result;
}
