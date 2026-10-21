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
#include "common/rpc/management.h"
#include "common/protocol/management.h"
#include <base_rpc_client_internal.h>
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
        CloseHandle(root);CloseHandle(capability);CloseHandle(pipe);CloseHandle(frontend);CloseHandle(ready);
        REQUIRE(OpenNtBaseClientTakeFrontend(&pipe,&frontend,&generation,&ready)==ERROR_ALREADY_EXISTS);
        REQUIRE(!pipe && !frontend && !generation && !ready);
        REQUIRE(OpenNtBaseClientWaitFrontend(&pipe,&frontend,&generation,&ready)==ERROR_ALREADY_EXISTS);
        REQUIRE(!pipe && !frontend && !generation && !ready);
    }
    REQUIRE(get_first_command(command,sizeof(command))==STATUS_SUCCESS);
    REQUIRE(!lstrcmpA(command,"MEM\\r\\n"));
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
    REQUIRE(WaitForSingleObject(released,10000)==WAIT_OBJECT_0);
    CloseHandle(released);
    OpenNtBaseClientDisconnectCurrent();
    return 0;
}

static int admitted_vdm_parent(void)
{
    BASE_API_MSG message={0};STARTUPINFOA startup={sizeof(startup)};
    CHAR command[]="MEM\\r\\n";WCHAR name[128],*environment,*end;
    HANDLE root=NULL,capability=NULL,restored=NULL,worker=NULL,parent=NULL,released;
    DWORD error,code=STILL_ACTIVE;NTSTATUS status;
    swprintf_s(name,ARRAYSIZE(name),L"Local\\NTVDM-RPC-admitted-release-%lu",GetCurrentProcessId());
    released=CreateEventW(NULL,TRUE,FALSE,name);REQUIRE(released);
    REQUIRE(SetEnvironmentVariableW(L"NTVDM_RPC_TEST_RELEASE",name));
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
    REQUIRE(WaitForSingleObject(parent,10000)==WAIT_OBJECT_0);
    REQUIRE(BaseCheckForVDM(parent,&code) && code==7);
    REQUIRE(SetEvent(released));
    REQUIRE(WaitForSingleObject(worker,10000)==WAIT_OBJECT_0);
    REQUIRE(GetExitCodeProcess(worker,&code) && !code);
    CloseHandle(parent);CloseHandle(worker);CloseHandle(released);
    CloseHandle(root);CloseHandle(capability);CloseHandle(restored);
    OpenNtBaseClientDisconnectCurrent();
    REQUIRE(SetEnvironmentVariableW(L"NTVDM_RPC_TEST_RELEASE",NULL));
    puts("PASS service-admitted VDM: original command/reentry, repeated wait identity, wait-only worker, exact exit 7 and child assertions");
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

/* Exercise the public facade and its output contract, not a replacement RPC. */
static DWORD worker_control_query(unsigned mode,HANDLE token)
{
    HANDLE output=NULL,peer=NULL,ready=NULL;DWORD generation=0,decision=WORKER_IO_KEEP,error;
    switch(mode) {
    case 22:
        error=OpenNtBaseClientAcquireConsoleContext(token,&output);break;
    case 23:return OpenNtBaseClientBindConsoleContext(token);
    case 24:return OpenNtBaseClientRegisterNativeBackend(token);
    case 25:
        error=OpenNtBaseClientWorkerIoCheckpoint(WORKER_IO_CHECKPOINT_COMPLETE,42,&decision);break;
    case 26:return OpenNtBaseClientWorkerIoTransition(WORKER_IO_ACQUIRE);
    case 27:error=OpenNtBaseClientTakeFrontend(&output,&peer,&generation,&ready);break;
    case 28:error=OpenNtBaseClientWaitFrontend(&output,&peer,&generation,&ready);break;
    default:return ERROR_INVALID_PARAMETER;
    }
    if(output || peer || ready || generation || decision!=WORKER_IO_KEEP) {
        if(output)CloseHandle(output);if(peer)CloseHandle(peer);if(ready)CloseHandle(ready);
        return ERROR_INVALID_DATA;
    }
    return error;
}

/* Each call reaches the same real RPC connection. Outputs must stay empty on
 * transport timeout, including StartVdmWorker's process-local receipt mapping. */
static DWORD launch_control_query(unsigned mode,HANDLE token)
{
    WCHAR environment[2]={0};BYTE payload=0;
    HANDLE output=NULL,peer=NULL;DWORD value=0,completed=0,error;
    switch(mode) {
    case 29:error=OpenNtBaseClientSubmitNativeRequest(OPENNT_BASE_NATIVE_REQUEST_LAUNCH,
        token,1,&payload,&output,&peer,&value);break;
    case 30:error=OpenNtBaseClientFinishNativeRequest(42,&value,&completed);break;
    case 31:error=OpenNtBaseClientStartVdmWorker(environment,2,SW_HIDE,token,&output,&peer);break;
    case 32:error=OpenNtBaseClientStartNativeWorker(0,&output);break;
    case 33:return OpenNtBaseClientCompleteNativeRequest(42,37,ERROR_WRITE_FAULT,0);
    case 34:return OpenNtBaseClientNativeStartupResult(1,42,ERROR_ACCESS_DENIED,NULL,NULL);
    case 35:return OpenNtBaseClientBindNativeTarget(42,GetCurrentProcess(),token);
    case 36:error=OpenNtBaseClientRetainFrontendRoot(token,&output,&value);break;
    case 37:return OpenNtBaseClientRequestFrontend(token);
    default:return ERROR_INVALID_PARAMETER;
    }
    if(output || peer || value || completed) {
        if(output)CloseHandle(output);if(peer)CloseHandle(peer);
        return ERROR_INVALID_DATA;
    }
    return error;
}

static int frontend_deadline(unsigned mode)
{
    DWORD (*events[])(HANDLE *)={OpenNtBaseClientWorkerFrontendCapability,
        OpenNtBaseClientWorkerShutdownEvent,OpenNtBaseClientWorkerConsoleCloseEvent,
        OpenNtBaseClientWorkerIoReleaseEvent};
    BOOL worker_exit=mode==1;
    BOOL startup=mode==3;
    typedef LONG (NTAPI *PROCESS_CONTROL)(HANDLE);
    PROCESS_CONTROL suspend=(PROCESS_CONTROL)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"NtSuspendProcess");
    PROCESS_CONTROL resume=(PROCESS_CONTROL)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"NtResumeProcess");
    HANDLE root=NULL,capability=NULL,restored=NULL,server=NULL,control=NULL,frontend_control=NULL;
    HANDLE pipe_read=NULL,pipe_write=NULL;
    DWORD error,input_mode=0;ULONGLONG began,elapsed;int result=1;BOOL paused=FALSE,frontend_paused=FALSE;
    /* Workers keep their shared connection after an individual report times
     * out. Launchers instead latch their bounded finalization failure. Match
     * those actual production connection entries, not one policy for both. */
    if(!suspend || !resume || (worker_exit ? OpenNtBaseClientConnectCurrent() :
        OpenNtBaseClientConnectCurrentTimeout(10000)))goto done;
    if(OpenNtBaseClientBrokerProcess(&server))goto done;
    if(mode>=18 && mode<=21) {
        HANDLE event=(HANDLE)1;
        /* Notifications require an admitted worker identity. */
        if(events[mode-18](&event)!=ERROR_ACCESS_DENIED || event)goto done;
    }
    if(mode>=22 && mode<=28) {
        DWORD expected=mode==24 ? ERROR_INVALID_STATE : ERROR_ACCESS_DENIED;
        capability=CreateEventW(NULL,TRUE,FALSE,NULL);
        if(!capability)goto done;
        error=worker_control_query(mode,capability);
        printf("worker control %u unadmitted=%lu expected=%lu\n",mode,error,expected);
        if(error!=expected)goto done;
    }
    if(mode>=29) {
        capability=CreateEventW(NULL,TRUE,FALSE,NULL);
        if(!capability)goto done;
    }
    if(mode==9 && !GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE),&input_mode))goto done;
    if(!worker_exit && !startup && (mode<4 || mode==9) && OpenNtBaseClientStartFrontend((uint64_t)(UINT_PTR)GetConsoleWindow(),TRUE,
        &root,&capability,&restored))goto done;
    if(startup)OpenNtBaseClientDisconnectCurrent(); /* End setup, before injecting the fault. */
    if(mode>=10 && mode<=16) {
        capability=CreateEventW(NULL,TRUE,FALSE,NULL);
        restored=CreateEventW(NULL,TRUE,FALSE,NULL);
        if(!capability || !restored)goto done;
        if(mode==13 && !CreatePipe(&pipe_read,&pipe_write,NULL,0))goto done;
    }
    if(mode==9) {
        DWORD exit_code=STILL_ACTIVE,after=0,wait,index,count=0;
        DTASKMGR_WORKER *items=NULL;DTASKMGR_KEY key={0};
        common_rpc_connection view=OpenNtBaseClientExecutionConnection(NULL);
        common_rpc_management management={view.binding,view.process};
        error=common_rpc_task_snapshot(&management,&count,&items);
        if(error){printf("FAIL snapshot=%lu\n",error);goto done;}
        for(index=0;index<count;++index)
            if(items[index].key.category==MANAGEMENT_FRONTEND && items[index].process_id==GetProcessId(root))key=items[index].key;
        if(items)MIDL_user_free(items);
        if(!key.generation){puts("FAIL actual frontend absent from management snapshot");goto done;}
        frontend_control=OpenProcess(0x0800,FALSE,GetProcessId(root));
        if(!frontend_control || suspend(frontend_control)<0)goto done;
        frontend_paused=TRUE;
        error=common_rpc_close_management_node(&management,&key);
        if(error){printf("FAIL management close=%lu\n",error);goto done;}
        control=OpenProcess(0x0800,FALSE,GetProcessId(server));
        if(!control || suspend(control)<0)goto done;
        paused=TRUE;began=GetTickCount64();
        if(resume(frontend_control)<0)goto done;
        frontend_paused=FALSE;
        wait=WaitForSingleObject(root,12000);
        (void)GetExitCodeProcess(root,&exit_code);
        (void)GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE),&after);
        printf("NTCON wait=%lu exit=%lu input=%lx expected=%lx elapsed=%llu\n",wait,exit_code,after,input_mode,
            (unsigned long long)(GetTickCount64()-began));
        if(wait!=WAIT_OBJECT_0 || exit_code!=ERROR_TIMEOUT || after!=input_mode ||
            WaitForSingleObject(server,0)!=WAIT_TIMEOUT)goto done;
        printf("PASS: actual NTCON timeout retirement and input restoration with service paused; elapsed=%llu\n",
            (unsigned long long)(GetTickCount64()-began));
        result=0;goto done;
    }
    /* Suspend only the owned frontend (0) or service (1/2). Production
     * launchers never receive this fault-injection access. */
    control=OpenProcess(0x0800,FALSE,GetProcessId(mode ? server : root)); /* PROCESS_SUSPEND_RESUME */
    if(!control || suspend(control)<0)goto done;
    paused=TRUE;began=GetTickCount64();
    if(mode==17) {
        HANDLE disconnected=(HANDLE)1;
        OpenNtBaseClientDisconnectCurrent();
        elapsed=GetTickCount64()-began;
        printf("disconnect elapsed=%llu\n",(unsigned long long)elapsed);
        if(elapsed<9000 || elapsed>13000 || WaitForSingleObject(server,0)!=WAIT_TIMEOUT ||
            OpenNtBaseClientBrokerProcess(&disconnected)!=ERROR_INVALID_STATE || disconnected)goto done;
        began=GetTickCount64();
        OpenNtBaseClientDisconnectCurrent();
        if(GetTickCount64()-began>=1000)goto done;
        puts("PASS: ordinary disconnect deadline and idempotent local cleanup");
        result=0;goto done;
    }
    if(mode>=29)error=launch_control_query(mode,capability);
    else if(mode>=22)error=worker_control_query(mode,capability);
    else if(mode>=18) {
        HANDLE event=(HANDLE)1;
        error=events[mode-18](&event);
        if(event)goto done;
    } else if(mode==8) {
        common_rpc_connection view=OpenNtBaseClientExecutionConnection(NULL);
        common_rpc_management management={view.binding,view.process};DTASKMGR_KEY key={0};
        error=common_rpc_close_management_node(&management,&key);
    } else if(mode==5)error=OpenNtBaseClientFrontendCloseQuiesced();
    else if(mode==6)error=OpenNtBaseClientFrontendConsoleRestored();
    else if(mode==10)error=OpenNtBaseClientRegisterFrontendRoot(capability);
    else if(mode==11)error=OpenNtBaseClientRegisterFrontendLease(
        (uint64_t)(UINT_PTR)GetConsoleWindow(),GetCurrentProcessId(),TRUE,capability,restored);
    else if(mode==12)error=OpenNtBaseClientReportCurrentConsoleMembers();
    else if(mode==13) {
        DWORD generation=99;
        error=OpenNtBaseClientAttachFrontendRequest(1,pipe_read,capability,&generation);
        if(generation)goto done;
    }
    else if(mode==14) {
        HANDLE owner=(HANDLE)1,token=(HANDLE)1,ack=(HANDLE)1;
        error=OpenNtBaseClientStartFrontend((uint64_t)(UINT_PTR)GetConsoleWindow(),TRUE,&owner,&token,&ack);
        if(owner || token || ack)goto done;
    }
    else if(mode==15) {
        HANDLE event=(HANDLE)1;
        error=OpenNtBaseClientFrontendStateChanged(&event);
        if(event)goto done;
    }
    else if(mode==16)error=OpenNtBaseClientFrontendStartupResult(capability,ERROR_SUCCESS);
    else error=startup ? OpenNtBaseClientConnectCurrentTimeout(10000) : worker_exit ? OpenNtBaseClientRequestWorkerExit() :
        OpenNtBaseClientWaitFrontendConsoleRestored();
    elapsed=GetTickCount64()-began;
    printf("finalization status=%lu elapsed=%llu\n",error,(unsigned long long)elapsed);
    if(error!=ERROR_TIMEOUT || elapsed<9000 || elapsed>13000 ||
        WaitForSingleObject(server,0)!=WAIT_TIMEOUT)goto done;
    if(startup) {
        puts("PASS: startup deadline with stalled service and no cleanup RPC");
        result=0;goto done;
    }
    if(worker_exit || mode==8) {
        BASE_API_MSG query={0};NTSTATUS status;
        if(resume(control)<0)goto done;
        paused=FALSE;
        /* Not an exit-request replay. An unrelated ordinary call verifies
         * the shared connection did not inherit this report's timeout. */
        status=OpenNtBaseClientCallServer((PCSR_API_MSG)&query,NULL,
            CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepIsFirstVDM),
            sizeof(query.u.IsFirstVDM));
        if(status!=STATUS_SUCCESS || query.ReturnValue!=STATUS_SUCCESS)goto done;
        puts(mode==8 ? "PASS: management close RPC deadline without poisoning shared connection" :
            "PASS: real worker exit RPC deadline without poisoning shared connection");
        result=0;goto done;
    }
    began=GetTickCount64();
    if(OpenNtBaseClientWaitFrontendConsoleRestored()!=ERROR_TIMEOUT)goto done;
    if(mode>=29) {
        unsigned i;
        for(i=29;i<=37;++i)if(launch_control_query(i,capability)!=ERROR_TIMEOUT)goto done;
    }
    if(mode>=22) {
        unsigned i;
        for(i=22;i<=28;++i)if(worker_control_query(i,capability)!=ERROR_TIMEOUT)goto done;
    }
    if(mode>=18) {
        unsigned i;
        for(i=0;i<sizeof(events)/sizeof(events[0]);++i) {
            HANDLE event=(HANDLE)1;
            if(events[i](&event)!=ERROR_TIMEOUT || event)goto done;
        }
    }
    if(mode>=10) {
        DWORD generation=99;
        HANDLE owner=(HANDLE)1,token=(HANDLE)1,ack=(HANDLE)1,event=(HANDLE)1;
        /* No cleanup/report call may issue another RPC while the first
         * deadline is latched and the authenticated service remains paused. */
        if(OpenNtBaseClientFrontendStartupResult(capability,ERROR_TIMEOUT)!=ERROR_TIMEOUT ||
            OpenNtBaseClientRegisterFrontendRoot(capability)!=ERROR_TIMEOUT ||
            OpenNtBaseClientRegisterFrontendLease(0,0,FALSE,capability,restored)!=ERROR_TIMEOUT ||
            OpenNtBaseClientReportCurrentConsoleMembers()!=ERROR_TIMEOUT ||
            OpenNtBaseClientAttachFrontendRequest(1,pipe_read,capability,&generation)!=ERROR_TIMEOUT ||
            generation ||
            OpenNtBaseClientStartFrontend(0,TRUE,&owner,&token,&ack)!=ERROR_TIMEOUT || owner || token || ack ||
            OpenNtBaseClientFrontendStateChanged(&event)!=ERROR_TIMEOUT || event)goto done;
    }
    OpenNtBaseClientDisconnectCurrent();
    if(GetTickCount64()-began>=1000)goto done;
    puts(mode>=4 ? "PASS: frontend control deadline and local disconnect" :
        mode==2 ? "PASS: finalization deadline with stalled service and local disconnect" :
        "PASS: real RPC timeout without service death or request replay");result=0;
