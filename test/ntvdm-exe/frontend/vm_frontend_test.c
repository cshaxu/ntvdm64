/* Unit boundary: real NTVDM registry and frontend-base threads, deterministic
 * BaseSrv control provider. Actual RPC/chain acceptance is separate. */
#include "ntvdm-exe/frontend/vm_frontend.h"
#include "ntvdm-exe/frontend/vm_host.h"
#include "common/base_srv/frontend_client.h"
#include "common/base_srv/base_rpc_client.h"
#include "common/protocol/frontend_lifecycle.h"
#include <stdio.h>

typedef struct slot {
    common_frontend_location location;
    common_frontend_key key;
    HANDLE notify,retire,changed,peer,attached,close_seen,close_release;
    BOOL closed,used,pending,attach_failure;
    DWORD sequence;
} slot;
static slot slots[4];
static HANDLE broker;
static DWORD serial;
static volatile LONG checks;
static BOOL fail_state,fail_context;
#define CHECK(x) do { InterlockedIncrement(&checks); if(!(x)) { printf("FAIL line %d: %s\n",__LINE__,#x); ExitProcess(1); } } while(0)
static DWORD copy(HANDLE source,HANDLE *out)
{return DuplicateHandle(GetCurrentProcess(),source,GetCurrentProcess(),out,SYNCHRONIZE,FALSE,0) ? 0 : GetLastError();}
static slot *lookup(const common_frontend_key *key)
{
    unsigned i;
    for(i=0;i<4;++i)if(slots[i].used && slots[i].key.instance==key->instance &&
        slots[i].key.generation==key->generation)return &slots[i];
    return NULL;
}
DWORD OpenNtBaseClientBrokerProcess(HANDLE *out){return copy(broker,out);}
DWORD OpenNtBaseClientAcquireConsoleContext(HANDLE frontend,HANDLE *out)
{
    DWORD flags;
    *out=NULL;CHECK(GetHandleInformation(frontend,&flags));
    if(fail_context)return ERROR_ACCESS_DENIED;
    *out=CreateEventW(NULL,TRUE,FALSE,NULL);
    return *out ? ERROR_SUCCESS : GetLastError();
}
DWORD OpenNtBaseClientRegisterEmbeddedFrontend(const common_frontend_location *location,
    common_frontend_key *key,HANDLE *notification,HANDLE *retire)
{
    unsigned i;
    for(i=0;i<4;++i)if(!slots[i].used) {
        slot *s=&slots[i];ZeroMemory(s,sizeof(*s));s->used=TRUE;s->location=*location;
        s->key.instance=++serial;s->key.generation=serial;*key=s->key;
        s->notify=CreateEventW(NULL,TRUE,FALSE,NULL);
        s->retire=CreateEventW(NULL,TRUE,FALSE,NULL);
        s->changed=CreateEventW(NULL,FALSE,FALSE,NULL);
        s->attached=CreateEventW(NULL,TRUE,FALSE,NULL);
        s->close_seen=CreateEventW(NULL,TRUE,FALSE,NULL);
        CHECK(s->notify && s->retire && s->changed && s->attached && s->close_seen);
        CHECK(!copy(s->notify,notification));CHECK(!copy(s->retire,retire));return 0;
    }
    return ERROR_NOT_ENOUGH_MEMORY;
}
DWORD OpenNtBaseClientFrontendStateChanged(const common_frontend_key *key,HANDLE *out)
{
    slot *s=lookup(key);*out=NULL;
    if(fail_state)return ERROR_NOT_ENOUGH_MEMORY;
    return s ? copy(s->changed,out) : ERROR_ACCESS_DENIED;
}
DWORD OpenNtBaseClientRetireWorkerlessFrontend(const common_frontend_key *key,DWORD *action)
{
    slot *s=lookup(key);if(!s)return ERROR_ACCESS_DENIED;
    *action=s->closed ? OPENNT_FRONTEND_RETIRE_FINAL_CLOSE : OPENNT_FRONTEND_RETIRE_NONE;
    return 0;
}
DWORD OpenNtBaseClientCloseFrontendInstance(const common_frontend_key *key)
{
    slot *s=lookup(key);if(!s)return ERROR_ACCESS_DENIED;
    SetEvent(s->close_seen);
    if(s->close_release)CHECK(WaitForSingleObject(s->close_release,5000)==WAIT_OBJECT_0);
    s->closed=TRUE;SetEvent(s->retire);SetEvent(s->changed);return 0;
}
static void clear_slot(slot *s)
{
    CloseHandle(s->notify);CloseHandle(s->retire);CloseHandle(s->changed);
    CloseHandle(s->attached);CloseHandle(s->close_seen);
    if(s->peer)CloseHandle(s->peer);
    s->used=FALSE;
}
DWORD OpenNtBaseClientUnregisterFrontendInstance(const common_frontend_key *key)
{
    slot *s=lookup(key);if(!s)return ERROR_ACCESS_DENIED;
    if(!s->closed)return ERROR_BUSY;
    clear_slot(s);return 0;
}
DWORD OpenNtBaseClientFrontendRequest(const common_frontend_key *key,DWORD *id,HANDLE *worker)
{
    slot *s=lookup(key);*id=0;*worker=NULL;
    if(!s)return ERROR_ACCESS_DENIED;
    ResetEvent(s->notify);
    if(!s->pending)return ERROR_NOT_FOUND;
    s->pending=FALSE;*id=17;return copy(GetCurrentProcess(),worker);
}
DWORD OpenNtBaseClientAttachFrontendRequest(const common_frontend_key *key,DWORD id,
    HANDLE pipe,HANDLE ready,DWORD *generation)
{
    slot *s=lookup(key);if(!s)return ERROR_ACCESS_DENIED;
    CHECK(id==17 && ready);
    if(s->attach_failure){SetEvent(s->attached);return ERROR_ACCESS_DENIED;}
    CHECK(DuplicateHandle(GetCurrentProcess(),pipe,GetCurrentProcess(),&s->peer,0,FALSE,DUPLICATE_SAME_ACCESS));
    *generation=23;SetEvent(s->attached);return 0;
}
DWORD OpenNtBaseClientFrontendIoDisconnected(const common_frontend_key *key)
{return lookup(key) ? 0 : ERROR_ACCESS_DENIED;}
typedef struct io_state { LONG calls,finished;HANDLE entered,release; } io_state;
static DWORD dispatch(void *ctx,const ntvdm_vm_cancel *cancel,const console_io_request *req,console_io_reply *reply,
    frontend_base_result *result)
{
    io_state *state=ctx;(void)result;
    CHECK(state && req->operation==CONSOLE_IO_BARRIER);
    InterlockedIncrement(&state->calls);
    if(req->state.mode==1) {
        SetEvent(state->entered);
        HANDLE waits[]={cancel->stop,cancel->worker,cancel->retired,cancel->broker,state->release};
        DWORD wait=WaitForMultipleObjects(5,waits,FALSE,5000);
        CHECK(wait>=WAIT_OBJECT_0 && wait<WAIT_OBJECT_0+5);
        if(wait!=WAIT_OBJECT_0+4)return ERROR_OPERATION_ABORTED;
    }
    reply->result=TRUE;reply->state.count=req->state.count;return 0;
}
static DWORD pending(void *ctx,BOOL *value){(void)ctx;*value=FALSE;return 0;}
static void finish(void *ctx){if(ctx)InterlockedIncrement(&((io_state *)ctx)->finished);}

