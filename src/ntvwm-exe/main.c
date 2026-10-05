/* Independent native worker. Launcher registration authorizes the broker
 * connection; no inherited frontend-private pipe or helper role is accepted. */
#define _WIN32_WINNT 0x0A00
#include "execution.h"
#include "console_state.h"
#include "presentation.h"
#include "worker-base/connection.h"
#include "worker-base/input_watch.h"
#include "common/protocol/frontend_protocol.h"
#include "next_command.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include "native_pc_font.h"
#include <stdio.h>

PVOID CsrPortHeap;


typedef struct native_membership {
    HANDLE quit,thread,capability,stop_requested,closed,admission_ready,shutdown,io_release;
    HANDLE pipe,frontend,ready;
    HANDLE route_changed;
    worker_base_input_watch *input_watch;
    ntvwm_presentation *presentation;
    CRITICAL_SECTION *lock;
    console_text_style font;
    DWORD users,admissions;
    BOOL presenting;
    /* A live native parent may be blocked in an inner launcher. Only an
     * authenticated command/resume admission can request ownership again. */
    BOOL io_released;
} native_membership;
/* Called with the execution/I/O lock held, after final paint/input return.
 * Dispose only local transport state; the broker retains logical association. */
static DWORD membership_release_io(native_membership *state)
{
    DWORD error=worker_base_input_watch_bind(state->input_watch,NULL);
    if(error)return error;
    ntvwm_presentation_close(state->presentation);state->presentation=NULL;
    error=worker_base_io_close(&state->pipe,&state->frontend,&state->ready);
    state->presenting=FALSE;state->io_released=TRUE;
    ResetEvent(state->admission_ready);
    return error;
}
static DWORD begin_io(void *context,HANDLE stop)
{
    native_membership *state=context;
    DWORD error=ERROR_SUCCESS,wait;
    ULONGLONG deadline=GetTickCount64()+10000;
    EnterCriticalSection(state->lock);
    ++state->admissions;
    if(!SetEvent(state->route_changed)) {
        error=GetLastError();--state->admissions;
        LeaveCriticalSection(state->lock);return error;
    }
    for(;;) {
        /* A Console target must not run before its frontend can receive output
         * and provide input. Broker delivery alone is not an I/O handoff. */
        HANDLE waits[4];
        ULONGLONG now;
        if(WaitForSingleObject(stop,0)!=WAIT_TIMEOUT ||
            WaitForSingleObject(state->stop_requested,0)!=WAIT_TIMEOUT) {
            error=ERROR_OPERATION_ABORTED;break;
        }
        /* Successful admission keeps the lock through CreateProcess, not
         * through the target lifetime or response I/O. */
        if(state->presentation && state->presenting) {
            ++state->users;--state->admissions;
            error=worker_base_input_watch_bind(state->input_watch,state->ready);
            if(!error && !SetEvent(state->route_changed))error=GetLastError();
            if(error){--state->users;LeaveCriticalSection(state->lock);return error;}
            return ERROR_SUCCESS;
        }
        now=GetTickCount64();
        if(now>=deadline){error=ERROR_TIMEOUT;break;}
        waits[0]=stop;waits[1]=state->stop_requested;
        waits[2]=state->thread;waits[3]=state->admission_ready;
        LeaveCriticalSection(state->lock);
        wait=WaitForMultipleObjects(4,waits,FALSE,(DWORD)(deadline-now));
        EnterCriticalSection(state->lock);
        if(wait==WAIT_OBJECT_0 || wait==WAIT_OBJECT_0+1)
            {error=ERROR_OPERATION_ABORTED;break;}
        if(wait==WAIT_OBJECT_0+2) {
            if(!GetExitCodeThread(state->thread,&error))error=GetLastError();
            if(!error)error=ERROR_OPERATION_ABORTED;
            break;
        }
        if(wait==WAIT_TIMEOUT){error=ERROR_TIMEOUT;break;}
        if(wait==WAIT_FAILED){error=GetLastError();break;}
    }
    --state->admissions;
    LeaveCriticalSection(state->lock);
    ntvwm_trace_error("begin-io",0,error);
    return error;
}
static void release_launch(void *context)
{
    native_membership *state=context;
    LeaveCriticalSection(state->lock);
}
static DWORD end_io(void *context,DWORD request)
{
    native_membership *state=context;DWORD error,decision=WORKER_IO_KEEP;
    EnterCriticalSection(state->lock);
    /* Report the exact direct completion. Neither local admissions nor
     * physical Console membership decides whether this channel closes. */
    error=worker_base_io_checkpoint(WORKER_IO_CHECKPOINT_COMPLETE,request,&decision);
    if(!error && decision==WORKER_IO_RELEASE && state->presenting) {
        error=worker_base_input_watch_bind(state->input_watch,NULL);
        if(!error)error=ntvwm_presentation_end(state->presentation,&state->font);
        if(!error)error=membership_release_io(state);
    } else if(!error && state->presenting)
        error=ntvwm_presentation_flush(state->presentation,&state->font);
    if(state->users)--state->users;
    ntvwm_trace_error("end-io",0,error);
    LeaveCriticalSection(state->lock);return error;
}
static DWORD resume_io(void *context)
{
    native_membership *state=context;
    /* begin_io's temporary admission is not a new target. The authenticated
     * resume keeps the parent presenting instead of releasing it again. */
    EnterCriticalSection(state->lock);
    if(state->users)--state->users;
    LeaveCriticalSection(state->lock);
    return ERROR_SUCCESS;
}
static DWORD take_presentation(native_membership *state)
{
    DWORD generation=0,error;
    console_io_request request={0};console_io_reply reply;
    if(state->presentation)return ERROR_SUCCESS;
    error=worker_base_io_open(&state->pipe,&state->frontend,&state->ready,&generation);
    if(error)return error;
    error=ntvwm_presentation_open(state->pipe,state->frontend,state->quit,generation,&state->presentation);
    if(error)return error;
    /* Establish transport, not input ownership. An inactive frontend is a
     * valid attached endpoint; only execution handoff may activate it. */
    request.operation=CONSOLE_IO_BARRIER;
    error=ntvwm_presentation_call(state->presentation,&request,&reply);
    return error==ERROR_NOT_READY || error==ERROR_BUSY ? ERROR_SUCCESS : error;
}
static DWORD WINAPI close_console(void *context)
{
    (void)context;
    return ntvwm_console_close();
}
/* A GetNext cancellation or failed final-I/O RPC may race the close watcher.
 * An already issued broker close must win over generic worker-fault teardown.
 * Reuse the I/O lock: exactly one caller closes the actual Console and exits;
 * this does not invent a close instruction on unexpected broker/worker loss. */
