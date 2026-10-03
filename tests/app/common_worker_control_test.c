#include "common/rpc/worker_control.h"
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
    if(mode==2)RpcRaiseException(RPC_S_CALL_FAILED);
    return mode==1 ? ERROR_ACCESS_DENIED : ERROR_SUCCESS;
}
static HANDLE attachment(unsigned index)
{
    exported[index]=mode==3 ? NULL : CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(mode==3 || exported[index]!=NULL);return exported[index];
}
#define EVENT_CALL(name) error_status_t Client_##name(handle_t binding,VDM_CONNECTION connection, \
    HANDLE process,unsigned long generation,HANDLE *output) { \
    peer(binding,connection,process,generation);*output=attachment(0);return result();}
EVENT_CALL(WorkerFrontendCapability)
EVENT_CALL(WorkerShutdownEvent)
EVENT_CALL(WorkerStateChanged)
error_status_t Client_AcquireConsoleContext(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,HANDLE frontend,HANDLE *capability)
{
    peer(binding,connection,process,generation);CHECK(frontend==expected.process);
    *capability=attachment(0);return result();
}
error_status_t Client_BindConsoleContext(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,HANDLE capability)
{
    peer(binding,connection,process,generation);CHECK(capability==expected.process);return result();
}
error_status_t Client_RegisterNativeBackend(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,HANDLE frontend,HANDLE stop,HANDLE closed)
{
    peer(binding,connection,process,generation);
    CHECK(frontend==expected.process && stop==expected.process && closed==NULL);return result();
}
static error_status_t route(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    unsigned long generation,HANDLE *pipe,HANDLE *frontend,unsigned long *root_generation,HANDLE *ready)
{
    peer(binding,connection,process,generation);*pipe=attachment(0);
    *frontend=attachment(1);*ready=attachment(2);*root_generation=42;return result();
}
error_status_t Client_TakeFrontend(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    unsigned long generation,HANDLE *pipe,HANDLE *frontend,unsigned long *root_generation,HANDLE *ready)
{
    ++takes;return route(binding,connection,process,generation,pipe,frontend,root_generation,ready);
}
error_status_t Client_WaitFrontend(handle_t binding,VDM_CONNECTION connection,HANDLE process,
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
    common_rpc_connection invalid={0};
    event_client events[]={common_rpc_worker_frontend_capability,
        common_rpc_worker_shutdown_event,common_rpc_worker_state_changed};
    expected.binding=(RPC_BINDING_HANDLE)(ULONG_PTR)11;
    expected.connection=(VDM_CONNECTION)(ULONG_PTR)12;
    expected.process=CreateEventW(NULL,TRUE,FALSE,NULL);expected.generation=13;
    CHECK(expected.process && GetProcessHandleCount(GetCurrentProcess(),&before));
    for(i=0;i<3;++i) {
        CHECK(events[i](&expected,NULL)==ERROR_INVALID_PARAMETER);
        output=expected.process;CHECK(events[i](NULL,&output)==ERROR_INVALID_STATE && !output);
        output=expected.process;CHECK(events[i](&invalid,&output)==ERROR_INVALID_STATE && !output);
    }
    CHECK(common_rpc_acquire_console_context(&expected,NULL,NULL)==ERROR_INVALID_PARAMETER);
    output=expected.process;
    CHECK(common_rpc_acquire_console_context(NULL,NULL,&output)==ERROR_INVALID_STATE && !output);
    CHECK(common_rpc_bind_console_context(NULL,NULL)==ERROR_INVALID_STATE);
    CHECK(common_rpc_register_native_backend(NULL,NULL,NULL,NULL)==ERROR_INVALID_STATE);
    CHECK(common_rpc_take_frontend(&expected,NULL,&frontend,&generation,&ready,FALSE)==ERROR_INVALID_PARAMETER);
    pipe=frontend=ready=expected.process;generation=99;
    CHECK(common_rpc_take_frontend(NULL,&pipe,&frontend,&generation,&ready,TRUE)==ERROR_INVALID_STATE);
    CHECK(!pipe && !frontend && !ready && !generation && !calls);
    for(mode=0;mode<3;++mode) {
        initial=calls;want=mode==0 ? 0 : mode==1 ? ERROR_ACCESS_DENIED : RPC_S_CALL_FAILED;
        for(i=0;i<3;++i) {
            CHECK(events[i](&expected,&output)==want);
            if(want){CHECK(!output);closed(1);}else {CHECK(output!=NULL);CloseHandle(output);}
        }
        CHECK(common_rpc_acquire_console_context(&expected,expected.process,&output)==want);
        if(want){CHECK(!output);closed(1);}else {CHECK(output!=NULL);CloseHandle(output);}
        CHECK(common_rpc_bind_console_context(&expected,expected.process)==want);
        CHECK(common_rpc_register_native_backend(&expected,expected.process,expected.process,NULL)==want);
        for(i=0;i<2;++i) {
            generation=99;
            error=common_rpc_take_frontend(&expected,&pipe,&frontend,&generation,&ready,i!=0);
            CHECK(error==want);
            if(want){CHECK(!pipe && !frontend && !ready && !generation);closed(3);}
            else {CHECK(pipe && frontend && ready && generation==42);
                CloseHandle(pipe);CloseHandle(frontend);CloseHandle(ready);}
        }
        CHECK(calls==initial+8);
    }
    CHECK(takes==3 && waits==3);
    mode=3;
    CHECK(common_rpc_worker_shutdown_event(&expected,&output)==ERROR_INVALID_HANDLE && !output);
    CHECK(common_rpc_worker_state_changed(&expected,&output)==ERROR_INVALID_HANDLE && !output);
    /* Capability exchanges preserve the prior provider-status contract;
     * only the notification APIs require a non-null successful attachment. */
    CHECK(!common_rpc_worker_frontend_capability(&expected,&output) && !output);
    CHECK(!common_rpc_acquire_console_context(&expected,expected.process,&output) && !output);
    mode=0;expected.binding=(RPC_BINDING_HANDLE)(ULONG_PTR)21;
    expected.connection=(VDM_CONNECTION)(ULONG_PTR)22;expected.generation=23;
    CHECK(!common_rpc_worker_shutdown_event(&expected,&output));CloseHandle(output);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after) && before==after);
    CHECK(WaitForSingleObject(expected.process,0)==WAIT_TIMEOUT);CloseHandle(expected.process);
    printf("%s common worker-control RPC %u checks %u failures; handle delta=%ld\n",
        failures ? "FAIL" : "PASS",checks,failures,(LONG)after-(LONG)before);
    return failures ? 1 : 0;
}
