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
#include "common/protocol/version.h"
#include "common/rpc/management.h"
#include "common/protocol/management.h"
#include <rpcasync.h>
#include "common/rpc/async_call.h"

#define CHECK(value) do { if (!(value)) { fprintf(stderr,"FAIL %d: %lu\n",__LINE__,(unsigned long)GetLastError()); return 1; } } while (0)

void *__RPC_USER MIDL_user_allocate(size_t bytes) { return malloc(bytes); }
void __RPC_USER MIDL_user_free(void *value) { free(value); }

static const unsigned char version[APP_VERSION_BYTES]=APP_VERSION;
/* Real async wire negative, deliberately allowing version mutation. Normal
 * publication uses the production common bounded client, not this fixture. */
static DWORD observation_call(RPC_BINDING_HANDLE binding,HANDLE reporter,ULONG protocol,
    HANDLE child,DWORD flags,hyper *node)
{
    RPC_ASYNC_STATE async={0};DWORD error,reply=ERROR_INVALID_STATE;
    HANDLE completed=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!completed)return GetLastError();*node=0;
    error=RpcAsyncInitializeHandle(&async,sizeof(async));
    if(!error) {
        async.NotificationType=RpcNotificationTypeEvent;async.u.hEvent=completed;
        RpcTryExcept {
            Client_ObserveNativeCreationAsync(&async,binding,reporter,protocol,
                (unsigned char *)version,child,flags,node);
        }
        RpcExcept(1) {error=RpcExceptionCode();}
        RpcEndExcept
        if(!error) {
            if(WaitForSingleObject(completed,1000)!=WAIT_OBJECT_0) {
                error=RpcAsyncCancelCall(&async,TRUE);
                (void)WaitForSingleObject(completed,INFINITE);
            }
            {DWORD completion=RpcAsyncCompleteCall(&async,&reply);if(!error)error=completion;}
            if(!error)error=reply;
        }
    }
    CloseHandle(completed);return error;
}

