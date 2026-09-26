#include "frontend_scope.h"
#include "console_channel.h"
#include "basesrv-exe/opennt/include/base_rpc_client.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/* A local handle locator, never a broker identity or authorization token. */
#define FRONTEND_ENV "NTVDM_FRONTEND_CAPABILITY"
typedef struct frontend_channel {
    struct frontend_channel *next;
    run16_console_channel *channel;
} frontend_channel;
struct run16_frontend_scope {
    HANDLE capability,notification,root,stop,thread;
    frontend_channel *channels;
    BOOL owns_environment;
    void (*channel_ready)(void);
};

static DWORD WINAPI frontend_pump(void *context)
{
    run16_frontend_scope *scope=context;
    HANDLE waits[2]={scope->stop,scope->notification};
    DWORD error=ERROR_SUCCESS;
    for (;;) {
        DWORD wait=WaitForMultipleObjects(2,waits,FALSE,INFINITE);
        if (wait==WAIT_OBJECT_0) break;
        if (wait!=WAIT_OBJECT_0+1) return GetLastError();
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
            error=run16_console_channel_start_request(request,worker,&entry->channel);
            if (error) {
                HeapFree(GetProcessHeap(),0,entry);
                if (error==ERROR_ALREADY_EXISTS) continue;
                return error;
            }
            entry->next=scope->channels;scope->channels=entry;
            if (scope->channel_ready) scope->channel_ready();
        }
    }
    return ERROR_SUCCESS;
}

void run16_frontend_scope_end(run16_frontend_scope *scope)
{
    frontend_channel *entry;
    if (!scope) return;
    if (scope->stop) SetEvent(scope->stop);
    if (scope->thread) {
        WaitForSingleObject(scope->thread,INFINITE);
        CloseHandle(scope->thread);
    }
    while ((entry=scope->channels)!=NULL) {
        scope->channels=entry->next;
        run16_console_channel_stop(entry->channel);
        HeapFree(GetProcessHeap(),0,entry);
    }
    if (scope->owns_environment) SetEnvironmentVariableA(FRONTEND_ENV,NULL);
    if (scope->capability) CloseHandle(scope->capability);
    if (scope->notification) CloseHandle(scope->notification);
    if (scope->root) CloseHandle(scope->root);
    if (scope->stop) CloseHandle(scope->stop);
    HeapFree(GetProcessHeap(),0,scope);
}

DWORD run16_frontend_scope_begin(run16_frontend_scope **output,void (*channel_ready)(void))
{
    run16_frontend_scope *scope;
    char text[32],*end;
    DWORD count,error=ERROR_SUCCESS,generation;
    ULONG_PTR value;
    if (!output) return ERROR_INVALID_PARAMETER;
    *output=NULL;
    scope=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*scope));
    if (!scope) return ERROR_NOT_ENOUGH_MEMORY;
    scope->channel_ready=channel_ready;
    count=GetEnvironmentVariableA(FRONTEND_ENV,text,sizeof(text));
    if (count) {
        if (count>=sizeof(text)) { error=ERROR_INVALID_DATA;goto fail; }
        value=(ULONG_PTR)strtoul(text,&end,16);
        if (!value || end==text || *end) { error=ERROR_INVALID_DATA;goto fail; }
        /* Inherited locator is untrusted until the broker matches the object. */
        error=OpenNtBaseClientRetainFrontendRoot((HANDLE)value,&scope->root,&generation);
        if (error) goto fail;
        if (!DuplicateHandle(GetCurrentProcess(),(HANDLE)value,GetCurrentProcess(),
            &scope->capability,SYNCHRONIZE,FALSE,0)) { error=GetLastError();goto fail; }
    } else {
        if (GetLastError()!=ERROR_ENVVAR_NOT_FOUND) { error=ERROR_INVALID_DATA;goto fail; }
        scope->notification=CreateEventW(NULL,TRUE,FALSE,NULL);
        scope->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
        if (!scope->notification || !scope->stop) { error=GetLastError();goto fail; }
        error=OpenNtBaseClientRegisterFrontendRoot(scope->notification);
        if (error) goto fail;
        if (!DuplicateHandle(GetCurrentProcess(),scope->notification,GetCurrentProcess(),
            &scope->capability,SYNCHRONIZE,TRUE,0)) { error=GetLastError();goto fail; }
        sprintf_s(text,sizeof(text),"%lx",(unsigned long)(uintptr_t)scope->capability);
        if (!SetEnvironmentVariableA(FRONTEND_ENV,text)) { error=GetLastError();goto fail; }
        scope->owns_environment=TRUE;
        scope->thread=CreateThread(NULL,0,frontend_pump,scope,0,NULL);
        if (!scope->thread) { error=GetLastError();goto fail; }
    }
    *output=scope;
    return ERROR_SUCCESS;
fail:
    run16_frontend_scope_end(scope);
    return error;
}

HANDLE run16_frontend_scope_capability(run16_frontend_scope *scope)
{
    return scope ? scope->capability : NULL;
}
