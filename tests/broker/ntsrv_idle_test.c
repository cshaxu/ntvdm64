/* Deterministic tests of the actual broker entry/lifetime functions.
 * Only clock, timer and service/peer boundaries are substituted. */
#include <windows.h>
#include <rpc.h>
#include <stdio.h>
#include "ntsrv-exe/opennt/include/base_service.h"
#include "ntsrv-exe/transport/rpc_security.h"
static ULONGLONG clock_ms=100;
static BOOL empty=TRUE,fail_timer,fail_create,fail_stop;
static DWORD peer_error,connect_error;
static unsigned armed,cancelled;
static ULONGLONG WINAPI test_clock(void) { return clock_ms; }
static BOOL WINAPI test_timer(HANDLE h,const LARGE_INTEGER *due,LONG period,
    PTIMERAPCROUTINE callback,LPVOID argument,BOOL resume)
{
    (void)h;(void)due;(void)period;(void)callback;(void)argument;(void)resume;
    ++armed;
    if (fail_timer) { SetLastError(ERROR_NOT_ENOUGH_MEMORY);return FALSE; }
    return TRUE;
}
static BOOL WINAPI test_cancel(HANDLE h) { (void)h;++cancelled;return TRUE; }
static HANDLE WINAPI test_create(LPSECURITY_ATTRIBUTES a,BOOL manual,LPCWSTR name)
{
    if (fail_create) { SetLastError(ERROR_NOT_ENOUGH_MEMORY);return NULL; }
    return CreateWaitableTimerW(a,manual,name);
}
static DWORD WINAPI test_wait(HANDLE h,DWORD timeout)
{
    if (fail_stop) { clock_ms+=10000;return WAIT_OBJECT_0; }
    return WaitForSingleObject(h,timeout);
}
static RPC_STATUS RPC_ENTRY test_stop(RPC_BINDING_HANDLE binding)
{ return fail_stop?RPC_S_ACCESS_DENIED:RpcMgmtStopServerListening(binding); }
static BOOL test_empty(OPENNT_BASE_SERVICE *s) { (void)s;return empty; }
static DWORD test_peer(const broker_rpc_scope *s,RPC_BINDING_HANDLE b,HANDLE p,DWORD *pid)
{ (void)s;(void)b;(void)p;*pid=1;return peer_error; }
static DWORD test_connect(OPENNT_BASE_SERVICE *s,HANDLE p,OPENNT_BASE_CONNECTION **c,ULONG *g)
{
    (void)s;(void)p;
    if (connect_error==ERROR_UNHANDLED_EXCEPTION) RaiseException(connect_error,0,0,NULL);
    if (connect_error) return connect_error;
    empty=FALSE;*c=(OPENNT_BASE_CONNECTION *)1;*g=1;return 0;
}
static RPC_STATUS test_authorize(const broker_rpc_scope *s,RPC_BINDING_HANDLE b)
{ (void)s;(void)b;return RPC_S_OK; }
#define GetTickCount64 test_clock
#define SetWaitableTimer test_timer
#define CancelWaitableTimer test_cancel
#define CreateWaitableTimerW test_create
#define WaitForSingleObject test_wait
#define RpcMgmtStopServerListening test_stop
#define OpenNtBaseServiceIsEmpty test_empty
#define OpenNtBaseServiceConnect test_connect
#define broker_rpc_peer_process test_peer
#define broker_rpc_authorize test_authorize
#define main ntsrv_product_main
#include "ntsrv-exe/main.c"
#undef main
#define CHECK(x) do { if (!(x)) { printf("FAIL line %d: %s\n",__LINE__,#x);return 1; } } while (0)
int main(int argc,char **argv)
{
    unsigned char version[APP_VERSION_BYTES]=APP_VERSION,reply[APP_VERSION_BYTES];
    ULONG protocol,generation;
    VDM_CONNECTION connection;
    ULONGLONG deadline;
    if (argc>1 && !strcmp(argv[1],"create-fatal")) {
        fail_create=TRUE;return ntsrv_product_main();
    }
    if (argc>1 && !strcmp(argv[1],"stop-fatal")) {
        fail_stop=TRUE;return ntsrv_product_main();
    }
    service=(OPENNT_BASE_SERVICE *)1;idle_timer=(HANDLE)1;
    if (argc>1) { fail_timer=TRUE;basesrv_schedule_empty_stop();return 99; }
    basesrv_schedule_empty_stop();
    CHECK(idle_deadline==10100 && armed==1);
    deadline=idle_deadline;
    CHECK(!authorize(NULL,NULL) && idle_deadline==deadline && !cancelled);
    basesrv_schedule_empty_stop();
    CHECK(idle_deadline==deadline && armed==1);
    clock_ms=10099;
    CHECK(!basesrv_claim_empty_stop() && !idle_stopping);
    CHECK(!basesrv_begin_connect() && !idle_deadline && pending_connects==1);
    CHECK(!basesrv_begin_connect() && pending_connects==2);
    basesrv_end_connect();
    CHECK(pending_connects==1 && !idle_deadline);
    clock_ms=20000;
    CHECK(!basesrv_claim_empty_stop());
    basesrv_end_connect();
    CHECK(idle_deadline==30000 && !pending_connects);
    /* Identity failure used to cancel the timer permanently. */
    clock_ms=25000;peer_error=ERROR_ACCESS_DENIED;
    CHECK(Server_Connect(NULL,NULL,APP_PROTOCOL_VERSION,version,&protocol,reply,
        &connection,&generation)==ERROR_ACCESS_DENIED);
    CHECK(idle_deadline==35000 && !pending_connects);
    peer_error=0;clock_ms=26000;
    CHECK(Server_Connect(NULL,NULL,APP_PROTOCOL_VERSION+1,version,&protocol,reply,
        &connection,&generation)==ERROR_REVISION_MISMATCH);
    CHECK(idle_deadline==36000 && !pending_connects);
    connect_error=ERROR_UNHANDLED_EXCEPTION;
    __try {
        (void)Server_Connect(NULL,NULL,APP_PROTOCOL_VERSION,version,&protocol,reply,&connection,&generation);
        return 2;
    } __except(EXCEPTION_EXECUTE_HANDLER) { }
    CHECK(!pending_connects && idle_deadline==36000);
    connect_error=0;
    CHECK(!Server_Connect(NULL,NULL,APP_PROTOCOL_VERSION,version,&protocol,reply,&connection,&generation));
    CHECK(!idle_deadline && !pending_connects && connection && generation);
    clock_ms=50000;CHECK(!basesrv_claim_empty_stop());
    empty=TRUE;basesrv_schedule_empty_stop();
    CHECK(idle_deadline==60000);
    clock_ms=60000;CHECK(basesrv_claim_empty_stop() && idle_stopping);
    CHECK(basesrv_begin_connect()==RPC_S_SERVER_UNAVAILABLE && !pending_connects);
    puts("PASS: idle, observer authentication, pending/failed/successful Connect, exception cleanup, disconnect and stop ordering");
    return 0;
}
