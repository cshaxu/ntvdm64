/* Independent native worker. Launcher registration authorizes the broker
 * connection; no inherited frontend-private pipe or helper role is accepted. */
#define _WIN32_WINNT 0x0A00
#include "execution.h"
#include "console_state.h"
#include "presentation.h"
#include "worker-base/connection.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include "native_pc_font.h"

PVOID CsrPortHeap;

typedef struct native_membership {
    HANDLE quit,thread,capability,stop_requested,closed;
    HANDLE pipe,frontend,ready;
    ntcon_presentation *presentation;
    CRITICAL_SECTION *lock;
    console_text_style font;
    DWORD users;
    BOOL presenting;
} native_membership;
static DWORD begin_io(void *context,HANDLE stop)
{
    native_membership *state=context;
    DWORD retries=0;
    for(;;) {
        DWORD error=ERROR_NOT_READY;
        if(WaitForSingleObject(stop,0)!=WAIT_TIMEOUT)return ERROR_OPERATION_ABORTED;
        if(WaitForSingleObject(state->stop_requested,0)!=WAIT_TIMEOUT)return ERROR_OPERATION_ABORTED;
        EnterCriticalSection(state->lock);
        if(WaitForSingleObject(state->stop_requested,0)!=WAIT_TIMEOUT) {
            LeaveCriticalSection(state->lock);return ERROR_OPERATION_ABORTED;
        }
        if(state->presentation) {
            HANDLE output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
                FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
            error=state->presenting ? ERROR_SUCCESS :
                output==INVALID_HANDLE_VALUE ? GetLastError() : ntcon_presentation_begin(state->presentation,output);
            if(output!=INVALID_HANDLE_VALUE)CloseHandle(output);
            if(!error){++state->users;state->presenting=TRUE;}
        }
        /* Successful admission keeps the lock through CreateProcess, not
         * through the target lifetime or response I/O. */
        if(!error)return ERROR_SUCCESS;
        LeaveCriticalSection(state->lock);
        /* A screen can change while the frontend is copied into this
         * Console. The presentation rejects that torn seed with RETRY and
         * deactivates itself; retry the complete admission, never a partial
         * cell batch or an already accepted execution request. */
        if(error==ERROR_RETRY && ++retries<8) {
            if(WaitForSingleObject(stop,10)!=WAIT_TIMEOUT)return ERROR_OPERATION_ABORTED;
            continue;
        }
        if(error!=ERROR_NOT_READY && error!=ERROR_BUSY) {
            ntcon_trace_error("begin-io",0,error);return error;
        }
        if(WaitForSingleObject(state->thread,10)!=WAIT_TIMEOUT) {
            if(!GetExitCodeThread(state->thread,&error))return GetLastError();
            return error ? error : ERROR_OPERATION_ABORTED;
        }
    }
}
static void release_launch(void *context)
{
    native_membership *state=context;
    LeaveCriticalSection(state->lock);
}
static DWORD end_io(void *context)
{
    native_membership *state=context;DWORD error,count=0,*members=NULL,attempt;
    EnterCriticalSection(state->lock);
    error=ntcon_console_members(&members,&count);
    if(members)HeapFree(GetProcessHeap(),0,members);
    /* Completion belongs to the direct target. Its surviving Console users
     * retain presentation independently; never reseed over their live output. */
    if(!error && state->presenting) {
        if(state->users>1 || count) {
            for(attempt=0;attempt<8;++attempt) {
                error=ntcon_presentation_capture(state->presentation,&state->font);
                if(error!=ERROR_RETRY)break;
                if(attempt<7)Sleep(10);
            }
        }
        else {
            error=ntcon_presentation_end(state->presentation,&state->font);
            state->presenting=FALSE;
        }
    }
    if(state->users)--state->users;
    ntcon_trace_error("end-io",0,error);
    LeaveCriticalSection(state->lock);return error;
}
static DWORD take_presentation(native_membership *state)
{
    DWORD generation=0,error;
    console_io_request request={0};console_io_reply reply;
    if(state->presentation)return ERROR_SUCCESS;
    error=OpenNtBaseClientTakeFrontend(&state->pipe,&state->frontend,&generation,&state->ready);
    if(error==ERROR_NOT_READY)return ERROR_SUCCESS;
    if(error)return error;
    error=ntcon_presentation_open(state->pipe,state->frontend,state->quit,generation,&state->presentation);
    if(error)return error;
    /* Establish transport, not input ownership. An inactive frontend is a
     * valid attached endpoint; only execution handoff may activate it. */
    request.operation=CONSOLE_IO_BARRIER;
    error=ntcon_presentation_call(state->presentation,&request,&reply);
    return error==ERROR_NOT_READY || error==ERROR_BUSY ? ERROR_SUCCESS : error;
}
static DWORD sample_members_loop(void *context)
{
    native_membership *state=context;DWORD error=0;
    while(WaitForSingleObject(state->quit,30)==WAIT_TIMEOUT) {
        uint64_t epoch=0;DWORD *members=NULL,count=0;WCHAR image[260]=L"";
        /* Keep the client sample and local request count in one handoff
         * interval. A pre-launch empty sample must not release/reseed the
         * screen after that request has already spawned a surviving client. */
        EnterCriticalSection(state->lock);
        if(WaitForSingleObject(state->stop_requested,0)==WAIT_OBJECT_0) {
            error=ntcon_console_close();
            if(!error && !SetEvent(state->closed))error=GetLastError();
            /* Explicit registered-session shutdown, never launcher death.
             * Close acknowledgment precedes worker termination. Do not wait
             * on the main thread's service request while the broker waits
             * for this worker to terminate. No descendant enumeration. */
            TerminateProcess(GetCurrentProcess(),error ? error : ERROR_CANCELLED);
            LeaveCriticalSection(state->lock);
            return error ? error : ERROR_CANCELLED;
        }
        error=OpenNtBaseClientNativeSampleEpoch(&epoch);
        if(!error)error=ntcon_console_members(&members,&count);
        if(!error && count) {
            DWORD image_error=ntcon_console_active_image(image);
            if(image_error && image_error!=ERROR_NOT_FOUND)error=image_error;
        }
        if(members)HeapFree(GetProcessHeap(),0,members);
        if(error) {
            LeaveCriticalSection(state->lock);
            if(error==ERROR_RETRY || error==ERROR_BUSY)continue;
            return error;
        }
        error=take_presentation(state);
        if(!error && state->presentation && (state->users || count)) {
            DWORD accepted=0;
            if(!state->presenting) {
                HANDLE output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
                    FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
                error=output==INVALID_HANDLE_VALUE ? GetLastError() : ntcon_presentation_begin(state->presentation,output);
                if(output!=INVALID_HANDLE_VALUE)CloseHandle(output);
                if(!error)state->presenting=TRUE;
            }
            /* A completed direct target may still be waiting for its final
             * presentation receipt. Do not feed the next DOS command to a
             * Console which has no native consumer during that interval. */
            if(!error && count)error=ntcon_presentation_input(state->presentation,GetStdHandle(STD_INPUT_HANDLE),&accepted);
            if(error==ERROR_BUSY) {
                /* DOS requested the shared screen. Publish before releasing,
                 * then import its final screen before resuming native I/O. */
                error=ntcon_presentation_end(state->presentation,&state->font);
                state->presenting=FALSE;
                if(!error)error=ERROR_NOT_READY;
            }
            if(!error)error=ntcon_presentation_capture(state->presentation,&state->font);
            if(error==ERROR_NOT_READY || error==ERROR_BUSY){state->presenting=FALSE;error=0;}
            if(error==ERROR_RETRY)error=0;
        } else if(!error && state->presentation && state->presenting) {
            error=ntcon_presentation_end(state->presentation,&state->font);
            state->presenting=FALSE;
        }
        LeaveCriticalSection(state->lock);
        if(error)return error;
        /* Publish empty membership only after the last screen/input handoff;
         * otherwise the frontend can retire before receiving that handoff. */
        error=OpenNtBaseClientReportNativeBackend(epoch,count,image);
        if(error && error!=ERROR_RETRY && error!=ERROR_BUSY)return error;
    }
    return 0;
}
static DWORD WINAPI sample_members(void *context)
{
    native_membership *state=context;
    DWORD error=sample_members_loop(context),count=0,*members=NULL,sample;
    ntcon_trace_error("pump",0,error);
    if(!error || WaitForSingleObject(state->quit,0)!=WAIT_TIMEOUT)return error;
    /* A dead frontend may leave an empty resident worker, but never a live
     * Console whose sole I/O pump has silently exited. Serialize with launch
     * admission, then use the same Console-owner close as explicit shutdown.
     * This is not launcher death, descendant enumeration or process-tree kill. */
    EnterCriticalSection(state->lock);
    sample=ntcon_console_members(&members,&count);
    if(members)HeapFree(GetProcessHeap(),0,members);
    if(state->users || count || sample) {
        SetEvent(state->stop_requested);
        (void)ntcon_console_close();
        /* Main can be blocked in WaitWorkerChannel; do not wait on that RPC
         * to report an unrecoverable worker-side presentation failure. */
        TerminateProcess(GetCurrentProcess(),error);
    }
    LeaveCriticalSection(state->lock);
    return error;
}
static void membership_close(native_membership *state)
{
    CRITICAL_SECTION *lock=state->lock;
    if(state->quit)SetEvent(state->quit);
    if(state->thread){WaitForSingleObject(state->thread,INFINITE);CloseHandle(state->thread);}
    ntcon_presentation_close(state->presentation);
    if(state->pipe)CloseHandle(state->pipe);
    if(state->frontend)CloseHandle(state->frontend);
    if(state->ready)CloseHandle(state->ready);
    if(state->quit)CloseHandle(state->quit);
    if(state->capability)CloseHandle(state->capability);
    if(state->stop_requested)CloseHandle(state->stop_requested);
    if(state->closed)CloseHandle(state->closed);
    ZeroMemory(state,sizeof(*state));
    state->lock=lock;
}
static DWORD membership_bind(native_membership *state,HANDLE frontend)
{
    typedef BOOL (WINAPI *compare_handles)(HANDLE,HANDLE);
    compare_handles compare=(compare_handles)GetProcAddress(GetModuleHandleW(L"kernelbase.dll"),"CompareObjectHandles");
    DWORD error;
    if(!compare)return ERROR_CALL_NOT_IMPLEMENTED;
    if(state->capability && compare(state->capability,frontend)) {
        if(WaitForSingleObject(state->thread,0)==WAIT_TIMEOUT)return 0;
        return GetExitCodeThread(state->thread,&error) ? (error ? error : ERROR_OPERATION_ABORTED) : GetLastError();
    }
    membership_close(state);
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
    if(!state->quit || !state->stop_requested || !state->closed)error=GetLastError();
    else if(!DuplicateHandle(GetCurrentProcess(),frontend,GetCurrentProcess(),&state->capability,SYNCHRONIZE,FALSE,0))error=GetLastError();
    else error=OpenNtBaseClientRegisterNativeBackend(frontend,state->stop_requested,state->closed);
    if(!error) {
        state->thread=CreateThread(NULL,0,sample_members,state,0,NULL);
        if(!state->thread)error=GetLastError();
    }
    if(error)membership_close(state);
    /* Only explicit management close may acknowledge actual Console closure. */
    return error;
}

