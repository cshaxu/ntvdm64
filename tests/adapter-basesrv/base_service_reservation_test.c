#include <base_service.h>
#include "basesrv.h"
#include <base_command.h>
#include "ntsrv-exe/transport/vdm_receipt.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern PCONSOLERECORD DOSHead;
extern PWOWHEAD WOWHead;

/* Keep the older focused fixture readable while exercising the production
 * WIN32RECORD transaction.  One accepted test channel has one request ID. */
static DWORD test_native_request;
#define OpenNtBaseServiceSubmitWorkerChannel(a,b,c,d,e) \
    OpenNtBaseServiceSubmitWorkerChannel(a,b,c,d,e,L"fixture.exe")
#define OpenNtBaseServiceTakeWorkerChannel(a,b,c,d,e,f,g) \
    OpenNtBaseServiceTakeWorkerChannel(a,b,c,d,e,f,g,&test_native_request)
#define OpenNtBaseServiceCompleteWorkerChannel(a,b,c) \
    OpenNtBaseServiceCompleteWorkerChannel(a,b,c,test_native_request,37)

#define CHECK(value) do { if (!(value)) { fprintf(stderr,"FAIL %d\\n",__LINE__);return 1; } } while (0)

static int frontend_pair(HANDLE *server,HANDLE *client)
{
    char name[96];
    SECURITY_DESCRIPTOR descriptor;
    SECURITY_ATTRIBUTES security={sizeof(security),&descriptor,FALSE};
    sprintf_s(name,sizeof(name),"\\\\.\\pipe\\ntvdm-frontend-test-%lu",GetCurrentProcessId());
    CHECK(InitializeSecurityDescriptor(&descriptor,SECURITY_DESCRIPTOR_REVISION));
    /* The fixture's client is created under this runner's restricted token.
     * Service admission still verifies the authenticated creator/server PIDs. */
    CHECK(SetSecurityDescriptorDacl(&descriptor,TRUE,NULL,FALSE));
    *server=CreateNamedPipeA(name,PIPE_ACCESS_DUPLEX|FILE_FLAG_FIRST_PIPE_INSTANCE,
        PIPE_TYPE_BYTE|PIPE_WAIT,1,1024,1024,0,&security);
    CHECK(*server!=INVALID_HANDLE_VALUE);
    *client=CreateFileA(name,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
    CHECK(*client!=INVALID_HANDLE_VALUE);
    CHECK(ConnectNamedPipe(*server,NULL) || GetLastError()==ERROR_PIPE_CONNECTED);
    return 0;
}

typedef struct FRONTEND_WAIT_TEST {
    OPENNT_BASE_CONNECTION *connection;
    DWORD pid,generation,error,frontend_generation;
    HANDLE pipe,frontend,ready;
} FRONTEND_WAIT_TEST;
static DWORD WINAPI frontend_wait(void *context)
{
    FRONTEND_WAIT_TEST *test=context;
    test->error=OpenNtBaseServiceWaitFrontend(test->connection,test->pid,test->generation,
        &test->pipe,&test->frontend,&test->frontend_generation,&test->ready);
    return 0;
}

typedef struct RUNDOWN_RACE_TEST {
    OPENNT_BASE_CONNECTION *connection;
    HANDLE ready,go;
    DWORD error;
} RUNDOWN_RACE_TEST;
static DWORD WINAPI competing_rundown(void *context)
{
    RUNDOWN_RACE_TEST *test=context;
    if (!SetEvent(test->ready) || WaitForSingleObject(test->go,5000)!=WAIT_OBJECT_0)
        return ERROR_TIMEOUT;
    test->error=OpenNtBaseServiceDisconnect(test->connection);
    return 0;
}

typedef struct NATIVE_CLOSE_TEST {
    HANDLE stop,closed,process;
    DWORD observed;
} NATIVE_CLOSE_TEST;

static DWORD WINAPI acknowledge_native_close(void *context)
{
    NATIVE_CLOSE_TEST *test=context;
    test->observed=WaitForSingleObject(test->stop,5000);
    if(test->observed!=WAIT_OBJECT_0)return ERROR_TIMEOUT;
    if(!TerminateProcess(test->process,ERROR_CANCELLED))return GetLastError();
    return SetEvent(test->closed) ? ERROR_SUCCESS : GetLastError();
}

/* The native participant fixture must create its child only after NTSRV has
 * bound the suspended direct root to the server-owned Job.  A named release
 * event keeps both processes alive while the test observes the one Win32Record
 * chain, then lets the fixture clean itself up without a tree kill. */
static int reservation_wait_child(const char *release_name)
{
    HANDLE release=OpenEventA(SYNCHRONIZE,FALSE,release_name);
    DWORD result;
    if(!release)return 2;
    result=WaitForSingleObject(release,15000);
    CloseHandle(release);
    return result==WAIT_OBJECT_0 ? 0 : 3;
}

static int reservation_descendant(const char *release_name)
{
    char executable[MAX_PATH],command[MAX_PATH+128];
    STARTUPINFOA startup={sizeof(startup)};
    PROCESS_INFORMATION child={0};
    HANDLE release;
    DWORD result;
    if(!GetModuleFileNameA(NULL,executable,MAX_PATH))return 2;
    sprintf_s(command,sizeof(command),"\"%s\" --reservation-wait-child %s",
        executable,release_name);
    if(!CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,
        &startup,&child))return 2;
    CloseHandle(child.hThread);
    release=OpenEventA(SYNCHRONIZE,FALSE,release_name);
    if(!release) { TerminateProcess(child.hProcess,ERROR_CANCELLED);CloseHandle(child.hProcess);return 2; }
    result=WaitForSingleObject(release,15000);
    CloseHandle(release);
    (void)WaitForSingleObject(child.hProcess,5000);
    CloseHandle(child.hProcess);
    return result==WAIT_OBJECT_0 ? 0 : 3;
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
    CHECK(OpenNtBaseServiceRegisterWowExec(connection,GetCurrentProcessId(),generation,0)==ERROR_INVALID_PARAMETER);
    CHECK(OpenNtBaseServiceRegisterWowExec(connection,GetCurrentProcessId(),generation+1,
        (DWORD)(ULONG_PTR)GetDesktopWindow())==ERROR_ACCESS_DENIED);
    CHECK(OpenNtBaseServiceRegisterWowExec(connection,GetCurrentProcessId(),generation,
        (DWORD)(ULONG_PTR)GetDesktopWindow())==ERROR_ACCESS_DENIED);
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
    {
        PROCESS_INFORMATION child={0};
        STARTUPINFOA startup={sizeof(startup)};
        BASE_API_MSG update={0};
        char child_command[MAX_PATH+32];
        HANDLE selected=NULL;
        OPENNT_BASE_CONNECTION *worker_connection=NULL;
        DWORD worker_generation=0;
        void *update_wire,*update_reply;
        uint32_t update_bytes=0,reply_bytes=0;
        CHECK(GetModuleFileNameA(NULL,child_command,MAX_PATH));
        strcat_s(child_command,sizeof(child_command)," --reservation-child");
        CHECK(CreateProcessA(NULL,child_command,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&startup,&child));
        CHECK(!OpenNtBaseServicePrepareWorker(connection,GetCurrentProcessId(),generation,reservation,child.hProcess));
        CHECK(OpenNtBaseServiceRetainCommandWorker(connection,GetCurrentProcessId(),generation,&selected)==ERROR_NOT_READY && !selected);
        update.u.UpdateVDMEntry.EntryIndex=UPDATE_VDM_PROCESS_HANDLE;
        update.u.UpdateVDMEntry.BinaryType=BINARY_TYPE_DOS;
        update.u.UpdateVDMEntry.iTask=reply.u.CheckVDM.iTask;
        CHECK(OpenNtBaseEncodeUpdateCommand(&update,2,generation,NULL,0,&update_bytes));
        update_wire=malloc(update_bytes);CHECK(update_wire);
        CHECK(OpenNtBaseEncodeUpdateCommand(&update,2,generation,update_wire,update_bytes,&update_bytes));
        CHECK(OpenNtBaseServiceUpdate(connection,GetCurrentProcessId(),generation,update_wire,update_bytes,
            NULL,0,&reply_bytes,&event,&receipt)==ERROR_INSUFFICIENT_BUFFER);
        update_reply=malloc(reply_bytes);CHECK(update_reply);
        CHECK(!OpenNtBaseServiceUpdate(connection,GetCurrentProcessId(),generation,update_wire,update_bytes,
            update_reply,reply_bytes,&reply_bytes,&event,&receipt));
        /* Original new-Console Update waits on the process, not a pair event. */
        CHECK(!event && !receipt);
        CHECK(!OpenNtBaseServiceRetainCommandWorker(connection,GetCurrentProcessId(),generation,&selected));
        CHECK(GetProcessId(selected)==child.dwProcessId);CloseHandle(selected);selected=NULL;
        CHECK(!OpenNtBaseServiceConnect(service,child.hProcess,&worker_connection,&worker_generation));
        CHECK(DOSHead && DOSHead->DosSesId==reply.u.CheckVDM.iTask &&
            DOSHead->SequenceNumber==worker_generation);
        CHECK(!OpenNtBaseServiceDisconnect(worker_connection));
        CHECK(TerminateProcess(child.hProcess,17));CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
        error=OpenNtBaseServiceRetainCommandWorker(connection,GetCurrentProcessId(),generation,&selected);
        /* The registered wait may already have reclaimed the reservation. */
        CHECK((error==ERROR_PROCESS_ABORTED || error==ERROR_NOT_FOUND || error==ERROR_NOT_READY) && !selected);
        CloseHandle(child.hThread);CloseHandle(child.hProcess);
        free(update_reply);free(update_wire);
        puts("PASS new-Console original process completion authorizes only the registered live worker");
    }
    CHECK(OpenNtBaseServiceDisconnect(connection)==ERROR_SUCCESS);
    { DWORD deadline=GetTickCount()+5000;while(!OpenNtBaseServiceIsEmpty(service) && (LONG)(deadline-GetTickCount())>0)Sleep(10); }
    CHECK(OpenNtBaseServiceIsEmpty(service));
    free(answer);free(wire);
    return 0;
}

static int frontend_console_identity(void)
{
    OPENNT_BASE_SERVICE *service=OpenNtBaseServiceStart();
    OPENNT_BASE_CONNECTION *root=NULL,*launcher=NULL;
    HANDLE self=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,
        FALSE,GetCurrentProcessId());
    HANDLE capability=CreateEventW(NULL,TRUE,FALSE,NULL),retained=NULL;
    STARTUPINFOA startup={sizeof(startup)};
    PROCESS_INFORMATION child={0};
    char image[MAX_PATH],command[MAX_PATH+32];
    DWORD root_generation=0,launcher_generation=0,verified_generation=0;
    DWORD member=GetCurrentProcessId(),duplicates[2]={member,member},missing=0;
    CHECK(service && self && capability);
    CHECK(!OpenNtBaseServiceConnect(service,self,&root,&root_generation));
    CHECK(GetModuleFileNameA(NULL,image,MAX_PATH));
    sprintf_s(command,sizeof(command),"\"%s\" --reservation-child",image);
    CHECK(CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,
        NULL,NULL,&startup,&child));
    CHECK(!OpenNtBaseServiceConnect(service,child.hProcess,&launcher,&launcher_generation));
    CHECK(OpenNtBaseServiceReportConsoleMembers(launcher,child.dwProcessId,launcher_generation,
        1,&member)==ERROR_ACCESS_DENIED);
    CHECK(OpenNtBaseServiceReportConsoleMembers(root,member,root_generation,
        1,&member)==ERROR_ACCESS_DENIED);
    CHECK(!OpenNtBaseServiceRegisterFrontendRoot(root,member,root_generation,capability));
    CHECK(OpenNtBaseServiceReportConsoleMembers(root,member,root_generation,
        0,&member)==ERROR_INVALID_PARAMETER);
    CHECK(OpenNtBaseServiceReportConsoleMembers(root,member,root_generation,
        2,duplicates)==ERROR_INVALID_DATA);
    CHECK(OpenNtBaseServiceReportConsoleMembers(root,member,root_generation,
        1,&missing)!=ERROR_SUCCESS);
    CHECK(OpenNtBaseServiceReportConsoleMembers(root,member,root_generation+1,
        1,&member)==ERROR_ACCESS_DENIED);
    CHECK(!OpenNtBaseServiceReportConsoleMembers(root,member,root_generation,1,&member));
    CHECK(OpenNtBaseServiceReportConsoleMembers(root,member,root_generation,
        1,&member)==ERROR_ALREADY_EXISTS);
    CHECK(!OpenNtBaseServiceRetainFrontendRoot(launcher,child.dwProcessId,launcher_generation,
        capability,&retained,&verified_generation));
    CHECK(retained && GetProcessId(retained)==member && verified_generation==root_generation);
    CloseHandle(retained);
    CHECK(!OpenNtBaseServiceDisconnect(launcher));
    CHECK(!OpenNtBaseServiceDisconnect(root));
    CHECK(OpenNtBaseServiceStop(service));
    CHECK(TerminateProcess(child.hProcess,0));
    CloseHandle(child.hThread);CloseHandle(child.hProcess);
    CloseHandle(capability);CloseHandle(self);
    puts("PASS: only an authenticated frontend root publishes a live, self-containing Console identity");
    return 0;
}

