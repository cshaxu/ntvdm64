#include "service.h"
#include "common/protocol/frontend_lifecycle.h"

struct frontend_base_service {
    HANDLE notification,stop,thread,retire,state_changed,broker;
    BOOL retire_requested,return_prepared;
    void *channel,*context;
    frontend_base_service_ops ops;
};
static DWORD frontend_retirement(frontend_base_service *scope,DWORD *action)
{
    DWORD error=scope->ops.retirement(scope->context,action);
    if(error)return error;
    switch(*action) {
    case OPENNT_FRONTEND_RETIRE_NONE:
    case OPENNT_FRONTEND_RETIRE_FINAL_CLOSE:return ERROR_SUCCESS;
    case OPENNT_FRONTEND_RETIRE_RETURN_CONSOLE:scope->return_prepared=TRUE;return ERROR_SUCCESS;
    case OPENNT_FRONTEND_RETIRE_PARK:scope->retire_requested=FALSE;return ERROR_SUCCESS;
    default:return ERROR_INVALID_DATA;
    }
}
static DWORD frontend_run(frontend_base_service *scope)
{
    HANDLE waits[7];
    DWORD error=ERROR_SUCCESS;
    for (;;) {
        DWORD wait_count=0,retire_index=MAXDWORD,broker_index=MAXDWORD,wait;
        waits[wait_count++]=scope->stop;
        if(scope->broker){broker_index=wait_count;waits[wait_count++]=scope->broker;}
        if(!scope->return_prepared)waits[wait_count++]=scope->notification;
        if(scope->retire && !scope->retire_requested && !scope->return_prepared) {
            retire_index=wait_count;waits[wait_count++]=scope->retire;
        }
        if(scope->state_changed) waits[wait_count++]=scope->state_changed;
        if(scope->channel && !scope->return_prepared)
            waits[wait_count++]=scope->ops.channel_thread(scope->context,scope->channel);
        wait=WaitForMultipleObjects(wait_count,waits,FALSE,INFINITE);
        if (wait==WAIT_OBJECT_0) break;
        if(broker_index!=MAXDWORD && wait==WAIT_OBJECT_0+broker_index)
            return ERROR_INVALID_HANDLE;
        if (wait<WAIT_OBJECT_0 || wait>=WAIT_OBJECT_0+wait_count) {
            return GetLastError();
        }
        /* Broker shutdown is authoritative even with a stale lease/pending
         * request. Check it before joins, channel work or lease restoration. */
        {
            DWORD retired=0;
            error=frontend_retirement(scope,&retired);
            if(error)return error;
            if(retired==OPENNT_FRONTEND_RETIRE_FINAL_CLOSE)return ERROR_SUCCESS;
            if(retired!=OPENNT_FRONTEND_RETIRE_NONE)continue;
        }
        /* The prepared close has frozen producer access. Its resulting pipe
         * EOF is not permission to finalize before NTSRV joins the workers.
         * Wait only for service state/stop, not the already-signalled pipe. */
        if(scope->return_prepared)continue;
        if(retire_index!=MAXDWORD && wait==WAIT_OBJECT_0+retire_index)
            scope->retire_requested=TRUE;
        error=scope->ops.prepare_requests(scope->context);
        if(error)return error;
        for (;;) {
            HANDLE worker=NULL;
            DWORD request=0;
            if (WaitForSingleObject(scope->stop,0)==WAIT_OBJECT_0) return ERROR_SUCCESS;
            error=scope->ops.request(scope->context,&request,&worker);
            if(error==ERROR_NOT_FOUND) {
                /* EOF is a transport failure unless NTSRV has authorized
                 * release. It cannot itself choose the next logical owner. */
                if(scope->channel && WaitForSingleObject(
                    scope->ops.channel_thread(scope->context,scope->channel),0)==WAIT_OBJECT_0)
                    return ERROR_PIPE_NOT_CONNECTED;
                break;
            }
            if(error)return error;
            if(!request && !worker) {
                /* NTSRV orders the endpoint closed after final I/O. Joining
                 * and disposal precede our acknowledgement, never vice versa. */
                error=scope->ops.channel_stop(scope->context,scope->channel);
                if(error)return error;
                scope->channel=NULL;
                error=scope->ops.disconnected(scope->context);
                if(error)return error;
                continue;
            }
            if(!request || !worker || scope->channel) {
                if(worker)CloseHandle(worker);
                return ERROR_INVALID_STATE;
            }
            error=scope->ops.channel_start(scope->context,request,worker,&scope->channel);
            if (error) {
                return error;
            }
        }
    }
    return ERROR_SUCCESS;
}

