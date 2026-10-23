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
#include "run16-exe/native_request_client.h"
#include "common/protocol/console_io.h"
#include "common/protocol/frontend_protocol.h"
#include "common/protocol/frontend_lifecycle.h"
#include "common/rpc/management.h"
#include "common/protocol/management.h"
#include <base_rpc_client_internal.h>
#include <base_rpc_client_launch.h>
#include "common/rpc/async_call.h"
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

/* A test-only ntvdm.exe copy is admitted by the real service startup path.
 * It exercises the original RPC facade, not guest execution or frontend I/O. */
static int admitted_vdm_child(void)
{
    CHAR command[1024]={0};WCHAR name[128];HANDLE released,first_wait=NULL;
    HANDLE lease_pipe=NULL,lease_frontend=NULL,lease_ready=NULL;
    BOOL io_fault=GetEnvironmentVariableW(L"NTVDM_RPC_TEST_IO_FAULT",name,ARRAYSIZE(name))!=0;
    BOOL console_exit=io_fault && !wcscmp(name,L"2");
    unsigned index;
    REQUIRE(GetEnvironmentVariableW(L"NTVDM_RPC_TEST_RELEASE",name,ARRAYSIZE(name)));
    released=OpenEventW(SYNCHRONIZE,FALSE,name);REQUIRE(released);
    REQUIRE(!OpenNtBaseClientConnectCurrent());
    REQUIRE(!OpenNtBaseClientWorkerIoTransition(WORKER_IO_ACQUIRE));
    {
        HANDLE pipe=NULL,frontend=NULL,ready=NULL,capability=NULL,root=NULL;
        DWORD generation=0,root_generation=0;
        REQUIRE(!OpenNtBaseClientWaitFrontend(&pipe,&frontend,&generation,&ready));
        REQUIRE(pipe && frontend && ready && generation);
        REQUIRE(!SetEvent(ready) && GetLastError()==ERROR_ACCESS_DENIED);
        REQUIRE(!TerminateProcess(frontend,99) && GetLastError()==ERROR_ACCESS_DENIED);
        REQUIRE(!OpenNtBaseClientWorkerFrontendCapability(&capability) && capability);
        REQUIRE(!SetEvent(capability) && GetLastError()==ERROR_ACCESS_DENIED);
        REQUIRE(!OpenNtBaseClientRetainFrontendRoot(capability,&root,&root_generation));
        REQUIRE(root_generation==generation && GetProcessId(root)==GetProcessId(frontend));
        CloseHandle(root);CloseHandle(capability);
        lease_pipe=pipe;lease_frontend=frontend;lease_ready=ready;
        pipe=frontend=ready=NULL;
        REQUIRE(OpenNtBaseClientTakeFrontend(&pipe,&frontend,&generation,&ready)==ERROR_ALREADY_EXISTS);
        REQUIRE(!pipe && !frontend && !generation && !ready);
        REQUIRE(OpenNtBaseClientWaitFrontend(&pipe,&frontend,&generation,&ready)==ERROR_ALREADY_EXISTS);
        REQUIRE(!pipe && !frontend && !generation && !ready);
    }
    REQUIRE(get_first_command(command,sizeof(command))==STATUS_SUCCESS);
    REQUIRE(!lstrcmpA(command,"MEM\\r\\n"));
    if(io_fault) {
        HANDLE close=NULL,shutdown=NULL,broker=NULL;
        REQUIRE(!OpenNtBaseClientWorkerConsoleCloseEvent(&close));
        REQUIRE(!OpenNtBaseClientWorkerShutdownEvent(&shutdown));
        REQUIRE(WaitForSingleObject(close,0)==WAIT_TIMEOUT);
        if(console_exit) {
            /* Keep I/O intact: only this authenticated request can initiate
             * the Console transaction. A duplicate must not restart it. */
            REQUIRE(!OpenNtBaseClientRequestConsoleExit());
            REQUIRE(!OpenNtBaseClientRequestConsoleExit());
        } else {
            /* Deliberate unauthorised EOF: the real NTCON reports the fault. */
            CloseHandle(lease_pipe);lease_pipe=NULL;
        }
        REQUIRE(WaitForSingleObject(close,10000)==WAIT_OBJECT_0);
        REQUIRE(WaitForSingleObject(shutdown,0)==WAIT_TIMEOUT);
        REQUIRE(!OpenNtBaseClientBrokerProcess(&broker));
        REQUIRE(WaitForSingleObject(broker,0)==WAIT_TIMEOUT);
        REQUIRE(WaitForSingleObject(lease_frontend,0)==WAIT_TIMEOUT);
        if(lease_pipe)CloseHandle(lease_pipe);
        CloseHandle(broker);CloseHandle(close);CloseHandle(shutdown);CloseHandle(lease_frontend);CloseHandle(lease_ready);
        CloseHandle(released);OpenNtBaseClientDisconnectCurrent();
        return 0; /* Test carrier honors the service's Console-close order. */
    }
    REQUIRE(set_reenter_count(INCREMENT_REENTER_COUNT)==STATUS_SUCCESS);
    REQUIRE(report_task_exit()==STATUS_SUCCESS);
    for(index=0;index<3;++index) {
        BASE_API_MSG message={0};STARTUPINFOA startup={sizeof(startup)};
        message.u.GetNextVDMCommand.VDMState=NO_PARENT_TO_WAKE;
        message.u.GetNextVDMCommand.StartupInfo=&startup;
        REQUIRE(OpenNtBaseClientCallServer((PCSR_API_MSG)&message,NULL,
            CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepGetNextVDMCommand),
            sizeof(message.u.GetNextVDMCommand))==STATUS_SUCCESS);
        REQUIRE(message.u.GetNextVDMCommand.WaitObjectForVDM);
        if(!first_wait)first_wait=message.u.GetNextVDMCommand.WaitObjectForVDM;
        REQUIRE(message.u.GetNextVDMCommand.WaitObjectForVDM==first_wait);
        REQUIRE(WaitForSingleObject(first_wait,0)==WAIT_TIMEOUT);
    }
    REQUIRE(set_reenter_count(DECREMENT_REENTER_COUNT)==STATUS_SUCCESS);
    {
        DWORD decision=WORKER_IO_KEEP;
        REQUIRE(!OpenNtBaseClientWorkerIoCheckpoint(WORKER_IO_CHECKPOINT_PAUSE,0,&decision));
        REQUIRE(decision==WORKER_IO_RELEASE);
        REQUIRE(!OpenNtBaseClientWorkerIoTransition(WORKER_IO_RELEASE_BEGIN));
        CloseHandle(lease_pipe);CloseHandle(lease_frontend);CloseHandle(lease_ready);
        REQUIRE(!OpenNtBaseClientWorkerIoTransition(WORKER_IO_RELEASED));
    }
    REQUIRE(WaitForSingleObject(released,10000)==WAIT_OBJECT_0);
    CloseHandle(released);
    OpenNtBaseClientDisconnectCurrent();
    return 0;
}