done:
    if(paused && resume(control)<0)result=1;
    if(frontend_paused && resume(frontend_control)<0)result=1;
    if(frontend_control)CloseHandle(frontend_control);
    if(pipe_read)CloseHandle(pipe_read);if(pipe_write)CloseHandle(pipe_write);
    if(control)CloseHandle(control);
    if(root)CloseHandle(root);if(capability)CloseHandle(capability);
    if(restored)CloseHandle(restored);if(server)CloseHandle(server);
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
    if(argc==2 && !lstrcmpA(argv[1],"--admitted-vdm"))return admitted_vdm_parent();
    if (argc==5 && !lstrcmpA(argv[1],"--ntvwm-reuse-child")) return ntvwm_reuse_child(argv);
    if (argc==2 && !lstrcmpA(argv[1],"--native-worker-startup")) return ntvwm_execution_rpc();
    if (argc==2 && !lstrcmpA(argv[1],"--ntvwm-execution")) return ntvwm_execution_rpc();
    if (argc==2 && !lstrcmpA(argv[1],"--ntvwm-public-startup")) return ntvwm_public_startup();
    if (argc==2 && !lstrcmpA(argv[1],"--frontend-timeout-parent")) return frontend_deadline(FALSE);
    if (argc==2 && !lstrcmpA(argv[1],"--worker-exit-timeout")) return frontend_deadline(TRUE);
    if (argc==2 && !lstrcmpA(argv[1],"--finalization-service-timeout")) return frontend_deadline(2);
    if (argc==2 && !lstrcmpA(argv[1],"--startup-service-timeout")) return frontend_deadline(3);
    if (argc==3 && !lstrcmpA(argv[1],"--control-service-timeout")) {
        unsigned mode=(unsigned)strtoul(argv[2],NULL,10);
        return mode>=5 && mode<=37 && mode!=7 ? frontend_deadline(mode) : 2;
    }
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
