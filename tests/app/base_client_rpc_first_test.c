/* Product-facing BaseClient transport cut.  The original vdm.c call sites
 * retain their CSR-shaped signature; this focused client proves the selected
 * replacement reaches the real standalone BaseSrv first-VDM owner. */
#include <windows.h>
#include <stdio.h>
#include "basesrv/opennt/include/base_client.h"
#include "basesrv/opennt/include/base_rpc_client.h"

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

static int reservation_child(void)
{
    CHAR command[1024]={0};
    REQUIRE(OpenNtBaseClientConnectCurrent()==ERROR_SUCCESS);
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
    OpenNtBaseClientDisconnectCurrent();
    puts("PASS: reserved worker claimed and received original command");
    return 0;
}

static int reservation_parent(void)
{
    BASE_API_MSG message={0};
    STARTUPINFOA startup={sizeof(startup)};
    PROCESS_INFORMATION child={0};
    CHAR command[]="MEM\\r\\n",image[MAX_PATH],childCommand[MAX_PATH+32];
    uint64_t reservation=0;
    NTSTATUS status;
    ULONG task;
    DWORD error,exit_code=STILL_ACTIVE;
    HANDLE parent_event;
    REQUIRE(OpenNtBaseClientConnectCurrent()==ERROR_SUCCESS);
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
        _snprintf_s(childCommand,sizeof(childCommand),_TRUNCATE,"\"%s\" --reservation-child",image)>0);
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
    REQUIRE(ResumeThread(child.hThread)!=(DWORD)-1);
    REQUIRE(WaitForSingleObject(child.hProcess,15000)==WAIT_OBJECT_0);
    REQUIRE(GetExitCodeProcess(child.hProcess,&error) && error==0);
    REQUIRE(WaitForSingleObject(parent_event,15000)==WAIT_OBJECT_0);
    {
    BOOL check_for_vdm=BaseCheckForVDM(parent_event,&exit_code);
    if (!check_for_vdm)
        fprintf(stderr,"BaseCheckForVDM failed: %lu\\n",GetLastError());
    if (exit_code!=7) fprintf(stderr,"BaseCheckForVDM returned: %lu\\n",exit_code);
    REQUIRE(check_for_vdm && exit_code==7);
    }
    CloseHandle(parent_event);
    CloseHandle(child.hThread);CloseHandle(child.hProcess);
    REQUIRE(OpenNtBaseClientReleaseWorker(reservation)==ERROR_SUCCESS);
    OpenNtBaseClientDisconnectCurrent();
    puts("PASS: launcher reserved and registered a real worker process");
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
    if (argc==2 && !lstrcmpA(argv[1],"--reservation-child")) return reservation_child();
    if (argc==2 && !lstrcmpA(argv[1],"--reservation-parent")) return reservation_parent();
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