static int admitted_vdm_parent(unsigned io_fault)
{
    BASE_API_MSG message={0};STARTUPINFOA startup={sizeof(startup)};
    CHAR command[]="MEM\\r\\n";WCHAR name[128],*environment,*end;
    HANDLE root=NULL,capability=NULL,restored=NULL,worker=NULL,parent=NULL,released;
    DWORD error,code=STILL_ACTIVE;NTSTATUS status;
    swprintf_s(name,ARRAYSIZE(name),L"Local\\NTVDM-RPC-admitted-release-%lu",GetCurrentProcessId());
    released=CreateEventW(NULL,TRUE,FALSE,name);REQUIRE(released);
    REQUIRE(SetEnvironmentVariableW(L"NTVDM_RPC_TEST_RELEASE",name));
    REQUIRE(SetEnvironmentVariableW(L"NTVDM_RPC_TEST_IO_FAULT",io_fault==2 ? L"2" : io_fault ? L"1" : NULL));
    REQUIRE(!OpenNtBaseClientConnectCurrent());
    REQUIRE(!OpenNtBaseClientStartFrontend((uint64_t)(UINT_PTR)GetConsoleWindow(),TRUE,
        &root,&capability,&restored));
    message.u.CheckVDM.ConsoleHandle=(HANDLE)1;
    message.u.CheckVDM.BinaryType=BINARY_TYPE_DOS;
    message.u.CheckVDM.CmdLine=command;message.u.CheckVDM.CmdLen=sizeof(command);
    message.u.CheckVDM.StartupInfo=&startup;
    status=OpenNtBaseClientCallServer((PCSR_API_MSG)&message,NULL,
        CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepCheckVDM),sizeof(message.u.CheckVDM));
    REQUIRE(status==STATUS_SUCCESS && message.ReturnValue==STATUS_SUCCESS);
    environment=GetEnvironmentStringsW();REQUIRE(environment);
    for(end=environment;*end;end+=wcslen(end)+1){}
    error=OpenNtBaseClientStartVdmWorker(environment,(DWORD)(end-environment+1),
        SW_HIDE,capability,&worker,&parent);
    FreeEnvironmentStringsW(environment);
    if(error)fprintf(stderr,"admitted VDM startup error=%lu\n",error);
    REQUIRE(!error && worker && parent);
    REQUIRE(!TerminateProcess(worker,99) && GetLastError()==ERROR_ACCESS_DENIED);
    if(!io_fault) {
        REQUIRE(WaitForSingleObject(parent,10000)==WAIT_OBJECT_0);
        REQUIRE(BaseCheckForVDM(parent,&code) && code==7);
        REQUIRE(SetEvent(released));
    }
    REQUIRE(WaitForSingleObject(worker,10000)==WAIT_OBJECT_0);
    REQUIRE(GetExitCodeProcess(worker,&code) && !code);
    if(io_fault) {
        HANDLE broker=NULL;
        REQUIRE(WaitForSingleObject(root,10000)==WAIT_OBJECT_0);
        REQUIRE(!OpenNtBaseClientBrokerProcess(&broker));
        REQUIRE(WaitForSingleObject(broker,0)==WAIT_TIMEOUT);
        CloseHandle(broker);
    }
    CloseHandle(parent);CloseHandle(worker);CloseHandle(released);
    CloseHandle(root);CloseHandle(capability);CloseHandle(restored);
    OpenNtBaseClientDisconnectCurrent();
    REQUIRE(SetEnvironmentVariableW(L"NTVDM_RPC_TEST_RELEASE",NULL));
    REQUIRE(SetEnvironmentVariableW(L"NTVDM_RPC_TEST_IO_FAULT",NULL));
    puts(io_fault==2 ? "PASS real worker Console-exit RPC: intact I/O, duplicate intent, Console-only instruction, live NTSRV and frontend retirement" :
        io_fault ? "PASS real frontend EOF reports to live NTSRV; Console-close instruction precedes carrier exit and frontend retirement" :
        "PASS service-admitted VDM: original command/reentry, repeated wait identity, wait-only worker, exact exit 7 and child assertions");
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
    HANDLE capability=NULL,restricted=NULL,root=NULL,frontend=NULL,restored=NULL;
    DWORD generation=0,result,index,root_pid;
    char executable[MAX_PATH],command[MAX_PATH+64],text[32];
    STARTUPINFOA startup={sizeof(startup)};
    PROCESS_INFORMATION child={0};
    REQUIRE(OpenNtBaseClientConnectCurrent()==ERROR_SUCCESS);
    REQUIRE(!OpenNtBaseClientStartFrontend((uint64_t)(UINT_PTR)GetConsoleWindow(),TRUE,
        &frontend,&capability,&restored));
    root_pid=GetProcessId(frontend);REQUIRE(root_pid && root_pid!=GetCurrentProcessId());
    REQUIRE(DuplicateHandle(GetCurrentProcess(),capability,GetCurrentProcess(),
        &restricted,SYNCHRONIZE,TRUE,0));
    /* A launcher cannot impersonate the service-created frontend. */
    REQUIRE(OpenNtBaseClientRegisterFrontendRoot(capability)==ERROR_ACCESS_DENIED);
    REQUIRE(OpenNtBaseClientRetainFrontendRoot(restricted,&root,&generation)==ERROR_SUCCESS);
    REQUIRE(generation && GetProcessId(root)==root_pid);
    REQUIRE(!SetEvent(restricted) && GetLastError()==ERROR_ACCESS_DENIED);
    REQUIRE(!TerminateProcess(root,99) && GetLastError()==ERROR_ACCESS_DENIED);
    CloseHandle(root);
    sprintf_s(text,sizeof(text),"%lx",(ULONG)(ULONG_PTR)restricted);
    REQUIRE(SetEnvironmentVariableA("NTVDM_RPC_TEST_CAPABILITY",text));
    sprintf_s(text,sizeof(text),"%lu",root_pid);
    REQUIRE(SetEnvironmentVariableA("NTVDM_RPC_TEST_ROOT",text));
    REQUIRE(GetModuleFileNameA(NULL,executable,MAX_PATH));
    for (index=0;index<2;++index) {
        sprintf_s(command,sizeof(command),"\"%s\" %s",executable,
            index ? "--frontend-root-stale" : "--frontend-root-child");
        REQUIRE(CreateProcessA(NULL,command,NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,NULL,&startup,&child));
        CloseHandle(child.hThread);
        if (WaitForSingleObject(child.hProcess,10000)!=WAIT_OBJECT_0) {
            TerminateProcess(child.hProcess,99);
            WaitForSingleObject(child.hProcess,10000);
            CloseHandle(child.hProcess);
            return 1;
        }
        REQUIRE(GetExitCodeProcess(child.hProcess,&result));CloseHandle(child.hProcess);
        REQUIRE(result==(index ? 38u : 37u));
        if (!index) {
            common_rpc_connection view=OpenNtBaseClientExecutionConnection(NULL);
            common_rpc_management management={view.binding,view.process};
            DTASKMGR_WORKER *items=NULL;DTASKMGR_KEY key={0};ULONG count=0,item;
            REQUIRE(!common_rpc_task_snapshot(&management,&count,&items));
            for(item=0;item<count;++item)
                if(items[item].key.category==MANAGEMENT_FRONTEND && items[item].process_id==root_pid)
                    key=items[item].key;
            if(items)MIDL_user_free(items);
            REQUIRE(key.generation && !common_rpc_close_management_node(&management,&key));
            REQUIRE(WaitForSingleObject(frontend,10000)==WAIT_OBJECT_0);
        }
    }
    REQUIRE(SetEnvironmentVariableA("NTVDM_RPC_TEST_CAPABILITY",NULL));
    REQUIRE(SetEnvironmentVariableA("NTVDM_RPC_TEST_ROOT",NULL));
    CloseHandle(restricted);CloseHandle(capability);CloseHandle(frontend);CloseHandle(restored);
    OpenNtBaseClientDisconnectCurrent();
    puts("PASS: service-created frontend capability, role and independent-event rejection, descendant rights and stale identity after managed close");
    return 0;
}

