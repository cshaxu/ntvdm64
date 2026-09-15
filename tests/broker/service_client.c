/* Native client of the formal basesrv executable, no service policy. */
#include <windows.h>
#include <rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "service.h"
#include "broker/rpc_security.h"
#include "app/version.h"
void *__RPC_USER midl_user_allocate(size_t bytes) { return malloc(bytes); }
void __RPC_USER midl_user_free(void *value) { free(value); }
static int phase;
typedef struct stream_rpc_test {
    RPC_BINDING_HANDLE binding;VDM_CONNECTION connection;HANDLE process;
    ULONG generation;DWORD delivered,revoked,error;
} stream_rpc_test;
DWORD test_original_stream_rpc(HANDLE,HANDLE,void *,LONG (*)(void *,HANDLE,uint32_t *),LONG (*)(void *,uint32_t));
static LONG deliver_original_stream(void *context,HANDLE stream,uint32_t *receipt)
{
    stream_rpc_test *test=context;ULONG id=0;
    ++test->delivered;
    test->error=Client_AttachPipe(test->binding,test->connection,test->process,test->generation,2,stream,&id);
    if (test->error) return (LONG)0xc0000001L;
    *receipt=id;return 0;
}
static LONG revoke_original_stream(void *context,uint32_t receipt)
{
    stream_rpc_test *test=context;
    ++test->revoked;
    test->error=Client_RevokeStream(test->binding,test->connection,test->process,test->generation,receipt);
    return test->error ? (LONG)0xc0000001L : 0;
}
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
    unsigned char app_version[APP_VERSION_BYTES]=APP_VERSION,server_version[APP_VERSION_BYTES]={0};
    ULONG server_protocol=0;
    HANDLE reader=NULL,writer=NULL;
    DWORD available;
    char temporary[MAX_PATH];
    char receiverPid[32];HANDLE receiver;
    stream_rpc_test streamTest={0};
    HANDLE process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,GetCurrentProcessId());
    int abandon=argc==2 && !strcmp(argv[1],"--abandon");
    ULONG expectedFirst=argc==2 && !strcmp(argv[1],"--existing")?0:1;
    REQUIRE(process && broker_rpc_capture_scope(&scope));
    swprintf_s(endpoint,128,L"ntvdm-basesrv-%lu-%08lx-%08lx",scope.session,(ULONG)scope.logon.HighPart,scope.logon.LowPart);
    REQUIRE(!RpcStringBindingComposeW(NULL,(RPC_WSTR)L"ncalrpc",NULL,(RPC_WSTR)endpoint,NULL,&text));
    REQUIRE(!RpcBindingFromStringBindingW(text,&binding));
    REQUIRE(!RpcBindingSetAuthInfoW(binding,NULL,RPC_C_AUTHN_LEVEL_PKT_PRIVACY,RPC_C_AUTHN_WINNT,NULL,RPC_C_AUTHZ_NONE));
    RpcTryExcept {
        REQUIRE(Client_Connect(binding,process,APP_PROTOCOL_VERSION+1,app_version,
            &server_protocol,server_version,&connection,&generation)==ERROR_REVISION_MISMATCH && !connection && !generation);
        REQUIRE(server_protocol==APP_PROTOCOL_VERSION && !memcmp(server_version,app_version,sizeof(app_version)));
        app_version[0]='9';
        REQUIRE(Client_Connect(binding,process,APP_PROTOCOL_VERSION,app_version,
            &server_protocol,server_version,&connection,&generation)==ERROR_REVISION_MISMATCH && !connection && !generation);
        memset(app_version,'X',sizeof(app_version)); /* no terminator */
        REQUIRE(Client_Connect(binding,process,APP_PROTOCOL_VERSION,app_version,
            &server_protocol,server_version,&connection,&generation)==ERROR_REVISION_MISMATCH && !connection && !generation);
        memset(app_version,0,sizeof(app_version));memcpy(app_version,APP_VERSION,sizeof(APP_VERSION));
        REQUIRE(!Client_Connect(binding,process,APP_PROTOCOL_VERSION,app_version,
            &server_protocol,server_version,&connection,&generation) && connection && generation);
        REQUIRE(server_protocol==APP_PROTOCOL_VERSION && !memcmp(server_version,app_version,sizeof(app_version)));
        if (abandon) {
            /* Deliberately leave the context open and let process exit test
             * the real RPC rundown path, not explicit Disconnect. */
            puts("PASS: registered context intentionally abandoned at process exit");
            return 0;
        }
        REQUIRE(Client_Connect(binding,process,APP_PROTOCOL_VERSION,app_version,
            &server_protocol,server_version,&duplicate,&next)==ERROR_ALREADY_EXISTS && !duplicate && !next);
        REQUIRE(Client_First(binding,connection,process,0,&first)==ERROR_ACCESS_DENIED && first==0);
        REQUIRE(!Client_First(binding,connection,process,generation,&first) && first==expectedFirst);
        REQUIRE(!Client_First(binding,connection,process,generation,&first) && first==0);
        REQUIRE(GetEnvironmentVariableA("BASESRV_TEST_PID",receiverPid,sizeof(receiverPid)));
        receiver=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,strtoul(receiverPid,NULL,10));
        REQUIRE(receiver && WaitForSingleObject(receiver,0)==WAIT_TIMEOUT);
        REQUIRE(CreatePipe(&reader,&writer,NULL,0));
        streamTest.binding=binding;streamTest.connection=connection;streamTest.process=process;streamTest.generation=generation;
        REQUIRE(!test_original_stream_rpc(receiver,writer,&streamTest,deliver_original_stream,revoke_original_stream));
        REQUIRE(streamTest.delivered==1 && streamTest.revoked==2 && !streamTest.error);
        REQUIRE(CloseHandle(writer) && CloseHandle(receiver));writer=NULL;
        REQUIRE(!PeekNamedPipe(reader,NULL,0,NULL,&available,NULL) && GetLastError()==ERROR_BROKEN_PIPE);
        REQUIRE(CloseHandle(reader));reader=NULL;
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
        REQUIRE(!Client_Connect(binding,process,APP_PROTOCOL_VERSION,app_version,
            &server_protocol,server_version,&connection,&next) && next>generation);
        REQUIRE(!Client_First(binding,connection,process,next,&first) && first==0);
        REQUIRE(!Client_Disconnect(binding,process,next,&connection) && !connection);
    }
    RpcExcept(1) { fprintf(stderr,"RPC exception %lu at line %d\n",RpcExceptionCode(),phase); return 2; }
    RpcEndExcept
    RpcBindingFree(&binding); RpcStringFreeW(&text); CloseHandle(process);
    puts("PASS: formal basesrv registration, file/pipe receipts, revoke/disconnect closes pipe, role/generation rejection, original first-VDM and reconnect");
    return 0;
}
