/* Unit resource-lifetime test of the production root pump. Broker discovery
 * and channel transport are substitutes; real threads/events exercise joining.
 * This does not replace authenticated RPC or real guest Console tests. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "run16-exe/frontend_scope.h"
#include "frontend-exe/console_channel.h"
#include "frontend-exe/native_console_request.h"
#include "frontend-exe/session_service.h"
#include "frontend-exe/bootstrap.h"
#include "product-abi/console_io.h"

#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %u error %lu: %s\n", \
    (unsigned)__LINE__,GetLastError(),#x); ExitProcess(1); } } while (0)
#define COUNT 34
struct run16_console_channel { HANDLE thread,release; DWORD index; };
static run16_console_channel *channels[COUNT];
static HANDLE notification,ready;
static CRITICAL_SECTION lock;
static DWORD pending,created;
static LONG stopped;
static DWORD registrations,retains,bindings,retain_error=ERROR_ACCESS_DENIED;
static HANDLE expected_execution;
static frontend_session_service *fixture_service;
static void attached(void);
DWORD frontend_bootstrap_start(PCWSTR image,frontend_connection *connection)
{
    DWORD error;
    CHECK(wcsstr(image,L"frontend.exe")!=NULL);
    ZeroMemory(connection,sizeof(*connection));
    notification=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(notification);++registrations;
    CHECK(DuplicateHandle(GetCurrentProcess(),notification,GetCurrentProcess(),&connection->capability,SYNCHRONIZE,TRUE,0));
    CHECK(DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),&connection->process,SYNCHRONIZE,FALSE,0));
    error=frontend_service_start(notification,notification,attached,&fixture_service);
    return error;
}
void frontend_bootstrap_release(frontend_connection *connection)
{
    if(connection->capability)CloseHandle(connection->capability);
    if(connection->process)CloseHandle(connection->process);
    if(connection->channel)CloseHandle(connection->channel);
    ZeroMemory(connection,sizeof(*connection));
}
/* This fixture exercises scope/channel lifetime, not native presentation. */
struct run16_native_frontend { DWORD unused; };
DWORD run16_native_frontend_create(run16_native_frontend **out)
{ *out=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(**out));return *out ? 0 : ERROR_NOT_ENOUGH_MEMORY; }
void run16_native_frontend_cancel(run16_native_frontend *value) { (void)value; }
DWORD run16_native_frontend_members(run16_native_frontend *value,DWORD *count)
{ (void)value;(void)count;return ERROR_INVALID_FUNCTION; }
DWORD run16_native_frontend_drain(run16_native_frontend *value)
{ (void)value;return ERROR_INVALID_FUNCTION; }
DWORD OpenNtBaseClientFrontendUsage(DWORD *pending_count,DWORD *tasks)
{ (void)pending_count;(void)tasks;return ERROR_INVALID_FUNCTION; }
DWORD OpenNtBaseClientRetireFrontend(void) { return ERROR_INVALID_FUNCTION; }
void run16_native_frontend_destroy(run16_native_frontend *value) { if(value)HeapFree(GetProcessHeap(),0,value); }
DWORD run16_native_frontend_launch(run16_native_frontend *value,const run16_native_start *start,HANDLE *out)
{ (void)value;(void)start;*out=NULL;return ERROR_NOT_SUPPORTED; }
DWORD run16_native_frontend_wait(run16_native_frontend *value,HANDLE target,DWORD *result)
{ (void)value;(void)target;(void)result;return ERROR_NOT_SUPPORTED; }
DWORD OpenNtBaseClientTakeFrontendChannel(HANDLE *channel,HANDLE *caller,HANDLE *execution)
{ *channel=*caller=*execution=NULL;return ERROR_NOT_FOUND; }
DWORD run16_native_request_start(run16_native_frontend *value,HANDLE root,HANDLE stop,
    HANDLE channel,HANDLE sender,HANDLE execution,run16_native_request **out)
{ (void)value;(void)root;(void)stop;(void)channel;(void)sender;(void)execution;*out=NULL;return ERROR_NOT_SUPPORTED; }
HANDLE run16_native_request_thread(run16_native_request *value) { (void)value;return NULL; }
void run16_native_request_close(run16_native_request *value) { (void)value; }
DWORD run16_native_request_submit_receipt(HANDLE root,HANDLE capability,const run16_native_start *start,HANDLE *out,HANDLE *receipt)
{ (void)root;(void)capability;(void)start;*out=*receipt=NULL;return ERROR_NOT_SUPPORTED; }

