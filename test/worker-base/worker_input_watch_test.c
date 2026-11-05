#include "worker-base/input_watch.h"
#include <stdio.h>
#define CHECK(x) do { if(!(x)){printf("FAIL line %d: %s (%lu)\n",__LINE__,#x,GetLastError());return 1;} ++checks; } while(0)
static unsigned checks;
static HANDLE task_close_event;
typedef struct probe { HANDLE seen,source; volatile LONG count; DWORD error; volatile LONG failures; } probe;
static DWORD ready(void *context)
{
    probe *p=context;
    ResetEvent(p->source);InterlockedIncrement(&p->count);SetEvent(p->seen);
    return p->error;
}
static void closed(void *context)
{probe *p=context;InterlockedIncrement(&p->count);SetEvent(p->seen);}
static DWORD task_closed(void *context)
{probe *p=context;ResetEvent(task_close_event);InterlockedIncrement(&p->count);SetEvent(p->seen);return ERROR_SUCCESS;}
static void failed(void *context,DWORD error)
{probe *p=context;p->error=error;InterlockedIncrement(&p->failures);SetEvent(p->seen);}
int main(void)
{
    DWORD before,after;unsigned i;
    CHECK(worker_base_input_watch_create(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)==NULL);
    CHECK(GetLastError()==ERROR_INVALID_PARAMETER);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
    for(i=0;i<50;++i) {
        HANDLE shutdown=CreateEventW(NULL,TRUE,FALSE,NULL);
        HANDLE stop=CreateEventW(NULL,TRUE,FALSE,NULL);
        HANDLE source=CreateEventW(NULL,TRUE,FALSE,NULL);
        HANDLE next=CreateEventW(NULL,TRUE,FALSE,NULL);
        worker_base_input_watch *w=worker_base_input_watch_create(NULL,shutdown,NULL,stop,NULL,NULL,NULL,NULL,NULL,NULL);
        CHECK(w!=NULL);CHECK(!worker_base_input_watch_bind(w,source));
        CHECK(SetEvent(source));
        CHECK(WaitForSingleObject(worker_base_input_watch_event(w),3000)==WAIT_OBJECT_0);
        CHECK(ResetEvent(source));CHECK(!worker_base_input_watch_ack(w));
        CHECK(WaitForSingleObject(worker_base_input_watch_event(w),20)==WAIT_TIMEOUT);
        CloseHandle(source);CHECK(!worker_base_input_watch_bind(w,NULL));
        CHECK(!worker_base_input_watch_bind(w,next));CHECK(SetEvent(next));
        CHECK(WaitForSingleObject(worker_base_input_watch_event(w),3000)==WAIT_OBJECT_0);
        CHECK(SetEvent(stop));worker_base_input_watch_destroy(w);
        CloseHandle(next);CloseHandle(stop);CloseHandle(shutdown);
    }
    {
        HANDLE shutdown=CreateEventW(NULL,TRUE,FALSE,NULL),stop=CreateEventW(NULL,TRUE,FALSE,NULL);
        probe p={CreateEventW(NULL,TRUE,FALSE,NULL),CreateEventW(NULL,TRUE,FALSE,NULL),0,0};
        worker_base_input_watch *w=worker_base_input_watch_create(NULL,shutdown,NULL,stop,ready,NULL,closed,NULL,failed,&p);
        CHECK(w!=NULL);CHECK(!worker_base_input_watch_bind(w,p.source));
        CHECK(SetEvent(p.source));CHECK(WaitForSingleObject(p.seen,3000)==WAIT_OBJECT_0);
        CHECK(p.count==1);CHECK(!worker_base_input_watch_ack(w));
        ResetEvent(p.seen);CHECK(SetEvent(p.source));
        CHECK(WaitForSingleObject(p.seen,3000)==WAIT_OBJECT_0);CHECK(p.count==2);
        CHECK(SetEvent(shutdown));ResetEvent(p.seen);
        /* Destroy joins the shutdown callback; no sleep guesses completion. */
        worker_base_input_watch_destroy(w);CHECK(p.count==3);
        CloseHandle(p.seen);CloseHandle(p.source);CloseHandle(stop);CloseHandle(shutdown);
    }
    {
        HANDLE shutdown=CreateEventW(NULL,TRUE,FALSE,NULL),stop=CreateEventW(NULL,TRUE,FALSE,NULL);
        probe p={CreateEventW(NULL,TRUE,FALSE,NULL),CreateEventW(NULL,TRUE,FALSE,NULL),0,ERROR_INVALID_DATA};
        worker_base_input_watch *w=worker_base_input_watch_create(NULL,shutdown,NULL,stop,ready,NULL,NULL,NULL,failed,&p);
        CHECK(w!=NULL);CHECK(!worker_base_input_watch_bind(w,p.source));
        CHECK(SetEvent(p.source));CHECK(WaitForSingleObject(p.seen,3000)==WAIT_OBJECT_0);
        worker_base_input_watch_destroy(w);CHECK(p.count==1 && p.error==ERROR_INVALID_DATA && p.failures==1);
        CloseHandle(p.seen);CloseHandle(p.source);CloseHandle(stop);CloseHandle(shutdown);
    }
    {
        HANDLE shutdown=CreateEventW(NULL,TRUE,FALSE,NULL),stop=CreateEventW(NULL,TRUE,TRUE,NULL);
        probe p={CreateEventW(NULL,TRUE,FALSE,NULL),CreateEventW(NULL,TRUE,TRUE,NULL),0,0};
        worker_base_input_watch *w=worker_base_input_watch_create(NULL,shutdown,NULL,stop,ready,NULL,closed,NULL,failed,&p);
        CHECK(w!=NULL);CHECK(!worker_base_input_watch_bind(w,p.source));
        worker_base_input_watch_destroy(w);CHECK(p.count==0);
        CloseHandle(p.seen);CloseHandle(p.source);CloseHandle(stop);CloseHandle(shutdown);
    }
    {
        HANDLE shutdown=CreateEventW(NULL,TRUE,TRUE,NULL),stop=CreateEventW(NULL,TRUE,FALSE,NULL);
        probe p={CreateEventW(NULL,TRUE,FALSE,NULL),CreateEventW(NULL,TRUE,TRUE,NULL),0,0};
        worker_base_input_watch *w=worker_base_input_watch_create(NULL,shutdown,NULL,stop,ready,NULL,closed,NULL,failed,&p);
        CHECK(w!=NULL);CHECK(WaitForSingleObject(p.seen,3000)==WAIT_OBJECT_0);
        worker_base_input_watch_destroy(w);CHECK(p.count==1);
        CloseHandle(p.seen);CloseHandle(p.source);CloseHandle(stop);CloseHandle(shutdown);
    }
    /* Lifecycle-only consumer: no source and no character callbacks. */
    {
        HANDLE stop=CreateEventW(NULL,TRUE,FALSE,NULL);
        HANDLE shutdown=CreateEventW(NULL,TRUE,FALSE,NULL);
        probe p={CreateEventW(NULL,TRUE,FALSE,NULL),NULL,0,0};
        worker_base_input_watch *w=worker_base_input_watch_create(NULL,shutdown,NULL,stop,NULL,NULL,closed,NULL,failed,&p);
        CHECK(w!=NULL);CHECK(SetEvent(shutdown));
        CHECK(WaitForSingleObject(p.seen,10000)==WAIT_OBJECT_0);
        worker_base_input_watch_destroy(w);CHECK(p.count==1 && !p.failures);
        CloseHandle(stop);CloseHandle(shutdown);CloseHandle(p.seen);
    }
    /* A task-close instruction is non-terminal: it must not consume the
     * worker-close path or prevent a subsequent input-source rearm. */
    {
        HANDLE task=CreateEventW(NULL,TRUE,FALSE,NULL),stop=CreateEventW(NULL,TRUE,FALSE,NULL);
        HANDLE source=CreateEventW(NULL,TRUE,FALSE,NULL);
        probe p={CreateEventW(NULL,TRUE,FALSE,NULL),source,0,0};
        worker_base_input_watch *w=worker_base_input_watch_create(NULL,NULL,task,stop,
            ready,NULL,NULL,task_closed,failed,&p);
        CHECK(w!=NULL);CHECK(!worker_base_input_watch_bind(w,source));
        task_close_event=task;
        CHECK(SetEvent(task));CHECK(WaitForSingleObject(p.seen,3000)==WAIT_OBJECT_0);
        CHECK(p.count==1);CHECK(ResetEvent(p.seen));
        CHECK(SetEvent(source));CHECK(WaitForSingleObject(p.seen,3000)==WAIT_OBJECT_0);
        CHECK(p.count==2);CHECK(SetEvent(stop));worker_base_input_watch_destroy(w);
        CloseHandle(p.seen);CloseHandle(source);CloseHandle(stop);CloseHandle(task);task_close_event=NULL;
    }
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after));CHECK(after==before);
    printf("PASS shared input watcher: %u assertions; event/callback, rearm, source replacement, close priority, 50-cycle handles\n",checks);
    return 0;
}
