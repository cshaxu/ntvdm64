#include "common/protocol/frontend_instance.h"
static const common_frontend_key frontend_key={0x123456789ULL,17};
#include "common/rpc/frontend_control.h"
#include <stdio.h>

static unsigned checks,failures,calls,mode;
static common_rpc_connection expected;
static HANDLE attachments[3];
#define CHECK(x) do {++checks;if(!(x)){++failures;printf("FAIL %u %s\n",__LINE__,#x);}} while(0)
static void peer(handle_t binding,VDM_CONNECTION connection,HANDLE process,DWORD generation)
{
    ++calls;
    CHECK(binding==expected.binding && connection==expected.connection &&
        process==expected.process && generation==expected.generation);
}
static HANDLE attachment(unsigned index)
{
    attachments[index]=CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(attachments[index]!=NULL);
    return attachments[index];
}
error_status_t Client_StartFrontend(PRPC_ASYNC_STATE async,handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,__int64 window,unsigned long borrowed,
    HANDLE *root,HANDLE *capability,HANDLE *restored)
{
    peer(binding,connection,process,generation);CHECK(async && window==0x123456789LL && borrowed==1);
    *root=attachment(0);*capability=attachment(1);*restored=attachment(2);
    RpcRaiseException(mode==1 ? ERROR_ACCESS_DENIED : RPC_S_CALL_FAILED);return ERROR_SUCCESS;
}
error_status_t Client_RetireWorkerlessFrontend(PRPC_ASYNC_STATE async,handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,LONGLONG instance,unsigned long frontend_generation,unsigned long *retired)
{
    CHECK((uint64_t)instance==frontend_key.instance && frontend_generation==frontend_key.generation);
    peer(binding,connection,process,generation);CHECK(async!=NULL);*retired=1;
    RpcRaiseException(mode==1 ? ERROR_ACCESS_DENIED : RPC_S_CALL_FAILED);
    return ERROR_SUCCESS;
}
error_status_t Client_FrontendStateChanged(PRPC_ASYNC_STATE async,handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,LONGLONG instance,unsigned long frontend_generation,HANDLE *event)
{
    CHECK((uint64_t)instance==frontend_key.instance && frontend_generation==frontend_key.generation);
    peer(binding,connection,process,generation);CHECK(async!=NULL);*event=attachment(0);
    RpcRaiseException(mode==1 ? ERROR_ACCESS_DENIED : RPC_S_CALL_FAILED);return ERROR_SUCCESS;
}
error_status_t Client_FrontendStartupResult(PRPC_ASYNC_STATE async,handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,HANDLE capability,unsigned long status)
{
    peer(binding,connection,process,generation);
    CHECK(async && capability==expected.process && status==ERROR_WRITE_FAULT);
    RpcRaiseException(mode==1 ? ERROR_ACCESS_DENIED : RPC_S_CALL_FAILED);return ERROR_SUCCESS;
}
/* Completion and positive replies belong to actual RPC/CAF tests. These
 * stubs exercise synchronous issuance exceptions and local event cleanup. */
#define NO_OUTPUT(name) error_status_t Client_##name(PRPC_ASYNC_STATE async,handle_t binding,VDM_CONNECTION connection, \
    HANDLE process,unsigned long generation) {peer(binding,connection,process,generation);CHECK(async!=NULL); \
    RpcRaiseException(mode==1 ? ERROR_ACCESS_DENIED : RPC_S_CALL_FAILED);return ERROR_SUCCESS;}
error_status_t Client_FrontendConsoleRestored(PRPC_ASYNC_STATE async,handle_t binding,
    VDM_CONNECTION connection,HANDLE process,unsigned long generation,LONGLONG instance,
    unsigned long frontend_generation)
{
    peer(binding,connection,process,generation);CHECK(async!=NULL);
    CHECK((uint64_t)instance==frontend_key.instance && frontend_generation==frontend_key.generation);
    RpcRaiseException(mode==1 ? ERROR_ACCESS_DENIED : RPC_S_CALL_FAILED);return ERROR_SUCCESS;
}
NO_OUTPUT(WaitFrontendConsoleRestored)

