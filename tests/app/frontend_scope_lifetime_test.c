/* Unit resource-lifetime test of the production root pump. Broker discovery
 * and channel transport are substitutes; real threads/events exercise joining.
 * This does not replace authenticated RPC or real guest Console tests. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "run16-exe/frontend_scope.h"
#include "ntcon-exe/console_channel.h"
#include "ntcon-exe/session_service.h"
#include "run16-exe/frontend_bootstrap.h"
#include "common/protocol/console_io.h"

#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %u error %lu: %s\n", \
    (unsigned)__LINE__,GetLastError(),#x); ExitProcess(1); } } while (0)
#define COUNT 34
struct frontend_io_channel { HANDLE thread,release; DWORD index; };
static frontend_io_channel *channels[COUNT];
static HANDLE notification,ready,retirement_state;
static CRITICAL_SECTION lock;
static DWORD pending,created,disconnects;
static BOOL disconnect_pending;
static HANDLE disconnect_seen;
static LONG stopped;
static DWORD registrations,retains,bindings,retain_error=ERROR_ACCESS_DENIED;
static HANDLE expected_execution;
static frontend_session_service *fixture_service;
static BOOL retirement_mode;
static BOOL native_mode;
static DWORD native_error;
static HANDLE diagnostic_target;
static volatile LONG usage_pending,retire_calls,drain_calls,park_calls,broker_shutdown;
static HANDLE usage_seen,park_seen;
static void attached(void);
static DWORD fixture_frontend_start(PCWSTR image,frontend_connection *connection)
{
    DWORD error;
    CHECK(wcsstr(image,L"ntcon.exe")!=NULL);
    ZeroMemory(connection,sizeof(*connection));
    notification=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(notification);++registrations;
    CHECK(DuplicateHandle(GetCurrentProcess(),notification,GetCurrentProcess(),&connection->capability,SYNCHRONIZE,TRUE,0));
    CHECK(DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),&connection->process,SYNCHRONIZE,FALSE,0));
    error=frontend_service_start(notification,attached,&fixture_service);
    return error;
}
DWORD OpenNtBaseClientStartFrontend(uint64_t window,BOOL borrowed,HANDLE *root,
    HANDLE *capability,HANDLE *restored)
{
    frontend_connection value={0};DWORD error;
    (void)window;(void)borrowed;
    error=fixture_frontend_start(L"ntcon.exe",&value);
    *root=value.process;*capability=value.capability;*restored=NULL;
    return error;
}
DWORD OpenNtBaseClientReturnFrontendConsole(void){return ERROR_SUCCESS;}
DWORD OpenNtBaseClientFrontendIoDisconnected(void)
{
    CHECK(created && !channels[created-1]);
    CHECK(InterlockedCompareExchange(&stopped,0,0)==(LONG)created);
    ++disconnects;CHECK(SetEvent(disconnect_seen));return ERROR_SUCCESS;
}
DWORD OpenNtBaseClientWaitFrontendConsoleRestored(void){return ERROR_SUCCESS;}
DWORD OpenNtBaseClientAcquireFrontendRoot(uint64_t window,DWORD *create_root,
    HANDLE *root,HANDLE *capability,HANDLE *retire,HANDLE *restored)
{ (void)window;*create_root=1;*root=*capability=*retire=*restored=NULL;return 0; }
DWORD OpenNtBaseClientCancelFrontendRootReservation(void){return 0;}
void frontend_bootstrap_release(frontend_connection *connection)
{
    if(connection->capability)CloseHandle(connection->capability);
    if(connection->restored)CloseHandle(connection->restored);
    if(connection->process)CloseHandle(connection->process);
    ZeroMemory(connection,sizeof(*connection));
}
/* This fixture exercises scope/channel lifetime, not native presentation. */
struct frontend_session { DWORD unused; };
DWORD frontend_session_create(frontend_session **out)
{ *out=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(**out));return *out ? 0 : ERROR_NOT_ENOUGH_MEMORY; }
void frontend_session_cancel(frontend_session *value) { (void)value; }
DWORD frontend_session_park(frontend_session *value)
{ (void)value;CHECK(retirement_mode);InterlockedIncrement(&park_calls);CHECK(SetEvent(park_seen));return 0; }
DWORD frontend_session_drain(frontend_session *value)
{ (void)value;CHECK(retirement_mode);InterlockedIncrement(&drain_calls);return 0; }
DWORD OpenNtBaseClientFrontendUsage(DWORD *pending_count,DWORD *tasks)
{
    CHECK(retirement_mode);
    *pending_count=(DWORD)InterlockedCompareExchange(&usage_pending,0,0);*tasks=0;
    CHECK(SetEvent(usage_seen));return 0;
}
DWORD OpenNtBaseClientFrontendStateChanged(HANDLE *state_changed)
{
    *state_changed=NULL;
    CHECK(retirement_state);
    return DuplicateHandle(GetCurrentProcess(),retirement_state,GetCurrentProcess(),state_changed,
        SYNCHRONIZE,FALSE,0) ? ERROR_SUCCESS : GetLastError();
}
DWORD OpenNtBaseClientWorkerStateChanged(HANDLE *state_changed)
{
    *state_changed=CreateEventW(NULL,FALSE,FALSE,NULL);
    return *state_changed ? ERROR_SUCCESS : GetLastError();
}
DWORD OpenNtBaseClientRetireFrontend(void)
{
    LONG attempt;
    CHECK(retirement_mode);
    /* Admission can race the idle snapshot. Model the broker's subsequent
     * state mutation: only that event may authorize the retry. */
    attempt=InterlockedIncrement(&retire_calls);
    if(attempt==1) { CHECK(SetEvent(retirement_state)); return ERROR_BUSY; }
    return ERROR_SUCCESS;
}
DWORD OpenNtBaseClientRetireWorkerlessFrontend(DWORD *retired)
{
    *retired=(DWORD)InterlockedCompareExchange(&broker_shutdown,0,0);
    return ERROR_SUCCESS;
}
DWORD frontend_session_destroy(frontend_session *value) { if(value)HeapFree(GetProcessHeap(),0,value);return 0; }
DWORD run16_native_request_submit(HANDLE capability,const run16_native_start *start,HANDLE *out,HANDLE *receipt,DWORD *request)
{
    (void)capability;(void)start;*out=*receipt=NULL;*request=0;
    if(!native_mode)return ERROR_NOT_SUPPORTED;
    /* Deliberately not a completed process. Scope must close diagnostics and
     * use the broker receipt/result rather than wait on this reference. */
    *out=diagnostic_target=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(*out);
    *receipt=CreateEventW(NULL,TRUE,TRUE,NULL);CHECK(*receipt);
    *request=41;return ERROR_SUCCESS;
}
DWORD run16_native_request_finish(DWORD request,DWORD *exit_code,DWORD *completed)
{
    CHECK(native_mode && request==41);
    *completed=native_error!=ERROR_PROCESS_ABORTED;
    *exit_code=*completed ? 37 : 0;
    return native_error;
}
DWORD run16_native_request_resume(HANDLE capability)
{ (void)capability;CHECK(FALSE);return ERROR_NOT_SUPPORTED; }
DWORD OpenNtBaseClientSelectNativeWorker(HANDLE *worker)
{
    *worker=NULL;
    if(!native_mode)return ERROR_NOT_SUPPORTED;
    CHECK(DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),worker,SYNCHRONIZE,FALSE,0));
    return ERROR_SUCCESS;
}
DWORD OpenNtBaseClientRequestFrontend(HANDLE capability)
{ (void)capability;CHECK(native_mode);return ERROR_SUCCESS; }
DWORD OpenNtBaseClientReserveNativeWorker(uint64_t *reservation)
{ *reservation=0;return ERROR_NOT_SUPPORTED; }
DWORD OpenNtBaseClientReleaseWorker(uint64_t reservation)
{ (void)reservation;return ERROR_NOT_SUPPORTED; }

