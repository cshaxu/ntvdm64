/* Package-private declarations for original BaseClient VDM commands.
 * Shapes retained from OpenNT public/sdk/inc/ntcsrdll.h. No CSR wire/runtime
 * implementation is imported. The selected binding must validate captures.
 */
#ifndef OPENNT_BASE_CLIENT_BINDING_H
#define OPENNT_BASE_CLIENT_BINDING_H
#include <ntlpcapi.h>
#include <ntcsrmsg.h>
#include <winss.h>
#include "opennt-host/base/win32/inc/basemsg.h"
#include "opennt-host/base/win32/inc/basevdm.h"
#include <vdmapi.h>
#include <base_event.h>
BOOL BaseCheckForVDM(HANDLE, LPDWORD);
/* Bind the admitted CSR-shaped boundary explicitly. Modern ntdll.lib also
 * exports CsrClientCallServer; it must never silently satisfy this dependency.
 * Original callers, parameters and NTAPI convention remain unchanged. */
#define CsrClientCallServer OpenNtBaseClientCallServer
NTSTATUS NTAPI CsrClientCallServer(PCSR_API_MSG, PCSR_CAPTURE_HEADER, CSR_API_NUMBER, ULONG);
PCSR_CAPTURE_HEADER NTAPI CsrAllocateCaptureBuffer(ULONG, ULONG, ULONG);
VOID NTAPI CsrFreeCaptureBuffer(PCSR_CAPTURE_HEADER);
ULONG NTAPI CsrAllocateMessagePointer(PCSR_CAPTURE_HEADER, ULONG, PVOID *);
BOOL BaseUpdateVDMEntry(ULONG, HANDLE *, ULONG, ULONG);
BOOL BaseCheckVDM(ULONG, PCWCH, PCWCH, PCWCH, ANSI_STRING *, PBASE_API_MSG, PULONG, DWORD, LPSTARTUPINFOW);
/* Original process shape is the finite support carrier, never the host PEB. */
#define PPEB POPENNT_SUPPORT_PEB
#ifndef STARTF_HASSHELLDATA
#define STARTF_HASSHELLDATA 0x00000400
#endif
#ifndef STATUS_INVALID_IMAGE_NOT_MZ
#define STATUS_INVALID_IMAGE_NOT_MZ ((NTSTATUS)0xC000012FL)
#endif
BOOL OpenNtBaseIsConsoleHandle(HANDLE);
/* Original conroute.h's low-two-bit test described NT4 pseudo handles;
 * modern Console identity is queried, not inferred from a numeric value. */
#undef CONSOLE_HANDLE
#define CONSOLE_HANDLE(value) OpenNtBaseIsConsoleHandle(value)
#undef swprintf
/* Original NT4 call has Microsoft's pre-ISO signature, without a count. */
#define swprintf _swprintf
#endif
