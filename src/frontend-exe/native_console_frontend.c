#include "native_console_frontend.h"
#include "native_console_view.h"

/* One lazy backend per root frontend, not per inner launcher or native child.
 * No target registry or execution scheduler: Windows owns the native Console
 * group and each requester owns its actual process-completion handle. */
struct run16_native_frontend {
    CRITICAL_SECTION lock,io_lock;
    run16_native_backend *backend;
    run16_native_console_view view;
    HANDLE stop,refresh,refreshed,thread,changed,control[2];
    const void *dos_owner;
    BOOL controls_live;
};
/* Win32's process-wide callback has no context argument. This single binding
 * belongs only to the root frontend; the lock joins callbacks before teardown.
 * Never wait for IPC or manipulate execution from the OS callback thread. */
static SRWLOCK control_lock=SRWLOCK_INIT;
static run16_native_frontend *control_owner;
static BOOL WINAPI frontend_control(DWORD event)
{
    BOOL handled=FALSE;
    if(event!=CTRL_C_EVENT && event!=CTRL_BREAK_EVENT)return FALSE;
    AcquireSRWLockShared(&control_lock);
    if(control_owner && control_owner->controls_live)
        handled=SetEvent(control_owner->control[event]);
    ReleaseSRWLockShared(&control_lock);
    return handled;
}
static DWORD WINAPI present(void *context)
{
    run16_native_frontend *frontend=context;
    HANDLE waits[7]={frontend->stop,frontend->changed,frontend->refresh,
        run16_native_backend_process(frontend->backend),frontend->control[0],
        frontend->control[1],frontend->view.input};
    for(;;) {
        DWORD error=ERROR_SUCCESS,wait;
        BOOL paused,requested=WaitForSingleObject(frontend->refresh,0)==WAIT_OBJECT_0;
        if(WaitForSingleObject(frontend->stop,0)==WAIT_OBJECT_0)return ERROR_OPERATION_ABORTED;
        if(requested)ResetEvent(frontend->refresh);
        EnterCriticalSection(&frontend->io_lock);
        if(WaitForSingleObject(frontend->stop,0)==WAIT_OBJECT_0){
            LeaveCriticalSection(&frontend->io_lock);return ERROR_OPERATION_ABORTED;
        }
        paused=frontend->dos_owner!=NULL;
        if(!paused) {
            error=run16_native_view_present(frontend->backend,&frontend->view);
            if(!error && WaitForSingleObject(frontend->view.input,0)==WAIT_OBJECT_0)
                error=run16_native_view_forward_input(frontend->backend,&frontend->view);
        }
        LeaveCriticalSection(&frontend->io_lock);
        if(error && error!=ERROR_RETRY)return error;
        if(requested) {
            if(error==ERROR_RETRY)SetEvent(frontend->refresh);
            else SetEvent(frontend->refreshed); /* No native redraw over active DOS. */
        }
        wait=WaitForMultipleObjects(paused ? 6 : 7,waits,FALSE,paused ? INFINITE : 30);
        if(wait==WAIT_OBJECT_0)return ERROR_OPERATION_ABORTED;
        if(wait==WAIT_OBJECT_0+3)return ERROR_BROKEN_PIPE;
        if(wait==WAIT_FAILED)return GetLastError();
        if(wait==WAIT_OBJECT_0+4 || wait==WAIT_OBJECT_0+5) {
            run16_native_host_request request={RUN16_NATIVE_HOST_VERSION,RUN16_NATIVE_CONTROL,0,0,wait-(WAIT_OBJECT_0+4)};
            run16_native_host_reply reply;
            /* Both Consoles implement one logical user Console. DOS workers
             * physically attached here already receive the original event;
             * forward once to the other Console, even while DOS owns I/O. */
            error=run16_native_backend_call(frontend->backend,&request,NULL,&reply,NULL,0);
            if(error || reply.status)return error ? error : reply.status;
        }
    }
}
DWORD run16_native_frontend_create(run16_native_frontend **output)
{
    run16_native_frontend *frontend;
    DWORD error;
    if(!output)return ERROR_INVALID_PARAMETER;
    *output=NULL;
    frontend=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*frontend));
    if(!frontend)return ERROR_NOT_ENOUGH_MEMORY;
    InitializeCriticalSection(&frontend->lock);
    InitializeCriticalSection(&frontend->io_lock);
    frontend->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    frontend->refresh=CreateEventW(NULL,TRUE,FALSE,NULL);
    frontend->refreshed=CreateEventW(NULL,TRUE,FALSE,NULL);
    frontend->changed=CreateEventW(NULL,FALSE,FALSE,NULL);
    frontend->control[0]=CreateEventW(NULL,FALSE,FALSE,NULL);
    frontend->control[1]=CreateEventW(NULL,FALSE,FALSE,NULL);
    if(!frontend->stop || !frontend->refresh || !frontend->refreshed || !frontend->changed ||
        !frontend->control[0] || !frontend->control[1]) {
        error=GetLastError();run16_native_frontend_destroy(frontend);return error;
    }
    AcquireSRWLockExclusive(&control_lock);
    error=control_owner ? ERROR_ALREADY_EXISTS : ERROR_SUCCESS;
    if(!error) {
        if(!SetConsoleCtrlHandler(frontend_control,TRUE))error=GetLastError();
        else control_owner=frontend;
    }
    ReleaseSRWLockExclusive(&control_lock);
    if(error) { run16_native_frontend_destroy(frontend);return error; }
    *output=frontend;return ERROR_SUCCESS;
}
DWORD run16_native_frontend_launch(run16_native_frontend *frontend,
    const run16_native_start *start,HANDLE *target)
{
    WCHAR image[MAX_PATH];
    DWORD error=ERROR_SUCCESS,length;
    if(!frontend || !start || !target)return ERROR_INVALID_PARAMETER;
    *target=NULL;
    EnterCriticalSection(&frontend->lock);
    if(WaitForSingleObject(frontend->stop,0)==WAIT_OBJECT_0) { error=ERROR_OPERATION_ABORTED;goto done; }
    if(frontend->thread && WaitForSingleObject(frontend->thread,0)==WAIT_OBJECT_0) {
        if(!GetExitCodeThread(frontend->thread,&error))error=GetLastError();
        if(!error)error=ERROR_BROKEN_PIPE;
        goto done;
    }
    if(!frontend->backend) {
        EnterCriticalSection(&frontend->io_lock);
        if(frontend->dos_owner) {
            LeaveCriticalSection(&frontend->io_lock);error=ERROR_BUSY;goto done;
        }
        length=GetModuleFileNameW(NULL,image,ARRAYSIZE(image));
        if(!length || length>=ARRAYSIZE(image))error=ERROR_FILENAME_EXCED_RANGE;
        else error=run16_native_backend_open_cancel(image,frontend->stop,&frontend->backend);
        if(!error)error=run16_native_view_begin(frontend->backend,&frontend->view);
        if(error) {
            run16_native_backend_close(frontend->backend);frontend->backend=NULL;
            LeaveCriticalSection(&frontend->io_lock);goto done;
        }
        frontend->thread=CreateThread(NULL,0,present,frontend,0,NULL);
        if(!frontend->thread) {
            error=GetLastError();run16_native_view_end(&frontend->view);
            run16_native_backend_close(frontend->backend);frontend->backend=NULL;
            LeaveCriticalSection(&frontend->io_lock);goto done;
        }
        LeaveCriticalSection(&frontend->io_lock);
        AcquireSRWLockExclusive(&control_lock);
        frontend->controls_live=TRUE;
        ReleaseSRWLockExclusive(&control_lock);
    }
    error=run16_native_backend_launch(frontend->backend,start,target);
done:
    LeaveCriticalSection(&frontend->lock);
    return error;
}
DWORD run16_native_frontend_wait(run16_native_frontend *frontend,HANDLE target,DWORD *result)
{
    HANDLE waits[2];DWORD wait,error;
    if(!frontend || !target || !result)return ERROR_INVALID_PARAMETER;
    waits[0]=target;waits[1]=frontend->thread;
    wait=WaitForMultipleObjects(2,waits,FALSE,INFINITE);
    if(WaitForSingleObject(target,0)==WAIT_OBJECT_0) {
        if(!GetExitCodeProcess(target,result))return GetLastError();
        /* Best-effort final display, independently bounded from completion.
         * A broken/stalled helper cannot replace an obtained target result. */
        ResetEvent(frontend->refreshed);SetEvent(frontend->refresh);
        waits[0]=frontend->refreshed;
        WaitForMultipleObjects(2,waits,FALSE,1000);
        return ERROR_SUCCESS;
    }
    if(wait==WAIT_FAILED)return GetLastError();
    if(!GetExitCodeThread(frontend->thread,&error))return GetLastError();
    return error ? error : ERROR_BROKEN_PIPE;
}
void run16_native_frontend_cancel(run16_native_frontend *frontend)
{
    if(frontend && frontend->stop)SetEvent(frontend->stop);
}
DWORD run16_native_frontend_members(run16_native_frontend *frontend,DWORD *members)
{
    DWORD error=ERROR_SUCCESS;
    if(!frontend || !members)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&frontend->lock);
    if(frontend->backend)error=run16_native_backend_members(frontend->backend,members);
    else *members=0;
    LeaveCriticalSection(&frontend->lock);
    return error;
}
DWORD run16_native_frontend_drain(run16_native_frontend *frontend)
{
    DWORD error=ERROR_SUCCESS,returned;
    if(!frontend)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&frontend->lock);
    EnterCriticalSection(&frontend->io_lock);
    if(frontend->dos_owner)error=ERROR_BUSY;
    else {
        if(frontend->backend){
            error=run16_native_view_present(frontend->backend,&frontend->view);
            if(!error)error=run16_native_view_reclaim_input(frontend->backend,&frontend->view,&returned);
        }
        /* No input forwarding after the final reclaim. Only after the above
         * exchanges may stop cancel the helper transport. */
        SetEvent(frontend->stop);
    }
    LeaveCriticalSection(&frontend->io_lock);
    LeaveCriticalSection(&frontend->lock);
    return error;
}
/* Only the original worker's block/resume edges select this binding. The
 * owner is a root-local channel address, never a process/task scheduler ID. */