static void transfer(HANDLE pipe,BOOL write,void *data,DWORD bytes)
{
    OVERLAPPED io={0};DWORD done=0;
    io.hEvent=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(io.hEvent);
    if(!(write ? WriteFile(pipe,data,bytes,&done,&io) : ReadFile(pipe,data,bytes,&done,&io))) {
        CHECK(GetLastError()==ERROR_IO_PENDING);
        CHECK(WaitForSingleObject(io.hEvent,5000)==WAIT_OBJECT_0);
        CHECK(GetOverlappedResult(pipe,&io,&done,FALSE));
    }
    CHECK(done==bytes);CloseHandle(io.hEvent);
}
static slot *connect_vm(const common_frontend_location *identity,BOOL fail)
{
    unsigned i;
    for(i=0;i<4;++i)if(slots[i].used && slots[i].location.incarnation==identity->incarnation) {
        slot *s=&slots[i];s->attach_failure=fail;s->pending=TRUE;SetEvent(s->notify);
        CHECK(WaitForSingleObject(s->attached,5000)==WAIT_OBJECT_0);return s;
    }
    CHECK(FALSE);return NULL;
}
static void exchange(slot *s,DWORD sequence,DWORD mode)
{
    console_io_request q={0};console_io_reply r={0};
    q.operation=CONSOLE_IO_BARRIER;q.sequence=++s->sequence;q.state.mode=mode;
    q.version=CONSOLE_IO_VERSION;q.generation=23;q.state.count=sequence;
    transfer(s->peer,TRUE,&q,CONSOLE_IO_REQUEST_HEADER_BYTES);
    if(!mode) {
        transfer(s->peer,FALSE,&r,CONSOLE_IO_REPLY_HEADER_BYTES);
        CHECK(r.result && r.sequence==q.sequence && r.state.count==sequence &&
            r.version==CONSOLE_IO_VERSION && r.generation==23);
    }
}
static DWORD WINAPI close_host(void *context){return ntvdm_vm_host_close(context);}

