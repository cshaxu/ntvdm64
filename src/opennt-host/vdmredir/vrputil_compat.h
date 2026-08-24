#ifndef NTDOS64_OPENNT_HOST_VDMREDIR_VRPUTIL_COMPAT_H
#define NTDOS64_OPENNT_HOST_VDMREDIR_VRPUTIL_COMPAT_H

/* OpenNT base/mvdm/vdmredir/vrputil.c compatibility surface. */
#include <windows.h>

WORD VrpMapLastError(VOID);
WORD VrpMapDosError(DWORD ErrorCode);
WORD VrpTranslateDosNetPath(LPSTR *InputString, LPSTR *OutputString);

#endif
