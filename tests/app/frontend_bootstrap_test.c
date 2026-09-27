/* Run on an isolated Console desktop with the sibling candidate BaseSrv.
 * This tests the real RPC path; it does not select the ordinary CLI route. */
#include <windows.h>
#include <stdio.h>
#include "frontend-exe/bootstrap.h"
#include "frontend-exe/native_request_client.h"
#include "frontend-exe/native_request_protocol.h"
#include "basesrv-exe/opennt/include/base_rpc_client.h"

PVOID CsrPortHeap;
static int rejected_peer(HANDLE pipe,HANDLE caller)
{
    WCHAR mode[24];DWORD error,bytes;
    HANDLE event=NULL,probe=NULL,remote=NULL;
    frontend_bootstrap_reply reply={FRONTEND_BOOTSTRAP_VERSION,0,APP_VERSION};
    if(!GetEnvironmentVariableW(L"FRONTEND_TEST_REPLY",mode,24)){Sleep(30000);return 74;}
    probe=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!probe)return 80;
    /* Bootstrap no longer grants permission to inject handles into caller. */
    if(DuplicateHandle(GetCurrentProcess(),probe,caller,&remote,SYNCHRONIZE,FALSE,0) ||
        GetLastError()!=ERROR_ACCESS_DENIED){CloseHandle(probe);return 81;}
    CloseHandle(probe);
    if(!wcscmp(mode,L"version"))++reply.version;
    else if(!wcscmp(mode,L"application"))reply.application[0]='!';
    else if(!wcscmp(mode,L"status"))reply.status=ERROR_ACCESS_DENIED;
    bytes=!wcscmp(mode,L"short") ? sizeof(reply)-1 : sizeof(reply);
    event=CreateEventW(NULL,TRUE,FALSE,NULL);if(!event)return 82;
    error=frontend_request_transfer(pipe,caller,NULL,event,TRUE,&reply,bytes);
    CloseHandle(event);CloseHandle(pipe);
    return error ? 83 : 0;
}
static int identity_role(PCWSTR label)
{
    WCHAR text[64],*end;HANDLE owner=NULL,capability;
    DWORD error,generation,pid;
    if(!GetEnvironmentVariableW(L"NTVDM_FRONTEND_CAPABILITY",text,64))return 70;
    capability=(HANDLE)(UINT_PTR)wcstoul(text,&end,16);
    if(!capability || *end)return 71;
    CsrPortHeap=HeapCreate(0,0,0);if(!CsrPortHeap)return 72;
    error=OpenNtBaseClientConnectCurrent();
    if(!error)error=OpenNtBaseClientRetainFrontendRoot(capability,&owner,&generation);
    if(!error){
        pid=GetProcessId(owner);
        if(!pid || pid==GetCurrentProcessId() || WaitForSingleObject(owner,0)!=WAIT_TIMEOUT)error=ERROR_INVALID_DATA;
        else printf("FRONTEND-%ls=%lu\n",label,pid);
    }
    if(owner)CloseHandle(owner);
    OpenNtBaseClientDisconnectCurrent();HeapDestroy(CsrPortHeap);CsrPortHeap=NULL;
    return error ? 73 : 37;
}
static int creator_role(HANDLE output,PCWSTR gate_name,PCWSTR ready_name)
{
    WCHAR self[MAX_PATH],frontend[MAX_PATH],cwd[MAX_PATH],command[1024],*slash;
    frontend_connection connection={0};run16_native_start start={0};
    HANDLE target=NULL;LPWCH environment=NULL;DWORD error,ids[2],written;
    CsrPortHeap=HeapCreate(0,0,0);if(!CsrPortHeap)return 20;
    error=OpenNtBaseClientConnectCurrent();if(error)return 21;
    if(!GetModuleFileNameW(NULL,self,MAX_PATH) || !GetCurrentDirectoryW(MAX_PATH,cwd))return 22;
    wcscpy_s(frontend,MAX_PATH,self);slash=wcsrchr(frontend,L'\\');if(!slash)return 23;
    wcscpy_s(slash+1,MAX_PATH-(size_t)(slash+1-frontend),L"frontend.exe");
    error=frontend_bootstrap_start(frontend,&connection);if(error)return 24;
    environment=GetEnvironmentStringsW();if(!environment)return 25;
    swprintf_s(command,1024,L"\"%ls\" --linger %ls %ls",self,gate_name,ready_name);
    start.application=self;start.command=command;start.directory=cwd;start.environment=environment;start.console_mask=7;
    error=run16_native_request_submit(connection.process,connection.capability,&start,&target);
    if(error)return 26;
    ids[0]=GetProcessId(connection.process);ids[1]=GetProcessId(target);
    if(!WriteFile(output,ids,sizeof(ids),&written,NULL) || written!=sizeof(ids))return 27;
    /* Parent deliberately kills this fixture launcher after both target and
     * frontend are pinned. No production launcher-death cleanup is invoked. */
    WaitForSingleObject(target,15000);return 28;
}
static BOOL creator_loss_case(void)
{
    WCHAR self[MAX_PATH],command[1024],gate_name[96],ready_name[96];
    HANDLE input=NULL,output=NULL,gate=NULL,ready=NULL,frontend=NULL,target=NULL;
    SECURITY_ATTRIBUTES sa={sizeof(sa),NULL,TRUE};STARTUPINFOW si={sizeof(si)};
    PROCESS_INFORMATION launcher={0};DWORD ids[2],bytes,result;BOOL passed=FALSE;
    if(!GetModuleFileNameW(NULL,self,MAX_PATH))goto done;
    swprintf_s(gate_name,96,L"Local\\frontend-loss-%lu-go",GetCurrentProcessId());
    swprintf_s(ready_name,96,L"Local\\frontend-loss-%lu-ready",GetCurrentProcessId());
    gate=CreateEventW(NULL,TRUE,FALSE,gate_name);ready=CreateEventW(NULL,TRUE,FALSE,ready_name);
    if(!gate || !ready || !CreatePipe(&input,&output,&sa,0) || !SetHandleInformation(input,HANDLE_FLAG_INHERIT,0))goto done;
    swprintf_s(command,1024,L"\"%ls\" --creator %Ix %ls %ls",self,(UINT_PTR)output,gate_name,ready_name);
    if(!CreateProcessW(self,command,NULL,NULL,TRUE,0,NULL,NULL,&si,&launcher))goto done;
    CloseHandle(output);output=NULL;CloseHandle(launcher.hThread);launcher.hThread=NULL;
    if(!ReadFile(input,ids,sizeof(ids),&bytes,NULL) || bytes!=sizeof(ids))goto done;
    /* IDs come only from our own test child pipe, never a product authority. */
    frontend=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION|PROCESS_TERMINATE,FALSE,ids[0]);
    target=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION|PROCESS_TERMINATE,FALSE,ids[1]);
    if(!frontend || !target || WaitForSingleObject(ready,5000)!=WAIT_OBJECT_0)goto done;
    if(!TerminateProcess(launcher.hProcess,123) || WaitForSingleObject(launcher.hProcess,5000)!=WAIT_OBJECT_0)goto done;
    if(WaitForSingleObject(frontend,250)!=WAIT_TIMEOUT || WaitForSingleObject(target,0)!=WAIT_TIMEOUT)goto done;
    SetEvent(gate);
    if(WaitForSingleObject(target,5000)!=WAIT_OBJECT_0 || !GetExitCodeProcess(target,&result) || result!=41)goto done;
    if(WaitForSingleObject(frontend,5000)!=WAIT_OBJECT_0 || !GetExitCodeProcess(frontend,&result) || result)goto done;
    passed=TRUE;
