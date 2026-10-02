/* Product-facing BaseClient transport cut.  The original vdm.c call sites
 * retain their CSR-shaped signature; this focused client proves the selected
 * replacement reaches the real standalone BaseSrv first-VDM owner. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "ntsrv-exe/opennt/include/base_client.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include "worker-base/connection.h"
#include "ntvwm-exe/next_command.h"
#include "interface/native_request_client.h"
#include "interface/console_io.h"
#include <stddef.h>

/* BaseCheckForVDM shares its original client translation unit with
 * BaseCheckVDM, whose retained capture helpers require this local carrier. */
PVOID CsrPortHeap;

#define REQUIRE(value) do { if (!(value)) { \
    fprintf(stderr,"FAIL line %d\\n",__LINE__); return 1; } } while (0)

static NTSTATUS get_first_command(CHAR *returnedCommand,ULONG returnedBytes)
{
    BASE_API_MSG message={0};
    STARTUPINFOA startup={sizeof(startup)};
    NTSTATUS status;
    message.u.GetNextVDMCommand.VDMState=ASKING_FOR_FIRST_COMMAND;
    message.u.GetNextVDMCommand.CmdLine=returnedCommand;
    message.u.GetNextVDMCommand.CmdLen=returnedBytes;
    message.u.GetNextVDMCommand.StartupInfo=&startup;
    status=OpenNtBaseClientCallServer((PCSR_API_MSG)&message,NULL,
        CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepGetNextVDMCommand),
        sizeof(message.u.GetNextVDMCommand));
    if (status!=STATUS_SUCCESS || message.ReturnValue!=STATUS_SUCCESS) return status;
    if (message.u.GetNextVDMCommand.WaitObjectForVDM) {
        CloseHandle(message.u.GetNextVDMCommand.WaitObjectForVDM);
        return STATUS_UNSUCCESSFUL;
    }
    return STATUS_SUCCESS;
}

/* A pure worker reports completion through the original ordinary follow-up
 * GetNextVDMCommand shape.  ASKING_FOR_SECOND_TIME is specifically a pending
 * re-entry query; it searches a ready record rather than the busy worker. */
static NTSTATUS report_task_exit(void)
{
    BASE_API_MSG message={0};
    CHAR command[1024]={0};
    STARTUPINFOA startup={sizeof(startup)};
    message.u.GetNextVDMCommand.VDMState=0;
    message.u.GetNextVDMCommand.ExitCode=7;
    message.u.GetNextVDMCommand.CmdLine=command;
    message.u.GetNextVDMCommand.CmdLen=sizeof(command);
    message.u.GetNextVDMCommand.StartupInfo=&startup;
    return OpenNtBaseClientCallServer((PCSR_API_MSG)&message,NULL,
        CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepGetNextVDMCommand),
        sizeof(message.u.GetNextVDMCommand));
}

/* Exercise the exact original vdm.c special case used by cmdCreateProcess:
 * its ConsoleHandle is intentionally ignored by the standalone client and
 * resolved from this authenticated worker connection at BaseSrv. */
static NTSTATUS set_reenter_count(ULONG state)
{
    BASE_API_MSG message={0};
    message.u.SetReenterCount.ConsoleHandle=(HANDLE)(ULONG_PTR)0x1234u;
    message.u.SetReenterCount.fIncDec=state;
    return OpenNtBaseClientCallServer((PCSR_API_MSG)&message,NULL,
        CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepSetReenterCount),
        sizeof(message.u.SetReenterCount));
}

static int reservation_child(int wait_frontend)
{
    CHAR command[1024]={0};
    REQUIRE(OpenNtBaseClientConnectCurrent()==ERROR_SUCCESS);
    {
        HANDLE pipe=NULL,frontend=NULL,ready=NULL;
        DWORD generation=0,count=0;
        char data=0;
        if (wait_frontend) {
            CHAR name[96];
            HANDLE entered;
            _snprintf_s(name,sizeof(name),_TRUNCATE,"Local\\NTVDM-RPC-test-wait-%lu",GetCurrentProcessId());
            entered=OpenEventA(EVENT_MODIFY_STATE,FALSE,name);
            REQUIRE(entered && SetEvent(entered));
            CloseHandle(entered);
            if (wait_frontend==2) {
                REQUIRE(OpenNtBaseClientWaitFrontend(&pipe,&frontend,&generation,&ready)==ERROR_PROCESS_ABORTED);
                REQUIRE(!pipe && !frontend && !generation && !ready);
                OpenNtBaseClientDisconnectCurrent();
                puts("PASS: real RPC pending worker receives cancellation with no handles");
                return 0;
            }
            REQUIRE(OpenNtBaseClientWaitFrontend(&pipe,&frontend,&generation,&ready)==ERROR_SUCCESS);
        } else {
            REQUIRE(OpenNtBaseClientTakeFrontend(&pipe,&frontend,&generation,&ready)==ERROR_SUCCESS);
        }
        REQUIRE(ready && WaitForSingleObject(ready,0)==WAIT_OBJECT_0);
        REQUIRE(!SetEvent(ready) && GetLastError()==ERROR_ACCESS_DENIED);
        REQUIRE(pipe && frontend && generation && GetProcessId(frontend));
        {
            HANDLE capability=NULL,root=NULL;
            DWORD root_generation=0;
            REQUIRE(OpenNtBaseClientWorkerFrontendCapability(&capability)==ERROR_SUCCESS && capability);
            REQUIRE(!SetEvent(capability) && GetLastError()==ERROR_ACCESS_DENIED);
            REQUIRE(OpenNtBaseClientRetainFrontendRoot(capability,&root,&root_generation)==ERROR_SUCCESS);
            REQUIRE(GetProcessId(root)==GetProcessId(frontend) && root_generation==generation);
            CloseHandle(root);CloseHandle(capability);
        }
        REQUIRE(!TerminateProcess(frontend,99) && GetLastError()==ERROR_ACCESS_DENIED);
        REQUIRE(ReadFile(pipe,&data,1,&count,NULL) && count==1 && data=='F');
        REQUIRE(WriteFile(pipe,"W",1,&count,NULL) && count==1);
        CloseHandle(pipe);CloseHandle(frontend);CloseHandle(ready);
        REQUIRE(OpenNtBaseClientTakeFrontend(&pipe,&frontend,&generation,&ready)==ERROR_ALREADY_EXISTS);
        REQUIRE(!pipe && !frontend && !generation && !ready);
        REQUIRE(OpenNtBaseClientWaitFrontend(&pipe,&frontend,&generation,&ready)==ERROR_ALREADY_EXISTS);
        REQUIRE(!pipe && !frontend && !generation && !ready);
        puts("PASS: duplicate frontend wait returns terminal status through real RPC");
    }
    REQUIRE(get_first_command(command,sizeof(command))==STATUS_SUCCESS);
    REQUIRE(!lstrcmpA(command,"MEM\\r\\n"));
    REQUIRE(set_reenter_count(INCREMENT_REENTER_COUNT)==STATUS_SUCCESS);
    REQUIRE(report_task_exit()==STATUS_SUCCESS);
    /* The original ConsoleRecord returns its wait again when no command is
     * queued. Repeated typed delivery must preserve one live local handle. */
    {
        HANDLE first_wait=NULL;
        unsigned index;
        for (index=0;index<3;++index) {
            BASE_API_MSG message={0};
            NTSTATUS wait_status;
            STARTUPINFOA startup={sizeof(startup)};
            message.u.GetNextVDMCommand.VDMState=NO_PARENT_TO_WAKE;
            message.u.GetNextVDMCommand.StartupInfo=&startup;
            wait_status=OpenNtBaseClientCallServer((PCSR_API_MSG)&message,NULL,
                CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepGetNextVDMCommand),
                sizeof(message.u.GetNextVDMCommand));
            if (wait_status!=STATUS_SUCCESS)
                fprintf(stderr,"repeat %u status=%08lx last=%lu\n",index,(ULONG)wait_status,GetLastError());
            REQUIRE(wait_status==STATUS_SUCCESS);
            REQUIRE(message.u.GetNextVDMCommand.WaitObjectForVDM!=NULL);
            if (!first_wait) first_wait=message.u.GetNextVDMCommand.WaitObjectForVDM;
            REQUIRE(message.u.GetNextVDMCommand.WaitObjectForVDM==first_wait);
            REQUIRE(WaitForSingleObject(first_wait,0)==WAIT_TIMEOUT);
        }
    }
    REQUIRE(set_reenter_count(DECREMENT_REENTER_COUNT)==STATUS_SUCCESS);
    {
        CHAR name[96];
        HANDLE released;
        _snprintf_s(name,sizeof(name),_TRUNCATE,"Local\\NTVDM-RPC-test-release-%lu",GetCurrentProcessId());
        released=OpenEventA(SYNCHRONIZE,FALSE,name);
        REQUIRE(released && WaitForSingleObject(released,15000)==WAIT_OBJECT_0);
        CloseHandle(released);
    }
    OpenNtBaseClientDisconnectCurrent();
    puts("PASS: reserved worker claimed and received original command");
    return 0;
}

