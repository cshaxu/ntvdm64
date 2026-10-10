#include "worker_performance.h"
#include <stdio.h>
#include <stdlib.h>
#define TEST_FALLBACK_PREFIX "O:\\tmp\\ntvdm64-worker-performance\\trace"
#define TEST_FALLBACK_SNAPSHOT_MS 5000
#define SAMPLE_LIMIT 8192
typedef struct sample { const char *phase;LONGLONG tick,duration;DWORD count,error; } sample;
static INIT_ONCE once=INIT_ONCE_STATIC_INIT;
static SRWLOCK lock=SRWLOCK_INIT;
static BOOL enabled;
static char prefix[MAX_PATH];
static LONGLONG frequency,origin,queued[MVDM_MOUSE_INPUT_CAPACITY];
static LONGLONG snapshot_deadline;
static volatile LONG snapshot_written;
static sample samples[SAMPLE_LIMIT];
static DWORD used,overflow,high_water;
static mvdm_mouse_input *observed_queue;
typedef struct aggregate {
    volatile LONGLONG calls,ticks,count,errors;
} aggregate;
static aggregate graphics_invalidations,graphics_signals,graphics_copies;
static BOOL CALLBACK initialize(PINIT_ONCE init,PVOID context,PVOID *result)
{
    LARGE_INTEGER value;DWORD length;
    (void)init;(void)context;(void)result;
    length=GetEnvironmentVariableA("MVDM_TEST_WORKER_PERFORMANCE",prefix,MAX_PATH);
    enabled=length && length<MAX_PATH;
    /* This source is linked only into the diagnostic worker.  The explicit
     * sentinel makes a manual desktop measurement independent of whether the
     * broker's VDM environment contains arbitrary host test variables. */
    if(!enabled) {
        char sentinel[MAX_PATH];
        if(sprintf_s(sentinel,sizeof(sentinel),"%s.enable",
            TEST_FALLBACK_PREFIX)>=0 &&
           GetFileAttributesA(sentinel)!=INVALID_FILE_ATTRIBUTES) {
            strcpy_s(prefix,ARRAYSIZE(prefix),TEST_FALLBACK_PREFIX);
            enabled=TRUE;
        }
    }
    QueryPerformanceFrequency(&value);frequency=value.QuadPart;
    QueryPerformanceCounter(&value);origin=value.QuadPart;
    {
        char milliseconds[16];
        DWORD count=GetEnvironmentVariableA("MVDM_TEST_WORKER_PERFORMANCE_SNAPSHOT_MS",
            milliseconds,ARRAYSIZE(milliseconds));
        if(count && count<ARRAYSIZE(milliseconds)) {
            char *end=NULL;
            unsigned long parsed=strtoul(milliseconds,&end,10);
            if(end && !*end && parsed)
                snapshot_deadline=origin+((LONGLONG)parsed*frequency)/1000;
        }
    }
    if(enabled && !snapshot_deadline)
    {
        char deadline_path[MAX_PATH],deadline_text[16]={0};
        DWORD read=0;
        HANDLE deadline;
        unsigned long milliseconds=TEST_FALLBACK_SNAPSHOT_MS;
        if(sprintf_s(deadline_path,sizeof(deadline_path),"%s.snapshot-ms",prefix)>=0 &&
           (deadline=CreateFileA(deadline_path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,
                                 NULL,OPEN_EXISTING,0,NULL))!=INVALID_HANDLE_VALUE) {
            if(ReadFile(deadline,deadline_text,sizeof(deadline_text)-1,&read,NULL)) {
                char *end=NULL;
                unsigned long parsed;
                deadline_text[read]=0;
                parsed=strtoul(deadline_text,&end,10);
                if(end && !*end && parsed)milliseconds=parsed;
            }
            CloseHandle(deadline);
        }
        snapshot_deadline=origin+((LONGLONG)milliseconds*frequency)/1000;
    }
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
static void maybe_snapshot(void);
void worker_performance_record(const char *phase,LONGLONG start,DWORD count,DWORD error)
{
    DWORD saved=GetLastError();LONGLONG end=worker_performance_clock();
    AcquireSRWLockExclusive(&lock);append(phase,start,end,count,error);ReleaseSRWLockExclusive(&lock);
    maybe_snapshot();
    SetLastError(saved);
}
void worker_performance_record_total(const char *phase,LONGLONG duration,DWORD count,DWORD error)
{
    DWORD saved=GetLastError();LONGLONG end=worker_performance_clock();
    AcquireSRWLockExclusive(&lock);append(phase,end-duration,end,count,error);ReleaseSRWLockExclusive(&lock);
    SetLastError(saved);
}
static void aggregate_record(aggregate *value,LONGLONG start,DWORD count,DWORD error)
{
    LONGLONG end=worker_performance_clock();
    InterlockedIncrement64(&value->calls);
    InterlockedAdd64(&value->ticks,end-start);
    InterlockedAdd64(&value->count,count);
    if(error)InterlockedIncrement64(&value->errors);
}
static void maybe_snapshot(void)
{
    if(snapshot_deadline && worker_performance_clock()>=snapshot_deadline &&
       InterlockedCompareExchange(&snapshot_written,1,0)==0)
        worker_performance_flush();
}
void worker_performance_graphics_invalidate(LONGLONG start,DWORD area,DWORD error)
{
    aggregate_record(&graphics_invalidations,start,area,error);
    /* Test-only: a passive desktop trace normally flushes only when its
       worker exits.  Permit exactly one optional cumulative snapshot so a
       bounded unattended sample does not need to alter the guest UI to exit. */
    maybe_snapshot();
}
void worker_performance_graphics_signal(LONGLONG start,DWORD error)
{
    aggregate_record(&graphics_signals,start,0,error);
    /* A Window-mode source may complete host flushes without a later dirty
     * rectangle.  Keep the observer's one bounded snapshot tied to that real
     * source event rather than requiring synthetic mouse input. */
    maybe_snapshot();
}
void worker_performance_graphics_copy(LONGLONG start,DWORD bytes,DWORD error)
{ aggregate_record(&graphics_copies,start,bytes,error); }
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
    maybe_snapshot();
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
    char path[MAX_PATH];FILE *file=NULL;DWORD index,saved=GetLastError();LONGLONG end;
    if(!worker_performance_enabled())return;
    if(sprintf_s(path,sizeof(path),"%s-%lu.txt",prefix,GetCurrentProcessId())<0)return;
    /* Flush only at the existing quiesced handoff/teardown boundary. The
     * measurement lock prevents a racing diagnostic snapshot, not scheduling. */
    AcquireSRWLockExclusive(&lock);
    if(!fopen_s(&file,path,"w") && file){
        end=worker_performance_clock();
        fprintf(file,"samples=%lu overflow=%lu queue-high-water=%lu frequency=%lld elapsed-us=%lld\n",used,overflow,high_water,frequency,(end-origin)*1000000/frequency);
        for(index=0;index<used;++index){const sample *s=&samples[index];
            fprintf(file,"phase=%s elapsed-us=%lld duration-us=%lld count=%lu error=%lu\n",
                s->phase,(s->tick-origin)*1000000/frequency,s->duration*1000000/frequency,s->count,s->error);
        }
        {
            const struct {const char *phase;aggregate *value;} values[]={
                {"graphics-invalidate",&graphics_invalidations},
                {"graphics-publisher-signal",&graphics_signals},
                {"graphics-dib-copy",&graphics_copies}};
            for(index=0;index<ARRAYSIZE(values);++index) {
                aggregate *value=values[index].value;
                fprintf(file,"aggregate phase=%s calls=%lld duration-us=%lld count=%lld errors=%lld\n",
                    values[index].phase,InterlockedCompareExchange64(&value->calls,0,0),
                    InterlockedCompareExchange64(&value->ticks,0,0)*1000000/frequency,
                    InterlockedCompareExchange64(&value->count,0,0),
                    InterlockedCompareExchange64(&value->errors,0,0));
            }
        }
        fclose(file);
    }
    ReleaseSRWLockExclusive(&lock);SetLastError(saved);
}