static DWORD dos_observation_call(RPC_BINDING_HANDLE binding,VDM_CONNECTION connection,
    HANDLE process,DWORD generation,DWORD protocol)
{
    RPC_ASYNC_STATE async={0};DOS_OBSERVATION_FACT fact={0};
    HANDLE completed;DWORD error,reply=ERROR_INVALID_STATE;BOOL issued=FALSE;
    fact.event=1;fact.occurrence=1;fact.direct=1;fact.psp=0x100;
    error=RpcAsyncInitializeHandle(&async,sizeof(async));if(error)return error;
    completed=CreateEventW(NULL,TRUE,FALSE,NULL);if(!completed)return GetLastError();
    async.NotificationType=RpcNotificationTypeEvent;async.u.hEvent=completed;
    RpcTryExcept {
        Client_ObserveDosEventAsync(&async,binding,connection,process,generation,protocol,
            (unsigned char *)version,&fact,0);
        issued=TRUE;error=ERROR_SUCCESS;
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    if(issued){error=common_rpc_finish_async(&async,completed,NULL,1000,&reply);if(!error)error=reply;}
    CloseHandle(completed);return error;
}

static void json_string(const WCHAR *value)
{
    const WCHAR *p;
    putwchar(L'"');
    for(p=value;*p;++p) {
        if(*p==L'"' || *p==L'\\')putwchar(L'\\');
        if(*p<32)wprintf(L"\\u%04x",(unsigned)*p);
        else putwchar(*p);
    }
    putwchar(L'"');
}
static void json_key(const DTASKMGR_KEY *key)
{
    wprintf(L"\"%016llx:%lu:%lu:%016llx\"",key->instance,
        key->category,key->generation,key->object);
}
static void json_snapshot(const DTASKMGR_WORKER *rows,ULONG count)
{
    ULONG i;
    putwchar(L'[');
    for(i=0;i<count;++i) {
        if(i)putwchar(L',');
        wprintf(L"{\"key\":");json_key(&rows[i].key);
        wprintf(L",\"parent\":");json_key(&rows[i].parent);
        wprintf(L",\"category\":%lu,\"pid\":%lu,\"kind\":%lu,\"depth\":%lu,\"actions\":%lu,\"state\":%lu,\"stack\":%lu,\"image\":",
            rows[i].key.category,rows[i].process_id,rows[i].kind,rows[i].depth,
            rows[i].actions,rows[i].display_state,rows[i].stack_depth);
        json_string(rows[i].image);putwchar(L'}');
    }
    wprintf(L"]\n");
}

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
    swprintf_s(text,32,L"%llx",(unsigned long long)(ULONG_PTR)inherited_frontend);
    CHECK(SetEnvironmentVariableW(L"NTVDM_FRONTEND_CAPABILITY",text));
    swprintf_s(text,32,L"%llx",(unsigned long long)(ULONG_PTR)inherited_execution);
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
    HANDLE frontend=NULL,execution=NULL,root=NULL,restored=NULL;
    HANDLE forged=CreateEventW(NULL,TRUE,FALSE,NULL);
    ULONG server_protocol=0,generation=0;
    unsigned char server_version[APP_VERSION_BYTES]={0};
    unsigned char wrong_version[APP_VERSION_BYTES];
    DWORD error;
#define RPC_CHECK(call,expected) do { \
    RpcTryExcept { error=(call); } \
    RpcExcept(1) { error=RpcExceptionCode(); } RpcEndExcept \
    if(error!=(expected))fprintf(stderr,"RPC actual=%lu expected=%lu\n",error,(DWORD)(expected)); \
    CHECK(error==(expected)); \
} while (0)
    CHECK(forged);
    RPC_CHECK(Client_Connect(binding,self,APP_PROTOCOL_VERSION-1,(unsigned char *)version,
        &server_protocol,server_version,&connection,&generation),ERROR_REVISION_MISMATCH);
    CHECK(!connection && !generation);
    memcpy(wrong_version,version,sizeof(wrong_version));wrong_version[0]='!';
    RPC_CHECK(Client_Connect(binding,self,APP_PROTOCOL_VERSION,wrong_version,
        &server_protocol,server_version,&connection,&generation),ERROR_REVISION_MISMATCH);
    CHECK(!connection && !generation);
    RPC_CHECK(Client_Connect(binding,self,APP_PROTOCOL_VERSION,(unsigned char *)version,
        &server_protocol,server_version,&connection,&generation),ERROR_SUCCESS);
    CHECK(connection && generation && server_protocol==APP_PROTOCOL_VERSION &&
        !memcmp(server_version,version,sizeof(version)));
    /* A launcher cannot impersonate a native worker or report a guessed
     * request's startup, even with valid process/event attachments. */
    RPC_CHECK(Client_NativeStartupResult(binding,connection,self,generation,
        generation,1,ERROR_SUCCESS,self,forged),ERROR_ACCESS_DENIED);
    RPC_CHECK(Client_NativeStartupResult(binding,connection,self,generation+1,
        generation,1,ERROR_ACCESS_DENIED,NULL,NULL),ERROR_ACCESS_DENIED);
    RPC_CHECK(Client_RegisterFrontendRoot(binding,connection,self,generation,forged),ERROR_ACCESS_DENIED);
    RPC_CHECK(Client_FrontendStartupResult(binding,connection,self,generation,
        forged,ERROR_ACCESS_DENIED),ERROR_ACCESS_DENIED);
    RPC_CHECK(Client_FrontendStartupResult(binding,connection,self,generation+1,
        forged,ERROR_SUCCESS),ERROR_ACCESS_DENIED);
    RPC_CHECK(Client_StartFrontend(binding,connection,self,generation,
        (ULONGLONG)(UINT_PTR)GetConsoleWindow(),TRUE,&root,&frontend,&restored),ERROR_SUCCESS);
    CHECK(root && frontend && restored);
    RPC_CHECK(Client_AcquireConsoleContext(binding,connection,self,generation+1,frontend,&execution),
        ERROR_ACCESS_DENIED);
    CHECK(!execution);
    RPC_CHECK(Client_AcquireConsoleContext(binding,connection,self,generation,frontend,&execution),
        ERROR_SUCCESS);
    CHECK(execution && WaitForSingleObject(execution,0)==WAIT_TIMEOUT);
    CHECK(!SetEvent(execution) && GetLastError()==ERROR_ACCESS_DENIED);
    {
        HANDLE received=NULL,sender=NULL,context=NULL,io=NULL,probe=NULL,target=NULL,receipt=NULL;
        BYTE malformed=0;
        DWORD caller_generation=0,bytes=0;
        ULONG request=0;
        RPC_CHECK(Client_SubmitNativeRequest(binding,connection,self,generation+1,frontend,0,NULL,
            &target,&receipt,&request),ERROR_ACCESS_DENIED);
        CHECK(!target && !receipt && !request);
        RPC_CHECK(Client_SubmitNativeRequest(binding,connection,self,generation,execution,0,NULL,
            &target,&receipt,&request),ERROR_ACCESS_DENIED);
        CHECK(!target && !receipt && !request);
        RPC_CHECK(Client_SubmitNativeRequest(binding,connection,self,generation,frontend,1,&malformed,
            &target,&receipt,&request),ERROR_INVALID_DATA);
        CHECK(!target && !receipt && !request);
        /* A zero-byte request is the accepted broker parent-resume operation.
         * This fresh launcher has no completed child/selected parent, so the
         * original service_prepare_parent_resume contract rejects its state;
         * it is not a pending command waiting for a worker to become ready. */
        RPC_CHECK(Client_SubmitNativeRequest(binding,connection,self,generation,frontend,0,NULL,
            &target,&receipt,&request),ERROR_INVALID_STATE);
        CHECK(!target && !receipt && !request);
        RPC_CHECK(Client_GetNextNativeCommand(binding,connection,self,generation,1,&malformed,&bytes,&sender,&context,&io,&request,&caller_generation),ERROR_ACCESS_DENIED);
        CHECK(!received && !sender && !context && !io);
        /* The launcher now retains a separate real NTCON root. It cannot
         * consume that root's worker-I/O request queue. */
        RPC_CHECK(Client_FrontendRequest(binding,connection,self,generation,&request,&probe),ERROR_ACCESS_DENIED);
        CHECK(!probe && !request && WaitForSingleObject(frontend,0)==WAIT_TIMEOUT);
        puts("PASS actual RPC worker-only execution: generation/capability/payload rejection, no client pipe attachment, no submit/take without worker admission");
    }
    RPC_CHECK(Client_BindConsoleContext(binding,connection,self,generation,frontend),ERROR_ACCESS_DENIED);
    RPC_CHECK(Client_BindConsoleContext(binding,connection,self,generation+1,execution),ERROR_ACCESS_DENIED);
    RPC_CHECK(Client_BindConsoleContext(binding,connection,self,generation,execution),ERROR_SUCCESS);
    CHECK(inner_launcher(frontend,frontend,CREATE_NO_WINDOW,ERROR_ACCESS_DENIED)==0);
    /* Positive nested execution belongs to the product CLI tests. */
    puts("PASS: production inner run16 rejects frontend object used as execution capability");
    RPC_CHECK(Client_Disconnect(binding,self,generation,&connection),ERROR_SUCCESS);
    CHECK(!connection);
    /* Launcher disconnect does not revoke a live root. Observe the broker's
     * real workerless retirement before asserting stale-context rejection. */
    CHECK(WaitForSingleObject(root,15000)==WAIT_OBJECT_0);
    RPC_CHECK(Client_Connect(binding,self,APP_PROTOCOL_VERSION,(unsigned char *)version,
        &server_protocol,server_version,&connection,&generation),ERROR_SUCCESS);
    RPC_CHECK(Client_BindConsoleContext(binding,connection,self,generation,execution),ERROR_ACCESS_DENIED);
    RPC_CHECK(Client_Disconnect(binding,self,generation,&connection),ERROR_SUCCESS);
    CloseHandle(execution);CloseHandle(frontend);CloseHandle(root);
    CloseHandle(restored);CloseHandle(forged);
    puts("PASS: typed execution Console RPC, restricted rights, separate frontend authority, generation and root rundown");