static int ntvwm_reuse_child(char **argv)
{
    HANDLE frontend=(HANDLE)(ULONG_PTR)strtoul(argv[2],NULL,16);
    HANDLE execution=(HANDLE)(ULONG_PTR)strtoul(argv[3],NULL,16);
    DWORD expected=(DWORD)strtoul(argv[4],NULL,10),code,request=0;
    HANDLE worker=NULL,target=NULL,receipt=NULL,foreign=CreateEventW(NULL,TRUE,FALSE,NULL);
    WCHAR native[MAX_PATH],directory[MAX_PATH],command[MAX_PATH+64],*environment;
    run16_native_start start={0};
    REQUIRE(frontend && execution && foreign && !OpenNtBaseClientConnectCurrent());
    REQUIRE(OpenNtBaseClientBindConsoleContext(foreign)==ERROR_ACCESS_DENIED);
    CloseHandle(foreign);
    REQUIRE(!OpenNtBaseClientBindConsoleContext(execution));
    {
        HANDLE root=NULL;DWORD generation=0;
        REQUIRE(!OpenNtBaseClientRetainFrontendRoot(frontend,&root,&generation) && root && generation);
        CloseHandle(root);
    }
    REQUIRE(!OpenNtBaseClientStartNativeWorker(OPENNT_BASE_NATIVE_LAUNCH_WIN32_CUI,&worker) &&
        GetProcessId(worker)==expected);
    /* Pin the actual Win32 image: a later x64 Run16 must not reinterpret the
     * x86 caller's redirected System32 spelling as the Win64 cmd.exe. */
    REQUIRE((GetSystemWow64DirectoryW(native,MAX_PATH) || GetSystemDirectoryW(native,MAX_PATH)) &&
        !wcscat_s(native,MAX_PATH,L"\\cmd.exe"));
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
        /* The x64 public launcher now deliberately selects native System32:
         * its Win64 target must reuse the same CUI carrier after Win32 exit 73.
         * Binding still checks that request's admitted machine/subsystem. */
        REQUIRE(GetSystemDirectoryW(native,MAX_PATH) && !wcscat_s(native,MAX_PATH,L"\\cmd.exe"));
        REQUIRE(swprintf_s(line,ARRAYSIZE(line),L"\"%ls\" \"%ls\" /d /c exit /b 61",launcher,native)>0);
        startup.dwFlags=STARTF_USESTDHANDLES;
        startup.hStdInput=GetStdHandle(STD_INPUT_HANDLE);
        startup.hStdOutput=GetStdHandle(STD_OUTPUT_HANDLE);startup.hStdError=GetStdHandle(STD_ERROR_HANDLE);
        REQUIRE(CreateProcessW(launcher,line,NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,NULL,&startup,&process));
        CloseHandle(process.hThread);
        wait=WaitForSingleObject(process.hProcess,10000);
        if(wait!=WAIT_OBJECT_0){TerminateProcess(process.hProcess,99);WaitForSingleObject(process.hProcess,5000);}
        REQUIRE(GetExitCodeProcess(process.hProcess,&code));CloseHandle(process.hProcess);
        if(wait!=WAIT_OBJECT_0 || code!=61)fprintf(stderr,"public run16 wait=%lu result=%lu\n",wait,code);
        REQUIRE(wait==WAIT_OBJECT_0 && code==61);
        REQUIRE(SetEnvironmentVariableW(L"NTVDM_FRONTEND_CAPABILITY",NULL));
        REQUIRE(SetEnvironmentVariableW(L"NTVDM_EXECUTION_CONSOLE",NULL));
    }
    FreeEnvironmentStringsW(environment);OpenNtBaseClientDisconnectCurrent();
    CloseHandle(frontend);CloseHandle(execution);return 73;
}

