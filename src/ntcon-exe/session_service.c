#include "session_service.h"
#include "common/console/members.h"
#include "console_channel.h"
#include "common/protocol/frontend_lifecycle.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include <stdio.h>
struct frontend_session_service {
    HANDLE notification,stop,thread,retire,state_changed;
    HANDLE broker;
    BOOL retire_requested,return_prepared;
    frontend_io_channel *channel;
    frontend_session *presentation;
    void (*channel_ready)(void);
};
/* One service-selected disposition path for healthy and faulted I/O. */
static DWORD frontend_retirement(frontend_session_service *scope,DWORD *action)
{
    DWORD error=OpenNtBaseClientRetireWorkerlessFrontend(action);
    if(error)return error;
    switch(*action) {
    case OPENNT_FRONTEND_RETIRE_NONE:
    case OPENNT_FRONTEND_RETIRE_FINAL_CLOSE:
        return ERROR_SUCCESS;
    case OPENNT_FRONTEND_RETIRE_RETURN_CONSOLE:
        error=frontend_session_return_console(scope->presentation);
        if(!error)error=OpenNtBaseClientFrontendCloseQuiesced();
        if(!error)scope->return_prepared=TRUE;
        return error;
    case OPENNT_FRONTEND_RETIRE_PARK:
        error=frontend_session_park(scope->presentation);
        if(error)return error;
        if(scope->retire && !ResetEvent(scope->retire))return GetLastError();
        scope->retire_requested=FALSE;
        return OpenNtBaseClientFrontendLeaseReady();
    default:
        return ERROR_INVALID_DATA;
    }
}
static DWORD frontend_run(frontend_session_service *scope)
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
            waits[wait_count++]=frontend_io_channel_thread(scope->channel);
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
        {
            DWORD nonce=0,candidate=0,join;
            /* NTSRV asks for this one-off, physical same-Console check while
             * admitting a new launcher.  It authorizes a relationship that
             * NTSRV owns; it is not a local liveness probe or retirement
             * policy. */
            while((join=OpenNtBaseClientFrontendJoinCandidate(&nonce,&candidate))==ERROR_SUCCESS) {
                DWORD count=0,*members=NULL,index,snapshot_error;
                BOOL same=FALSE;
                snapshot_error=common_console_members_read(16,4096,0,&members,&count);
                if(snapshot_error==ERROR_NOT_ENOUGH_MEMORY || snapshot_error==ERROR_BUFFER_OVERFLOW)
                    return snapshot_error;
                if(!snapshot_error)
                    for(index=0;index<count;++index)if(members[index]==candidate){same=TRUE;break;}
                common_console_members_release(members);
                error=OpenNtBaseClientFrontendJoinDecision(nonce,same);
                if(error)return error;
            }
            if(join!=ERROR_NOT_FOUND)return join;
        }
        for (;;) {
            HANDLE worker=NULL;
            DWORD request=0;
            if (WaitForSingleObject(scope->stop,0)==WAIT_OBJECT_0) return ERROR_SUCCESS;
            error=OpenNtBaseClientFrontendRequest(&request,&worker);
            if(error==ERROR_NOT_FOUND) {
                /* EOF is a transport failure unless NTSRV has authorized
                 * release. It cannot itself choose the next logical owner. */
                if(scope->channel && WaitForSingleObject(
                    frontend_io_channel_thread(scope->channel),0)==WAIT_OBJECT_0)
                    return ERROR_PIPE_NOT_CONNECTED;
                break;
            }
            if(error)return error;
            if(!request && !worker) {
                /* NTSRV orders the endpoint closed after final I/O. Joining
                 * and disposal precede our acknowledgement, never vice versa. */
                error=frontend_io_channel_stop(scope->channel);
                if(error)return error;
                scope->channel=NULL;
                error=OpenNtBaseClientFrontendIoDisconnected();
                if(error)return error;
                continue;
            }
            if(!request || !worker || scope->channel) {
                if(worker)CloseHandle(worker);
                return ERROR_INVALID_STATE;
            }
            if(!scope->presentation) {
                error=frontend_session_create(&scope->presentation);
                if(error){CloseHandle(worker);return error;}
            }
            error=frontend_io_channel_start_request(request,worker,scope->presentation,&scope->channel);
            if (error) {
                return error;
            }
            if (scope->channel_ready) scope->channel_ready();
        }
    }
    return ERROR_SUCCESS;
}