static int cancel_monitor(DWORD root_pid,DWORD worker_pid)
{
    char path[MAX_PATH],result='F';
    HANDLE root=OpenProcess(PROCESS_TERMINATE|SYNCHRONIZE,FALSE,root_pid);
    HANDLE worker=OpenProcess(PROCESS_TERMINATE|PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,worker_pid);
    HANDLE file;
    DWORD count,code;
    REQUIRE(GetEnvironmentVariableA("NTVDM_RPC_CANCEL_REPORT",path,sizeof(path)));
    if (root && worker && TerminateProcess(root,92) &&
        WaitForSingleObject(root,5000)==WAIT_OBJECT_0 &&
        WaitForSingleObject(worker,5000)==WAIT_OBJECT_0 &&
        GetExitCodeProcess(worker,&code) && code==0) result='P';
    if (worker) {
        if (WaitForSingleObject(worker,0)==WAIT_TIMEOUT) {
            TerminateProcess(worker,99);WaitForSingleObject(worker,5000);
        }
        CloseHandle(worker);
    }
    if (root) CloseHandle(root);
    file=CreateFileA(path,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
    REQUIRE(file!=INVALID_HANDLE_VALUE);
    REQUIRE(WriteFile(file,&result,1,&count,NULL) && count==1);
    CloseHandle(file);
    return result=='P' ? 0 : 1;
}

static int reservation_parent(int wait_frontend)
{
    BASE_API_MSG message={0};
    STARTUPINFOA startup={sizeof(startup)};
    PROCESS_INFORMATION child={0};
    CHAR command[]="MEM\\r\\n",image[MAX_PATH],childCommand[MAX_PATH+64];
    uint64_t reservation=0;
    NTSTATUS status;
    ULONG task;
    DWORD error,exit_code=STILL_ACTIVE;
    HANDLE parent_event,released,channel,ui,ready,capability;
    CHAR release_name[96];
    CHAR pipe_name[96],data=0;
    DWORD transferred=0,frontend_generation=0;
    REQUIRE(OpenNtBaseClientConnectCurrent()==ERROR_SUCCESS);
    capability=CreateEventW(NULL,TRUE,FALSE,NULL);REQUIRE(capability);
    REQUIRE(OpenNtBaseClientRegisterFrontendRoot(capability)==ERROR_SUCCESS);
    {
        HANDLE denied=NULL;
        REQUIRE(OpenNtBaseClientWorkerFrontendCapability(&denied)==ERROR_ACCESS_DENIED && !denied);
    }
    message.u.CheckVDM.ConsoleHandle=(HANDLE)1;
    message.u.CheckVDM.BinaryType=BINARY_TYPE_DOS;
    message.u.CheckVDM.CmdLine=command;
    message.u.CheckVDM.CmdLen=sizeof(command);
    startup.dwFlags=STARTF_USESTDHANDLES;
    startup.hStdInput=GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput=GetStdHandle(STD_OUTPUT_HANDLE);
    startup.hStdError=GetStdHandle(STD_ERROR_HANDLE);
    message.u.CheckVDM.StartupInfo=&startup;
    status=OpenNtBaseClientCallServer((PCSR_API_MSG)&message,NULL,
        CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepCheckVDM),sizeof(message.u.CheckVDM));
    REQUIRE(status==STATUS_SUCCESS && message.ReturnValue==STATUS_SUCCESS);
    task=message.u.CheckVDM.iTask;
    REQUIRE(OpenNtBaseClientReserveWorker(task,&reservation)==ERROR_SUCCESS);
    REQUIRE(GetModuleFileNameA(NULL,image,sizeof(image)) &&
        _snprintf_s(childCommand,sizeof(childCommand),_TRUNCATE,"\"%s\" %s",image,
            wait_frontend>=2 ? "--frontend-cancel-child" :
            wait_frontend ? "--frontend-wait-child" : "--reservation-child")>0);
    ZeroMemory(&startup,sizeof(startup));
    startup.cb=sizeof(startup);
    REQUIRE(CreateProcessA(NULL,childCommand,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&startup,&child));
    error=OpenNtBaseClientPrepareWorker(reservation,child.hProcess);
    if (error) {
        TerminateProcess(child.hProcess,error);
        CloseHandle(child.hThread);CloseHandle(child.hProcess);
        (void)OpenNtBaseClientReleaseWorker(reservation);
        OpenNtBaseClientDisconnectCurrent();
        return (int)error;
    }
    /* The original parent side, not the worker, registers the new VDM
     * process.  The RPC binding resolves this through the launch reservation,
     * then returns the original BaseSrv parent-completion event. */
    ZeroMemory(&message,sizeof(message));
    parent_event=child.hProcess;
    message.u.UpdateVDMEntry.EntryIndex=UPDATE_VDM_PROCESS_HANDLE;
    message.u.UpdateVDMEntry.iTask=task;
    message.u.UpdateVDMEntry.BinaryType=BINARY_TYPE_DOS;
    status=OpenNtBaseClientCallServer((PCSR_API_MSG)&message,NULL,
        CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepUpdateVDMEntry),
        sizeof(message.u.UpdateVDMEntry));
    REQUIRE(status==STATUS_SUCCESS && message.ReturnValue==STATUS_SUCCESS);
    parent_event=message.u.UpdateVDMEntry.WaitObjectForParent;
    REQUIRE(parent_event!=NULL && WaitForSingleObject(parent_event,0)==WAIT_TIMEOUT);
    _snprintf_s(pipe_name,sizeof(pipe_name),_TRUNCATE,"\\\\.\\pipe\\ntvdm-rpc-frontend-%lu",GetCurrentProcessId());
    channel=CreateNamedPipeA(pipe_name,PIPE_ACCESS_DUPLEX|FILE_FLAG_FIRST_PIPE_INSTANCE,
        PIPE_TYPE_BYTE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,1024,1024,0,NULL);
    REQUIRE(channel!=INVALID_HANDLE_VALUE);
    ui=CreateFileA(pipe_name,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
    REQUIRE(ui!=INVALID_HANDLE_VALUE);
    REQUIRE(ConnectNamedPipe(channel,NULL) || GetLastError()==ERROR_PIPE_CONNECTED);
    ready=CreateEventW(NULL,TRUE,TRUE,NULL);
    REQUIRE(ready);
    _snprintf_s(release_name,sizeof(release_name),_TRUNCATE,"Local\\NTVDM-RPC-test-release-%lu",child.dwProcessId);
    released=CreateEventA(NULL,TRUE,FALSE,release_name);
    REQUIRE(released);
    if (wait_frontend) {
        CHAR name[96];
        HANDLE entered;
        _snprintf_s(name,sizeof(name),_TRUNCATE,"Local\\NTVDM-RPC-test-wait-%lu",child.dwProcessId);
        entered=CreateEventA(NULL,TRUE,FALSE,name);
        REQUIRE(entered && ResumeThread(child.hThread)!=(DWORD)-1);
        REQUIRE(WaitForSingleObject(entered,5000)==WAIT_OBJECT_0);
        REQUIRE(WaitForSingleObject(child.hProcess,100)==WAIT_TIMEOUT);
        CloseHandle(entered);
    }
    if (wait_frontend) {
        DWORD request=0;
        HANDLE selected=NULL;
        REQUIRE(OpenNtBaseClientRequestFrontend(capability)==ERROR_SUCCESS);
        REQUIRE(WaitForSingleObject(capability,0)==WAIT_OBJECT_0);
        if (wait_frontend>=2) {
            DWORD outcome;
            if (wait_frontend==3) {
                PROCESS_INFORMATION monitor={0};
                REQUIRE(_snprintf_s(childCommand,sizeof(childCommand),_TRUNCATE,
                    "\"%s\" --frontend-cancel-monitor %lu %lu",image,
                    GetCurrentProcessId(),child.dwProcessId)>0);
                REQUIRE(CreateProcessA(NULL,childCommand,NULL,NULL,FALSE,CREATE_NO_WINDOW,
                    NULL,NULL,&startup,&monitor));
                CloseHandle(monitor.hThread);CloseHandle(monitor.hProcess);
                /* No Disconnect: the independent monitor kills this root. */
                Sleep(15000);
                return 1;
            }
            OpenNtBaseClientDisconnectCurrent();
            outcome=WaitForSingleObject(child.hProcess,5000);
            if (outcome!=WAIT_OBJECT_0) {
                TerminateProcess(child.hProcess,99);
                WaitForSingleObject(child.hProcess,5000);
            }
            REQUIRE(outcome==WAIT_OBJECT_0);
            REQUIRE(GetExitCodeProcess(child.hProcess,&error) && error==0);
            CloseHandle(child.hThread);CloseHandle(child.hProcess);
            CloseHandle(parent_event);CloseHandle(released);CloseHandle(capability);
            CloseHandle(ready);CloseHandle(channel);CloseHandle(ui);
            puts("PASS: root RPC disconnect cancels unattached original worker request");
            return 0;
        }
        REQUIRE(OpenNtBaseClientFrontendRequest(&request,&selected)==ERROR_SUCCESS && request);
        REQUIRE(GetProcessId(selected)==child.dwProcessId);CloseHandle(selected);
        REQUIRE(OpenNtBaseClientAttachFrontendRequest(request,channel,ready,&frontend_generation)==ERROR_SUCCESS);
        REQUIRE(frontend_generation);
        REQUIRE(OpenNtBaseClientFrontendRequest(&request,&selected)==ERROR_NOT_FOUND && !request && !selected);
        REQUIRE(WaitForSingleObject(capability,0)==WAIT_TIMEOUT);
    } else REQUIRE(OpenNtBaseClientAttachFrontend(channel,&frontend_generation,ready)==ERROR_SUCCESS && frontend_generation);
    REQUIRE(OpenNtBaseClientAttachFrontend(channel,&frontend_generation,ready)==ERROR_ALREADY_EXISTS && !frontend_generation);
    CloseHandle(ready);
    CloseHandle(channel);
    {
        HANDLE worker=NULL;
        DWORD flags=HANDLE_FLAG_INHERIT;
        REQUIRE(OpenNtBaseClientCommandWorker(&worker)==ERROR_SUCCESS && worker);
        REQUIRE(GetProcessId(worker)==child.dwProcessId);
        REQUIRE(GetHandleInformation(worker,&flags) && !(flags&HANDLE_FLAG_INHERIT));
        REQUIRE(!TerminateProcess(worker,123) && GetLastError()==ERROR_ACCESS_DENIED);
        REQUIRE(CloseHandle(worker));
    }
    if (!wait_frontend) REQUIRE(ResumeThread(child.hThread)!=(DWORD)-1);
    REQUIRE(WriteFile(ui,"F",1,&transferred,NULL) && transferred==1);
    REQUIRE(ReadFile(ui,&data,1,&transferred,NULL) && transferred==1 && data=='W');
    CloseHandle(ui);
    REQUIRE(WaitForSingleObject(parent_event,15000)==WAIT_OBJECT_0);
    {
    BOOL check_for_vdm=BaseCheckForVDM(parent_event,&exit_code);
    if (!check_for_vdm)
        fprintf(stderr,"BaseCheckForVDM failed: %lu\\n",GetLastError());
    if (exit_code!=7) fprintf(stderr,"BaseCheckForVDM returned: %lu\\n",exit_code);
    REQUIRE(check_for_vdm && exit_code==7);
    }
    REQUIRE(SetEvent(released));
    REQUIRE(WaitForSingleObject(child.hProcess,15000)==WAIT_OBJECT_0);
    REQUIRE(GetExitCodeProcess(child.hProcess,&error) && error==0);
    CloseHandle(released);
    CloseHandle(parent_event);
    CloseHandle(child.hThread);CloseHandle(child.hProcess);
    /* The retained process-exit watch may already have released this exact
     * reservation. Both orders must leave no reservation behind. */
    error=OpenNtBaseClientReleaseWorker(reservation);
    if (error!=ERROR_SUCCESS && error!=ERROR_NOT_FOUND)
        fprintf(stderr,"reservation release error=%lu\n",error);
    REQUIRE(error==ERROR_SUCCESS || error==ERROR_NOT_FOUND);
    REQUIRE(OpenNtBaseClientReleaseWorker(reservation)==ERROR_NOT_FOUND);
    OpenNtBaseClientDisconnectCurrent();
    CloseHandle(capability);
    puts(wait_frontend ? "PASS: delayed frontend attachment crosses real RPC with bounded resource rights" :
        "PASS: launcher reserved and registered a real worker process");
    return 0;
}

