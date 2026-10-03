#include "ntvwm-exe/execution.h"
#include "common/protocol/frontend_protocol.h"
#include "common/codec/native_launch.h"
#include <stdio.h>

static FILE *log;
static volatile LONG checks,failures,completed_resume_count,completed_direct_count,completed_cleanup_count;
static unsigned serial;
static DWORD completion_error,observed_broker_fault;
static DWORD completed_exit,completed_io_error,completed_io_flags;
typedef struct startup_observation {
    HANDLE ready,target,receipt;
    DWORD request,status;
} startup_observation;
static startup_observation observations[1024];
#define CHECK(x) do {InterlockedIncrement(&checks);if(!(x)){InterlockedIncrement(&failures);fprintf(log,"FAIL %d %s\n",__LINE__,#x);}} while(0)

BOOL ntvwm_console_quiescent(DWORD completed_target)
{ (void)completed_target;return FALSE; } /* No carrier in this fixture. */
static void record_broker_fault(void *context,DWORD error)
{ (void)context;observed_broker_fault=error; }
DWORD ntvwm_complete_next_command(DWORD request,DWORD exit_code)
{ (void)exit_code;if(!request)InterlockedIncrement(&completed_resume_count);else InterlockedIncrement(&completed_cleanup_count);return completion_error; }
DWORD ntvwm_complete_native_request(DWORD request,DWORD exit_code,DWORD io_error,DWORD io_flags)
{
    if(!request)return ERROR_INVALID_PARAMETER;
    completed_exit=exit_code;completed_io_error=io_error;completed_io_flags=io_flags;
    InterlockedIncrement(&completed_direct_count);return completion_error;
}
DWORD OpenNtBaseClientBindNativeTarget(DWORD request,HANDLE target,HANDLE receipt)
{ return request && target && receipt ? ERROR_SUCCESS : ERROR_INVALID_PARAMETER; }

/* Substitute the authenticated RPC carrier only. Observations are local typed
 * handles, not old control headers or trusted sender-local handle numbers. */
