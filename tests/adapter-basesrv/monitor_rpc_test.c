/* Product-level management RPC fixture.  It starts the sibling BaseSrv,
 * exercises the same authenticated, connection-less wire calls used by
 * DTASKMGR, then terminates only that fixture-owned server. */
#include <windows.h>
#include <rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "service.h"
#include "ntsrv-exe/transport/rpc_security.h"
#include "interface/version.h"

#define CHECK(value) do { if (!(value)) { fprintf(stderr,"FAIL %d: %lu\n",__LINE__,(unsigned long)GetLastError()); return 1; } } while (0)

void *__RPC_USER MIDL_user_allocate(size_t bytes) { return malloc(bytes); }
void __RPC_USER MIDL_user_free(void *value) { free(value); }

static const unsigned char version[APP_VERSION_BYTES]=APP_VERSION;

static RPC_BINDING_HANDLE bind_server(const broker_rpc_scope *scope)
{
    WCHAR endpoint[128];
    RPC_WSTR text=NULL;
    RPC_BINDING_HANDLE binding=NULL;
    RPC_STATUS status;
    swprintf_s(endpoint,ARRAYSIZE(endpoint),L"ntvdm-basesrv-%lu-%08lx-%08lx",scope->session,
        (ULONG)scope->logon.HighPart,(ULONG)scope->logon.LowPart);
    status=RpcStringBindingComposeW(NULL,(RPC_WSTR)L"ncalrpc",NULL,(RPC_WSTR)endpoint,NULL,&text);
    if (status==RPC_S_OK) status=RpcBindingFromStringBindingW(text,&binding);
    if (text) RpcStringFreeW(&text);
    if (status==RPC_S_OK) status=RpcBindingSetAuthInfoW(binding,NULL,
        RPC_C_AUTHN_LEVEL_PKT_PRIVACY,RPC_C_AUTHN_WINNT,NULL,RPC_C_AUTHZ_NONE);
    if (status!=RPC_S_OK) { if (binding) RpcBindingFree(&binding); SetLastError(status); return NULL; }
    return binding;
}

/* Exercise the production inner launcher, not a substitute role detector.
 * This is native launch/result evidence; no hidden backend or DOS is claimed. */
static int inner_launcher(HANDLE frontend,HANDLE execution,DWORD flags,DWORD expected)
{
    STARTUPINFOW startup={sizeof(startup)};
    PROCESS_INFORMATION child={0};
    HANDLE inherited_frontend=NULL,inherited_execution=NULL;
    WCHAR executable[MAX_PATH],cmd[MAX_PATH],command[1024],text[32],*slash;
    DWORD result,wait;
    CHECK(DuplicateHandle(GetCurrentProcess(),frontend,GetCurrentProcess(),
        &inherited_frontend,SYNCHRONIZE,TRUE,0));
    CHECK(DuplicateHandle(GetCurrentProcess(),execution,GetCurrentProcess(),
        &inherited_execution,SYNCHRONIZE,TRUE,0));
    swprintf_s(text,32,L"%lx",(unsigned long)(ULONG_PTR)inherited_frontend);
    CHECK(SetEnvironmentVariableW(L"NTVDM_FRONTEND_CAPABILITY",text));
    swprintf_s(text,32,L"%lx",(unsigned long)(ULONG_PTR)inherited_execution);
    CHECK(SetEnvironmentVariableW(L"NTVDM_EXECUTION_CONSOLE",text));
    CHECK(GetModuleFileNameW(NULL,executable,MAX_PATH));
    slash=wcsrchr(executable,L'\\');CHECK(slash);
    CHECK(!wcscpy_s(slash+1,MAX_PATH-(size_t)(slash+1-executable),L"run16.exe"));
    CHECK(GetEnvironmentVariableW(L"COMSPEC",cmd,MAX_PATH));
    CHECK(swprintf_s(command,1024,L"\"%ls\" \"%ls\" /d /c exit /b 37",executable,cmd)>0);
    CHECK(CreateProcessW(executable,command,NULL,NULL,TRUE,flags,NULL,NULL,&startup,&child));
    CloseHandle(child.hThread);
    CloseHandle(inherited_frontend);CloseHandle(inherited_execution);
    CHECK(SetEnvironmentVariableW(L"NTVDM_FRONTEND_CAPABILITY",NULL));
    CHECK(SetEnvironmentVariableW(L"NTVDM_EXECUTION_CONSOLE",NULL));
    wait=WaitForSingleObject(child.hProcess,15000);
    if (wait!=WAIT_OBJECT_0) {
        TerminateProcess(child.hProcess,99);WaitForSingleObject(child.hProcess,5000);
        CloseHandle(child.hProcess);fputs("inner launcher timed out\n",stderr);return 1;
    }
    CHECK(GetExitCodeProcess(child.hProcess,&result));
    CloseHandle(child.hProcess);
    if (result!=expected) { fprintf(stderr,"inner result=%lu expected=%lu\n",result,expected);return 1; }
    return 0;
}