static int ntvwm_reuse_launcher(HANDLE frontend,DWORD expected)
{
    HANDLE execution=NULL,inherited[2]={0},log,input;WCHAR image[MAX_PATH],command[MAX_PATH+120],report[MAX_PATH];
    SECURITY_ATTRIBUTES attributes={sizeof(attributes),NULL,TRUE};
    STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION process={0};DWORD result,wait;
    REQUIRE(!OpenNtBaseClientAcquireConsoleContext(frontend,&execution));
    REQUIRE(DuplicateHandle(GetCurrentProcess(),frontend,GetCurrentProcess(),&inherited[0],SYNCHRONIZE,TRUE,0));
    REQUIRE(DuplicateHandle(GetCurrentProcess(),execution,GetCurrentProcess(),&inherited[1],SYNCHRONIZE,TRUE,0));
    REQUIRE(GetModuleFileNameW(NULL,image,MAX_PATH));
    REQUIRE(swprintf_s(report,ARRAYSIZE(report),L"%ls.reuse.txt",image)>0);
    log=CreateFileW(report,GENERIC_WRITE,FILE_SHARE_READ,&attributes,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
    REQUIRE(log!=INVALID_HANDLE_VALUE);
    input=CreateFileW(L"NUL",GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,&attributes,OPEN_EXISTING,0,NULL);
    REQUIRE(input!=INVALID_HANDLE_VALUE);
    startup.dwFlags=STARTF_USESTDHANDLES;startup.hStdInput=input;startup.hStdOutput=startup.hStdError=log;
    REQUIRE(swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" --ntvwm-reuse-child %lx %lx %lu",
        image,(ULONG)(ULONG_PTR)inherited[0],(ULONG)(ULONG_PTR)inherited[1],expected)>0);
    REQUIRE(CreateProcessW(image,command,NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,NULL,&startup,&process));
    CloseHandle(log);CloseHandle(input);
    CloseHandle(process.hThread);CloseHandle(inherited[0]);CloseHandle(inherited[1]);CloseHandle(execution);
    wait=WaitForSingleObject(process.hProcess,10000);
    if(wait!=WAIT_OBJECT_0){TerminateProcess(process.hProcess,99);WaitForSingleObject(process.hProcess,5000);}
    REQUIRE(GetExitCodeProcess(process.hProcess,&result));CloseHandle(process.hProcess);
    if(wait!=WAIT_OBJECT_0 || result!=73)fprintf(stderr,"reuse child wait=%lu result=%lu\n",wait,result);
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
    {
        HANDLE root=NULL,restored=NULL;
        REQUIRE(!OpenNtBaseClientStartFrontend((uint64_t)(UINT_PTR)GetConsoleWindow(),TRUE,
            &root,&frontend,&restored));
        CloseHandle(root);CloseHandle(restored);
    }
    REQUIRE(!OpenNtBaseClientAcquireConsoleContext(frontend,&execution));
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
    /* The service-owned start operation selects the resident worker or starts
     * one; this alone is not proof of uniqueness during the concurrent launch. */
    REQUIRE(!OpenNtBaseClientStartNativeWorker(OPENNT_BASE_NATIVE_LAUNCH_WIN32_CUI,&worker));
    REQUIRE(WaitForSingleObject(worker,0)==WAIT_TIMEOUT);
    shutdown=OpenProcess(PROCESS_TERMINATE|SYNCHRONIZE,FALSE,GetProcessId(worker));
    REQUIRE(shutdown && TerminateProcess(shutdown,0));
    REQUIRE(WaitForSingleObject(shutdown,5000)==WAIT_OBJECT_0);
    CloseHandle(shutdown);CloseHandle(worker);
    CloseHandle(inherited[0]);CloseHandle(inherited[1]);CloseHandle(execution);CloseHandle(frontend);
    OpenNtBaseClientDisconnectCurrent();
    puts("PASS public run16 concurrent first startup: exact 61/62 results, reusable resident NTVWM after launcher exit; real service-created frontend");
    return 0;
}

