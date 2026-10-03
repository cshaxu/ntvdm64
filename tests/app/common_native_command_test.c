#include "common/rpc/native_command.h"
#include "common/codec/native_launch.h"
#include <stdio.h>

static unsigned checks,failures,calls;
static DWORD injected;
static BOOL raise_error;
static HANDLE exported[3];
static common_rpc_connection expected;
static DWORD completed_value=1;
#define CHECK(x) do {++checks;if(!(x)){++failures;printf("FAIL %u %s\n",__LINE__,#x);}} while(0)
static void peer(handle_t binding,VDM_CONNECTION connection,HANDLE process,DWORD generation)
{
    ++calls;
    CHECK(binding==expected.binding && connection==expected.connection &&
        process==expected.process && generation==expected.generation);
}
error_status_t Client_GetNextNativeCommand(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,unsigned long capacity,byte *payload,
    unsigned long *bytes,HANDLE *sender,HANDLE *execution,HANDLE *frontend,
    unsigned long *request,unsigned long *caller_generation)
{
    unsigned i;
    peer(binding,connection,process,generation);
    CHECK(capacity>=4);
    for(i=0;i<3;++i){exported[i]=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(exported[i]!=NULL);}
    *sender=exported[0];*execution=exported[1];*frontend=exported[2];
    CopyMemory(payload,"RPC!",4);*bytes=4;*request=71;*caller_generation=81;
    if(raise_error)RpcRaiseException(injected);
    return injected;
}
error_status_t Client_NativeStartupResult(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,unsigned long caller_generation,
    unsigned long request,unsigned long status,HANDLE target,HANDLE receipt)
{
    peer(binding,connection,process,generation);
    CHECK(caller_generation==81 && request==71 && status==ERROR_ACCESS_DENIED);
    CHECK(target==expected.process && receipt==NULL);
    if(raise_error)RpcRaiseException(injected);
    return injected;
}
error_status_t Client_CompleteWorkerChannel(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,unsigned long request,unsigned long exit_code,
    unsigned long io_error,unsigned long io_flags)
{
    peer(binding,connection,process,generation);
    CHECK(request==0 && exit_code==37 && io_error==ERROR_BROKEN_PIPE && io_flags==2);
    if(raise_error)RpcRaiseException(injected);
    return injected;
}

error_status_t Client_SubmitNativeRequest(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,HANDLE frontend,unsigned long bytes,
    byte *payload,HANDLE *target,HANDLE *receipt,unsigned long *request)
{
    peer(binding,connection,process,generation);
    CHECK(frontend==expected.process && bytes==4 && !memcmp(payload,"RPC!",4));
    exported[0]=CreateEventW(NULL,TRUE,FALSE,NULL);
    exported[1]=CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(exported[0] && exported[1]);
    *target=exported[0];*receipt=exported[1];*request=71;
    if(raise_error)RpcRaiseException(injected);
    return injected;
}

error_status_t Client_FinishNativeRequest(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation,unsigned long request,unsigned long *exit_code,
    unsigned long *target_completed)
{
    peer(binding,connection,process,generation);
    CHECK(request==71);
    *exit_code=37;*target_completed=completed_value;
    if(raise_error)RpcRaiseException(injected);
    return injected;
}

