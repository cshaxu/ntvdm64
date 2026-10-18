#include "wow_graph_cost_instrument.h"
#include <stdio.h>
static DWORD WINAPI overlap(void *unused)
{
    LONGLONG start;
    (void)unused;
    start=graph_cost_begin();graph_cost_end(5,start,1);return 0;
}
int main(void)
{
    LONGLONG outer,inner;
    HANDLE thread;
    DWORD code;
    SetLastError(447);
    outer=graph_cost_begin();inner=graph_cost_begin();
    graph_cost_end(0,inner,3);graph_cost_end(2,outer,7);
    if(GetLastError()!=447 || T447ProfileData.sequence!=2 ||
        T447ProfileData.rows[0].calls!=1 || T447ProfileData.rows[2].items!=7 ||
        T447ProfileData.rows[0].ticks>T447ProfileData.rows[2].ticks ||
        graph_cost_owner || graph_cost_depth || T447ProfileData.reserved)return 1;
    __try {
        outer=graph_cost_begin();
        __try {RaiseException(0xe0447001,0,0,NULL);}
        __finally {graph_cost_end(1,outer,1);}
    } __except(GetExceptionCode()==0xe0447001?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH) {}
    if(T447ProfileData.sequence!=4 || graph_cost_owner || graph_cost_depth ||
        T447ProfileData.rows[1].calls!=1)return 2;
    outer=graph_cost_begin();
    thread=CreateThread(NULL,0,overlap,NULL,0,NULL);
    if(!thread || WaitForSingleObject(thread,5000)!=WAIT_OBJECT_0 ||
        !GetExitCodeThread(thread,&code) || code)return 3;
    CloseHandle(thread);graph_cost_end(3,outer,1);
    if(T447ProfileData.reserved!=1 || T447ProfileData.rows[5].calls ||
        T447ProfileData.sequence!=6 || graph_cost_owner || graph_cost_depth)return 4;
    puts("GRAPH_COST_CONTROL nested=1 last_error=1 exception=1 concurrent_refusal=1");
    return 0;
}
