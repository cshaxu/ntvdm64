#include "input_watch.h"

struct worker_base_input_watch {
    CRITICAL_SECTION lock;
    HANDLE shutdown,stop,local_stop,rearm,wake,source,thread;
    worker_base_input_ready_fn ready;
    worker_base_input_shutdown_fn close;
    worker_base_input_failure_fn failed;
    void *context;
    DWORD error;
};

static DWORD fail_watch(worker_base_input_watch *watch,DWORD error)
{
    EnterCriticalSection(&watch->lock);watch->error=error;
    SetEvent(watch->wake);LeaveCriticalSection(&watch->lock);
    if(watch->failed)watch->failed(watch->context,error);
    return error;
}
static DWORD WINAPI input_thread(void *context)
{
    worker_base_input_watch *watch=context;
    BOOL pending=FALSE;
    for(;;) {
        HANDLE source=NULL;
        HANDLE waits[5]={watch->shutdown,watch->stop,watch->local_stop,watch->rearm,NULL};
        DWORD count=4,result,error=ERROR_SUCCESS;
        EnterCriticalSection(&watch->lock);
        if(!pending && watch->source) {
            if(!DuplicateHandle(GetCurrentProcess(),watch->source,GetCurrentProcess(),
                &source,SYNCHRONIZE,FALSE,0))error=GetLastError();
            else waits[count++]=source;
        }
        LeaveCriticalSection(&watch->lock);
        if(error)return fail_watch(watch,error);
        result=WaitForMultipleObjects(count,waits,FALSE,INFINITE);
        if(result==WAIT_FAILED)error=GetLastError();
        if(source)CloseHandle(source);
        /* Recheck priority before calling an owner: readiness is not permission
         * to overtake broker close or local cancellation. */
        if(WaitForSingleObject(watch->shutdown,0)==WAIT_OBJECT_0) {
            if(watch->close)watch->close(watch->context);
            return ERROR_PROCESS_ABORTED;
        }
        if(WaitForSingleObject(watch->stop,0)==WAIT_OBJECT_0 ||
            WaitForSingleObject(watch->local_stop,0)==WAIT_OBJECT_0)return ERROR_SUCCESS;
        if(result==WAIT_OBJECT_0+3){pending=FALSE;continue;}
        if(result!=WAIT_OBJECT_0+4)
            return fail_watch(watch,result==WAIT_FAILED ? error : ERROR_PIPE_NOT_CONNECTED);
        if(!SetEvent(watch->wake))return fail_watch(watch,GetLastError());
        pending=TRUE;
        if(watch->ready) {
            error=watch->ready(watch->context);
            if(error)return fail_watch(watch,error);
        }
    }
}
worker_base_input_watch *worker_base_input_watch_create(HANDLE shutdown,HANDLE stop,
    worker_base_input_ready_fn ready,worker_base_input_shutdown_fn close,
    worker_base_input_failure_fn failed,void *context)
{
    worker_base_input_watch *watch;DWORD error;
    if(!shutdown || !stop){SetLastError(ERROR_INVALID_PARAMETER);return NULL;}
    watch=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*watch));
    if(!watch){SetLastError(ERROR_NOT_ENOUGH_MEMORY);return NULL;}
    InitializeCriticalSection(&watch->lock);
    watch->shutdown=shutdown;watch->stop=stop;watch->ready=ready;
    watch->close=close;watch->failed=failed;watch->context=context;
    watch->local_stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    watch->rearm=CreateEventW(NULL,FALSE,FALSE,NULL);
    watch->wake=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!watch->local_stop || !watch->rearm || !watch->wake)goto failed;
    watch->thread=CreateThread(NULL,0,input_thread,watch,0,NULL);
    if(!watch->thread)goto failed;
    return watch;
failed:
    error=GetLastError();worker_base_input_watch_destroy(watch);
    SetLastError(error);return NULL;
}
DWORD worker_base_input_watch_bind(worker_base_input_watch *watch,HANDLE source)
{
    HANDLE owned=NULL,old;DWORD error;
    if(!watch)return ERROR_INVALID_PARAMETER;
    if(source && !DuplicateHandle(GetCurrentProcess(),source,GetCurrentProcess(),
        &owned,SYNCHRONIZE,FALSE,0))return GetLastError();
    EnterCriticalSection(&watch->lock);error=watch->error;
    if(!error) {
        old=watch->source;watch->source=owned;owned=NULL;
        if(!ResetEvent(watch->wake) || !SetEvent(watch->rearm))error=GetLastError();
    } else old=NULL;
    LeaveCriticalSection(&watch->lock);
    if(old)CloseHandle(old);
    if(owned)CloseHandle(owned);
    return error;
}
DWORD worker_base_input_watch_ack(worker_base_input_watch *watch)
{
    DWORD error;
    if(!watch)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&watch->lock);error=watch->error;
    if(!error && (!ResetEvent(watch->wake) || !SetEvent(watch->rearm)))error=GetLastError();
    LeaveCriticalSection(&watch->lock);return error;
}
HANDLE worker_base_input_watch_event(worker_base_input_watch *watch)
{return watch ? watch->wake : NULL;}
void worker_base_input_watch_destroy(worker_base_input_watch *watch)
{
    if(!watch)return;
    if(watch->local_stop)SetEvent(watch->local_stop);
    if(watch->thread){WaitForSingleObject(watch->thread,INFINITE);CloseHandle(watch->thread);}
    if(watch->source)CloseHandle(watch->source);
    if(watch->wake)CloseHandle(watch->wake);
    if(watch->rearm)CloseHandle(watch->rearm);
    if(watch->local_stop)CloseHandle(watch->local_stop);
    DeleteCriticalSection(&watch->lock);HeapFree(GetProcessHeap(),0,watch);
}
