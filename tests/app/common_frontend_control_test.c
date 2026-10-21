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
static error_status_t result(void)
{
    if(mode==2)RpcRaiseException(RPC_S_CALL_FAILED);
    return mode==1 ? ERROR_ACCESS_DENIED : ERROR_SUCCESS;
}
static HANDLE attachment(unsigned index)
{
    attachments[index]=mode==3 ? NULL : CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(mode==3 || attachments[index]!=NULL);
    return attachments[index];
}
error_status_t Client_StartFrontend(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,__int64 window,unsigned long borrowed,
    HANDLE *root,HANDLE *capability,HANDLE *restored)
{
    peer(binding,connection,process,generation);CHECK(window==0x123456789LL && borrowed==1);
    *root=attachment(0);*capability=attachment(1);*restored=attachment(2);return result();
}
error_status_t Client_FrontendUsage(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,unsigned long *pending,unsigned long *tasks)
{
    peer(binding,connection,process,generation);*pending=3;*tasks=7;return result();
}
error_status_t Client_RetireWorkerlessFrontend(PRPC_ASYNC_STATE async,handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,unsigned long *retired)
{
    peer(binding,connection,process,generation);CHECK(async!=NULL);*retired=1;
    RpcRaiseException(mode==1 ? ERROR_ACCESS_DENIED : RPC_S_CALL_FAILED);
    return ERROR_SUCCESS;
}
error_status_t Client_FrontendStateChanged(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,HANDLE *event)
{
    peer(binding,connection,process,generation);*event=attachment(0);return result();
}
error_status_t Client_FrontendStartupResult(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,HANDLE capability,unsigned long status)
{
    peer(binding,connection,process,generation);
    CHECK(capability==expected.process && status==ERROR_WRITE_FAULT);return result();
}
/* Completion and positive replies belong to actual RPC/CAF tests. These
 * stubs exercise synchronous issuance exceptions and local event cleanup. */
#define NO_OUTPUT(name) error_status_t Client_##name(PRPC_ASYNC_STATE async,handle_t binding,VDM_CONNECTION connection, \
    HANDLE process,unsigned long generation) {peer(binding,connection,process,generation);CHECK(async!=NULL); \
    RpcRaiseException(mode==1 ? ERROR_ACCESS_DENIED : RPC_S_CALL_FAILED);return ERROR_SUCCESS;}
NO_OUTPUT(FrontendConsoleRestored)
NO_OUTPUT(WaitFrontendConsoleRestored)

static void closed(unsigned count)
{
    unsigned i;DWORD flags;
    for(i=0;i<count;++i)CHECK(!GetHandleInformation(attachments[i],&flags));
}
int main(void)
{
    DWORD before=0,after=0,pending,tasks,retired,want;
    HANDLE root,capability,restored,event;unsigned initial;
    RPC_ASYNC_STATE runtime;
    common_rpc_connection invalid={0};
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
    CHECK(common_rpc_frontend_usage(&expected,NULL,&tasks)==ERROR_INVALID_PARAMETER);
    CHECK(common_rpc_retire_workerless_frontend(&expected,NULL)==ERROR_INVALID_PARAMETER);
    CHECK(common_rpc_frontend_state_changed(&expected,NULL)==ERROR_INVALID_PARAMETER);
    pending=88;tasks=99;retired=77;event=expected.process;
    CHECK(common_rpc_frontend_usage(&invalid,&pending,&tasks)==ERROR_INVALID_STATE && pending==88 && tasks==99);
    CHECK(common_rpc_retire_workerless_frontend(NULL,&retired)==ERROR_INVALID_STATE && retired==77);
    CHECK(common_rpc_frontend_state_changed(NULL,&event)==ERROR_INVALID_STATE && !event);
    CHECK(common_rpc_frontend_console_restored(NULL)==ERROR_INVALID_STATE);
    CHECK(common_rpc_frontend_startup_result(NULL,NULL,0)==ERROR_INVALID_STATE);
    CHECK(common_rpc_wait_frontend_console_restored(NULL)==ERROR_INVALID_STATE);
    CHECK(!calls);
    for(mode=0;mode<3;++mode) {
        initial=calls;want=mode==0 ? 0 : mode==1 ? ERROR_ACCESS_DENIED : RPC_S_CALL_FAILED;
        CHECK(common_rpc_start_frontend(&expected,0x123456789ULL,17,&root,&capability,&restored)==want);
        if(want){CHECK(!root && !capability && !restored);closed(3);}
        else {CHECK(root && capability && restored);CloseHandle(root);CloseHandle(capability);CloseHandle(restored);}
        pending=88;tasks=99;retired=77;
        CHECK(common_rpc_frontend_usage(&expected,&pending,&tasks)==want);
        CHECK(want ? pending==88 && tasks==99 : pending==3 && tasks==7);
        if(want){CHECK(common_rpc_retire_workerless_frontend(&expected,&retired)==want);CHECK(retired==77);}
        CHECK(common_rpc_frontend_state_changed(&expected,&event)==want);
        if(want){CHECK(!event);closed(1);}else {CHECK(event!=NULL);CloseHandle(event);}
        if(want){
            CHECK(common_rpc_frontend_console_restored(&expected)==want);
            CHECK(common_rpc_wait_frontend_console_restored(&expected)==want);
        }
        CHECK(common_rpc_frontend_startup_result(&expected,expected.process,ERROR_WRITE_FAULT)==want);
        CHECK(calls==initial+(want ? 7U : 4U));
    }
    mode=3;event=expected.process;
    CHECK(common_rpc_frontend_state_changed(&expected,&event)==ERROR_INVALID_HANDLE && !event);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after) && before==after);
    CHECK(WaitForSingleObject(expected.process,0)==WAIT_TIMEOUT);
    CloseHandle(expected.process);
    printf("%s common frontend-control RPC %u checks %u failures; handle delta=%ld\n",
        failures ? "FAIL" : "PASS",checks,failures,(LONG)after-(LONG)before);
    return failures ? 1 : 0;
}
