#include "worker-base/task_observation.h"
#include <stdio.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"FAIL %u: %s\n",__LINE__,#x);return 1;}}while(0)
typedef struct fixture {
    HANDLE entered,release,done;
    DWORD producer,consumer;
    LONG count,gaps,cancelled;
    uint64_t seen[WORKER_OBSERVATION_CAPACITY+1];
} fixture;
static DWORD publish(void *context,const common_dos_observation *fact,BOOL gap)
{
    fixture *state=context;LONG index=state->count;
    state->consumer=GetCurrentThreadId();
    if(!index){SetEvent(state->entered);if(WaitForSingleObject(state->release,5000)!=WAIT_OBJECT_0)return ERROR_TIMEOUT;}
    if(gap)InterlockedIncrement(&state->gaps);
    if(index>=WORKER_OBSERVATION_CAPACITY+1)return ERROR_INVALID_DATA;
    state->seen[index]=fact->occurrence;
    if(InterlockedIncrement(&state->count)==WORKER_OBSERVATION_CAPACITY+1)SetEvent(state->done);
    return ERROR_SUCCESS;
}
static void cancel(void *context)
{
    fixture *state=context;
    InterlockedIncrement(&state->cancelled);SetEvent(state->release);
}
int main(void)
{
    worker_task_observer observer;fixture state={0};common_dos_observation fact={0};DWORD index;
    state.producer=GetCurrentThreadId();
    state.entered=CreateEventW(NULL,TRUE,FALSE,NULL);
    state.release=CreateEventW(NULL,TRUE,FALSE,NULL);
    state.done=CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(state.entered && state.release && state.done);
    CHECK(worker_task_observer_start(NULL,publish,cancel,&state)==ERROR_INVALID_PARAMETER);
    CHECK(!worker_task_observer_start(&observer,publish,cancel,&state));
    fact.event=DOS_OBSERVATION_ENTER;fact.occurrence=1;
    CHECK(worker_task_observer_offer(&observer,&fact));
    CHECK(WaitForSingleObject(state.entered,5000)==WAIT_OBJECT_0);
    CHECK(worker_task_observer_drain(&observer,0)==ERROR_TIMEOUT);
    /* The consumer is held outside the queue lock. Filling/overflow never
     * waits for transport and preserves all accepted distinct events. */
    for(index=0;index<WORKER_OBSERVATION_CAPACITY;++index){fact.occurrence=index+2;CHECK(worker_task_observer_offer(&observer,&fact));}
    ++fact.occurrence;CHECK(!worker_task_observer_offer(&observer,&fact));
    CHECK(SetEvent(state.release));CHECK(WaitForSingleObject(state.done,5000)==WAIT_OBJECT_0);
    CHECK(!worker_task_observer_drain(&observer,5000));
    worker_task_observer_stop(&observer);
    CHECK(state.count==WORKER_OBSERVATION_CAPACITY+1 && state.gaps==1);
    CHECK(state.consumer!=state.producer && state.cancelled==1 && !observer.thread);
    for(index=0;index<WORKER_OBSERVATION_CAPACITY+1;++index)CHECK(state.seen[index]==index+1);
    worker_task_observer_stop(&observer);CHECK(state.cancelled==1);
    /* Pending publication cancellation joins before context/handles die. */
    state.count=state.gaps=state.cancelled=0;ResetEvent(state.entered);ResetEvent(state.release);
    CHECK(!worker_task_observer_start(&observer,publish,cancel,&state));
    CHECK(worker_task_observer_offer(&observer,&fact));
    CHECK(WaitForSingleObject(state.entered,5000)==WAIT_OBJECT_0);
    worker_task_observer_stop(&observer);
    CHECK(state.cancelled==1 && state.count==1 && !observer.thread);
    CloseHandle(state.entered);CloseHandle(state.release);CloseHandle(state.done);
    puts("PASS copied ordered outbox, explicit overflow, off-producer publication and cancellation/join");
    return 0;
}
