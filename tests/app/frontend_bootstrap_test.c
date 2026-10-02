/* Run on an isolated Console desktop with the sibling candidate BaseSrv.
 * This tests the real RPC path; it does not select the ordinary CLI route. */
#include <windows.h>
#include <stdio.h>
#include "interface/frontend_bootstrap.h"
#include "interface/native_request_protocol.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"

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
int wmain(int argc,WCHAR **argv)
{
    WCHAR image[MAX_PATH],broker_image[MAX_PATH],command[2*MAX_PATH];
    WCHAR *slash;
    STARTUPINFOW startup={sizeof(startup)}; PROCESS_INFORMATION broker={0};
    frontend_connection connection={0},second={0};
    HANDLE retained=NULL,capability=NULL,fixture_capability=NULL,fixture_execution=NULL;
    DWORD error=0,result=0,generation,i; int failed=1;
    if(argc==2 && !wcscmp(argv[1],L"--warm-create"))return 0;
    /* A deliberately silent, live bootstrap peer. Production timeout must
     * cancel its pending read and roll back this unaccepted process only. */
    if(argc==7 && !wcscmp(argv[1],L"--session"))return rejected_peer(
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
    CsrPortHeap=HeapCreate(0,0,0);if(!CsrPortHeap)return 1;
    /* Never terminate or use a broker that belongs to another test/user. */
    error=OpenNtBaseClientConnectCurrent();
    if(!error){puts("FAIL precondition: broker already running");goto done;}
    if(error!=RPC_S_SERVER_UNAVAILABLE){printf("FAIL precondition broker error=%lu\n",error);goto done;}
    if(!GetModuleFileNameW(NULL,image,MAX_PATH))goto done;
    slash=wcsrchr(image,L'\\');if(!slash)goto done;slash[1]=0;
    wcscpy_s(broker_image,MAX_PATH,image);wcscat_s(broker_image,MAX_PATH,L"ntsrv.exe");
    wcscat_s(image,MAX_PATH,L"ntkvm.exe");
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
    /* Native execution/final-frame/creator-loss tests use the actual NTW32
     * route in verify-frontend-lifetime and the ordinary Console/Window suites.
     * Bootstrap must not retain a second target executor in the frontend. */
    error=frontend_bootstrap_start(image,&second);
    if(error || GetProcessId(second.process)==GetProcessId(retained)){
        printf("FAIL distinct frontend=%lu\n",error);goto done;
    }
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
    if(WaitForSingleObject(retained,5000)!=WAIT_OBJECT_0 ||
        !GetExitCodeProcess(retained,&result) || result!=RPC_S_SERVER_UNAVAILABLE){
        puts("FAIL released frontend broker loss");goto done;
    }
    puts("PASS owner-only usage; retirement barrier; released creator capability preserves frontend; distinct authenticated owners; broker loss");
    failed=0;
done:
    /* Fixture-owned processes only; no product execution tree termination. */
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
