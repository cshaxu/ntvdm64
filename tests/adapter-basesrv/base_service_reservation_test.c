#include <base_service.h>
#include "basesrv.h"
#include <base_command.h>
#include "basesrv-exe/transport/vdm_receipt.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHECK(value) do { if (!(value)) { fprintf(stderr,"FAIL %d\\n",__LINE__);return 1; } } while (0)

static DWORD WINAPI same_console_query(void *context,HANDLE caller,const HANDLE *candidates,
    DWORD count,HANDLE cancel,DWORD timeout,BYTE *members)
{
    DWORD *calls=context,index;
    (void)cancel;(void)timeout;
    if (!calls || !count || !members) return ERROR_INVALID_PARAMETER;
    /* Exercise the service-to-query ABI, not just the membership result.
     * The original launcher and worker are two distinct live candidates. */
    if (!GetProcessId(caller)) return ERROR_INVALID_HANDLE;
    for (index=0;index<count;++index)
        if (!GetProcessId(candidates[index]) ||
            WaitForSingleObject(candidates[index],0)!=WAIT_TIMEOUT)
            return ERROR_INVALID_HANDLE;
    ++*calls;
    ZeroMemory(members,count);
    members[0]=1;
    return ERROR_SUCCESS;
}

static int detached_reservation(OPENNT_BASE_SERVICE *service,HANDLE self)
{
    OPENNT_BASE_CONNECTION *connection=NULL;
    BASE_API_MSG request={0},reply={0};
    DWORD generation=0,error;
    uint32_t bytes=0,required=0,receipt=0;
    HANDLE event=NULL;
    uint64_t reservation=0;
    void *wire,*answer;
    char app[]="MEM.EXE",cmd[]="MEM.EXE\r\n",directory[]="C:\\",environment[]="X=Y\0";
    CHECK(OpenNtBaseServiceConnect(service,self,&connection,&generation)==ERROR_SUCCESS);
    request.u.CheckVDM.BinaryType=BINARY_TYPE_DOS;
    request.u.CheckVDM.ConsoleHandle=NULL;
    request.u.CheckVDM.CodePage=437;
    request.u.CheckVDM.AppName=app;request.u.CheckVDM.AppLen=sizeof(app);
    request.u.CheckVDM.CmdLine=cmd;request.u.CheckVDM.CmdLen=sizeof(cmd);
    request.u.CheckVDM.CurDirectory=directory;request.u.CheckVDM.CurDirectoryLen=sizeof(directory);
    request.u.CheckVDM.Env=environment;request.u.CheckVDM.EnvLen=sizeof(environment);
    CHECK(OpenNtBaseEncodeCheckCommand(&request,1,generation,NULL,0,&bytes));
    wire=malloc(bytes);
    CHECK(wire && OpenNtBaseEncodeCheckCommand(&request,1,generation,wire,bytes,&bytes));
    CHECK(OpenNtBaseServiceCheck(connection,GetCurrentProcessId(),generation,wire,bytes,
        NULL,0,&required,&event,&receipt)==ERROR_INSUFFICIENT_BUFFER);
    answer=malloc(required);
    CHECK(answer && OpenNtBaseServiceCheck(connection,GetCurrentProcessId(),generation,
        wire,bytes,answer,required,&required,&event,&receipt)==ERROR_SUCCESS);
    CHECK(OpenNtBaseApplyCheckReply(answer,required,generation,1,&reply));
    CHECK(reply.ReturnValue==STATUS_SUCCESS && reply.u.CheckVDM.iTask &&
        reply.u.CheckVDM.VDMState==VDM_NOT_PRESENT);
    error=OpenNtBaseServiceCreateReservation(connection,GetCurrentProcessId(),generation,
        reply.u.CheckVDM.iTask,&reservation);
    fprintf(stderr,"detached CheckDOS task=%lu reservation-error=%lu\n",reply.u.CheckVDM.iTask,error);
    CHECK(error==ERROR_SUCCESS && reservation);
    CHECK(OpenNtBaseServiceDisconnect(connection)==ERROR_SUCCESS);
    CHECK(OpenNtBaseServiceIsEmpty(service));
    free(answer);free(wire);
    return 0;
}