done:
    if(gate)SetEvent(gate);
    if(launcher.hProcess){TerminateProcess(launcher.hProcess,ERROR_CANCELLED);WaitForSingleObject(launcher.hProcess,5000);CloseHandle(launcher.hProcess);}
    if(target){if(WaitForSingleObject(target,5000)==WAIT_TIMEOUT)TerminateProcess(target,ERROR_CANCELLED);CloseHandle(target);}
    if(frontend){if(WaitForSingleObject(frontend,5000)==WAIT_TIMEOUT)TerminateProcess(frontend,ERROR_CANCELLED);CloseHandle(frontend);}
    if(input)CloseHandle(input);if(output)CloseHandle(output);if(gate)CloseHandle(gate);if(ready)CloseHandle(ready);
    return passed;
}
int wmain(int argc,WCHAR **argv)
{
    WCHAR image[MAX_PATH],broker_image[MAX_PATH],command[2*MAX_PATH],cwd[MAX_PATH];
    WCHAR *slash; LPWCH environment=NULL;
    STARTUPINFOW startup={sizeof(startup)}; PROCESS_INFORMATION broker={0};
    frontend_connection connection={0},second={0}; run16_native_start start={0};
    HANDLE target=NULL,retained=NULL,capability=NULL,fixture_capability=NULL,fixture_execution=NULL;
    DWORD error=0,result=0,generation,i; int failed=1;
    if(argc==2 && !wcscmp(argv[1],L"--warm-create"))return 0;
    /* A deliberately silent, live bootstrap peer. Production timeout must
     * cancel its pending read and roll back this unaccepted process only. */
    if(argc==5 && !wcscmp(argv[1],L"--session"))return rejected_peer(
        (HANDLE)(UINT_PTR)wcstoul(argv[2],NULL,16),(HANDLE)(UINT_PTR)wcstoul(argv[3],NULL,16));
    if(argc==2 && !wcscmp(argv[1],L"--startup-rejections")){
        static const WCHAR *modes[]={L"version",L"application",L"status",L"short",L"unregistered"};
        DWORD before,after;
        if(!GetModuleFileNameW(NULL,image,MAX_PATH))return 84;
        /* Initialize Windows' process-creation machinery independently of
         * the bootstrap under test; do not hide a first-bootstrap leak. */
        swprintf_s(command,2*MAX_PATH,L"\"%ls\" --warm-create",image);
        if(!CreateProcessW(image,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&startup,&broker))return 87;
        if(WaitForSingleObject(broker.hProcess,5000)!=WAIT_OBJECT_0)return 88;
        CloseHandle(broker.hThread);CloseHandle(broker.hProcess);ZeroMemory(&broker,sizeof(broker));
        for(i=0;i<ARRAYSIZE(modes);++i){
            if(!SetEnvironmentVariableW(L"FRONTEND_TEST_REPLY",modes[i]) ||
                !GetProcessHandleCount(GetCurrentProcess(),&before))return 85;
            error=frontend_bootstrap_start(image,&connection);
            if(!GetProcessHandleCount(GetCurrentProcess(),&after) || before!=after ||
                connection.process || connection.channel || connection.capability ||
                !error || error==ERROR_TIMEOUT ||
                (i<2 && error!=ERROR_REVISION_MISMATCH) ||
                (i==2 && error!=ERROR_ACCESS_DENIED)){
                printf("FAIL rejection %ls error=%lu handles=%lu/%lu\n",modes[i],error,before,after);return 86;
            }
            printf("PASS rejection %ls error=%lu no local handle leak\n",modes[i],error);
        }
        SetEnvironmentVariableW(L"FRONTEND_TEST_REPLY",NULL);return 0;
    }
    if(argc==2 && !wcscmp(argv[1],L"--startup-timeout")){
        ULONGLONG began=GetTickCount64(),elapsed;
        if(!GetModuleFileNameW(NULL,image,MAX_PATH))return 75;
        error=frontend_bootstrap_start(image,&connection);
        elapsed=GetTickCount64()-began;
        if(error!=ERROR_TIMEOUT || elapsed<9000 || elapsed>15000 ||
            connection.process || connection.channel || connection.capability){
            printf("FAIL silent bootstrap error=%lu elapsed=%llu\n",error,elapsed);return 76;
        }
        puts("PASS silent live bootstrap times out, cancels I/O and clears unaccepted connection");return 0;
    }
    if(argc==3 && !wcscmp(argv[1],L"--identity"))return identity_role(argv[2]);
    if(argc==5 && !wcscmp(argv[1],L"--creator"))return creator_role((HANDLE)(UINT_PTR)wcstoul(argv[2],NULL,16),argv[3],argv[4]);
    if(argc==4 && !wcscmp(argv[1],L"--linger")){
        HANDLE gate=OpenEventW(SYNCHRONIZE,FALSE,argv[2]),ready=OpenEventW(EVENT_MODIFY_STATE,FALSE,argv[3]);
        if(!gate || !ready || !SetEvent(ready))return 30;
        result=WaitForSingleObject(gate,15000)==WAIT_OBJECT_0 ? 41 : 31;
        CloseHandle(gate);CloseHandle(ready);puts("TARGET-SURVIVED-CREATOR");return (int)result;
    }
    CsrPortHeap=HeapCreate(0,0,0);if(!CsrPortHeap)return 1;
    /* Never terminate or use a broker that belongs to another test/user. */
    error=OpenNtBaseClientConnectCurrent();
    if(!error){puts("FAIL precondition: broker already running");goto done;}
    if(error!=RPC_S_SERVER_UNAVAILABLE){printf("FAIL precondition broker error=%lu\n",error);goto done;}
    if(!GetModuleFileNameW(NULL,image,MAX_PATH))goto done;
    slash=wcsrchr(image,L'\\');if(!slash)goto done;slash[1]=0;
    wcscpy_s(broker_image,MAX_PATH,image);wcscat_s(broker_image,MAX_PATH,L"basesrv.exe");
    wcscat_s(image,MAX_PATH,L"frontend.exe");
    swprintf_s(command,2*MAX_PATH,L"\"%ls\"",broker_image);
    if(!CreateProcessW(broker_image,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&startup,&broker))goto done;
    CloseHandle(broker.hThread);broker.hThread=NULL;
    for(i=0;i<100;++i){
        error=OpenNtBaseClientConnectCurrent();if(!error)break;
        if(WaitForSingleObject(broker.hProcess,50)==WAIT_OBJECT_0)break;
    }
    if(error){printf("FAIL broker connect=%lu\n",error);goto done;}
    {
        DWORD pending=99,tasks=99;
        error=OpenNtBaseClientFrontendUsage(&pending,&tasks);
        if(error!=ERROR_ACCESS_DENIED || pending!=99 || tasks!=99){puts("FAIL non-owner usage accepted");goto done;}
        fixture_capability=CreateEventW(NULL,TRUE,FALSE,NULL);if(!fixture_capability)goto done;
        error=OpenNtBaseClientRegisterFrontendRoot(fixture_capability);if(error)goto done;
        error=OpenNtBaseClientAcquireConsoleContext(fixture_capability,&fixture_execution);if(error)goto done;
        error=OpenNtBaseClientFrontendUsage(&pending,&tasks);
        if(error || pending || tasks){printf("FAIL empty owner usage=%lu\n",error);goto done;}
        error=OpenNtBaseClientRetireFrontend();if(error){puts("FAIL empty owner retirement");goto done;}
        error=OpenNtBaseClientBindConsoleContext(fixture_execution);
        if(error!=ERROR_PIPE_NOT_CONNECTED){puts("FAIL retiring owner accepted old execution capability");goto done;}
        puts("PASS retirement rejects previously issued execution capability over RPC");
        {
            HANDLE rejected=NULL;DWORD rejected_generation;
            error=OpenNtBaseClientRetainFrontendRoot(fixture_capability,&rejected,&rejected_generation);
            if(rejected)CloseHandle(rejected);
            if(error!=ERROR_PIPE_NOT_CONNECTED){puts("FAIL retiring owner accepted a new join");goto done;}
        }
    }
    error=frontend_bootstrap_start(image,&connection);
    if(error){printf("FAIL bootstrap=%lu\n",error);goto done;}
    error=OpenNtBaseClientRetainFrontendRoot(connection.capability,&retained,&generation);
    if(error || GetProcessId(retained)!=GetProcessId(connection.process) || GetProcessId(retained)==GetCurrentProcessId()){
        printf("FAIL authenticated owner=%lu\n",error);goto done;
    }
    if(!DuplicateHandle(GetCurrentProcess(),connection.capability,GetCurrentProcess(),&capability,0,FALSE,DUPLICATE_SAME_ACCESS))goto done;
    frontend_bootstrap_release(&connection);
    if(WaitForSingleObject(retained,100)!=WAIT_TIMEOUT){puts("FAIL creator release killed frontend");goto done;}
    if(!GetEnvironmentVariableW(L"COMSPEC",broker_image,MAX_PATH) || !GetCurrentDirectoryW(MAX_PATH,cwd))goto done;
    environment=GetEnvironmentStringsW();if(!environment)goto done;
    swprintf_s(command,2*MAX_PATH,L"\"%ls\" /d /c echo FRONTEND-FINAL-37 & exit 37",broker_image);
    start.application=broker_image;start.command=command;start.directory=cwd;start.environment=environment;start.console_mask=7;
    error=run16_native_request_submit(retained,capability,&start,&target);
    if(error || WaitForSingleObject(target,10000)!=WAIT_OBJECT_0 || !GetExitCodeProcess(target,&result) || result!=37){
        printf("FAIL native handoff=%lu result=%lu\n",error,result);goto done;
    }
    if(WaitForSingleObject(retained,5000)!=WAIT_OBJECT_0 || !GetExitCodeProcess(retained,&result) || result){
        printf("FAIL last-user retirement=%lu\n",result);goto done;
    }
    {
        WCHAR screen[4096];DWORD chars;COORD origin={0,0};
        if(!ReadConsoleOutputCharacterW(GetStdHandle(STD_OUTPUT_HANDLE),screen,4095,origin,&chars))goto done;
        screen[chars]=0;
        if(!wcsstr(screen,L"FRONTEND-FINAL-37")){puts("FAIL missing final frame");goto done;}
    }
    error=frontend_bootstrap_start(image,&second);
    if(error || GetProcessId(second.process)==GetProcessId(retained)){
        printf("FAIL distinct frontend=%lu\n",error);goto done;
    }
    if(!creator_loss_case()){puts("FAIL creator loss lifecycle");goto done;}
    {
        HANDLE fake=CreateEventW(NULL,TRUE,FALSE,NULL),owner=NULL;
        if(!fake)goto done;
        error=OpenNtBaseClientRetainFrontendRoot(fake,&owner,&generation);CloseHandle(fake);
        if(owner)CloseHandle(owner);
        if(error!=ERROR_ACCESS_DENIED){printf("FAIL unauthenticated capability=%lu\n",error);goto done;}
    }
    TerminateProcess(broker.hProcess,123);WaitForSingleObject(broker.hProcess,5000);
    if(WaitForSingleObject(second.process,5000)!=WAIT_OBJECT_0 || !GetExitCodeProcess(second.process,&result) || result!=RPC_S_SERVER_UNAVAILABLE){
        printf("FAIL broker loss result=%lu\n",result);goto done;
    }
    if(WaitForSingleObject(second.process,5000)!=WAIT_OBJECT_0){puts("FAIL second frontend broker loss");goto done;}
    puts("PASS owner-only usage; final frame/exit 37; killed creator preserves target/frontend until target exit 41; normal retirement; distinct owner and broker loss");
    failed=0;
done:
    /* Fixture-owned processes only; no product execution tree termination. */
    if(target)CloseHandle(target);
    if(environment)FreeEnvironmentStringsW(environment);
    if(capability)CloseHandle(capability);
    if(connection.process)TerminateProcess(connection.process,ERROR_CANCELLED);
    frontend_bootstrap_release(&connection);
    if(second.process)TerminateProcess(second.process,ERROR_CANCELLED);
    frontend_bootstrap_release(&second);
    OpenNtBaseClientDisconnectCurrent();
    if(fixture_capability)CloseHandle(fixture_capability);
    if(fixture_execution)CloseHandle(fixture_execution);
    if(broker.hProcess){TerminateProcess(broker.hProcess,ERROR_CANCELLED);WaitForSingleObject(broker.hProcess,5000);CloseHandle(broker.hProcess);}
    if(retained){WaitForSingleObject(retained,5000);CloseHandle(retained);}
    HeapDestroy(CsrPortHeap);return failed;
}