static void wire_rejections(HANDLE input,const ntvdm_vm_io *io)
{
    unsigned i;
    for(i=0;i<4;++i) {
        ntvdm_vm_frontends *registry=NULL;
        common_frontend_location identity;
        io_state state={0};slot *s;
        console_io_request request={0};
        CHECK(!ntvdm_vm_frontends_create(50+i,&registry));
        CHECK(!ntvdm_vm_frontend_register(registry,0,input,io,&state,&identity));
        s=connect_vm(&identity,FALSE);
        request.version=CONSOLE_IO_VERSION;request.generation=23;request.sequence=1;
        request.operation=CONSOLE_IO_BARRIER;
        if(i==0)request.version++;
        if(i==1)request.generation++;
        if(i==2)request.sequence++;
        if(i==3)request.operation=38;
        transfer(s->peer,TRUE,&request,CONSOLE_IO_REQUEST_HEADER_BYTES);
        CHECK(WaitForSingleObject(s->close_seen,5000)==WAIT_OBJECT_0);
        CHECK(!ntvdm_vm_frontends_destroy(registry));
        CHECK(!state.calls && state.finished==1);
    }
}

typedef struct retire_call {
    ntvdm_vm_host *host;
    common_frontend_location identity;
} retire_call;
static DWORD WINAPI retire_vm(void *context)
{
    retire_call *call=context;
    return ntvdm_vm_host_retire(call->host,&call->identity);
}
static void independent_retirement(HANDLE input,const ntvdm_vm_io *io)
{
    ntvdm_vm_host *host=NULL;
    uint64_t payload;
    common_frontend_location a,b,c;
    io_state one={0},two={0};
    slot *sa,*sb;
    retire_call call;
    HANDLE release,seen,thread,origin=NULL,execution=NULL;
    DWORD result;
    CHECK(!ntvdm_vm_host_create(&host));
    CHECK(!ntvdm_vm_payload_begin(host,&payload));
    CHECK(!ntvdm_vm_host_register(host,payload,1,input,io,&one,&a));
    CHECK(!ntvdm_vm_host_register(host,payload,2,input,io,&two,&b));
    CHECK(!ntvdm_vm_host_launch_context(host,&a,&origin,&execution));
    CHECK(origin && execution);CloseHandle(origin);CloseHandle(execution);
    origin=execution=NULL;fail_context=TRUE;
    CHECK(ntvdm_vm_host_launch_context(host,&a,&origin,&execution)==ERROR_ACCESS_DENIED);
    CHECK(!origin && !execution);fail_context=FALSE;
    sa=connect_vm(&a,FALSE);sb=connect_vm(&b,FALSE);
    release=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(release);
    sa->close_release=release;CHECK(!copy(sa->close_seen,&seen));
    call.host=host;call.identity=a;
    thread=CreateThread(NULL,0,retire_vm,&call,0,NULL);CHECK(thread);
    CHECK(WaitForSingleObject(seen,5000)==WAIT_OBJECT_0);
    /* A is in a held RPC: B and a new VM in the SAME payload must progress. */
    CHECK(!ntvdm_vm_host_origin(host,&b,&origin));CloseHandle(origin);origin=NULL;
    exchange(sb,42,0);CHECK(two.calls==1);
    CHECK(!ntvdm_vm_host_register(host,payload,3,input,io,NULL,&c));
    CHECK(ntvdm_vm_host_origin(host,&a,&origin)==ERROR_INVALID_STATE && !origin);
    CHECK(WaitForSingleObject(thread,0)==WAIT_TIMEOUT);
    CHECK(SetEvent(release));
    CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(thread,&result) && result==ERROR_SUCCESS);
    CHECK(ntvdm_vm_host_origin(host,&a,&origin)==ERROR_NOT_FOUND && !origin);
    CHECK(!ntvdm_vm_host_destroy(host));
    CloseHandle(thread);CloseHandle(seen);CloseHandle(release);
}

