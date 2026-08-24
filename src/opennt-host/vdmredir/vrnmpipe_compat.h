#ifndef NTDOS64_OPENNT_HOST_VDMREDIR_VRNMPIPE_COMPAT_H
#define NTDOS64_OPENNT_HOST_VDMREDIR_VRNMPIPE_COMPAT_H

/* OpenNT base/mvdm/vdmredir/vrnmpipe.c compatibility surface.
 *
 * This preserves the reached VDMREDIR spellings consumed by the directly
 * imported DEM files.  Its private record is source-owned VDMREDIR metadata,
 * not a guest/host handle mapper: external HANDLE identity remains private to
 * the existing session-owned handle manager. */
#include <windows.h>

BOOLEAN bx_ntvdm_vr_initialized_provider(void);
BOOLEAN bx_ntvdm_vr_initialize_provider(void);
VOID bx_ntvdm_vr_uninitialize_provider(void);
VOID VrTerminateNamedPipes(VOID);
BOOL VrAddOpenNamedPipeInfo(HANDLE Handle, LPSTR lpFileName);
BOOL VrRemoveOpenNamedPipeInfo(HANDLE Handle);
BOOL VrIsNamedPipeHandle(HANDLE Handle);
BOOL VrReadNamedPipe(HANDLE Handle, LPBYTE Buffer, DWORD Buflen,
    LPDWORD BytesRead, LPDWORD Error);
BOOL VrWriteNamedPipe(HANDLE Handle, LPBYTE Buffer, DWORD Buflen,
    LPDWORD BytesWritten);
BOOL VrIsNamedPipeName(LPSTR Name);
LPSTR VrConvertLocalNtPipeName(LPSTR Buffer, LPSTR Name);

#endif