static DWORD WINAPI frontend_pump(void *context)
{
    frontend_session_service *scope=context;
    DWORD error=frontend_run(scope),action,step,wait;
    HANDLE waits[3]={scope->broker,scope->stop,scope->state_changed};
    /* The in-process fixture has no admitted service lifetime. Production
     * keeps the authenticated broker handle until this thread has joined. */
    if(!error || !scope->broker)return error;
    if(WaitForSingleObject(scope->broker,0)==WAIT_OBJECT_0)return ERROR_INVALID_HANDLE;
    /* Stop admitting I/O after a fault, but do not close the presentation or
     * report restoration before NTSRV's instruction. Submission is one-shot;
     * failure does not authorize local exit, reconnection or replay. */
    step=OpenNtBaseClientRequestParticipantExit();
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

DWORD frontend_service_close(frontend_session_service *scope)
{
    DWORD error=ERROR_SUCCESS,step;
    if (!scope) return ERROR_SUCCESS;
    if (scope->stop) SetEvent(scope->stop);
    if (scope->thread) {
        WaitForSingleObject(scope->thread,INFINITE);
        CloseHandle(scope->thread);
        scope->thread=NULL;
    }
    if(scope->channel) {
        step=frontend_io_channel_stop(scope->channel);
        if(step) {
            /* The channel's join failed, so its storage must remain
             * live.  Its stop was nevertheless issued; return the borrowed
             * Console through the still-running presentation before asking
             * that presentation thread to stop. */
            error=step;
            step=frontend_session_return_console(scope->presentation);
            if(!error && step)error=step;
        } else scope->channel=NULL;
    }
    frontend_session_cancel(scope->presentation);
    /* If channel join failed it may still borrow presentation state, so do
     * not free it.  Cancellation above remains the safe convergence request. */
    step=scope->channel ? ERROR_BUSY : frontend_session_destroy(scope->presentation);
    if(!error && step)error=step;
    /* A failed teardown deliberately retains the presentation object and its
     * handles for process cleanup.  Do not free the owning service storage:
     * the caller must report failure rather than manufacture a handoff. */
    if(error)return error;
    if(scope->stop)CloseHandle(scope->stop);
    if(scope->state_changed)CloseHandle(scope->state_changed);
    if(scope->broker)CloseHandle(scope->broker);
    HeapFree(GetProcessHeap(),0,scope);
    return ERROR_SUCCESS;
}
static DWORD service_start(HANDLE notification,HANDLE creator,HANDLE retire,
    void (*ready)(void),frontend_session_service **output)
{
    frontend_session_service *scope;
    DWORD error;
    if(!output || !notification)return ERROR_INVALID_PARAMETER;
    *output=NULL;
    scope=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*scope));
    if(!scope)return ERROR_NOT_ENOUGH_MEMORY;
    scope->notification=notification;scope->channel_ready=ready;
    scope->retire=retire;
    if(creator) {
        error=OpenNtBaseClientBrokerProcess(&scope->broker);
        if(error)goto fail;
        error=OpenNtBaseClientFrontendStateChanged(&scope->state_changed);
        if(error)goto fail;
    }
    scope->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!scope->stop){error=GetLastError();goto fail;}
    error=frontend_session_create(&scope->presentation);if(error)goto fail;
    scope->thread=CreateThread(NULL,0,frontend_pump,scope,0,NULL);
    if(!scope->thread){error=GetLastError();goto fail;}
    *output=scope;return ERROR_SUCCESS;
fail:
    (void)frontend_service_close(scope);return error;
}
DWORD frontend_service_start(HANDLE notification,
    void (*ready)(void),frontend_session_service **output)
{
    return service_start(notification,NULL,NULL,ready,output);
}
DWORD frontend_service_start_process(HANDLE notification,
    HANDLE creator,HANDLE retire,frontend_session_service **output)
{
    if(!creator || !retire)return ERROR_INVALID_PARAMETER;
    return service_start(notification,creator,retire,NULL,output);
}
HANDLE frontend_service_thread(frontend_session_service *scope){return scope ? scope->thread : NULL;}