static int frontend_root_child(BOOL stale)
{
    char text[32];
    HANDLE capability,root=NULL,other;
    DWORD generation=0,pid,flags;
    REQUIRE(GetEnvironmentVariableA("NTVDM_RPC_TEST_CAPABILITY",text,sizeof(text)));
    capability=(HANDLE)(ULONG_PTR)strtoul(text,NULL,16);
    REQUIRE(GetEnvironmentVariableA("NTVDM_RPC_TEST_ROOT",text,sizeof(text)));
    pid=strtoul(text,NULL,10);
    REQUIRE(OpenNtBaseClientConnectCurrent()==ERROR_SUCCESS);
    if (stale) {
        REQUIRE(OpenNtBaseClientRetainFrontendRoot(capability,&root,&generation)==ERROR_ACCESS_DENIED);
        REQUIRE(!root && !generation);
        OpenNtBaseClientDisconnectCurrent();
        return 38;
    }
    other=CreateEventW(NULL,TRUE,FALSE,NULL);REQUIRE(other);
    REQUIRE(OpenNtBaseClientRetainFrontendRoot(other,&root,&generation)==ERROR_ACCESS_DENIED);
    REQUIRE(!root && !generation);
    CloseHandle(other);
    REQUIRE(OpenNtBaseClientRetainFrontendRoot(capability,&root,&generation)==ERROR_SUCCESS);
    REQUIRE(generation && GetProcessId(root)==pid);
    REQUIRE(GetHandleInformation(root,&flags) && !(flags&HANDLE_FLAG_INHERIT));
    REQUIRE(WaitForSingleObject(root,0)==WAIT_TIMEOUT);
    CloseHandle(root);
    OpenNtBaseClientDisconnectCurrent();
    return 37;
}