static int console_context_rpc(RPC_BINDING_HANDLE binding,HANDLE self)
{
    VDM_CONNECTION connection=NULL;
    HANDLE frontend=CreateEventW(NULL,TRUE,FALSE,NULL),execution=NULL;
    ULONG server_protocol=0,generation=0;
    unsigned char server_version[APP_VERSION_BYTES]={0};
    DWORD error;
#define RPC_CHECK(call,expected) do { \
    RpcTryExcept { error=(call); } \
    RpcExcept(1) { error=RpcExceptionCode(); } RpcEndExcept \
    CHECK(error==(expected)); \
} while (0)
    CHECK(frontend);
    RPC_CHECK(Client_Connect(binding,self,APP_PROTOCOL_VERSION,(unsigned char *)version,
        &server_protocol,server_version,&connection,&generation),ERROR_SUCCESS);
    CHECK(connection && generation && server_protocol==APP_PROTOCOL_VERSION &&
        !memcmp(server_version,version,sizeof(version)));
    RPC_CHECK(Client_RegisterFrontendRoot(binding,connection,self,generation,frontend),ERROR_SUCCESS);
    RPC_CHECK(Client_AcquireConsoleContext(binding,connection,self,generation+1,frontend,&execution),
        ERROR_ACCESS_DENIED);
    CHECK(!execution);
    RPC_CHECK(Client_AcquireConsoleContext(binding,connection,self,generation,frontend,&execution),
        ERROR_SUCCESS);
    CHECK(execution && WaitForSingleObject(execution,0)==WAIT_TIMEOUT);
    CHECK(!SetEvent(execution) && GetLastError()==ERROR_ACCESS_DENIED);
    {
        WCHAR name[96];
        HANDLE server,client,received=NULL,sender=NULL,context=NULL,io=NULL,probe=NULL;
        ULONG request=0;
        swprintf_s(name,96,L"\\\\.\\pipe\\ntvdm-worker-channel-rpc-%lu",GetCurrentProcessId());
        server=CreateNamedPipeW(name,PIPE_ACCESS_DUPLEX|FILE_FLAG_FIRST_PIPE_INSTANCE,
            PIPE_TYPE_BYTE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,1024,1024,0,NULL);
        CHECK(server!=INVALID_HANDLE_VALUE);
        client=CreateFileW(name,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
        CHECK(client!=INVALID_HANDLE_VALUE && (ConnectNamedPipe(server,NULL) || GetLastError()==ERROR_PIPE_CONNECTED));
        RPC_CHECK(Client_SubmitWorkerChannel(binding,connection,self,generation+1,frontend,server),ERROR_ACCESS_DENIED);
        RPC_CHECK(Client_SubmitWorkerChannel(binding,connection,self,generation,execution,server),ERROR_ACCESS_DENIED);
        RPC_CHECK(Client_SubmitWorkerChannel(binding,connection,self,generation,frontend,client),ERROR_INVALID_PARAMETER);
        /* Frontend identity alone cannot nominate an execution recipient. */
        RPC_CHECK(Client_SubmitWorkerChannel(binding,connection,self,generation,frontend,server),ERROR_NOT_READY);
        RPC_CHECK(Client_TakeWorkerChannel(binding,connection,self,generation,&received,&sender,&context,&io),ERROR_ACCESS_DENIED);
        CHECK(!received && !sender && !context && !io);
        RPC_CHECK(Client_FrontendRequest(binding,connection,self,generation,&request,&probe),ERROR_NOT_FOUND);
        CHECK(!probe && !request && WaitForSingleObject(frontend,0)==WAIT_TIMEOUT);
        CloseHandle(client);CloseHandle(server);
        puts("PASS actual RPC worker-only execution: frontend identity cannot submit/take without worker admission; typed pipe and generation rejection");
    }
    RPC_CHECK(Client_BindConsoleContext(binding,connection,self,generation,frontend),ERROR_ACCESS_DENIED);
    RPC_CHECK(Client_BindConsoleContext(binding,connection,self,generation+1,execution),ERROR_ACCESS_DENIED);
    RPC_CHECK(Client_BindConsoleContext(binding,connection,self,generation,execution),ERROR_SUCCESS);
    CHECK(inner_launcher(frontend,frontend,CREATE_NO_WINDOW,ERROR_ACCESS_DENIED)==0);
    /* This fixture registers only a root identity, not a frontend request
     * pump. Positive inner launch now belongs to the real root CLI tests. */
    puts("PASS: production inner run16 rejects frontend object used as execution capability");
    RPC_CHECK(Client_Disconnect(binding,self,generation,&connection),ERROR_SUCCESS);
    CHECK(!connection);
    RPC_CHECK(Client_Connect(binding,self,APP_PROTOCOL_VERSION,(unsigned char *)version,
        &server_protocol,server_version,&connection,&generation),ERROR_SUCCESS);
    RPC_CHECK(Client_BindConsoleContext(binding,connection,self,generation,execution),ERROR_ACCESS_DENIED);
    RPC_CHECK(Client_Disconnect(binding,self,generation,&connection),ERROR_SUCCESS);
    CloseHandle(execution);CloseHandle(frontend);
    puts("PASS: typed execution Console RPC, restricted rights, separate frontend authority, generation and root rundown");
#undef RPC_CHECK
    return 0;
}

