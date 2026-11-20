/* BaseSrv-private native declarations; original owners are OpenNT
 * public/sdk/inc/{ntlpcapi,ntcsrsrv,ntexapi,ntseapi,ntrtl,nturtl,ntldr,
 * ntobapi,ntdef}.h. No CSR transport or server policy is implemented here.
 * The process binding supplies the authenticated request thread; callers
 * must never synthesize it from an untrusted wire PID. */
#ifndef OPENNT_BASE_SERVER_BINDING_H
#define OPENNT_BASE_SERVER_BINDING_H
#include <nt.h>
#include <ntlpcapi.h>
#include <ntcsrsrv.h>
#include "compat/include/base_server_native.h"
NTSTATUS _UserTestTokenForInteractive(HANDLE, PLUID);
BOOL BaseSrvDOSWorkerWaitPending(HANDLE ConsoleHandle, PBOOL ConsoleRecordExists);
#endif
