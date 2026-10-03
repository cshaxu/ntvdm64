/* Real authenticated RPC client: observation never creates a VDM context. */
#include <windows.h>
#include <rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "service.h"
#include "ntsrv-exe/transport/rpc_security.h"
#include "common/protocol/version.h"
void *__RPC_USER midl_user_allocate(size_t n) { return malloc(n); }
void __RPC_USER midl_user_free(void *p) { free(p); }
int main(int argc,char **argv)
{
    broker_rpc_scope scope;
    WCHAR endpoint[128];RPC_WSTR text=NULL;RPC_BINDING_HANDLE binding=NULL;
    HANDLE process;
    VDM_CONNECTION connection=NULL;
    ULONG generation=0,protocol=0;
    unsigned char version[APP_VERSION_BYTES]=APP_VERSION,reply[APP_VERSION_BYTES];
    DWORD error=0,duration=argc>2?strtoul(argv[2],NULL,10):0;
    ULONGLONG deadline;
    if (argc<2 || !broker_rpc_capture_scope(&scope)) return 2;
    swprintf_s(endpoint,128,L"ntvdm-basesrv-%lu-%08lx-%08lx",scope.session,
        (ULONG)scope.logon.HighPart,scope.logon.LowPart);
    if (RpcStringBindingComposeW(NULL,(RPC_WSTR)L"ncalrpc",NULL,(RPC_WSTR)endpoint,NULL,&text) ||
        RpcBindingFromStringBindingW(text,&binding) ||
        RpcBindingSetAuthInfoW(binding,NULL,RPC_C_AUTHN_LEVEL_PKT_PRIVACY,RPC_C_AUTHN_WINNT,NULL,RPC_C_AUTHZ_NONE)) return 3;
    RpcStringFreeW(&text);
    process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,GetCurrentProcessId());
    if (!process) return 4;
    RpcTryExcept {
        if (!strcmp(argv[1],"observe")) {
            deadline=GetTickCount64()+duration;
            do {
                ULONG count=0;DTASKMGR_WORKER *entries=NULL;
                error=Client_TaskSnapshot(binding,process,APP_PROTOCOL_VERSION,version,&count,&entries);
                if (entries) midl_user_free(entries);
                if (error || count) { error=error?error:ERROR_INVALID_DATA;break; }
                puts("OBSERVED EMPTY");fflush(stdout);
                Sleep(200);
            } while (GetTickCount64()<deadline);
        } else {
            if (!strcmp(argv[1],"badpeer")) {
                CloseHandle(process);
                if (argc<4) return 5;
                process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,
                    strtoul(argv[3],NULL,10));
                if (!process) return 6;
            }
            error=Client_Connect(binding,process,APP_PROTOCOL_VERSION+(!strcmp(argv[1],"reject")),
                version,&protocol,reply,&connection,&generation);
            if (!strcmp(argv[1],"reject") || !strcmp(argv[1],"badpeer")) {
                DWORD expected=!strcmp(argv[1],"reject")?ERROR_REVISION_MISMATCH:RPC_S_ACCESS_DENIED;
                error=(error==expected && !connection)?0:ERROR_INVALID_DATA;
            } else if (!error) {
                HANDLE denied_event=NULL;
                ULONG started=99;
                DWORD denied=Client_WowStarted(binding,connection,process,generation,1);
                if (denied!=ERROR_ACCESS_DENIED) { error=ERROR_INVALID_DATA; __leave; }
                denied=Client_WowStartup(binding,connection,process,generation,1,&denied_event,&started);
                if (denied_event) CloseHandle(denied_event);
                if (denied!=ERROR_ACCESS_DENIED || denied_event || started) {
                    error=ERROR_INVALID_DATA; __leave;
                }
                puts("WOW STARTUP UNAUTHORIZED REJECTED");fflush(stdout);
                puts("CONNECTED");fflush(stdout);
                Sleep(duration);
                if (!strcmp(argv[1],"crash")) TerminateProcess(GetCurrentProcess(),73);
                error=Client_Disconnect(binding,process,generation,&connection);
                puts("DISCONNECTED");fflush(stdout);
            }
        }
    } RpcExcept(1) { error=RpcExceptionCode(); } RpcEndExcept
    if (!strcmp(argv[1],"observe") && (error==RPC_S_SERVER_UNAVAILABLE || error==RPC_S_CALL_FAILED || error==RPC_S_CALL_FAILED_DNE)) {
        puts("OBSERVED BROKER EXIT");error=0;
    }
    CloseHandle(process);RpcBindingFree(&binding);
    if (error) fprintf(stderr,"FAIL %s: %lu\n",argv[1],error);
    return (int)error;
}
