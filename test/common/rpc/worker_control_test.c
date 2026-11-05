#include "common/rpc/worker_control.h"
#include "common/protocol/frontend_protocol.h"
#include <stdio.h>

static unsigned checks,failures,calls,mode,takes,waits;
static common_rpc_connection expected;
static HANDLE exported[3];
#define CHECK(x) do {++checks;if(!(x)){++failures;printf("FAIL %u %s\n",__LINE__,#x);}} while(0)
static void peer(handle_t binding,VDM_CONNECTION connection,HANDLE process,DWORD generation)
{
    ++calls;
    CHECK(binding==expected.binding && connection==expected.connection &&
        process==expected.process && generation==expected.generation);
}
static error_status_t result(void)
{
    RpcRaiseException(mode==1 ? ERROR_ACCESS_DENIED : RPC_S_CALL_FAILED);
    return ERROR_SUCCESS;
}
static HANDLE attachment(unsigned index)
{
    exported[index]=mode==3 ? NULL : CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(mode==3 || exported[index]!=NULL);return exported[index];
}
#define EVENT_CALL(name) error_status_t Client_##name(PRPC_ASYNC_STATE async,handle_t binding,VDM_CONNECTION connection, \
    HANDLE process,unsigned long generation,HANDLE *output) { \
    peer(binding,connection,process,generation);CHECK(async!=NULL);*output=attachment(0); \
    RpcRaiseException(mode==1 ? ERROR_ACCESS_DENIED : RPC_S_CALL_FAILED);return ERROR_SUCCESS;}
