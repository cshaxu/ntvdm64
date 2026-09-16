/*
 * Source-shaped boundary for the original VDMREDIR remote RAP cohort.
 *
 * The original callers in vdmredir/vrremote.c and vrnetapi.c retain their
 * packet construction, status mapping and DOS-register failure paths.  Their
 * original providers live in the coupled RpcXlate/RxApi and XACTSRV products
 * and require private Lanman redirector FSCTL/RAP transport that modern
 * public Win32 does not expose.  Returning STATUS_NOT_IMPLEMENTED at that
 * original import boundary is therefore preferable to fabricating an SMB/RAP
 * client or silently treating unrelated NetAPI calls as equivalent.
 */

#include <nt.h>
#include <windows.h>
#include <lmerr.h>
#include <rxp.h>
#include <rxuser.h>
#include <xstypes.h>
#include <xsprocs.h>

NET_API_STATUS
RxpTransactSmb(
    IN LPTSTR UncServerName,
    IN LPTSTR TransportName,
    IN LPVOID SendParmPtr,
    IN DWORD SendParmSize,
    IN LPVOID SendDataPtr,
    IN DWORD SendDataSize,
    OUT LPVOID RetParmPtr,
    IN DWORD RetParmSize,
    OUT LPVOID RetDataPtr,
    IN OUT LPDWORD RetDataSize,
    IN BOOL NoPermissionRequired
    )
{
    UNREFERENCED_PARAMETER(UncServerName);
    UNREFERENCED_PARAMETER(TransportName);
    UNREFERENCED_PARAMETER(SendParmPtr);
    UNREFERENCED_PARAMETER(SendParmSize);
    UNREFERENCED_PARAMETER(SendDataPtr);
    UNREFERENCED_PARAMETER(SendDataSize);
    UNREFERENCED_PARAMETER(RetParmPtr);
    UNREFERENCED_PARAMETER(RetParmSize);
    UNREFERENCED_PARAMETER(RetDataPtr);
    UNREFERENCED_PARAMETER(RetDataSize);
    UNREFERENCED_PARAMETER(NoPermissionRequired);
    return ERROR_CALL_NOT_IMPLEMENTED;
}

NET_API_STATUS
RxNetUserPasswordSet(
    IN LPTSTR ServerName,
    IN LPTSTR UserName,
    IN LPTSTR OldPassword,
    IN LPTSTR NewPassword
    )
{
    UNREFERENCED_PARAMETER(ServerName);
    UNREFERENCED_PARAMETER(UserName);
    UNREFERENCED_PARAMETER(OldPassword);
    UNREFERENCED_PARAMETER(NewPassword);
    return ERROR_CALL_NOT_IMPLEMENTED;
}

NET_API_STATUS
GetLanmanSessionKey(
    IN LPWSTR ServerName,
    OUT LPBYTE SessionKey
    )
{
    UNREFERENCED_PARAMETER(ServerName);
    UNREFERENCED_PARAMETER(SessionKey);
    return ERROR_CALL_NOT_IMPLEMENTED;
}

static NTSTATUS
VdmRedirRemoteUnavailable(
    API_HANDLER_PARAMETERS
    )
{
    UNREFERENCED_PARAMETER(Header);
    UNREFERENCED_PARAMETER(Parameters);
    UNREFERENCED_PARAMETER(StructureDesc);
    UNREFERENCED_PARAMETER(AuxStructureDesc);
    return STATUS_NOT_IMPLEMENTED;
}

#define VDMREDIR_REMOTE_UNAVAILABLE(name) \
    NTSTATUS name(API_HANDLER_PARAMETERS) { return VdmRedirRemoteUnavailable(Header, Parameters, StructureDesc, AuxStructureDesc); }

VDMREDIR_REMOTE_UNAVAILABLE(XsNetMessageBufferSend)
VDMREDIR_REMOTE_UNAVAILABLE(XsNetServerEnum2)
VDMREDIR_REMOTE_UNAVAILABLE(XsNetServiceControl)
VDMREDIR_REMOTE_UNAVAILABLE(XsNetUseAdd)
VDMREDIR_REMOTE_UNAVAILABLE(XsNetUseDel)
VDMREDIR_REMOTE_UNAVAILABLE(XsNetUseEnum)
VDMREDIR_REMOTE_UNAVAILABLE(XsNetUseGetInfo)
VDMREDIR_REMOTE_UNAVAILABLE(XsNetWkstaGetInfo)
