/* Actual broker-owned Console bootstrap/return. Run with no existing broker
 * in an isolated Console; this fixture never terminates an unrelated service. */
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include "run16-exe/native_request_client.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include "common/codec/native_launch.h"

PVOID CsrPortHeap;
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
    if(!error) {
        pid=GetProcessId(owner);
        if(!pid || pid==GetCurrentProcessId() || WaitForSingleObject(owner,0)!=WAIT_TIMEOUT)error=ERROR_INVALID_DATA;
        else printf("FRONTEND-%ls=%lu\n",label,pid);
    }
    if(owner)CloseHandle(owner);
    OpenNtBaseClientDisconnectCurrent();HeapDestroy(CsrPortHeap);CsrPortHeap=NULL;
    return error ? 73 : 37;
}
static DWORD start_broker(PCWSTR image,PROCESS_INFORMATION *broker)
{
    STARTUPINFOW startup={sizeof(startup)};WCHAR command[MAX_PATH+3];DWORD error,i;
    swprintf_s(command,ARRAYSIZE(command),L"\"%ls\"",image);
    if(!CreateProcessW(image,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&startup,broker))return GetLastError();
    CloseHandle(broker->hThread);broker->hThread=NULL;
    for(i=0;i<100;++i) {
        error=OpenNtBaseClientConnectCurrent();if(!error)return 0;
        if(WaitForSingleObject(broker->hProcess,50)==WAIT_OBJECT_0)break;
    }
    return error;
}
static void stop_broker(PROCESS_INFORMATION *broker)
{
    OpenNtBaseClientDisconnectCurrent();
    if(broker->hProcess) {
        TerminateProcess(broker->hProcess,ERROR_CANCELLED);
        WaitForSingleObject(broker->hProcess,5000);CloseHandle(broker->hProcess);
    }
    ZeroMemory(broker,sizeof(*broker));
}
/* Test-only sibling provider. Production StartFrontend has no caller-selected
 * executable: the fixture broker lives in an isolated build package whose
 * trusted sibling ntcon.exe is this controlled adversarial executable. */