static int frontend_root_parent(void)
{
    HANDLE capability=CreateEventW(NULL,TRUE,FALSE,NULL),restricted=NULL,root=NULL;
    DWORD generation=0,result,index;
    char executable[MAX_PATH],command[MAX_PATH+64],text[32];
    STARTUPINFOA startup={sizeof(startup)};
    PROCESS_INFORMATION child={0};
    REQUIRE(capability);
    REQUIRE(DuplicateHandle(GetCurrentProcess(),capability,GetCurrentProcess(),
        &restricted,SYNCHRONIZE,TRUE,0));
    REQUIRE(OpenNtBaseClientConnectCurrent()==ERROR_SUCCESS);
    REQUIRE(OpenNtBaseClientRegisterFrontendRoot(capability)==ERROR_SUCCESS);
    REQUIRE(OpenNtBaseClientRegisterFrontendRoot(capability)==ERROR_ALREADY_EXISTS);
    REQUIRE(OpenNtBaseClientRetainFrontendRoot(restricted,&root,&generation)==ERROR_SUCCESS);
    REQUIRE(generation && GetProcessId(root)==GetCurrentProcessId());
    CloseHandle(root);
    sprintf_s(text,sizeof(text),"%lx",(ULONG)(ULONG_PTR)restricted);
    REQUIRE(SetEnvironmentVariableA("NTVDM_RPC_TEST_CAPABILITY",text));
    sprintf_s(text,sizeof(text),"%lu",GetCurrentProcessId());
    REQUIRE(SetEnvironmentVariableA("NTVDM_RPC_TEST_ROOT",text));
    REQUIRE(GetModuleFileNameA(NULL,executable,MAX_PATH));
    for (index=0;index<2;++index) {
        sprintf_s(command,sizeof(command),"\"%s\" %s",executable,
            index ? "--frontend-root-stale" : "--frontend-root-child");
        REQUIRE(CreateProcessA(NULL,command,NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,NULL,&startup,&child));
        CloseHandle(child.hThread);
        if (WaitForSingleObject(child.hProcess,10000)!=WAIT_OBJECT_0) {
            TerminateProcess(child.hProcess,99);
            WaitForSingleObject(child.hProcess,5000);
            CloseHandle(child.hProcess);
            return 1;
        }
        REQUIRE(GetExitCodeProcess(child.hProcess,&result));CloseHandle(child.hProcess);
        REQUIRE(result==(index ? 38u : 37u));
        if (!index) OpenNtBaseClientDisconnectCurrent();
    }
    CloseHandle(restricted);CloseHandle(capability);
    puts("PASS: real RPC root capability, independent-event rejection, descendant and rundown");
    return 0;
}

static int native_reservation_rpc(void)
{
    uint64_t reservation=0,duplicate=0;
    REQUIRE(OpenNtBaseClientConnectCurrent()==ERROR_SUCCESS);
    REQUIRE(OpenNtBaseClientReserveNativeWorker(NULL)==ERROR_INVALID_PARAMETER);
    REQUIRE(OpenNtBaseClientReserveNativeWorker(&reservation)==ERROR_SUCCESS && reservation);
    REQUIRE(OpenNtBaseClientReserveNativeWorker(&duplicate)==ERROR_INVALID_STATE && !duplicate);
    REQUIRE(OpenNtBaseClientReleaseWorker(reservation)==ERROR_SUCCESS);
    REQUIRE(OpenNtBaseClientReserveNativeWorker(&duplicate)==ERROR_SUCCESS && duplicate);
    REQUIRE(duplicate!=reservation);
    REQUIRE(OpenNtBaseClientReleaseWorker(reservation)!=ERROR_SUCCESS);
    REQUIRE(OpenNtBaseClientReleaseWorker(duplicate)==ERROR_SUCCESS);
    OpenNtBaseClientDisconnectCurrent();
    puts("PASS native reservation RPC: no frontend/guest command, duplicate rejection, release/reuse, stale identity rejection");
    return 0;
}


