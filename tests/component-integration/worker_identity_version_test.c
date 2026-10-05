/* Rename-package identity gate against the real authenticated NTSRV endpoint.
 * No worker/provider substitute and no desktop interaction. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc.h>
#include "service.h"
#include "common/protocol/version.h"
#include "common/protocol/management.h"
#include "common/image_classification.h"
#include "common/codec/native_launch.h"
#include "ntsrv-exe/transport/rpc_security.h"

void *__RPC_USER midl_user_allocate(size_t bytes) { return malloc(bytes); }
void __RPC_USER midl_user_free(void *value) { free(value); }

/* Real service direct GUI execution; not a text-I/O substitute. */
static DWORD direct_native(RPC_BINDING_HANDLE binding,HANDLE self,VDM_CONNECTION connection,
    ULONG generation,PCWSTR image)
{
    WCHAR command[2*MAX_PATH],directory[MAX_PATH];
    PWSTR environment=GetEnvironmentStringsW();
    BYTE *payload=NULL;DWORD bytes=0,error=0;
    run16_native_start start={0};
    if(!environment)return GetLastError();
    if(!GetCurrentDirectoryW(MAX_PATH,directory) ||
        swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" grandchild",image)<0)
        {error=ERROR_INVALID_DATA;goto done;}
    start.application=image;start.command=command;start.directory=directory;
    start.environment=environment;start.console_mask=7;
    error=run16_native_launch_pack(&start,&payload,&bytes);
    if(error)goto done;
    for(unsigned round=0;round<2;++round) {
        HANDLE target=NULL,receipt=NULL;ULONG request=0,code=MAXDWORD,completed=0;
        error=Client_SubmitNativeRequest(binding,connection,self,generation,NULL,
            bytes,payload,&target,&receipt,&request);
        if(!error && (!target || !receipt || !request))error=ERROR_INVALID_DATA;
        if(!error && WaitForSingleObject(target,10000)!=WAIT_OBJECT_0)error=ERROR_TIMEOUT;
        if(!error && (!GetExitCodeProcess(target,&code) || code))error=ERROR_INVALID_DATA;
        if(!error && WaitForSingleObject(receipt,10000)!=WAIT_OBJECT_0)error=ERROR_TIMEOUT;
        if(!error)error=Client_FinishNativeRequest(binding,connection,self,generation,request,&code,&completed);
        if(!error && (!completed || code))error=ERROR_INVALID_DATA;
        if(target)CloseHandle(target);if(receipt)CloseHandle(receipt);
        if(error)goto done;
    }
    puts("PASS real native direct execution twice: matching Hook loaded, actual process result, service receipt");