#undef RPC_CHECK
    return 0;
}

int main(int argc,char **argv)
{
    broker_rpc_scope scope={0};
    RPC_BINDING_HANDLE binding;
    common_rpc_management management;
    PROCESS_INFORMATION broker={0};
    STARTUPINFOW startup={sizeof(startup)};
    WCHAR path[MAX_PATH],*slash;
    HANDLE self;
    DWORD error=RPC_S_SERVER_UNAVAILABLE,attempt;
    ULONG count=0;
    DTASKMGR_WORKER *entries=NULL;
    BOOL empty=argc==2 && !_stricmp(argv[1],"--empty");
    BOOL tree=argc==2 && !_stricmp(argv[1],"--tree-json");
    BOOL trace=argc==3 && !_stricmp(argv[1],"--trace-json");
    BOOL node_close=argc==4 && !_stricmp(argv[1],"--close-node");
    ULONG selected_category=node_close ? strtoul(argv[2],NULL,10) : MANAGEMENT_WORKER;
    BOOL existing=trace || tree || node_close || (argc>=2 && (!_stricmp(argv[1],"--existing") || !_stricmp(argv[1],"--terminate")));
    BOOL terminate=(argc==2 || argc==3) && !_stricmp(argv[1],"--terminate");
    DWORD selected_pid=node_close ? strtoul(argv[3],NULL,10) : terminate && argc==3 ? strtoul(argv[2],NULL,10) : 0;
    ULONG index;
    if (argc!=1 && !empty && !existing) { fputs("usage: monitor-rpc-test [--empty|--existing|--tree-json|--terminate [pid]|--close-node category pid]\n",stderr); return 2; }
    CHECK(broker_rpc_capture_scope(&scope));
    if (!existing) {
        CHECK(GetModuleFileNameW(NULL,path,MAX_PATH));
        slash=wcsrchr(path,L'\\'); CHECK(slash!=NULL); lstrcpyW(slash+1,L"ntsrv.exe");
        CHECK(CreateProcessW(path,NULL,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&startup,&broker));
    }
    self=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE|PROCESS_DUP_HANDLE,FALSE,GetCurrentProcessId());
    CHECK(self!=NULL);
    binding=bind_server(&scope); CHECK(binding!=NULL);
    management.binding=binding;management.process=self;
    for (attempt=0;attempt<100;++attempt) {
        error=common_rpc_task_snapshot(&management,&count,&entries);
        if (error==ERROR_SUCCESS) break;
        Sleep(50);
    }
    if(error!=ERROR_SUCCESS || (existing && !tree && !count)) {
        fprintf(stderr,"TaskSnapshot error=%lu count=%lu existing=%d\n",
            (unsigned long)error,(unsigned long)count,existing);
        return 1;
    }
    if (existing) {
        if(trace) {
            WORKER_TRACE_NODE *nodes=NULL;ULONG actual=0,coverage=0;
            DWORD pid=strtoul(argv[2],NULL,10);
            CHECK(pid);
            for(index=0;index<count;++index)
                if(entries[index].key.category==MANAGEMENT_WORKER && entries[index].process_id==pid)break;
            CHECK(index<count);
            CHECK(!common_rpc_worker_task_trace(&management,&entries[index].key,&coverage,&actual,&nodes));
            wprintf(L"{\"coverage\":%lu,\"nodes\":[",coverage);
            for(index=0;index<actual;++index) {
                if(index)putwchar(L',');
                wprintf(L"{\"node\":%llu,\"parent\":%llu,\"relation\":%lu,\"source\":%lu,\"kind\":%lu,\"pid\":%lu,\"task\":%lu,\"state\":%lu,\"flags\":%lu,\"exit\":%lu,\"psp\":%lu,\"image\":",
                    nodes[index].node,nodes[index].parent,nodes[index].relation,nodes[index].source,
                    nodes[index].kind,nodes[index].process_id,nodes[index].task,
                    nodes[index].state,nodes[index].flags,nodes[index].reserved,nodes[index].dos_psp);
                json_string(nodes[index].image);putwchar(L'}');
            }
            wprintf(L"]}\n");if(nodes)MIDL_user_free(nodes);
            MIDL_user_free(entries);RpcBindingFree(&binding);CloseHandle(self);return 0;
        }
        if(tree) {
            json_snapshot(entries,count);
            MIDL_user_free(entries);RpcBindingFree(&binding);CloseHandle(self);
            return 0;
        }
        for (index=0;index<count;++index)
            wprintf(L"%ls pid=%lu task=%lu kind=%lu state=%lu image=%ls\n",
                entries[index].key.category==MANAGEMENT_WORKER ? L"WORKER" : L"NODE",
                (unsigned long)entries[index].process_id,(unsigned long)entries[index].task,
                (unsigned long)entries[index].kind,(unsigned long)entries[index].state,
                entries[index].image);
        if (terminate || node_close) {
            ULONG selected=0;
            if(argc==3 || node_close) {
                CHECK(selected_pid!=0);
                for(selected=0;selected<count;++selected)
                    if(entries[selected].process_id==selected_pid &&
                        entries[selected].key.category==selected_category)break;
                CHECK(selected<count);
            } else {
                ULONG workers=0;
                for(index=0;index<count;++index)if(entries[index].key.category==MANAGEMENT_WORKER) {
                    selected=index;++workers;
                }
                CHECK(workers==1);
            }
            error=common_rpc_close_management_node(&management,&entries[selected].key);
            if(error)fprintf(stderr,"TerminateWorker pid=%lu error=%lu\n",
                (unsigned long)entries[selected].process_id,(unsigned long)error);
            CHECK(error==ERROR_SUCCESS);
            puts("PASS: authenticated DTASKMGR RPC accepted selected live node close");
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
    {
        unsigned char wrong_version[APP_VERSION_BYTES];
        memcpy(wrong_version,version,sizeof(wrong_version));wrong_version[0]^=1;
        RpcTryExcept {
            error=Client_TaskSnapshot(binding,self,APP_PROTOCOL_VERSION,wrong_version,&count,&entries);
        }
        RpcExcept(1) {error=RpcExceptionCode();}
        RpcEndExcept
        CHECK(error==ERROR_REVISION_MISMATCH);
    }
    RpcTryExcept {
        DTASKMGR_KEY stale={1,MANAGEMENT_WORKER,1,0};
        error=Client_CloseManagementNode(binding,self,APP_PROTOCOL_VERSION,(unsigned char *)version,&stale);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    CHECK(error==ERROR_INVALID_HANDLE);
    {
        DTASKMGR_KEY absent={1,MANAGEMENT_WORKER,1,0};
        WORKER_TRACE_NODE *nodes=NULL;ULONG actual=0,coverage=0;
        CHECK(common_rpc_worker_task_trace(&management,&absent,&coverage,&actual,&nodes)==ERROR_INVALID_HANDLE);
        CHECK(!actual && !coverage && !nodes);
        RpcTryExcept {
            error=Client_WorkerTaskTrace(binding,self,APP_PROTOCOL_VERSION-1,
                (unsigned char *)version,&absent,&coverage,&actual,&nodes);
        }
        RpcExcept(1) {error=RpcExceptionCode();}
        RpcEndExcept
        CHECK(error==ERROR_REVISION_MISMATCH && !actual && !coverage && !nodes);
    }
    {
        /* Real typed object whose parent is this authenticated client, but
         * this client is not a registered Direct/Observed reporter. */
        STARTUPINFOW created_start={sizeof(created_start)};PROCESS_INFORMATION created={0};
        WCHAR own[MAX_PATH],line[MAX_PATH+32];hyper identity=99;
        CHECK(GetModuleFileNameW(NULL,own,MAX_PATH));
        swprintf_s(line,ARRAYSIZE(line),L"\"%s\" --empty",own);
        CHECK(CreateProcessW(own,line,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,
            NULL,NULL,&created_start,&created));
        RpcTryExcept {
            error=observation_call(binding,self,APP_PROTOCOL_VERSION,created.hProcess,CREATE_SUSPENDED,&identity);
        }
        RpcExcept(1) {error=RpcExceptionCode();}
        RpcEndExcept
        CHECK(error==ERROR_ACCESS_DENIED && !identity);
        identity=99;
        RpcTryExcept {
            /* An attachment of a different real process cannot stand in for
             * the RPC caller even though both processes share the logon. */
            error=observation_call(binding,created.hProcess,APP_PROTOCOL_VERSION,self,0,&identity);
        }
        RpcExcept(1) {error=RpcExceptionCode();}
        RpcEndExcept
        CHECK(error==RPC_S_ACCESS_DENIED && !identity);
        identity=99;
        RpcTryExcept {
            error=observation_call(binding,self,APP_PROTOCOL_VERSION-1,created.hProcess,0,&identity);
        }
        RpcExcept(1) {error=RpcExceptionCode();}
        RpcEndExcept
        CHECK(error==ERROR_REVISION_MISMATCH && !identity);
        {
            VDM_CONNECTION connected=NULL;ULONG generation=0,protocol=0;
            unsigned char application[32]={0};
            CHECK(!Client_Connect(binding,self,APP_PROTOCOL_VERSION,(unsigned char *)version,
                &protocol,application,&connected,&generation) && connected && generation);
            CHECK(dos_observation_call(binding,connected,self,generation,APP_PROTOCOL_VERSION)==ERROR_ACCESS_DENIED);
            CHECK(dos_observation_call(binding,connected,self,generation+1,APP_PROTOCOL_VERSION)==ERROR_ACCESS_DENIED);
            CHECK(dos_observation_call(binding,connected,created.hProcess,generation,APP_PROTOCOL_VERSION)==RPC_S_ACCESS_DENIED);
            CHECK(dos_observation_call(binding,connected,self,generation,APP_PROTOCOL_VERSION-1)==ERROR_REVISION_MISMATCH);
            CHECK(!Client_Disconnect(binding,self,generation,&connected) && !connected);
            puts("PASS real DOS observation rejects non-worker, wrong generation, false process attachment and old protocol");
        }
        CHECK(WaitForSingleObject(created.hProcess,0)==WAIT_TIMEOUT);
        CHECK(TerminateProcess(created.hProcess,0) && WaitForSingleObject(created.hProcess,5000)==WAIT_OBJECT_0);
        CloseHandle(created.hThread);CloseHandle(created.hProcess);
        CHECK(!common_rpc_task_snapshot(&management,&count,&entries) && !count && !entries);
        puts("PASS real native observation rejects unknown reporter, false process attachment and old protocol without creating tasks");
    }
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