static void honor_console_close(native_membership *state)
{
    if((state->shutdown && WaitForSingleObject(state->shutdown,0)==WAIT_OBJECT_0) ||
        (state->stop_requested && WaitForSingleObject(state->stop_requested,0)==WAIT_OBJECT_0)) {
        EnterCriticalSection(state->lock);
        worker_base_shutdown_close(close_console,state,INFINITE,ERROR_CANCELLED,state->closed);
        LeaveCriticalSection(state->lock);
    }
}
static void input_failure(void *context,DWORD error)
{
    native_membership *state=context;
    honor_console_close(state);
    if(WaitForSingleObject(state->quit,0)!=WAIT_TIMEOUT)return;
    ntvwm_trace_error("input",0,error);
    EnterCriticalSection(state->lock);
    SetEvent(state->stop_requested);
    (void)ntvwm_console_close();
    TerminateProcess(GetCurrentProcess(),error);
    LeaveCriticalSection(state->lock);
}
static DWORD consume_input(void *context)
{
    native_membership *state=context;DWORD error=ERROR_SUCCESS,accepted=0;
    EnterCriticalSection(state->lock);
    /* The lock serializes actual reads with final input return. A wake from
     * a retired source is never permission to consume a newly routed queue. */
    if(state->users && state->presenting && !state->io_released &&
        WaitForSingleObject(state->io_release,0)==WAIT_TIMEOUT &&
        WaitForSingleObject(state->quit,0)==WAIT_TIMEOUT) {
        error=ntvwm_presentation_input(state->presentation,GetStdHandle(STD_INPUT_HANDLE),&accepted);
        if(!error)error=worker_base_input_watch_ack(state->input_watch);
    } else error=worker_base_input_watch_bind(state->input_watch,NULL);
    LeaveCriticalSection(state->lock);return error;
}
static DWORD presentation_loop(void *context)
{
    native_membership *state=context;DWORD error=0;
    HANDLE waits[5]={state->shutdown,state->stop_requested,state->quit,state->io_release,state->route_changed};
    DWORD wait,timeout;
    for(;;) {
        EnterCriticalSection(state->lock);
        timeout=state->presenting && state->users && !state->io_released ? 20 : INFINITE;
        LeaveCriticalSection(state->lock);
        wait=WaitForMultipleObjects(5,waits,FALSE,timeout);
        if(wait==WAIT_OBJECT_0+2)break;
        EnterCriticalSection(state->lock);
        if(wait==WAIT_OBJECT_0 || wait==WAIT_OBJECT_0+1) {
            /* The authenticated broker orders Console-session closure,
             * just as it does for NTVDM. Closing a direct launcher does not.
             * Explicit management close uses this same backend operation. */
            worker_base_shutdown_close(close_console,state,INFINITE,ERROR_CANCELLED,state->closed);
            LeaveCriticalSection(state->lock);
            return error ? error : ERROR_CANCELLED;
        }
        if(wait==WAIT_OBJECT_0+3) {
            error=worker_base_input_watch_bind(state->input_watch,NULL);
            if(!error && state->presenting)error=ntvwm_presentation_end(state->presentation,&state->font);
            if(!error && state->presentation)error=membership_release_io(state);
            LeaveCriticalSection(state->lock);
            if(error)return error;
            continue;
        }
        if(wait!=WAIT_TIMEOUT && wait!=WAIT_OBJECT_0+4) {
            error=wait==WAIT_FAILED ? GetLastError() : ERROR_INVALID_STATE;
            LeaveCriticalSection(state->lock);return error;
        }
        if(wait==WAIT_OBJECT_0+4 && state->admissions) {
            error=take_presentation(state);
            if(!error && !state->presenting) {
                HANDLE output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
                    FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
                error=output==INVALID_HANDLE_VALUE ? GetLastError() : ntvwm_presentation_begin(state->presentation,output);
                if(output!=INVALID_HANDLE_VALUE)CloseHandle(output);
                if(!error) {
                    state->presenting=TRUE;
                    state->io_released=FALSE;
                    if(!SetEvent(state->admission_ready))error=GetLastError();
                }
            }
        }
        /* Only hidden Console output lacks a change notification. Input and
         * launch admission above never depend on this acquisition timeout. */
        if(wait==WAIT_TIMEOUT && state->presenting && state->users && !state->io_released)
            error=ntvwm_presentation_capture(state->presentation,&state->font);
        if(error==ERROR_RETRY)error=0;
        /* Unexpected transport failure is not permission to rediscover or
         * reconnect a frontend. Expected broker release was handled above. */
        LeaveCriticalSection(state->lock);
        if(error)return error;
    }
    return 0;
}
static DWORD WINAPI presentation_pump(void *context)
{
    native_membership *state=context;
    DWORD error=presentation_loop(context);
    ntvwm_trace_error("pump",0,error);
    honor_console_close(state);
    if(!error || WaitForSingleObject(state->quit,0)!=WAIT_TIMEOUT)return error;
    /* Only an unrecoverable worker-side failure reaches here. A worker whose
     * presentation watcher has stopped cannot remain resident: it would no
     * longer consume the broker's Console-session close, even while idle. */
    EnterCriticalSection(state->lock);
    SetEvent(state->stop_requested);
    (void)ntvwm_console_close();
    /* Main can be blocked in GetNextCommand; do not wait on that RPC
     * to report an unrecoverable worker-side presentation failure. */
    TerminateProcess(GetCurrentProcess(),error);
    LeaveCriticalSection(state->lock);
    return error;
}
static void membership_close(native_membership *state)
{
    CRITICAL_SECTION *lock=state->lock;
    if(state->quit)SetEvent(state->quit);
    if(state->thread){WaitForSingleObject(state->thread,INFINITE);CloseHandle(state->thread);}
    worker_base_input_watch_destroy(state->input_watch);
    ntvwm_presentation_close(state->presentation);
    if(state->pipe)CloseHandle(state->pipe);
    if(state->frontend)CloseHandle(state->frontend);
    if(state->ready)CloseHandle(state->ready);
    if(state->quit)CloseHandle(state->quit);
    if(state->capability)CloseHandle(state->capability);
    if(state->stop_requested)CloseHandle(state->stop_requested);
    if(state->closed)CloseHandle(state->closed);
    if(state->admission_ready)CloseHandle(state->admission_ready);
    if(state->shutdown)CloseHandle(state->shutdown);
    if(state->io_release)CloseHandle(state->io_release);
    if(state->route_changed)CloseHandle(state->route_changed);
    ZeroMemory(state,sizeof(*state));
    state->lock=lock;
}
static DWORD membership_initialize(native_membership *state)
{
    DWORD error;
    {
        unsigned bank,glyph;
        /* Match original startup 80x25 VGA; later handoffs supply the actual
         * DOS font banks. This bitmap font is not the hidden Console font. */
        state->font.font_height=16;
        for(bank=0;bank<2;++bank)for(glyph=0;glyph<256;++glyph)
            memcpy(state->font.fonts[bank][glyph],frontend_native_vga_font[glyph],16);
    }
    state->quit=CreateEventW(NULL,TRUE,FALSE,NULL);
    state->stop_requested=CreateEventW(NULL,TRUE,FALSE,NULL);
    state->closed=CreateEventW(NULL,TRUE,FALSE,NULL);
    state->admission_ready=CreateEventW(NULL,TRUE,FALSE,NULL);
    state->route_changed=CreateEventW(NULL,FALSE,FALSE,NULL);
    if(!state->quit || !state->stop_requested || !state->closed || !state->admission_ready || !state->route_changed)error=GetLastError();
    else error=ERROR_SUCCESS;
    if(!error)error=OpenNtBaseClientRegisterNativeBackend(NULL,state->stop_requested,state->closed);
    if(!error)error=worker_base_shutdown_event(&state->shutdown);
    if(!error)error=worker_base_io_release_event(&state->io_release);
    if(!error) {
        state->input_watch=worker_base_input_watch_create(state->shutdown,state->quit,
            consume_input,NULL,input_failure,state);
        if(!state->input_watch)error=GetLastError();
    }
    if(!error) {
        state->thread=CreateThread(NULL,0,presentation_pump,state,0,NULL);
        if(!state->thread)error=GetLastError();
    }
    if(error)membership_close(state);
    /* Only explicit management close may acknowledge actual Console closure. */
    return error;
}