DWORD OpenNtBaseClientRegisterFrontendRoot(HANDLE value)
{
    ++registrations;
    return DuplicateHandle(GetCurrentProcess(),value,GetCurrentProcess(),
        &notification,0,FALSE,DUPLICATE_SAME_ACCESS) ? 0 : GetLastError();
}
DWORD OpenNtBaseClientRetainFrontendRoot(HANDLE value,HANDLE *root,DWORD *generation)
{
    (void)value;
    ++retains;*root=NULL;*generation=0;
    if (retain_error) return retain_error;
    if (!DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),
        root,SYNCHRONIZE,FALSE,0)) return GetLastError();
    *generation=1;
    return ERROR_SUCCESS;
}
DWORD OpenNtBaseClientBindConsoleContext(HANDLE value)
{
    ++bindings;
    return value==expected_execution ? ERROR_SUCCESS : ERROR_ACCESS_DENIED;
}
DWORD OpenNtBaseClientFrontendRequest(DWORD *request,HANDLE *worker)
{
    DWORD error=ERROR_NOT_FOUND;
    EnterCriticalSection(&lock);
    if (pending) {
        *request=pending;pending=0;
        error=DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),
            GetCurrentProcess(),worker,SYNCHRONIZE,FALSE,0) ? 0 : GetLastError();
    } else ResetEvent(notification);
    LeaveCriticalSection(&lock);
    return error;
}
static DWORD WINAPI channel_main(void *value)
{
    run16_console_channel *channel=value;
    return WaitForSingleObject(channel->release,INFINITE)==WAIT_OBJECT_0 ? 0 : 1;
}
DWORD run16_console_channel_start_request(DWORD request,HANDLE worker,
    run16_native_frontend *native,run16_console_channel **output)
{
    run16_console_channel *channel;
    CHECK(native!=NULL);
    CloseHandle(worker);
    CHECK(request && created<COUNT);
    channel=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*channel));
    CHECK(channel);
    channel->index=created;
    channel->release=CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(channel->release);
    channel->thread=CreateThread(NULL,0,channel_main,channel,0,NULL);
    CHECK(channel->thread);
    channels[created++]=channel;
    *output=channel;
    return 0;
}
HANDLE run16_console_channel_thread(run16_console_channel *channel)
{
    return channel->thread;
}
void run16_console_channel_stop(run16_console_channel *channel)
{
    CHECK(SetEvent(channel->release));
    CHECK(WaitForSingleObject(channel->thread,5000)==WAIT_OBJECT_0);
    CloseHandle(channel->thread);CloseHandle(channel->release);
    channels[channel->index]=NULL;
    HeapFree(GetProcessHeap(),0,channel);
    InterlockedIncrement(&stopped);
}
static void attached(void) { CHECK(SetEvent(ready)); }
static void submit(DWORD id)
{
    EnterCriticalSection(&lock);
    CHECK(!pending);pending=id;
    CHECK(SetEvent(notification));
    LeaveCriticalSection(&lock);
    CHECK(WaitForSingleObject(ready,5000)==WAIT_OBJECT_0);
}
int main(void)
{
    run16_frontend_scope *scope=NULL,*inner=NULL;
    HANDLE execution;
    char frontend_text[32],execution_text[32],retained_text[32];
    DWORD i;
    CHECK(SetEnvironmentVariableW(L"NTVDM_FRONTEND_CAPABILITY",NULL));
    CHECK(SetEnvironmentVariableW(L"NTVDM_EXECUTION_CONSOLE",NULL));
    CHECK(SetEnvironmentVariableA("NTVDM_EXECUTION_CONSOLE","1"));
    CHECK(run16_frontend_scope_begin(&inner)==ERROR_INVALID_DATA && !inner && !registrations);
    CHECK(SetEnvironmentVariableA("NTVDM_EXECUTION_CONSOLE",NULL));
    InitializeCriticalSection(&lock);
    ready=CreateEventW(NULL,FALSE,FALSE,NULL);CHECK(ready);
    CHECK(run16_frontend_scope_begin(&scope)==0);
    CHECK(scope && !run16_frontend_scope_capability(NULL));
    CHECK(registrations==1);
    CHECK(GetEnvironmentVariableA("NTVDM_FRONTEND_CAPABILITY",frontend_text,sizeof(frontend_text)));
    CHECK(run16_frontend_scope_begin(&inner)==ERROR_ACCESS_DENIED && !inner);
    CHECK(registrations==1 && retains==1 && !bindings);
    retain_error=ERROR_PIPE_NOT_CONNECTED;
    CHECK(run16_frontend_scope_begin(&inner)==ERROR_PIPE_NOT_CONNECTED && !inner && registrations==1);
    retain_error=ERROR_SUCCESS;
    CHECK(run16_frontend_scope_begin(&inner)==0 && inner);
    run16_frontend_scope_end(inner);inner=NULL;
    CHECK(registrations==1 && !created && !stopped && !bindings);
    execution=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(execution);
    sprintf_s(execution_text,sizeof(execution_text),"%lx",(unsigned long)(ULONG_PTR)execution);
    CHECK(SetEnvironmentVariableA("NTVDM_EXECUTION_CONSOLE",execution_text));
    CHECK(run16_frontend_scope_begin(&inner)==ERROR_ACCESS_DENIED && !inner);
    CHECK(registrations==1 && bindings==1);
    expected_execution=execution;
    CHECK(run16_frontend_scope_begin(&inner)==0 && inner);
    run16_frontend_scope_end(inner);inner=NULL;
    CHECK(registrations==1 && bindings==2 && !created && !stopped);
    {
        HANDLE saved=GetStdHandle(STD_OUTPUT_HANDLE),endpoint=CreateEventW(NULL,TRUE,FALSE,NULL);
        HANDLE read_pipe,write_pipe;
        CHECK(endpoint && SetStdHandle(STD_OUTPUT_HANDLE,endpoint));
        CHECK(SetEnvironmentVariableA(CONSOLE_COMMAND_STREAMS_ENV,"2"));
        CHECK(run16_frontend_scope_begin(&inner)==0 && inner);
        CHECK(run16_frontend_scope_has_execution(inner) && run16_frontend_scope_console_mask(inner)==2);
        CHECK(!GetEnvironmentVariableA(CONSOLE_COMMAND_STREAMS_ENV,retained_text,sizeof(retained_text)) && GetLastError()==ERROR_ENVVAR_NOT_FOUND);
        run16_frontend_scope_end(inner);inner=NULL;
        CHECK(CreatePipe(&read_pipe,&write_pipe,NULL,0) && SetStdHandle(STD_OUTPUT_HANDLE,write_pipe));
        CHECK(SetEnvironmentVariableA(CONSOLE_COMMAND_STREAMS_ENV,"2"));
        CHECK(run16_frontend_scope_begin(&inner)==ERROR_INVALID_HANDLE && !inner);
        CHECK(SetEnvironmentVariableA(CONSOLE_COMMAND_STREAMS_ENV,"0"));
        CHECK(run16_frontend_scope_begin(&inner)==0 && run16_frontend_scope_console_mask(inner)==0);
        run16_frontend_scope_end(inner);inner=NULL;
        CHECK(SetEnvironmentVariableA(CONSOLE_COMMAND_STREAMS_ENV,"8"));
        CHECK(run16_frontend_scope_begin(&inner)==ERROR_INVALID_DATA && !inner);
        CHECK(SetStdHandle(STD_OUTPUT_HANDLE,saved));
        CloseHandle(endpoint);CloseHandle(read_pipe);CloseHandle(write_pipe);
        puts("PASS one-hop stream roles consumed; actual pipes cannot be reclassified as Console endpoints");
    }
    CHECK(GetEnvironmentVariableA("NTVDM_EXECUTION_CONSOLE",retained_text,sizeof(retained_text)) &&
        !strcmp(retained_text,execution_text));
    CHECK(GetEnvironmentVariableA("NTVDM_FRONTEND_CAPABILITY",retained_text,sizeof(retained_text)) &&
        !strcmp(retained_text,frontend_text));
    CHECK(SetEnvironmentVariableA("NTVDM_EXECUTION_CONSOLE","not-a-handle"));
    CHECK(run16_frontend_scope_begin(&inner)==ERROR_INVALID_DATA && !inner && registrations==1);
    CHECK(SetEnvironmentVariableA("NTVDM_EXECUTION_CONSOLE",NULL));
    CloseHandle(execution);
    puts("PASS inner joins without frontend ownership; orphan, invalid and disconnected capabilities never promote to root");
    submit(1); /* Keep this channel live throughout the repeated short ones. */
    for (i=1;i<COUNT;++i) {
        HANDLE release,thread;
        submit(i+1);
        CHECK(channels[0] && WaitForSingleObject(channels[0]->thread,0)==WAIT_TIMEOUT);
        CHECK(InterlockedCompareExchange(&stopped,0,0)==(LONG)i-1);
        /* The pump may reclaim the entry immediately after thread exit. */
        CHECK(DuplicateHandle(GetCurrentProcess(),channels[i]->release,GetCurrentProcess(),
            &release,0,FALSE,DUPLICATE_SAME_ACCESS));
        CHECK(DuplicateHandle(GetCurrentProcess(),channels[i]->thread,GetCurrentProcess(),
            &thread,SYNCHRONIZE,FALSE,0));
        CHECK(SetEvent(release));
        CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);
        CloseHandle(release);CloseHandle(thread);
    }
    run16_frontend_scope_end(scope);
    CHECK(channels[0] && WaitForSingleObject(channels[0]->thread,0)==WAIT_TIMEOUT);
    frontend_service_close(fixture_service);
    CHECK(stopped==COUNT);
    for (i=0;i<COUNT;++i) CHECK(!channels[i]);
    CloseHandle(notification);CloseHandle(ready);DeleteCriticalSection(&lock);
    puts("PASS completed channels reclaimed; live channel preserved; final join complete");
    return 0;
}
