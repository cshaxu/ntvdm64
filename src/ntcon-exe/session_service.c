#include "session_service.h"
#include "console_channel.h"
#include "common/console/members.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include <stdio.h>
struct frontend_session_service {
    HANDLE notification,stop,thread,retire,state_changed;
    HANDLE creator,console_anchor;
    BOOL retire_requested,creator_exited;
    run16_console_channel *channel;
    run16_native_frontend *native;
    void (*channel_ready)(void);
};
/* Borrowed roots must not keep an otherwise closed user Console alive just
 * because NTCON itself remains attached. Follow one real Console member at
 * a time; on its exit, resample only once to find the next surviving member.
 * This is Console ownership, never a worker/task or descendant census. */
static DWORD next_console_anchor(HANDLE *anchor)
{
    DWORD count=0,index,self=GetCurrentProcessId(),error=ERROR_NOT_FOUND;
    BOOL inaccessible=FALSE;
    DWORD *members=NULL;
    HANDLE selected=NULL;
    *anchor=NULL;
    error=common_console_members_read(16,4096,0,&members,&count);
    if(error)return error;
    error=ERROR_NOT_FOUND;
    for(index=0;index<count;++index) {
        if(members[index]==self)continue;
        selected=OpenProcess(SYNCHRONIZE,FALSE,members[index]);
        if(!selected) {
            if(GetLastError()==ERROR_ACCESS_DENIED)inaccessible=TRUE;
            continue; /* It may have exited since the snapshot. */
        }
        if(WaitForSingleObject(selected,0)==WAIT_TIMEOUT) {
            *anchor=selected;error=ERROR_SUCCESS;break;
        }
        CloseHandle(selected);selected=NULL;
    }
    common_console_members_release(members);
    if(error==ERROR_NOT_FOUND && inaccessible)error=ERROR_ACCESS_DENIED;
    return error;
}
static DWORD WINAPI frontend_pump(void *context)
{
    frontend_session_service *scope=context;
    HANDLE waits[7];
    DWORD error=ERROR_SUCCESS;
    for (;;) {
        DWORD wait_count=0,retire_index=MAXDWORD,creator_index=MAXDWORD,
            anchor_index=MAXDWORD,wait;
        waits[wait_count++]=scope->stop;
        waits[wait_count++]=scope->notification;
        if(scope->retire && !scope->retire_requested) {
            retire_index=wait_count;waits[wait_count++]=scope->retire;
        }
        if(scope->creator && !scope->creator_exited) { creator_index=wait_count;waits[wait_count++]=scope->creator; }
        if(scope->console_anchor) { anchor_index=wait_count;waits[wait_count++]=scope->console_anchor; }
        if(scope->state_changed) waits[wait_count++]=scope->state_changed;
        if(scope->channel)waits[wait_count++]=run16_console_channel_thread(scope->channel);
        wait=WaitForMultipleObjects(wait_count,waits,FALSE,INFINITE);
        if (wait==WAIT_OBJECT_0) break;
        if (wait<WAIT_OBJECT_0 || wait>=WAIT_OBJECT_0+wait_count) {
            return GetLastError();
        }
        /* Broker shutdown is authoritative even with a stale lease/pending
         * request. Check it before joins, channel work or lease restoration. */
        {
            DWORD retired=0;
            error=OpenNtBaseClientRetireWorkerlessFrontend(&retired);
            if(error)return error;
            if(retired)return ERROR_SUCCESS;
        }
        if(retire_index!=MAXDWORD && wait==WAIT_OBJECT_0+retire_index)
            scope->retire_requested=TRUE;
        if(creator_index!=MAXDWORD && wait==WAIT_OBJECT_0+creator_index)
            scope->creator_exited=TRUE;
        if(anchor_index!=MAXDWORD && wait==WAIT_OBJECT_0+anchor_index) {
            CloseHandle(scope->console_anchor);scope->console_anchor=NULL;
            error=next_console_anchor(&scope->console_anchor);
            if(error==ERROR_NOT_FOUND)return ERROR_SUCCESS;
            if(error)return error;
        }
        {
            DWORD nonce=0,candidate=0,join;
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
                    run16_console_channel_thread(scope->channel),0)==WAIT_OBJECT_0)
                    return ERROR_PIPE_NOT_CONNECTED;
                break;
            }
            if(error)return error;
            if(!request && !worker) {
                /* NTSRV orders the endpoint closed after final I/O. Joining
                 * and disposal precede our acknowledgement, never vice versa. */
                error=run16_console_channel_stop(scope->channel);
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
            if(!scope->native) {
                error=run16_native_frontend_create(&scope->native);
                if(error){CloseHandle(worker);return error;}
            }
            error=run16_console_channel_start_request(request,worker,scope->native,&scope->channel);
            if (error) {
                return error;
            }
            if (scope->channel_ready) scope->channel_ready();
        }
        if(scope->native && ((scope->creator && scope->creator_exited) ||
            scope->retire_requested)){
            DWORD pending=0,tasks=0;
            error=OpenNtBaseClientFrontendUsage(&pending,&tasks);
            if(error)return error;
            /* NTSRV includes native admissions and reported Console members.
             * The frontend owns neither targets nor a second backend census. */
            if(pending || tasks)continue;
            /* Return the visible Console but retain resident worker I/O.
             * Only the broker decides when the root must retire. */
            {
                error=run16_native_frontend_park(scope->native);
                if(error)return error;
            }
            /* Resident channels remain admitted across borrowed leases. A
             * second launch must be able to retire the same frontend. */
            scope->retire_requested=FALSE;
            scope->creator=NULL; /* First launcher is not the next phase's owner. */
            if(!ResetEvent(scope->retire))return GetLastError();
            error=OpenNtBaseClientFrontendLeaseReady();
            if(error)return error;
        }
    }
    return ERROR_SUCCESS;
}

