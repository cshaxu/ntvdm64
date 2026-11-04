#ifndef COMMON_RPC_ASYNC_CALL_H
#define COMMON_RPC_ASYNC_CALL_H
#include <windows.h>
#include <rpc.h>
#include <rpcasync.h>
/* Zero denotes an unbounded operation, not a fresh per-phase budget. */
static __inline DWORD common_rpc_remaining_ms(ULONGLONG deadline)
{
    ULONGLONG now=GetTickCount64();
    return !deadline ? INFINITE : now>=deadline ? 0 : (DWORD)min(deadline-now,MAXDWORD-1u);
}
static __inline DWORD common_rpc_begin_async(RPC_ASYNC_STATE *call)
{
    DWORD error;
    ZeroMemory(call,sizeof(*call));
    error=RpcAsyncInitializeHandle(call,sizeof(*call));if(error)return error;
    call->NotificationType=RpcNotificationTypeEvent;
    call->u.hEvent=CreateEventW(NULL,TRUE,FALSE,NULL);
    return call->u.hEvent ? ERROR_SUCCESS : GetLastError();
}
/* The issuer owns call inputs, async state and notification event until this
 * returns. Abort is followed by real notification/Complete, not detached
 * stack storage. Optional stop takes priority over a simultaneous reply. */
static __inline DWORD common_rpc_finish_async(RPC_ASYNC_STATE *call,HANDLE completed,
    HANDLE stop,DWORD timeout,DWORD *reply)
{
    HANDLE waits[2]={stop,completed};DWORD wait,error=ERROR_SUCCESS,result;
    wait=stop ? WaitForMultipleObjects(2,waits,FALSE,timeout) : WaitForSingleObject(completed,timeout);
    if(wait!=(stop ? WAIT_OBJECT_0+1 : WAIT_OBJECT_0)) {
        error=wait==WAIT_TIMEOUT ? ERROR_TIMEOUT :
            stop && wait==WAIT_OBJECT_0 ? ERROR_OPERATION_ABORTED : GetLastError();
        result=RpcAsyncCancelCall(call,TRUE);
        if(result)error=result;
        /* Cancellation failure still does not permit releasing pending
         * marshalling buffers. Drain completion even on that failure. */
        if(WaitForSingleObject(completed,INFINITE)!=WAIT_OBJECT_0)
            RaiseFailFastException(NULL,NULL,0);
    }
    result=RpcAsyncCompleteCall(call,reply);
    return error ? error : result;
}
/* A synchronous stub exception means no async call was issued. In either
 * case release the event exactly once, after any issued call has completed. */
static __inline DWORD common_rpc_end_async(RPC_ASYNC_STATE *call,DWORD error,
    HANDLE stop,DWORD timeout)
{
    DWORD reply=ERROR_INVALID_STATE;
    if(!error) {
        error=common_rpc_finish_async(call,call->u.hEvent,stop,timeout,&reply);
        if(!error)error=reply;
    }
    CloseHandle(call->u.hEvent);
    return error;
}
#endif