done:
    if(payload)HeapFree(GetProcessHeap(),0,payload);
    FreeEnvironmentStringsW(environment);
    return error;
}
static DWORD native_workers(RPC_BINDING_HANDLE binding,HANDLE self,PCWSTR gui32,PCWSTR gui64)
{
    const ULONG machines[]={IMAGE_FILE_MACHINE_I386,IMAGE_FILE_MACHINE_AMD64};
    unsigned char app[APP_VERSION_BYTES]=APP_VERSION,server[APP_VERSION_BYTES];
    unsigned index;
    for(index=0;index<2;++index) {
        VDM_CONNECTION connection=NULL;
        ULONG protocol=0,generation=0,count=0;
        DTASKMGR_WORKER *items=NULL;
        HANDLE worker=NULL,reused=NULL,changed=NULL;
        DWORD error=0,machine=0,pid=0;
        const char *phase="connect";
        ULONGLONG deadline;
        BOOL found=FALSE;
        error=Client_Connect(binding,self,APP_PROTOCOL_VERSION,app,
            &protocol,server,&connection,&generation);
        if(error)goto cleanup;
        phase="state-event";
        error=Client_WorkerStateChanged(binding,connection,self,generation,&changed);
        if(error)goto cleanup;
        phase="start";
        error=Client_StartNativeWorker(binding,connection,self,generation,machines[index],&worker);
        if(error)goto cleanup;
        pid=GetProcessId(worker);
        error=common_process_machine(worker,&machine);
        if(error)goto cleanup;
        if(!pid || machine!=machines[index]){error=ERROR_BAD_EXE_FORMAT;goto cleanup;}
        deadline=GetTickCount64()+10000;
        phase="ready-snapshot";
        for(;;) {
            error=Client_TaskSnapshot(binding,self,APP_PROTOCOL_VERSION,app,&count,&items);
            if(error)goto cleanup;
            for(ULONG row=0;row<count;++row)
                if(items[row].key.category==MANAGEMENT_WORKER && items[row].process_id==pid &&
                   items[row].kind==(index ? MANAGEMENT_KIND_WIN64 : MANAGEMENT_KIND_WIN32) &&
                   items[row].display_state==MANAGEMENT_IDLE)found=TRUE;
            midl_user_free(items);items=NULL;
            if(found)break;
            ULONGLONG now=GetTickCount64();
            if(now>=deadline){error=ERROR_TIMEOUT;goto cleanup;}
            HANDLE wait[]={worker,changed};
            DWORD result=WaitForMultipleObjects(2,wait,FALSE,(DWORD)(deadline-now));
            if(result!=WAIT_OBJECT_0+1){error=result==WAIT_TIMEOUT ? ERROR_TIMEOUT : ERROR_PROCESS_ABORTED;goto cleanup;}
        }
        /* A new launcher gets a new authenticated connection. The creation
         * reservation belongs to the first launcher, not a reusable task. */
        phase="disconnect-creator";
        error=Client_Disconnect(binding,self,generation,&connection);
        if(error)goto cleanup;
        generation=0;
        phase="connect-next-launcher";
        error=Client_Connect(binding,self,APP_PROTOCOL_VERSION,app,
            &protocol,server,&connection,&generation);
        if(error)goto cleanup;
        phase="reuse";
        error=Client_StartNativeWorker(binding,connection,self,generation,machines[index],&reused);
        if(error)goto cleanup;
        if(GetProcessId(reused)!=pid || WaitForSingleObject(worker,0)!=WAIT_TIMEOUT)
            {error=ERROR_INVALID_DATA;goto cleanup;}
        CloseHandle(reused);reused=NULL;
        error=Client_StartNativeWorker(binding,connection,self,generation,
            machines[1-index],&reused);
        if(error!=ERROR_INVALID_STATE || reused){error=ERROR_INVALID_DATA;goto cleanup;}
        printf("PASS real RPC native machine=%04lx pid=%lu READY projection, same-PID reuse, selected-width change rejected\n",machines[index],pid);
        error=ERROR_SUCCESS;
        if(index ? gui64!=NULL : gui32!=NULL) {
            phase="direct-execution";
            error=direct_native(binding,self,connection,generation,index ? gui64 : gui32);
        }
cleanup:
        if(error)fprintf(stderr,"native RPC machine=%04lx phase=%s error=%lu pid=%lu\n",machines[index],phase,error,pid);
        if(items)midl_user_free(items);
        if(reused)CloseHandle(reused);
        if(worker) {
            /* Exact handle returned for this fixture's own creation. Cleanup
             * is not evidence of ordinary worker retirement. */
            if(WaitForSingleObject(worker,0)==WAIT_TIMEOUT) {
                FILETIME original,opened,exit,kernel,user;
                HANDLE terminate=OpenProcess(PROCESS_TERMINATE|PROCESS_QUERY_LIMITED_INFORMATION,
                    FALSE,GetProcessId(worker));
                if(!terminate || !GetProcessTimes(worker,&original,&exit,&kernel,&user) ||
                    !GetProcessTimes(terminate,&opened,&exit,&kernel,&user) ||
                    memcmp(&original,&opened,sizeof(original)) || !TerminateProcess(terminate,0)) {
                    if(!error)error=ERROR_ACCESS_DENIED;
                }
                if(terminate)CloseHandle(terminate);
            }
            if(WaitForSingleObject(worker,5000)!=WAIT_OBJECT_0 && !error)error=ERROR_TIMEOUT;
            CloseHandle(worker);
        }
        if(changed)CloseHandle(changed);
        if(connection)Client_Disconnect(binding,self,generation,&connection);
        if(error)return error;
    }
    return ERROR_SUCCESS;
}

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
    if ((argc < 2 || argc > 6) || !broker_rpc_capture_scope(&scope)) return 2;
    if(argc>=4 && wcscmp(argv[3],L"--native-workers"))return 2;
    if(RpcIfInqId(Client_vdm_service_v39_0_c_ifspec,&interface_id) ||
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
    memcpy(app,"0.0.423",sizeof("0.0.423"));
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
    if(argc>=3) {
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
    if(argc>=4) {
        error=native_workers(binding,self,argc>=5 ? argv[4] : NULL,argc==6 ? argv[5] : NULL);
        if(error)goto done;
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