static int ntvwm_reuse_child(char **argv)
{
    HANDLE frontend=(HANDLE)(ULONG_PTR)strtoul(argv[2],NULL,16);
    HANDLE execution=(HANDLE)(ULONG_PTR)strtoul(argv[3],NULL,16);
    DWORD expected=(DWORD)strtoul(argv[4],NULL,10),code,request=0;
    HANDLE worker=NULL,target=NULL,receipt=NULL,foreign=CreateEventW(NULL,TRUE,FALSE,NULL);
    WCHAR native[MAX_PATH],directory[MAX_PATH],command[MAX_PATH+64],*environment;
    run16_native_start start={0};uint64_t reservation=0;
    REQUIRE(frontend && execution && foreign && !OpenNtBaseClientConnectCurrent());
    REQUIRE(OpenNtBaseClientBindConsoleContext(foreign)==ERROR_ACCESS_DENIED);
    CloseHandle(foreign);
    REQUIRE(!OpenNtBaseClientBindConsoleContext(execution));
    REQUIRE(OpenNtBaseClientCommandWorker(&worker)==ERROR_NOT_READY && !worker);
    REQUIRE(!OpenNtBaseClientSelectNativeWorker(&worker) && GetProcessId(worker)==expected);
    REQUIRE(OpenNtBaseClientReserveNativeWorker(&reservation)==ERROR_INVALID_STATE && !reservation);
    REQUIRE(GetSystemDirectoryW(native,MAX_PATH) && !wcscat_s(native,MAX_PATH,L"\\cmd.exe"));
    REQUIRE(GetCurrentDirectoryW(MAX_PATH,directory));
    REQUIRE(swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" /d /c exit /b 73",native)>0);
    environment=GetEnvironmentStringsW();REQUIRE(environment);
    start.application=native;start.command=command;start.directory=directory;
    start.environment=environment;start.console_mask=7;
    REQUIRE(!run16_native_request_submit(frontend,&start,&target,&receipt,&request));
    REQUIRE(WaitForSingleObject(target,5000)==WAIT_OBJECT_0);
    REQUIRE(GetExitCodeProcess(target,&code) && code==73);
    REQUIRE(WaitForSingleObject(receipt,5000)==WAIT_OBJECT_0);
    { DWORD completed=0; REQUIRE(!run16_native_request_finish(request,&code,&completed) && code==73 && completed==TRUE); }
    REQUIRE(WaitForSingleObject(worker,0)==WAIT_TIMEOUT);
    CloseHandle(target);CloseHandle(receipt);CloseHandle(worker);
    {
        WCHAR launcher[MAX_PATH],line[MAX_PATH*2+80],value[32],*slash;
        STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION process={0};DWORD wait;
        REQUIRE(GetModuleFileNameW(NULL,launcher,MAX_PATH));
        slash=wcsrchr(launcher,L'\\');REQUIRE(slash);
        REQUIRE(!wcscpy_s(slash+1,MAX_PATH-(size_t)(slash+1-launcher),L"run16.exe"));
        swprintf_s(value,32,L"%lx",(ULONG)(ULONG_PTR)frontend);
        REQUIRE(SetEnvironmentVariableW(L"NTVDM_FRONTEND_CAPABILITY",value));
        swprintf_s(value,32,L"%lx",(ULONG)(ULONG_PTR)execution);
        REQUIRE(SetEnvironmentVariableW(L"NTVDM_EXECUTION_CONSOLE",value));
        REQUIRE(swprintf_s(line,ARRAYSIZE(line),L"\"%ls\" \"%ls\" /d /c exit /b 61",launcher,native)>0);
        REQUIRE(CreateProcessW(launcher,line,NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,NULL,&startup,&process));
        CloseHandle(process.hThread);
        wait=WaitForSingleObject(process.hProcess,10000);
        if(wait!=WAIT_OBJECT_0){TerminateProcess(process.hProcess,99);WaitForSingleObject(process.hProcess,5000);}
        REQUIRE(GetExitCodeProcess(process.hProcess,&code));CloseHandle(process.hProcess);
        REQUIRE(wait==WAIT_OBJECT_0 && code==61);
        REQUIRE(SetEnvironmentVariableW(L"NTVDM_FRONTEND_CAPABILITY",NULL));
        REQUIRE(SetEnvironmentVariableW(L"NTVDM_EXECUTION_CONSOLE",NULL));
    }
    FreeEnvironmentStringsW(environment);OpenNtBaseClientDisconnectCurrent();
    CloseHandle(frontend);CloseHandle(execution);return 73;
}

static int ntvwm_reuse_launcher(HANDLE frontend,DWORD expected)
{
    HANDLE execution=NULL,inherited[2]={0};WCHAR image[MAX_PATH],command[MAX_PATH+120];
    STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION process={0};DWORD result,wait;
    REQUIRE(!OpenNtBaseClientAcquireConsoleContext(frontend,&execution));
    REQUIRE(DuplicateHandle(GetCurrentProcess(),frontend,GetCurrentProcess(),&inherited[0],SYNCHRONIZE,TRUE,0));
    REQUIRE(DuplicateHandle(GetCurrentProcess(),execution,GetCurrentProcess(),&inherited[1],SYNCHRONIZE,TRUE,0));
    REQUIRE(GetModuleFileNameW(NULL,image,MAX_PATH));
    REQUIRE(swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" --ntvwm-reuse-child %lx %lx %lu",
        image,(ULONG)(ULONG_PTR)inherited[0],(ULONG)(ULONG_PTR)inherited[1],expected)>0);
    REQUIRE(CreateProcessW(image,command,NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,NULL,&startup,&process));
    CloseHandle(process.hThread);CloseHandle(inherited[0]);CloseHandle(inherited[1]);CloseHandle(execution);
    wait=WaitForSingleObject(process.hProcess,10000);
    if(wait!=WAIT_OBJECT_0){TerminateProcess(process.hProcess,99);WaitForSingleObject(process.hProcess,5000);}
    REQUIRE(GetExitCodeProcess(process.hProcess,&result));CloseHandle(process.hProcess);
    REQUIRE(wait==WAIT_OBJECT_0 && result==73);return 0;
}

