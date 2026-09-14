#include <base_service.h>
#include <stdio.h>
#include <string.h>

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
    if (argc==2 && !strcmp(argv[1],"--reservation-child")) { Sleep(15000);return 0; }
    self=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,GetCurrentProcessId());
    service=OpenNtBaseServiceStart();
    CHECK(self && service!=NULL);
    CHECK(OpenNtBaseServiceConnect(service,self,&launcher,&launcherGeneration)==ERROR_SUCCESS);
    CHECK(OpenNtBaseServiceCreateReservation(launcher,GetCurrentProcessId(),launcherGeneration,
        73,(HANDLE)0x1444,&reservation)==ERROR_SUCCESS);
    CHECK(GetModuleFileNameA(NULL,command,MAX_PATH));
    strcat_s(command,sizeof(command)," --reservation-child");
    CHECK(CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&startup,&child));
    CHECK(OpenNtBaseServicePrepareWorker(launcher,GetCurrentProcessId(),launcherGeneration,
        reservation,child.hProcess)==ERROR_SUCCESS);
    CHECK(ResumeThread(child.hThread)!=(DWORD)-1);
    CHECK(OpenNtBaseServiceConnect(service,child.hProcess,&worker,&workerGeneration)==ERROR_SUCCESS);
    CHECK(OpenNtBaseServiceWorkerReservation(worker,&claimed,&task,&console));
    CHECK(claimed==reservation && task==73 && console==(HANDLE)0x1444);
    CHECK(OpenNtBaseServiceDisconnect(worker)==ERROR_SUCCESS);worker=NULL;
    CHECK(OpenNtBaseServicePrepareWorker(launcher,GetCurrentProcessId(),launcherGeneration,
        reservation,child.hProcess)==ERROR_ALREADY_EXISTS);
    CHECK(OpenNtBaseServiceReleaseReservation(launcher,GetCurrentProcessId(),launcherGeneration,reservation)==ERROR_SUCCESS);
    CHECK(OpenNtBaseServiceDisconnect(launcher)==ERROR_SUCCESS);launcher=NULL;
    CHECK(OpenNtBaseServiceStop(service));service=NULL;
    TerminateProcess(child.hProcess,0);WaitForSingleObject(child.hProcess,INFINITE);
    CloseHandle(child.hThread);CloseHandle(child.hProcess);CloseHandle(self);
    puts("PASS: service connection claims only launcher-prepared worker reservation");
    return 0;
}
