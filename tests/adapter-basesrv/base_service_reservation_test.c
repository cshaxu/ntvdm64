#include <base_service.h>
#include "basesrv.h"
#include <base_command.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHECK(value) do { if (!(value)) { fprintf(stderr,"FAIL %d\\n",__LINE__);return 1; } } while (0)

int main(int argc,char **argv)
{
    OPENNT_BASE_SERVICE *service=NULL;
    OPENNT_BASE_CONNECTION *launcher=NULL,*worker=NULL;
    PROCESS_INFORMATION child={0};
    STARTUPINFOA startup={sizeof(startup)};
    HANDLE self=NULL;
    DWORD launcherGeneration=0,workerGeneration=0;
    uint64_t reservation=0,claimed=0;
    ULONG task=0;
    HANDLE console=NULL;
    char command[MAX_PATH+32];
    BASE_API_MSG check={0},reply={0},update={0},get={0};
    char cmd[]="MEM.EXE\r\n",app[]="MEM.EXE",directory[]="C:\\",environment[]="X=Y\0\0";
    char getCmd[128]={0},getApp[128]={0},getEnv[128]={0},getPif[MAX_PATH]={0},getDirectory[MAX_PATH]={0};
    void *wire=NULL,*answer=NULL,*updateWire=NULL,*updateAnswer=NULL,*getWire=NULL,*getAnswer=NULL;
    uint32_t wireBytes=0,answerBytes=0,updateWireBytes=0,updateAnswerBytes=0,getWireBytes=0;
    HANDLE parentEvent=NULL,getWait=NULL,standard[3]={NULL,NULL,NULL};
    uint32_t parentReceipt=0;
    ULONG standardCount=0;
    BOOL closeWorkerWait=FALSE;
    STARTUPINFOA getStartup={sizeof(getStartup)};
    if (argc==2 && !strcmp(argv[1],"--reservation-child")) { Sleep(15000);return 0; }
    self=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,GetCurrentProcessId());
    service=OpenNtBaseServiceStart();
    CHECK(self && service!=NULL);
    CHECK(OpenNtBaseServiceIsEmpty(service));
    CHECK(OpenNtBaseServiceConnect(service,self,&launcher,&launcherGeneration)==ERROR_SUCCESS);
    CHECK(!OpenNtBaseServiceIsEmpty(service));
    check.u.CheckVDM.CmdLine=cmd;check.u.CheckVDM.CmdLen=sizeof(cmd);
    check.u.CheckVDM.AppName=app;check.u.CheckVDM.AppLen=sizeof(app);
    check.u.CheckVDM.CurDirectory=directory;check.u.CheckVDM.CurDirectoryLen=sizeof(directory);
    check.u.CheckVDM.Env=environment;check.u.CheckVDM.EnvLen=sizeof(environment);
    check.u.CheckVDM.BinaryType=BINARY_TYPE_DOS;check.u.CheckVDM.CodePage=437;
    check.u.CheckVDM.CurDrive=2;check.u.CheckVDM.ConsoleHandle=OPENNT_BASE_CONSOLE_EXISTING;
    CHECK(OpenNtBaseEncodeCheckCommand(&check,1,launcherGeneration,NULL,0,&wireBytes));
    wire=malloc(wireBytes);CHECK(wire && OpenNtBaseEncodeCheckCommand(&check,1,launcherGeneration,wire,wireBytes,&wireBytes));
    CHECK(OpenNtBaseServiceCheck(launcher,GetCurrentProcessId(),launcherGeneration,wire,wireBytes,NULL,0,&answerBytes)==ERROR_INSUFFICIENT_BUFFER && answerBytes);
    answer=malloc(answerBytes);CHECK(answer && OpenNtBaseServiceCheck(launcher,GetCurrentProcessId(),launcherGeneration,wire,wireBytes,answer,answerBytes,&answerBytes)==ERROR_SUCCESS);
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
    CHECK(ResumeThread(child.hThread)!=(DWORD)-1);
    CHECK(OpenNtBaseServiceConnect(service,child.hProcess,&worker,&workerGeneration)==ERROR_SUCCESS);
    CHECK(OpenNtBaseServiceWorkerReservation(worker,&claimed,&task,&console));
    CHECK(claimed==reservation && task==reply.u.CheckVDM.iTask && console!=NULL);
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
    CHECK(getAnswer!=NULL && getWait==NULL && standardCount==0);
    OpenNtBaseServiceReleaseCommandReply(getAnswer);getAnswer=NULL;
    free(getWire);getWire=NULL;
    ZeroMemory(&get,sizeof(get));
    get.u.GetNextVDMCommand.StartupInfo=&getStartup;
    /* The original command client requests again after consuming its initial
     * record.  With no new record available BaseSrv creates the paired VDM
     * wait that ExitVDM must close/release. */
    get.u.GetNextVDMCommand.VDMState=ASKING_FOR_SECOND_TIME|ASKING_FOR_DOS_BINARY;
    CHECK(OpenNtBaseEncodeGetCommand(&get,5,workerGeneration,NULL,0,&getWireBytes));
    getWire=malloc(getWireBytes);CHECK(getWire && OpenNtBaseEncodeGetCommand(
        &get,5,workerGeneration,getWire,getWireBytes,&getWireBytes));
    CHECK(OpenNtBaseServiceGet(worker,child.dwProcessId,workerGeneration,getWire,getWireBytes,
        &getAnswer,&wireBytes,&getWait,standard,&standardCount)==ERROR_SUCCESS);
    CHECK(getAnswer!=NULL && getWait!=NULL && standardCount==0);
    OpenNtBaseServiceReleaseCommandReply(getAnswer);getAnswer=NULL;
    CHECK(OpenNtBaseServiceExit(worker,child.dwProcessId,workerGeneration,FALSE,0,
        &closeWorkerWait)==ERROR_SUCCESS && closeWorkerWait);
    /* The service revoked its receipt after original BaseSrvExitDOSTask
     * removed the ConsoleRecord.  This is the client-side typed duplicate
     * which original ExitVDM closes; this direct test is that client. */
    CloseHandle(getWait);getWait=NULL;
    CloseHandle(parentEvent);parentEvent=NULL;
    CHECK(OpenNtBaseServiceDisconnect(worker)==ERROR_SUCCESS);worker=NULL;
    CHECK(OpenNtBaseServicePrepareWorker(launcher,GetCurrentProcessId(),launcherGeneration,
        reservation,child.hProcess)==ERROR_ALREADY_EXISTS);
    CHECK(OpenNtBaseServiceReleaseReservation(launcher,GetCurrentProcessId(),launcherGeneration,reservation)==ERROR_SUCCESS);
    CHECK(OpenNtBaseServiceDisconnect(launcher)==ERROR_SUCCESS);launcher=NULL;
    CHECK(OpenNtBaseServiceIsEmpty(service));
    CHECK(OpenNtBaseServiceStop(service));service=NULL;
    TerminateProcess(child.hProcess,0);WaitForSingleObject(child.hProcess,INFINITE);
    CloseHandle(child.hThread);CloseHandle(child.hProcess);CloseHandle(self);
    free(getWire);free(updateAnswer);free(updateWire);free(answer);free(wire);
    puts("PASS: original Check/Update/Get/ExitVDM lifecycle completes through authenticated worker binding");
    return 0;
}
