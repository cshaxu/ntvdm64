#include "worker_performance.h"
#include "common/protocol/console_mouse.h"
#include <stdio.h>
#include <string.h>
/* Measurement/queue unit only; real CCPU guest evidence is a separate runner. */
int main(int argc,char **argv)
{
    mvdm_mouse_input queue={0};mvdm_mouse_input_sample input={1,0,0,CONSOLE_MOUSE_MOVE},output;
    DWORD index,before,head,burst,bursts=1;int accepted;long total=0;
    BOOL enabled,cost;LARGE_INTEGER start,end,frequency;
    if(argc!=3)return 64;
    cost=!strcmp(argv[1],"cost-enabled") || !strcmp(argv[1],"cost-disabled");
    if(cost)bursts=32; /* 6944 measurements: below the fixed 8192 limit. */
    if(!strcmp(argv[1],"disabled") || !strcmp(argv[1],"cost-disabled"))SetEnvironmentVariableA("MVDM_TEST_WORKER_PERFORMANCE",NULL);
    else SetEnvironmentVariableA("MVDM_TEST_WORKER_PERFORMANCE",argv[2]);
    SetLastError(0x53510002);enabled=worker_performance_enabled();
    if(GetLastError()!=0x53510002)return 65;
    QueryPerformanceFrequency(&frequency);QueryPerformanceCounter(&start);
    if(!strcmp(argv[1],"overflow")){
        for(index=0;index<8193;++index)worker_performance_record("unit-overflow",worker_performance_clock(),0,0);
    }else{
        for(burst=0;burst<bursts;++burst){
            for(index=0;index<200;++index){
                LONGLONG tick;
                /* Cost cases include the test wrapper's actual disabled
                 * InitOnce/branch path, not only a cached enabled boolean. */
                if(cost)enabled=worker_performance_enabled();
                tick=enabled ? worker_performance_clock() : 0;before=queue.count;
                accepted=mvdm_mouse_input_push(&queue,&input);if(!accepted)return 66;
                if(enabled)worker_performance_push(&queue,before,accepted,tick);
                if(GetLastError()!=0x53510002)return 67;
            }
            if(queue.count!=17)return 68;
            while(queue.count){
                if(cost)enabled=worker_performance_enabled();
                before=queue.count;head=queue.head;
                if(!mvdm_mouse_input_take(&queue,&output))return 69;
                total+=output.dx;
                if(enabled)worker_performance_take(&queue,head,before,1,worker_performance_clock());
                if(GetLastError()!=0x53510002)return 70;
            }
        }
        if(total!=(long)(200*bursts))return 71;
    }
    QueryPerformanceCounter(&end);
    if(!strcmp(argv[1],"total"))
        worker_performance_record_total("unit-total",frequency.QuadPart/1000,17,0);
    worker_performance_flush();
    if(GetLastError()!=0x53510002)return 72;
    printf("PASS measurement unit mode=%s pid=%lu displacement=%ld\n",argv[1],GetCurrentProcessId(),total);
    if(cost)printf("queue-cost bursts=%lu operations=%lu elapsed-ns=%lld\n",bursts,bursts*217,(end.QuadPart-start.QuadPart)*1000000000/frequency.QuadPart);
    return 0;
}