static int ntvwm_public_startup(void)
{
    HANDLE frontend=NULL,execution=NULL,inherited[2]={0},worker=NULL,shutdown=NULL;
    PROCESS_INFORMATION children[2]={{0}};
    STARTUPINFOW startup={sizeof(startup)};
    WCHAR image[MAX_PATH],native[MAX_PATH],line[MAX_PATH*2+80],value[32],*slash;
    DWORD index,wait,code;
    REQUIRE(!OpenNtBaseClientConnectCurrent());
    frontend=CreateEventW(NULL,TRUE,FALSE,NULL);
    REQUIRE(frontend && !OpenNtBaseClientRegisterFrontendRoot(frontend));
    REQUIRE(!OpenNtBaseClientAcquireConsoleContext(frontend,&execution));
    REQUIRE(OpenNtBaseClientSelectNativeWorker(&worker)==ERROR_NOT_FOUND && !worker);
    REQUIRE(DuplicateHandle(GetCurrentProcess(),frontend,GetCurrentProcess(),&inherited[0],SYNCHRONIZE,TRUE,0));
    REQUIRE(DuplicateHandle(GetCurrentProcess(),execution,GetCurrentProcess(),&inherited[1],SYNCHRONIZE,TRUE,0));
    REQUIRE(GetModuleFileNameW(NULL,image,MAX_PATH));
    slash=wcsrchr(image,L'\\');REQUIRE(slash);
    REQUIRE(!wcscpy_s(slash+1,MAX_PATH-(size_t)(slash+1-image),L"run16.exe"));
    REQUIRE(GetSystemDirectoryW(native,MAX_PATH) && !wcscat_s(native,MAX_PATH,L"\\cmd.exe"));
    swprintf_s(value,32,L"%lx",(ULONG)(ULONG_PTR)inherited[0]);
    REQUIRE(SetEnvironmentVariableW(L"NTVDM_FRONTEND_CAPABILITY",value));
    swprintf_s(value,32,L"%lx",(ULONG)(ULONG_PTR)inherited[1]);
    REQUIRE(SetEnvironmentVariableW(L"NTVDM_EXECUTION_CONSOLE",value));
    /* Both public launchers start without a worker. The production reservation
     * lock, not fixture serialization, must prevent duplicate NTVWM creation. */
    for(index=0;index<2;++index) {
        REQUIRE(swprintf_s(line,ARRAYSIZE(line),L"\"%ls\" \"%ls\" /d /c exit /b %lu",image,native,61ul+index)>0);
        REQUIRE(CreateProcessW(image,line,NULL,NULL,TRUE,CREATE_NO_WINDOW|CREATE_SUSPENDED,
            NULL,NULL,&startup,&children[index]));
    }
    REQUIRE(SetEnvironmentVariableW(L"NTVDM_FRONTEND_CAPABILITY",NULL));
    REQUIRE(SetEnvironmentVariableW(L"NTVDM_EXECUTION_CONSOLE",NULL));
    for(index=0;index<2;++index) {
        REQUIRE(ResumeThread(children[index].hThread)==1);
        CloseHandle(children[index].hThread);
    }
    for(index=0;index<2;++index) {
        wait=WaitForSingleObject(children[index].hProcess,10000);
        if(wait!=WAIT_OBJECT_0) {
            TerminateProcess(children[index].hProcess,99);
            WaitForSingleObject(children[index].hProcess,5000);
        }
        REQUIRE(GetExitCodeProcess(children[index].hProcess,&code));
        CloseHandle(children[index].hProcess);
        if(wait!=WAIT_OBJECT_0 || code!=61+index)
            fprintf(stderr,"native child %lu wait=%lu exit=%lu expected=%lu\n",
                (unsigned long)index,(unsigned long)wait,(unsigned long)code,61ul+index);
        REQUIRE(wait==WAIT_OBJECT_0 && code==61+index);
    }
    /* Selection rejects ambiguous workers. Successful selection after both
     * launchers exit proves one registered survivor for this execution context. */
    REQUIRE(!OpenNtBaseClientSelectNativeWorker(&worker));
    REQUIRE(WaitForSingleObject(worker,0)==WAIT_TIMEOUT);
    shutdown=OpenProcess(PROCESS_TERMINATE|SYNCHRONIZE,FALSE,GetProcessId(worker));
    REQUIRE(shutdown && TerminateProcess(shutdown,0));
    REQUIRE(WaitForSingleObject(shutdown,5000)==WAIT_OBJECT_0);
    CloseHandle(shutdown);CloseHandle(worker);
    CloseHandle(inherited[0]);CloseHandle(inherited[1]);CloseHandle(execution);CloseHandle(frontend);
    OpenNtBaseClientDisconnectCurrent();
    puts("PASS public run16 concurrent first startup: exact 61/62 results, unique resident NTVWM after launcher exit; fixture frontend only, presentation not tested");
    return 0;
}