error_status_t Client_FrontendIoDisconnected(PRPC_ASYNC_STATE async,handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,LONGLONG instance,unsigned long frontend_generation)
{
    CHECK((uint64_t)instance==frontend_key.instance && frontend_generation==frontend_key.generation);
    peer(binding,connection,process,generation);CHECK(async!=NULL);
    RpcRaiseException(mode==1 ? ERROR_ACCESS_DENIED : RPC_S_CALL_FAILED);
    return ERROR_SUCCESS;
}

static void closed(unsigned count)
{
    unsigned i;DWORD flags;
    for(i=0;i<count;++i)CHECK(!GetHandleInformation(attachments[i],&flags));
}
int main(void)
{
    DWORD before=0,after=0,retired,want;
    HANDLE root,capability,restored,event;unsigned initial;
    RPC_ASYNC_STATE runtime;
    /* Initialize RPC's process-wide resources before measuring per-call
     * attachment and notification cleanup. No project call is warmed up. */
    CHECK(RpcAsyncInitializeHandle(&runtime,sizeof(runtime))==RPC_S_OK);
    expected.binding=(RPC_BINDING_HANDLE)(ULONG_PTR)11;
    expected.connection=(VDM_CONNECTION)(ULONG_PTR)12;
    expected.process=CreateEventW(NULL,TRUE,FALSE,NULL);expected.generation=13;
    CHECK(expected.process && GetProcessHandleCount(GetCurrentProcess(),&before));
    root=capability=restored=expected.process;
    CHECK(common_rpc_start_frontend(NULL,0,FALSE,&root,&capability,&restored)==ERROR_INVALID_STATE);
    CHECK(!root && !capability && !restored);
    CHECK(common_rpc_start_frontend(&expected,0,FALSE,NULL,&capability,&restored)==ERROR_INVALID_PARAMETER);
    CHECK(common_rpc_retire_workerless_frontend(&expected,&frontend_key,NULL)==ERROR_INVALID_PARAMETER);
    CHECK(common_rpc_frontend_state_changed(&expected,&frontend_key,NULL)==ERROR_INVALID_PARAMETER);
    retired=77;event=expected.process;
    CHECK(common_rpc_retire_workerless_frontend(NULL,&frontend_key,&retired)==ERROR_INVALID_STATE && retired==77);
    CHECK(common_rpc_frontend_state_changed(NULL,&frontend_key,&event)==ERROR_INVALID_STATE && !event);
    CHECK(common_rpc_frontend_console_restored(NULL,&frontend_key)==ERROR_INVALID_STATE);
    CHECK(common_rpc_frontend_startup_result(NULL,NULL,0)==ERROR_INVALID_STATE);
    CHECK(common_rpc_wait_frontend_console_restored(NULL)==ERROR_INVALID_STATE);
    CHECK(common_rpc_frontend_io_disconnected(NULL,&frontend_key)==ERROR_INVALID_STATE);
    CHECK(!calls);
    for(mode=1;mode<3;++mode) {
        initial=calls;want=mode==1 ? ERROR_ACCESS_DENIED : RPC_S_CALL_FAILED;
        CHECK(common_rpc_start_frontend(&expected,0x123456789ULL,17,&root,&capability,&restored)==want);
        CHECK(!root && !capability && !restored);closed(3);
        retired=77;
        CHECK(common_rpc_retire_workerless_frontend(&expected,&frontend_key,&retired)==want);CHECK(retired==77);
        CHECK(common_rpc_frontend_state_changed(&expected,&frontend_key,&event)==want);
        CHECK(!event);closed(1);
        CHECK(common_rpc_frontend_console_restored(&expected,&frontend_key)==want);
        CHECK(common_rpc_wait_frontend_console_restored(&expected)==want);
        CHECK(common_rpc_frontend_startup_result(&expected,expected.process,ERROR_WRITE_FAULT)==want);
        CHECK(common_rpc_frontend_io_disconnected(&expected,&frontend_key)==want);
        CHECK(calls==initial+7U);
    }
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after) && before==after);
    CHECK(WaitForSingleObject(expected.process,0)==WAIT_TIMEOUT);
    CloseHandle(expected.process);
    printf("%s common frontend-control RPC %u checks %u failures; handle delta=%ld\n",
        failures ? "FAIL" : "PASS",checks,failures,(LONG)after-(LONG)before);
    return failures ? 1 : 0;
}