static int ntvwm_execution_rpc(void)
{
    WCHAR image[MAX_PATH],command[MAX_PATH+64],directory[MAX_PATH],native[MAX_PATH];
    WCHAR *slash,*environment;
    HANDLE worker=NULL;DWORD error=0,code=0,index;
    HANDLE capability=NULL;
    REQUIRE(!OpenNtBaseClientConnectCurrent());
    REQUIRE(GetModuleFileNameW(NULL,image,MAX_PATH));
    slash=wcsrchr(image,L'\\');REQUIRE(slash);
    REQUIRE(!wcscpy_s(slash+1,MAX_PATH-(size_t)(slash+1-image),L"ntvwm.exe"));
    REQUIRE(swprintf_s(command,ARRAYSIZE(command),L"\"%ls\"",image)>0);
    {
        HANDLE root=NULL,restored=NULL;
        REQUIRE(!OpenNtBaseClientStartFrontend((uint64_t)(UINT_PTR)GetConsoleWindow(),TRUE,
            &root,&capability,&restored));
        CloseHandle(root);CloseHandle(restored);
    }
    REQUIRE(!OpenNtBaseClientStartNativeWorker(OPENNT_BASE_NATIVE_LAUNCH_WIN32_CUI,&worker) && worker);
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
        error=run16_native_request_submit(capability,&start,&target,&receipt,&request);
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
    /* A launcher cannot impersonate NTCON to attach its own presentation pipe.
     * Observe the actual service-coordinated return instead. Inactive channel
     * barrier handling belongs to the frontend/worker I/O fixture. */
    REQUIRE(!OpenNtBaseClientWaitFrontendConsoleRestored());
    REQUIRE(WaitForSingleObject(worker,0)==WAIT_TIMEOUT);
    /* Fixture shutdown is explicit, not launcher-death or target completion. */
    {
        HANDLE terminate=OpenProcess(PROCESS_TERMINATE|SYNCHRONIZE,FALSE,GetProcessId(worker));
        REQUIRE(terminate && TerminateProcess(terminate,0) &&
            WaitForSingleObject(terminate,5000)==WAIT_OBJECT_0);
        CloseHandle(terminate);
    }
    CloseHandle(worker);CloseHandle(capability);
    OpenNtBaseClientDisconnectCurrent();
    puts("PASS actual NTVWM: independent entry, authenticated requests, exact 37/19 results, alias/EOF, failed launch/reuse; Win32 second launcher returns 73 and Win64 public run16 returns 61 through existing worker; forged context rejected; worker survives launcher");
    return 0;
}

