#include "publication.h"
#include <string.h>

typedef struct frame_copy {
    SIZE_T bytes;
    BYTE *payload;
} frame_copy;
struct worker_base_publication {
    CRITICAL_SECTION lock;
    HANDLE stop,changed,idle,timer,thread,shutdown;
    worker_base_publication_send_fn send;
    void *context;
    frame_copy pending,last;
    LARGE_INTEGER frequency,last_sent;
    BOOL active;
    DWORD error;
};

static void free_frame(frame_copy *frame)
{
    if(frame->payload)HeapFree(GetProcessHeap(),0,frame->payload);
    memset(frame,0,sizeof(*frame));
}
static BOOL same_frame(const frame_copy *a,const frame_copy *b)
{
    return a->payload && b->payload &&
        a->bytes==b->bytes && !memcmp(a->payload,b->payload,a->bytes);
}
/* Only one sender: publisher, or owner final drain after disabling admission
 * and waiting for idle. No state/painter lock covers transport. */
static DWORD deliver(worker_base_publication *p,frame_copy *copy)
{
    DWORD error=ERROR_SUCCESS;BOOL same;
    EnterCriticalSection(&p->lock);same=same_frame(copy,&p->last);LeaveCriticalSection(&p->lock);
    if(!same) {
        error=p->send(p->context,copy->payload,copy->bytes);
        if(!error) {
            EnterCriticalSection(&p->lock);
            free_frame(&p->last);p->last=*copy;memset(copy,0,sizeof(*copy));
            QueryPerformanceCounter(&p->last_sent);
            LeaveCriticalSection(&p->lock);
        }
    }
    free_frame(copy);return error;
}
static DWORD publisher_exit(worker_base_publication *p,DWORD error)
{
    EnterCriticalSection(&p->lock);p->error=error;SetEvent(p->idle);LeaveCriticalSection(&p->lock);
    return error;
}
static DWORD WINAPI publish_thread(void *context)
{
    worker_base_publication *p=context;
    HANDLE waits[3]={p->shutdown,p->stop,p->changed};
    for(;;) {
        DWORD status=WaitForMultipleObjects(3,waits,FALSE,INFINITE);
        LARGE_INTEGER now,due;LONGLONG ticks;
        frame_copy copy={0};DWORD error;
        if(status!=WAIT_OBJECT_0+2)return publisher_exit(p,status==WAIT_FAILED ? GetLastError() : ERROR_OPERATION_ABORTED);
        QueryPerformanceCounter(&now);
        EnterCriticalSection(&p->lock);
        ticks=p->last_sent.QuadPart ? p->last_sent.QuadPart+p->frequency.QuadPart/50-now.QuadPart : 0;
        LeaveCriticalSection(&p->lock);
        if(ticks>0) {
            HANDLE rate_waits[3]={p->shutdown,p->stop,p->timer};
            due.QuadPart=-(ticks*10000000+p->frequency.QuadPart-1)/p->frequency.QuadPart;
            if(!SetWaitableTimer(p->timer,&due,0,NULL,NULL,FALSE))return publisher_exit(p,GetLastError());
            /* One-shot only while dirty. More updates replace pending state;
             * they cannot restart the deadline or form a frame backlog. */
            status=WaitForMultipleObjects(3,rate_waits,FALSE,INFINITE);
            if(status!=WAIT_OBJECT_0+2)return publisher_exit(p,status==WAIT_FAILED ? GetLastError() : ERROR_OPERATION_ABORTED);
        }
        EnterCriticalSection(&p->lock);
        ResetEvent(p->changed);
        if(p->active && p->pending.payload) {
            copy=p->pending;memset(&p->pending,0,sizeof(p->pending));ResetEvent(p->idle);
        }
        LeaveCriticalSection(&p->lock);
        if(!copy.payload)continue;
        error=deliver(p,&copy);
        EnterCriticalSection(&p->lock);p->error=error;SetEvent(p->idle);LeaveCriticalSection(&p->lock);
        if(error)return error;
    }
}

