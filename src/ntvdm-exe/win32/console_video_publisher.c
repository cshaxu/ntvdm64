#include "console_video_publisher.h"
#include <string.h>

typedef struct frame_copy {
    console_video_description description;
    BYTE *payload;
} frame_copy;
struct ntvdm_video_publisher {
    CRITICAL_SECTION lock;
    HANDLE stop,changed,idle,timer,thread,shutdown;
    ntvdm_video_send_fn send;
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
        !memcmp(&a->description,&b->description,sizeof(a->description)) &&
        !memcmp(a->payload,b->payload,a->description.bytes);
}
/* Only one sender: publisher, or guest final drain after disabling admission
 * and waiting for idle. No state/painter lock covers transport. */
static DWORD deliver(ntvdm_video_publisher *p,frame_copy *copy)
{
    DWORD error=ERROR_SUCCESS;BOOL same;
    EnterCriticalSection(&p->lock);same=same_frame(copy,&p->last);LeaveCriticalSection(&p->lock);
    if(!same) {
        error=p->send(p->context,&copy->description,copy->payload);
        if(!error) {
            EnterCriticalSection(&p->lock);
            free_frame(&p->last);p->last=*copy;memset(copy,0,sizeof(*copy));
            QueryPerformanceCounter(&p->last_sent);
            LeaveCriticalSection(&p->lock);
        }
    }
    free_frame(copy);return error;
}
static DWORD publisher_exit(ntvdm_video_publisher *p,DWORD error)
{
    EnterCriticalSection(&p->lock);p->error=error;SetEvent(p->idle);LeaveCriticalSection(&p->lock);
    return error;
}
static DWORD WINAPI publish_thread(void *context)
{
    ntvdm_video_publisher *p=context;
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

ntvdm_video_publisher *ntvdm_video_publisher_create(ntvdm_video_send_fn send,void *context,HANDLE shutdown)
{
    ntvdm_video_publisher *p;
    if(!send || !shutdown){SetLastError(ERROR_INVALID_PARAMETER);return NULL;}
    p=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*p));
    if(!p){SetLastError(ERROR_NOT_ENOUGH_MEMORY);return NULL;}
    InitializeCriticalSection(&p->lock);p->send=send;p->context=context;p->shutdown=shutdown;
    QueryPerformanceFrequency(&p->frequency);
    p->stop=CreateEventW(NULL,TRUE,FALSE,NULL);p->changed=CreateEventW(NULL,TRUE,FALSE,NULL);
    p->idle=CreateEventW(NULL,TRUE,TRUE,NULL);p->timer=CreateWaitableTimerW(NULL,FALSE,NULL);
    if(p->stop && p->changed && p->idle && p->timer)
        p->thread=CreateThread(NULL,0,publish_thread,p,0,NULL);
    if(!p->thread){DWORD error=GetLastError();ntvdm_video_publisher_destroy(p);SetLastError(error);return NULL;}
    return p;
}

DWORD ntvdm_video_publisher_offer(ntvdm_video_publisher *p,const console_video_description *description,
    const void *payload,BOOL *queued)
{
    BYTE *copy;DWORD error;
    *queued=FALSE;
    if(!p)return ERROR_SUCCESS;
    EnterCriticalSection(&p->lock);
    error=p->error;
    if(!error && p->active) {
        copy=HeapAlloc(GetProcessHeap(),0,description->bytes);
        if(!copy)error=ERROR_NOT_ENOUGH_MEMORY;
        else {
            memcpy(copy,payload,description->bytes);free_frame(&p->pending);
            p->pending.description=*description;p->pending.payload=copy;
            SetEvent(p->changed);*queued=TRUE;
        }
    }
    LeaveCriticalSection(&p->lock);return error;
}

DWORD ntvdm_video_publisher_active(ntvdm_video_publisher *p,BOOL active)
{
    frame_copy copy={0};DWORD error,status;
    if(!p)return ERROR_SUCCESS;
    EnterCriticalSection(&p->lock);
    if(active==p->active){error=p->error;LeaveCriticalSection(&p->lock);return error;}
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

void ntvdm_video_publisher_destroy(ntvdm_video_publisher *p)
{
    if(!p)return;
    if(p->stop)SetEvent(p->stop);
    if(p->thread){WaitForSingleObject(p->thread,INFINITE);CloseHandle(p->thread);}
    free_frame(&p->pending);free_frame(&p->last);
    if(p->timer)CloseHandle(p->timer);if(p->idle)CloseHandle(p->idle);
    if(p->changed)CloseHandle(p->changed);if(p->stop)CloseHandle(p->stop);
    DeleteCriticalSection(&p->lock);HeapFree(GetProcessHeap(),0,p);
}