static DWORD membership_bind(native_membership *state,HANDLE frontend)
{
    typedef BOOL (WINAPI *compare_handles)(HANDLE,HANDLE);
    compare_handles compare=(compare_handles)GetProcAddress(GetModuleHandleW(L"kernelbase.dll"),"CompareObjectHandles");
    DWORD error;
    if(!compare)return ERROR_CALL_NOT_IMPLEMENTED;
    if(state->capability) {
        if(!compare(state->capability,frontend))return ERROR_PIPE_NOT_CONNECTED;
        if(WaitForSingleObject(state->thread,0)==WAIT_TIMEOUT)return ERROR_SUCCESS;
        return GetExitCodeThread(state->thread,&error) ? (error ? error : ERROR_OPERATION_ABORTED) : GetLastError();
    }
    if(!DuplicateHandle(GetCurrentProcess(),frontend,GetCurrentProcess(),
        &state->capability,SYNCHRONIZE,FALSE,0))return GetLastError();
    error=OpenNtBaseClientRegisterNativeBackend(frontend,state->stop_requested,state->closed);
    if(error){CloseHandle(state->capability);state->capability=NULL;}
    return error;
}

static BOOL WINAPI control_event(DWORD event)
{ return event==CTRL_C_EVENT || event==CTRL_BREAK_EVENT; }

