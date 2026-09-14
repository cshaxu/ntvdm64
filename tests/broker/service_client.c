/* Native client of the formal basesrv executable, no service policy. */
#include <windows.h>
#include <rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "service.h"
#include "broker/rpc_security.h"
void *__RPC_USER midl_user_allocate(size_t bytes) { return malloc(bytes); }
void __RPC_USER midl_user_free(void *value) { free(value); }
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr,"FAIL line %d\n",__LINE__); return 1; } } while(0)
int main(int argc,char **argv)
{
    broker_rpc_scope scope;
    WCHAR endpoint[128];
    RPC_WSTR text=NULL;
    RPC_BINDING_HANDLE binding=NULL;
    VDM_CONNECTION connection=NULL,duplicate=NULL;
    ULONG generation=0,next=0,first=99;
    HANDLE process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,GetCurrentProcessId());
    int abandon=argc==2 && !strcmp(argv[1],"--abandon");
    ULONG expectedFirst=argc==2 && !strcmp(argv[1],"--existing")?0:1;
    REQUIRE(process && broker_rpc_capture_scope(&scope));
    swprintf_s(endpoint,128,L"ntvdm-basesrv-%lu-%08lx-%08lx",scope.session,(ULONG)scope.logon.HighPart,scope.logon.LowPart);
    REQUIRE(!RpcStringBindingComposeW(NULL,(RPC_WSTR)L"ncalrpc",NULL,(RPC_WSTR)endpoint,NULL,&text));
    REQUIRE(!RpcBindingFromStringBindingW(text,&binding));
    REQUIRE(!RpcBindingSetAuthInfoW(binding,NULL,RPC_C_AUTHN_LEVEL_PKT_PRIVACY,RPC_C_AUTHN_WINNT,NULL,RPC_C_AUTHZ_NONE));
    RpcTryExcept {
        REQUIRE(!Client_Connect(binding,process,&connection,&generation) && connection && generation);
        if (abandon) {
            /* Deliberately leave the context open and let process exit test
             * the real RPC rundown path, not explicit Disconnect. */
            puts("PASS: registered context intentionally abandoned at process exit");
            return 0;
        }
        REQUIRE(Client_Connect(binding,process,&duplicate,&next)==ERROR_ALREADY_EXISTS && !duplicate && !next);
        REQUIRE(Client_First(binding,connection,process,0,&first)==ERROR_ACCESS_DENIED && first==0);
        REQUIRE(!Client_First(binding,connection,process,generation,&first) && first==expectedFirst);
        REQUIRE(!Client_First(binding,connection,process,generation,&first) && first==0);
        REQUIRE(Client_Disconnect(binding,process,0,&connection)==ERROR_ACCESS_DENIED && connection);
        REQUIRE(!Client_Disconnect(binding,process,generation,&connection) && !connection);
        REQUIRE(!Client_Connect(binding,process,&connection,&next) && next>generation);
        REQUIRE(!Client_First(binding,connection,process,next,&first) && first==0);
        REQUIRE(!Client_Disconnect(binding,process,next,&connection) && !connection);
    }
    RpcExcept(1) { fprintf(stderr,"RPC exception %lu\n",RpcExceptionCode()); return 2; }
    RpcEndExcept
    RpcBindingFree(&binding); RpcStringFreeW(&text); CloseHandle(process);
    puts("PASS: formal basesrv registration, duplicate/generation rejection, original first-VDM and reconnect");
    return 0;
}