static int rejected_peer(HANDLE caller,HANDLE capability)
{
    WCHAR mode[24];HANDLE probe=NULL,remote=NULL;DWORD error;
    if(!GetEnvironmentVariableW(L"FRONTEND_TEST_REPLY",mode,ARRAYSIZE(mode))) {
        (void)WaitForSingleObject(caller,30000);return 74;
    }
    probe=CreateEventW(NULL,TRUE,FALSE,NULL);if(!probe)return 80;
    if(DuplicateHandle(GetCurrentProcess(),probe,caller,&remote,SYNCHRONIZE,FALSE,0) ||
        GetLastError()!=ERROR_ACCESS_DENIED){CloseHandle(probe);return 81;}
    CloseHandle(probe);
    CsrPortHeap=HeapCreate(0,0,0);if(!CsrPortHeap)return 82;
    error=OpenNtBaseClientConnectCurrent();
    if(!error && !wcscmp(mode,L"capability")) {
        probe=CreateEventW(NULL,TRUE,FALSE,NULL);
        if(!probe)error=GetLastError();
        else {
            error=OpenNtBaseClientFrontendStartupResult(probe,ERROR_ACCESS_DENIED);
            if(error!=ERROR_ACCESS_DENIED) {
                (void)OpenNtBaseClientFrontendStartupResult(capability,ERROR_INVALID_DATA);
                error=ERROR_INVALID_DATA;
            }
            CloseHandle(probe);
        }
    } else if(!error) {
        /* Failure may be reported before registration; success may not.
         * Version/application mismatch is covered by the real RPC Connect
         * tests, not by a now-removed pipe reply version field. */
        error=OpenNtBaseClientFrontendStartupResult(capability,
            !wcscmp(mode,L"status") ? ERROR_ACCESS_DENIED : ERROR_SUCCESS);
        if(!wcscmp(mode,L"unregistered") && error!=ERROR_ACCESS_DENIED) {
            (void)OpenNtBaseClientFrontendStartupResult(capability,ERROR_INVALID_DATA);
            error=ERROR_INVALID_DATA;
        }
    }
    OpenNtBaseClientDisconnectCurrent();HeapDestroy(CsrPortHeap);CsrPortHeap=NULL;
    CloseHandle(caller);CloseHandle(capability);return (int)error;
}
static int startup_failures(void)
{
    static const WCHAR *modes[]={L"status",L"unregistered",L"capability"};
    WCHAR self[MAX_PATH],source[MAX_PATH],package[MAX_PATH],image[MAX_PATH],*slash;
    PROCESS_INFORMATION broker={0};HANDLE root=NULL,cap=NULL,restored=NULL;
    DWORD error,i,before=0,after=0;int failed=1;
    if(!GetModuleFileNameW(NULL,self,ARRAYSIZE(self)) || !wcsstr(self,L"\\build\\"))return 84;
    wcscpy_s(source,ARRAYSIZE(source),self);slash=wcsrchr(source,L'\\');if(!slash)return 84;slash[1]=0;
    swprintf_s(package,ARRAYSIZE(package),L"%lsbootstrap-rejections",source);
    if(!CreateDirectoryW(package,NULL) && GetLastError()!=ERROR_ALREADY_EXISTS)return 84;
    /* The real service derives its package root from a system32 executable.
     * Keep the rejection-provider substitute at that same layout boundary. */
    wcscat_s(package,ARRAYSIZE(package),L"\\system32");
    if(!CreateDirectoryW(package,NULL) && GetLastError()!=ERROR_ALREADY_EXISTS)return 84;
    swprintf_s(image,ARRAYSIZE(image),L"%ls\\ntcon.exe",package);
    if(!CopyFileW(self,image,FALSE))return 84;
    wcscat_s(source,ARRAYSIZE(source),L"ntsrv.exe");
    swprintf_s(image,ARRAYSIZE(image),L"%ls\\ntsrv.exe",package);
    if(!CopyFileW(source,image,FALSE))return 84;
    CsrPortHeap=HeapCreate(0,0,0);if(!CsrPortHeap)return 1;
    error=OpenNtBaseClientConnectCurrent();
    if(error!=RPC_S_SERVER_UNAVAILABLE){printf("FAIL existing broker/status=%lu\n",error);goto done;}
    for(i=0;i<ARRAYSIZE(modes);++i) {
        ULONGLONG began,elapsed;
        SetEnvironmentVariableW(L"FRONTEND_TEST_REPLY",modes[i]);
        error=start_broker(image,&broker);if(error)goto done;
        /* Warm the new RPC marshalling path independently of the rejection. */
        (void)OpenNtBaseClientWaitFrontendConsoleRestored();
        if(!GetProcessHandleCount(GetCurrentProcess(),&before))goto done;
        began=GetTickCount64();
        error=OpenNtBaseClientStartFrontend((uint64_t)(UINT_PTR)GetConsoleWindow(),TRUE,&root,&cap,&restored);
        elapsed=GetTickCount64()-began;
        if(!GetProcessHandleCount(GetCurrentProcess(),&after) || before!=after || root || cap || restored ||
            error!=ERROR_ACCESS_DENIED) {
            printf("FAIL rejection %lu error=%lu elapsed=%llu handles=%lu/%lu\n",i,error,elapsed,before,after);goto done;
        }
        printf("PASS broker-owned rejection %ls error=%lu no local handle leak\n",modes[i],error);
        stop_broker(&broker);
    }
    failed=0;
done:
    stop_broker(&broker);SetEnvironmentVariableW(L"FRONTEND_TEST_REPLY",NULL);
    if(root)CloseHandle(root);if(cap)CloseHandle(cap);if(restored)CloseHandle(restored);
    HeapDestroy(CsrPortHeap);CsrPortHeap=NULL;return failed;
}
static DWORD parent_pid(DWORD pid)
{
    HANDLE snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    PROCESSENTRY32W entry={sizeof(entry)};DWORD parent=0;
    if(snapshot==INVALID_HANDLE_VALUE)return 0;
    if(Process32FirstW(snapshot,&entry))do {
        if(entry.th32ProcessID==pid){parent=entry.th32ParentProcessID;break;}
    }while(Process32NextW(snapshot,&entry));
    CloseHandle(snapshot);return parent;
}
static DWORD native_worker_result(HANDLE worker,HANDLE capability,BOOL completed,BOOL kill_worker,DWORD expected_io)
{
    WCHAR image[MAX_PATH],command[MAX_PATH+16],directory[MAX_PATH];
    LPWCH environment=NULL;run16_native_start start={0};
    HANDLE target=NULL,receipt=NULL,control=NULL;
    DWORD request=0,error,result=0,target_completed=99;
    if(!GetEnvironmentVariableW(L"COMSPEC",image,ARRAYSIZE(image)) ||
        !GetCurrentDirectoryW(ARRAYSIZE(directory),directory)) {error=GetLastError();goto done;}
    environment=GetEnvironmentStringsW();if(!environment){error=GetLastError();goto done;}
    swprintf_s(command,ARRAYSIZE(command),completed ? L"\"%ls\" /d /c exit 37" : L"\"%ls\" /d /k",image);
    start.application=image;start.command=command;start.directory=directory;
    start.environment=environment;start.console_mask=7;
    /* Same one-shot admission as Run16: NTSRV owns readiness and I/O binding. */
    error=run16_native_request_submit(capability,&start,&target,&receipt,&request);
    if(error)goto done;
    if(!target || !receipt || !request || (!completed && WaitForSingleObject(target,0)!=WAIT_TIMEOUT))
        {error=ERROR_INVALID_DATA;goto done;}
    if(completed && WaitForSingleObject(receipt,10000)!=WAIT_OBJECT_0){error=ERROR_TIMEOUT;goto done;}
    if(kill_worker) {
        control=OpenProcess(PROCESS_TERMINATE|SYNCHRONIZE,FALSE,GetProcessId(worker));
        if(!control || !TerminateProcess(control,123)){error=GetLastError();goto done;}
    }
    if(WaitForSingleObject(receipt,10000)!=WAIT_OBJECT_0){error=ERROR_TIMEOUT;goto done;}
    if(control && WaitForSingleObject(control,5000)!=WAIT_OBJECT_0){error=ERROR_TIMEOUT;goto done;}
    /* The receipt is now broker-owned, not inferred from the killed process.
     * A wrong request cannot consume the latched result. */
    error=run16_native_request_finish(request+1,&result,&target_completed);
    if(error!=ERROR_NOT_FOUND || target_completed || result){error=ERROR_INVALID_DATA;goto done;}
    error=run16_native_request_finish(request,&result,&target_completed);
    printf("native finish: status=%lu result=%lu target-completed=%lu\n",
        error,result,target_completed);
    if(!kill_worker && error!=expected_io){error=ERROR_INVALID_DATA;goto done;}
    /* A broker-latched completion remains consumable after worker death.
     * A recorded final-I/O error cannot erase the actual result/return grant;
     * there is no final worker pipe reply to read after rundown. */
    if(completed ? (error && error!=expected_io && error!=ERROR_BROKEN_PIPE && error!=ERROR_PROCESS_ABORTED) || result!=37 || target_completed!=TRUE :
        error!=ERROR_PROCESS_ABORTED || result || target_completed)
        {error=ERROR_INVALID_DATA;goto done;}
    error=run16_native_request_finish(request,&result,&target_completed);
    if(error!=ERROR_NOT_FOUND || target_completed || result){error=ERROR_INVALID_DATA;goto done;}
    puts(!kill_worker ? "PASS real native command completed; worker authenticated before service-loss injection" : completed ?
        "PASS actual target result 37 retained across worker rundown; wrong receipt denied; result consumed once" :
        "PASS broker signals native worker failure 1067; wrong receipt denied; result consumed once; no dead-channel read");
    error=0;
done:
    /* Fixture cleanup only; never a product target/tree termination policy. */
    if(target){
        if(WaitForSingleObject(target,5000)!=WAIT_OBJECT_0) {
            HANDLE cleanup=OpenProcess(PROCESS_TERMINATE,FALSE,GetProcessId(target));
            if(cleanup){TerminateProcess(cleanup,ERROR_CANCELLED);CloseHandle(cleanup);}
        }
        CloseHandle(target);
    }
    if(control)CloseHandle(control);if(receipt)CloseHandle(receipt);
    if(environment)FreeEnvironmentStringsW(environment);
    return error;
}
/* Test-only authenticated sibling, never installed. The real service creates
 * this worker from its reservation. Only final I/O is injected; target binding,
 * startup, completion and one-shot result consumption use production RPC. */
