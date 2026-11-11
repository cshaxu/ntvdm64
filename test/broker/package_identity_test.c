/* Runs from a disposable staged fixture directory. The harness stamps the
 * executable and ten component copies, then exercises missing/mixed identities. */
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include "ntsrv-exe/transport/rpc_security.h"

int wmain(int argc,WCHAR **argv)
{
    broker_rpc_scope scope;
    WCHAR endpoint[128];
    if (argc!=2)return 2;
    if (!broker_rpc_capture_scope(&scope)) {
        DWORD error=GetLastError();
        wprintf(L"identity-error=%lu\n",error);
        return !_wcsicmp(argv[1],L"reject") ? 0 : 3;
    }
    if (_wcsicmp(argv[1],L"accept"))return 4;
    if (swprintf_s(endpoint,128,BROKER_RPC_ENDPOINT_FORMAT,scope.session,
            (ULONG)scope.logon.HighPart,scope.logon.LowPart,scope.package_id)<0)return 5;
    wprintf(L"identity=%ls endpoint=%ls\n",scope.package_id,endpoint);
    return 0;
}