static DWORD WINAPI frontend_pump(void *context)
{
    frontend_base_service *scope=context;
    DWORD error=frontend_run(scope),action,step,wait;
    HANDLE waits[3]={scope->broker,scope->stop,scope->state_changed};
    /* The in-process fixture has no admitted service lifetime. Production
     * keeps the authenticated broker handle until this thread has joined. */
    if(!error || !scope->broker)return error;
    if(WaitForSingleObject(scope->broker,0)==WAIT_OBJECT_0)return ERROR_INVALID_HANDLE;
    /* Stop admitting I/O after a fault, but do not close the presentation or
     * report restoration before NTSRV's instruction. Submission is one-shot;
     * failure does not authorize local exit, reconnection or replay. */
    step=scope->ops.fault(scope->context);
    for(;;) {
        if(WaitForSingleObject(scope->broker,0)==WAIT_OBJECT_0)return ERROR_INVALID_HANDLE;
        if(!step) {
            step=frontend_retirement(scope,&action);
            if(!step && action==OPENNT_FRONTEND_RETIRE_FINAL_CLOSE)return error;
        }
        /* A failed control/restore operation must not be retried or converted
         * into a successful acknowledgement. Only service death permits local
         * cleanup when this control path can no longer execute instructions. */
        wait=WaitForMultipleObjects(step ? 1 : 3,waits,FALSE,INFINITE);
        if(wait==WAIT_OBJECT_0)return ERROR_INVALID_HANDLE;
        if(wait==WAIT_OBJECT_0+1)return error;
        if(wait!=WAIT_OBJECT_0+2)step=ERROR_INVALID_STATE;
    }
}


HANDLE frontend_base_service_thread(frontend_base_service *scope)
{return scope ? scope->thread : NULL;}
DWORD frontend_base_service_stop(frontend_base_service *scope)
{
    DWORD error,wait;
    if(!scope)return ERROR_SUCCESS;
    if(scope->stop)SetEvent(scope->stop);
    if(scope->thread) {
        wait=WaitForSingleObject(scope->thread,INFINITE);
        if(wait!=WAIT_OBJECT_0)return GetLastError();
        CloseHandle(scope->thread);scope->thread=NULL;
    }
    if(scope->channel) {
        error=scope->ops.channel_stop(scope->context,scope->channel);
        if(error)return error;
        scope->channel=NULL;
    }
    if(scope->stop)CloseHandle(scope->stop);
    HeapFree(GetProcessHeap(),0,scope);
    return ERROR_SUCCESS;
}
DWORD frontend_base_service_start(HANDLE notification,HANDLE retire,HANDLE broker,
    HANDLE state_changed,const frontend_base_service_ops *ops,void *context,frontend_base_service **output)
{
    frontend_base_service *scope;
    DWORD error;
    if(!output || !notification || (broker && !state_changed) || !ops ||
        !ops->retirement || !ops->prepare_requests || !ops->request ||
        !ops->channel_start || !ops->channel_stop || !ops->channel_thread ||
        !ops->disconnected || !ops->fault) {
        if(output)*output=NULL;
        return ERROR_INVALID_PARAMETER;
    }
    *output=NULL;
    scope=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*scope));
    if(!scope)return ERROR_NOT_ENOUGH_MEMORY;
    scope->notification=notification;scope->retire=retire;scope->broker=broker;
    scope->state_changed=state_changed;scope->ops=*ops;scope->context=context;
    scope->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!scope->stop){error=GetLastError();goto fail;}
    scope->thread=CreateThread(NULL,0,frontend_pump,scope,0,NULL);
    if(!scope->thread){error=GetLastError();goto fail;}
    *output=scope;return ERROR_SUCCESS;
fail:
    (void)frontend_base_service_stop(scope);
    return error;
}