static int ntvwm_execution_rpc(void)
{
    WCHAR image[MAX_PATH],command[MAX_PATH+64],directory[MAX_PATH],native[MAX_PATH];
    WCHAR *slash,*environment;
    HANDLE worker=NULL;DWORD error=0,code=0,deadline,index;
    HANDLE capability=NULL;
    REQUIRE(!OpenNtBaseClientConnectCurrent());
    {
        HANDLE selected=NULL;
        REQUIRE(OpenNtBaseClientSelectNativeWorker(&selected)==ERROR_NOT_FOUND && !selected);
    }
    REQUIRE(GetModuleFileNameW(NULL,image,MAX_PATH));
    slash=wcsrchr(image,L'\\');REQUIRE(slash);
    REQUIRE(!wcscpy_s(slash+1,MAX_PATH-(size_t)(slash+1-image),L"ntvwm.exe"));
    REQUIRE(swprintf_s(command,ARRAYSIZE(command),L"\"%ls\"",image)>0);
    capability=CreateEventW(NULL,TRUE,FALSE,NULL);
    REQUIRE(capability && !OpenNtBaseClientRegisterFrontendRoot(capability));
    /* This standalone fixture is its own frontend root. A direct launcher
     * may no longer publish the outer Console identity. */
    REQUIRE(!OpenNtBaseClientReportCurrentConsoleMembers());
    REQUIRE(!OpenNtBaseClientStartNativeWorker(&worker) && worker);
    REQUIRE(GetCurrentDirectoryW(MAX_PATH,directory));
    REQUIRE(GetSystemDirectoryW(native,MAX_PATH));
    REQUIRE(!wcscat_s(native,MAX_PATH,L"\\cmd.exe"));
    environment=GetEnvironmentStringsW();REQUIRE(environment);
    for(index=0;index<2;++index) {
        run16_native_start start={0};
        HANDLE output=NULL,writer=NULL,target=NULL,receipt=NULL;
        DWORD bytes=0,request=0;char text[128]={0},expected[32];
        REQUIRE(CreatePipe(&output,&writer,NULL,0));
        REQUIRE(swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" /d /c \"echo NTVWM-EXEC-%lu&exit /b %lu\"",
            native,index,index ? 19ul : 37ul)>0);
        start.application=native;start.command=command;start.directory=directory;
        start.environment=environment;start.console_mask=1;
        start.standard[1]=start.standard[2]=writer;
        deadline=GetTickCount()+5000;
        do {
            error=run16_native_request_submit(capability,&start,&target,&receipt,&request);
            if(error==ERROR_NOT_READY)Sleep(10);
        }while(error==ERROR_NOT_READY && (LONG)(deadline-GetTickCount())>0);
        if(error || !target || !receipt)
            fprintf(stderr,"native submit error=%lu target=%p receipt=%p worker=%lu\n",
                (unsigned long)error,target,receipt,(unsigned long)GetProcessId(worker));
        REQUIRE(!error && target && receipt);
        REQUIRE(GetProcessId(target)!=GetProcessId(worker));
        CloseHandle(writer);
        {
            DWORD target_wait=WaitForSingleObject(target,5000);
            if(target_wait!=WAIT_OBJECT_0) {
                DWORD target_code=MAXDWORD;
                (void)GetExitCodeProcess(target,&target_code);
                fprintf(stderr,"native target wait=%lu pid=%lu worker=%lu status=%lu\n",
                    (unsigned long)target_wait,(unsigned long)GetProcessId(target),
                    (unsigned long)GetProcessId(worker),(unsigned long)target_code);
            }
            REQUIRE(target_wait==WAIT_OBJECT_0);
        }
        REQUIRE(GetExitCodeProcess(target,&code) && code==(index ? 19u : 37u));
        REQUIRE(WaitForSingleObject(receipt,5000)==WAIT_OBJECT_0);
        { DWORD completed=0; REQUIRE(!run16_native_request_finish(request,&code,&completed) && code==(index ? 19u : 37u) && completed==TRUE); }
        sprintf_s(expected,sizeof(expected),"NTVWM-EXEC-%lu",index);
        REQUIRE(ReadFile(output,text,sizeof(text)-1,&bytes,NULL) && bytes && strstr(text,expected));
        REQUIRE(!ReadFile(output,text,sizeof(text)-1,&bytes,NULL) && GetLastError()==ERROR_BROKEN_PIPE);
        REQUIRE(WaitForSingleObject(worker,0)==WAIT_TIMEOUT);
        CloseHandle(output);CloseHandle(target);CloseHandle(receipt);
        if(!index) {
            WCHAR missing[MAX_PATH];HANDLE failed=NULL,failed_receipt=NULL;
            REQUIRE(!wcscpy_s(missing,MAX_PATH,image));
            slash=wcsrchr(missing,L'\\');REQUIRE(slash);
            REQUIRE(!wcscpy_s(slash+1,MAX_PATH-(size_t)(slash+1-missing),L"ntvwm-missing-target.exe"));
            REQUIRE(GetFileAttributesW(missing)==INVALID_FILE_ATTRIBUTES && GetLastError()==ERROR_FILE_NOT_FOUND);
            start.application=missing;start.console_mask=7;
            start.standard[0]=start.standard[1]=start.standard[2]=NULL;
            error=run16_native_request_submit(capability,&start,&failed,&failed_receipt,&request);
            REQUIRE(error==ERROR_FILE_NOT_FOUND && !failed && !failed_receipt);
            REQUIRE(WaitForSingleObject(worker,0)==WAIT_TIMEOUT);
        }
    }
    FreeEnvironmentStringsW(environment);
    REQUIRE(!ntvwm_reuse_launcher(capability,GetProcessId(worker)));
    REQUIRE(WaitForSingleObject(worker,0)==WAIT_TIMEOUT);
    {
        WCHAR name[128];HANDLE server,client,ready,selected=NULL;
        DWORD request_id=0,generation=0,bytes=0;
        console_io_request request;console_io_reply reply={0};
        /* Public run16 already requested this route before returning. Do not
         * mask a lost request by recreating it from the fixture connection. */
        REQUIRE(!OpenNtBaseClientFrontendRequest(&request_id,&selected));
        REQUIRE(GetProcessId(selected)==GetProcessId(worker));CloseHandle(selected);
        swprintf_s(name,ARRAYSIZE(name),L"\\\\.\\pipe\\ntvwm-runtime-presentation-%lu",GetCurrentProcessId());
        server=CreateNamedPipeW(name,PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED|FILE_FLAG_FIRST_PIPE_INSTANCE,
            PIPE_TYPE_BYTE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,32768,32768,0,NULL);
        REQUIRE(server!=INVALID_HANDLE_VALUE);
        client=CreateFileW(name,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
        REQUIRE(client!=INVALID_HANDLE_VALUE);
        REQUIRE(ConnectNamedPipe(server,NULL) || GetLastError()==ERROR_PIPE_CONNECTED);
        ready=CreateEventW(NULL,TRUE,FALSE,NULL);REQUIRE(ready);
        REQUIRE(!OpenNtBaseClientAttachFrontendRequest(request_id,server,ready,&generation));
        REQUIRE(ReadFile(client,&request,offsetof(console_io_request,data),&bytes,NULL) &&
            bytes==offsetof(console_io_request,data));
        REQUIRE(request.version==CONSOLE_IO_VERSION && request.generation==generation &&
            request.sequence==1 && request.operation==CONSOLE_IO_BARRIER && !request.bytes);
        reply.version=CONSOLE_IO_VERSION;reply.generation=generation;reply.sequence=1;
        /* Attached but inactive is legitimate. Receiving this reply must not
         * activate the frontend or terminate the resident worker. */
        reply.error=ERROR_NOT_READY;
        REQUIRE(WriteFile(client,&reply,offsetof(console_io_reply,data),&bytes,NULL) &&
            bytes==offsetof(console_io_reply,data));
        Sleep(200);REQUIRE(WaitForSingleObject(worker,0)==WAIT_TIMEOUT);
        CloseHandle(client);CloseHandle(server);CloseHandle(ready);
    }
    {
        DWORD pending=1,tasks=0;
        deadline=GetTickCount()+5000;
        do {
            REQUIRE(!OpenNtBaseClientFrontendUsage(&pending,&tasks));
            REQUIRE(!tasks);
            if(pending)Sleep(10);
        }while(pending && (LONG)(deadline-GetTickCount())>0);
        REQUIRE(!pending);
    }
    /* Fixture shutdown is explicit, not launcher-death or target completion. */
    {
        HANDLE terminate=OpenProcess(PROCESS_TERMINATE|SYNCHRONIZE,FALSE,GetProcessId(worker));
        REQUIRE(terminate && TerminateProcess(terminate,0) &&
            WaitForSingleObject(terminate,5000)==WAIT_OBJECT_0);
        CloseHandle(terminate);
    }
    CloseHandle(worker);CloseHandle(capability);
    OpenNtBaseClientDisconnectCurrent();
    puts("PASS actual NTVWM: independent entry, authenticated requests, exact 37/19 results, alias/EOF, failed launch/reuse; second launcher returns 73 and public run16 returns 61 through existing worker; forged context rejected; worker survives launcher");
    return 0;
}

