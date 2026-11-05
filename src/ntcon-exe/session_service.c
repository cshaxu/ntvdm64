#include "session_service.h"
#include "lib-win32/process-observation/console_members.h"
#include "console_channel.h"
#include "common/frontend-base/service.h"
#include "common/protocol/frontend_lifecycle.h"
#include "common/base_srv/base_rpc_client.h"
struct frontend_session_service {
    HANDLE retire,state_changed,broker;
    frontend_base_service *base;
    frontend_session *presentation;
    void (*channel_ready)(void);
};
/* One service-selected disposition path for healthy and faulted I/O. */
static DWORD retirement(void *context,DWORD *action)
{
    frontend_session_service *scope=context;
    DWORD error=OpenNtBaseClientRetireWorkerlessFrontend(action);
    if(error)return error;
    switch(*action) {
    case OPENNT_FRONTEND_RETIRE_NONE:
    case OPENNT_FRONTEND_RETIRE_FINAL_CLOSE:
        return ERROR_SUCCESS;
    case OPENNT_FRONTEND_RETIRE_RETURN_CONSOLE:
        error=frontend_session_return_console(scope->presentation);
        if(!error)error=OpenNtBaseClientFrontendCloseQuiesced();
        return error;
    case OPENNT_FRONTEND_RETIRE_PARK:
        error=frontend_session_park(scope->presentation);
        if(error)return error;
        if(scope->retire && !ResetEvent(scope->retire))return GetLastError();
        return OpenNtBaseClientFrontendLeaseReady();
    default:
        return ERROR_INVALID_DATA;
    }
}

static DWORD prepare_requests(void *context)
{
    DWORD error,nonce=0,candidate=0,join;
    (void)context;
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

    return ERROR_SUCCESS;
}
static DWORD next_request(void *context,DWORD *request,HANDLE *worker)
{(void)context;return OpenNtBaseClientFrontendRequest(request,worker);}
static DWORD start_channel(void *context,DWORD request,HANDLE worker,void **output)
{
    frontend_session_service *scope=context;
    frontend_io_channel *channel=NULL;
    DWORD error;
    *output=NULL;
    if(!scope->presentation) {
        error=frontend_session_create(&scope->presentation);
        if(error){CloseHandle(worker);return error;}
    }
    error=frontend_io_channel_start_request(request,worker,scope->presentation,&channel);
    if(error)return error;
    *output=channel;
    if(scope->channel_ready)scope->channel_ready();
    return ERROR_SUCCESS;
}
static DWORD stop_channel(void *context,void *channel)
{(void)context;return frontend_io_channel_stop(channel);}
static HANDLE channel_thread(void *context,void *channel)
{(void)context;return frontend_io_channel_thread(channel);}
static DWORD disconnected(void *context)
{(void)context;return OpenNtBaseClientFrontendIoDisconnected();}
static DWORD fault(void *context)
{(void)context;return OpenNtBaseClientRequestParticipantExit();}
DWORD frontend_service_close(frontend_session_service *scope)
{
    DWORD error,step;
    if(!scope)return ERROR_SUCCESS;
    error=frontend_base_service_stop(scope->base);
    if(error) {
        /* Failed join retains borrowers. Return/cancel presentation without
         * freeing it or manufacturing a restored acknowledgement. */
        (void)frontend_session_return_console(scope->presentation);
        frontend_session_cancel(scope->presentation);
        return error;
    }
    scope->base=NULL;
    frontend_session_cancel(scope->presentation);
    step=frontend_session_destroy(scope->presentation);
    if(step)return step;
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
    static const frontend_base_service_ops ops={retirement,prepare_requests,next_request,
        start_channel,stop_channel,channel_thread,disconnected,fault};
    if(!output || !notification)return ERROR_INVALID_PARAMETER;
    *output=NULL;
    scope=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*scope));
    if(!scope)return ERROR_NOT_ENOUGH_MEMORY;
    scope->channel_ready=ready;
    scope->retire=retire;
    if(creator) {
        error=OpenNtBaseClientBrokerProcess(&scope->broker);
        if(error)goto fail;
        error=OpenNtBaseClientFrontendStateChanged(&scope->state_changed);
        if(error)goto fail;
    }
    error=frontend_session_create(&scope->presentation);if(error)goto fail;
    error=frontend_base_service_start(notification,retire,scope->broker,scope->state_changed,
        &ops,scope,&scope->base);
    if(error)goto fail;
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
HANDLE frontend_service_thread(frontend_session_service *scope){return scope ? frontend_base_service_thread(scope->base) : NULL;}