static void active_host_tests(HANDLE input,const ntvdm_vm_io *io)
{
    ntvdm_vm_host *host=NULL;
    uint64_t first,second,replacement;
    common_frontend_location a,b,c;
    io_state one={0},two={0};
    slot *sa,*sb;
    HANDLE thread,close_seen,origin=NULL;
    DWORD result;
    CHECK(!ntvdm_vm_host_create(&host));
    CHECK(!ntvdm_vm_payload_begin(host,&first));CHECK(!ntvdm_vm_payload_begin(host,&second));
    CHECK(first!=second);
    CHECK(!ntvdm_vm_host_register(host,first,0,input,io,&one,&a));
    CHECK(!ntvdm_vm_host_register(host,second,0,input,io,&two,&b));
    sa=connect_vm(&a,FALSE);sb=connect_vm(&b,FALSE);
    exchange(sa,11,0);exchange(sb,22,0);
    CHECK(one.calls==1 && two.calls==1);
    CHECK(!ntvdm_vm_payload_end(host,first));CHECK(one.finished==1 && !two.finished);
    CHECK(ntvdm_vm_host_origin(host,&a,&origin)==ERROR_NOT_FOUND && !origin);
    exchange(sb,23,0);CHECK(two.calls==2);
    CHECK(!ntvdm_vm_payload_begin(host,&replacement));CHECK(replacement!=first);
    CHECK(!ntvdm_vm_host_register(host,replacement,0,input,io,NULL,&c));
    sa=connect_vm(&c,TRUE);CHECK(WaitForSingleObject(sa->close_seen,5000)==WAIT_OBJECT_0);
    CHECK(!ntvdm_vm_payload_end(host,replacement));
    two.entered=CreateEventW(NULL,TRUE,FALSE,NULL);two.release=CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(two.entered && two.release);
    exchange(sb,24,1);CHECK(WaitForSingleObject(two.entered,5000)==WAIT_OBJECT_0);
    CHECK(!copy(sb->close_seen,&close_seen));
    thread=CreateThread(NULL,0,close_host,host,0,NULL);CHECK(thread);
    CHECK(WaitForSingleObject(close_seen,5000)==WAIT_OBJECT_0);
    /* Retirement must cancel provider wait without any test-side release. */
    CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(thread,&result) && result==0 && two.finished==1);
    CHECK(ntvdm_vm_payload_begin(host,&replacement)==ERROR_OPERATION_ABORTED && !replacement);
    CHECK(ntvdm_vm_host_origin(host,&b,&origin)==ERROR_OPERATION_ABORTED && !origin);
    CHECK(!ntvdm_vm_host_destroy(host));
    CloseHandle(thread);CloseHandle(close_seen);CloseHandle(two.entered);CloseHandle(two.release);
}