int main(int argc,char **argv)
{
    OPENNT_BASE_SERVICE *service=NULL;
    OPENNT_BASE_CONNECTION *launcher=NULL,*worker=NULL,*later=NULL,*wowWorker=NULL;
    PROCESS_INFORMATION child={0},laterChild={0},wowChild={0};
    STARTUPINFOA startup={sizeof(startup)};
    HANDLE self=NULL,wowFrontend=NULL,wowContext=NULL,laterFrontend=NULL;
    DWORD launcherGeneration=0,workerGeneration=0,laterGeneration=0,wowGeneration=0;
    uint64_t reservation=0,claimed=0,wowReservation=0;
    ULONG task=0,wowTask=0;
    HANDLE wowStartup=NULL;
    BOOL wowStarted=FALSE;
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
    if(argc==3 && !strcmp(argv[1],"--reservation-wait-child"))
        return reservation_wait_child(argv[2]);
    if(argc==3 && !strcmp(argv[1],"--reservation-descendant"))
        return reservation_descendant(argv[2]);
    if(argc==2 && !strcmp(argv[1],"--console-identity"))
        return frontend_console_identity();
    if (argc!=1) {
        static const char *modes[]={
            "--reservation-child","--native-worker","--native-backend","--frontend-root","--worker-channel","--frontend-unclaimed-stop",
            "--frontend-unclaimed-reconnect","--frontend-delegated",
            "--frontend-wait-root-loss","--frontend-wait-request-loss",
            "--frontend-wait","--frontend-wait-worker-loss","--frontend-rundown",
            "--reenter-before-return","--reenter-after-return","--reenter-nested-return",
            "--reenter-pending-command","--reenter-before-increment",
            "--launcher-completed-rundown","--launcher-completed-uncollected",
            "--completed-worker-loss",
            "--completed-worker-exit",
            "--unfinished-worker-exit",
            "--management-terminate","--launcher-exit-survival","--launcher-disconnect-survival",
            "--completion-rundown-race","--wow-start-late-query",
            "--console-identity"
        };
        size_t index;
        if (argc!=2) return 64;
        for (index=0;index<sizeof(modes)/sizeof(modes[0]);++index)
            if (!strcmp(argv[1],modes[index])) break;
        if (index==sizeof(modes)/sizeof(modes[0])) {
            fprintf(stderr,"Unknown fixture mode: %s\n",argv[1]);return 64;
        }
    }
    if (argc==2 && !strcmp(argv[1],"--reservation-child")) { Sleep(15000);return 0; }
    self=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,GetCurrentProcessId());
    service=OpenNtBaseServiceStart();
    CHECK(self && service!=NULL);
    CHECK(OpenNtBaseServiceIsEmpty(service));
    if(argc==2 && !strcmp(argv[1],"--native-worker")) {
        uint64_t duplicate=0;
        OPENNT_BASE_CONNECTION *initial_root=NULL;
        PROCESS_INFORMATION initial_process={0};
        HANDLE initial_capability=CreateEventW(NULL,TRUE,FALSE,NULL),retained_root=NULL;
        DWORD initial_generation=0,retained_generation=0;
        CHECK(!OpenNtBaseServiceConnect(service,self,&launcher,&launcherGeneration));
        CHECK(OpenNtBaseServiceCreateNativeReservation(launcher,GetCurrentProcessId(),
            launcherGeneration+1,&reservation)==ERROR_ACCESS_DENIED && !reservation);
        CHECK(initial_capability && GetModuleFileNameA(NULL,command,MAX_PATH));
        {char executable[MAX_PATH];strcpy_s(executable,MAX_PATH,command);
            sprintf_s(command,sizeof(command),"\"%s\" --reservation-child",executable);}
        CHECK(CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,
            NULL,NULL,&startup,&initial_process));
        CHECK(!OpenNtBaseServiceConnect(service,initial_process.hProcess,
            &initial_root,&initial_generation));
        CHECK(!OpenNtBaseServiceRegisterFrontendRoot(initial_root,initial_process.dwProcessId,
            initial_generation,initial_capability));
        {DWORD members[2]={initial_process.dwProcessId,GetCurrentProcessId()};
            CHECK(!OpenNtBaseServiceReportConsoleMembers(initial_root,initial_process.dwProcessId,
                initial_generation,2,members));}
        CHECK(!OpenNtBaseServiceRetainFrontendRoot(launcher,GetCurrentProcessId(),
            launcherGeneration,initial_capability,&retained_root,&retained_generation));
        CHECK(retained_generation==initial_generation);CloseHandle(retained_root);
        CHECK(!OpenNtBaseServiceCreateNativeReservation(launcher,GetCurrentProcessId(),
            launcherGeneration,&reservation) && reservation);
        CHECK(OpenNtBaseServiceCreateNativeReservation(launcher,GetCurrentProcessId(),
            launcherGeneration,&duplicate)==ERROR_INVALID_STATE && !duplicate);
        CHECK(GetModuleFileNameA(NULL,command,MAX_PATH));
        {char executable[MAX_PATH];strcpy_s(executable,MAX_PATH,command);
            sprintf_s(command,sizeof(command),"\"%s\" --reservation-child",executable);}
        CHECK(CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,
            NULL,NULL,&startup,&child));
        CHECK(!OpenNtBaseServicePrepareWorker(launcher,GetCurrentProcessId(),launcherGeneration,
            reservation,child.hProcess));
        CHECK(!OpenNtBaseServiceConnect(service,child.hProcess,&worker,&workerGeneration));
        CHECK(OpenNtBaseServiceWorkerReservation(worker,&claimed,&task,&console));
        CHECK(claimed==reservation && !task && console && !DOSHead && !WOWHead);
        CHECK(!OpenNtBaseServiceSnapshot(service,&managementEpoch,&workerInfo,1,&workerInfoCount));
        CHECK(workerInfoCount==1 && workerInfo.kind==2 && workerInfo.sequence==workerGeneration &&
            workerInfo.process_id==child.dwProcessId && workerInfo.state);
        CHECK(OpenNtBaseServiceTerminateWorker(service,child.dwProcessId)==ERROR_NOT_READY);
        {
            HANDLE capability=CreateEventW(NULL,TRUE,FALSE,NULL),foreign=CreateEventW(NULL,TRUE,FALSE,NULL);
            HANDLE ready=CreateEventW(NULL,TRUE,FALSE,NULL),server=NULL,client=NULL;
            HANDLE selected=NULL,taken=NULL,frontend=NULL,input_ready=NULL,retained=NULL;
            HANDLE participant_release=NULL;
            OPENNT_BASE_CONNECTION *root=NULL;
            DWORD request=0,frontend_generation=0,root_generation=0,old_generation;
            char participant_name[96],participant_command[MAX_PATH+192];
            HANDLE native_stop=CreateEventW(NULL,TRUE,FALSE,NULL);
            HANDLE native_closed=CreateEventW(NULL,TRUE,FALSE,NULL);
            CHECK(native_stop && native_closed);
            CHECK(capability && foreign && ready);
            CHECK(OpenNtBaseServiceWorkerFrontendCapability(launcher,GetCurrentProcessId(),
                launcherGeneration,&retained)==ERROR_ACCESS_DENIED && !retained);
            CHECK(OpenNtBaseServiceWorkerFrontendCapability(worker,child.dwProcessId,
                workerGeneration,&retained)==ERROR_NOT_FOUND && !retained);
            sprintf_s(participant_name,sizeof(participant_name),
                "Local\\ntvdm-native-participant-%lu",GetCurrentProcessId());
            participant_release=CreateEventA(NULL,TRUE,FALSE,participant_name);
            CHECK(participant_release);
            {
                char executable[MAX_PATH];
                CHECK(GetModuleFileNameA(NULL,executable,MAX_PATH));
                sprintf_s(participant_command,sizeof(participant_command),
                    "\"%s\" --reservation-descendant %s",executable,participant_name);
            }
            CHECK(CreateProcessA(NULL,participant_command,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,
                NULL,NULL,&startup,&laterChild));
            CHECK(!OpenNtBaseServiceConnect(service,laterChild.hProcess,&root,&root_generation));
            {
                /* Model the authenticated same-Console report explicitly.
                 * The fixture's suspended child has no live Console of its
                 * own, so omitting this precondition cannot select a worker. */
                DWORD root_members[2]={laterChild.dwProcessId,GetCurrentProcessId()};
                CHECK(OpenNtBaseServiceReportConsoleMembers(launcher,GetCurrentProcessId(),
                    launcherGeneration,1,root_members+1)==ERROR_ACCESS_DENIED);
                CHECK(!OpenNtBaseServiceRegisterFrontendRoot(root,laterChild.dwProcessId,
                    root_generation,capability));
                CHECK(!OpenNtBaseServiceReportConsoleMembers(root,laterChild.dwProcessId,
                    root_generation,2,root_members));
            }
            /* A second authenticated launcher explicitly selects the existing
             * native worker; merely connecting did not grant command access. */
            CHECK(OpenNtBaseServiceRetainCommandWorker(root,laterChild.dwProcessId,
                root_generation,&selected)==ERROR_NOT_READY && !selected);
            CHECK(OpenNtBaseServiceSelectNativeWorker(root,laterChild.dwProcessId,
                root_generation+1,&selected)==ERROR_ACCESS_DENIED && !selected);
            CHECK(!OpenNtBaseServiceSelectNativeWorker(root,laterChild.dwProcessId,root_generation,&selected));
            CHECK(GetProcessId(selected)==child.dwProcessId);CloseHandle(selected);selected=NULL;
            CHECK(!OpenNtBaseServiceRetainCommandWorker(root,laterChild.dwProcessId,root_generation,&selected));
            CHECK(GetProcessId(selected)==child.dwProcessId);CloseHandle(selected);selected=NULL;
            CHECK(OpenNtBaseServiceCreateNativeReservation(root,laterChild.dwProcessId,
                root_generation,&duplicate)==ERROR_INVALID_STATE && !duplicate);
            CHECK(OpenNtBaseServiceRegisterFrontendRoot(root,laterChild.dwProcessId,
                root_generation,capability)==ERROR_ALREADY_EXISTS);
            CHECK(OpenNtBaseServiceRequestFrontend(launcher,GetCurrentProcessId(),launcherGeneration,foreign)==ERROR_ACCESS_DENIED);
            CHECK(!OpenNtBaseServiceRequestFrontend(launcher,GetCurrentProcessId(),launcherGeneration,capability));
            CHECK(!OpenNtBaseServiceFrontendRequest(root,laterChild.dwProcessId,root_generation,&request,&selected));
            CHECK(request==launcherGeneration && GetProcessId(selected)==child.dwProcessId);
            CloseHandle(selected);
            CHECK(!frontend_pair(&server,&client));
            {
                HANDLE request_pipe=NULL,sender=NULL,execution=NULL,io_capability=NULL;
                char actual[4]={0};DWORD count=0,completed_request=0;
                CHECK(OpenNtBaseServiceSubmitWorkerChannel(launcher,GetCurrentProcessId(),
                    launcherGeneration+1,capability,server)==ERROR_ACCESS_DENIED);
                CHECK(OpenNtBaseServiceSubmitWorkerChannel(launcher,GetCurrentProcessId(),
                    launcherGeneration,foreign,server)==ERROR_ACCESS_DENIED);
                CHECK(OpenNtBaseServiceSubmitWorkerChannel(launcher,GetCurrentProcessId(),
                    launcherGeneration,capability,client)==ERROR_INVALID_PARAMETER);
                CHECK(!OpenNtBaseServiceSubmitWorkerChannel(launcher,GetCurrentProcessId(),
                    launcherGeneration,capability,server));
                CHECK(OpenNtBaseServiceSubmitWorkerChannel(launcher,GetCurrentProcessId(),
                    launcherGeneration,capability,server)==ERROR_BUSY);
                CHECK(OpenNtBaseServiceTakeWorkerChannel(root,laterChild.dwProcessId,root_generation,
                    &request_pipe,&sender,&execution,&io_capability)==ERROR_ACCESS_DENIED);
                CHECK(OpenNtBaseServiceTakeWorkerChannel(worker,child.dwProcessId,workerGeneration+1,
                    &request_pipe,&sender,&execution,&io_capability)==ERROR_ACCESS_DENIED);
                CHECK(!OpenNtBaseServiceTakeWorkerChannel(worker,child.dwProcessId,workerGeneration,
                    &request_pipe,&sender,&execution,&io_capability));
                CHECK(request_pipe && execution && io_capability && GetProcessId(sender)==GetCurrentProcessId());
                completed_request=test_native_request;
                /* The worker, not a launcher-supplied number, binds its
                 * actual CreateProcess target to the direct WIN32RECORD. */
                {HANDLE completion=CreateEventW(NULL,TRUE,FALSE,NULL);
                    CHECK(completion);
                    CHECK(!OpenNtBaseServiceBindNativeTarget(worker,child.dwProcessId,workerGeneration,
                        test_native_request,laterChild.hProcess,completion));
                    CloseHandle(completion);
                }
                /* Bind precedes resume; it publishes only the admitted Direct
                 * target identity, never a sampled Console participant. */
                CHECK(ResumeThread(laterChild.hThread)!=(DWORD)-1);
                CHECK(!OpenNtBaseServiceSnapshot(service,&managementEpoch,&workerInfo,1,&workerInfoCount));
                CHECK(workerInfoCount==1 && workerInfo.kind==2 && workerInfo.stack_depth==1 &&
                    workerInfo.task==test_native_request && workerInfo.image[0]);
                CHECK(WriteFile(client,"NTC",3,&count,NULL) && count==3);
                CHECK(ReadFile(request_pipe,actual,3,&count,NULL) && count==3 && !memcmp(actual,"NTC",3));
                CHECK(!OpenNtBaseServiceRetainFrontendRoot(worker,child.dwProcessId,workerGeneration,
                    io_capability,&retained,&frontend_generation));
                CHECK(frontend_generation==root_generation && GetProcessId(retained)==laterChild.dwProcessId);
                CloseHandle(retained);retained=NULL;
                CloseHandle(request_pipe);CloseHandle(sender);CloseHandle(execution);CloseHandle(io_capability);
                CHECK(OpenNtBaseServiceTakeWorkerChannel(worker,child.dwProcessId,workerGeneration,
                    &request_pipe,&sender,&execution,&io_capability)==ERROR_NOT_FOUND);
                CHECK(!request_pipe && !sender && !execution && !io_capability);
                /* TakeWorkerChannel zeroes its request out parameter on a
                 * no-route result. Keep the prior direct completion ID for
                 * the subsequent completion proof. */
                test_native_request=completed_request;
            }
            CHECK(!OpenNtBaseServiceAttachFrontendRequest(root,laterChild.dwProcessId,root_generation,
                request,client,ready));
            CHECK(OpenNtBaseServiceTakeFrontend(worker,child.dwProcessId,workerGeneration+1,
                &taken,&frontend,&frontend_generation,&input_ready)==ERROR_ACCESS_DENIED);
            CHECK(!OpenNtBaseServiceTakeFrontend(worker,child.dwProcessId,workerGeneration,
                &taken,&frontend,&frontend_generation,&input_ready));
            CHECK(taken && input_ready && GetProcessId(frontend)==laterChild.dwProcessId && frontend_generation==root_generation);
            CHECK(!OpenNtBaseServiceWorkerFrontendCapability(worker,child.dwProcessId,workerGeneration,&retained));
            CloseHandle(retained);CloseHandle(taken);CloseHandle(frontend);CloseHandle(input_ready);
            CHECK(OpenNtBaseServiceTakeFrontend(worker,child.dwProcessId,workerGeneration,
                &taken,&frontend,&frontend_generation,&input_ready)==ERROR_ALREADY_EXISTS);
            CHECK(!taken && !frontend && !input_ready && !frontend_generation);
            {
                DWORD pending=0,tasks=0;
                CHECK(!OpenNtBaseServiceFrontendUsage(root,laterChild.dwProcessId,root_generation,&pending,&tasks));
                CHECK(pending==1 && !tasks);
                CHECK(OpenNtBaseServiceRetireFrontend(root,laterChild.dwProcessId,root_generation)==ERROR_BUSY);
                CHECK(OpenNtBaseServiceCompleteWorkerChannel(launcher,GetCurrentProcessId(),launcherGeneration)==ERROR_ACCESS_DENIED);
                CHECK(OpenNtBaseServiceCompleteWorkerChannel(worker,child.dwProcessId,workerGeneration+1)==ERROR_ACCESS_DENIED);
                CHECK(!OpenNtBaseServiceCompleteWorkerChannel(worker,child.dwProcessId,workerGeneration));
                CHECK(OpenNtBaseServiceCompleteWorkerChannel(worker,child.dwProcessId,workerGeneration)==ERROR_INVALID_STATE);
                {DWORD exit_code=0;
                    CHECK(!OpenNtBaseServiceNativeExitCode(launcher,GetCurrentProcessId(),launcherGeneration,
                        test_native_request,&exit_code) && exit_code==37);}
                CHECK(!OpenNtBaseServiceSnapshot(service,&managementEpoch,&workerInfo,1,&workerInfoCount));
                /* The still-live native process is not a broker task after
                 * its Direct receipt completes. Windows owns its lifetime. */
                CHECK(workerInfoCount==1 && workerInfo.state && !workerInfo.stack_depth &&
                    !workerInfo.task && !wcscmp(workerInfo.image,L"<EMPTY>"));
                CHECK(!OpenNtBaseServiceFrontendUsage(root,laterChild.dwProcessId,root_generation,&pending,&tasks));
                CHECK(!pending && !tasks);
            }
            /* A delivered route is owned by the authenticated root, not by
             * the launcher or worker's execution lifetime. Root rundown must
             * permit a later root without recreating the resident worker. */
            CHECK(!OpenNtBaseServiceRegisterNativeBackend(worker,child.dwProcessId,workerGeneration,
                capability,native_stop,native_closed));
            CHECK(!OpenNtBaseServiceSnapshot(service,&managementEpoch,&workerInfo,1,&workerInfoCount));
            /* Re-registering presentation does not turn an ordinary attached
             * native process into a broker Direct task. */
            CHECK(workerInfoCount==1 && workerInfo.kind==2 && !workerInfo.stack_depth &&
                workerInfo.process_id==child.dwProcessId && workerInfo.sequence==workerGeneration);
            old_generation=root_generation;
            CHECK(!OpenNtBaseServiceSubmitWorkerChannel(launcher,GetCurrentProcessId(),
                launcherGeneration,capability,server));
            CHECK(!OpenNtBaseServiceDisconnect(root));root=NULL;
            {
                HANDLE request_pipe=NULL,sender=NULL,execution=NULL,io_capability=NULL;
                CHECK(OpenNtBaseServiceTakeWorkerChannel(worker,child.dwProcessId,workerGeneration,
                    &request_pipe,&sender,&execution,&io_capability)==ERROR_NOT_FOUND);
                CHECK(!request_pipe && !sender && !execution && !io_capability);
            }
            CHECK(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT);
            CHECK(OpenNtBaseServiceWorkerFrontendCapability(worker,child.dwProcessId,
                workerGeneration,&retained)==ERROR_NOT_FOUND && !retained);
            CHECK(OpenNtBaseServiceTakeFrontend(worker,child.dwProcessId,workerGeneration,
                &taken,&frontend,&frontend_generation,&input_ready)==ERROR_NOT_READY);
            CHECK(!taken && !frontend && !input_ready && !frontend_generation);
            CHECK(OpenNtBaseServiceRequestFrontend(launcher,GetCurrentProcessId(),
                launcherGeneration,capability)==ERROR_ACCESS_DENIED);
            CloseHandle(capability);capability=CreateEventW(NULL,TRUE,FALSE,NULL);
            CHECK(capability && !OpenNtBaseServiceConnect(service,laterChild.hProcess,&root,&root_generation));
            CHECK(root_generation!=old_generation);
            CHECK(!OpenNtBaseServiceRegisterFrontendRoot(root,laterChild.dwProcessId,root_generation,capability));
            {DWORD members[2]={laterChild.dwProcessId,GetCurrentProcessId()};
                CHECK(!OpenNtBaseServiceReportConsoleMembers(root,laterChild.dwProcessId,
                    root_generation,2,members));}
            CHECK(!OpenNtBaseServiceRegisterNativeBackend(worker,child.dwProcessId,workerGeneration,
                capability,native_stop,native_closed));
            CHECK(!OpenNtBaseServiceSnapshot(service,&managementEpoch,&workerInfo,1,&workerInfoCount));
            CHECK(workerInfoCount==1 && !workerInfo.stack_depth && workerInfo.sequence==workerGeneration);
            CHECK(!OpenNtBaseServiceRequestFrontend(launcher,GetCurrentProcessId(),launcherGeneration,capability));
            CHECK(!OpenNtBaseServiceFrontendRequest(root,laterChild.dwProcessId,root_generation,&request,&selected));
            CHECK(GetProcessId(selected)==child.dwProcessId);CloseHandle(selected);
            CHECK(OpenNtBaseServiceAttachFrontendRequest(root,laterChild.dwProcessId,old_generation,
                request,client,ready)==ERROR_ACCESS_DENIED);
            CHECK(!OpenNtBaseServiceAttachFrontendRequest(root,laterChild.dwProcessId,root_generation,
                request,client,ready));
            CHECK(!OpenNtBaseServiceTakeFrontend(worker,child.dwProcessId,workerGeneration,
                &taken,&frontend,&frontend_generation,&input_ready));
            CHECK(frontend_generation==root_generation);
            CloseHandle(taken);CloseHandle(frontend);CloseHandle(input_ready);
            CHECK(!OpenNtBaseServiceWorkerFrontendCapability(worker,child.dwProcessId,workerGeneration,&retained));
            CloseHandle(retained);
            CHECK(!OpenNtBaseServiceDisconnect(root));root=NULL;
            CHECK(SetEvent(participant_release));
            CHECK(WaitForSingleObject(laterChild.hProcess,5000)==WAIT_OBJECT_0);
            /* No Job observer is admitted: exit does not complete an invented
             * Win32Record or change the already READY resident worker. */
            CHECK(!OpenNtBaseServiceSnapshot(service,&managementEpoch,&workerInfo,1,&workerInfoCount));
            CHECK(workerInfoCount==1 && workerInfo.state && !workerInfo.stack_depth &&
                !workerInfo.task);
            CloseHandle(laterChild.hThread);CloseHandle(laterChild.hProcess);
            CloseHandle(participant_release);participant_release=NULL;
            CloseHandle(client);CloseHandle(server);CloseHandle(ready);CloseHandle(foreign);CloseHandle(capability);
            {
                NATIVE_CLOSE_TEST close_test={native_stop,native_closed,child.hProcess,0};
                HANDLE closer=CreateThread(NULL,0,acknowledge_native_close,&close_test,0,NULL);
                DWORD closer_exit=ERROR_GEN_FAILURE;
                CHECK(closer);
                /* The route/root is already gone.  Service authority comes
                 * from the authenticated native worker watch, not the former
                 * frontend association. */
                CHECK(!OpenNtBaseServiceTerminateWorker(service,child.dwProcessId));
                CHECK(WaitForSingleObject(closer,5000)==WAIT_OBJECT_0);
                CHECK(GetExitCodeThread(closer,&closer_exit) && !closer_exit);
                CHECK(close_test.observed==WAIT_OBJECT_0);
                CloseHandle(closer);
            }
            CloseHandle(native_stop);CloseHandle(native_closed);
            CHECK(!DOSHead && !WOWHead);
        }
        CHECK(!OpenNtBaseServiceDisconnect(launcher));launcher=NULL;
        CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
        CHECK(!OpenNtBaseServiceDisconnect(worker));worker=NULL;
        CHECK(!OpenNtBaseServiceDisconnect(initial_root));initial_root=NULL;
        {DWORD deadline=GetTickCount()+5000;
            while(!OpenNtBaseServiceIsEmpty(service) && (LONG)(deadline-GetTickCount())>0)Sleep(10);}
        CHECK(!OpenNtBaseServiceSnapshot(service,&managementEpoch,&workerInfo,1,&workerInfoCount));
        CHECK(!workerInfoCount && !DOSHead && !WOWHead && OpenNtBaseServiceIsEmpty(service));
        CHECK(OpenNtBaseServiceStop(service));
        CHECK(TerminateProcess(initial_process.hProcess,0));
        CloseHandle(initial_process.hThread);CloseHandle(initial_process.hProcess);
        CloseHandle(initial_capability);
        CloseHandle(child.hThread);CloseHandle(child.hProcess);CloseHandle(self);
        puts("PASS independent native worker: reservation/authentication, shared frontend route and negative capabilities, no guest record, launcher/RPC loss survival, actual process rundown");
        return 0;
    }
    if(argc==2 && !strcmp(argv[1],"--native-backend")) {
        HANDLE capability=CreateEventW(NULL,TRUE,FALSE,NULL);
        HANDLE stop=CreateEventW(NULL,TRUE,FALSE,NULL),closed=CreateEventW(NULL,TRUE,FALSE,NULL);
        CHECK(capability && stop && closed);
        CHECK(GetModuleFileNameA(NULL,command,MAX_PATH));
        {char executable[MAX_PATH];strcpy_s(executable,MAX_PATH,command);
            sprintf_s(command,sizeof(command),"\"%s\" --reservation-child",executable);}
        CHECK(CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&startup,&child));
        CloseHandle(child.hThread);
        CHECK(!OpenNtBaseServiceConnect(service,self,&launcher,&launcherGeneration));
        CHECK(!OpenNtBaseServiceConnect(service,child.hProcess,&worker,&workerGeneration));
        CHECK(!OpenNtBaseServiceRegisterFrontendRoot(launcher,GetCurrentProcessId(),launcherGeneration,capability));
        /* A live process plus an authenticated frontend is not a worker
         * reservation.  Native backend registration is exclusive to the
         * process that claimed a prepared native reservation; the complete
         * reserved registration/rebind path is exercised by --native-worker. */
        CHECK(OpenNtBaseServiceRegisterNativeBackend(worker,child.dwProcessId,workerGeneration,
            capability,stop,closed)==ERROR_INVALID_STATE);
        CHECK(!OpenNtBaseServiceSnapshot(service,&managementEpoch,&workerInfo,1,&workerInfoCount));
        CHECK(!workerInfoCount);
        CHECK(!OpenNtBaseServiceDisconnect(worker));worker=NULL;
        CHECK(!OpenNtBaseServiceDisconnect(launcher));launcher=NULL;
        CHECK(OpenNtBaseServiceIsEmpty(service) && OpenNtBaseServiceStop(service));
        CHECK(TerminateProcess(child.hProcess,0));
        CloseHandle(child.hProcess);CloseHandle(self);
        CloseHandle(capability);CloseHandle(stop);CloseHandle(closed);
        puts("PASS native backend registry: rejects unreserved backend registration");
        return 0;
    }
    if (argc==2 && !strcmp(argv[1],"--worker-channel")) {
        HANDLE capability=CreateEventW(NULL,TRUE,FALSE,NULL),wrong=CreateEventW(NULL,TRUE,FALSE,NULL);
        HANDLE server=NULL,client=NULL,taken=NULL,sender=NULL,execution=NULL,expected=NULL,io=NULL;
        DWORD request=0,transferred=0;char byte=0;
        BOOL (WINAPI *compare)(HANDLE,HANDLE)=(BOOL (WINAPI *)(HANDLE,HANDLE))
            GetProcAddress(GetModuleHandleW(L"kernelbase.dll"),"CompareObjectHandles");
        CHECK(capability && wrong && compare);
        CloseHandle(self);
        self=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|PROCESS_DUP_HANDLE|SYNCHRONIZE,FALSE,GetCurrentProcessId());
        CHECK(self && GetModuleFileNameA(NULL,command,MAX_PATH));
        { char executable[MAX_PATH];strcpy_s(executable,MAX_PATH,command);
          sprintf_s(command,sizeof(command),"\"%s\" --reservation-child",executable); }
        CHECK(CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&startup,&child));
        CloseHandle(child.hThread);
        CHECK(!OpenNtBaseServiceConnect(service,self,&launcher,&launcherGeneration));
        CHECK(!OpenNtBaseServiceConnect(service,child.hProcess,&later,&laterGeneration));
        CHECK(!OpenNtBaseServiceRegisterFrontendRoot(later,child.dwProcessId,laterGeneration,capability));
        {DWORD members[2]={child.dwProcessId,GetCurrentProcessId()};
            HANDLE retained=NULL;DWORD generation=0;
            CHECK(!OpenNtBaseServiceReportConsoleMembers(later,child.dwProcessId,
                laterGeneration,2,members));
            CHECK(!OpenNtBaseServiceRetainFrontendRoot(launcher,GetCurrentProcessId(),
                launcherGeneration,capability,&retained,&generation));
            CHECK(generation==laterGeneration);CloseHandle(retained);}
        CHECK(!OpenNtBaseServiceCreateNativeReservation(launcher,GetCurrentProcessId(),launcherGeneration,&reservation));
        CHECK(CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,NULL,NULL,&startup,&laterChild));
        CHECK(!OpenNtBaseServicePrepareWorker(launcher,GetCurrentProcessId(),launcherGeneration,reservation,laterChild.hProcess));
        CHECK(!OpenNtBaseServiceConnect(service,laterChild.hProcess,&worker,&workerGeneration));
        CHECK(!frontend_pair(&server,&client));
        CHECK(OpenNtBaseServiceSubmitWorkerChannel(launcher,GetCurrentProcessId(),launcherGeneration+1,
            capability,server)==ERROR_ACCESS_DENIED);
        CHECK(OpenNtBaseServiceSubmitWorkerChannel(launcher,GetCurrentProcessId(),launcherGeneration,
            wrong,server)==ERROR_ACCESS_DENIED);
        CHECK(OpenNtBaseServiceSubmitWorkerChannel(launcher,GetCurrentProcessId(),launcherGeneration,
            capability,wrong)==ERROR_INVALID_PARAMETER);
        CHECK(OpenNtBaseServiceSubmitWorkerChannel(launcher,GetCurrentProcessId(),launcherGeneration,
            capability,client)==ERROR_INVALID_PARAMETER);
        CHECK(OpenNtBaseServiceSubmitWorkerChannel(later,child.dwProcessId,laterGeneration,
            capability,server)==ERROR_INVALID_PARAMETER); /* Not the pipe creator. */
        CHECK(!OpenNtBaseServiceAcquireConsoleContext(launcher,GetCurrentProcessId(),launcherGeneration,
            capability,&expected));
        CHECK(!OpenNtBaseServiceSubmitWorkerChannel(launcher,GetCurrentProcessId(),launcherGeneration,
            capability,server));
        CHECK(OpenNtBaseServiceSubmitWorkerChannel(launcher,GetCurrentProcessId(),launcherGeneration,
            capability,server)==ERROR_BUSY);
        CHECK(OpenNtBaseServiceTakeWorkerChannel(launcher,GetCurrentProcessId(),launcherGeneration,
            &taken,&sender,&execution,&io)==ERROR_ACCESS_DENIED && !taken && !sender && !execution);
        CHECK(OpenNtBaseServiceFrontendRequest(later,child.dwProcessId,laterGeneration,&request,&sender)==ERROR_NOT_FOUND);
        CHECK(WaitForSingleObject(capability,0)==WAIT_TIMEOUT); /* Execution does not wake presentation. */
        CHECK(!OpenNtBaseServiceTakeWorkerChannel(worker,laterChild.dwProcessId,workerGeneration,&taken,&sender,&execution,&io));
        CHECK(GetProcessId(sender)==GetCurrentProcessId() && compare(execution,expected) && compare(io,capability));
        CHECK(!SetEvent(execution) && GetLastError()==ERROR_ACCESS_DENIED);
        CloseHandle(server);server=NULL;
        CHECK(WriteFile(client,"X",1,&transferred,NULL) && transferred==1);
        CHECK(ReadFile(taken,&byte,1,&transferred,NULL) && transferred==1 && byte=='X');
        CHECK(!OpenNtBaseServiceDisconnect(launcher));launcher=NULL;
        CHECK(WriteFile(taken,"Y",1,&transferred,NULL) && transferred==1);
        CHECK(ReadFile(client,&byte,1,&transferred,NULL) && byte=='Y'); /* Delivered route survives sender rundown. */
        CloseHandle(taken);CloseHandle(sender);CloseHandle(execution);CloseHandle(io);CloseHandle(client);
        CHECK(!OpenNtBaseServiceCompleteWorkerChannel(worker,laterChild.dwProcessId,workerGeneration));
        CHECK(OpenNtBaseServiceTakeWorkerChannel(worker,laterChild.dwProcessId,workerGeneration,
            &taken,&sender,&execution,&io)==ERROR_NOT_FOUND && !taken && !sender && !execution);
        CHECK(WaitForSingleObject(capability,0)==WAIT_TIMEOUT);
        CHECK(!OpenNtBaseServiceConnect(service,self,&launcher,&launcherGeneration));
        {
            HANDLE retained=NULL;DWORD generation=0;
            CHECK(!OpenNtBaseServiceRetainFrontendRoot(launcher,GetCurrentProcessId(),
                launcherGeneration,capability,&retained,&generation));
            CHECK(generation==laterGeneration);CloseHandle(retained);
        }
        {HANDLE selected=NULL;CHECK(!OpenNtBaseServiceSelectNativeWorker(launcher,GetCurrentProcessId(),launcherGeneration,&selected));
         CHECK(GetProcessId(selected)==laterChild.dwProcessId);CloseHandle(selected);}
        CHECK(!frontend_pair(&server,&client));
        CHECK(!OpenNtBaseServiceSubmitWorkerChannel(launcher,GetCurrentProcessId(),launcherGeneration,capability,server));
        CloseHandle(server);server=NULL;
        CHECK(!OpenNtBaseServiceDisconnect(launcher));launcher=NULL;
        CHECK(!ReadFile(client,&byte,1,&transferred,NULL) && GetLastError()==ERROR_BROKEN_PIPE);
        CloseHandle(client);
        CHECK(!OpenNtBaseServiceConnect(service,self,&launcher,&launcherGeneration));
        {
            HANDLE retained=NULL;DWORD generation=0;
            CHECK(!OpenNtBaseServiceRetainFrontendRoot(launcher,GetCurrentProcessId(),
                launcherGeneration,capability,&retained,&generation));
            CHECK(generation==laterGeneration);CloseHandle(retained);
        }
        {HANDLE selected=NULL;CHECK(!OpenNtBaseServiceSelectNativeWorker(launcher,GetCurrentProcessId(),launcherGeneration,&selected));
         CHECK(GetProcessId(selected)==laterChild.dwProcessId);CloseHandle(selected);}
        CHECK(!frontend_pair(&server,&client));
        CHECK(!OpenNtBaseServiceSubmitWorkerChannel(launcher,GetCurrentProcessId(),launcherGeneration,capability,server));
        CloseHandle(server);server=NULL;
        CHECK(!OpenNtBaseServiceDisconnect(later));later=NULL;
        CHECK(!ReadFile(client,&byte,1,&transferred,NULL) && GetLastError()==ERROR_BROKEN_PIPE);
        CHECK(OpenNtBaseServiceSubmitWorkerChannel(launcher,GetCurrentProcessId(),launcherGeneration,
            capability,client)==ERROR_INVALID_PARAMETER);
        CloseHandle(client);
        CHECK(!OpenNtBaseServiceDisconnect(launcher));
        CHECK(!OpenNtBaseServiceDisconnect(worker));
        CHECK(!OpenNtBaseServiceIsEmpty(service)); /* RPC loss does not kill the worker. */
        CHECK(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT);
        CHECK(TerminateProcess(child.hProcess,0) && WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
        CHECK(TerminateProcess(laterChild.hProcess,0) && WaitForSingleObject(laterChild.hProcess,5000)==WAIT_OBJECT_0);
        {DWORD deadline=GetTickCount()+5000;
            while(!OpenNtBaseServiceIsEmpty(service) && (LONG)(deadline-GetTickCount())>0)Sleep(10);}
        CHECK(OpenNtBaseServiceIsEmpty(service) && OpenNtBaseServiceStop(service));
        CloseHandle(laterChild.hProcess);CloseHandle(laterChild.hThread);
        CloseHandle(child.hProcess);CloseHandle(self);CloseHandle(capability);CloseHandle(wrong);CloseHandle(expected);
        puts("PASS worker-only attachment: authenticated worker/root/sender, type/creator/generation, one pending route, no presentation wake, transfer/rundown ownership");
        return 0;
    }
    if (argc==2 && !strcmp(argv[1],"--frontend-root")) {
        HANDLE capability=CreateEventW(NULL,TRUE,FALSE,NULL),restricted=NULL;
        HANDLE different=CreateEventW(NULL,TRUE,FALSE,NULL),automatic=CreateEventW(NULL,FALSE,FALSE,NULL);
        HANDLE named,root=NULL,execution=NULL,execution_again=NULL;
        BOOL (WINAPI *compare)(HANDLE,HANDLE)=(BOOL (WINAPI *)(HANDLE,HANDLE))
            GetProcAddress(GetModuleHandleW(L"kernelbase.dll"),"CompareObjectHandles");
        DWORD rootGeneration=0,flags=0;
        char name[96];
        CHECK(capability && different && automatic && compare);
        sprintf_s(name,96,"Local\\ntvdm-frontend-fixture-%lu",GetCurrentProcessId());
        named=CreateEventA(NULL,TRUE,FALSE,name);CHECK(named);
        CHECK(DuplicateHandle(GetCurrentProcess(),capability,GetCurrentProcess(),
            &restricted,SYNCHRONIZE,FALSE,0));
        CHECK(GetModuleFileNameA(NULL,command,MAX_PATH));
        { char executable[MAX_PATH];strcpy_s(executable,MAX_PATH,command);
          sprintf_s(command,sizeof(command),"\"%s\" --reservation-child",executable); }
        CHECK(CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&startup,&child));
        CloseHandle(child.hThread);child.hThread=NULL;
        CHECK(OpenNtBaseServiceConnect(service,self,&launcher,&launcherGeneration)==ERROR_SUCCESS);
        CHECK(OpenNtBaseServiceConnect(service,child.hProcess,&later,&laterGeneration)==ERROR_SUCCESS);
        CHECK(OpenNtBaseServiceRegisterFrontendRoot(launcher,GetCurrentProcessId(),
            launcherGeneration+1,capability)==ERROR_ACCESS_DENIED);
        CHECK(OpenNtBaseServiceRegisterFrontendRoot(launcher,GetCurrentProcessId(),
            launcherGeneration,self)==ERROR_INVALID_PARAMETER);
        CHECK(OpenNtBaseServiceRegisterFrontendRoot(launcher,GetCurrentProcessId(),
            launcherGeneration,automatic)==ERROR_INVALID_PARAMETER);
        CHECK(OpenNtBaseServiceRegisterFrontendRoot(launcher,GetCurrentProcessId(),
            launcherGeneration,named)==ERROR_INVALID_PARAMETER);
        CHECK(OpenNtBaseServiceRegisterFrontendRoot(launcher,GetCurrentProcessId(),
            launcherGeneration,capability)==ERROR_SUCCESS);
        CHECK(OpenNtBaseServiceRegisterFrontendRoot(launcher,GetCurrentProcessId(),
            launcherGeneration,capability)==ERROR_ALREADY_EXISTS);
        CHECK(OpenNtBaseServiceRegisterFrontendRoot(later,child.dwProcessId,
            laterGeneration,capability)==ERROR_ALREADY_EXISTS);
        CHECK(OpenNtBaseServiceAcquireConsoleContext(launcher,GetCurrentProcessId(),
            launcherGeneration+1,restricted,&execution)==ERROR_ACCESS_DENIED && !execution);
        CHECK(OpenNtBaseServiceAcquireConsoleContext(launcher,GetCurrentProcessId(),
            launcherGeneration,different,&execution)==ERROR_ACCESS_DENIED && !execution);
        CHECK(OpenNtBaseServiceAcquireConsoleContext(launcher,GetCurrentProcessId(),
            launcherGeneration,restricted,&execution)==ERROR_SUCCESS && execution);
        CHECK(GetHandleInformation(execution,&flags) && !(flags&HANDLE_FLAG_INHERIT));
        CHECK(!SetEvent(execution) && GetLastError()==ERROR_ACCESS_DENIED);
        /* A frontend event is not execution authority; only the distinct
         * server-issued object can bind the pre-admission Console context. */
        CHECK(OpenNtBaseServiceBindConsoleContext(later,child.dwProcessId,laterGeneration,
            restricted)==ERROR_ACCESS_DENIED);
        CHECK(OpenNtBaseServiceBindConsoleContext(later,child.dwProcessId,laterGeneration+1,
            execution)==ERROR_ACCESS_DENIED);
        CHECK(OpenNtBaseServiceBindConsoleContext(later,child.dwProcessId,laterGeneration,
            execution)==ERROR_SUCCESS);
        CHECK(OpenNtBaseServiceBindConsoleContext(later,child.dwProcessId,laterGeneration,
            execution)==ERROR_SUCCESS);
        CHECK(OpenNtBaseServiceAcquireConsoleContext(later,child.dwProcessId,laterGeneration,
            restricted,&execution_again)==ERROR_SUCCESS && execution_again);
        CHECK(compare(execution,execution_again) && !compare(execution,restricted));
        /* The helper/request connection is not the association owner. */
        CHECK(OpenNtBaseServiceDisconnect(later)==ERROR_SUCCESS);later=NULL;
        CHECK(OpenNtBaseServiceConnect(service,child.hProcess,&later,&laterGeneration)==ERROR_SUCCESS);
        CHECK(OpenNtBaseServiceBindConsoleContext(later,child.dwProcessId,laterGeneration,
            execution_again)==ERROR_SUCCESS);
        CHECK(OpenNtBaseServiceRetainFrontendRoot(later,child.dwProcessId,laterGeneration+1,
            restricted,&root,&rootGeneration)==ERROR_ACCESS_DENIED && !root && !rootGeneration);
        CHECK(OpenNtBaseServiceRetainFrontendRoot(later,child.dwProcessId,laterGeneration,
            different,&root,&rootGeneration)==ERROR_ACCESS_DENIED && !root && !rootGeneration);
        CHECK(OpenNtBaseServiceRetainFrontendRoot(later,child.dwProcessId,laterGeneration,
            restricted,&root,&rootGeneration)==ERROR_SUCCESS);
        CHECK(GetProcessId(root)==GetCurrentProcessId() && rootGeneration==launcherGeneration);
        CHECK(GetHandleInformation(root,&flags) && !(flags&HANDLE_FLAG_INHERIT));
        CloseHandle(root);root=NULL;
        /* Retirement is an admission barrier even while the owner is alive.
         * A previously issued execution capability cannot bypass that barrier. */
        CHECK(OpenNtBaseServiceRetireFrontend(launcher,GetCurrentProcessId(),launcherGeneration)==ERROR_SUCCESS);
        CHECK(WaitForSingleObject(self,0)==WAIT_TIMEOUT);
        CHECK(OpenNtBaseServiceBindConsoleContext(later,child.dwProcessId,laterGeneration,
            execution_again)==ERROR_PIPE_NOT_CONNECTED);
        CHECK(OpenNtBaseServiceDisconnect(launcher)==ERROR_SUCCESS);launcher=NULL;
        CHECK(OpenNtBaseServiceBindConsoleContext(later,child.dwProcessId,laterGeneration,
            execution)==ERROR_ACCESS_DENIED);
        CHECK(OpenNtBaseServiceRetainFrontendRoot(later,child.dwProcessId,laterGeneration,
            restricted,&root,&rootGeneration)==ERROR_ACCESS_DENIED && !root && !rootGeneration);
        CHECK(OpenNtBaseServiceConnect(service,self,&launcher,&launcherGeneration)==ERROR_SUCCESS);
        CHECK(OpenNtBaseServiceRegisterFrontendRoot(later,child.dwProcessId,
            laterGeneration,different)==ERROR_SUCCESS);
        CloseHandle(execution_again);execution_again=NULL;
        CHECK(OpenNtBaseServiceAcquireConsoleContext(later,child.dwProcessId,laterGeneration,
            different,&execution_again)==ERROR_SUCCESS && execution_again);
        CHECK(TerminateProcess(child.hProcess,23));
        CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
        CHECK(OpenNtBaseServiceBindConsoleContext(launcher,GetCurrentProcessId(),launcherGeneration,
            execution_again)==ERROR_PIPE_NOT_CONNECTED);
        CHECK(OpenNtBaseServiceRetainFrontendRoot(launcher,GetCurrentProcessId(),launcherGeneration,
            different,&root,&rootGeneration)==ERROR_PIPE_NOT_CONNECTED && !root && !rootGeneration);
        CHECK(OpenNtBaseServiceDisconnect(later)==ERROR_SUCCESS);
        CHECK(OpenNtBaseServiceDisconnect(launcher)==ERROR_SUCCESS);
        CHECK(OpenNtBaseServiceIsEmpty(service));
        CHECK(OpenNtBaseServiceStop(service));
        CloseHandle(child.hProcess);CloseHandle(self);
        CloseHandle(capability);CloseHandle(restricted);CloseHandle(different);
        CloseHandle(automatic);CloseHandle(named);
        CloseHandle(execution);CloseHandle(execution_again);
        puts("PASS frontend/execution contexts: distinct object authority, type, generation, helper rundown, root rundown/death");
        return 0;
    }
    /* Repeating after rundown proves the failed-start record is not reused. */
    CHECK(detached_reservation(service,self)==0);
    CHECK(detached_reservation(service,self)==0);
    CHECK(OpenNtBaseServiceConnect(service,self,&launcher,&launcherGeneration)==ERROR_SUCCESS);
    CHECK(!OpenNtBaseServiceIsEmpty(service));
    {
        HANDLE peer=(HANDLE)1;
        CHECK(OpenNtBaseServiceRetainCommandWorker(launcher,GetCurrentProcessId(),
            launcherGeneration+1,&peer)==ERROR_ACCESS_DENIED && peer==NULL);
        CHECK(OpenNtBaseServiceRetainCommandWorker(launcher,GetCurrentProcessId(),
            launcherGeneration,&peer)==ERROR_NOT_READY && peer==NULL);
    }
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
    if (argc==2 && (!strcmp(argv[1],"--frontend-unclaimed-stop") ||
        !strcmp(argv[1],"--frontend-unclaimed-reconnect"))) {
        HANDLE capability=CreateEventW(NULL,TRUE,FALSE,NULL);
        DWORD beforeStop=0,afterStop=0;
        CHECK(capability);
        CHECK(OpenNtBaseServiceRegisterFrontendRoot(launcher,GetCurrentProcessId(),
            launcherGeneration,capability)==ERROR_SUCCESS);
        CHECK(OpenNtBaseServiceRequestFrontend(launcher,GetCurrentProcessId(),
            launcherGeneration,capability)==ERROR_SUCCESS);
        /* Never connect/resume this prepared worker: no process-exit watch
         * exists to collect the cancelled frontend route. Rundown owns only
         * this unclaimed startup, not a running guest task. */
        CHECK(OpenNtBaseServiceDisconnect(launcher)==ERROR_SUCCESS);launcher=NULL;
        CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
        CHECK(OpenNtBaseServiceIsEmpty(service));
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&beforeStop));
        if (!strcmp(argv[1],"--frontend-unclaimed-reconnect")) {
            OPENNT_BASE_CONNECTION *next=NULL;
            DWORD nextGeneration=0,afterReconnect=0;
            CHECK(OpenNtBaseServiceConnect(service,self,&next,&nextGeneration)==ERROR_SUCCESS);
            CHECK(OpenNtBaseServiceDisconnect(next)==ERROR_SUCCESS);
            CHECK(GetProcessHandleCount(GetCurrentProcess(),&afterReconnect));
            fprintf(stdout,"unclaimed frontend reconnect: handles before=%lu after=%lu\n",
                beforeStop,afterReconnect);
            CHECK(beforeStop==afterReconnect+1);
            beforeStop=afterReconnect;
        }
        CHECK(OpenNtBaseServiceStop(service));service=NULL;
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&afterStop));
        fprintf(stdout,"unclaimed frontend stop: handles before=%lu after=%lu\n",beforeStop,afterStop);
        CloseHandle(capability);CloseHandle(child.hThread);CloseHandle(child.hProcess);
        CloseHandle(stdinRead);CloseHandle(stdinWrite);CloseHandle(stdoutRead);CloseHandle(stdoutWrite);
        CloseHandle(self);free(updateAnswer);free(updateWire);free(answer);free(wire);
        CHECK(beforeStop==afterStop+(!strcmp(argv[1],"--frontend-unclaimed-stop") ? 1 : 0));
        puts(!strcmp(argv[1],"--frontend-unclaimed-stop") ?
            "PASS: service stop releases the unclaimed cancelled frontend worker handle" :
            "PASS: new admission releases dead cancelled route before service stop");
        return 0;
    }
    CHECK(ResumeThread(child.hThread)!=(DWORD)-1);
    CHECK(OpenNtBaseServiceConnect(service,child.hProcess,&worker,&workerGeneration)==ERROR_SUCCESS);
    {
        HANDLE peer=NULL;
        DWORD flags=0;
        CHECK(OpenNtBaseServiceRetainCommandWorker(launcher,GetCurrentProcessId(),
            launcherGeneration,&peer)==ERROR_SUCCESS);
        CHECK(peer && GetProcessId(peer)==child.dwProcessId);
        CHECK(GetHandleInformation(peer,&flags) && !(flags&HANDLE_FLAG_INHERIT));
        CHECK(!TerminateProcess(peer,99) && GetLastError()==ERROR_ACCESS_DENIED);
        CloseHandle(peer);peer=NULL;
        CHECK(OpenNtBaseServiceRetainCommandWorker(worker,child.dwProcessId,
            workerGeneration,&peer)==ERROR_NOT_READY && peer==NULL);
    }
    CHECK(OpenNtBaseServiceWorkerReservation(worker,&claimed,&task,&console));
    if (argc==2 && (!strcmp(argv[1],"--frontend-delegated") ||
        !strcmp(argv[1],"--frontend-wait-root-loss") ||
        !strcmp(argv[1],"--frontend-wait-request-loss"))) {
        OPENNT_BASE_CONNECTION *rootConnection=NULL;
        PROCESS_INFORMATION rootProcess={0};
        HANDLE capability=CreateEventW(NULL,TRUE,FALSE,NULL),other=CreateEventW(NULL,TRUE,FALSE,NULL);
        HANDLE pending=NULL,ui=NULL,ready=CreateEventW(NULL,TRUE,FALSE,NULL);
        HANDLE selected=NULL,delivered=NULL,peer=NULL,deliveredReady=NULL;
        DWORD rootGeneration=0,request=0,deliveredGeneration=0,count,usage_pending=99,usage_tasks=99;
        FRONTEND_WAIT_TEST duplicate={0};
        HANDLE duplicateThread=NULL;
        DWORD duplicateWait=WAIT_FAILED;
        char payload;
        CHECK(capability && other && ready);
        CHECK(GetModuleFileNameA(NULL,command,MAX_PATH));
        { char executable[MAX_PATH];strcpy_s(executable,MAX_PATH,command);
          sprintf_s(command,sizeof(command),"\"%s\" --reservation-child",executable); }
        CHECK(CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&startup,&rootProcess));
        CHECK(OpenNtBaseServiceConnect(service,rootProcess.hProcess,&rootConnection,&rootGeneration)==ERROR_SUCCESS);
        CHECK(OpenNtBaseServiceRegisterFrontendRoot(rootConnection,rootProcess.dwProcessId,
            rootGeneration,capability)==ERROR_SUCCESS);
        CHECK(OpenNtBaseServiceFrontendUsage(launcher,GetCurrentProcessId(),launcherGeneration,
            &usage_pending,&usage_tasks)==ERROR_ACCESS_DENIED && usage_pending==99 && usage_tasks==99);
        CHECK(OpenNtBaseServiceFrontendUsage(rootConnection,rootProcess.dwProcessId,rootGeneration+1,
            &usage_pending,&usage_tasks)==ERROR_ACCESS_DENIED);
        CHECK(!OpenNtBaseServiceFrontendUsage(rootConnection,rootProcess.dwProcessId,rootGeneration,
            &usage_pending,&usage_tasks) && !usage_pending && !usage_tasks);
        CHECK(OpenNtBaseServiceFrontendRequest(rootConnection,rootProcess.dwProcessId,
            rootGeneration,&request,&selected)==ERROR_NOT_FOUND && !request && !selected);
        CHECK(WaitForSingleObject(capability,0)==WAIT_TIMEOUT);
        CHECK(OpenNtBaseServiceRequestFrontend(launcher,GetCurrentProcessId(),
            launcherGeneration,other)==ERROR_ACCESS_DENIED);
        CHECK(OpenNtBaseServiceRequestFrontend(launcher,GetCurrentProcessId(),
            launcherGeneration,capability)==ERROR_SUCCESS);
        CHECK(WaitForSingleObject(capability,0)==WAIT_OBJECT_0);
        CHECK(!OpenNtBaseServiceFrontendUsage(rootConnection,rootProcess.dwProcessId,rootGeneration,
            &usage_pending,&usage_tasks) && usage_pending && usage_tasks==1);
        CHECK(OpenNtBaseServiceRetireFrontend(rootConnection,rootProcess.dwProcessId,rootGeneration)==ERROR_BUSY);
        if (strcmp(argv[1],"--frontend-delegated")) {
            FRONTEND_WAIT_TEST waiting={0};
            HANDLE waitingThread;
            DWORD outcome;
            waiting.connection=worker;waiting.pid=child.dwProcessId;waiting.generation=workerGeneration;
            waitingThread=CreateThread(NULL,0,frontend_wait,&waiting,0,NULL);CHECK(waitingThread);
            CHECK(WaitForSingleObject(waitingThread,100)==WAIT_TIMEOUT);
            if (!strcmp(argv[1],"--frontend-wait-root-loss")) {
                CHECK(OpenNtBaseServiceDisconnect(rootConnection)==ERROR_SUCCESS);rootConnection=NULL;
            } else {
                CHECK(OpenNtBaseServiceDisconnect(launcher)==ERROR_SUCCESS);launcher=NULL;
            }
            outcome=WaitForSingleObject(waitingThread,5000);
            fprintf(stdout,"pending frontend cancellation: wait=%lu error=%lu\n",outcome,waiting.error);
            /* Both kinds of rundown revoke only the I/O route. Cleanup below
             * is a fixture action, never a product worker-termination result. */
            CHECK(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT);
            CHECK(TerminateProcess(child.hProcess,0));
            CHECK(TerminateProcess(rootProcess.hProcess,0));
            CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
            CHECK(WaitForSingleObject(rootProcess.hProcess,5000)==WAIT_OBJECT_0);
            CHECK(WaitForSingleObject(waitingThread,5000)==WAIT_OBJECT_0);
            CHECK(outcome==WAIT_OBJECT_0 && waiting.error==ERROR_PIPE_NOT_CONNECTED &&
                !waiting.pipe && !waiting.frontend && !waiting.ready && !waiting.frontend_generation);
            CloseHandle(waitingThread);
            CHECK(OpenNtBaseServiceDisconnect(worker)==ERROR_SUCCESS);
            if (launcher) CHECK(OpenNtBaseServiceDisconnect(launcher)==ERROR_SUCCESS);
            if (rootConnection) CHECK(OpenNtBaseServiceDisconnect(rootConnection)==ERROR_SUCCESS);
            CloseHandle(rootProcess.hThread);CloseHandle(rootProcess.hProcess);
            CloseHandle(child.hThread);CloseHandle(child.hProcess);CloseHandle(self);
            CloseHandle(capability);CloseHandle(other);CloseHandle(ready);
            puts("PASS: pending root/request rundown cancels worker wait without publishing handles");
            return 0;
        }
        CHECK(OpenNtBaseServiceFrontendRequest(rootConnection,rootProcess.dwProcessId,
            rootGeneration,&request,&selected)==ERROR_SUCCESS);
        CHECK(request==launcherGeneration && GetProcessId(selected)==child.dwProcessId);
        CloseHandle(selected);
        CHECK(frontend_pair(&pending,&ui)==0);
        CHECK(OpenNtBaseServiceAttachFrontendRequest(rootConnection,rootProcess.dwProcessId,
            rootGeneration,request+1,pending,ready)==ERROR_ACCESS_DENIED);
        CHECK(OpenNtBaseServiceAttachFrontendRequest(rootConnection,rootProcess.dwProcessId,
            rootGeneration,request,pending,ready)==ERROR_SUCCESS);
        CloseHandle(pending);
        CHECK(OpenNtBaseServiceRequestFrontend(launcher,GetCurrentProcessId(),
            launcherGeneration,capability)==ERROR_ALREADY_EXISTS);
        CHECK(OpenNtBaseServiceFrontendRequest(rootConnection,rootProcess.dwProcessId,
            rootGeneration,&request,&selected)==ERROR_NOT_FOUND && !request && !selected);
        CHECK(WaitForSingleObject(capability,0)==WAIT_TIMEOUT);
        /* The root owns I/O, independently of the submitting launcher. */
        CHECK(OpenNtBaseServiceTakeFrontend(worker,child.dwProcessId,workerGeneration,
            &delivered,&peer,&deliveredGeneration,&deliveredReady)==ERROR_SUCCESS);
        CHECK(!OpenNtBaseServiceFrontendUsage(rootConnection,rootProcess.dwProcessId,rootGeneration,
            &usage_pending,&usage_tasks) && !usage_pending && usage_tasks==1);
        CHECK(OpenNtBaseServiceRetireFrontend(rootConnection,rootProcess.dwProcessId,rootGeneration)==ERROR_BUSY);
        {
            PCONSOLERECORD source;PDOSRECORD record;ULONG saved;
            for(source=DOSHead;source && source->SequenceNumber!=workerGeneration;source=source->Next){}
            CHECK(source && source->DOSRecord);record=source->DOSRecord;saved=record->VDMState;
            /* State-projection unit test only: not guest execution acceptance.
             * Actual original dispatch/record creation is exercised above. */
            record->VDMState=VDM_READY;
            CHECK(!OpenNtBaseServiceFrontendUsage(rootConnection,rootProcess.dwProcessId,rootGeneration,
                &usage_pending,&usage_tasks) && !usage_pending && !usage_tasks);
            record->VDMState=VDM_HAS_RETURNED_ERROR_CODE;
            CHECK(!OpenNtBaseServiceFrontendUsage(rootConnection,rootProcess.dwProcessId,rootGeneration,
                &usage_pending,&usage_tasks) && !usage_tasks);
            record->VDMState=VDM_BUSY;
            CHECK(!OpenNtBaseServiceFrontendUsage(rootConnection,rootProcess.dwProcessId,rootGeneration,
                &usage_pending,&usage_tasks) && usage_tasks==1);
            record->VDMState=saved;
        }
        CHECK(GetProcessId(peer)==rootProcess.dwProcessId && deliveredGeneration==rootGeneration);
        CHECK(WriteFile(ui,"R",1,&count,NULL) && count==1);
        CHECK(ReadFile(delivered,&payload,1,&count,NULL) && count==1 && payload=='R');
        CloseHandle(delivered);CloseHandle(peer);CloseHandle(deliveredReady);CloseHandle(ui);
        /* Delivery is one-shot. A second wait has no future capability to
         * await and must return the same terminal status as TakeFrontend. */
        duplicate.connection=worker;duplicate.pid=child.dwProcessId;duplicate.generation=workerGeneration;
        duplicateThread=CreateThread(NULL,0,frontend_wait,&duplicate,0,NULL);CHECK(duplicateThread);
        duplicateWait=WaitForSingleObject(duplicateThread,1000);
        fprintf(stdout,"duplicate frontend wait: wait=%lu error=%lu\n",duplicateWait,duplicate.error);
        CHECK(OpenNtBaseServiceDisconnect(launcher)==ERROR_SUCCESS);launcher=NULL;
        CHECK(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT);
        CHECK(WaitForSingleObject(rootProcess.hProcess,0)==WAIT_TIMEOUT);
        CHECK(!OpenNtBaseServiceFrontendUsage(rootConnection,rootProcess.dwProcessId,rootGeneration,
            &usage_pending,&usage_tasks) && !usage_pending && usage_tasks==1);
        CHECK(OpenNtBaseServiceDisconnect(rootConnection)==ERROR_SUCCESS);
        CHECK(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT);
        CHECK(TerminateProcess(child.hProcess,0));
        CHECK(TerminateProcess(rootProcess.hProcess,0));
        CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
        CHECK(WaitForSingleObject(rootProcess.hProcess,5000)==WAIT_OBJECT_0);
        CHECK(WaitForSingleObject(duplicateThread,5000)==WAIT_OBJECT_0);
        CloseHandle(duplicateThread);
        CHECK(OpenNtBaseServiceDisconnect(worker)==ERROR_SUCCESS);
        CloseHandle(rootProcess.hThread);CloseHandle(rootProcess.hProcess);
        CloseHandle(child.hThread);CloseHandle(child.hProcess);CloseHandle(self);
        CloseHandle(capability);CloseHandle(other);CloseHandle(ready);
        CHECK(duplicateWait==WAIT_OBJECT_0 && duplicate.error==ERROR_ALREADY_EXISTS &&
            !duplicate.pipe && !duplicate.frontend && !duplicate.ready && !duplicate.frontend_generation);
        puts("PASS: original command authorizes root channel; launcher/root rundown preserves worker");
        return 0;
    }
    {
        HANDLE pending=NULL,ui=NULL,delivered=NULL,peer=NULL,ready,deliveredReady=NULL;
        DWORD rootGeneration=0,count=0,flags=HANDLE_FLAG_INHERIT;
        char payload=0;
        FRONTEND_WAIT_TEST waiting={0};
        HANDLE waitingThread=NULL;
        ready=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(ready);
        CHECK(frontend_pair(&pending,&ui)==0);
        CHECK(OpenNtBaseServiceAttachFrontend(launcher,GetCurrentProcessId(),
            launcherGeneration+1,pending,ready)==ERROR_ACCESS_DENIED);
        CHECK(OpenNtBaseServiceAttachFrontend(launcher,GetCurrentProcessId(),
            launcherGeneration,self,ready)==ERROR_INVALID_PARAMETER);
        CHECK(OpenNtBaseServiceAttachFrontend(launcher,GetCurrentProcessId(),
            launcherGeneration,pending,self)==ERROR_INVALID_PARAMETER);
        if(argc==2 && (!strcmp(argv[1],"--frontend-wait") ||
            !strcmp(argv[1],"--frontend-wait-worker-loss"))) {
            waiting.connection=worker;waiting.pid=child.dwProcessId;waiting.generation=workerGeneration;
            waitingThread=CreateThread(NULL,0,frontend_wait,&waiting,0,NULL);CHECK(waitingThread);
            CHECK(WaitForSingleObject(waitingThread,100)==WAIT_TIMEOUT);
            if(!strcmp(argv[1],"--frontend-wait-worker-loss")) {
                CHECK(TerminateProcess(child.hProcess,23));
                CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
                CHECK(WaitForSingleObject(waitingThread,5000)==WAIT_OBJECT_0);
                CHECK(waiting.error==ERROR_ACCESS_DENIED && !waiting.pipe &&
                    !waiting.frontend && !waiting.ready && !waiting.frontend_generation);
                CloseHandle(waitingThread);CloseHandle(pending);CloseHandle(ui);CloseHandle(ready);
                CHECK(OpenNtBaseServiceDisconnect(worker)==ERROR_SUCCESS);worker=NULL;
                CHECK(OpenNtBaseServiceDisconnect(launcher)==ERROR_SUCCESS);launcher=NULL;
                CloseHandle(child.hThread);CloseHandle(child.hProcess);CloseHandle(self);
                puts("PASS: waiting frontend delivery cancels on worker death without publishing handles");
                return 0;
            }
        }
        CHECK(OpenNtBaseServiceAttachFrontend(launcher,GetCurrentProcessId(),
            launcherGeneration,pending,ready)==ERROR_SUCCESS);
        CHECK(OpenNtBaseServiceAttachFrontend(launcher,GetCurrentProcessId(),
            launcherGeneration,pending,ready)==ERROR_ALREADY_EXISTS);
        CloseHandle(pending);
        if (argc==2 && !strcmp(argv[1],"--frontend-rundown")) {
            CHECK(OpenNtBaseServiceDisconnect(launcher)==ERROR_SUCCESS);launcher=NULL;
            CHECK(!ReadFile(ui,&payload,1,&count,NULL) && GetLastError()==ERROR_BROKEN_PIPE);
            CHECK(OpenNtBaseServiceTakeFrontend(worker,child.dwProcessId,workerGeneration,
                &delivered,&peer,&rootGeneration,&deliveredReady)==ERROR_PIPE_NOT_CONNECTED && !delivered && !peer && !deliveredReady);
            CloseHandle(ui);CloseHandle(ready);
            CHECK(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT);
            CHECK(TerminateProcess(child.hProcess,0));
            CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
            CHECK(OpenNtBaseServiceDisconnect(worker)==ERROR_SUCCESS);worker=NULL;
            CloseHandle(child.hThread);CloseHandle(child.hProcess);CloseHandle(self);
            puts("PASS: launcher rundown closes I/O channel without terminating worker");
            return 0;
        }
        CHECK(OpenNtBaseServiceTakeFrontend(launcher,GetCurrentProcessId(),launcherGeneration,
            &delivered,&peer,&rootGeneration,&deliveredReady)==ERROR_ACCESS_DENIED && !delivered && !peer && !deliveredReady);
        CHECK(OpenNtBaseServiceTakeFrontend(worker,child.dwProcessId,workerGeneration+1,
            &delivered,&peer,&rootGeneration,&deliveredReady)==ERROR_ACCESS_DENIED);
        if(waitingThread) {
            CHECK(WaitForSingleObject(waitingThread,5000)==WAIT_OBJECT_0 && !waiting.error);
            CloseHandle(waitingThread);
            delivered=waiting.pipe;peer=waiting.frontend;rootGeneration=waiting.frontend_generation;
            deliveredReady=waiting.ready;
        } else {
            CHECK(OpenNtBaseServiceTakeFrontend(worker,child.dwProcessId,workerGeneration,
                &delivered,&peer,&rootGeneration,&deliveredReady)==ERROR_SUCCESS);
        }
        CHECK(WaitForSingleObject(deliveredReady,0)==WAIT_TIMEOUT);
        CHECK(!SetEvent(deliveredReady) && GetLastError()==ERROR_ACCESS_DENIED);
        CHECK(SetEvent(ready) && WaitForSingleObject(deliveredReady,0)==WAIT_OBJECT_0);
        CHECK(ResetEvent(ready) && WaitForSingleObject(deliveredReady,0)==WAIT_TIMEOUT);
        CHECK(GetHandleInformation(deliveredReady,&flags) && !(flags&HANDLE_FLAG_INHERIT));
        CHECK(GetProcessId(peer)==GetCurrentProcessId() && rootGeneration==launcherGeneration);
        CHECK(!TerminateProcess(peer,99) && GetLastError()==ERROR_ACCESS_DENIED);
        CHECK(GetHandleInformation(delivered,&flags) && !(flags&HANDLE_FLAG_INHERIT));
        CHECK(WriteFile(ui,"F",1,&count,NULL) && count==1);
        CHECK(ReadFile(delivered,&payload,1,&count,NULL) && count==1 && payload=='F');
        CHECK(WriteFile(delivered,"W",1,&count,NULL) && count==1);
        CHECK(ReadFile(ui,&payload,1,&count,NULL) && count==1 && payload=='W');
        CloseHandle(peer);CloseHandle(delivered);CloseHandle(deliveredReady);CloseHandle(ready);
        CHECK(OpenNtBaseServiceTakeFrontend(worker,child.dwProcessId,workerGeneration,
            &delivered,&peer,&rootGeneration,&deliveredReady)==ERROR_ALREADY_EXISTS && !delivered && !peer && !deliveredReady);
        CHECK(!ReadFile(ui,&payload,1,&count,NULL) && GetLastError()==ERROR_BROKEN_PIPE);
        CloseHandle(ui);
    }
    CHECK(claimed==reservation && task==reply.u.CheckVDM.iTask && console!=NULL);
    /* The management plane copies a server-owned worker projection.  First
     * probe the count, then require the same broker generation/sequence
     * which original BaseSrv assigned at authenticated registration. */
    CHECK(OpenNtBaseServiceSnapshot(service,&managementEpoch,NULL,0,&workerInfoCount)==ERROR_INSUFFICIENT_BUFFER);
    CHECK(managementEpoch!=0 && workerInfoCount==1);
    CHECK(OpenNtBaseServiceSnapshot(service,&managementEpoch,&workerInfo,1,&workerInfoCount)==ERROR_SUCCESS);
    CHECK(workerInfoCount==1 && workerInfo.sequence==workerGeneration &&
        workerInfo.started_filetime!=0 && !wcscmp(workerInfo.image,L"MEM.EXE"));
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
    { HANDLE peer=NULL;
      CHECK(OpenNtBaseServiceRetainCommandWorker(launcher,GetCurrentProcessId(),
          launcherGeneration,&peer)==ERROR_NOT_READY && peer==NULL); }
    { DWORD exitCode=STILL_ACTIVE;
      HANDLE native=NULL;
      CHECK(OpenNtBaseServiceSelectNativeWorker(launcher,GetCurrentProcessId(),
          launcherGeneration,&native)==ERROR_INVALID_STATE && !native);
      CHECK(OpenNtBaseServiceExitCode(launcher,GetCurrentProcessId(),launcherGeneration,
          parentReceipt,&exitCode)==ERROR_SUCCESS && exitCode==7);
      CHECK(OpenNtBaseServiceSelectNativeWorker(launcher,GetCurrentProcessId(),
          launcherGeneration,&native)==ERROR_NOT_FOUND && !native); }
    /* Service API handles are borrowed receipt references; the RPC boundary
     * duplicates them for a remote caller. This in-process fixture must not
     * close them and later let receipt teardown close a reused handle value. */
    parentEvent=NULL;

    /* A same-Console launch may now use the original ready record: CheckDOS
     * signals the actual worker wait and the second-time Get consumes it. */
    CHECK(CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&startup,&laterChild));
    CHECK(OpenNtBaseServiceConnect(service,laterChild.hProcess,&later,&laterGeneration)==ERROR_SUCCESS);
    {
        DWORD members[2]={laterChild.dwProcessId,child.dwProcessId};
        laterFrontend=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(laterFrontend);
        CHECK(!OpenNtBaseServiceRegisterFrontendRoot(later,laterChild.dwProcessId,
            laterGeneration,laterFrontend));
        CHECK(OpenNtBaseServiceReportConsoleMembers(later,laterChild.dwProcessId,
            laterGeneration,2,members)==ERROR_SUCCESS);
    }
    check.u.CheckVDM.ConsoleHandle=OPENNT_BASE_CONSOLE_EXISTING;
    CHECK(OpenNtBaseEncodeCheckCommand(&check,6,laterGeneration,NULL,0,&wireBytes));
    free(wire);wire=malloc(wireBytes);CHECK(wire && OpenNtBaseEncodeCheckCommand(
        &check,6,laterGeneration,wire,wireBytes,&wireBytes));
    CHECK(OpenNtBaseServiceCheck(later,laterChild.dwProcessId,laterGeneration,
        wire,wireBytes,NULL,0,&answerBytes,&laterParentEvent,&laterParentReceipt)==ERROR_INSUFFICIENT_BUFFER && answerBytes);
    free(answer);answer=malloc(answerBytes);CHECK(answer && OpenNtBaseServiceCheck(
        later,laterChild.dwProcessId,laterGeneration,wire,wireBytes,answer,answerBytes,&answerBytes,
        &laterParentEvent,&laterParentReceipt)==ERROR_SUCCESS);
    CHECK(OpenNtBaseApplyCheckReply(answer,answerBytes,laterGeneration,6,&reply));
    CHECK(reply.ReturnValue==STATUS_SUCCESS && reply.u.CheckVDM.VDMState==VDM_PRESENT_AND_READY &&
        laterParentEvent!=NULL && laterParentReceipt!=0);
    CHECK(laterParentReceipt!=parentReceipt);
    {
        HANDLE pending=NULL,ui=NULL,ready=CreateEventW(NULL,TRUE,FALSE,NULL);
        CHECK(ready);
        CHECK(frontend_pair(&pending,&ui)==0);
        CHECK(OpenNtBaseServiceAttachFrontend(later,laterChild.dwProcessId,
            laterGeneration,pending,ready)==ERROR_ALREADY_EXISTS);
        CloseHandle(pending);CloseHandle(ui);CloseHandle(ready);
    }
    CHECK(WaitForSingleObject(getWait,0)==WAIT_OBJECT_0);
    { HANDLE peer=NULL;
      CHECK(OpenNtBaseServiceRetainCommandWorker(later,laterChild.dwProcessId,
          laterGeneration,&peer)==ERROR_SUCCESS);
      CHECK(peer && GetProcessId(peer)==child.dwProcessId);
      CloseHandle(peer); }
    getWait=NULL;

    ZeroMemory(&get,sizeof(get));
    get.u.GetNextVDMCommand.StartupInfo=&getStartup;
    get.u.GetNextVDMCommand.VDMState=ASKING_FOR_SECOND_TIME|ASKING_FOR_DOS_BINARY;
    get.u.GetNextVDMCommand.CmdLine=getCmd;get.u.GetNextVDMCommand.CmdLen=sizeof(getCmd);
    get.u.GetNextVDMCommand.AppName=getApp;get.u.GetNextVDMCommand.AppLen=sizeof(getApp);
    get.u.GetNextVDMCommand.Env=getEnv;get.u.GetNextVDMCommand.EnvLen=sizeof(getEnv);
    get.u.GetNextVDMCommand.PifFile=getPif;get.u.GetNextVDMCommand.PifLen=sizeof(getPif);
    get.u.GetNextVDMCommand.CurDirectory=getDirectory;
    get.u.GetNextVDMCommand.CurDirectoryLen=sizeof(getDirectory);
    if (argc==2 && !strcmp(argv[1],"--reenter-pending-command")) {
        CHECK(OpenNtBaseServiceReenter(worker,child.dwProcessId,workerGeneration,
            INCREMENT_REENTER_COUNT)==ERROR_SUCCESS);
        CHECK(OpenNtBaseServiceReenter(worker,child.dwProcessId,workerGeneration,
            DECREMENT_REENTER_COUNT)==ERROR_SUCCESS);
    }
    CHECK(OpenNtBaseEncodeGetCommand(&get,7,workerGeneration,NULL,0,&getWireBytes));
    free(getWire);getWire=malloc(getWireBytes);CHECK(getWire && OpenNtBaseEncodeGetCommand(
        &get,7,workerGeneration,getWire,getWireBytes,&getWireBytes));
    CHECK(OpenNtBaseServiceGet(worker,child.dwProcessId,workerGeneration,getWire,getWireBytes,
        &getAnswer,&wireBytes,&getWait,standard,&standardCount)==ERROR_SUCCESS);
    CHECK(getAnswer!=NULL && getWait==NULL && standardCount==0);
    CHECK(OpenNtBaseApplyGetCommand(getAnswer,wireBytes,workerGeneration,7,&get));
    CHECK(get.ReturnValue==STATUS_SUCCESS &&
        get.u.GetNextVDMCommand.CmdLen==sizeof(cmd) && !memcmp(getCmd,cmd,sizeof(cmd)));
    OpenNtBaseServiceReleaseCommandReply(getAnswer);getAnswer=NULL;
    if (argc==2 && (!strcmp(argv[1],"--reenter-before-return") ||
        !strcmp(argv[1],"--reenter-after-return") ||
        !strcmp(argv[1],"--reenter-nested-return") ||
        !strcmp(argv[1],"--reenter-pending-command") ||
        !strcmp(argv[1],"--reenter-before-increment"))) {
        BOOL startupOrder=!strcmp(argv[1],"--reenter-before-increment");
        BOOL queued=!strcmp(argv[1],"--reenter-pending-command");
        BOOL nested=!strcmp(argv[1],"--reenter-nested-return");
        BOOL early=strcmp(argv[1],"--reenter-after-return") && !startupOrder;
        DWORD wake,exitCode=STILL_ACTIVE;
        /* Same completed nested command, with the native parent finishing
         * either before or after cmdReturnExitCode requests its next command.
         * No sleeps: calls fix the ordering at the real service boundary. */
        if (nested) CHECK(OpenNtBaseServiceReenter(worker,child.dwProcessId,
            workerGeneration,INCREMENT_REENTER_COUNT)==ERROR_SUCCESS);
        if (!queued && !startupOrder) CHECK(OpenNtBaseServiceReenter(worker,
            child.dwProcessId,workerGeneration,INCREMENT_REENTER_COUNT)==ERROR_SUCCESS);
        if (early && !queued) CHECK(OpenNtBaseServiceReenter(worker,child.dwProcessId,
            workerGeneration,DECREMENT_REENTER_COUNT)==ERROR_SUCCESS);
        get.u.GetNextVDMCommand.VDMState=RETURN_ON_NO_COMMAND |
            (startupOrder ? NO_PARENT_TO_WAKE : 0);
        get.u.GetNextVDMCommand.ExitCode=29;
        CHECK(OpenNtBaseEncodeGetCommand(&get,8,workerGeneration,NULL,0,&getWireBytes));
        free(getWire);getWire=malloc(getWireBytes);
        CHECK(getWire && OpenNtBaseEncodeGetCommand(&get,8,workerGeneration,
            getWire,getWireBytes,&getWireBytes));
        CHECK(OpenNtBaseServiceGet(worker,child.dwProcessId,workerGeneration,
            getWire,getWireBytes,&getAnswer,&wireBytes,&getWait,standard,
            &standardCount)==ERROR_SUCCESS);
        CHECK(getAnswer && !standardCount);
        CHECK(OpenNtBaseApplyGetCommand(getAnswer,wireBytes,workerGeneration,8,&get));
        OpenNtBaseServiceReleaseCommandReply(getAnswer);getAnswer=NULL;
        if (!startupOrder) {
            CHECK(WaitForSingleObject(laterParentEvent,0)==WAIT_OBJECT_0);
            CHECK(OpenNtBaseServiceExitCode(later,laterChild.dwProcessId,laterGeneration,
                laterParentReceipt,&exitCode)==ERROR_SUCCESS && exitCode==29);
        }
        if (!early) {
            CHECK(getWait);
            CHECK(WaitForSingleObject(getWait,0)==WAIT_TIMEOUT);
            if (startupOrder) {
                CHECK(OpenNtBaseServiceReenter(worker,child.dwProcessId,
                    workerGeneration,INCREMENT_REENTER_COUNT)==ERROR_SUCCESS);
                CHECK(WaitForSingleObject(getWait,0)==WAIT_TIMEOUT);
            }
            CHECK(OpenNtBaseServiceReenter(worker,child.dwProcessId,
                workerGeneration,DECREMENT_REENTER_COUNT)==ERROR_SUCCESS);
        }
        wake=getWait ? WaitForSingleObject(getWait,0) :
            (get.ReturnValue==STATUS_NO_MEMORY ? WAIT_OBJECT_0 : WAIT_FAILED);
        fprintf(stdout,"reenter mode=%s wait=%lu expected=0\n",argv[1],wake);
        if (wake==WAIT_OBJECT_0) {
            if (getWait) {
                get.u.GetNextVDMCommand.VDMState=RETURN_ON_NO_COMMAND|ASKING_FOR_SECOND_TIME;
                get.u.GetNextVDMCommand.ExitCode=0;
                CHECK(OpenNtBaseEncodeGetCommand(&get,9,workerGeneration,
                    getWire,getWireBytes,&getWireBytes));
                CHECK(OpenNtBaseServiceGet(worker,child.dwProcessId,workerGeneration,
                    getWire,getWireBytes,&getAnswer,&wireBytes,&getWait,standard,
                    &standardCount)==ERROR_SUCCESS);
                CHECK(!getWait && getAnswer && !standardCount);
                CHECK(OpenNtBaseApplyGetCommand(getAnswer,wireBytes,workerGeneration,9,&get));
                CHECK(get.ReturnValue==STATUS_NO_MEMORY);
                OpenNtBaseServiceReleaseCommandReply(getAnswer);getAnswer=NULL;
            }
            /* The consumed completion cannot wake a later shell-out; nor
             * may a stale command-delivery event bypass its native wait. */
            get.u.GetNextVDMCommand.VDMState=RETURN_ON_NO_COMMAND|NO_PARENT_TO_WAKE;
            get.u.GetNextVDMCommand.ExitCode=0;
            CHECK(OpenNtBaseEncodeGetCommand(&get,10,workerGeneration,
                getWire,getWireBytes,&getWireBytes));
            CHECK(OpenNtBaseServiceGet(worker,child.dwProcessId,workerGeneration,
                getWire,getWireBytes,&getAnswer,&wireBytes,&getWait,standard,
                &standardCount)==ERROR_SUCCESS);
            CHECK(getAnswer && getWait && !standardCount);
            CHECK(WaitForSingleObject(getWait,0)==WAIT_TIMEOUT);
            OpenNtBaseServiceReleaseCommandReply(getAnswer);getAnswer=NULL;
        }
        /* Clean the fixture-owned suspended children even on the expected
         * red result; a missing wake must not strand test processes. */
        CHECK(TerminateProcess(child.hProcess,0));
        CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
        CHECK(OpenNtBaseServiceDisconnect(worker)==ERROR_SUCCESS);worker=NULL;
        CHECK(OpenNtBaseServiceDisconnect(later)==ERROR_SUCCESS);later=NULL;
        CHECK(TerminateProcess(laterChild.hProcess,0));
        CHECK(WaitForSingleObject(laterChild.hProcess,5000)==WAIT_OBJECT_0);
        CHECK(OpenNtBaseServiceDisconnect(launcher)==ERROR_SUCCESS);launcher=NULL;
        CloseHandle(laterChild.hThread);CloseHandle(laterChild.hProcess);
        CloseHandle(child.hThread);CloseHandle(child.hProcess);CloseHandle(self);
        free(getWire);free(updateAnswer);free(updateWire);free(answer);free(wire);
        /* Process signalling precedes completion of its registered rundown. */
        { ULONGLONG deadline=GetTickCount64()+5000;
          while (!OpenNtBaseServiceIsEmpty(service) && GetTickCount64()<deadline) Sleep(1); }
        CHECK(OpenNtBaseServiceIsEmpty(service));
        CHECK(OpenNtBaseServiceStop(service));
        CHECK(wake==WAIT_OBJECT_0);
        puts("PASS: native completion wakes nested DOS return");
        return 0;
    }
    if (argc==2 && (!strcmp(argv[1],"--launcher-completed-rundown") ||
        !strcmp(argv[1],"--launcher-completed-uncollected") ||
        !strcmp(argv[1],"--completed-worker-loss") ||
        !strcmp(argv[1],"--completed-worker-exit"))) {
        DWORD exitCode=STILL_ACTIVE;
        DWORD collectedError=ERROR_IO_PENDING;
        BOOL collected=!strcmp(argv[1],"--launcher-completed-rundown");
        BOOL workerExit=!strcmp(argv[1],"--completed-worker-exit");
        BOOL workerLoss=!strcmp(argv[1],"--completed-worker-loss") || workerExit;
        BOOL recordExists=FALSE;
        if (collected) {
            HANDLE native=NULL;
            CHECK(OpenNtBaseServiceSelectNativeWorker(later,laterChild.dwProcessId,
                laterGeneration,&native)==ERROR_INVALID_STATE && !native);
        }
        /* Complete the second command through original GetNext before its
         * launcher dies. A late process notification/rundown must not treat
         * this retired pair as an unfinished task and kill the idle worker. */
        get.u.GetNextVDMCommand.VDMState=ASKING_FOR_DOS_BINARY;
        get.u.GetNextVDMCommand.ExitCode=29;
        CHECK(OpenNtBaseEncodeGetCommand(&get,8,workerGeneration,NULL,0,&getWireBytes));
        free(getWire);getWire=malloc(getWireBytes);
        CHECK(getWire && OpenNtBaseEncodeGetCommand(&get,8,workerGeneration,
            getWire,getWireBytes,&getWireBytes));
        CHECK(OpenNtBaseServiceGet(worker,child.dwProcessId,workerGeneration,getWire,getWireBytes,
            &getAnswer,&wireBytes,&getWait,standard,&standardCount)==ERROR_SUCCESS);
        CHECK(getAnswer && getWait && !standardCount);
        OpenNtBaseServiceReleaseCommandReply(getAnswer);getAnswer=NULL;
        CHECK(WaitForSingleObject(laterParentEvent,0)==WAIT_OBJECT_0);
        if (workerLoss) {
            ULONGLONG deadline=GetTickCount64()+5000;
            if (workerExit) {
                BOOL closeWait=FALSE;
                CHECK(!OpenNtBaseServiceExit(worker,child.dwProcessId,workerGeneration,FALSE,0,&closeWait));
            }
            CHECK(TerminateProcess(child.hProcess,91));
            CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
            do {
                (void)BaseSrvDOSWorkerWaitPending(console,&recordExists);
                if (!recordExists) break;
                Sleep(1);
            } while (GetTickCount64()<deadline);
            CHECK(!recordExists);
            /* A different receipt must not consume the completed reply. */
            CHECK(OpenNtBaseServiceExitCode(later,laterChild.dwProcessId,laterGeneration,
                laterParentReceipt+2,&exitCode)==ERROR_SUCCESS && exitCode==0);
            collectedError=OpenNtBaseServiceExitCode(later,laterChild.dwProcessId,laterGeneration,
                laterParentReceipt,&exitCode);
            fprintf(stdout,"completed-before-worker-loss: query=%lu exit=%lu expected=29\n",collectedError,exitCode);
            {
                DWORD repeated=STILL_ACTIVE;
                CHECK(OpenNtBaseServiceExitCode(later,laterChild.dwProcessId,laterGeneration,
                    laterParentReceipt,&repeated)==ERROR_SUCCESS && repeated==0);
            }
        } else if (collected) {
            HANDLE native=NULL;
            CHECK(OpenNtBaseServiceExitCode(later,laterChild.dwProcessId,laterGeneration,
                laterParentReceipt+2,&exitCode)==ERROR_SUCCESS && exitCode==0);
            CHECK(OpenNtBaseServiceSelectNativeWorker(later,laterChild.dwProcessId,
                laterGeneration,&native)==ERROR_INVALID_STATE && !native);
            CHECK(OpenNtBaseServiceExitCode(later,laterChild.dwProcessId,laterGeneration,
                laterParentReceipt,&exitCode)==ERROR_SUCCESS && exitCode==29);
            /* Only collecting the exact completed DOS receipt permits native
             * selection. This fixture has no native worker to select. */
            CHECK(OpenNtBaseServiceSelectNativeWorker(later,laterChild.dwProcessId,
                laterGeneration,&native)==ERROR_NOT_FOUND && !native);
        }
        laterParentEvent=NULL; /* borrowed receipt, not ours to close */
        CHECK(TerminateProcess(laterChild.hProcess,71));
        CHECK(WaitForSingleObject(laterChild.hProcess,5000)==WAIT_OBJECT_0);
        CHECK(OpenNtBaseServiceDisconnect(later)==ERROR_SUCCESS);later=NULL;
        /* Disconnect is synchronous: observe its result, not a sleep-based
         * guess that the connection rundown has happened. */
        if (!workerLoss) {
            CHECK(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT);
            CHECK(WaitForSingleObject(getWait,0)==WAIT_TIMEOUT);
            CHECK(BaseSrvDOSWorkerWaitPending(console,&recordExists) && recordExists);
            puts(collected ?
            "PASS: completed command=29 collected=1; late launcher death/rundown preserves idle worker and original GetNext wait" :
            "PASS: completed command=29 collected=0; late launcher death/rundown preserves idle worker and original GetNext wait");
            CHECK(TerminateProcess(child.hProcess,0));
        }
        CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
        CHECK(OpenNtBaseServiceDisconnect(worker)==ERROR_SUCCESS);worker=NULL;
        CHECK(OpenNtBaseServiceDisconnect(launcher)==ERROR_SUCCESS);launcher=NULL;
        CloseHandle(laterChild.hThread);CloseHandle(laterChild.hProcess);
        CloseHandle(child.hThread);CloseHandle(child.hProcess);CloseHandle(self);
        free(getWire);free(updateAnswer);free(updateWire);free(answer);free(wire);
        { ULONGLONG deadline=GetTickCount64()+5000;
          while (!OpenNtBaseServiceIsEmpty(service) && GetTickCount64()<deadline) Sleep(1); }
        CHECK(OpenNtBaseServiceIsEmpty(service));
        CHECK(OpenNtBaseServiceStop(service));
        if (workerLoss) {
            CHECK(collectedError==ERROR_SUCCESS && exitCode==29);
            puts("PASS: completed DOS exit code survives subsequent worker death before parent collection");
        }
        return 0;
    }
    if (argc==2 && (!strcmp(argv[1],"--management-terminate") ||
        !strcmp(argv[1],"--launcher-exit-survival") ||
        !strcmp(argv[1],"--launcher-disconnect-survival") ||
        !strcmp(argv[1],"--completion-rundown-race") ||
        !strcmp(argv[1],"--unfinished-worker-exit"))) {
        DWORD exitCode=STILL_ACTIVE;
        BOOL race=!strcmp(argv[1],"--completion-rundown-race");
        RUNDOWN_RACE_TEST competing={0};
        HANDLE rundownThread=NULL;
        /* The manager's positive path receives only the selected snapshot
         * identity.  The service resolves its retained watch and the normal
         * worker-exit callback must wake the waiting parent and remove the
         * original record before this fixture tears down its own processes. */
        if (!strcmp(argv[1],"--unfinished-worker-exit")) {
            BOOL closeWait=FALSE;
            CHECK(!OpenNtBaseServiceExit(worker,child.dwProcessId,workerGeneration,FALSE,0,&closeWait));
            CHECK(TerminateProcess(child.hProcess,91));
        } else if (strcmp(argv[1],"--management-terminate")) {
            HANDLE retainedEvent=NULL;
            CHECK(DuplicateHandle(GetCurrentProcess(),laterParentEvent,GetCurrentProcess(),
                &retainedEvent,SYNCHRONIZE,FALSE,0));
            /* ServiceDisconnect drains its borrowed receipt handle. Keep
             * a fixture-owned reference to observe the original completion. */
            laterParentEvent=retainedEvent;
            if (!strcmp(argv[1],"--launcher-exit-survival")) {
                CHECK(TerminateProcess(laterChild.hProcess,71));
                CHECK(WaitForSingleObject(laterChild.hProcess,5000)==WAIT_OBJECT_0);
            }
            if (race) {
                competing.connection=later;competing.error=ERROR_IO_PENDING;
                competing.ready=CreateEventW(NULL,TRUE,FALSE,NULL);
                competing.go=CreateEventW(NULL,TRUE,FALSE,NULL);
                CHECK(competing.ready && competing.go);
                rundownThread=CreateThread(NULL,0,competing_rundown,&competing,0,NULL);
                CHECK(rundownThread && WaitForSingleObject(competing.ready,5000)==WAIT_OBJECT_0);
            } else CHECK(OpenNtBaseServiceDisconnect(later)==ERROR_SUCCESS);
            later=NULL;
            CHECK(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT);
            CHECK(WaitForSingleObject(laterParentEvent,0)==WAIT_TIMEOUT);
            /* Complete the original DOS record after its submitter vanished:
             * nonzero task exit is not a worker failure. */
            get.u.GetNextVDMCommand.VDMState=ASKING_FOR_DOS_BINARY;
            get.u.GetNextVDMCommand.ExitCode=29;
            CHECK(OpenNtBaseEncodeGetCommand(&get,8,workerGeneration,
                getWire,getWireBytes,&getWireBytes));
            if (race) CHECK(SetEvent(competing.go));
            CHECK(OpenNtBaseServiceGet(worker,child.dwProcessId,workerGeneration,
                getWire,getWireBytes,&getAnswer,&wireBytes,&getWait,standard,
                &standardCount)==ERROR_SUCCESS);
            CHECK(getAnswer && getWait && !standardCount);
            OpenNtBaseServiceReleaseCommandReply(getAnswer);getAnswer=NULL;
            if (race) {
                DWORD threadCode;
                CHECK(WaitForSingleObject(rundownThread,5000)==WAIT_OBJECT_0);
                CHECK(GetExitCodeThread(rundownThread,&threadCode) && !threadCode && !competing.error);
                CloseHandle(rundownThread);CloseHandle(competing.go);CloseHandle(competing.ready);
            }
            CHECK(WaitForSingleObject(laterParentEvent,0)==WAIT_OBJECT_0);
            CHECK(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT);
            CHECK(TerminateProcess(child.hProcess,0)); /* fixture cleanup */
            CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
        } else CHECK(OpenNtBaseServiceTerminateWorker(service,child.dwProcessId)==ERROR_SUCCESS);
        { DWORD wait=WaitForSingleObject(laterParentEvent,5000);
          if (wait!=WAIT_OBJECT_0) fprintf(stderr,"pair completion wait=%lu error=%lu handle=%p\n",wait,GetLastError(),laterParentEvent);
          CHECK(wait==WAIT_OBJECT_0); }
        if (later) CHECK(OpenNtBaseServiceExitCode(later,laterChild.dwProcessId,laterGeneration,
            laterParentReceipt,&exitCode)==ERROR_PROCESS_ABORTED);
        if (strcmp(argv[1],"--management-terminate") && strcmp(argv[1],"--unfinished-worker-exit")) CloseHandle(laterParentEvent);
        laterParentEvent=NULL;
        CHECK(OpenNtBaseServiceDisconnect(worker)==ERROR_SUCCESS);worker=NULL;
        { DWORD release=OpenNtBaseServiceReleaseReservation(launcher,GetCurrentProcessId(),
              launcherGeneration,reservation);
          /* Process-exit cleanup may already have released the reservation. */
          CHECK(release==ERROR_SUCCESS || release==ERROR_NOT_FOUND); }
        if (later) { CHECK(OpenNtBaseServiceDisconnect(later)==ERROR_SUCCESS);later=NULL; }
        CHECK(OpenNtBaseServiceDisconnect(launcher)==ERROR_SUCCESS);launcher=NULL;
        CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
        TerminateProcess(laterChild.hProcess,0);WaitForSingleObject(laterChild.hProcess,5000);
        CloseHandle(laterChild.hThread);CloseHandle(laterChild.hProcess);
        CloseHandle(child.hThread);CloseHandle(child.hProcess);CloseHandle(self);
        free(getWire);free(updateAnswer);free(updateWire);free(answer);free(wire);
        /* Task completion now precedes fixture-induced worker termination.
         * Process signalling alone does not join the broker exit callback. */
        { ULONGLONG deadline=GetTickCount64()+5000;
          while (!OpenNtBaseServiceIsEmpty(service) && GetTickCount64()<deadline) Sleep(1); }
        CHECK(OpenNtBaseServiceIsEmpty(service));
        CHECK(OpenNtBaseServiceStop(service));
        if (race) puts("PASS: competing original task completion and launcher rundown preserve completion event and live worker");
        puts(!strcmp(argv[1],"--unfinished-worker-exit") ?
            "PASS: original ExitVDM fails unfinished parent before deleting its record" :
            !strcmp(argv[1],"--management-terminate") ?
            "PASS: explicit management shutdown performs original worker-exit cleanup" :
            "PASS: launcher loss preserves worker; later nonzero DOS completion signals parent without worker failure");
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
    laterParentEvent=NULL;
    parentEvent=NULL;
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
    CloseHandle(laterFrontend);laterFrontend=NULL;
    TerminateProcess(laterChild.hProcess,0);WaitForSingleObject(laterChild.hProcess,INFINITE);
    CloseHandle(laterChild.hThread);CloseHandle(laterChild.hProcess);laterChild.hThread=laterChild.hProcess=NULL;

    /* The worker learns WOW from its launcher reservation.  This retains the
     * original -1 Console sentinel for sequence publication and ExitVDM;
     * it is not inferred from a worker-supplied request bit. */
    wowFrontend=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(wowFrontend);
    CHECK(OpenNtBaseServiceRegisterFrontendRoot(launcher,GetCurrentProcessId(),
        launcherGeneration,wowFrontend)==ERROR_SUCCESS);
    CHECK(OpenNtBaseServiceAcquireConsoleContext(launcher,GetCurrentProcessId(),
        launcherGeneration,wowFrontend,&wowContext)==ERROR_SUCCESS && wowContext);
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
    CHECK(!OpenNtBaseServiceSnapshot(service,&managementEpoch,&workerInfo,1,&workerInfoCount));
    CHECK(workerInfoCount==1 && workerInfo.kind==1 && workerInfo.stack_depth==1 &&
        workerInfo.task==wowTask);
    CHECK(OpenNtBaseServiceWowStarted(launcher,GetCurrentProcessId(),launcherGeneration,
        wowTask)==ERROR_ACCESS_DENIED);
    CHECK(OpenNtBaseServiceWowStarted(wowWorker,wowChild.dwProcessId,wowGeneration+1,
        wowTask)==ERROR_ACCESS_DENIED);
    CHECK(OpenNtBaseServiceWowStarted(wowWorker,wowChild.dwProcessId,wowGeneration,
        wowTask)==ERROR_INVALID_STATE);
    CHECK(OpenNtBaseServiceWowStartup(launcher,GetCurrentProcessId(),launcherGeneration,
        parentReceipt+2,&wowStartup,&wowStarted)==ERROR_ACCESS_DENIED && !wowStartup && !wowStarted);
    CHECK(OpenNtBaseServiceWowStartup(wowWorker,wowChild.dwProcessId,wowGeneration,
        parentReceipt,&wowStartup,&wowStarted)==ERROR_ACCESS_DENIED && !wowStartup);
    if (argc==1) {
        CHECK(OpenNtBaseServiceWowStartup(launcher,GetCurrentProcessId(),launcherGeneration,
            parentReceipt,&wowStartup,&wowStarted)==ERROR_SUCCESS && wowStartup && !wowStarted);
        CHECK(WaitForSingleObject(wowStartup,0)==WAIT_TIMEOUT);
        CHECK(!SetEvent(wowStartup) && GetLastError()==ERROR_ACCESS_DENIED);
    }
    {
        HANDLE denied=NULL;
        /* Even a valid C-segment capability cannot turn a registered WOW
         * worker or its submitted WOW request into a character I/O member.
         * Original Get/Exit below must still work after these rejections. */
        CHECK(OpenNtBaseServiceAcquireConsoleContext(wowWorker,wowChild.dwProcessId,
            wowGeneration,wowFrontend,&denied)==ERROR_ACCESS_DENIED && !denied);
        CHECK(OpenNtBaseServiceAcquireConsoleContext(launcher,GetCurrentProcessId(),
            launcherGeneration,wowFrontend,&denied)==ERROR_ACCESS_DENIED && !denied);
        CHECK(OpenNtBaseServiceBindConsoleContext(wowWorker,wowChild.dwProcessId,
            wowGeneration,wowContext)==ERROR_ACCESS_DENIED);
        CHECK(OpenNtBaseServiceRequestFrontend(launcher,GetCurrentProcessId(),
            launcherGeneration,wowFrontend)==ERROR_NOT_READY);
        CHECK(OpenNtBaseServiceRegisterFrontendRoot(wowWorker,wowChild.dwProcessId,
            wowGeneration,wowFrontend)==ERROR_ACCESS_DENIED);
        puts("PASS WOW request/worker rejects valid character frontend/context without changing original WOW Get/Exit");
    }
    /* A registered WOW worker still cannot enroll another process's HWND. */
    CHECK(OpenNtBaseServiceRegisterWowExec(wowWorker,wowChild.dwProcessId,wowGeneration,
        (DWORD)(ULONG_PTR)GetDesktopWindow())==ERROR_ACCESS_DENIED);
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
    if (wowStartup) {
        /* Test-only identity fault: same task number, different original
         * parent receipt must not notify this launcher. No product mutation. */
        HANDLE original;
        CHECK(WOWHead && WOWHead->WOWRecord && WOWHead->WOWRecord->iTask==wowTask);
        original=WOWHead->WOWRecord->hWaitForParentServer;
        WOWHead->WOWRecord->hWaitForParentServer=(HANDLE)(ULONG_PTR)(parentReceipt+2);
        CHECK(OpenNtBaseServiceWowStarted(wowWorker,wowChild.dwProcessId,wowGeneration,
            wowTask)==ERROR_SUCCESS);
        CHECK(WaitForSingleObject(wowStartup,0)==WAIT_TIMEOUT);
        WOWHead->WOWRecord->hWaitForParentServer=original;
    }
    CHECK(OpenNtBaseServiceWowStarted(wowWorker,wowChild.dwProcessId,wowGeneration,
        wowTask)==ERROR_SUCCESS);
    if (wowStartup) CHECK(WaitForSingleObject(wowStartup,0)==WAIT_OBJECT_0);
    CHECK(WaitForSingleObject(parentEvent,0)==WAIT_TIMEOUT);
    if (wowStartup) CloseHandle(wowStartup);
    wowStartup=NULL;
    { BOOL closeWowWait=FALSE;
      CHECK(OpenNtBaseServiceExit(wowWorker,wowChild.dwProcessId,wowGeneration,TRUE,wowTask,
          &closeWowWait)==ERROR_SUCCESS && !closeWowWait); }
    CHECK(OpenNtBaseServiceWowStartup(launcher,GetCurrentProcessId(),launcherGeneration,
        parentReceipt,&wowStartup,&wowStarted)==ERROR_SUCCESS && wowStarted);
    CHECK(WaitForSingleObject(wowStartup,0)==WAIT_OBJECT_0);
    CHECK(WaitForSingleObject(parentEvent,0)==WAIT_OBJECT_0);
    CloseHandle(wowStartup);wowStartup=NULL;
    CHECK(OpenNtBaseServiceWowStarted(wowWorker,wowChild.dwProcessId,wowGeneration,
        wowTask)==ERROR_INVALID_STATE);
    puts("PASS WOW startup authentication, wait-only notification and immediate-completion latch");
    {
        DWORD oldReceipt=parentReceipt;
        ULONG oldTask=wowTask;
        BOOL closeWowWait=FALSE;
        /* Reuse the original WOWHead with another Check, not a second
         * worker or a fixture-authored task record. */
        CHECK(OpenNtBaseEncodeCheckCommand(&check,11,launcherGeneration,NULL,0,&wireBytes));
        free(wire);wire=malloc(wireBytes);
        CHECK(wire && OpenNtBaseEncodeCheckCommand(&check,11,launcherGeneration,
            wire,wireBytes,&wireBytes));
        CHECK(OpenNtBaseServiceCheck(launcher,GetCurrentProcessId(),launcherGeneration,
            wire,wireBytes,answer,answerBytes,&answerBytes,&parentEvent,&parentReceipt)==ERROR_SUCCESS);
        CHECK(OpenNtBaseApplyCheckReply(answer,answerBytes,launcherGeneration,11,&reply));
        CHECK(reply.ReturnValue==STATUS_SUCCESS && reply.u.CheckVDM.VDMState==VDM_PRESENT_AND_READY);
        wowTask=reply.u.CheckVDM.iTask;
        CHECK(wowTask!=oldTask && parentReceipt!=oldReceipt);
        CHECK(OpenNtBaseServiceWowStartup(launcher,GetCurrentProcessId(),launcherGeneration,
            oldReceipt,&wowStartup,&wowStarted)==ERROR_ACCESS_DENIED && !wowStartup);
        CHECK(OpenNtBaseServiceWowStartup(launcher,GetCurrentProcessId(),launcherGeneration,
            parentReceipt,&wowStartup,&wowStarted)==ERROR_SUCCESS && !wowStarted);
        CHECK(WaitForSingleObject(wowStartup,0)==WAIT_TIMEOUT);
        CHECK(OpenNtBaseServiceGet(wowWorker,wowChild.dwProcessId,wowGeneration,getWire,getWireBytes,
            &getAnswer,&wireBytes,&getWait,standard,&standardCount)==ERROR_SUCCESS);
        CHECK(getAnswer && !getWait && !standardCount);
        OpenNtBaseServiceReleaseCommandReply(getAnswer);getAnswer=NULL;
        CHECK(OpenNtBaseServiceWowStarted(wowWorker,wowChild.dwProcessId,wowGeneration,
            wowTask)==ERROR_SUCCESS);
        CHECK(WaitForSingleObject(wowStartup,0)==WAIT_OBJECT_0);
        CHECK(WaitForSingleObject(parentEvent,0)==WAIT_TIMEOUT);
        CloseHandle(wowStartup);wowStartup=NULL;
        CHECK(OpenNtBaseServiceExit(wowWorker,wowChild.dwProcessId,wowGeneration,TRUE,wowTask,
            &closeWowWait)==ERROR_SUCCESS && !closeWowWait);
        CHECK(WaitForSingleObject(parentEvent,0)==WAIT_OBJECT_0);
        puts("PASS reused WOW gets a fresh receipt/latch; stale startup identity rejected");
    }
    /* Original failed-exec removes a dispatched task without InitTask. Its
     * zero WOW completion must not become a positive startup receipt. */
    { BOOL closeWowWait=FALSE;
    CHECK(OpenNtBaseEncodeCheckCommand(&check,12,launcherGeneration,NULL,0,&wireBytes));
    free(wire);wire=malloc(wireBytes);
    CHECK(wire && OpenNtBaseEncodeCheckCommand(&check,12,launcherGeneration,wire,wireBytes,&wireBytes));
    CHECK(OpenNtBaseServiceCheck(launcher,GetCurrentProcessId(),launcherGeneration,
        wire,wireBytes,answer,answerBytes,&answerBytes,&parentEvent,&parentReceipt)==ERROR_SUCCESS);
    CHECK(OpenNtBaseApplyCheckReply(answer,answerBytes,launcherGeneration,12,&reply));
    CHECK(reply.ReturnValue==STATUS_SUCCESS && reply.u.CheckVDM.VDMState==VDM_PRESENT_AND_READY);
    wowTask=reply.u.CheckVDM.iTask;
    CHECK(OpenNtBaseServiceGet(wowWorker,wowChild.dwProcessId,wowGeneration,getWire,getWireBytes,
        &getAnswer,&wireBytes,&getWait,standard,&standardCount)==ERROR_SUCCESS);
    CHECK(getAnswer && !getWait && !standardCount);
    OpenNtBaseServiceReleaseCommandReply(getAnswer);getAnswer=NULL;
    CHECK(OpenNtBaseServiceExit(wowWorker,wowChild.dwProcessId,wowGeneration,TRUE,wowTask,
        &closeWowWait)==ERROR_SUCCESS && !closeWowWait);
    CHECK(WaitForSingleObject(parentEvent,0)==WAIT_OBJECT_0);
    CHECK(OpenNtBaseServiceWowStartup(launcher,GetCurrentProcessId(),launcherGeneration,
        parentReceipt,&wowStartup,&wowStarted)==ERROR_SUCCESS && !wowStarted);
    CHECK(WaitForSingleObject(wowStartup,0)==WAIT_TIMEOUT);
    CHECK(OpenNtBaseServiceWowStarted(wowWorker,wowChild.dwProcessId,wowGeneration,
        wowTask)==ERROR_INVALID_STATE);
    CloseHandle(wowStartup);wowStartup=NULL;
    puts("PASS failed WOW completion has no startup acknowledgement; late report rejected");
    }
    CHECK(OpenNtBaseServiceDisconnect(wowWorker)==ERROR_SUCCESS);wowWorker=NULL;
    CloseHandle(wowContext);CloseHandle(wowFrontend);wowContext=wowFrontend=NULL;
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