/* Real RPC waits: elapsed time is not a failure except during explicitly
 * bounded service bootstrap. Fault injection is confined to the test service. */
typedef struct rpc_wait_test {
    HANDLE entered;
    const char *kind;
    DWORD error;
} rpc_wait_test;

static DWORD first_vdm_query(void)
{
    BASE_API_MSG message={0};
    DWORD error;
    NTSTATUS status=OpenNtBaseClientCallServer((PCSR_API_MSG)&message,NULL,
        CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepIsFirstVDM),
        sizeof(message.u.IsFirstVDM));
    if(status==STATUS_SUCCESS && message.ReturnValue==STATUS_SUCCESS)return ERROR_SUCCESS;
    error=GetLastError();
    return error ? error : ERROR_INVALID_DATA;
}

static DWORD WINAPI rpc_wait_call(void *context)
{
    rpc_wait_test *test=context;
    SetEvent(test->entered);
    if(!strcmp(test->kind,"connect"))test->error=OpenNtBaseClientConnectCurrent();
    else if(!strcmp(test->kind,"disconnect")) {
        OpenNtBaseClientDisconnectCurrent();
        test->error=ERROR_SUCCESS;
    } else test->error=first_vdm_query();
    return test->error;
}

static int rpc_wait_keeper(const char *ready_name,const char *release_name)
{
    HANDLE ready=OpenEventA(EVENT_MODIFY_STATE,FALSE,ready_name);
    HANDLE release=OpenEventA(SYNCHRONIZE,FALSE,release_name);
    DWORD error=ERROR_INVALID_HANDLE;
    if(ready && release) {
        error=OpenNtBaseClientConnectCurrent();
        if(!error && (!SetEvent(ready) || WaitForSingleObject(release,INFINITE)!=WAIT_OBJECT_0))
            error=GetLastError();
    }
    OpenNtBaseClientDisconnectCurrent();
    if(ready)CloseHandle(ready);if(release)CloseHandle(release);
    return (int)error;
}

