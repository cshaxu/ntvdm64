/* Modern local RPC authentication only. Original BaseSrv remains the VDM
 * policy owner; NT4 CSR port/process internals are not recreated here. */
#include "rpc_security.h"
#include <rpcasync.h>
#include <string.h>
#include <wchar.h>

/* This code may execute in an injected Hook DLL. The package identity belongs
 * to the module containing the RPC client, never to the native host EXE. */
static BOOL broker_package_identity(WCHAR output[BROKER_PACKAGE_ID_HEX_CHARS+1u])
{
    static const WCHAR hex[]=L"0123456789abcdef";
    static const WCHAR *const components[]={
        L"run16.exe",L"ntsrv.exe",L"ntcon.exe",L"ntvdm.exe",L"ntvwm.exe",
        L"ntmon.exe",L"WOW32.DLL",L"VDMREDIR.DLL",L"nthook32.dll",L"nthook64.dll"
    };
    HMODULE module=NULL;
    HRSRC resource;
    HGLOBAL loaded;
    const unsigned char *bytes;
    WCHAR path[32768],*separator;
    unsigned i,nonzero=0;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCWSTR)(ULONG_PTR)&broker_rpc_capture_scope,&module)) return FALSE;
    resource=FindResourceW(module,BROKER_PACKAGE_RESOURCE,MAKEINTRESOURCEW(10));
    if (!resource) { SetLastError(ERROR_INVALID_DATA);return FALSE; }
    if (SizeofResource(module,resource)!=16u) { SetLastError(ERROR_INVALID_DATA);return FALSE; }
    loaded=LoadResource(module,resource);
    bytes=loaded ? (const unsigned char *)LockResource(loaded) : NULL;
    if (!bytes) { SetLastError(ERROR_INVALID_DATA);return FALSE; }
    for (i=0;i<16u;++i) {
        nonzero|=bytes[i];output[2u*i]=hex[bytes[i]>>4];output[2u*i+1u]=hex[bytes[i]&15u];
    }
    output[BROKER_PACKAGE_ID_HEX_CHARS]=0;
    if (!nonzero) { SetLastError(ERROR_INVALID_DATA);return FALSE; }
    /* Reject a mixed or incomplete package before it binds to any server.
     * DATAFILE loading reads resources only; no component code is executed. */
    {
        DWORD length=GetModuleFileNameW(module,path,(DWORD)(sizeof(path)/sizeof(path[0])));
        if (!length)return FALSE;
        if (length>=sizeof(path)/sizeof(path[0])) { SetLastError(ERROR_INSUFFICIENT_BUFFER);return FALSE; }
    }
    separator=wcsrchr(path,L'\\');
    if (!separator) { SetLastError(ERROR_BAD_PATHNAME);return FALSE; }
    for (i=0;i<sizeof(components)/sizeof(components[0]);++i) {
        HMODULE image;
        HRSRC item;
        HGLOBAL data;
        const unsigned char *identity;
        size_t used=(size_t)(separator+1-path);
        if (used+wcslen(components[i])+1u>sizeof(path)/sizeof(path[0])) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);return FALSE;
        }
        wcscpy_s(separator+1,sizeof(path)/sizeof(path[0])-used,components[i]);
        image=LoadLibraryExW(path,NULL,LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
        if (!image) return FALSE;
        item=FindResourceW(image,BROKER_PACKAGE_RESOURCE,MAKEINTRESOURCEW(10));
        data=item && SizeofResource(image,item)==16u ? LoadResource(image,item) : NULL;
        identity=data ? (const unsigned char *)LockResource(data) : NULL;
        if (!identity || memcmp(identity,bytes,16u)) {
            FreeLibrary(image);SetLastError(ERROR_REVISION_MISMATCH);return FALSE;
        }
        FreeLibrary(image);
    }
    return TRUE;
}

RPC_STATUS broker_rpc_peer_process(const broker_rpc_scope *scope,
    RPC_BINDING_HANDLE binding, HANDLE process, DWORD *pid)
{
    RPC_CALL_ATTRIBUTES_V2_W call={0};
    DWORD actual;
    RPC_STATUS status;
    if (!pid) return RPC_S_INVALID_ARG;
    *pid=0;
    status=broker_rpc_authorize(scope,binding);
    if (status!=RPC_S_OK) return status;
    call.Version=2;
    call.Flags=RPC_QUERY_CLIENT_PID | RPC_QUERY_IS_CLIENT_LOCAL;
    status=RpcServerInqCallAttributesW(binding,&call);
    if (status!=RPC_S_OK) return status;
    actual=GetProcessId(process);
    if (call.IsClientLocal!=rcclLocal || !actual ||
        (ULONG_PTR)call.ClientPID!=actual || WaitForSingleObject(process,0)!=WAIT_TIMEOUT)
        return RPC_S_ACCESS_DENIED;
    *pid=actual;
    return RPC_S_OK;
}

static BOOL token_scope(HANDLE token, broker_rpc_scope *scope)
{
    TOKEN_STATISTICS stats;
    DWORD bytes;
    if (!GetTokenInformation(token,TokenStatistics,&stats,sizeof(stats),&bytes) ||
        !GetTokenInformation(token,TokenSessionId,&scope->session,sizeof(scope->session),&bytes)) return FALSE;
    scope->logon=stats.AuthenticationId;
    scope->valid=TRUE;
    return TRUE;
}

BOOL broker_rpc_capture_scope(broker_rpc_scope *scope)
{
    HANDLE token;
    BOOL result;
    DWORD error;
    if (!scope) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    ZeroMemory(scope,sizeof(*scope));
    if (!OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&token)) return FALSE;
    result=token_scope(token,scope);
    if (result && !broker_package_identity(scope->package_id)) {
        scope->valid=FALSE;
        result=FALSE;
    }
    error=GetLastError();
    CloseHandle(token);
    SetLastError(error);
    return result;
}

RPC_STATUS broker_rpc_authorize(const broker_rpc_scope *scope, RPC_BINDING_HANDLE binding)
{
    HANDLE token=NULL;
    broker_rpc_scope peer={0};
    ULONG level=0, service=0;
    RPC_STATUS result=RPC_S_ACCESS_DENIED;
    if (!scope || !scope->valid) return result;
    if (RpcBindingInqAuthClientW(binding,NULL,NULL,&level,&service,NULL)!=RPC_S_OK ||
        level!=RPC_C_AUTHN_LEVEL_PKT_PRIVACY || service!=RPC_C_AUTHN_WINNT) return result;
    if (RpcImpersonateClient(binding)!=RPC_S_OK) return result;
    if (OpenThreadToken(GetCurrentThread(),TOKEN_QUERY,TRUE,&token) &&
        token_scope(token,&peer) && peer.session==scope->session &&
        peer.logon.LowPart==scope->logon.LowPart && peer.logon.HighPart==scope->logon.HighPart)
        result=RPC_S_OK;
    if (token) CloseHandle(token);
    if (RpcRevertToSelf()!=RPC_S_OK) {
        /* Never dispatch or return a pooled thread still impersonating a peer. */
        if (!RevertToSelf()) RaiseFailFastException(NULL,NULL,0);
        return RPC_S_ACCESS_DENIED;
    }
    return result;
}
