#include "dos_observation.h"
#include "async_call.h"
#include "common/protocol/version.h"
#include <string.h>
DWORD common_rpc_observe_dos_event(const common_rpc_connection *state,
    const common_dos_observation *fact,BOOL gap,HANDLE stop)
{
    RPC_ASYNC_STATE async={0};DOS_OBSERVATION_FACT copied;
    HANDLE completed;DWORD error,reply=ERROR_INVALID_STATE;BOOL issued=FALSE;
    static unsigned char application[APP_VERSION_BYTES]=APP_VERSION;
    typedef char fact_abi[sizeof(copied)==sizeof(*fact)?1:-1];
    if(!state || !state->binding || !state->connection || !state->process || !fact || !stop)
        return ERROR_INVALID_PARAMETER;
    if(WaitForSingleObject(stop,0)==WAIT_OBJECT_0)return ERROR_OPERATION_ABORTED;
    memcpy(&copied,fact,sizeof(copied));
    error=RpcAsyncInitializeHandle(&async,sizeof(async));if(error)return error;
    completed=CreateEventW(NULL,TRUE,FALSE,NULL);if(!completed)return GetLastError();
    async.NotificationType=RpcNotificationTypeEvent;async.u.hEvent=completed;
    RpcTryExcept {
        Client_ObserveDosEventAsync(&async,state->binding,state->connection,state->process,
            state->generation,APP_PROTOCOL_VERSION,application,&copied,gap ? 1u : 0u);
        issued=TRUE;error=ERROR_SUCCESS;
    }
    RpcExcept(1) {error=RpcExceptionCode();}
    RpcEndExcept
    if(issued){error=common_rpc_finish_async(&async,completed,stop,10000,&reply);if(!error)error=reply;}
    CloseHandle(completed);return error;
}
