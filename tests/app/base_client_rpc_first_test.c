/* Product-facing BaseClient transport cut.  The original vdm.c call sites
 * retain their CSR-shaped signature; this focused client proves the selected
 * replacement reaches the real standalone BaseSrv first-VDM owner. */
#include <windows.h>
#include <stdio.h>
#include "adapter-opennt-host/basesrv/include/base_client.h"
#include "adapter-opennt-host/basesrv/include/base_rpc_client.h"

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

static int reservation_child(void)
{
    BASE_API_MSG message={0};
    HANDLE parent_event=NULL;
    CHAR command[1024]={0};
    NTSTATUS status;
    REQUIRE(OpenNtBaseClientConnectCurrent()==ERROR_SUCCESS);
    message.u.UpdateVDMEntry.EntryIndex=UPDATE_VDM_PROCESS_HANDLE;
    message.u.UpdateVDMEntry.BinaryType=BINARY_TYPE_DOS;
    status=OpenNtBaseClientCallServer((PCSR_API_MSG)&message,NULL,
        CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepUpdateVDMEntry),
        sizeof(message.u.UpdateVDMEntry));
    REQUIRE(status==STATUS_SUCCESS && message.ReturnValue==STATUS_SUCCESS);
    parent_event=message.u.UpdateVDMEntry.WaitObjectForParent;
    REQUIRE(parent_event!=NULL && WaitForSingleObject(parent_event,0)==WAIT_TIMEOUT);
    REQUIRE(get_first_command(command,sizeof(command))==STATUS_SUCCESS);
    REQUIRE(!lstrcmpA(command,"MEM\\r\\n"));
    CloseHandle(parent_event);
    OpenNtBaseClientDisconnectCurrent();
    puts("PASS: reserved worker claimed, registered and received original command");
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
    DWORD error;
    REQUIRE(OpenNtBaseClientConnectCurrent()==ERROR_SUCCESS);
    message.u.CheckVDM.ConsoleHandle=(HANDLE)1;
    message.u.CheckVDM.BinaryType=BINARY_TYPE_DOS;
    message.u.CheckVDM.CmdLine=command;
    message.u.CheckVDM.CmdLen=sizeof(command);
    message.u.CheckVDM.StartupInfo=&startup;
    status=OpenNtBaseClientCallServer((PCSR_API_MSG)&message,NULL,
        CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepCheckVDM),sizeof(message.u.CheckVDM));
    REQUIRE(status==STATUS_SUCCESS && message.ReturnValue==STATUS_SUCCESS);
    REQUIRE(OpenNtBaseClientReserveWorker(message.u.CheckVDM.iTask,&reservation)==ERROR_SUCCESS);
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
    REQUIRE(ResumeThread(child.hThread)!=(DWORD)-1);
    REQUIRE(WaitForSingleObject(child.hProcess,15000)==WAIT_OBJECT_0);
    REQUIRE(GetExitCodeProcess(child.hProcess,&error) && error==0);
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