EVENT_CALL(WorkerFrontendCapability)
EVENT_CALL(WorkerIoReleaseEvent)
error_status_t Client_WorkerIoCheckpoint(PRPC_ASYNC_STATE async,handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,unsigned long reason,unsigned long request,unsigned long *decision)
{
    peer(binding,connection,process,generation);
    CHECK(reason==WORKER_IO_CHECKPOINT_COMPLETE && request==42);
    *decision=mode==3 ? 99 : mode==4 ? WORKER_IO_KEEP : WORKER_IO_RELEASE;
    return result();
}
error_status_t Client_WorkerIoTransition(PRPC_ASYNC_STATE async,handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,unsigned long action)
{
    peer(binding,connection,process,generation);CHECK(action==1);return result();
}
error_status_t Client_FrontendIoDisconnected(PRPC_ASYNC_STATE async,handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation)
{
    peer(binding,connection,process,generation);CHECK(async!=NULL);
    RpcRaiseException(mode==1 ? ERROR_ACCESS_DENIED : RPC_S_CALL_FAILED);
    return ERROR_SUCCESS;
}
error_status_t Client_AcquireConsoleContext(PRPC_ASYNC_STATE async,handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,HANDLE frontend,HANDLE *capability)
{
    peer(binding,connection,process,generation);CHECK(frontend==expected.process);
    *capability=attachment(0);return result();
}
error_status_t Client_BindConsoleContext(PRPC_ASYNC_STATE async,handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,HANDLE capability)
{
    peer(binding,connection,process,generation);CHECK(capability==expected.process);return result();
}
error_status_t Client_RegisterNativeBackend(PRPC_ASYNC_STATE async,handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,HANDLE frontend)
{
    peer(binding,connection,process,generation);
    CHECK(frontend==expected.process);return result();
}
static error_status_t route(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    unsigned long generation,HANDLE *pipe,HANDLE *frontend,unsigned long *root_generation,HANDLE *ready)
{
    peer(binding,connection,process,generation);*pipe=attachment(0);
    *frontend=attachment(1);*ready=attachment(2);*root_generation=42;return result();
}
error_status_t Client_TakeFrontend(PRPC_ASYNC_STATE async,handle_t binding,VDM_CONNECTION connection,HANDLE process,
    unsigned long generation,HANDLE *pipe,HANDLE *frontend,unsigned long *root_generation,HANDLE *ready)
{
    ++takes;return route(binding,connection,process,generation,pipe,frontend,root_generation,ready);
}
error_status_t Client_WaitFrontend(PRPC_ASYNC_STATE async,handle_t binding,VDM_CONNECTION connection,HANDLE process,
    unsigned long generation,HANDLE *pipe,HANDLE *frontend,unsigned long *root_generation,HANDLE *ready)
{
    ++waits;return route(binding,connection,process,generation,pipe,frontend,root_generation,ready);
}
static void closed(unsigned count)
{
    unsigned i;DWORD flags;
    for(i=0;i<count;++i)CHECK(!GetHandleInformation(exported[i],&flags));
}
typedef DWORD (*event_client)(const common_rpc_connection *,HANDLE *);
int main(void)
{
    DWORD before=0,after=0,generation,want,error;HANDLE output,pipe,frontend,ready;
    unsigned i,initial;
    RPC_ASYNC_STATE runtime;
    common_rpc_connection invalid={0};
    /* Exclude RPC's first-use process resources, not per-call resources. */
    CHECK(RpcAsyncInitializeHandle(&runtime,sizeof(runtime))==RPC_S_OK);
    event_client events[]={common_rpc_worker_frontend_capability,
        common_rpc_worker_io_release_event};
    expected.binding=(RPC_BINDING_HANDLE)(ULONG_PTR)11;
    expected.connection=(VDM_CONNECTION)(ULONG_PTR)12;
    expected.process=CreateEventW(NULL,TRUE,FALSE,NULL);expected.generation=13;
    CHECK(expected.process && GetProcessHandleCount(GetCurrentProcess(),&before));
    for(i=0;i<ARRAYSIZE(events);++i) {
        CHECK(events[i](&expected,NULL)==ERROR_INVALID_PARAMETER);
        output=expected.process;CHECK(events[i](NULL,&output)==ERROR_INVALID_STATE && !output);
        output=expected.process;CHECK(events[i](&invalid,&output)==ERROR_INVALID_STATE && !output);
    }
    CHECK(common_rpc_acquire_console_context(&expected,NULL,NULL)==ERROR_INVALID_PARAMETER);
    output=expected.process;
    CHECK(common_rpc_acquire_console_context(NULL,NULL,&output)==ERROR_INVALID_STATE && !output);
    CHECK(common_rpc_bind_console_context(NULL,NULL)==ERROR_INVALID_STATE);
    CHECK(common_rpc_register_native_backend(NULL,NULL)==ERROR_INVALID_STATE);
    CHECK(common_rpc_worker_io_transition(NULL,1)==ERROR_INVALID_STATE);
    CHECK(common_rpc_worker_io_checkpoint(&expected,2,42,NULL)==ERROR_INVALID_PARAMETER);
    generation=99;
    CHECK(common_rpc_worker_io_checkpoint(NULL,2,42,&generation)==ERROR_INVALID_STATE && generation==WORKER_IO_KEEP);
    CHECK(common_rpc_frontend_io_disconnected(NULL)==ERROR_INVALID_STATE);
    CHECK(common_rpc_take_frontend(&expected,NULL,&frontend,&generation,&ready,FALSE)==ERROR_INVALID_PARAMETER);
    pipe=frontend=ready=expected.process;generation=99;
    CHECK(common_rpc_take_frontend(NULL,&pipe,&frontend,&generation,&ready,TRUE)==ERROR_INVALID_STATE);
    CHECK(!pipe && !frontend && !ready && !generation && !calls);
    for(mode=1;mode<3;++mode) {
        generation=99;
        want=mode==1 ? ERROR_ACCESS_DENIED : RPC_S_CALL_FAILED;
        CHECK(common_rpc_worker_io_checkpoint(&expected,WORKER_IO_CHECKPOINT_COMPLETE,42,&generation)==want);
        CHECK(generation==WORKER_IO_KEEP);
    }
    for(mode=1;mode<3;++mode) {
        initial=calls;want=mode==1 ? ERROR_ACCESS_DENIED : RPC_S_CALL_FAILED;
        /* Successful async replies use the actual NTSRV/runtime tests.
         * These stubs assert issuance exceptions and attachment cleanup. */
        for(i=0;i<ARRAYSIZE(events);++i) {
            CHECK(events[i](&expected,&output)==want);
            CHECK(!output);closed(1);
        }
        CHECK(common_rpc_acquire_console_context(&expected,expected.process,&output)==want);
        CHECK(!output);closed(1);
        CHECK(common_rpc_bind_console_context(&expected,expected.process)==want);
        CHECK(common_rpc_register_native_backend(&expected,expected.process)==want);
        CHECK(common_rpc_worker_io_transition(&expected,1)==want);
        /* Actual RPC/CAF exercises successful completion; this stub only
         * covers issuance failure and notification-event cleanup. */
        CHECK(common_rpc_frontend_io_disconnected(&expected)==want);
        for(i=0;i<2;++i) {
            generation=99;
            error=common_rpc_take_frontend(&expected,&pipe,&frontend,&generation,&ready,i!=0);
            CHECK(error==want);
            CHECK(!pipe && !frontend && !ready && !generation);closed(3);
        }
        CHECK(calls==initial+ARRAYSIZE(events)+7);
    }
    CHECK(takes==2 && waits==2);
    mode=2;expected.binding=(RPC_BINDING_HANDLE)(ULONG_PTR)21;
    expected.connection=(VDM_CONNECTION)(ULONG_PTR)22;expected.generation=23;
    CHECK(common_rpc_worker_io_release_event(&expected,&output)==RPC_S_CALL_FAILED && !output);closed(1);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after) && before==after);
    CHECK(WaitForSingleObject(expected.process,0)==WAIT_TIMEOUT);CloseHandle(expected.process);
    printf("%s common worker-control RPC %u checks %u failures; handle delta=%ld\n",
        failures ? "FAIL" : "PASS",checks,failures,(LONG)after-(LONG)before);
    return failures ? 1 : 0;
}