static int rpc_wait_contract(const char *kind)
{
    typedef LONG (NTAPI *PROCESS_CONTROL)(HANDLE);
    PROCESS_CONTROL suspend=(PROCESS_CONTROL)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"NtSuspendProcess");
    PROCESS_CONTROL resume=(PROCESS_CONTROL)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"NtResumeProcess");
    HANDLE server=NULL,control=NULL,thread=NULL;
    HANDLE keeper_ready=NULL,keeper_release=NULL;
    PROCESS_INFORMATION keeper={0};STARTUPINFOA startup={sizeof(startup)};
    rpc_wait_test test={0};
    BOOL paused=FALSE,bootstrap=!strcmp(kind,"bootstrap"),loss=!strcmp(kind,"loss");
    DWORD error;ULONGLONG began,elapsed;int result=1;
    if(strcmp(kind,"reply") && strcmp(kind,"connect") && strcmp(kind,"disconnect") &&
        !bootstrap && !loss)return 2;
    if(!suspend || !resume || OpenNtBaseClientConnectCurrent())goto done;
    if(OpenNtBaseClientBrokerProcess(&server))goto done;
    control=OpenProcess(PROCESS_SUSPEND_RESUME|PROCESS_TERMINATE,FALSE,GetProcessId(server));
    if(!control)goto done;
    if(bootstrap || !strcmp(kind,"connect")) {
        char image[MAX_PATH],command[MAX_PATH+256],ready_name[80],release_name[80];
        /* A separate real client prevents the approved empty-service timer
         * from competing with the connection wait under test. */
        sprintf_s(ready_name,sizeof(ready_name),"Local\\rpc-wait-ready-%lu",GetCurrentProcessId());
        sprintf_s(release_name,sizeof(release_name),"Local\\rpc-wait-release-%lu",GetCurrentProcessId());
        keeper_ready=CreateEventA(NULL,TRUE,FALSE,ready_name);
        keeper_release=CreateEventA(NULL,TRUE,FALSE,release_name);
        if(!keeper_ready || !keeper_release || !GetModuleFileNameA(NULL,image,MAX_PATH))goto done;
        sprintf_s(command,sizeof(command),"\"%s\" --rpc-wait-keeper %s %s",image,ready_name,release_name);
        if(!CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&startup,&keeper))goto done;
        if(WaitForSingleObject(keeper_ready,5000)!=WAIT_OBJECT_0)goto done;
        OpenNtBaseClientDisconnectCurrent();
    }
    if(suspend(control)<0)goto done;
    paused=TRUE;
    if(bootstrap) {
        began=GetTickCount64();
        error=OpenNtBaseClientConnectCurrentTimeout(OPENNT_LIFECYCLE_GRACE_MS);
        elapsed=GetTickCount64()-began;
        printf("bootstrap error=%lu elapsed=%llu\n",error,(unsigned long long)elapsed);
        if(error!=ERROR_TIMEOUT || elapsed<4500 || elapsed>8000 ||
            WaitForSingleObject(server,0)!=WAIT_TIMEOUT)goto done;
    } else {
        test.kind=kind;test.entered=CreateEventW(NULL,TRUE,FALSE,NULL);
        if(!test.entered)goto done;
        thread=CreateThread(NULL,0,rpc_wait_call,&test,0,NULL);if(!thread)goto done;
        if(WaitForSingleObject(test.entered,5000)!=WAIT_OBJECT_0)goto done;
        /* Cross the removed ten-second policy for ordinary calls, not merely
         * a short scheduling delay. Loss needs only an outstanding call. */
        if(WaitForSingleObject(thread,loss ? 100 : 11000)!=WAIT_TIMEOUT)goto done;
        if(loss) {
            if(!TerminateProcess(control,ERROR_CANCELLED) ||
                WaitForSingleObject(server,5000)!=WAIT_OBJECT_0)goto done;
            paused=FALSE;
        } else {
            if(resume(control)<0)goto done;
            paused=FALSE;
        }
        if(WaitForSingleObject(thread,5000)!=WAIT_OBJECT_0)goto done;
        printf("RPC wait %s result=%lu service-wait=%lu\n",kind,test.error,WaitForSingleObject(server,0));
        if(loss) {
            /* RPC can report its broken call before the process object is
             * signalled. Once death is confirmed, the public loss mapping
             * must be the original disconnected-handle error. */
            if((test.error!=ERROR_INVALID_HANDLE && test.error!=RPC_S_CALL_FAILED) ||
                OpenNtBaseClientFailure(test.error)!=ERROR_INVALID_HANDLE ||
                OpenNtBaseClientFailure(0)!=ERROR_INVALID_HANDLE)goto done;
        } else if(test.error)goto done;
        if(!loss && strcmp(kind,"disconnect") && first_vdm_query())goto done;
        if(!strcmp(kind,"disconnect")) {
            HANDLE unexpected=NULL;
            if(OpenNtBaseClientBrokerProcess(&unexpected)!=ERROR_INVALID_STATE || unexpected)goto done;
            OpenNtBaseClientDisconnectCurrent(); /* Idempotent local cleanup. */
        }
    }
    printf("PASS RPC wait contract: %s\n",kind);result=0;