worker_base_publication *worker_base_publication_create(worker_base_publication_send_fn send,void *context,HANDLE shutdown)
{
    worker_base_publication *p;
    if(!send || !shutdown){SetLastError(ERROR_INVALID_PARAMETER);return NULL;}
    p=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*p));
    if(!p){SetLastError(ERROR_NOT_ENOUGH_MEMORY);return NULL;}
    InitializeCriticalSection(&p->lock);p->send=send;p->context=context;p->shutdown=shutdown;
    QueryPerformanceFrequency(&p->frequency);
    p->stop=CreateEventW(NULL,TRUE,FALSE,NULL);p->changed=CreateEventW(NULL,TRUE,FALSE,NULL);
    p->idle=CreateEventW(NULL,TRUE,TRUE,NULL);p->timer=CreateWaitableTimerW(NULL,FALSE,NULL);
    if(p->stop && p->changed && p->idle && p->timer)
        p->thread=CreateThread(NULL,0,publish_thread,p,0,NULL);
    if(!p->thread){DWORD error=GetLastError();worker_base_publication_destroy(p);SetLastError(error);return NULL;}
    return p;
}

DWORD worker_base_publication_offer(worker_base_publication *p,const void *payload,SIZE_T bytes,BOOL *queued)
{
    BYTE *copy;DWORD error;
    if(!queued || !payload || !bytes)return ERROR_INVALID_PARAMETER;
    *queued=FALSE;
    if(!p)return ERROR_SUCCESS;
    EnterCriticalSection(&p->lock);
    error=p->error;
    if(!error && p->active) {
        copy=HeapAlloc(GetProcessHeap(),0,bytes);
        if(!copy)error=ERROR_NOT_ENOUGH_MEMORY;
        else {
            memcpy(copy,payload,bytes);free_frame(&p->pending);
            p->pending.bytes=bytes;p->pending.payload=copy;
            SetEvent(p->changed);*queued=TRUE;
        }
    }
    LeaveCriticalSection(&p->lock);return error;
}

DWORD worker_base_publication_active(worker_base_publication *p,BOOL active)
{
    frame_copy copy={0};DWORD error,status;
    if(!p)return ERROR_SUCCESS;
    EnterCriticalSection(&p->lock);
    if(active==p->active){error=p->error;LeaveCriticalSection(&p->lock);return error;}
    if(active && p->error){error=p->error;LeaveCriticalSection(&p->lock);return error;}
    p->active=active;
    if(active) {free_frame(&p->last);p->last_sent.QuadPart=0;}
    LeaveCriticalSection(&p->lock);
    if(active)return ERROR_SUCCESS;
    /* No new claim after active=false. Wait for the already claimed send,
     * then force-drain latest state without the rate cap before final paint. */
    {
        HANDLE waits[2]={p->idle,p->thread};
        status=WaitForMultipleObjects(2,waits,FALSE,INFINITE);
        if(status==WAIT_FAILED)return GetLastError();
    }
    EnterCriticalSection(&p->lock);
    error=p->error;copy=p->pending;memset(&p->pending,0,sizeof(p->pending));ResetEvent(p->changed);
    LeaveCriticalSection(&p->lock);
    if(copy.payload) {
        if(!error)error=deliver(p,&copy);else free_frame(&copy);
    }
    if(error) {
        EnterCriticalSection(&p->lock);p->error=error;LeaveCriticalSection(&p->lock);
    }
    return error;
}

DWORD worker_base_publication_commit(worker_base_publication *p,const void *payload,SIZE_T bytes)
{
    frame_copy copy={0};DWORD error;
    if(!p || !payload || !bytes)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&p->lock);
    error=p->error;
    if(!error && p->active)error=ERROR_BUSY;
    LeaveCriticalSection(&p->lock);
    if(error)return error;
    copy.payload=HeapAlloc(GetProcessHeap(),0,bytes);
    if(!copy.payload)return ERROR_NOT_ENOUGH_MEMORY;
    copy.bytes=bytes;memcpy(copy.payload,payload,bytes);
    error=deliver(p,&copy);
    if(error) {
        EnterCriticalSection(&p->lock);p->error=error;LeaveCriticalSection(&p->lock);
    }
    return error;
}

void worker_base_publication_destroy(worker_base_publication *p)
{
    if(!p)return;
    if(p->stop)SetEvent(p->stop);
    if(p->thread){WaitForSingleObject(p->thread,INFINITE);CloseHandle(p->thread);}
    free_frame(&p->pending);free_frame(&p->last);
    if(p->timer)CloseHandle(p->timer);if(p->idle)CloseHandle(p->idle);
    if(p->changed)CloseHandle(p->changed);if(p->stop)CloseHandle(p->stop);
    DeleteCriticalSection(&p->lock);HeapFree(GetProcessHeap(),0,p);
}