static void broker_completion_fault(void *context,DWORD error)
{
    honor_console_close(context);
    ntvwm_trace_error("broker-complete",0,error);
    /* GetNext may be blocked in a synchronous RPC while a serving thread
     * discovers the failure. Process exit closes the worker's handles; it
     * does not terminate any native target or descendant. NTSRV owns the
     * resulting dead-worker rundown. */
    TerminateProcess(GetCurrentProcess(),error);
}

int wmain(int argc,WCHAR **argv)
{
    DWORD error;
    ntvwm_executions *requests=NULL;
    native_membership membership={0};
    CRITICAL_SECTION io_lock;
    ntvwm_execution_io io={&membership,begin_io,end_io,release_launch,resume_io};
    (void)argv;
    if(argc!=1)return ERROR_INVALID_PARAMETER;
    InitializeCriticalSection(&io_lock);membership.lock=&io_lock;
    CsrPortHeap=HeapCreate(0,0,0);
    if(!CsrPortHeap)return GetLastError();
    error=worker_base_connect();
    if(!error) error=ntvwm_console_initialize();
    if(!error) error=membership_initialize(&membership);
    if(!error) error=ntvwm_executions_open(&requests);
    if(!error) {
        ntvwm_executions_bind_io(requests,&io);
        ntvwm_executions_bind_fault(requests,broker_completion_fault,&membership);
    }
    if(!error && !SetConsoleCtrlHandler(control_event,TRUE))error=GetLastError();
    while(!error) {
        ntvwm_next_command command;
        /* An active CMD may wait on an inner run16. GetNext must remain
         * available to that same frontend while earlier requests execute;
         * different frontends are rejected by the binding rule below. */
        error=ntvwm_get_next_command(&command);
        if(error)break;
        /* An unaccepted request closes its attachments; it neither ends the
         * worker nor cancels other requests or already running targets. */
        {
            typedef BOOL (WINAPI *compare_handles)(HANDLE,HANDLE);
            compare_handles compare=(compare_handles)GetProcAddress(GetModuleHandleW(L"kernelbase.dll"),"CompareObjectHandles");
            /* A GUI command has no frontend attachment. Do not replace an
             * existing text membership when the same carrier starts a GUI. */
            DWORD binding=command.frontend ?
                (membership.capability && (!compare || !compare(membership.capability,command.frontend)) &&
                !ntvwm_executions_idle(requests) ? ERROR_BUSY : membership_bind(&membership,command.frontend)) :
                (command.bytes ? ERROR_SUCCESS : ERROR_INVALID_DATA);
            /* Even allocation/thread failure must report startup before
             * completing the record. No serving thread owns this command. */
            DWORD start_error=ntvwm_execution_start(requests,&command,binding);
            if(start_error) {
                DWORD completion=OpenNtBaseClientNativeStartupResult(command.caller_generation,
                    command.request,start_error,NULL,NULL);
                if(!completion)completion=ntvwm_complete_next_command(command.request,0);
                if(completion)ntvwm_executions_note_broker_failure(requests,completion);
                ntvwm_dispose_next_command(&command);
            }
            if(binding==ERROR_PIPE_NOT_CONNECTED || binding==ERROR_ACCESS_DENIED) {
                /* The first root may disappear before membership is bound.
                 * Let the request's structured failure reach its launcher,
                 * then leave instead of becoming an unowned EMPTY worker. */
                DWORD drained=ntvwm_executions_wait_idle(requests);
                error=drained ? drained : binding;
                break;
            }
            continue;
        }
    }
    honor_console_close(&membership);
    if(membership.quit)SetEvent(membership.quit);
    ntvwm_executions_close(requests);
    membership_close(&membership);
    worker_base_disconnect();
    HeapDestroy(CsrPortHeap);CsrPortHeap=NULL;
    DeleteCriticalSection(&io_lock);
    return (int)error;
}