int main(int argc,char **argv)
{
    broker_rpc_scope scope={0};
    RPC_BINDING_HANDLE binding;
    PROCESS_INFORMATION broker={0};
    STARTUPINFOW startup={sizeof(startup)};
    WCHAR path[MAX_PATH],*slash;
    HANDLE self;
    DWORD error=RPC_S_SERVER_UNAVAILABLE,attempt;
    ULONG count=0;
    DTASKMGR_WORKER *entries=NULL;
    BOOL empty=argc==2 && !_stricmp(argv[1],"--empty");
    BOOL existing=argc>=2 && (!_stricmp(argv[1],"--existing") || !_stricmp(argv[1],"--terminate"));
    BOOL terminate=(argc==2 || argc==3) && !_stricmp(argv[1],"--terminate");
    DWORD selected_pid=terminate && argc==3 ? strtoul(argv[2],NULL,10) : 0;
    ULONG index;
    if (argc!=1 && !empty && !existing) { fputs("usage: monitor-rpc-test [--empty|--existing|--terminate [pid]]\n",stderr); return 2; }
    CHECK(broker_rpc_capture_scope(&scope));
    if (!existing) {
        CHECK(GetModuleFileNameW(NULL,path,MAX_PATH));
        slash=wcsrchr(path,L'\\'); CHECK(slash!=NULL); lstrcpyW(slash+1,L"ntsrv.exe");
        CHECK(CreateProcessW(path,NULL,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&startup,&broker));
    }
    self=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE|PROCESS_DUP_HANDLE,FALSE,GetCurrentProcessId());
    CHECK(self!=NULL);
    binding=bind_server(&scope); CHECK(binding!=NULL);
    for (attempt=0;attempt<100;++attempt) {
        RpcTryExcept {
            error=Client_TaskSnapshot(binding,self,APP_PROTOCOL_VERSION,(unsigned char *)version,
                &count,&entries);
        }
        RpcExcept(1) { error=RpcExceptionCode(); }
        RpcEndExcept
        if (error==ERROR_SUCCESS) break;
        Sleep(50);
    }
    CHECK(error==ERROR_SUCCESS && (!existing || count));
    if (existing) {
        for (index=0;index<count;++index)
            wprintf(L"WORKER pid=%lu task=%lu kind=%lu state=%lu image=%ls\n",
                (unsigned long)entries[index].process_id,(unsigned long)entries[index].task,
                (unsigned long)entries[index].kind,(unsigned long)entries[index].state,
                entries[index].image);
        if (terminate) {
            ULONG selected=0;
            if(argc==3) {
                CHECK(selected_pid!=0);
                for(selected=0;selected<count;++selected)
                    if(entries[selected].process_id==selected_pid)break;
                CHECK(selected<count);
            } else CHECK(count==1);
            RpcTryExcept {
                error=Client_TerminateWorker(binding,self,APP_PROTOCOL_VERSION,(unsigned char *)version,
                    entries[selected].process_id);
            }
            RpcExcept(1) { error=RpcExceptionCode(); }
            RpcEndExcept
            CHECK(error==ERROR_SUCCESS);
            puts("PASS: authenticated DTASKMGR RPC accepted selected live worker termination");
        }
        MIDL_user_free(entries); RpcBindingFree(&binding); CloseHandle(self);
        puts("PASS: authenticated DTASKMGR RPC sees at least one live worker");
        return 0;
    }
    CHECK(count==0 && entries==NULL);
    /* The empty path isolates the public management ABI from the host's
     * separately exercised Console named-pipe policy. */
    if (!empty) CHECK(console_context_rpc(binding,self)==0);
    RpcTryExcept {
        error=Client_TaskSnapshot(binding,self,APP_PROTOCOL_VERSION+1,(unsigned char *)version,
            &count,&entries);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    CHECK(error==ERROR_REVISION_MISMATCH);
    RpcTryExcept {
        error=Client_TerminateWorker(binding,self,APP_PROTOCOL_VERSION,(unsigned char *)version,1);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    CHECK(error==ERROR_NOT_FOUND);
    RpcBindingFree(&binding); CloseHandle(self);
    /* A preceding isolated root test can still own the singleton during its
     * empty grace. Our duplicate broker then exits normally; never terminate
     * that other process or mistake termination of our exited copy for a fail. */
    if(WaitForSingleObject(broker.hProcess,0)==WAIT_TIMEOUT)
        CHECK(TerminateProcess(broker.hProcess,0));
    else {
        DWORD code;
        CHECK(GetExitCodeProcess(broker.hProcess,&code) && code==0);
    }
    CHECK(WaitForSingleObject(broker.hProcess,5000)==WAIT_OBJECT_0);
    CloseHandle(broker.hThread);CloseHandle(broker.hProcess);
    puts(empty ? "PASS: empty PID-only DTASKMGR RPC rejects version and absent worker"
        : "PASS: authenticated DTASKMGR RPC sees empty broker, rejects version and cannot terminate absent worker");
    return 0;
}