done:
    if(paused && resume(control)<0)result=1;
    if(thread) {
        /* The harness may fail a diagnostic bound, but cannot release a live
         * test call's stack or disconnect its borrowed client concurrently. */
        WaitForSingleObject(thread,INFINITE);CloseHandle(thread);
    }
    if(test.entered)CloseHandle(test.entered);
    if(keeper_release)SetEvent(keeper_release);
    if(keeper.hProcess) {
        DWORD exit_code;
        if(WaitForSingleObject(keeper.hProcess,5000)!=WAIT_OBJECT_0) {
            TerminateProcess(keeper.hProcess,ERROR_CANCELLED);
            WaitForSingleObject(keeper.hProcess,INFINITE);result=1;
        }
        if(!GetExitCodeProcess(keeper.hProcess,&exit_code) || exit_code)result=1;
        CloseHandle(keeper.hThread);CloseHandle(keeper.hProcess);
    }
    if(keeper_ready)CloseHandle(keeper_ready);if(keeper_release)CloseHandle(keeper_release);
    if(control)CloseHandle(control);if(server)CloseHandle(server);
    OpenNtBaseClientDisconnectCurrent();
    return result;
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
    {
        WCHAR executable[MAX_PATH],*name;
        REQUIRE(GetModuleFileNameW(NULL,executable,ARRAYSIZE(executable)));
        name=wcsrchr(executable,L'\\');
        if(name && !_wcsicmp(name+1,L"ntvdm.exe"))return admitted_vdm_child();
    }
    if(argc==2 && !lstrcmpA(argv[1],"--admitted-vdm"))return admitted_vdm_parent(FALSE);
    if(argc==2 && !lstrcmpA(argv[1],"--frontend-io-fault"))return admitted_vdm_parent(TRUE);
    if(argc==2 && !lstrcmpA(argv[1],"--worker-console-exit"))return admitted_vdm_parent(2);
    if (argc==5 && !lstrcmpA(argv[1],"--ntvwm-reuse-child")) return ntvwm_reuse_child(argv);
    if (argc==2 && !lstrcmpA(argv[1],"--native-worker-startup")) return ntvwm_execution_rpc();
    if (argc==2 && !lstrcmpA(argv[1],"--ntvwm-execution")) return ntvwm_execution_rpc();
    if (argc==2 && !lstrcmpA(argv[1],"--ntvwm-public-startup")) return ntvwm_public_startup();
    if (argc==3 && !lstrcmpA(argv[1],"--rpc-wait"))return rpc_wait_contract(argv[2]);
    if (argc==4 && !lstrcmpA(argv[1],"--rpc-wait-keeper"))return rpc_wait_keeper(argv[2],argv[3]);
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
     * --admitted-vdm route proves the service-owned positive path. */
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