int main(void)
{
    BYTE payload[4];
    DWORD bytes=99,request=99,generation=99,before,after,exit_code,completed;
    HANDLE sender=NULL,execution=NULL,frontend=NULL;
    unsigned i,j;
    common_rpc_connection empty={0};
    expected.binding=(RPC_BINDING_HANDLE)(ULONG_PTR)1;
    expected.connection=(VDM_CONNECTION)(ULONG_PTR)2;
    expected.process=GetCurrentProcess();expected.generation=3;
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
    CHECK(common_rpc_next_native_command(&expected,0,payload,&bytes,&sender,&execution,&frontend,&request,&generation)==ERROR_INVALID_PARAMETER);
    CHECK(common_rpc_next_native_command(&expected,NATIVE_LAUNCH_MAX_BYTES+1,payload,&bytes,&sender,&execution,&frontend,&request,&generation)==ERROR_INVALID_PARAMETER);
    CHECK(common_rpc_next_native_command(&expected,4,payload,&bytes,&sender,&execution,NULL,&request,&generation)==ERROR_INVALID_PARAMETER);
    CHECK(common_rpc_next_native_command(&empty,4,payload,&bytes,&sender,&execution,&frontend,&request,&generation)==ERROR_INVALID_STATE);
    CHECK(!bytes && !sender && !execution && !frontend && !request && !generation && !calls);
    CHECK(common_rpc_native_startup_result(NULL,81,71,5,expected.process,NULL)==ERROR_INVALID_STATE);
    CHECK(common_rpc_complete_native_command(&empty,0,37,109,2)==ERROR_INVALID_STATE && !calls);
    CHECK(!common_rpc_next_native_command(&expected,4,payload,&bytes,&sender,&execution,&frontend,&request,&generation));
    CHECK(bytes==4 && !memcmp(payload,"RPC!",4) && request==71 && generation==81);
    CHECK(sender==exported[0] && execution==exported[1] && frontend==exported[2]);
    for(i=0;i<3;++i){CHECK(WaitForSingleObject(exported[i],0)==WAIT_TIMEOUT);CloseHandle(exported[i]);}
    for(j=0;j<2;++j) {
        raise_error=j!=0;injected=j ? RPC_S_SERVER_UNAVAILABLE : ERROR_ACCESS_DENIED;
        CHECK(common_rpc_next_native_command(&expected,4,payload,&bytes,&sender,&execution,&frontend,&request,&generation)==injected);
        CHECK(!bytes && !sender && !execution && !frontend && !request && !generation);
        for(i=0;i<3;++i)CHECK(WaitForSingleObject(exported[i],0)==WAIT_FAILED);
        CHECK(common_rpc_native_startup_result(&expected,81,71,ERROR_ACCESS_DENIED,expected.process,NULL)==injected);
        CHECK(common_rpc_complete_native_command(&expected,0,37,ERROR_BROKEN_PIPE,2)==injected);
    }
    raise_error=FALSE;injected=0;
    CHECK(!common_rpc_native_startup_result(&expected,81,71,ERROR_ACCESS_DENIED,expected.process,NULL));
    CHECK(!common_rpc_complete_native_command(&expected,0,37,ERROR_BROKEN_PIPE,2));
    CHECK(common_rpc_submit_native_request(&expected,expected.process,4,payload,NULL,&execution,&request)==ERROR_INVALID_PARAMETER);
    CHECK(common_rpc_submit_native_request(&empty,expected.process,4,payload,&sender,&execution,&request)==ERROR_INVALID_STATE);
    CHECK(!sender && !execution && !request);
    CHECK(common_rpc_finish_native_request(&expected,0,&exit_code,&completed)==ERROR_INVALID_PARAMETER);
    CHECK(common_rpc_finish_native_request(&expected,71,NULL,&completed)==ERROR_INVALID_PARAMETER);
    CHECK(common_rpc_finish_native_request(&empty,71,&exit_code,&completed)==ERROR_INVALID_STATE);
    CHECK(!exit_code && !completed);
    CopyMemory(payload,"RPC!",4);
    CHECK(!common_rpc_submit_native_request(&expected,expected.process,4,payload,&sender,&execution,&request));
    CHECK(sender==exported[0] && execution==exported[1] && request==71);
    CHECK(WaitForSingleObject(sender,0)==WAIT_TIMEOUT && WaitForSingleObject(execution,0)==WAIT_TIMEOUT);
    CloseHandle(sender);CloseHandle(execution);
    CHECK(!common_rpc_finish_native_request(&expected,71,&exit_code,&completed));
    CHECK(exit_code==37 && completed==1);
    completed_value=0;
    CHECK(!common_rpc_finish_native_request(&expected,71,&exit_code,&completed));
    CHECK(exit_code==37 && !completed);
    completed_value=2;
    CHECK(common_rpc_finish_native_request(&expected,71,&exit_code,&completed)==ERROR_INVALID_DATA);
    CHECK(!exit_code && !completed);
    completed_value=1;
    for(j=0;j<2;++j) {
        raise_error=j!=0;injected=j ? RPC_S_SERVER_UNAVAILABLE : ERROR_ACCESS_DENIED;
        CHECK(common_rpc_submit_native_request(&expected,expected.process,4,payload,&sender,&execution,&request)==injected);
        CHECK(!sender && !execution && !request);
        for(i=0;i<2;++i)CHECK(WaitForSingleObject(exported[i],0)==WAIT_FAILED);
        CHECK(common_rpc_finish_native_request(&expected,71,&exit_code,&completed)==injected);
        /* Preserve the existing facade's exact contract: an RPC exception
         * clears partial scalar outputs; an ordinary status is forwarded. */
        CHECK(raise_error ? (!exit_code && !completed) : (exit_code==37 && completed==1));
    }
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after) && before==after);
    printf("COMMON-NATIVE-RPC checks=%u failures=%u handle-delta=%ld\n",checks,failures,(LONG)after-(LONG)before);
    return failures ? 1 : 0;
}
