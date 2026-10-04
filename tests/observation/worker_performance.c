#include "worker_performance.h"
#include <stdio.h>
#define SAMPLE_LIMIT 8192
typedef struct sample { const char *phase;LONGLONG tick,duration;DWORD count,error; } sample;
static INIT_ONCE once=INIT_ONCE_STATIC_INIT;
static SRWLOCK lock=SRWLOCK_INIT;
static BOOL enabled;
static char prefix[MAX_PATH];
static LONGLONG frequency,origin,queued[MVDM_MOUSE_INPUT_CAPACITY];
static sample samples[SAMPLE_LIMIT];
static DWORD used,overflow,high_water;
static mvdm_mouse_input *observed_queue;
static BOOL CALLBACK initialize(PINIT_ONCE init,PVOID context,PVOID *result)
{
    LARGE_INTEGER value;DWORD length;
    (void)init;(void)context;(void)result;
    length=GetEnvironmentVariableA("MVDM_TEST_WORKER_PERFORMANCE",prefix,MAX_PATH);
    enabled=length && length<MAX_PATH;
    QueryPerformanceFrequency(&value);frequency=value.QuadPart;
    QueryPerformanceCounter(&value);origin=value.QuadPart;
    return TRUE;
}
BOOL worker_performance_enabled(void)
{
    DWORD saved=GetLastError();
    InitOnceExecuteOnce(&once,initialize,NULL,NULL);
    SetLastError(saved);return enabled;
}
LONGLONG worker_performance_clock(void)
{ LARGE_INTEGER value;QueryPerformanceCounter(&value);return value.QuadPart; }
static void append(const char *phase,LONGLONG start,LONGLONG end,DWORD count,DWORD error)
{
    if(used==SAMPLE_LIMIT){++overflow;return;}
    samples[used++]=(sample){phase,end,end-start,count,error};
}
void worker_performance_record(const char *phase,LONGLONG start,DWORD count,DWORD error)
{
    DWORD saved=GetLastError();LONGLONG end=worker_performance_clock();
    AcquireSRWLockExclusive(&lock);append(phase,start,end,count,error);ReleaseSRWLockExclusive(&lock);
    SetLastError(saved);
}
void worker_performance_record_total(const char *phase,LONGLONG duration,DWORD count,DWORD error)
{
    DWORD saved=GetLastError();LONGLONG end=worker_performance_clock();
    AcquireSRWLockExclusive(&lock);append(phase,end-duration,end,count,error);ReleaseSRWLockExclusive(&lock);
    SetLastError(saved);
}
void worker_performance_push(mvdm_mouse_input *queue,DWORD before,int accepted,LONGLONG start)
{
    DWORD saved=GetLastError();LONGLONG end=worker_performance_clock();DWORD slot;
    AcquireSRWLockExclusive(&lock);
    if(observed_queue && observed_queue!=queue){++overflow;}
    observed_queue=queue;
    if(accepted && queue->count>before){
        slot=(queue->head+queue->count-1)%MVDM_MOUSE_INPUT_CAPACITY;queued[slot]=start;
    }
    if(queue->count>high_water)high_water=queue->count;
    append(!accepted ? "mouse-rejected" : queue->count==before ? "mouse-merged" : "mouse-enqueued",
        start,end,queue->count,accepted ? 0 : ERROR_BUFFER_OVERFLOW);
    ReleaseSRWLockExclusive(&lock);
    SetLastError(saved);
}
void worker_performance_take(mvdm_mouse_input *queue,DWORD head,DWORD before,int taken,LONGLONG end)
{
    DWORD saved=GetLastError();
    AcquireSRWLockExclusive(&lock);
    if(taken && queued[head]){
        append(queue->count==before ? "mouse-partial-consumption" : "mouse-irq-consumption",
            queued[head],end,queue->count,0);
        if(queue->count<before)queued[head]=0;
    } else if(taken){append("mouse-unmatched-consumption",end,end,queue->count,ERROR_INVALID_DATA);}
    ReleaseSRWLockExclusive(&lock);
    SetLastError(saved);
}
void worker_performance_flush(void)
{
    char path[MAX_PATH];FILE *file=NULL;DWORD index,saved=GetLastError();
    if(!worker_performance_enabled())return;
    if(sprintf_s(path,sizeof(path),"%s-%lu.txt",prefix,GetCurrentProcessId())<0)return;
    /* Flush only at the existing quiesced handoff/teardown boundary. The
     * measurement lock prevents a racing diagnostic snapshot, not scheduling. */
    AcquireSRWLockExclusive(&lock);
    if(!fopen_s(&file,path,"w") && file){
        fprintf(file,"samples=%lu overflow=%lu queue-high-water=%lu frequency=%lld\n",used,overflow,high_water,frequency);
        for(index=0;index<used;++index){const sample *s=&samples[index];
            fprintf(file,"phase=%s elapsed-us=%lld duration-us=%lld count=%lu error=%lu\n",
                s->phase,(s->tick-origin)*1000000/frequency,s->duration*1000000/frequency,s->count,s->error);
        }
        fclose(file);
    }
    ReleaseSRWLockExclusive(&lock);SetLastError(saved);
}
