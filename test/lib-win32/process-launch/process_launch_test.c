#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include "lib-win32/process-launch/command_line.h"
#include "lib-win32/process-launch/process.h"
#define CHECK(x) do {if(!(x)){fprintf(stderr,"process launch line %d\n",__LINE__);return 1;}}while(0)
int main(void)
{
    static const struct {const WCHAR *input,*tail;} cases[]={
        {L"app.exe arg  two",L"arg  two"},
        {L"  \"C:\\Program Files\\app.exe\"  /x \"a b\"",L"/x \"a b\""},
        {L"\"a\\\" b\" tail",L"tail"},
        {L"cmd /c ver",L"/c ver"},
        {L"cmd /c echo hi > out",L"/c echo hi > out"},
        {L"cmd /c echo hi >> out",L"/c echo hi >> out"},
        {L"cmd /c echo hi | more",L"/c echo hi | more"},
        {L"single",L""}, {L"   ",L""}
    };
    for(unsigned i=0;i<sizeof(cases)/sizeof(cases[0]);++i)
        CHECK(!wcscmp(win32_process_next_argument(cases[i].input),cases[i].tail));
    {
        STARTUPINFOW startup={0}; PROCESS_INFORMATION child={0}; DWORD code=0;
        WCHAR command[]=L"cmd.exe /c exit 37";
        startup.cb=sizeof(startup);
        CHECK(win32_process_create(NULL,command,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,
            NULL,NULL,&startup,&child));
        CHECK(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT);
        CHECK(ResumeThread(child.hThread)==1);
        CHECK(WaitForSingleObject(child.hProcess,INFINITE)==WAIT_OBJECT_0);
        CHECK(GetExitCodeProcess(child.hProcess,&code) && code==37);
        CloseHandle(child.hThread);CloseHandle(child.hProcess);
    }
    printf("PASS suspended process launch and raw command line: %u cases\n",(unsigned)(sizeof(cases)/sizeof(cases[0])));
    return 0;
}
