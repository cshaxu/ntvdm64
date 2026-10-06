#include "task_observation.h"
#include <string.h>

static DWORD WINAPI observation_pump(void *context)
{
    worker_task_observer *state=context;
    HANDLE waits[2]={state->stop,state->wake};
    for(;;) {
        DWORD wait=WaitForMultipleObjects(2,waits,FALSE,INFINITE);
        if(wait!=WAIT_OBJECT_0+1)return wait==WAIT_OBJECT_0 ? ERROR_SUCCESS : GetLastError();
        for(;;) {
            common_dos_observation fact={0};BOOL present=FALSE,gap;
            if(WaitForSingleObject(state->stop,0)==WAIT_OBJECT_0)return ERROR_SUCCESS;
            EnterCriticalSection(&state->lock);
            if(state->count) {
                fact=state->queue[state->head];
                state->head=(state->head+1)%WORKER_OBSERVATION_CAPACITY;
                --state->count;present=TRUE;
            }
            gap=InterlockedExchange(&state->lost,0)!=0;
            LeaveCriticalSection(&state->lock);
            if(!present && !gap)break;
            if(!present)fact.event=DOS_OBSERVATION_GAP;
            if(state->publish(state->context,&fact,gap))
                InterlockedExchange(&state->lost,1); /* No unbounded report retry. */
            EnterCriticalSection(&state->lock);
            if(!state->count)SetEvent(state->drained);
            LeaveCriticalSection(&state->lock);
            /* On failure with an empty queue, retain loss for the next fact;
             * do not repeatedly send GAP or spin on an unavailable service. */
            if(!present)break;
        }
    }
}

DWORD worker_task_observer_start(worker_task_observer *state,
    worker_observation_publish publish,worker_observation_cancel cancel,void *context)
{
    DWORD error;
    if(!state || !publish || !cancel)return ERROR_INVALID_PARAMETER;
    memset(state,0,sizeof(*state));
    if(!InitializeCriticalSectionEx(&state->lock,0,0))return GetLastError();
    state->publish=publish;state->cancel=cancel;state->context=context;
    state->wake=CreateEventW(NULL,FALSE,FALSE,NULL);
    state->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    state->drained=CreateEventW(NULL,TRUE,TRUE,NULL);
    if(!state->wake || !state->stop || !state->drained)goto fail;
    state->thread=CreateThread(NULL,0,observation_pump,state,0,NULL);
    if(!state->thread)goto fail;
    return ERROR_SUCCESS;
fail:
    error=GetLastError();
    if(state->wake)CloseHandle(state->wake);
    if(state->stop)CloseHandle(state->stop);
    if(state->drained)CloseHandle(state->drained);
    DeleteCriticalSection(&state->lock);memset(state,0,sizeof(*state));return error;
}

BOOL worker_task_observer_offer(worker_task_observer *state,const common_dos_observation *fact)
{
    BOOL accepted=FALSE;
    if(!state || !state->thread || !fact)return FALSE;
    if(TryEnterCriticalSection(&state->lock)) {
        if(state->count<WORKER_OBSERVATION_CAPACITY) {
            ResetEvent(state->drained);
            state->queue[(state->head+state->count)%WORKER_OBSERVATION_CAPACITY]=*fact;
            ++state->count;accepted=TRUE;
        }
        LeaveCriticalSection(&state->lock);
    }
    if(!accepted)InterlockedExchange(&state->lost,1);
    SetEvent(state->wake);return accepted;
}

DWORD worker_task_observer_drain(worker_task_observer *state,DWORD timeout)
{
    DWORD wait;
    if(!state || !state->thread || timeout==INFINITE)return ERROR_INVALID_PARAMETER;
    wait=WaitForSingleObject(state->drained,timeout);
    return wait==WAIT_OBJECT_0 ? ERROR_SUCCESS : wait==WAIT_TIMEOUT ? ERROR_TIMEOUT : GetLastError();
}

void worker_task_observer_stop(worker_task_observer *state)
{
    if(!state || !state->thread)return;
    SetEvent(state->stop);state->cancel(state->context);
    if(WaitForSingleObject(state->thread,INFINITE)!=WAIT_OBJECT_0)
        RaiseFailFastException(NULL,NULL,0); /* Never free a running publisher. */
    CloseHandle(state->thread);CloseHandle(state->wake);CloseHandle(state->stop);
    CloseHandle(state->drained);
    DeleteCriticalSection(&state->lock);memset(state,0,sizeof(*state));
}
