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
static int phase;
#define REQUIRE(x) do { phase=__LINE__; if (!(x)) { fprintf(stderr,"FAIL line %d\n",__LINE__); return 1; } } while(0)
int main(int argc,char **argv)
{
    broker_rpc_scope scope;
    WCHAR endpoint[128];
    RPC_WSTR text=NULL;
    RPC_BINDING_HANDLE binding=NULL;
    VDM_CONNECTION connection=NULL,duplicate=NULL;
    ULONG generation=0,next=0,first=99;
    ULONG receipt=0;
    HANDLE reader=NULL,writer=NULL;
    DWORD available;
    char temporary[MAX_PATH];
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
        REQUIRE(GetTempFileNameA(".","vdm",0,temporary));
        writer=CreateFileA(temporary,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
            NULL,OPEN_EXISTING,FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE,NULL);
        REQUIRE(writer!=INVALID_HANDLE_VALUE);
        REQUIRE(!Client_AttachFile(binding,connection,process,generation,1,writer,&receipt) && receipt);
        REQUIRE(!Client_RevokeStream(binding,connection,process,generation,receipt));
        REQUIRE(CloseHandle(writer));writer=NULL;
        REQUIRE(CreatePipe(&reader,&writer,NULL,0));
        REQUIRE(Client_AttachPipe(binding,connection,process,0,2,writer,&receipt)==ERROR_ACCESS_DENIED && !receipt);
        REQUIRE(Client_AttachPipe(binding,connection,process,generation,4,writer,&receipt)==ERROR_INVALID_PARAMETER && !receipt);
        REQUIRE(!Client_AttachPipe(binding,connection,process,generation,2,writer,&receipt) && receipt);
        REQUIRE(CloseHandle(writer)); writer=NULL;
        REQUIRE(PeekNamedPipe(reader,NULL,0,NULL,&available,NULL) && available==0);
        REQUIRE(Client_RevokeStream(binding,connection,process,0,receipt)==ERROR_ACCESS_DENIED);
        REQUIRE(PeekNamedPipe(reader,NULL,0,NULL,&available,NULL));
        REQUIRE(!Client_RevokeStream(binding,connection,process,generation,receipt));
        REQUIRE(!Client_RevokeStream(binding,connection,process,generation,receipt));
        REQUIRE(!PeekNamedPipe(reader,NULL,0,NULL,&available,NULL) && GetLastError()==ERROR_BROKEN_PIPE);
        REQUIRE(CloseHandle(reader));reader=NULL;
        REQUIRE(CreatePipe(&reader,&writer,NULL,0));
        REQUIRE(!Client_AttachPipe(binding,connection,process,generation,2,writer,&receipt) && receipt);
        REQUIRE(CloseHandle(writer));writer=NULL;
        REQUIRE(Client_Disconnect(binding,process,0,&connection)==ERROR_ACCESS_DENIED && connection);
        REQUIRE(!Client_Disconnect(binding,process,generation,&connection) && !connection);
        REQUIRE(!PeekNamedPipe(reader,NULL,0,NULL,&available,NULL) && GetLastError()==ERROR_BROKEN_PIPE);
        REQUIRE(CloseHandle(reader));reader=NULL;
        REQUIRE(!Client_Connect(binding,process,&connection,&next) && next>generation);
        REQUIRE(!Client_First(binding,connection,process,next,&first) && first==0);
        REQUIRE(!Client_Disconnect(binding,process,next,&connection) && !connection);
    }
    RpcExcept(1) { fprintf(stderr,"RPC exception %lu at line %d\n",RpcExceptionCode(),phase); return 2; }
    RpcEndExcept
    RpcBindingFree(&binding); RpcStringFreeW(&text); CloseHandle(process);
    puts("PASS: formal basesrv registration, file/pipe receipts, revoke/disconnect closes pipe, role/generation rejection, original first-VDM and reconnect");
    return 0;
}
