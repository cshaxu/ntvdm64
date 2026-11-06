#include "lib-win32/process-launch/process.h"
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"PROCESS_CREATE_FAIL line=%d error=%lu\n",__LINE__,GetLastError()); result=1; goto cleanup; } } while (0)
int main(int argc, char **argv)
{
    WCHAR self[MAX_PATH], command[MAX_PATH+16];
    STARTUPINFOW startup={0}; PROCESS_INFORMATION child={0};
    DWORD code=0; int result=0;
    if(argc==2 && !strcmp(argv[1],"child"))return 37;
    startup.cb=sizeof(startup);
    CHECK(GetModuleFileNameW(NULL,self,MAX_PATH));
    CHECK(swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" child",self)>0);
    CHECK(win32_process_create(self,command,NULL,NULL,FALSE,
        CREATE_SUSPENDED|CREATE_NO_WINDOW,NULL,NULL,&startup,&child));
    CHECK(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT);
    CHECK(ResumeThread(child.hThread)==1);
    CHECK(WaitForSingleObject(child.hProcess,10000)==WAIT_OBJECT_0);
    CHECK(GetExitCodeProcess(child.hProcess,&code) && code==37);
    CloseHandle(child.hThread);CloseHandle(child.hProcess);
    ZeroMemory(&child,sizeof(child));
    CHECK(!win32_process_create(L"?:\\missing\\program.exe",NULL,NULL,NULL,FALSE,
        CREATE_NO_WINDOW,NULL,NULL,&startup,&child));
    CHECK(GetLastError()!=ERROR_SUCCESS && !child.hProcess && !child.hThread);
cleanup:
    if(child.hProcess){
        if(WaitForSingleObject(child.hProcess,0)!=WAIT_OBJECT_0){
            TerminateProcess(child.hProcess,1);WaitForSingleObject(child.hProcess,INFINITE);
        }
        CloseHandle(child.hProcess);
    }
    if(child.hThread)CloseHandle(child.hThread);
    if(!result)puts("PROCESS_CREATE_PASS suspended=1 exit=37 failure=1");
    return result;
}
