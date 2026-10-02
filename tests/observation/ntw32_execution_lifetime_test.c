#include "ntw32-exe/execution.h"
#include "interface/frontend_protocol.h"
#include "interface/native_launch.h"
#include <sddl.h>
#include <stdio.h>
static FILE *log;
static unsigned checks,failures,serial;
static DWORD completion_error;
static volatile LONG completed_resume_count;
static DWORD observed_broker_fault;
static void record_broker_fault(void *context,DWORD error)
{ (void)context;observed_broker_fault=error; }
/* This fixture owns attachments directly, without a broker delivery lease. */
DWORD ntw32_complete_next_command(DWORD request,DWORD exit_code)
{ (void)exit_code;if(!request)InterlockedIncrement(&completed_resume_count);return completion_error; }
DWORD OpenNtBaseClientBindNativeTarget(DWORD request,HANDLE target,HANDLE receipt)
{ return request && target && receipt ? ERROR_SUCCESS : ERROR_INVALID_PARAMETER; }
#define CHECK(x) do {++checks;if(!(x)){++failures;fprintf(log,"FAIL %d %s\n",__LINE__,#x);}} while(0)
static HANDLE submit_access_kind(ntw32_executions *owner,DWORD access,DWORD preflight_error,DWORD request_id)
{
    WCHAR name[128];HANDLE server,client,process=NULL,frontend,execution;PSECURITY_DESCRIPTOR descriptor=NULL;
    SECURITY_ATTRIBUTES attributes={sizeof(attributes),NULL,FALSE};
    DWORD error;
    swprintf_s(name,128,L"\\\\.\\pipe\\ntw32-lifetime-%lu-%u",GetCurrentProcessId(),++serial);
    /* This is an in-process lifetime fixture.  Its local client must not
     * inherit a restrictive interactive-session pipe DACL. */
    CHECK(ConvertStringSecurityDescriptorToSecurityDescriptorW(L"D:(A;;GA;;;WD)",SDDL_REVISION_1,
        &descriptor,NULL));
    if(!descriptor)return NULL;
    attributes.lpSecurityDescriptor=descriptor;
    server=CreateNamedPipeW(name,PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED|FILE_FLAG_FIRST_PIPE_INSTANCE,
        PIPE_TYPE_BYTE|PIPE_WAIT,1,1024,1024,0,&attributes);
    LocalFree(descriptor);
    CHECK(server!=INVALID_HANDLE_VALUE);if(server==INVALID_HANDLE_VALUE)return NULL;
    client=CreateFileW(name,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
    CHECK(client!=INVALID_HANDLE_VALUE);if(client==INVALID_HANDLE_VALUE){CloseHandle(server);return NULL;}
    /* Client has already connected; there is no pending connect operation. */
    frontend=CreateEventW(NULL,TRUE,FALSE,NULL);execution=CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(frontend && execution);
    CHECK(DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),
        &process,access,FALSE,0));
    {
        ntw32_next_command command={server,process,execution,frontend,request_id};
        error=ntw32_execution_start(owner,&command,preflight_error);
        CHECK(!command.channel && !command.sender && !command.execution && !command.frontend && !command.request);
    }
    CHECK(!error);if(error){CloseHandle(client);return NULL;}
    return client;
}
static HANDLE submit_access(ntw32_executions *owner,DWORD access,DWORD preflight_error)
{
    return submit_access_kind(owner,access,preflight_error,1);
}
static HANDLE submit(ntw32_executions *owner)
{
    return submit_access(owner,SYNCHRONIZE|PROCESS_DUP_HANDLE|PROCESS_QUERY_LIMITED_INFORMATION,ERROR_SUCCESS);
}
typedef struct resume_state { unsigned sequence; DWORD begin_error,end_error; } resume_state;
static DWORD resume_begin(void *context,HANDLE stop)
{
    resume_state *state=context;
    CHECK(state->sequence++==0);
    CHECK(WaitForSingleObject(stop,0)==WAIT_TIMEOUT);
    return state->begin_error;
}
static void resume_release(void *context)
{ resume_state *state=context;CHECK(state->sequence++==1); }
static DWORD resume_end(void *context)
{ resume_state *state=context;CHECK(state->sequence++==2);return state->end_error; }
static void resume_barrier(DWORD begin_error,DWORD end_error)
{
    ntw32_executions *owner=NULL;HANDLE client;DWORD bytes;
    resume_state state={0,begin_error,end_error};
    ntw32_execution_io io={&state,resume_begin,resume_end,resume_release};
    native_request_header header={NATIVE_REQUEST_VERSION,0};native_request_reply reply={0};
    LONG before=completed_resume_count;
    CHECK(!ntw32_executions_open(&owner));if(!owner)return;
    ntw32_executions_bind_io(owner,&io);
    client=submit_access_kind(owner,SYNCHRONIZE|PROCESS_DUP_HANDLE|
        PROCESS_QUERY_LIMITED_INFORMATION,ERROR_SUCCESS,0);
    if(client) {
        CHECK(WriteFile(client,&header,sizeof(header),&bytes,NULL) && bytes==sizeof(header));
        CHECK(ReadFile(client,&reply,sizeof(reply),&bytes,NULL) && bytes==sizeof(reply));
        CHECK(reply.version==NATIVE_REQUEST_VERSION);
        CHECK(reply.error==(begin_error ? begin_error : end_error));
        CHECK(!reply.target && !reply.receipt);
        CHECK(!ReadFile(client,&reply,1,&bytes,NULL) && GetLastError()==ERROR_BROKEN_PIPE);
        CloseHandle(client);
    }
    ntw32_executions_close(owner);
    CHECK(completed_resume_count==before+1);
    CHECK(state.sequence==(begin_error ? 1u : 3u));
}
static void export_failure_does_not_launch(void)
{
    ntw32_executions *owner=NULL;HANDLE client,marker;
    WCHAR image[MAX_PATH],command[MAX_PATH+180],directory[MAX_PATH],name[128],*environment;
    run16_native_start start={0};BYTE *payload=NULL;DWORD size,bytes;
    native_request_header header={NATIVE_REQUEST_VERSION,0};native_request_reply reply={0};
    swprintf_s(name,ARRAYSIZE(name),L"Local\\ntw32-no-launch-%lu-%u",GetCurrentProcessId(),++serial);
    marker=CreateEventW(NULL,TRUE,FALSE,name);CHECK(marker!=NULL);if(!marker)return;
    CHECK(!ntw32_executions_open(&owner));if(!owner){CloseHandle(marker);return;}
    /* Deliberately omit PROCESS_DUP_HANDLE: the real OS rejects the export,
     * without replacing CreateProcess or the production request executor. */
    client=submit_access(owner,SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,ERROR_SUCCESS);
    if(!client){ntw32_executions_close(owner);CloseHandle(marker);return;}
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));
    CHECK(GetCurrentDirectoryW(MAX_PATH,directory));
    swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" --probe-target %ls",image,name);
    environment=GetEnvironmentStringsW();CHECK(environment!=NULL);
    start.application=image;start.command=command;start.directory=directory;
    start.environment=environment;start.console_mask=7;
    CHECK(!run16_native_launch_pack(&start,&payload,&size));
    if(payload){
        header.bytes=size;
        CHECK(WriteFile(client,&header,sizeof(header),&bytes,NULL) && bytes==sizeof(header));
        CHECK(WriteFile(client,payload,size,&bytes,NULL) && bytes==size);
        CHECK(ReadFile(client,&reply,sizeof(reply),&bytes,NULL) && bytes==sizeof(reply));
        CHECK(reply.version==NATIVE_REQUEST_VERSION && reply.error==ERROR_ACCESS_DENIED && !reply.target && !reply.receipt);
        CHECK(!ReadFile(client,&reply,1,&bytes,NULL) && GetLastError()==ERROR_BROKEN_PIPE);
        HeapFree(GetProcessHeap(),0,payload);
    }
    ntw32_executions_close(owner);
    CHECK(WaitForSingleObject(marker,1500)==WAIT_TIMEOUT);
    CloseHandle(client);CloseHandle(marker);FreeEnvironmentStringsW(environment);
}
static void preflight_failure_replies_before_launch(void)
{
    ntw32_executions *owner=NULL;HANDLE client,marker;
    WCHAR image[MAX_PATH],command[MAX_PATH+180],directory[MAX_PATH],name[128],*environment;
    run16_native_start start={0};BYTE *payload=NULL;DWORD size,bytes;
    native_request_header header={NATIVE_REQUEST_VERSION,0};native_request_reply reply={0};
    swprintf_s(name,ARRAYSIZE(name),L"Local\\ntw32-preflight-%lu-%u",GetCurrentProcessId(),++serial);
    marker=CreateEventW(NULL,TRUE,FALSE,name);CHECK(marker!=NULL);if(!marker)return;
    CHECK(!ntw32_executions_open(&owner));if(!owner){CloseHandle(marker);return;}
    client=submit_access(owner,SYNCHRONIZE|PROCESS_DUP_HANDLE|PROCESS_QUERY_LIMITED_INFORMATION,ERROR_BUSY);
    if(!client){ntw32_executions_close(owner);CloseHandle(marker);return;}
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));CHECK(GetCurrentDirectoryW(MAX_PATH,directory));
    swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" --probe-target %ls",image,name);
    environment=GetEnvironmentStringsW();CHECK(environment!=NULL);
    start.application=image;start.command=command;start.directory=directory;
    start.environment=environment;start.console_mask=7;
    CHECK(!run16_native_launch_pack(&start,&payload,&size));
    if(payload) {
        header.bytes=size;
        CHECK(WriteFile(client,&header,sizeof(header),&bytes,NULL) && bytes==sizeof(header));
        CHECK(WriteFile(client,payload,size,&bytes,NULL) && bytes==size);
        CHECK(ReadFile(client,&reply,sizeof(reply),&bytes,NULL) && bytes==sizeof(reply));
        CHECK(reply.version==NATIVE_REQUEST_VERSION && reply.error==ERROR_BUSY && !reply.target && !reply.receipt);
        CHECK(!ReadFile(client,&reply,1,&bytes,NULL) && GetLastError()==ERROR_BROKEN_PIPE);
        HeapFree(GetProcessHeap(),0,payload);
    }
    ntw32_executions_close(owner);
    CHECK(WaitForSingleObject(marker,1500)==WAIT_TIMEOUT);
    CloseHandle(client);CloseHandle(marker);FreeEnvironmentStringsW(environment);
}
static void broker_completion_failure_stops_reentry(void)
{
    ntw32_executions *owner=NULL;HANDLE client;DWORD bytes;
    native_request_header header={0,0};native_request_reply reply={0};
    CHECK(!ntw32_executions_open(&owner));if(!owner)return;
    observed_broker_fault=ERROR_SUCCESS;
    ntw32_executions_bind_fault(owner,record_broker_fault,NULL);
    completion_error=RPC_S_SERVER_UNAVAILABLE;
    client=submit(owner);
    if(client) {
        CHECK(WriteFile(client,&header,sizeof(header),&bytes,NULL) && bytes==sizeof(header));
        CHECK(ReadFile(client,&reply,sizeof(reply),&bytes,NULL) && bytes==sizeof(reply));
        CHECK(reply.version==NATIVE_REQUEST_VERSION && reply.error==ERROR_INVALID_DATA);
        CHECK(ntw32_executions_wait_idle(owner)==RPC_S_SERVER_UNAVAILABLE);
        CHECK(observed_broker_fault==RPC_S_SERVER_UNAVAILABLE);
        CloseHandle(client);
    }
    completion_error=ERROR_SUCCESS;
    ntw32_executions_close(owner);
}
static SIZE_T busy_heap_bytes(void)
{
    PROCESS_HEAP_ENTRY entry={0};SIZE_T bytes=0;
    CHECK(HeapLock(GetProcessHeap()));
    while(HeapWalk(GetProcessHeap(),&entry))
        if(entry.wFlags&PROCESS_HEAP_ENTRY_BUSY)bytes+=entry.cbData;
    CHECK(GetLastError()==ERROR_NO_MORE_ITEMS);
    CHECK(HeapUnlock(GetProcessHeap()));return bytes;
}
static void invalid_arguments_do_not_allocate(void)
{
    ntw32_executions *owner=NULL;ntw32_next_command command={0};
    SIZE_T before,after;unsigned i;
    CHECK(!ntw32_executions_open(&owner));if(!owner)return;
    before=busy_heap_bytes();
    for(i=0;i<32;++i) {
        CHECK(ntw32_execution_start(NULL,&command,0)==ERROR_INVALID_PARAMETER);
        CHECK(ntw32_execution_start(owner,NULL,0)==ERROR_INVALID_PARAMETER);
        CHECK(ntw32_execution_start(NULL,NULL,0)==ERROR_INVALID_PARAMETER);
    }
    after=busy_heap_bytes();CHECK(before==after);
    CHECK(ntw32_executions_idle(owner));ntw32_executions_close(owner);
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
static void oversized_headers_do_not_wait_for_payload(void)
{
    ntw32_executions *owner=NULL;DWORD lengths[]={NATIVE_LAUNCH_MAX_BYTES+1,MAXDWORD},i,bytes;
    CHECK(!ntw32_executions_open(&owner));if(!owner)return;
    for(i=0;i<ARRAYSIZE(lengths);++i) {
        HANDLE client=submit(owner);native_request_header header={NATIVE_REQUEST_VERSION,lengths[i]};
        native_request_reply reply={0};if(!client)continue;
        /* Send only the header: a receiver attempting payload allocation/read
         * would hang here. Rejection must precede reading the advertised body. */
        CHECK(WriteFile(client,&header,sizeof(header),&bytes,NULL) && bytes==sizeof(header));
        CHECK(ReadFile(client,&reply,sizeof(reply),&bytes,NULL) && bytes==sizeof(reply));
        CHECK(reply.error==ERROR_INVALID_DATA && !reply.target && !reply.receipt);
        CloseHandle(client);CHECK(!ntw32_executions_wait_idle(owner));
    }
    ntw32_executions_close(owner);
}
static void target_survives_close(void)
{
    ntw32_executions *owner=NULL;HANDLE client,target,receipt;
    WCHAR image[MAX_PATH],command[MAX_PATH+40],directory[MAX_PATH],*environment;
    run16_native_start start={0};BYTE *payload=NULL;DWORD size,bytes;
    native_request_header header={NATIVE_REQUEST_VERSION,0};native_request_reply reply={0};
    CHECK(!ntw32_executions_open(&owner));if(!owner)return;
    client=submit(owner);if(!client){ntw32_executions_close(owner);return;}
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));
    CHECK(GetCurrentDirectoryW(MAX_PATH,directory));
    swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" --held-target",image);
    environment=GetEnvironmentStringsW();CHECK(environment!=NULL);
    start.application=image;start.command=command;start.directory=directory;
    start.environment=environment;start.console_mask=7;
    CHECK(!run16_native_launch_pack(&start,&payload,&size));
    if(!payload){FreeEnvironmentStringsW(environment);CloseHandle(client);ntw32_executions_close(owner);return;}
    header.bytes=size;
    CHECK(WriteFile(client,&header,sizeof(header),&bytes,NULL) && bytes==sizeof(header));
    CHECK(WriteFile(client,payload,size,&bytes,NULL) && bytes==size);
    CHECK(ReadFile(client,&reply,sizeof(reply),&bytes,NULL) && bytes==sizeof(reply));
    CHECK(reply.version==NATIVE_REQUEST_VERSION && !reply.error && reply.target && reply.receipt);
    target=(HANDLE)(ULONG_PTR)reply.target;receipt=(HANDLE)(ULONG_PTR)reply.receipt;
    ntw32_executions_close(owner);
    CHECK(target && WaitForSingleObject(target,0)==WAIT_TIMEOUT);
    CHECK(receipt && WaitForSingleObject(receipt,0)==WAIT_TIMEOUT);
    /* Only the fixture disposes its own known target; product request cleanup
     * has already returned without ending it or fabricating completion. */
    if(target) {
        HANDLE cleanup=OpenProcess(PROCESS_TERMINATE|SYNCHRONIZE,FALSE,GetProcessId(target));
        CHECK(cleanup!=NULL);
        if(cleanup){CHECK(TerminateProcess(cleanup,0));WaitForSingleObject(cleanup,5000);CloseHandle(cleanup);}
        CloseHandle(target);
    }
    if(receipt)CloseHandle(receipt);
    CloseHandle(client);HeapFree(GetProcessHeap(),0,payload);FreeEnvironmentStringsW(environment);
}
int wmain(int argc,WCHAR **argv)
{
    ntw32_executions *owner=NULL;DWORD baseline,current,bytes,deadline;
    HANDLE pending[16]={0};unsigned i;
    if(argc==3 && !lstrcmpW(argv[1],L"--probe-target")){
        HANDLE marker=OpenEventW(EVENT_MODIFY_STATE,FALSE,argv[2]);
        if(!marker)return 4;
        SetEvent(marker);CloseHandle(marker);return 0;
    }
    if(argc==2 && !lstrcmpW(argv[1],L"--held-target")){Sleep(10000);return 73;}
    if(argc==3 && !lstrcmpW(argv[1],L"--invalid-arguments")) {
        if(_wfopen_s(&log,argv[2],L"wx"))return 2;
        invalid_arguments_do_not_allocate();
        fprintf(log,"INVALID-ARGUMENT-HEAP checks=%u failures=%u\n",checks,failures);
        fclose(log);return failures ? 1 : 0;
    }
    if(argc!=2 || _wfopen_s(&log,argv[1],L"wx"))return 2;
    invalid_arguments_do_not_allocate();
    launch_packet_boundaries();
    oversized_headers_do_not_wait_for_payload();
    /* Initialize the host's process-creation facilities before measuring
     * retained request handles; repeat the same target case below. */
    target_survives_close();
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&baseline));
    CHECK(ntw32_executions_open(&owner)==0);if(!owner)return 3;
    for(i=0;i<12;++i) {
        HANDLE client=submit(owner);native_request_header header={0,0};native_request_reply reply={0};
        if(!client)continue;
        CHECK(WriteFile(client,&header,sizeof(header),&bytes,NULL) && bytes==sizeof(header));
        CHECK(ReadFile(client,&reply,sizeof(reply),&bytes,NULL) && bytes==sizeof(reply));
        CHECK(reply.version==NATIVE_REQUEST_VERSION && reply.error==ERROR_INVALID_DATA && !reply.target && !reply.receipt);
        CHECK(!ReadFile(client,&reply,1,&bytes,NULL) && GetLastError()==ERROR_BROKEN_PIPE);
        CloseHandle(client);
        /* This fixture drains before checking resource counts; production
         * may enter its next GetNext while an earlier request is active. */
        CHECK(ntw32_executions_wait_idle(owner)==ERROR_SUCCESS);
        /* No next request is needed to release the completed request's process,
         * capability, I/O event and thread handles. The idle, stop and broker
         * completion-failure events are the only group-owned handles. */
        deadline=GetTickCount()+2000;
        do {
            CHECK(GetProcessHandleCount(GetCurrentProcess(),&current));
            if(current==baseline+3)break;
            Sleep(1);
        }while((LONG)(deadline-GetTickCount())>0);
        CHECK(current==baseline+3);
    }
    for(i=0;i<16;++i)pending[i]=submit(owner);
    /* Sixteen blocked header reads must cancel and clean up without a helper,
     * a new launcher request or a native process termination. */
    ntw32_executions_close(owner);
    for(i=0;i<16;++i)if(pending[i]) {
        char byte;
        CHECK(!ReadFile(pending[i],&byte,1,&bytes,NULL) && GetLastError()==ERROR_BROKEN_PIPE);
        CloseHandle(pending[i]);
    }
    target_survives_close();
    export_failure_does_not_launch();
    preflight_failure_replies_before_launch();
    broker_completion_failure_stops_reentry();
    resume_barrier(ERROR_SUCCESS,ERROR_SUCCESS);
    resume_barrier(ERROR_ACCESS_DENIED,ERROR_SUCCESS);
    resume_barrier(ERROR_SUCCESS,ERROR_PIPE_NOT_CONNECTED);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&current) && current==baseline);
    fprintf(log,"NTW32-REQUEST-LIFETIME checks=%u failures=%u completed=12 cancelled=16 target-survival=yes remaining-handles=%ld\n",
        checks,failures,(long)(current-baseline));
    fclose(log);return failures ? 1 : 0;
}
