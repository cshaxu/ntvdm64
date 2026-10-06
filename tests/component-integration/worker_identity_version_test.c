/* Rename-package identity gate against the real authenticated NTSRV endpoint.
 * No worker/provider substitute and no desktop interaction. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc.h>
#include "service.h"
#include "common/protocol/version.h"
#include "ntsrv-exe/transport/rpc_security.h"

void *__RPC_USER midl_user_allocate(size_t bytes) { return malloc(bytes); }
void __RPC_USER midl_user_free(void *value) { free(value); }

int wmain(int argc, WCHAR **argv)
{
    broker_rpc_scope scope = {0};
    RPC_WSTR text = NULL;
    RPC_BINDING_HANDLE binding = NULL;
    PROCESS_INFORMATION broker = {0};
    STARTUPINFOW startup = {sizeof(startup)};
    VDM_CONNECTION connection = NULL;
    unsigned char app[APP_VERSION_BYTES] = APP_VERSION;
    unsigned char server[APP_VERSION_BYTES] = {0};
    WCHAR endpoint[128];
    HANDLE self = NULL;
    ULONG protocol = 0, generation = 0;
    DWORD error = 0, attempt;
    RPC_IF_ID interface_id;
    int failed = 1;
    if ((argc != 2 && argc != 3) || !broker_rpc_capture_scope(&scope)) return 2;
    if(RpcIfInqId(Client_vdm_service_v44_0_c_ifspec,&interface_id) ||
        interface_id.VersMajor!=APP_PROTOCOL_VERSION || interface_id.VersMinor!=0)return 2;
    swprintf_s(endpoint,128,L"ntvdm-basesrv-%lu-%08lx-%08lx",
        scope.session,(ULONG)scope.logon.HighPart,scope.logon.LowPart);
    if (RpcStringBindingComposeW(NULL,(RPC_WSTR)L"ncalrpc",NULL,
        (RPC_WSTR)endpoint,NULL,&text)) goto done;
    if (RpcBindingFromStringBindingW(text,&binding)) goto done;
    if (RpcBindingSetAuthInfoW(binding,NULL,RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
        RPC_C_AUTHN_WINNT,NULL,RPC_C_AUTHZ_NONE)) goto done;
    self = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|PROCESS_DUP_HANDLE|
        SYNCHRONIZE,FALSE,GetCurrentProcessId());
    if (!self) goto done;
    /* Refuse to test or terminate an already-running singleton. */
    RpcTryExcept {
        error=Client_Connect(binding,self,APP_PROTOCOL_VERSION,app,
            &protocol,server,&connection,&generation);
    } RpcExcept(1) { error=RpcExceptionCode(); } RpcEndExcept
    if (error!=RPC_S_SERVER_UNAVAILABLE) goto done;
    if (!CreateProcessW(argv[1],NULL,NULL,NULL,FALSE,CREATE_NO_WINDOW,
        NULL,NULL,&startup,&broker)) goto done;
    for (attempt=0;attempt<100;++attempt) {
        RpcTryExcept {
            error=Client_Connect(binding,self,APP_PROTOCOL_VERSION,app,
                &protocol,server,&connection,&generation);
        } RpcExcept(1) { error=RpcExceptionCode(); } RpcEndExcept
        if (!error) break;
        if (WaitForSingleObject(broker.hProcess,50)==WAIT_OBJECT_0) goto done;
    }
    if (error || !connection || !generation || protocol!=APP_PROTOCOL_VERSION ||
        memcmp(app,server,sizeof(app))) goto done;
    if (Client_Disconnect(binding,self,generation,&connection)) goto done;
    generation=0;
    memcpy(app,"0.0.427",sizeof("0.0.427"));
    RpcTryExcept {
        error=Client_Connect(binding,self,APP_PROTOCOL_VERSION,app,
            &protocol,server,&connection,&generation);
    } RpcExcept(1) { error=RpcExceptionCode(); } RpcEndExcept
    if (error!=ERROR_REVISION_MISMATCH || connection || generation) goto done;
    puts("PASS previous T application identity rejected by current real service");
    memset(app,0,sizeof(app));memcpy(app,APP_VERSION,sizeof(APP_VERSION));
    RpcTryExcept {
        error=Client_Connect(binding,self,APP_PROTOCOL_VERSION-1,app,
            &protocol,server,&connection,&generation);
    } RpcExcept(1) { error=RpcExceptionCode(); } RpcEndExcept
    if (error!=ERROR_REVISION_MISMATCH || connection || generation) goto done;
    puts("PASS wrong application protocol rejected; RPC major matches application protocol");
    if(argc==3) {
        PROCESS_INFORMATION legacy={0};
        WCHAR command[2*MAX_PATH];
        DWORD legacy_exit=0;
        swprintf_s(command,2*MAX_PATH,L"\"%ls\" cmd /c exit 0",argv[2]);
        if(!CreateProcessW(argv[2],command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&startup,&legacy))goto done;
        if(WaitForSingleObject(legacy.hProcess,15000)!=WAIT_OBJECT_0) {
            TerminateProcess(legacy.hProcess,ERROR_TIMEOUT);
            CloseHandle(legacy.hThread);CloseHandle(legacy.hProcess);goto done;
        }
        error=GetExitCodeProcess(legacy.hProcess,&legacy_exit) ? legacy_exit : GetLastError();
        CloseHandle(legacy.hThread);CloseHandle(legacy.hProcess);
        if(error!=ERROR_REVISION_MISMATCH)goto done;
        puts("PASS previous RPC interface client rejected before launching a target");
    }
    failed=0;
done:
    if (connection && binding) Client_Disconnect(binding,self,generation,&connection);
    if (self) CloseHandle(self);
    if (binding) RpcBindingFree(&binding);
    if (text) RpcStringFreeW(&text);
    if (broker.hProcess) {
        if (WaitForSingleObject(broker.hProcess,0)==WAIT_TIMEOUT)
            TerminateProcess(broker.hProcess,0);
        WaitForSingleObject(broker.hProcess,5000);
        CloseHandle(broker.hProcess);CloseHandle(broker.hThread);
    }
    if (failed) fprintf(stderr,"FAIL identity gate error=%lu\n",error);
    return failed;
}