int main(void)
{
    ntvdm_vm_frontends *registry=NULL;
    common_frontend_location a,b,reused,failed;
    const ntvdm_vm_io io={dispatch,pending,finish};
    HANDLE input=CreateEventW(NULL,TRUE,FALSE,NULL),origin=NULL;
    unsigned i;
    broker=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(input && broker);
    CHECK(ntvdm_vm_frontends_create(0,&registry)==ERROR_INVALID_PARAMETER);
    CHECK(!ntvdm_vm_frontends_create(7,&registry));
    CHECK(!ntvdm_vm_frontend_register(registry,0,input,&io,NULL,&a));
    CHECK(!ntvdm_vm_frontend_register(registry,1,input,&io,NULL,&b));
    CHECK(a.incarnation!=b.incarnation);
    CHECK(ntvdm_vm_frontend_register(registry,0,input,&io,NULL,&failed)==ERROR_ALREADY_EXISTS);
    CHECK(!ntvdm_vm_frontend_origin(registry,&a,&origin));CloseHandle(origin);
    CHECK(!ntvdm_vm_frontend_retire(registry,&a));
    CHECK(ntvdm_vm_frontend_origin(registry,&a,&origin)==ERROR_NOT_FOUND && !origin);
    CHECK(!ntvdm_vm_frontend_origin(registry,&b,&origin));CloseHandle(origin);
    CHECK(!ntvdm_vm_frontend_register(registry,0,input,&io,NULL,&reused));
    CHECK(reused.incarnation!=a.incarnation);
    CHECK(ntvdm_vm_frontend_retire(registry,&a)==ERROR_NOT_FOUND);
    CHECK(!ntvdm_vm_frontends_destroy(registry));
    CHECK(!ntvdm_vm_frontends_create(7,&registry));
    fail_state=TRUE;
    CHECK(ntvdm_vm_frontend_register(registry,0,input,&io,NULL,&failed)==ERROR_NOT_ENOUGH_MEMORY);
    CHECK(failed.incarnation && failed.incarnation!=reused.incarnation);
    CHECK(ntvdm_vm_frontend_origin(registry,&failed,&origin)==ERROR_INVALID_STATE);
    CHECK(!ntvdm_vm_frontends_destroy(registry));fail_state=FALSE;
    independent_retirement(input,&io);
    active_host_tests(input,&io);
    wire_rejections(input,&io);
    CHECK(!ntvdm_vm_frontends_create(8,&registry));
    {
        io_state state={0},waiting={0};slot *s,*other;
        CHECK(!ntvdm_vm_frontend_register(registry,0,input,&io,&state,&a));
        CHECK(!ntvdm_vm_frontend_register(registry,1,input,&io,&waiting,&b));
        s=connect_vm(&a,FALSE);other=connect_vm(&b,FALSE);
        waiting.entered=CreateEventW(NULL,TRUE,FALSE,NULL);
        waiting.release=CreateEventW(NULL,TRUE,FALSE,NULL);
        CHECK(waiting.entered && waiting.release);
        transfer(s->peer,TRUE,"x",1); /* A blocked partial-header read. */
        exchange(other,1,1);CHECK(WaitForSingleObject(waiting.entered,5000)==WAIT_OBJECT_0);
        SetEvent(broker);
        CHECK(!ntvdm_vm_frontends_destroy(registry));
        CHECK(state.finished==1 && !state.calls);
        CHECK(waiting.finished==1 && waiting.calls==1);
        CloseHandle(waiting.entered);CloseHandle(waiting.release);
    }
    for(i=0;i<4;++i)if(slots[i].used)clear_slot(&slots[i]);
    CloseHandle(input);CloseHandle(broker);
    printf("PASS %ld registry/active-I/O assertions; actual RPC remains an integration gate\n",checks);
    return 0;
}
