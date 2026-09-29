#include "session_service.h"
#include "console_channel.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
typedef struct frontend_channel {
    struct frontend_channel *next;
    run16_console_channel *channel;
} frontend_channel;
struct frontend_session_service {
    HANDLE capability,notification,stop,thread;
    HANDLE creator;
    BOOL admitted;
    frontend_channel *channels;
    run16_native_frontend *native;
    void (*channel_ready)(void);
};
static DWORD WINAPI frontend_pump(void *context)
{
    frontend_session_service *scope=context;
    HANDLE waits[2]={scope->stop,scope->notification};
    DWORD error=ERROR_SUCCESS;
    for (;;) {
        DWORD wait=WaitForMultipleObjects(2,waits,FALSE,scope->creator ? 100 : INFINITE);
        if (wait==WAIT_OBJECT_0) break;
        if (wait!=WAIT_OBJECT_0+1 && wait!=WAIT_TIMEOUT) return GetLastError();
        for (;;) {
            HANDLE worker=NULL;
            DWORD request=0;
            frontend_channel *entry;
            frontend_channel **link=&scope->channels;
            if (WaitForSingleObject(scope->stop,0)==WAIT_OBJECT_0) return ERROR_SUCCESS;
            /* This pump alone owns the list. Retire only joined channels;
             * live workers and their task lifetimes are not affected. */
            while ((entry=*link)!=NULL) {
                if (WaitForSingleObject(run16_console_channel_thread(entry->channel),0)==WAIT_OBJECT_0) {
                    *link=entry->next;
                    run16_console_channel_stop(entry->channel);
                    HeapFree(GetProcessHeap(),0,entry);
                } else link=&entry->next;
            }
            error=OpenNtBaseClientFrontendRequest(&request,&worker);
            if (error==ERROR_NOT_FOUND) break;
            if (error) return error;
            entry=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*entry));
            if (!entry) { CloseHandle(worker);return ERROR_NOT_ENOUGH_MEMORY; }
            error=run16_console_channel_start_request(request,worker,scope->native,&entry->channel);
            if (error) {
                HeapFree(GetProcessHeap(),0,entry);
                if (error==ERROR_ALREADY_EXISTS) continue;
                return error;
            }
            entry->next=scope->channels;scope->channels=entry;
            scope->admitted=TRUE;
            if (scope->channel_ready) scope->channel_ready();
        }
        if(scope->creator &&
            (scope->admitted || WaitForSingleObject(scope->creator,0)==WAIT_OBJECT_0)){
            DWORD pending=0,tasks=0;
            error=OpenNtBaseClientFrontendUsage(&pending,&tasks);if(error)return error;
            /* NTSRV includes native admissions and reported Console members.
             * The frontend owns neither targets nor a second backend census. */
            if(pending || tasks)continue;
            error=OpenNtBaseClientRetireFrontend();
            if(error==ERROR_BUSY)continue;
            if(error)return error;
            /* No new admissions after the broker barrier. End idle DOS I/O
             * channels before draining presentation; never stop
             * execution based on the creator's process or direct target. */
            while(scope->channels){
                frontend_channel *entry=scope->channels;
                scope->channels=entry->next;
                run16_console_channel_stop(entry->channel);
                HeapFree(GetProcessHeap(),0,entry);
            }
            return run16_native_frontend_drain(scope->native);
        }
    }
    return ERROR_SUCCESS;
}

void frontend_service_close(frontend_session_service *scope)
{
    frontend_channel *entry;
    if (!scope) return;
    if (scope->stop) SetEvent(scope->stop);
    run16_native_frontend_cancel(scope->native);
    if (scope->thread) {
        WaitForSingleObject(scope->thread,INFINITE);
        CloseHandle(scope->thread);
    }
    while ((entry=scope->channels)!=NULL) {
        scope->channels=entry->next;
        run16_console_channel_stop(entry->channel);
        HeapFree(GetProcessHeap(),0,entry);
    }
    run16_native_frontend_destroy(scope->native);
    if(scope->stop)CloseHandle(scope->stop);
    HeapFree(GetProcessHeap(),0,scope);
}
static DWORD service_start(HANDLE capability,HANDLE notification,HANDLE creator,
    void (*ready)(void),frontend_session_service **output)
{
    frontend_session_service *scope;
    DWORD error;
    if(!output || !capability || !notification)return ERROR_INVALID_PARAMETER;
    *output=NULL;
    scope=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*scope));
    if(!scope)return ERROR_NOT_ENOUGH_MEMORY;
    scope->capability=capability;scope->notification=notification;scope->channel_ready=ready;
    scope->creator=creator;
    scope->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!scope->stop){error=GetLastError();goto fail;}
    error=run16_native_frontend_create(&scope->native);if(error)goto fail;
    scope->thread=CreateThread(NULL,0,frontend_pump,scope,0,NULL);
    if(!scope->thread){error=GetLastError();goto fail;}
    *output=scope;return ERROR_SUCCESS;
fail:
    frontend_service_close(scope);return error;
}
DWORD frontend_service_start(HANDLE capability,HANDLE notification,
    void (*ready)(void),frontend_session_service **output)
{
    return service_start(capability,notification,NULL,ready,output);
}
DWORD frontend_service_start_process(HANDLE capability,HANDLE notification,
    HANDLE creator,frontend_session_service **output)
{
    if(!creator)return ERROR_INVALID_PARAMETER;
    return service_start(capability,notification,creator,NULL,output);
}
HANDLE frontend_service_thread(frontend_session_service *scope){return scope ? scope->thread : NULL;}