DWORD frontend_service_close(frontend_session_service *scope)
{
    DWORD error=ERROR_SUCCESS;
    if (!scope) return ERROR_SUCCESS;
    if (scope->stop) SetEvent(scope->stop);
    run16_native_frontend_cancel(scope->native);
    if (scope->thread) {
        WaitForSingleObject(scope->thread,INFINITE);
        CloseHandle(scope->thread);
    }
    if(scope->channel) {
        error=run16_console_channel_stop(scope->channel);
        if(error)return error;
        scope->channel=NULL;
    }
    error=run16_native_frontend_destroy(scope->native);
    /* A failed teardown deliberately retains the native object and its
     * handles for process cleanup.  Do not free the owning service storage:
     * the caller must report failure rather than manufacture a handoff. */
    if(error)return error;
    if(scope->stop)CloseHandle(scope->stop);
    if(scope->state_changed)CloseHandle(scope->state_changed);
    if(scope->console_anchor)CloseHandle(scope->console_anchor);
    HeapFree(GetProcessHeap(),0,scope);
    return ERROR_SUCCESS;
}
static DWORD service_start(HANDLE notification,HANDLE creator,HANDLE retire,
    BOOL borrowed,void (*ready)(void),frontend_session_service **output)
{
    frontend_session_service *scope;
    DWORD error;
    if(!output || !notification)return ERROR_INVALID_PARAMETER;
    *output=NULL;
    scope=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*scope));
    if(!scope)return ERROR_NOT_ENOUGH_MEMORY;
    scope->notification=notification;scope->channel_ready=ready;
    scope->creator=creator;scope->retire=retire;
    if(borrowed) {
        error=next_console_anchor(&scope->console_anchor);
        if(error)goto fail;
    }
    if(creator) {
        error=OpenNtBaseClientFrontendStateChanged(&scope->state_changed);
        if(error)goto fail;
    }
    scope->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!scope->stop){error=GetLastError();goto fail;}
    error=run16_native_frontend_create(&scope->native);if(error)goto fail;
    scope->thread=CreateThread(NULL,0,frontend_pump,scope,0,NULL);
    if(!scope->thread){error=GetLastError();goto fail;}
    *output=scope;return ERROR_SUCCESS;
fail:
    (void)frontend_service_close(scope);return error;
}
DWORD frontend_service_start(HANDLE notification,
    void (*ready)(void),frontend_session_service **output)
{
    return service_start(notification,NULL,NULL,FALSE,ready,output);
}
DWORD frontend_service_start_process(HANDLE notification,
    HANDLE creator,HANDLE retire,BOOL borrowed,frontend_session_service **output)
{
    if(!creator || !retire)return ERROR_INVALID_PARAMETER;
    return service_start(notification,creator,retire,borrowed,NULL,output);
}
HANDLE frontend_service_thread(frontend_session_service *scope){return scope ? scope->thread : NULL;}
