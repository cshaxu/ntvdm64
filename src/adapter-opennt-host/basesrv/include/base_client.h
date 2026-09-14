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
NTSTATUS NTAPI CsrClientCallServer(PCSR_API_MSG, PCSR_CAPTURE_HEADER, CSR_API_NUMBER, ULONG);
PCSR_CAPTURE_HEADER NTAPI CsrAllocateCaptureBuffer(ULONG, ULONG, ULONG);
VOID NTAPI CsrFreeCaptureBuffer(PCSR_CAPTURE_HEADER);
ULONG NTAPI CsrAllocateMessagePointer(PCSR_CAPTURE_HEADER, ULONG, PVOID *);
#endif
