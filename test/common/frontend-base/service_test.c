/* Service pump unit: real threads/events, explicit local control callbacks.
 * NTSRV authentication and Console restoration require integration tests. */
#include "common/frontend-base/service.h"
#include "common/protocol/frontend_lifecycle.h"
#include <stdio.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"FAIL %u: %s error=%lu\n",__LINE__,#x,GetLastError());ExitProcess(1);}}while(0)
typedef struct fixture {
    frontend_base_service *service;
    HANDLE notify,state,broker,retire,attached,detached,faulted,retired,channel;
    volatile LONG request,action,starts,stops,acks,faults;
    DWORD stop_error,fault_error,retire_error;
} fixture;
static DWORD retirement(void *context,DWORD *action)
{
    fixture *f=context;
    ResetEvent(f->state);
    *action=(DWORD)InterlockedExchange(&f->action,0);
    if(*action)SetEvent(f->retired);
    return f->retire_error;
}
static DWORD prepare(void *context){(void)context;return 0;}
static DWORD request(void *context,DWORD *id,HANDLE *worker)
{
    fixture *f=context;
    LONG value=InterlockedExchange(&f->request,0);
    ResetEvent(f->notify);*id=0;*worker=NULL;
    if(!value)return ERROR_NOT_FOUND;
    if(value==2)return 0;
    *id=17;
    CHECK(DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),worker,SYNCHRONIZE,FALSE,0));
    return 0;
}
static DWORD channel_start(void *context,DWORD id,HANDLE worker,void **channel)
{
    fixture *f=context;
    CHECK(id==17 && worker && !f->channel);CloseHandle(worker);
    f->channel=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(f->channel);
    *channel=f->channel;InterlockedIncrement(&f->starts);
    SetEvent(f->attached);return 0;
}
static DWORD channel_stop(void *context,void *channel)
{
    fixture *f=context;
    CHECK(channel==f->channel);
    if(f->stop_error)return f->stop_error;
    CloseHandle(f->channel);f->channel=NULL;InterlockedIncrement(&f->stops);
    return 0;
}
static HANDLE channel_thread(void *context,void *channel){(void)context;return channel;}
static DWORD disconnected(void *context)
{
    fixture *f=context;
    CHECK(!f->channel && f->starts==f->stops);
    InterlockedIncrement(&f->acks);SetEvent(f->detached);return 0;
}
static DWORD fault(void *context)
{
    fixture *f=context;
    InterlockedIncrement(&f->faults);SetEvent(f->faulted);return f->fault_error;
}
static const frontend_base_service_ops ops={retirement,prepare,request,channel_start,channel_stop,channel_thread,disconnected,fault};
static void initialize(fixture *f)
{
    ZeroMemory(f,sizeof(*f));
    f->notify=CreateEventW(NULL,TRUE,FALSE,NULL);f->state=CreateEventW(NULL,TRUE,FALSE,NULL);
    /* Waitable local event substitutes service liveness only in this unit. */
    f->broker=CreateEventW(NULL,TRUE,FALSE,NULL);f->retire=CreateEventW(NULL,TRUE,FALSE,NULL);
    f->attached=CreateEventW(NULL,TRUE,FALSE,NULL);f->detached=CreateEventW(NULL,TRUE,FALSE,NULL);
    f->faulted=CreateEventW(NULL,TRUE,FALSE,NULL);
    f->retired=CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(f->notify && f->state && f->broker && f->retire && f->attached && f->detached && f->faulted && f->retired);
    CHECK(!frontend_base_service_start(f->notify,f->retire,f->broker,f->state,&ops,f,&f->service));
}
static void attach(fixture *f)
{
    ResetEvent(f->attached);InterlockedExchange(&f->request,1);SetEvent(f->notify);
    CHECK(WaitForSingleObject(f->attached,5000)==WAIT_OBJECT_0);
}
static void dispose(fixture *f)
{
    CHECK(!frontend_base_service_stop(f->service));
    CHECK(!f->channel);
    CloseHandle(f->notify);CloseHandle(f->state);CloseHandle(f->broker);CloseHandle(f->retire);
    CloseHandle(f->attached);CloseHandle(f->detached);CloseHandle(f->faulted);CloseHandle(f->retired);
}
static DWORD wait_exit(fixture *f)
{
    DWORD code;
    HANDLE thread=frontend_base_service_thread(f->service);
    CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(thread,&code));return code;
}
int main(void)
{
    fixture a,b;
    initialize(&a);initialize(&b);attach(&a);attach(&b);
    InterlockedExchange(&a.request,2);SetEvent(a.notify);
    CHECK(WaitForSingleObject(a.detached,5000)==WAIT_OBJECT_0);
    CHECK(a.acks==1 && !a.channel && b.channel && b.stops==0);
    attach(&a);CHECK(a.starts==2);
    dispose(&a);CHECK(b.channel && b.stops==0);dispose(&b);

    initialize(&a);attach(&a);
    SetEvent(a.channel); /* EOF is not an NTSRV release. */
    CHECK(WaitForSingleObject(a.faulted,5000)==WAIT_OBJECT_0);
    CHECK(a.faults==1 && a.acks==0 && a.stops==0);
    InterlockedExchange(&a.action,OPENNT_FRONTEND_RETIRE_FINAL_CLOSE);SetEvent(a.state);
    CHECK(wait_exit(&a)==ERROR_PIPE_NOT_CONNECTED);
    dispose(&a);CHECK(a.stops==1 && a.faults==1);

    initialize(&a);attach(&a);a.stop_error=ERROR_BUSY;
    CHECK(frontend_base_service_stop(a.service)==ERROR_BUSY);
    CHECK(a.channel && !a.stops && !a.acks);
    a.stop_error=0;dispose(&a);CHECK(a.stops==1);

    initialize(&a);attach(&a);SetEvent(a.broker);
    CHECK(wait_exit(&a)==ERROR_INVALID_HANDLE);
    CHECK(!a.faults && !a.acks);dispose(&a);

    initialize(&a);attach(&a);a.fault_error=ERROR_ACCESS_DENIED;
    SetEvent(a.channel);
    CHECK(WaitForSingleObject(a.faulted,5000)==WAIT_OBJECT_0);
    SetEvent(a.broker);CHECK(wait_exit(&a)==ERROR_INVALID_HANDLE);
    CHECK(a.faults==1 && !a.acks);dispose(&a);
    initialize(&a);attach(&a);
    InterlockedExchange(&a.action,OPENNT_FRONTEND_RETIRE_RETURN_CONSOLE);SetEvent(a.state);
    CHECK(WaitForSingleObject(a.retired,5000)==WAIT_OBJECT_0);
    /* Return preparation freezes new I/O admission and ignores channel EOF. */
    InterlockedExchange(&a.request,1);SetEvent(a.notify);SetEvent(a.channel);
    InterlockedExchange(&a.action,OPENNT_FRONTEND_RETIRE_FINAL_CLOSE);SetEvent(a.state);
    CHECK(wait_exit(&a)==ERROR_SUCCESS);
    CHECK(a.starts==1 && !a.acks && !a.faults);dispose(&a);

    initialize(&a);attach(&a);a.retire_error=ERROR_WRITE_FAULT;
    InterlockedExchange(&a.action,OPENNT_FRONTEND_RETIRE_RETURN_CONSOLE);SetEvent(a.state);
    CHECK(WaitForSingleObject(a.faulted,5000)==WAIT_OBJECT_0);
    CHECK(a.faults==1 && !a.acks && !a.stops);
    SetEvent(a.broker);CHECK(wait_exit(&a)==ERROR_INVALID_HANDLE);dispose(&a);
    puts("PASS frontend-base service isolation release ordering fault authority and retained failed join");
    return 0;
}