int main(int argc,char **argv)
{
    OPENNT_BASE_SERVICE *service=NULL;
    OPENNT_BASE_CONNECTION *launcher=NULL,*worker=NULL,*later=NULL,*wowWorker=NULL;
    PROCESS_INFORMATION child={0},laterChild={0},wowChild={0};
    STARTUPINFOA startup={sizeof(startup)};
    HANDLE self=NULL;
    DWORD launcherGeneration=0,workerGeneration=0,laterGeneration=0,wowGeneration=0,queryCalls=0;
    uint64_t reservation=0,claimed=0,wowReservation=0;
    ULONG task=0,wowTask=0;
    HANDLE console=NULL;
    char command[MAX_PATH+32];
    BASE_API_MSG check={0},reply={0},update={0},get={0};
    char cmd[]="MEM.EXE\r\n",app[]="MEM.EXE",directory[]="C:\\",environment[]="X=Y\0\0";
    char getCmd[128]={0},getApp[128]={0},getEnv[128]={0},getPif[MAX_PATH]={0},getDirectory[MAX_PATH]={0};
    void *wire=NULL,*answer=NULL,*updateWire=NULL,*updateAnswer=NULL,*getWire=NULL,*getAnswer=NULL;
    uint32_t wireBytes=0,answerBytes=0,updateWireBytes=0,updateAnswerBytes=0,getWireBytes=0;
    HANDLE parentEvent=NULL,laterParentEvent=NULL,getWait=NULL,standard[3]={NULL,NULL,NULL};
    HANDLE stdinRead=NULL,stdinWrite=NULL,stdoutRead=NULL,stdoutWrite=NULL;
    DWORD stdinReceipt=0,stdoutReceipt=0,bytes=0;
    char streamText[16]={0};
    uint32_t parentReceipt=0,laterParentReceipt=0;
    uint64_t managementEpoch=0;
    OPENNT_BASE_WORKER_INFO workerInfo={0};
    uint32_t workerInfoCount=0;
    ULONG standardCount=0;
    STARTUPINFOA getStartup={sizeof(getStartup)};
    if (argc==2 && !strcmp(argv[1],"--reservation-child")) { Sleep(15000);return 0; }
    self=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,GetCurrentProcessId());
    service=OpenNtBaseServiceStart();
    CHECK(self && service!=NULL);
    CHECK(OpenNtBaseServiceConfigureConsoleQuery(service,same_console_query,&queryCalls));
    CHECK(OpenNtBaseServiceIsEmpty(service));
    /* Repeating after rundown proves the failed-start record is not reused. */
    CHECK(detached_reservation(service,self)==0);
    CHECK(detached_reservation(service,self)==0);
    CHECK(OpenNtBaseServiceConnect(service,self,&launcher,&launcherGeneration)==ERROR_SUCCESS);
    CHECK(!OpenNtBaseServiceIsEmpty(service));
    /* Standard streams cross the standalone service as receipts.  Keep the
     * opposite pipe ends in this fixture so that the first worker delivery
     * can prove it got usable stream attachments rather than local HANDLE
     * numbers from the broker record. */
    CHECK(CreatePipe(&stdinRead,&stdinWrite,NULL,0));
    CHECK(CreatePipe(&stdoutRead,&stdoutWrite,NULL,0));
    CHECK(OpenNtBaseServiceAttachStream(launcher,GetCurrentProcessId(),launcherGeneration,
        BROKER_VDM_STDIN,stdinRead,&stdinReceipt)==ERROR_SUCCESS && stdinReceipt);
    CHECK(OpenNtBaseServiceAttachStream(launcher,GetCurrentProcessId(),launcherGeneration,
        BROKER_VDM_STDOUT,stdoutWrite,&stdoutReceipt)==ERROR_SUCCESS && stdoutReceipt);
    check.u.CheckVDM.CmdLine=cmd;check.u.CheckVDM.CmdLen=sizeof(cmd);
    check.u.CheckVDM.AppName=app;check.u.CheckVDM.AppLen=sizeof(app);
    check.u.CheckVDM.CurDirectory=directory;check.u.CheckVDM.CurDirectoryLen=sizeof(directory);
    check.u.CheckVDM.Env=environment;check.u.CheckVDM.EnvLen=sizeof(environment);
    check.u.CheckVDM.BinaryType=BINARY_TYPE_DOS;check.u.CheckVDM.CodePage=437;
    check.u.CheckVDM.CurDrive=2;check.u.CheckVDM.ConsoleHandle=OPENNT_BASE_CONSOLE_EXISTING;
    check.u.CheckVDM.StdIn=(HANDLE)(ULONG_PTR)stdinReceipt;
    check.u.CheckVDM.StdOut=(HANDLE)(ULONG_PTR)stdoutReceipt;
    check.u.CheckVDM.StdErr=(HANDLE)(ULONG_PTR)stdoutReceipt;
    CHECK(OpenNtBaseEncodeCheckCommand(&check,1,launcherGeneration,NULL,0,&wireBytes));
    wire=malloc(wireBytes);CHECK(wire && OpenNtBaseEncodeCheckCommand(&check,1,launcherGeneration,wire,wireBytes,&wireBytes));
    CHECK(OpenNtBaseServiceCheck(launcher,GetCurrentProcessId(),launcherGeneration,wire,wireBytes,NULL,0,&answerBytes,&parentEvent,&parentReceipt)==ERROR_INSUFFICIENT_BUFFER && answerBytes);
    answer=malloc(answerBytes);CHECK(answer && OpenNtBaseServiceCheck(launcher,GetCurrentProcessId(),launcherGeneration,wire,wireBytes,answer,answerBytes,&answerBytes,&parentEvent,&parentReceipt)==ERROR_SUCCESS);
    CHECK(OpenNtBaseApplyCheckReply(answer,answerBytes,launcherGeneration,1,&reply));
    CHECK(reply.ReturnValue==STATUS_SUCCESS && !reply.u.CheckVDM.iTask && reply.u.CheckVDM.VDMState==VDM_NOT_PRESENT);
    task=reply.u.CheckVDM.iTask;
    CHECK(OpenNtBaseServiceCreateReservation(launcher,GetCurrentProcessId(),launcherGeneration,
        task,&reservation)==ERROR_SUCCESS);
    CHECK(GetModuleFileNameA(NULL,command,MAX_PATH));
    strcat_s(command,sizeof(command)," --reservation-child");
    CHECK(CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&startup,&child));
    CHECK(OpenNtBaseServicePrepareWorker(launcher,GetCurrentProcessId(),launcherGeneration,
        reservation,child.hProcess)==ERROR_SUCCESS);
    /* Match the original launcher ordering: Check publishes the record,
     * Update records its VDM process, then the worker connects and acquires.
     * This gives ExitVDM a real original ConsoleRecord and paired waits to
     * complete, rather than unit-testing a synthetic completion flag. */
    update.u.UpdateVDMEntry.EntryIndex=UPDATE_VDM_PROCESS_HANDLE;
    update.u.UpdateVDMEntry.BinaryType=BINARY_TYPE_DOS;
    update.u.UpdateVDMEntry.iTask=task;
    CHECK(OpenNtBaseEncodeUpdateCommand(&update,2,launcherGeneration,NULL,0,&updateWireBytes));
    updateWire=malloc(updateWireBytes);CHECK(updateWire && OpenNtBaseEncodeUpdateCommand(
        &update,2,launcherGeneration,updateWire,updateWireBytes,&updateWireBytes));
    CHECK(OpenNtBaseServiceUpdate(launcher,GetCurrentProcessId(),launcherGeneration,
        updateWire,updateWireBytes,NULL,0,&updateAnswerBytes,&parentEvent,&parentReceipt)==ERROR_INSUFFICIENT_BUFFER && updateAnswerBytes);
    updateAnswer=malloc(updateAnswerBytes);CHECK(updateAnswer && OpenNtBaseServiceUpdate(
        launcher,GetCurrentProcessId(),launcherGeneration,updateWire,updateWireBytes,
        updateAnswer,updateAnswerBytes,&updateAnswerBytes,&parentEvent,&parentReceipt)==ERROR_SUCCESS);
    CHECK(parentEvent!=NULL && parentReceipt!=0);
    /* Update has consumed the launcher's source receipts.  Subsequent
     * independent Check calls must not replay those revoked source tokens. */
    check.u.CheckVDM.StdIn=NULL;
    check.u.CheckVDM.StdOut=NULL;
    check.u.CheckVDM.StdErr=NULL;
    CHECK(ResumeThread(child.hThread)!=(DWORD)-1);
    CHECK(OpenNtBaseServiceConnect(service,child.hProcess,&worker,&workerGeneration)==ERROR_SUCCESS);
    CHECK(OpenNtBaseServiceWorkerReservation(worker,&claimed,&task,&console));
    CHECK(claimed==reservation && task==reply.u.CheckVDM.iTask && console!=NULL);
    /* The management plane copies a server-owned worker projection.  First
     * probe the count, then require the same broker generation/sequence
     * which original BaseSrv assigned at authenticated registration. */
    CHECK(OpenNtBaseServiceSnapshot(service,&managementEpoch,NULL,0,&workerInfoCount)==ERROR_INSUFFICIENT_BUFFER);
    CHECK(managementEpoch!=0 && workerInfoCount==1);
    CHECK(OpenNtBaseServiceSnapshot(service,&managementEpoch,&workerInfo,1,&workerInfoCount)==ERROR_SUCCESS);
    CHECK(workerInfoCount==1 && workerInfo.sequence==workerGeneration &&
        workerInfo.started_filetime!=0 && !wcscmp(workerInfo.image,L"MEM.EXE"));
    CHECK(OpenNtBaseServiceTerminateWorker(service,managementEpoch+1,workerGeneration)==ERROR_REVISION_MISMATCH);
    /* READY alone is not worker readiness: a resident COMMAND prompt has no
     * outstanding GetNextVDMCommand wait to receive an unrelated launch. */
    { BOOL recordExists=FALSE;
      CHECK(!BaseSrvDOSWorkerWaitPending(console,&recordExists) && recordExists); }
    get.u.GetNextVDMCommand.StartupInfo=&getStartup;
    get.u.GetNextVDMCommand.VDMState=ASKING_FOR_PIF|ASKING_FOR_DOS_BINARY;
    CHECK(OpenNtBaseEncodeGetCommand(&get,3,workerGeneration,NULL,0,&getWireBytes));
    getWire=malloc(getWireBytes);CHECK(getWire && OpenNtBaseEncodeGetCommand(
        &get,3,workerGeneration,getWire,getWireBytes,&getWireBytes));
    CHECK(OpenNtBaseServiceGet(worker,child.dwProcessId,workerGeneration,getWire,getWireBytes,
        &getAnswer,&wireBytes,&getWait,standard,&standardCount)==ERROR_SUCCESS);
    CHECK(getAnswer!=NULL && getWait==NULL && standardCount==0);
    OpenNtBaseServiceReleaseCommandReply(getAnswer);getAnswer=NULL;
    free(getWire);getWire=NULL;
    ZeroMemory(&get,sizeof(get));
    get.u.GetNextVDMCommand.StartupInfo=&getStartup;
    get.u.GetNextVDMCommand.VDMState=ASKING_FOR_FIRST_COMMAND|ASKING_FOR_DOS_BINARY;
    get.u.GetNextVDMCommand.CmdLine=getCmd;get.u.GetNextVDMCommand.CmdLen=sizeof(getCmd);
    get.u.GetNextVDMCommand.AppName=getApp;get.u.GetNextVDMCommand.AppLen=sizeof(getApp);
    get.u.GetNextVDMCommand.Env=getEnv;get.u.GetNextVDMCommand.EnvLen=sizeof(getEnv);
    get.u.GetNextVDMCommand.PifFile=getPif;get.u.GetNextVDMCommand.PifLen=sizeof(getPif);
    get.u.GetNextVDMCommand.CurDirectory=getDirectory;
    get.u.GetNextVDMCommand.CurDirectoryLen=sizeof(getDirectory);
    CHECK(OpenNtBaseEncodeGetCommand(&get,4,workerGeneration,NULL,0,&getWireBytes));
    getWire=malloc(getWireBytes);CHECK(getWire && OpenNtBaseEncodeGetCommand(
        &get,4,workerGeneration,getWire,getWireBytes,&getWireBytes));
    CHECK(OpenNtBaseServiceGet(worker,child.dwProcessId,workerGeneration,getWire,getWireBytes,
        &getAnswer,&wireBytes,&getWait,standard,&standardCount)==ERROR_SUCCESS);
    CHECK(getAnswer!=NULL && getWait==NULL && standardCount==3 &&
        standard[0]!=NULL && standard[1]!=NULL && standard[2]!=NULL);
    CHECK(WriteFile(standard[1],"S34\n",4,&bytes,NULL) && bytes==4);
    CHECK(ReadFile(stdoutRead,streamText,4,&bytes,NULL) && bytes==4 &&
        !memcmp(streamText,"S34\n",4));
    OpenNtBaseServiceReleaseCommandReply(getAnswer);getAnswer=NULL;
    free(getWire);getWire=NULL;
    ZeroMemory(&get,sizeof(get));
    get.u.GetNextVDMCommand.StartupInfo=&getStartup;
    /* Complete with a nonzero result and enter the original GetNext wait.
     * The later task's zero must not inherit this result (S8). */
    get.u.GetNextVDMCommand.VDMState=ASKING_FOR_DOS_BINARY;
    get.u.GetNextVDMCommand.ExitCode=7;
    CHECK(OpenNtBaseEncodeGetCommand(&get,5,workerGeneration,NULL,0,&getWireBytes));
    getWire=malloc(getWireBytes);CHECK(getWire && OpenNtBaseEncodeGetCommand(
        &get,5,workerGeneration,getWire,getWireBytes,&getWireBytes));
    CHECK(OpenNtBaseServiceGet(worker,child.dwProcessId,workerGeneration,getWire,getWireBytes,
        &getAnswer,&wireBytes,&getWait,standard,&standardCount)==ERROR_SUCCESS);
    CHECK(getAnswer!=NULL && getWait!=NULL && standardCount==0);
    OpenNtBaseServiceReleaseCommandReply(getAnswer);getAnswer=NULL;
    CHECK(WaitForSingleObject(getWait,0)==WAIT_TIMEOUT);
    { BOOL recordExists=FALSE;
      CHECK(BaseSrvDOSWorkerWaitPending(console,&recordExists) && recordExists); }
    CHECK(WaitForSingleObject(parentEvent,0)==WAIT_OBJECT_0);
    { DWORD exitCode=STILL_ACTIVE;
      CHECK(OpenNtBaseServiceExitCode(launcher,GetCurrentProcessId(),launcherGeneration,
          parentReceipt,&exitCode)==ERROR_SUCCESS && exitCode==7); }
    CloseHandle(parentEvent);parentEvent=NULL;

    /* A same-Console launch may now use the original ready record: CheckDOS
     * signals the actual worker wait and the second-time Get consumes it. */
    CHECK(CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&startup,&laterChild));
    CHECK(OpenNtBaseServiceConnect(service,laterChild.hProcess,&later,&laterGeneration)==ERROR_SUCCESS);
    check.u.CheckVDM.ConsoleHandle=OPENNT_BASE_CONSOLE_EXISTING;
    CHECK(OpenNtBaseEncodeCheckCommand(&check,6,laterGeneration,NULL,0,&wireBytes));
    free(wire);wire=malloc(wireBytes);CHECK(wire && OpenNtBaseEncodeCheckCommand(
        &check,6,laterGeneration,wire,wireBytes,&wireBytes));
    CHECK(OpenNtBaseServiceCheck(later,laterChild.dwProcessId,laterGeneration,
        wire,wireBytes,NULL,0,&answerBytes,&laterParentEvent,&laterParentReceipt)==ERROR_INSUFFICIENT_BUFFER && answerBytes);
    free(answer);answer=malloc(answerBytes);CHECK(answer && OpenNtBaseServiceCheck(
        later,laterChild.dwProcessId,laterGeneration,wire,wireBytes,answer,answerBytes,&answerBytes,
        &laterParentEvent,&laterParentReceipt)==ERROR_SUCCESS);
    CHECK(queryCalls==1 && OpenNtBaseApplyCheckReply(answer,answerBytes,laterGeneration,6,&reply));
    CHECK(reply.ReturnValue==STATUS_SUCCESS && reply.u.CheckVDM.VDMState==VDM_PRESENT_AND_READY &&
        laterParentEvent!=NULL && laterParentReceipt!=0);
    CHECK(laterParentReceipt!=parentReceipt);
    CHECK(WaitForSingleObject(getWait,0)==WAIT_OBJECT_0);
    CloseHandle(getWait);getWait=NULL;

    ZeroMemory(&get,sizeof(get));
    get.u.GetNextVDMCommand.StartupInfo=&getStartup;
    get.u.GetNextVDMCommand.VDMState=ASKING_FOR_SECOND_TIME|ASKING_FOR_DOS_BINARY;
    get.u.GetNextVDMCommand.CmdLine=getCmd;get.u.GetNextVDMCommand.CmdLen=sizeof(getCmd);
    get.u.GetNextVDMCommand.AppName=getApp;get.u.GetNextVDMCommand.AppLen=sizeof(getApp);
    get.u.GetNextVDMCommand.Env=getEnv;get.u.GetNextVDMCommand.EnvLen=sizeof(getEnv);
    get.u.GetNextVDMCommand.PifFile=getPif;get.u.GetNextVDMCommand.PifLen=sizeof(getPif);
    get.u.GetNextVDMCommand.CurDirectory=getDirectory;
    get.u.GetNextVDMCommand.CurDirectoryLen=sizeof(getDirectory);
    CHECK(OpenNtBaseEncodeGetCommand(&get,7,workerGeneration,NULL,0,&getWireBytes));
    free(getWire);getWire=malloc(getWireBytes);CHECK(getWire && OpenNtBaseEncodeGetCommand(
        &get,7,workerGeneration,getWire,getWireBytes,&getWireBytes));
    CHECK(OpenNtBaseServiceGet(worker,child.dwProcessId,workerGeneration,getWire,getWireBytes,
        &getAnswer,&wireBytes,&getWait,standard,&standardCount)==ERROR_SUCCESS);
    CHECK(getAnswer!=NULL && getWait==NULL && standardCount==0);
    OpenNtBaseServiceReleaseCommandReply(getAnswer);getAnswer=NULL;
    if (argc==2 && !strcmp(argv[1],"--management-terminate")) {
        DWORD exitCode=STILL_ACTIVE;
        /* The manager's positive path receives only the selected snapshot
         * identity.  The service resolves its retained watch and the normal
         * worker-exit callback must wake the waiting parent and remove the
         * original record before this fixture tears down its own processes. */
        CHECK(OpenNtBaseServiceTerminateWorker(service,managementEpoch,workerGeneration)==ERROR_SUCCESS);
        CHECK(WaitForSingleObject(laterParentEvent,5000)==WAIT_OBJECT_0);
        CHECK(OpenNtBaseServiceExitCode(later,laterChild.dwProcessId,laterGeneration,
            laterParentReceipt,&exitCode)==ERROR_PROCESS_ABORTED);
        CloseHandle(laterParentEvent);laterParentEvent=NULL;
        CHECK(OpenNtBaseServiceDisconnect(worker)==ERROR_SUCCESS);worker=NULL;
        CHECK(OpenNtBaseServiceReleaseReservation(launcher,GetCurrentProcessId(),launcherGeneration,
            reservation)==ERROR_SUCCESS);
        CHECK(OpenNtBaseServiceDisconnect(later)==ERROR_SUCCESS);later=NULL;
        CHECK(OpenNtBaseServiceDisconnect(launcher)==ERROR_SUCCESS);launcher=NULL;
        CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
        TerminateProcess(laterChild.hProcess,0);WaitForSingleObject(laterChild.hProcess,5000);
        CloseHandle(laterChild.hThread);CloseHandle(laterChild.hProcess);
        CloseHandle(child.hThread);CloseHandle(child.hProcess);CloseHandle(self);
        free(getWire);free(updateAnswer);free(updateWire);free(answer);free(wire);
        CHECK(OpenNtBaseServiceIsEmpty(service));
        CHECK(OpenNtBaseServiceStop(service));
        puts("PASS: management epoch/worker termination performs original worker-exit cleanup");
        return 0;
    }
    /* The authenticated worker disappears before ExitVDM.  The retained OS
     * process-exit watch is the standalone source for the original CSR
     * disconnect cleanup: the queued parent must wake and the DOS record
     * must be removed.  A mere RPC-context disconnect is deliberately not
     * enough because resident COMMAND can be alive without that context. */
    /* Preserve the existing lifecycle fixture's independent abrupt-worker
     * stimulus; management termination has its own product-level fixture. */
    CHECK(TerminateProcess(child.hProcess,0));
    CHECK(WaitForSingleObject(laterParentEvent,5000)==WAIT_OBJECT_0);
    { DWORD exitCode=STILL_ACTIVE;
      CHECK(OpenNtBaseServiceExitCode(later,laterChild.dwProcessId,laterGeneration,
          laterParentReceipt,&exitCode)==ERROR_PROCESS_ABORTED); }
    CloseHandle(laterParentEvent);laterParentEvent=NULL;
    CloseHandle(parentEvent);parentEvent=NULL;
    CHECK(OpenNtBaseServiceDisconnect(worker)==ERROR_SUCCESS);worker=NULL;
    CHECK(OpenNtBaseServicePrepareWorker(launcher,GetCurrentProcessId(),launcherGeneration,
        reservation,child.hProcess)==ERROR_PROCESS_ABORTED);
    { DWORD release=OpenNtBaseServiceReleaseReservation(launcher,GetCurrentProcessId(),
          launcherGeneration,reservation);
      /* The worker-exit watch owns reservation teardown.  Depending on the
       * scheduling point, either this launcher call performs the final
       * release or the watch has already done so. */
      CHECK(release==ERROR_SUCCESS || release==ERROR_NOT_FOUND); }
    CHECK(OpenNtBaseServiceDisconnect(later)==ERROR_SUCCESS);later=NULL;
    TerminateProcess(laterChild.hProcess,0);WaitForSingleObject(laterChild.hProcess,INFINITE);
    CloseHandle(laterChild.hThread);CloseHandle(laterChild.hProcess);laterChild.hThread=laterChild.hProcess=NULL;

    /* The worker learns WOW from its launcher reservation.  This retains the
     * original -1 Console sentinel for sequence publication and ExitVDM;
     * it is not inferred from a worker-supplied request bit. */
    check.u.CheckVDM.BinaryType=BINARY_TYPE_WIN16;
    check.u.CheckVDM.ConsoleHandle=OPENNT_BASE_CONSOLE_EXISTING;
    CHECK(OpenNtBaseEncodeCheckCommand(&check,8,launcherGeneration,NULL,0,&wireBytes));
    free(wire);wire=malloc(wireBytes);CHECK(wire && OpenNtBaseEncodeCheckCommand(
        &check,8,launcherGeneration,wire,wireBytes,&wireBytes));
    CHECK(OpenNtBaseServiceCheck(launcher,GetCurrentProcessId(),launcherGeneration,
        wire,wireBytes,NULL,0,&answerBytes,&parentEvent,&parentReceipt)==ERROR_INSUFFICIENT_BUFFER && answerBytes);
    free(answer);answer=malloc(answerBytes);CHECK(answer && OpenNtBaseServiceCheck(
        launcher,GetCurrentProcessId(),launcherGeneration,wire,wireBytes,answer,answerBytes,
        &answerBytes,&parentEvent,&parentReceipt)==ERROR_SUCCESS);
    CHECK(OpenNtBaseApplyCheckReply(answer,answerBytes,launcherGeneration,8,&reply));
    CHECK(reply.ReturnValue==STATUS_SUCCESS && reply.u.CheckVDM.VDMState==VDM_NOT_PRESENT);
    wowTask=reply.u.CheckVDM.iTask;
    CHECK(OpenNtBaseServiceCreateReservation(launcher,GetCurrentProcessId(),launcherGeneration,
        wowTask,&wowReservation)==ERROR_SUCCESS);
    CHECK(CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&startup,&wowChild));
    CHECK(OpenNtBaseServicePrepareWorker(launcher,GetCurrentProcessId(),launcherGeneration,
        wowReservation,wowChild.hProcess)==ERROR_SUCCESS);
    ZeroMemory(&update,sizeof(update));
    update.u.UpdateVDMEntry.EntryIndex=UPDATE_VDM_PROCESS_HANDLE;
    update.u.UpdateVDMEntry.BinaryType=BINARY_TYPE_WIN16;
    update.u.UpdateVDMEntry.iTask=wowTask;
    CHECK(OpenNtBaseEncodeUpdateCommand(&update,9,launcherGeneration,NULL,0,&updateWireBytes));
    free(updateWire);updateWire=malloc(updateWireBytes);CHECK(updateWire && OpenNtBaseEncodeUpdateCommand(
        &update,9,launcherGeneration,updateWire,updateWireBytes,&updateWireBytes));
    CHECK(OpenNtBaseServiceUpdate(launcher,GetCurrentProcessId(),launcherGeneration,
        updateWire,updateWireBytes,NULL,0,&updateAnswerBytes,&parentEvent,&parentReceipt)==ERROR_INSUFFICIENT_BUFFER && updateAnswerBytes);
    free(updateAnswer);updateAnswer=malloc(updateAnswerBytes);CHECK(updateAnswer && OpenNtBaseServiceUpdate(
        launcher,GetCurrentProcessId(),launcherGeneration,updateWire,updateWireBytes,
        updateAnswer,updateAnswerBytes,&updateAnswerBytes,&parentEvent,&parentReceipt)==ERROR_SUCCESS);
    CHECK(ResumeThread(wowChild.hThread)!=(DWORD)-1);
    CHECK(OpenNtBaseServiceConnect(service,wowChild.hProcess,&wowWorker,&wowGeneration)==ERROR_SUCCESS);
    ZeroMemory(&get,sizeof(get));
    get.u.GetNextVDMCommand.StartupInfo=&getStartup;
    get.u.GetNextVDMCommand.VDMState=ASKING_FOR_WOW_BINARY;
    get.u.GetNextVDMCommand.CmdLine=getCmd;get.u.GetNextVDMCommand.CmdLen=sizeof(getCmd);
    get.u.GetNextVDMCommand.AppName=getApp;get.u.GetNextVDMCommand.AppLen=sizeof(getApp);
    get.u.GetNextVDMCommand.Env=getEnv;get.u.GetNextVDMCommand.EnvLen=sizeof(getEnv);
    get.u.GetNextVDMCommand.PifFile=getPif;get.u.GetNextVDMCommand.PifLen=sizeof(getPif);
    get.u.GetNextVDMCommand.CurDirectory=getDirectory;get.u.GetNextVDMCommand.CurDirectoryLen=sizeof(getDirectory);
    CHECK(OpenNtBaseEncodeGetCommand(&get,10,wowGeneration,NULL,0,&getWireBytes));
    free(getWire);getWire=malloc(getWireBytes);CHECK(getWire && OpenNtBaseEncodeGetCommand(
        &get,10,wowGeneration,getWire,getWireBytes,&getWireBytes));
    CHECK(OpenNtBaseServiceGet(wowWorker,wowChild.dwProcessId,wowGeneration,getWire,getWireBytes,
        &getAnswer,&wireBytes,&getWait,standard,&standardCount)==ERROR_SUCCESS);
    CHECK(getAnswer!=NULL && getWait==NULL && standardCount==0);
    OpenNtBaseServiceReleaseCommandReply(getAnswer);getAnswer=NULL;
    { BOOL closeWowWait=FALSE;
      CHECK(OpenNtBaseServiceExit(wowWorker,wowChild.dwProcessId,wowGeneration,TRUE,wowTask,
          &closeWowWait)==ERROR_SUCCESS && !closeWowWait); }
    CHECK(OpenNtBaseServiceDisconnect(wowWorker)==ERROR_SUCCESS);wowWorker=NULL;
    CHECK(OpenNtBaseServiceReleaseReservation(launcher,GetCurrentProcessId(),launcherGeneration,wowReservation)==ERROR_SUCCESS);
    TerminateProcess(wowChild.hProcess,0);WaitForSingleObject(wowChild.hProcess,INFINITE);
    CloseHandle(wowChild.hThread);CloseHandle(wowChild.hProcess);wowChild.hThread=wowChild.hProcess=NULL;
    Sleep(100);
    /* Launcher dies after Check/Prepare but before worker Connect. Exercise
     * the same Disconnect entry used by real RPC rundown, including original
     * UndoCreation and termination of the still-suspended, unclaimed child. */
    CHECK(OpenNtBaseEncodeCheckCommand(&check,8,launcherGeneration,NULL,0,&wireBytes));
    free(wire);wire=malloc(wireBytes);
    CHECK(wire && OpenNtBaseEncodeCheckCommand(&check,8,launcherGeneration,wire,wireBytes,&wireBytes));
    CHECK(OpenNtBaseServiceCheck(launcher,GetCurrentProcessId(),launcherGeneration,
        wire,wireBytes,answer,answerBytes,&answerBytes,&parentEvent,&parentReceipt)==ERROR_SUCCESS);
    CHECK(OpenNtBaseApplyCheckReply(answer,answerBytes,launcherGeneration,8,&reply));
    CHECK(reply.ReturnValue==STATUS_SUCCESS && reply.u.CheckVDM.VDMState==VDM_NOT_PRESENT);
    CHECK(OpenNtBaseServiceCreateReservation(launcher,GetCurrentProcessId(),launcherGeneration,
        reply.u.CheckVDM.iTask,&wowReservation)==ERROR_SUCCESS);
    CHECK(CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&startup,&wowChild));
    CHECK(OpenNtBaseServicePrepareWorker(launcher,GetCurrentProcessId(),launcherGeneration,
        wowReservation,wowChild.hProcess)==ERROR_SUCCESS);
    CHECK(OpenNtBaseServiceDisconnect(launcher)==ERROR_SUCCESS);launcher=NULL;
    CHECK(WaitForSingleObject(wowChild.hProcess,5000)==WAIT_OBJECT_0);
    CloseHandle(wowChild.hThread);CloseHandle(wowChild.hProcess);
    wowChild.hThread=wowChild.hProcess=NULL;
    CHECK(OpenNtBaseServiceIsEmpty(service));
    CHECK(OpenNtBaseServiceStop(service));service=NULL;
    WaitForSingleObject(child.hProcess,INFINITE);
    CloseHandle(child.hThread);CloseHandle(child.hProcess);CloseHandle(self);
    if (laterChild.hThread) CloseHandle(laterChild.hThread);
    if (laterChild.hProcess) CloseHandle(laterChild.hProcess);
    free(getWire);free(updateAnswer);free(updateWire);free(answer);free(wire);
    puts("PASS: original Check/Update/Get/ExitVDM lifecycle completes through authenticated worker binding");
    return 0;
}