int main(int argc,char **argv)
{
    BASE_API_MSG message={0};
    STARTUPINFOA startup={sizeof(startup)};
    CHAR command[]="MEM\r\n";
    CHAR returnedCommand[1024]={0},app[1024]={0},pif[1024]={0},directory[1024]={0};
    CHAR environment[1024]={0},desktop[1024]={0},title[1024]={0},reserved[1024]={0};
    HANDLE parent_event=NULL;
    NTSTATUS status;
    ULONG expected=argc==2 && !lstrcmpA(argv[1],"--existing")?0:1;
    if (argc==5 && !lstrcmpA(argv[1],"--ntvwm-reuse-child")) return ntvwm_reuse_child(argv);
    if (argc==2 && !lstrcmpA(argv[1],"--native-reservation")) return native_reservation_rpc();
    if (argc==2 && !lstrcmpA(argv[1],"--native-worker-startup")) return ntvwm_execution_rpc();
    if (argc==2 && !lstrcmpA(argv[1],"--ntvwm-execution")) return ntvwm_execution_rpc();
    if (argc==2 && !lstrcmpA(argv[1],"--ntvwm-public-startup")) return ntvwm_public_startup();
    if (argc==2 && !lstrcmpA(argv[1],"--reservation-child")) return reservation_child(FALSE);
    if (argc==2 && !lstrcmpA(argv[1],"--reservation-parent")) return reservation_parent(FALSE);
    if (argc==2 && !lstrcmpA(argv[1],"--frontend-cancel-child")) return reservation_child(2);
    if (argc==2 && !lstrcmpA(argv[1],"--frontend-cancel-parent")) return reservation_parent(2);
    if (argc==2 && !lstrcmpA(argv[1],"--frontend-cancel-killed-parent")) return reservation_parent(3);
    if (argc==4 && !lstrcmpA(argv[1],"--frontend-cancel-monitor"))
        return cancel_monitor(strtoul(argv[2],NULL,10),strtoul(argv[3],NULL,10));
    if (argc==2 && !lstrcmpA(argv[1],"--frontend-wait-child")) return reservation_child(TRUE);
    if (argc==2 && !lstrcmpA(argv[1],"--frontend-wait-parent")) return reservation_parent(TRUE);
    if (argc==2 && !lstrcmpA(argv[1],"--frontend-root-parent")) return frontend_root_parent();
    if (argc==2 && !lstrcmpA(argv[1],"--frontend-root-child")) return frontend_root_child(FALSE);
    if (argc==2 && !lstrcmpA(argv[1],"--frontend-root-stale")) return frontend_root_child(TRUE);
    REQUIRE(OpenNtBaseClientConnectCurrent()==ERROR_SUCCESS);
    status=OpenNtBaseClientCallServer((PCSR_API_MSG)&message,NULL,
        CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepIsFirstVDM),
        sizeof(message.u.IsFirstVDM));
    REQUIRE(status==STATUS_SUCCESS && message.ReturnValue==STATUS_SUCCESS);
    REQUIRE(message.u.IsFirstVDM.FirstVDM==expected);
    /* CheckVDM is sent as the existing copied command binding. Console mode
     * one is materialized by BaseSrv as its service-local identity. */
    message.u.CheckVDM.ConsoleHandle=(HANDLE)1;
    message.u.CheckVDM.BinaryType=BINARY_TYPE_DOS;
    message.u.CheckVDM.CmdLine=command;
    message.u.CheckVDM.CmdLen=sizeof(command);
    message.u.CheckVDM.StartupInfo=&startup;
    status=OpenNtBaseClientCallServer((PCSR_API_MSG)&message,NULL,
        CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepCheckVDM),sizeof(message.u.CheckVDM));
    if (status!=STATUS_SUCCESS || message.ReturnValue!=STATUS_SUCCESS)
        fprintf(stderr,"CheckVDM status=%08lx return=%08lx last=%lu\n",
            (ULONG)status,message.ReturnValue,GetLastError());
    REQUIRE(status==STATUS_SUCCESS && message.ReturnValue==STATUS_SUCCESS);
    REQUIRE(message.u.CheckVDM.VDMState==VDM_NOT_PRESENT);
    ZeroMemory(&message,sizeof(message));
    message.u.UpdateVDMEntry.EntryIndex=UPDATE_VDM_PROCESS_HANDLE;
    message.u.UpdateVDMEntry.BinaryType=BINARY_TYPE_DOS;
    status=OpenNtBaseClientCallServer((PCSR_API_MSG)&message,NULL,
        CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepUpdateVDMEntry),
        sizeof(message.u.UpdateVDMEntry));
    /* A process-handle registration is valid only after the launcher has
     * reserved and prepared this worker.  The old fixture fabricated the
     * CSR-local process carrier and therefore tested a path the product no
     * longer exposes.  Keep it as the explicit negative half; the
     * --reservation-parent route below proves the admitted positive path. */
    if (argc==2 && !lstrcmpA(argv[1],"--existing")) {
        REQUIRE(status==STATUS_UNSUCCESSFUL && message.ReturnValue==STATUS_UNSUCCESSFUL);
        REQUIRE(GetLastError()==ERROR_INVALID_PARAMETER);
        OpenNtBaseClientDisconnectCurrent();
        puts("PASS: unreserved process-handle registration is rejected");
        return 0;
    }
    if (status!=STATUS_SUCCESS || message.ReturnValue!=STATUS_SUCCESS)
        fprintf(stderr,"Update status=%08lx return=%08lx last=%lu\n",
            (ULONG)status,message.ReturnValue,GetLastError());
    REQUIRE(status==STATUS_SUCCESS && message.ReturnValue==STATUS_SUCCESS);
    parent_event=message.u.UpdateVDMEntry.WaitObjectForParent;
    REQUIRE(parent_event!=NULL);
    REQUIRE(WaitForSingleObject(parent_event,0)==WAIT_TIMEOUT);
    ZeroMemory(&message,sizeof(message));
    message.u.GetNextVDMCommand.VDMState=ASKING_FOR_FIRST_COMMAND;
    message.u.GetNextVDMCommand.CmdLine=returnedCommand;
    message.u.GetNextVDMCommand.CmdLen=sizeof(returnedCommand);
    message.u.GetNextVDMCommand.AppName=app;
    message.u.GetNextVDMCommand.AppLen=sizeof(app);
    message.u.GetNextVDMCommand.PifFile=pif;
    message.u.GetNextVDMCommand.PifLen=sizeof(pif);
    message.u.GetNextVDMCommand.CurDirectory=directory;
    message.u.GetNextVDMCommand.CurDirectoryLen=sizeof(directory);
    message.u.GetNextVDMCommand.Env=environment;
    message.u.GetNextVDMCommand.EnvLen=sizeof(environment);
    message.u.GetNextVDMCommand.Desktop=desktop;
    message.u.GetNextVDMCommand.DesktopLen=sizeof(desktop);
    message.u.GetNextVDMCommand.Title=title;
    message.u.GetNextVDMCommand.TitleLen=sizeof(title);
    message.u.GetNextVDMCommand.Reserved=reserved;
    message.u.GetNextVDMCommand.ReservedLen=sizeof(reserved);
    message.u.GetNextVDMCommand.StartupInfo=&startup;
    status=OpenNtBaseClientCallServer((PCSR_API_MSG)&message,NULL,
        CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepGetNextVDMCommand),
        sizeof(message.u.GetNextVDMCommand));
    if (status!=STATUS_SUCCESS || message.ReturnValue!=STATUS_SUCCESS)
        fprintf(stderr,"GetNext status=%08lx return=%08lx last=%lu\n",
            (ULONG)status,message.ReturnValue,GetLastError());
    REQUIRE(status==STATUS_SUCCESS && message.ReturnValue==STATUS_SUCCESS);
    REQUIRE(message.u.GetNextVDMCommand.WaitObjectForVDM==NULL);
    REQUIRE(!lstrcmpA(returnedCommand,command));
    ZeroMemory(&message,sizeof(message));
    message.u.GetNextVDMCommand.ExitCode=7;
    message.u.GetNextVDMCommand.StartupInfo=&startup;
    status=OpenNtBaseClientCallServer((PCSR_API_MSG)&message,NULL,
        CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepGetNextVDMCommand),
        sizeof(message.u.GetNextVDMCommand));
    REQUIRE(status==STATUS_SUCCESS && message.ReturnValue==STATUS_SUCCESS);
    REQUIRE(message.u.GetNextVDMCommand.WaitObjectForVDM!=NULL);
    REQUIRE(WaitForSingleObject(message.u.GetNextVDMCommand.WaitObjectForVDM,0)==WAIT_TIMEOUT);
    REQUIRE(WaitForSingleObject(parent_event,0)==WAIT_OBJECT_0);
    CloseHandle(message.u.GetNextVDMCommand.WaitObjectForVDM);
    CloseHandle(parent_event);
    OpenNtBaseClientDisconnectCurrent();
    puts("PASS: product BaseClient RPC Check, Update and GetNext wait-event route");
    return 0;
}