static BOOL WINAPI control_event(DWORD event)
{ return event==CTRL_C_EVENT || event==CTRL_BREAK_EVENT; }

int wmain(int argc,WCHAR **argv)
{
    DWORD error;
    ntcon_executions *requests=NULL;
    native_membership membership={0};
    CRITICAL_SECTION io_lock;
    ntcon_execution_io io={&membership,begin_io,end_io,release_launch};
    (void)argv;
    if(argc!=1)return ERROR_INVALID_PARAMETER;
    InitializeCriticalSection(&io_lock);membership.lock=&io_lock;
    CsrPortHeap=HeapCreate(0,0,0);
    if(!CsrPortHeap)return GetLastError();
    error=worker_base_connect();
    if(!error)error=ntcon_console_initialize();
    if(!error)error=ntcon_executions_open(&requests);
    if(!error)ntcon_executions_bind_io(requests,&io);
    if(!error && !SetConsoleCtrlHandler(control_event,TRUE))error=GetLastError();
    while(!error) {
        HANDLE channel=NULL,sender=NULL,execution=NULL,frontend=NULL;DWORD request=0;
        error=OpenNtBaseClientWaitWorkerChannel(&channel,&sender,&execution,&frontend,&request);
        if(error)break;
        /* An unaccepted request closes its attachments; it neither ends the
         * worker nor cancels other requests or already running targets. */
        {
            typedef BOOL (WINAPI *compare_handles)(HANDLE,HANDLE);
            compare_handles compare=(compare_handles)GetProcAddress(GetModuleHandleW(L"kernelbase.dll"),"CompareObjectHandles");
            DWORD binding=membership.capability && (!compare || !compare(membership.capability,frontend)) &&
                !ntcon_executions_idle(requests) ? ERROR_BUSY : membership_bind(&membership,frontend);
            if(binding) {
                CloseHandle(frontend);CloseHandle(channel);CloseHandle(sender);CloseHandle(execution);
                (void)OpenNtBaseClientCompleteWorkerChannel(request);
                continue;
            }
        }
        if(ntcon_execution_start(requests,frontend,channel,sender,execution,request))
            (void)OpenNtBaseClientCompleteWorkerChannel(request);
    }
    if(membership.quit)SetEvent(membership.quit);
    ntcon_executions_close(requests);
    membership_close(&membership);
    worker_base_disconnect();
    HeapDestroy(CsrPortHeap);CsrPortHeap=NULL;
    DeleteCriticalSection(&io_lock);
    return (int)error;
}
