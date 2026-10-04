#include "worker_performance.h"
#include "common/protocol/console_mouse.h"
#include <stdio.h>
#include <string.h>
/* Measurement/queue unit only; real CCPU guest evidence is a separate runner. */
int main(int argc,char **argv)
{
    mvdm_mouse_input queue={0};mvdm_mouse_input_sample input={1,0,0,CONSOLE_MOUSE_MOVE},output;
    DWORD index,before,head;int accepted;long total=0;
    BOOL enabled;
    if(argc!=3)return 64;
    if(!strcmp(argv[1],"disabled"))SetEnvironmentVariableA("MVDM_TEST_WORKER_PERFORMANCE",NULL);
    else SetEnvironmentVariableA("MVDM_TEST_WORKER_PERFORMANCE",argv[2]);
    SetLastError(0x53510002);enabled=worker_performance_enabled();
    if(GetLastError()!=0x53510002)return 65;
    if(!strcmp(argv[1],"overflow")){
        for(index=0;index<8193;++index)worker_performance_record("unit-overflow",worker_performance_clock(),0,0);
    }else{
        for(index=0;index<200;++index){
            LONGLONG start=worker_performance_clock();before=queue.count;
            accepted=mvdm_mouse_input_push(&queue,&input);if(!accepted)return 66;
            if(enabled)worker_performance_push(&queue,before,accepted,start);
            if(GetLastError()!=0x53510002)return 67;
        }
        if(queue.count!=17)return 68;
        while(queue.count){
            before=queue.count;head=queue.head;
            if(!mvdm_mouse_input_take(&queue,&output))return 69;
            total+=output.dx;
            if(enabled)worker_performance_take(&queue,head,before,1,worker_performance_clock());
            if(GetLastError()!=0x53510002)return 70;
        }
        if(total!=200)return 71;
    }
    worker_performance_flush();
    if(GetLastError()!=0x53510002)return 72;
    printf("PASS measurement unit mode=%s pid=%lu displacement=%ld\n",argv[1],GetCurrentProcessId(),total);
    return 0;
}
