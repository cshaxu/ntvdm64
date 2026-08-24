#ifndef APP_OPENNT_DEM_OEM_FACADE_H
#define APP_OPENNT_DEM_OEM_FACADE_H

/* Same-shaped public-Win32 replacement for the OEM directory calls reached
 * by OpenNT DOS/DEM providers.  Provider sequencing and DOS error mapping
 * stay in the imported OpenNT body. */
#include <windows.h>

BOOL CreateDirectoryOem(LPSTR name, LPSECURITY_ATTRIBUTES security);
BOOL RemoveDirectoryOem(LPSTR name);
BOOL SetCurrentDirectoryOem(LPSTR name);

#endif