DWORD OpenNtBaseClientRegisterFrontendRoot(HANDLE value)
{
    ++registrations;
    return DuplicateHandle(GetCurrentProcess(),value,GetCurrentProcess(),
        &notification,0,FALSE,DUPLICATE_SAME_ACCESS) ? 0 : GetLastError();
}
DWORD OpenNtBaseClientStartNativeWorker(DWORD machine,HANDLE *worker)
{CHECK(machine==IMAGE_FILE_MACHINE_I386 || machine==IMAGE_FILE_MACHINE_AMD64);return OpenNtBaseClientSelectNativeWorker(worker);}
DWORD OpenNtBaseClientFrontendJoinCandidate(DWORD *nonce,DWORD *pid)
{*nonce=*pid=0;return ERROR_NOT_FOUND;}
DWORD OpenNtBaseClientFrontendJoinDecision(DWORD nonce,BOOL same)
{(void)nonce;(void)same;return ERROR_INVALID_STATE;}
DWORD OpenNtBaseClientFrontendLeaseReady(void){CHECK(retirement_mode);return ERROR_SUCCESS;}
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
    if(disconnect_pending) {
        CHECK(!pending);disconnect_pending=FALSE;*request=0;*worker=NULL;
        error=ERROR_SUCCESS;
    } else if (pending) {
        *request=pending;pending=0;
        error=DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),
            GetCurrentProcess(),worker,SYNCHRONIZE,FALSE,0) ? 0 : GetLastError();
    } else ResetEvent(notification);
    LeaveCriticalSection(&lock);
    return error;
}
static DWORD WINAPI channel_main(void *value)
{
    frontend_io_channel *channel=value;
    return WaitForSingleObject(channel->release,INFINITE)==WAIT_OBJECT_0 ? 0 : 1;
}
DWORD frontend_io_channel_start_request(DWORD request,HANDLE worker,
    frontend_session *native,frontend_io_channel **output)
{
    frontend_io_channel *channel;
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
HANDLE frontend_io_channel_thread(frontend_io_channel *channel)
{
    return channel->thread;
}
DWORD frontend_io_channel_stop(frontend_io_channel *channel)
{
    CHECK(SetEvent(channel->release));
    CHECK(WaitForSingleObject(channel->thread,5000)==WAIT_OBJECT_0);
    CloseHandle(channel->thread);CloseHandle(channel->release);
    channels[channel->index]=NULL;
    HeapFree(GetProcessHeap(),0,channel);
    InterlockedIncrement(&stopped);
    return ERROR_SUCCESS;
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
static void service_controls_retirement(void)
{
    frontend_session_service *service=NULL;
    HANDLE creator=CreateEventW(NULL,TRUE,TRUE,NULL);
    usage_seen=CreateEventW(NULL,TRUE,FALSE,NULL);
    park_seen=CreateEventW(NULL,TRUE,FALSE,NULL);
    retirement_state=CreateEventW(NULL,FALSE,FALSE,NULL);
    CHECK(creator && usage_seen && park_seen && retirement_state);
    retirement_mode=TRUE;usage_pending=1;
    CHECK(!frontend_service_start_process(notification,creator,notification,FALSE,&service));
    CHECK(WaitForSingleObject(usage_seen,5000)==WAIT_OBJECT_0);
    CHECK(WaitForSingleObject(frontend_service_thread(service),0)==WAIT_TIMEOUT);
    CHECK(!retire_calls && !drain_calls);
    InterlockedExchange(&usage_pending,0);CHECK(SetEvent(retirement_state));
    CHECK(WaitForSingleObject(park_seen,5000)==WAIT_OBJECT_0);
    CHECK(WaitForSingleObject(frontend_service_thread(service),0)==WAIT_TIMEOUT);
    CHECK(park_calls==1 && !retire_calls && !drain_calls);
    /* A stale pending admission must never veto the broker's close order. */
    InterlockedExchange(&usage_pending,1);InterlockedExchange(&broker_shutdown,1);
    CHECK(SetEvent(retirement_state));
    CHECK(WaitForSingleObject(frontend_service_thread(service),5000)==WAIT_OBJECT_0);
    CHECK(!retire_calls && !drain_calls);
    frontend_service_close(service);
    retirement_mode=FALSE;CloseHandle(retirement_state);retirement_state=NULL;
    CloseHandle(usage_seen);CloseHandle(park_seen);CloseHandle(creator);
    puts("PASS completed lease parks without self-retirement; broker shutdown overrides stale pending usage");
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
    disconnect_seen=CreateEventW(NULL,FALSE,FALSE,NULL);CHECK(disconnect_seen);
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
    {
        static const DWORD errors[]={0,ERROR_WRITE_FAULT,ERROR_PROCESS_ABORTED};
        run16_native_start start={0};DWORD case_index,flags,result,completed,before,after;
        WCHAR application[MAX_PATH];
        CHECK(GetModuleFileNameW(NULL,application,ARRAYSIZE(application)));
        start.application=application;
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
        native_mode=TRUE;
        for(case_index=0;case_index<ARRAYSIZE(errors);++case_index) {
            native_error=errors[case_index];
            CHECK(!run16_frontend_scope_launch_win32_text(scope,&start));
            CHECK(!GetHandleInformation(diagnostic_target,&flags) && GetLastError()==ERROR_INVALID_HANDLE);
            result=completed=99;
            CHECK(run16_frontend_scope_wait_native(scope,&result,&completed)==native_error);
            CHECK(completed==(DWORD)(native_error!=ERROR_PROCESS_ABORTED));
            CHECK(result==(completed ? 37u : 0u));
        }
        native_mode=FALSE;
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&after) && after==before);
        puts("PASS native receipt alone distinguishes target completion/final-I/O failure/unfinished failure; diagnostic reference closed before wait; no leaked handles");
    }
    for (i=0;i<COUNT;++i) {
        submit(i+1);
        CHECK(channels[i] && WaitForSingleObject(channels[i]->thread,0)==WAIT_TIMEOUT);
        CHECK(InterlockedCompareExchange(&stopped,0,0)==(LONG)i);
        if(i+1==COUNT)break; /* Keep the final physical lease alive. */
        /* NTSRV, not an EOF or a new claimant, orders disposal. The
         * acknowledgment may only follow joining and freeing this lease. */
        EnterCriticalSection(&lock);
        CHECK(!pending && !disconnect_pending);disconnect_pending=TRUE;
        CHECK(SetEvent(notification));LeaveCriticalSection(&lock);
        CHECK(WaitForSingleObject(disconnect_seen,5000)==WAIT_OBJECT_0);
        CHECK(!channels[i] && disconnects==i+1);
    }
    run16_frontend_scope_end(scope);
    CHECK(channels[COUNT-1] && WaitForSingleObject(channels[COUNT-1]->thread,0)==WAIT_TIMEOUT);
    frontend_service_close(fixture_service);
    CHECK(stopped==COUNT);
    for (i=0;i<COUNT;++i) CHECK(!channels[i]);
    service_controls_retirement();
    CloseHandle(notification);CloseHandle(ready);CloseHandle(disconnect_seen);DeleteCriticalSection(&lock);
    puts("PASS completed channels reclaimed; live channel preserved; final join complete");
    return 0;
}
