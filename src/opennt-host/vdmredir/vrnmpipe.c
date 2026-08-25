/* DIVERGENCE(HOST-DIV-015,HOST-DIV-016,HOST-DIV-020,HOST-DIV-023,
 * HOST-DIV-024,HOST-DIV-025,HOST-DIV-028): retain the original exported
 * synchronous-pipe ABI as thin boundaries.  The admitted but substantially
 * cropped original source is private overlay code, avoiding an over-threshold
 * mirror difference while leaving external callers on the original names. */
#define VrAddOpenNamedPipeInfo opennt_host_overlay_VrAddOpenNamedPipeInfo
#define VrRemoveOpenNamedPipeInfo opennt_host_overlay_VrRemoveOpenNamedPipeInfo
#define VrReadNamedPipe opennt_host_overlay_VrReadNamedPipe
#define VrWriteNamedPipe opennt_host_overlay_VrWriteNamedPipe
#define VrIsNamedPipeName opennt_host_overlay_VrIsNamedPipeName
#define VrIsNamedPipeHandle opennt_host_overlay_VrIsNamedPipeHandle
#define VrConvertLocalNtPipeName opennt_host_overlay_VrConvertLocalNtPipeName
#include "opennt-host-overlay/vdmredir/vrnmpipe.c"
#undef VrAddOpenNamedPipeInfo
#undef VrRemoveOpenNamedPipeInfo
#undef VrReadNamedPipe
#undef VrWriteNamedPipe
#undef VrIsNamedPipeName
#undef VrIsNamedPipeHandle
#undef VrConvertLocalNtPipeName

BOOL VrAddOpenNamedPipeInfo(HANDLE handle, LPSTR name) { return opennt_host_overlay_VrAddOpenNamedPipeInfo(handle, name); }
BOOL VrRemoveOpenNamedPipeInfo(HANDLE handle) { return opennt_host_overlay_VrRemoveOpenNamedPipeInfo(handle); }
BOOL VrReadNamedPipe(HANDLE handle, LPBYTE buffer, DWORD length, LPDWORD read, LPDWORD error) { return opennt_host_overlay_VrReadNamedPipe(handle, buffer, length, read, error); }
BOOL VrWriteNamedPipe(HANDLE handle, LPBYTE buffer, DWORD length, LPDWORD written) { return opennt_host_overlay_VrWriteNamedPipe(handle, buffer, length, written); }
BOOL VrIsNamedPipeName(LPSTR name) { return opennt_host_overlay_VrIsNamedPipeName(name); }
BOOL VrIsNamedPipeHandle(HANDLE handle) { return opennt_host_overlay_VrIsNamedPipeHandle(handle); }
LPSTR VrConvertLocalNtPipeName(LPSTR buffer, LPSTR name) { return opennt_host_overlay_VrConvertLocalNtPipeName(buffer, name); }
