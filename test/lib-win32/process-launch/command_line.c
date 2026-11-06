#include "lib-win32/process-launch/command_line.h"
#include <stdio.h>
#include <wchar.h>
int main(void)
{
    static const struct {const WCHAR *input,*tail;} cases[]={
        {L"app.exe arg  two",L"arg  two"},
        {L"  \"C:\\Program Files\\app.exe\"  /x \"a b\"",L"/x \"a b\""},
        {L"app\t\tsecond",L"second"},
        {L"\"a\\\" b\" tail",L"tail"},
        {L"cmd /c ver",L"/c ver"},
        {L"cmd /c echo hi > out",L"/c echo hi > out"},
        {L"cmd /c echo hi >> out",L"/c echo hi >> out"},
        {L"cmd /c echo hi | more",L"/c echo hi | more"},
        {L"single",L""},
        {L"   ",L""}
    };
    for(unsigned i=0;i<sizeof(cases)/sizeof(cases[0]);++i)
        if(wcscmp(win32_process_next_argument(cases[i].input),cases[i].tail)){
            fprintf(stderr,"PROCESS_LAUNCH_FAIL case=%u\n",i);return 1;
        }
    puts("PROCESS_LAUNCH_PASS");return 0;
}