DWORD run16_native_frontend_dos_bind(run16_native_frontend *frontend,const void *owner,BOOL active)
{
    DWORD error=ERROR_SUCCESS,returned;
    if(!frontend || !owner)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&frontend->io_lock);
    if(active && frontend->dos_owner==owner)goto done;
    if(frontend->dos_owner && frontend->dos_owner!=owner) { error=ERROR_BUSY;goto done; }
    if(!active && !frontend->dos_owner)goto done;
    if(frontend->backend) {
        if(active) {
            error=run16_native_view_present(frontend->backend,&frontend->view);
            if(!error)error=run16_native_view_reclaim_input(frontend->backend,&frontend->view,&returned);
            if(!error && !SetConsoleMode(frontend->view.input,frontend->view.input_mode))error=GetLastError();
        } else error=run16_native_view_seed(frontend->backend,&frontend->view);
    }
    if(!error)frontend->dos_owner=active ? owner : NULL;
done:
    SetEvent(frontend->changed);
    LeaveCriticalSection(&frontend->io_lock);
    return error;
}
DWORD run16_native_frontend_dos_enter(run16_native_frontend *frontend,const void *owner)
{
    if(!frontend || !owner)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&frontend->io_lock);
    if(frontend->dos_owner==owner)return ERROR_SUCCESS;
    LeaveCriticalSection(&frontend->io_lock);return ERROR_NOT_READY;
}
void run16_native_frontend_dos_leave(run16_native_frontend *frontend)
{
    LeaveCriticalSection(&frontend->io_lock);
}
void run16_native_frontend_dos_forget(run16_native_frontend *frontend,const void *owner)
{
    if(!frontend)return;
    EnterCriticalSection(&frontend->io_lock);
    if(frontend->dos_owner==owner)frontend->dos_owner=NULL;
    SetEvent(frontend->changed);
    LeaveCriticalSection(&frontend->io_lock);
}
void run16_native_frontend_destroy(run16_native_frontend *frontend)
{
    if(!frontend)return;
    AcquireSRWLockExclusive(&control_lock);
    if(control_owner==frontend) {
        control_owner=NULL;
        SetConsoleCtrlHandler(frontend_control,FALSE);
    }
    ReleaseSRWLockExclusive(&control_lock);
    if(frontend->stop)SetEvent(frontend->stop);
    run16_native_backend_cancel(frontend->backend);
    if(frontend->thread) { WaitForSingleObject(frontend->thread,INFINITE);CloseHandle(frontend->thread); }
    run16_native_view_end(&frontend->view);
    run16_native_backend_close(frontend->backend);
    if(frontend->stop)CloseHandle(frontend->stop);
    if(frontend->refresh)CloseHandle(frontend->refresh);
    if(frontend->refreshed)CloseHandle(frontend->refreshed);
    if(frontend->changed)CloseHandle(frontend->changed);
    if(frontend->control[0])CloseHandle(frontend->control[0]);
    if(frontend->control[1])CloseHandle(frontend->control[1]);
    DeleteCriticalSection(&frontend->io_lock);
    DeleteCriticalSection(&frontend->lock);
    HeapFree(GetProcessHeap(),0,frontend);
}