static int result_worker(void)
{
    BYTE *payload=NULL;WCHAR *strings[4];run16_native_launch_packet packet;
    HANDLE caller=NULL,execution=NULL,frontend=NULL,receipt=NULL;
    STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION target={0};
    DWORD error,bytes=0,request=0,generation=0,exit_code=0;
    payload=HeapAlloc(GetProcessHeap(),0,NATIVE_LAUNCH_MAX_BYTES);
    if(!payload)return ERROR_NOT_ENOUGH_MEMORY;
    error=OpenNtBaseClientConnectCurrent();
    if(!error)error=OpenNtBaseClientRegisterNativeBackend(NULL);
    if(!error)error=OpenNtBaseClientGetNextNativeCommand(NATIVE_LAUNCH_MAX_BYTES,payload,
        &bytes,&caller,&execution,&frontend,&request,&generation);
    if(!error)error=run16_native_launch_unpack(payload,bytes,&packet,strings);
    if(!error) {
        receipt=CreateEventW(NULL,TRUE,FALSE,NULL);
        if(!receipt)error=GetLastError();
    }
    if(!error && !CreateProcessW(strings[0],strings[1],NULL,NULL,FALSE,CREATE_SUSPENDED,
        NULL,NULL,&startup,&target))error=GetLastError();
    if(!error)error=OpenNtBaseClientBindNativeTarget(request,target.hProcess,receipt);
    if(!error && ResumeThread(target.hThread)==(DWORD)-1)error=GetLastError();
    if(!error)error=OpenNtBaseClientNativeStartupResult(generation,request,0,target.hProcess,receipt);
    if(!error && WaitForSingleObject(target.hProcess,10000)!=WAIT_OBJECT_0)error=ERROR_TIMEOUT;
    if(!error && !GetExitCodeProcess(target.hProcess,&exit_code))error=GetLastError();
    if(!error)error=OpenNtBaseClientCompleteNativeRequest(request,exit_code,ERROR_WRITE_FAULT,0);
    if(target.hProcess) {
        if(WaitForSingleObject(target.hProcess,0)!=WAIT_OBJECT_0) {
            TerminateProcess(target.hProcess,ERROR_CANCELLED);WaitForSingleObject(target.hProcess,10000);
        }
        CloseHandle(target.hProcess);
    }
    if(target.hThread)CloseHandle(target.hThread);
    if(caller)CloseHandle(caller);if(execution)CloseHandle(execution);
    if(frontend)CloseHandle(frontend);if(receipt)CloseHandle(receipt);
    HeapFree(GetProcessHeap(),0,payload);OpenNtBaseClientDisconnectCurrent();return (int)error;
}
static int native_io_error(void)
{
    WCHAR self[MAX_PATH],source[MAX_PATH],directory[MAX_PATH],image[MAX_PATH],*slash;
    PROCESS_INFORMATION broker={0};HANDLE worker=NULL,root=NULL,capability=NULL,restored=NULL;
    DWORD error;const char *stage="connect";int failed=1;
    if(!GetModuleFileNameW(NULL,self,ARRAYSIZE(self)) || !wcsstr(self,L"\\build\\"))return 84;
    wcscpy_s(source,ARRAYSIZE(source),self);slash=wcsrchr(source,L'\\');if(!slash)return 84;slash[1]=0;
    swprintf_s(directory,ARRAYSIZE(directory),L"%lsnative-result",source);
    if(!CreateDirectoryW(directory,NULL))return 84;
    wcscat_s(directory,ARRAYSIZE(directory),L"\\system32");
    if(!CreateDirectoryW(directory,NULL))return 84;
    swprintf_s(image,ARRAYSIZE(image),L"%ls\\ntvwm.exe",directory);
    if(!CopyFileW(self,image,TRUE))return 84;
    wcscat_s(source,ARRAYSIZE(source),L"ntcon.exe");
    swprintf_s(image,ARRAYSIZE(image),L"%ls\\ntcon.exe",directory);
    if(!CopyFileW(source,image,TRUE))return 84;
    slash=wcsrchr(source,L'\\');wcscpy_s(slash+1,ARRAYSIZE(source)-(size_t)(slash+1-source),L"ntsrv.exe");
    swprintf_s(image,ARRAYSIZE(image),L"%ls\\ntsrv.exe",directory);
    if(!CopyFileW(source,image,TRUE))return 84;
    CsrPortHeap=HeapCreate(0,0,0);if(!CsrPortHeap)return 1;
    error=OpenNtBaseClientConnectCurrent();
    if(error!=RPC_S_SERVER_UNAVAILABLE)goto done;
    error=start_broker(image,&broker);if(error)goto done;
    stage="frontend";
    error=OpenNtBaseClientStartFrontend((uint64_t)(UINT_PTR)GetConsoleWindow(),TRUE,&root,&capability,&restored);
    if(error)goto done;
    stage="worker";
    error=OpenNtBaseClientStartNativeWorker(OPENNT_BASE_NATIVE_LAUNCH_WIN32_CUI,&worker);
    if(!error){stage="result";error=native_worker_result(worker,capability,TRUE,FALSE,ERROR_WRITE_FAULT);}
    if(!error){puts("PASS actual RPC I/O-error reply retains exit 37/completed; consumed exactly once");failed=0;}
done:
    printf("native I/O-error fixture stage=%s status=%lu\n",stage,error);
    stop_broker(&broker);
    if(worker){
        if(WaitForSingleObject(worker,10000)!=WAIT_OBJECT_0)failed=1;
        CloseHandle(worker);
    }
    if(root){if(WaitForSingleObject(root,10000)!=WAIT_OBJECT_0)failed=1;CloseHandle(root);}
    if(capability)CloseHandle(capability);if(restored)CloseHandle(restored);
    HeapDestroy(CsrPortHeap);CsrPortHeap=NULL;return failed;
}
int wmain(int argc,WCHAR **argv)
{
    WCHAR image[MAX_PATH],command[MAX_PATH+3],*slash;
    STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION broker={0};
    HANDLE root=NULL,capability=NULL,restored=NULL,fake=NULL,owner=NULL,worker=NULL;
    DWORD error,result=0,generation=0,i;int failed=1;
    if(argc==2 && !wcscmp(argv[1],L"--cui-worker"))return result_worker();
    if(argc==2 && !wcscmp(argv[1],L"--native-io-error"))return native_io_error();
    if(argc==8 && !wcscmp(argv[1],L"--session"))return rejected_peer(
        (HANDLE)(UINT_PTR)wcstoul(argv[2],NULL,16),(HANDLE)(UINT_PTR)wcstoul(argv[3],NULL,16));
    if(argc==3 && !wcscmp(argv[1],L"--identity"))return identity_role(argv[2]);
    if(argc==2 && !wcscmp(argv[1],L"--startup-rejections"))return startup_failures();
    CsrPortHeap=HeapCreate(0,0,0);if(!CsrPortHeap)return 1;
    error=OpenNtBaseClientConnectCurrent();
    if(error!=RPC_S_SERVER_UNAVAILABLE){printf("FAIL existing broker/status=%lu\n",error);goto done;}
    if(!GetModuleFileNameW(NULL,image,ARRAYSIZE(image)) || !(slash=wcsrchr(image,L'\\')))goto done;
    if(wcscpy_s(slash+1,ARRAYSIZE(image)-(size_t)(slash+1-image),L"ntsrv.exe"))goto done;
    swprintf_s(command,ARRAYSIZE(command),L"\"%ls\"",image);
    if(!CreateProcessW(image,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&startup,&broker)) {
        printf("FAIL broker CreateProcess=%lu image=%ls\n",GetLastError(),image);goto done;
    }
    CloseHandle(broker.hThread);broker.hThread=NULL;
    /* Test readiness only: production admission is blocking RPC. */
    for(i=0;i<100;++i) {
        error=OpenNtBaseClientConnectCurrent();if(!error)break;
        if(WaitForSingleObject(broker.hProcess,50)==WAIT_OBJECT_0)break;
    }
    if(error){printf("FAIL connect=%lu\n",error);goto done;}
    {
        static const WCHAR empty_environment[2]={0,0};
        static const WCHAR malformed_environment[2]={L'X',L'X'};
        HANDLE denied_worker=NULL,denied_parent=NULL;
        error=OpenNtBaseClientStartVdmWorker(empty_environment,2,SW_HIDE,NULL,
            &denied_worker,&denied_parent);
        if(error!=ERROR_INVALID_STATE || denied_worker || denied_parent) {
            puts("FAIL VDM creation without original Check accepted");goto done;
        }
        error=OpenNtBaseClientStartVdmWorker(malformed_environment,2,SW_HIDE,NULL,
            &denied_worker,&denied_parent);
        if(error!=ERROR_INVALID_PARAMETER || denied_worker || denied_parent) {
            puts("FAIL malformed VDM environment accepted");goto done;
        }
        puts("PASS broker VDM creation requires original Check; malformed environment denied; no exported handles");
    }
    error=OpenNtBaseClientStartFrontend(0,TRUE,NULL,&capability,&restored);
    if(error!=ERROR_INVALID_PARAMETER || root || capability || restored){puts("FAIL invalid output accepted");goto done;}
    error=OpenNtBaseClientWaitFrontendConsoleRestored();
    if(error){puts("FAIL request without a Console lease required a return");goto done;}
    /* Missing HWND metadata must still use the authenticated creator and
     * process/capability admission, not be rejected as a missing Console. */
    error=OpenNtBaseClientStartFrontend(0,TRUE,
        &root,&capability,&restored);
    if(error){printf("FAIL bootstrap=%lu\n",error);goto done;}
    if(parent_pid(GetProcessId(root))!=broker.dwProcessId){puts("FAIL frontend not created by broker");goto done;}
    error=OpenNtBaseClientRetainFrontendRoot(capability,&owner,&generation);
    if(error || GetProcessId(owner)!=GetProcessId(root)){puts("FAIL authenticated root identity");goto done;}
    CloseHandle(owner);owner=NULL;
    fake=CreateEventW(NULL,TRUE,FALSE,NULL);if(!fake)goto done;
    error=OpenNtBaseClientRegisterFrontendRoot(fake);
    if(error!=ERROR_ACCESS_DENIED){puts("FAIL uncreated root registered");goto done;}
    error=OpenNtBaseClientRetainFrontendRoot(fake,&owner,&generation);
    if(error!=ERROR_ACCESS_DENIED || owner){puts("FAIL forged root capability accepted");goto done;}
    error=OpenNtBaseClientFrontendConsoleRestored();
    if(error!=ERROR_ACCESS_DENIED){puts("FAIL launcher forged restoration");goto done;}
    error=OpenNtBaseClientWaitFrontendConsoleRestored();
    if(error){printf("FAIL broker return barrier=%lu\n",error);goto done;}
    if(WaitForSingleObject(restored,0)!=WAIT_OBJECT_0){puts("FAIL missing broker restoration acknowledgement");goto done;}
    /* Reacquire the same root: its shared restoration event resets. The old
     * invocation's confirmation above must not depend on root process exit. */
    if(WaitForSingleObject(root,0)!=WAIT_TIMEOUT){puts("FAIL return killed resident frontend");goto done;}
    if(argc==2 && !wcscmp(argv[1],L"--workerless-park")) {
        if(WaitForSingleObject(root,11000)!=WAIT_TIMEOUT) {
            puts("FAIL live workerless root expired");goto done;
        }
        puts("PASS workerless root remains parked beyond the removed expiry");
        failed=0;goto done;
    }
    error=OpenNtBaseClientStartNativeWorker(OPENNT_BASE_NATIVE_LAUNCH_WIN32_CUI,&worker);
    if(error || !worker || parent_pid(GetProcessId(worker))!=broker.dwProcessId) {
        printf("FAIL native worker creation/parent=%lu\n",error);goto done;
    }
    puts("PASS authenticated service-created native worker; launcher does not create worker");
    if(argc==2 && (!wcscmp(argv[1],L"--native-worker-failure") ||
        !wcscmp(argv[1],L"--native-completed-worker-loss"))) {
        error=native_worker_result(worker,capability,!wcscmp(argv[1],L"--native-completed-worker-loss"),TRUE,0);
        if(error){printf("FAIL native worker failure receipt=%lu\n",error);goto done;}
        failed=0;goto done;
    }
    if(argc==2 && !wcscmp(argv[1],L"--worker-loss-park")) {
        HANDLE control;
        if(WaitForSingleObject(root,11000)!=WAIT_TIMEOUT) {
            puts("FAIL live worker root expired");goto done;
        }
        control=OpenProcess(PROCESS_TERMINATE|SYNCHRONIZE,FALSE,GetProcessId(worker));
        if(!control)goto done;
        if(!TerminateProcess(control,123)){CloseHandle(control);goto done;}
        WaitForSingleObject(control,5000);CloseHandle(control);
        if(WaitForSingleObject(root,11000)!=WAIT_TIMEOUT) {
            puts("FAIL worker loss armed an unapproved root expiry");goto done;
        }
        puts("PASS live worker and subsequent worker loss preserve parked frontend without an expiry");
        failed=0;goto done;
    }
    /* StartNativeWorker returns after Resume, not after worker Connect.
     * Complete real work before testing loss of an established connection;
     * killing the server immediately tests a different startup failure. */
    error=native_worker_result(worker,capability,TRUE,FALSE,0);
    if(error){printf("FAIL worker ready command=%lu\n",error);goto done;}
    TerminateProcess(broker.hProcess,123);
    if(WaitForSingleObject(broker.hProcess,5000)!=WAIT_OBJECT_0 ||
        WaitForSingleObject(root,5000)!=WAIT_OBJECT_0 || !GetExitCodeProcess(root,&result) ||
        result!=ERROR_INVALID_HANDLE){printf("FAIL broker-loss result=%lu\n",result);goto done;}
    if(WaitForSingleObject(worker,5000)!=WAIT_OBJECT_0 || !GetExitCodeProcess(worker,&result) ||
        result!=ERROR_INVALID_HANDLE){printf("FAIL native worker broker-loss result=%lu\n",result);goto done;}
    puts("PASS broker parent; authenticated root; invalid Console/capability/ack denied; RPC return barrier; broker loss");
    failed=0;
done:
    OpenNtBaseClientDisconnectCurrent();
    if(broker.hProcess){TerminateProcess(broker.hProcess,ERROR_CANCELLED);WaitForSingleObject(broker.hProcess,5000);CloseHandle(broker.hProcess);}
    if(root){if(WaitForSingleObject(root,5000)!=WAIT_OBJECT_0)TerminateProcess(root,ERROR_CANCELLED);CloseHandle(root);}
    if(worker){if(WaitForSingleObject(worker,5000)!=WAIT_OBJECT_0)TerminateProcess(worker,ERROR_CANCELLED);CloseHandle(worker);}
    if(capability)CloseHandle(capability);if(restored)CloseHandle(restored);
    if(fake)CloseHandle(fake);if(owner)CloseHandle(owner);
    HeapDestroy(CsrPortHeap);return failed;
}