DWORD OpenNtBaseClientNativeStartupResult(DWORD generation,DWORD request,DWORD status,HANDLE target,HANDLE receipt)
{
    startup_observation *result;
    CHECK(generation && generation<ARRAYSIZE(observations));
    if(!generation || generation>=ARRAYSIZE(observations))return ERROR_INVALID_PARAMETER;
    result=&observations[generation];
    CHECK(result->ready && result->request==request);
    CHECK(WaitForSingleObject(result->ready,0)==WAIT_TIMEOUT);
    if(status || !request)CHECK(!target && !receipt);
    else {
        CHECK(target && receipt);
        if(!DuplicateHandle(GetCurrentProcess(),target,GetCurrentProcess(),&result->target,
                SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,0) ||
            !DuplicateHandle(GetCurrentProcess(),receipt,GetCurrentProcess(),&result->receipt,
                SYNCHRONIZE,FALSE,0))return GetLastError();
    }
    result->status=status;
    return SetEvent(result->ready) ? ERROR_SUCCESS : GetLastError();
}
static void dispose_observation(unsigned id)
{
    startup_observation *result=&observations[id];
    if(result->target)CloseHandle(result->target);
    if(result->receipt)CloseHandle(result->receipt);
    if(result->ready)CloseHandle(result->ready);
    ZeroMemory(result,sizeof(*result));
}
static startup_observation *collect(unsigned id)
{
    CHECK(id && id<ARRAYSIZE(observations));
    if(!id || id>=ARRAYSIZE(observations))return NULL;
    CHECK(WaitForSingleObject(observations[id].ready,5000)==WAIT_OBJECT_0);
    return &observations[id];
}
static void dispose_unstarted_command(ntvwm_next_command *command)
{
    if(command->payload)HeapFree(GetProcessHeap(),0,command->payload);
    if(command->sender)CloseHandle(command->sender);
    if(command->execution)CloseHandle(command->execution);
    if(command->frontend)CloseHandle(command->frontend);
    ZeroMemory(command,sizeof(*command));
}
static unsigned submit_kind(ntvwm_executions *owner,const BYTE *payload,DWORD bytes,
    DWORD access,DWORD preflight_error,DWORD request_id,BOOL gui)
{
    ntvwm_next_command command={0};DWORD error;unsigned id=++serial;
    CHECK(id<ARRAYSIZE(observations));if(id>=ARRAYSIZE(observations))return 0;
    observations[id].ready=CreateEventW(NULL,TRUE,FALSE,NULL);
    observations[id].request=request_id;
    CHECK(observations[id].ready!=NULL);
    command.bytes=bytes;command.request=request_id;command.caller_generation=id;
    if(bytes) {
        /* Oversize declarations must be rejected without reading the body. */
        DWORD allocated=bytes>NATIVE_LAUNCH_MAX_BYTES ? 1 : bytes;
        command.payload=HeapAlloc(GetProcessHeap(),0,allocated);
        CHECK(command.payload!=NULL);
        if(command.payload)CopyMemory(command.payload,payload,allocated);
    }
    if(!gui) {
        command.frontend=CreateEventW(NULL,TRUE,FALSE,NULL);
        command.execution=CreateEventW(NULL,TRUE,FALSE,NULL);
        CHECK(command.frontend && command.execution);
    }
    CHECK(DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),
        &command.sender,access,FALSE,0));
    if(!observations[id].ready || (!gui && (!command.frontend || !command.execution)) || !command.sender ||
        (bytes && !command.payload)) {
        dispose_unstarted_command(&command);dispose_observation(id);return 0;
    }
    error=ntvwm_execution_start(owner,&command,preflight_error);CHECK(!error);
    if(error){dispose_unstarted_command(&command);dispose_observation(id);return 0;}
    CHECK(!command.payload && !command.bytes && !command.sender && !command.execution &&
        !command.frontend && !command.request && !command.caller_generation);
    return id;
}
static unsigned submit(ntvwm_executions *owner,const BYTE *payload,DWORD bytes,
    DWORD access,DWORD preflight_error,DWORD request_id)
{ return submit_kind(owner,payload,bytes,access,preflight_error,request_id,FALSE); }
#define SENDER_ACCESS (SYNCHRONIZE|PROCESS_DUP_HANDLE|PROCESS_QUERY_LIMITED_INFORMATION)
static DWORD make_target_packet(const WCHAR *argument,HANDLE input,BYTE **payload,DWORD *bytes)
{
    WCHAR image[MAX_PATH],command[MAX_PATH+200],directory[MAX_PATH],*environment;
    run16_native_start start={0};DWORD error;
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));CHECK(GetCurrentDirectoryW(MAX_PATH,directory));
    swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" %ls",image,argument);
    environment=GetEnvironmentStringsW();CHECK(environment!=NULL);
    start.application=image;start.command=command;start.directory=directory;
    start.environment=environment;start.console_mask=input ? 6 : 7;start.standard[0]=input;
    error=run16_native_launch_pack(&start,payload,bytes);CHECK(!error);
    if(environment)FreeEnvironmentStringsW(environment);return error;
}
static SIZE_T busy_heap_bytes(void)
{
    PROCESS_HEAP_ENTRY entry={0};SIZE_T bytes=0;
    CHECK(HeapLock(GetProcessHeap()));
    while(HeapWalk(GetProcessHeap(),&entry))if(entry.wFlags&PROCESS_HEAP_ENTRY_BUSY)bytes+=entry.cbData;
    CHECK(GetLastError()==ERROR_NO_MORE_ITEMS);CHECK(HeapUnlock(GetProcessHeap()));return bytes;
}
static void invalid_arguments_do_not_allocate(void)
{
    ntvwm_executions *owner=NULL;ntvwm_next_command command={0};SIZE_T before,after;unsigned i;
    CHECK(!ntvwm_executions_open(&owner));if(!owner)return;
    before=busy_heap_bytes();
    for(i=0;i<32;++i) {
        CHECK(ntvwm_execution_start(NULL,&command,0)==ERROR_INVALID_PARAMETER);
        CHECK(ntvwm_execution_start(owner,NULL,0)==ERROR_INVALID_PARAMETER);
        CHECK(ntvwm_execution_start(NULL,NULL,0)==ERROR_INVALID_PARAMETER);
    }
    after=busy_heap_bytes();CHECK(before==after);
    CHECK(ntvwm_executions_idle(owner));ntvwm_executions_close(owner);
}
static void launch_packet_boundaries(void)
{
    run16_native_start start={0};run16_native_launch_packet decoded;
    WCHAR *strings[4],*environment;BYTE *payload=NULL;DWORD bytes=0;
    DWORD count=(NATIVE_LAUNCH_MAX_BYTES-sizeof(decoded))/sizeof(WCHAR)-5,i;
    environment=HeapAlloc(GetProcessHeap(),0,(count+1)*sizeof(WCHAR));
    CHECK(environment!=NULL);if(!environment)return;
    for(i=0;i<count+1;++i)environment[i]=L'A';
    environment[1]=L'=';environment[count-2]=environment[count-1]=0;
    start.command=L"x";start.directory=L".";start.environment=environment;
    CHECK(!run16_native_launch_pack(&start,&payload,&bytes));
    CHECK(bytes==NATIVE_LAUNCH_MAX_BYTES && bytes>65536);
    if(payload) {
        CHECK(!run16_native_launch_unpack(payload,bytes,&decoded,strings));
        CHECK(run16_native_launch_unpack(payload,bytes+2,&decoded,strings)==ERROR_INVALID_DATA);
        CHECK(run16_native_launch_unpack(payload,bytes,NULL,strings)==ERROR_INVALID_DATA);
        CHECK(run16_native_launch_unpack(payload,bytes,&decoded,NULL)==ERROR_INVALID_DATA);
        ((run16_native_launch_packet *)payload)->characters[3]=MAXDWORD;
        CHECK(run16_native_launch_unpack(payload,bytes,&decoded,strings)==ERROR_INVALID_DATA);
        HeapFree(GetProcessHeap(),0,payload);payload=NULL;
    }
    environment[count-2]=L'A';environment[count]=0;
    CHECK(run16_native_launch_pack(&start,&payload,&bytes)==ERROR_BUFFER_OVERFLOW);
    CHECK(!payload && !bytes);HeapFree(GetProcessHeap(),0,environment);
}
static void malformed_commands(void)
{
    ntvwm_executions *owner=NULL;BYTE invalid=0;unsigned i,id;
    DWORD lengths[]={NATIVE_LAUNCH_MAX_BYTES+1,MAXDWORD};
    CHECK(!ntvwm_executions_open(&owner));if(!owner)return;
    for(i=0;i<14;++i) {
        DWORD baseline,current;startup_observation *result;
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&baseline));
        id=submit(owner,&invalid,i<12 ? 1 : lengths[i-12],SENDER_ACCESS,0,1);
        if(!id)continue;
        result=collect(id);CHECK(result && result->status==ERROR_INVALID_DATA && !result->target && !result->receipt);
        CHECK(!ntvwm_executions_wait_idle(owner));dispose_observation(id);
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&current) && current==baseline);
    }
    ntvwm_executions_close(owner);
}
typedef struct resume_state { unsigned sequence;DWORD begin_error,end_error; } resume_state;
static DWORD resume_begin(void *context,HANDLE stop)
{ resume_state *state=context;CHECK(state->sequence++==0);CHECK(WaitForSingleObject(stop,0)==WAIT_TIMEOUT);return state->begin_error; }
static void resume_release(void *context)
{ resume_state *state=context;CHECK(state->sequence++==1); }
static DWORD resume_end(void *context)
{ resume_state *state=context;CHECK(state->sequence++==2);return state->end_error; }
static void resume_barrier(DWORD begin_error,DWORD end_error)
{
    ntvwm_executions *owner=NULL;unsigned id;startup_observation *result;
    resume_state state={0,begin_error,end_error};
    ntvwm_execution_io io={&state,resume_begin,resume_end,resume_release};LONG before=completed_resume_count;
    CHECK(!ntvwm_executions_open(&owner));if(!owner)return;
    ntvwm_executions_bind_io(owner,&io);id=submit(owner,NULL,0,SENDER_ACCESS,0,0);
    if(id) {
        result=collect(id);CHECK(result && result->status==(begin_error ? begin_error : end_error));
        CHECK(!result->target && !result->receipt);
    }
    ntvwm_executions_close(owner);
    CHECK(completed_resume_count==before+1 && state.sequence==(begin_error ? 1u : 3u));
    if(id)dispose_observation(id);
}
typedef struct blocked_io { volatile LONG entered;HANDLE all_entered; } blocked_io;
static DWORD blocked_begin(void *context,HANDLE stop)
{
    blocked_io *state=context;
    if(InterlockedIncrement(&state->entered)==16)SetEvent(state->all_entered);
    return WaitForSingleObject(stop,INFINITE)==WAIT_OBJECT_0 ? ERROR_OPERATION_ABORTED : GetLastError();
}
static void concurrent_io_cancellation(void)
{
    ntvwm_executions *owner=NULL;unsigned ids[16]={0},i;BYTE *payload=NULL;DWORD bytes;
    LONG before=completed_cleanup_count;
    blocked_io state={0};ntvwm_execution_io io={&state,blocked_begin,NULL,NULL};
    CHECK(!ntvwm_executions_open(&owner));if(!owner)return;
    state.all_entered=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(state.all_entered!=NULL);
    ntvwm_executions_bind_io(owner,&io);
    if(!make_target_packet(L"--completed-target",NULL,&payload,&bytes)) {
        for(i=0;i<16;++i)ids[i]=submit(owner,payload,bytes,SENDER_ACCESS,0,1);
        CHECK(WaitForSingleObject(state.all_entered,5000)==WAIT_OBJECT_0);CHECK(state.entered==16);
    }
    ntvwm_executions_close(owner);
    CHECK(completed_cleanup_count==before+16);
    for(i=0;i<16;++i)if(ids[i]) {
        startup_observation *result=collect(ids[i]);
        CHECK(result && result->status==ERROR_OPERATION_ABORTED && !result->target && !result->receipt);
        dispose_observation(ids[i]);
    }
    if(payload)HeapFree(GetProcessHeap(),0,payload);if(state.all_entered)CloseHandle(state.all_entered);
}
static void no_launch_failure(BOOL preflight)
{
    ntvwm_executions *owner=NULL;HANDLE marker,input=NULL,output=NULL;
    WCHAR name[128],argument[180];BYTE *payload=NULL;DWORD bytes;unsigned id=0;
    swprintf_s(name,ARRAYSIZE(name),L"Local\\ntvwm-no-launch-%lu-%u",GetCurrentProcessId(),serial+1);
    swprintf_s(argument,ARRAYSIZE(argument),L"--probe-target %ls",name);
    marker=CreateEventW(NULL,TRUE,FALSE,name);CHECK(marker!=NULL);if(!marker)return;
    CHECK(!ntvwm_executions_open(&owner));if(!owner){CloseHandle(marker);return;}
    if(!preflight)CHECK(CreatePipe(&input,&output,NULL,0));
    if(!make_target_packet(argument,input,&payload,&bytes)) {
        id=submit(owner,payload,bytes,preflight ? SENDER_ACCESS : SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,
            preflight ? ERROR_BUSY : 0,1);
        if(id) {
            startup_observation *result=collect(id);
            CHECK(result && result->status==(preflight ? ERROR_BUSY : ERROR_ACCESS_DENIED) && !result->target && !result->receipt);
        }
    }
    ntvwm_executions_close(owner);CHECK(WaitForSingleObject(marker,1500)==WAIT_TIMEOUT);
    if(id)dispose_observation(id);if(payload)HeapFree(GetProcessHeap(),0,payload);
    CloseHandle(marker);if(input)CloseHandle(input);if(output)CloseHandle(output);
}
static void broker_completion_failure_stops_reentry(void)
{
    ntvwm_executions *owner=NULL;BYTE invalid=0;unsigned id;
    CHECK(!ntvwm_executions_open(&owner));if(!owner)return;
    observed_broker_fault=0;ntvwm_executions_bind_fault(owner,record_broker_fault,NULL);
    completion_error=RPC_S_SERVER_UNAVAILABLE;id=submit(owner,&invalid,1,SENDER_ACCESS,0,1);
    if(id) {
        startup_observation *result=collect(id);CHECK(result && result->status==ERROR_INVALID_DATA);
        CHECK(ntvwm_executions_wait_idle(owner)==RPC_S_SERVER_UNAVAILABLE);
        CHECK(observed_broker_fault==RPC_S_SERVER_UNAVAILABLE);
    }
    ntvwm_executions_close(owner);completion_error=0;if(id)dispose_observation(id);
}
static DWORD target_begin(void *context,HANDLE stop)
{ (void)context;return WaitForSingleObject(stop,0)==WAIT_TIMEOUT ? ERROR_SUCCESS : ERROR_OPERATION_ABORTED; }
static DWORD target_end(void *context)
{ return *(DWORD *)context; }
static void target_case(BOOL held,DWORD io_error,DWORD broker_error)
{
    ntvwm_executions *owner=NULL;BYTE *payload=NULL;DWORD bytes;unsigned id=0;
    LONG before=completed_direct_count;
    ntvwm_execution_io io={&io_error,target_begin,target_end,NULL};
    CHECK(!ntvwm_executions_open(&owner));if(!owner)return;
    completion_error=broker_error;observed_broker_fault=0;
    ntvwm_executions_bind_fault(owner,record_broker_fault,NULL);
    if(!held)ntvwm_executions_bind_io(owner,&io);
    if(!make_target_packet(held ? L"--held-target" : L"--completed-target",NULL,&payload,&bytes)) {
        id=submit(owner,payload,bytes,SENDER_ACCESS,0,1);
        if(id) {
            startup_observation *result=collect(id);CHECK(result && !result->status && result->target && result->receipt);
            if(!held) {
                CHECK(ntvwm_executions_wait_idle(owner)==broker_error);
                CHECK(completed_direct_count==before+1 && completed_exit==73);
                CHECK(completed_io_error==io_error && !completed_io_flags);
                CHECK(observed_broker_fault==broker_error);
            }
        }
    }
    ntvwm_executions_close(owner);
    completion_error=0;
    if(id && held) {
        startup_observation *result=&observations[id];
        CHECK(result->target && WaitForSingleObject(result->target,0)==WAIT_TIMEOUT);
        CHECK(result->receipt && WaitForSingleObject(result->receipt,0)==WAIT_TIMEOUT);
        CHECK(completed_direct_count==before);
        /* Only the fixture kills its own held target after product cleanup. */
        if(result->target) {
            HANDLE cleanup=OpenProcess(PROCESS_TERMINATE|SYNCHRONIZE,FALSE,GetProcessId(result->target));
            CHECK(cleanup!=NULL);
            if(cleanup){CHECK(TerminateProcess(cleanup,0));CHECK(WaitForSingleObject(cleanup,5000)==WAIT_OBJECT_0);CloseHandle(cleanup);}
        }
    }
    if(id)dispose_observation(id);if(payload)HeapFree(GetProcessHeap(),0,payload);
}
static DWORD gui_forbidden_io(void *context,HANDLE stop)
{ (void)context;(void)stop;CHECK(FALSE);return ERROR_ACCESS_DENIED; }
static void raw_failed_create_control(void)
{
    WCHAR image[MAX_PATH];STARTUPINFOW startup={sizeof(startup)};
    PROCESS_INFORMATION process={0};DWORD before=0,after=0,error=0;
    CHECK(GetModuleFileNameW(NULL,image,ARRAYSIZE(image)));
    image[wcslen(image)-1]=L'!';
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
    CHECK(!CreateProcessW(image,NULL,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&startup,&process));
    error=GetLastError();CHECK(error==ERROR_FILE_NOT_FOUND);
    CHECK(!process.hProcess && !process.hThread);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after));
    fprintf(log,"RAW-CreateProcess-failure handles-before=%lu handles-after=%lu error=%lu\n",before,after,error);
}
static void gui_startup_failure_cleans_request(void)
{
    ntvwm_executions *owner=NULL;BYTE *payload=NULL;DWORD bytes;unsigned id=0;
    DWORD handles_before=0,handles_after=0;
    run16_native_launch_packet header;WCHAR *strings[4];
    LONG before=completed_cleanup_count;
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&handles_before));
    CHECK(!ntvwm_executions_open(&owner));if(!owner)return;
    if(!make_target_packet(L"--completed-target",NULL,&payload,&bytes)) {
        CHECK(!run16_native_launch_unpack(payload,bytes,&header,strings));
        strings[0][wcslen(strings[0])-1]=L'!';
        CHECK(GetFileAttributesW(strings[0])==INVALID_FILE_ATTRIBUTES);
        id=submit_kind(owner,payload,bytes,SENDER_ACCESS,0,1,TRUE);
        if(id) {
            startup_observation *result=collect(id);
            CHECK(result && result->status==ERROR_FILE_NOT_FOUND && !result->target && !result->receipt);
            CHECK(!ntvwm_executions_wait_idle(owner));
            CHECK(completed_cleanup_count==before+1);
        }
    }
    ntvwm_executions_close(owner);
    if(id)dispose_observation(id);if(payload)HeapFree(GetProcessHeap(),0,payload);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&handles_after));
    fprintf(log,"GUI-failed-start handles-before=%lu handles-after=%lu\n",handles_before,handles_after);
}
static void gui_startup_releases_execution(void)
{
    ntvwm_executions *owner=NULL;BYTE *payload=NULL;DWORD bytes;unsigned id=0;
    DWORD denied=ERROR_ACCESS_DENIED;HANDLE cleanup=NULL;
    LONG direct_before=completed_direct_count,cleanup_before=completed_cleanup_count;
    ntvwm_execution_io io={&denied,gui_forbidden_io,target_end,NULL};
    CHECK(!ntvwm_executions_open(&owner));if(!owner)return;
    /* An accidental text bind would reject this startup. This fixture tests
     * execution ownership, not subsystem discovery (the actual GUI integration
     * probe separately exercises the GUI image through the real broker). */
    ntvwm_executions_bind_io(owner,&io);
    if(!make_target_packet(L"--held-target",NULL,&payload,&bytes)) {
        id=submit_kind(owner,payload,bytes,SENDER_ACCESS,0,1,TRUE);
        if(id) {
            startup_observation *result=collect(id);
            CHECK(result && !result->status && result->target && result->receipt);
            CHECK(!ntvwm_executions_wait_idle(owner));
            CHECK(result->target && WaitForSingleObject(result->target,0)==WAIT_TIMEOUT);
            CHECK(completed_direct_count==direct_before && completed_cleanup_count==cleanup_before);
        }
    }
    ntvwm_executions_close(owner);
    if(id && observations[id].target) {
        CHECK(WaitForSingleObject(observations[id].target,0)==WAIT_TIMEOUT);
        cleanup=OpenProcess(PROCESS_TERMINATE|SYNCHRONIZE,FALSE,GetProcessId(observations[id].target));
        CHECK(cleanup!=NULL);
        if(cleanup){CHECK(TerminateProcess(cleanup,0));CHECK(WaitForSingleObject(cleanup,5000)==WAIT_OBJECT_0);CloseHandle(cleanup);}
    }
    if(id)dispose_observation(id);if(payload)HeapFree(GetProcessHeap(),0,payload);
}
int wmain(int argc,WCHAR **argv)
{
    DWORD baseline,current;
    if(argc==3 && !lstrcmpW(argv[1],L"--probe-target")) {
        HANDLE marker=OpenEventW(EVENT_MODIFY_STATE,FALSE,argv[2]);
        if(!marker)return 4;SetEvent(marker);CloseHandle(marker);return 0;
    }
    if(argc==2 && !lstrcmpW(argv[1],L"--held-target")){Sleep(10000);return 73;}
    if(argc==2 && !lstrcmpW(argv[1],L"--completed-target"))return 73;
    if(argc==3 && !lstrcmpW(argv[1],L"--invalid-arguments")) {
        if(_wfopen_s(&log,argv[2],L"wx"))return 2;
        invalid_arguments_do_not_allocate();
        fprintf(log,"INVALID-ARGUMENT-HEAP checks=%ld failures=%ld\n",checks,failures);
        fclose(log);return failures ? 1 : 0;
    }
    if(argc!=2 || _wfopen_s(&log,argv[1],L"wx"))return 2;
    invalid_arguments_do_not_allocate();launch_packet_boundaries();target_case(TRUE,0,0);
    {HANDLE input=NULL,output=NULL;CHECK(CreatePipe(&input,&output,NULL,0));
        if(input)CloseHandle(input);if(output)CloseHandle(output);}
    /* Control the OS's first failed CreateProcess initialization separately.
     * Its measured handle growth occurs without any project launch/execution
     * code. The unchanged aggregate assertion below still covers all repeated
     * production requests, including real GUI creation failures. */
    raw_failed_create_control();
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&baseline));
    malformed_commands();concurrent_io_cancellation();target_case(TRUE,0,0);
    gui_startup_releases_execution();
    {unsigned attempt;for(attempt=0;attempt<8;++attempt)gui_startup_failure_cleans_request();}
    no_launch_failure(FALSE);no_launch_failure(TRUE);broker_completion_failure_stops_reentry();
    target_case(FALSE,0,0);target_case(FALSE,ERROR_WRITE_FAULT,0);
    target_case(FALSE,ERROR_BROKEN_PIPE,0);target_case(FALSE,0,RPC_S_SERVER_UNAVAILABLE);
    resume_barrier(ERROR_SUCCESS,ERROR_SUCCESS);resume_barrier(ERROR_ACCESS_DENIED,ERROR_SUCCESS);
    resume_barrier(ERROR_SUCCESS,ERROR_PIPE_NOT_CONNECTED);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&current) && current==baseline);
    fprintf(log,"NTVWM-REQUEST-LIFETIME checks=%ld failures=%ld completed=%ld cancelled=16 target-survival=yes remaining-handles=%ld\n",
        checks,failures,completed_direct_count+completed_cleanup_count+completed_resume_count,(long)(current-baseline));
    fclose(log);return failures ? 1 : 0;
}
