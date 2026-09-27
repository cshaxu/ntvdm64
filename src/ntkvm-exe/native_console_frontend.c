#include "native_console_frontend.h"
#include "native_console_view.h"
#include "window_controller.h"
#include "window_keyboard.h"
#include "window_mouse.h"
#include "window_frame.h"
#include "lib/kvm-window/render.h"

/* One lazy backend per root frontend, not per inner launcher or native child.
 * No target registry or execution scheduler: Windows owns the native Console
 * group and each requester owns its actual process-completion handle. */
struct run16_native_frontend {
    CRITICAL_SECTION lock,io_lock,handoff_lock;
    run16_native_backend *backend;
    run16_native_console_view view;
    HANDLE stop,refresh,refreshed,thread,changed,control[2];
    HANDLE console_input,console_output,console_surface;
    HANDLE handoff,handoff_done;
    const void *handoff_owner;
    BOOL handoff_active;
    DWORD handoff_error;
    const void *dos_owner;
    const run16_console_video *dos_video;
    uint32_t dos_video_serial;
    BOOL controls_live;
    frontend_window_controller *window;
    frontend_keyboard_delivery keyboard;
    frontend_native_mouse native_mouse;
    frontend_dos_mouse dos_mouse;
    kvm_window_frame *window_frame;
    BOOL window_active;
    BOOL console_f_down,console_shortcut;
    INPUT_RECORD *dos_input;
    DWORD dos_input_count,dos_input_capacity;
    HANDLE dos_input_ready;
    volatile LONG display_request;
};
/* Win32's process-wide callback has no context argument. This single binding
 * belongs only to the root frontend; the lock joins callbacks before teardown.
 * Never wait for IPC or manipulate execution from the OS callback thread. */
static SRWLOCK control_lock=SRWLOCK_INIT;
static run16_native_frontend *control_owner;
static DWORD apply_dos_binding(run16_native_frontend *,const void *,BOOL);
static DWORD collect_dos_console(run16_native_frontend *);
/* Copied frontend transport records, serialized by io_lock. Windows still
 * owns Console processing and the guest owns its keyboard device. Grow before
 * mutation; no truncation or partial prepend on allocation failure. */
static DWORD dos_input_write(run16_native_frontend *frontend,
    const INPUT_RECORD *records,DWORD count,BOOL prepend)
{
    DWORD total;SIZE_T capacity;INPUT_RECORD *grown;
    if(count>MAXDWORD-frontend->dos_input_count)return ERROR_ARITHMETIC_OVERFLOW;
    total=frontend->dos_input_count+count;
    if(total>frontend->dos_input_capacity) {
        capacity=(SIZE_T)total;
        if(capacity>((SIZE_T)-1)/sizeof(*grown)-64)return ERROR_NOT_ENOUGH_MEMORY;
        capacity+=64;
        grown=frontend->dos_input ? HeapReAlloc(GetProcessHeap(),0,frontend->dos_input,capacity*sizeof(*grown)) :
            HeapAlloc(GetProcessHeap(),0,capacity*sizeof(*grown));
        if(!grown)return ERROR_NOT_ENOUGH_MEMORY;
        frontend->dos_input=grown;frontend->dos_input_capacity=(DWORD)capacity;
    }
    if(count) {
        if(prepend)memmove(frontend->dos_input+count,frontend->dos_input,
            frontend->dos_input_count*sizeof(*records));
        memcpy(frontend->dos_input+(prepend ? 0 : frontend->dos_input_count),records,count*sizeof(*records));
        frontend->dos_input_count=total;
    }
    return !total || SetEvent(frontend->dos_input_ready) ? ERROR_SUCCESS : GetLastError();
}
static DWORD window_records(void *context,const INPUT_RECORD *records,DWORD count)
{
    run16_native_frontend *frontend=context;
    /* Also retain native typeahead during the gap between DOS yielding and
     * lazy helper creation. The presentation owner drains it once available. */
    return dos_input_write(frontend,records,count,FALSE);
}
static lib_bool window_input(void *context,const frontend_window_input *input)
{
    run16_native_frontend *frontend=context;
    DWORD error;
    if(frontend->dos_owner) {
        error=frontend_dos_mouse_dispatch(&frontend->dos_mouse,input,window_records,frontend);
        if(error)return FALSE;
    } else {
        error=frontend_native_mouse_dispatch(&frontend->native_mouse,input,window_records,frontend);
        if(error)return FALSE;
    }
    return frontend_keyboard_dispatch(&frontend->keyboard,input,!frontend->dos_owner,
        window_records,frontend)==ERROR_SUCCESS;
}
static DWORD window_route(void *context,BOOL window,BOOL graphics)
{
    run16_native_frontend *frontend=context;
    (void)graphics;
    if(!window && frontend->dos_owner) {
        DWORD error=frontend_dos_mouse_leave(&frontend->dos_mouse,window_records,frontend);
        if(error)return error;
    }
    /* Drain records with their old source policy before changing visibility.
     * Guest prepend never uses this physical Console queue. */
    if((frontend->dos_owner || frontend->window_active) && frontend->window_active!=window) {
        DWORD error=collect_dos_console(frontend);if(error)return error;
    } else if(window && !frontend->window_active && frontend->backend) {
        DWORD pending,error;
        /* The old Console route still owns these records, including COMMAND
         * prepend. Deliver them before suppressing subsequent physical input;
         * reuse the native route's filtering/order and bounded batches. */
        if(!GetNumberOfConsoleInputEvents(frontend->view.input,&pending))return GetLastError();
        while(pending) {
            error=run16_native_view_forward_input(frontend->backend,&frontend->view);
            if(error)return error;
            pending=pending>64 ? pending-64 : 0;
        }
    }
    /* Recovered from the former presentation/console_route owner. Canonical
     * text remains writable off-screen; neither streams nor input move to this
     * surface. Only this frontend's presentation thread selects visibility. */
    if(window && !frontend->console_surface) {
        HANDLE surface=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
        CONSOLE_CURSOR_INFO cursor={1,FALSE};
        if(surface==INVALID_HANDLE_VALUE)return GetLastError();
        if(!SetConsoleCursorInfo(surface,&cursor)) {
            DWORD error=GetLastError();CloseHandle(surface);return error;
        }
        frontend->console_surface=surface;
    }
    if(frontend->window_active!=window &&
        !SetConsoleActiveScreenBuffer(window ? frontend->console_surface : frontend->console_output))
        return GetLastError();
    frontend->window_active=window;
    /* Painting must work before the user moves the mouse. Geometry may arrive
       after a forced Window selection; the frame path completes that case. */
    if(window && frontend->dos_owner && frontend->dos_mouse.width)
        return frontend_dos_mouse_enter(&frontend->dos_mouse,window_records,frontend);
    return ERROR_SUCCESS;
}
static DWORD native_frame(void *context,const run16_native_frame_info *info,const CHAR_INFO *cells,SIZE_T count)
{
    run16_native_frontend *frontend=context;DWORD error;
    if(frontend_window_mode(frontend->window)==FRONTEND_DISPLAY_WINDOW) {
        POINT pointer;
        error=frontend_native_mouse_geometry(&frontend->native_mouse,info->screen.srWindow,
            FRONTEND_NATIVE_CELL_WIDTH,FRONTEND_NATIVE_CELL_HEIGHT);
        pointer.x=frontend->native_mouse.x;pointer.y=frontend->native_mouse.y;
        if(!error)error=frontend_window_native_frame_pointer(info,cells,count,
            frontend->native_mouse.source ? &pointer : NULL,frontend->window_frame);
        /* present drains queued input synchronously. Publish the matching
         * geometry before that drain, not after it. */
        if(!error)error=frontend_window_present(frontend->window,frontend->window_frame,FALSE);
        return error;
    }
    error=run16_native_screen_apply(frontend->console_output,&info->screen,&info->cursor);
    if(!error)error=run16_native_cells_write(frontend->console_output,0,cells,(DWORD)count);
    return error;
}
/* Recover the former window_input_binding.c Console CAF debounce, with no
 * worker-global binding. Runs only after the frontend consumes a physical
 * Console record; guest peeking never triggers a policy change. Alt+Enter
 * remains Console-owned. */
static DWORD console_input(void *context,const INPUT_RECORD *record,BOOL *keep)
{
    run16_native_frontend *frontend=context;
    *keep=TRUE;
    if(record->EventType==KEY_EVENT && record->Event.KeyEvent.wVirtualKeyCode=='F') {
        const KEY_EVENT_RECORD *key=&record->Event.KeyEvent;
        BOOL chord=(key->dwControlKeyState&(LEFT_CTRL_PRESSED|RIGHT_CTRL_PRESSED)) &&
            (key->dwControlKeyState&(LEFT_ALT_PRESSED|RIGHT_ALT_PRESSED));
        if(!key->bKeyDown) {
            *keep=!frontend->console_shortcut;
            frontend->console_shortcut=frontend->console_f_down=FALSE;
        } else if(frontend->console_shortcut)*keep=FALSE;
        else {
            if(!frontend->window_active && !frontend->console_f_down && chord) {
                frontend->console_shortcut=TRUE;*keep=FALSE;
                InterlockedExchange(&frontend->display_request,2);
                if(!SetEvent(frontend->changed))return GetLastError();
            }
            frontend->console_f_down=TRUE;
        }
    }
    if(record->EventType==FOCUS_EVENT && !record->Event.FocusEvent.bSetFocus)
        frontend->console_f_down=frontend->console_shortcut=FALSE;
    return ERROR_SUCCESS;
}
static DWORD collect_dos_console(run16_native_frontend *frontend)
{
    typedef BOOL (WINAPI *read_input_ex)(HANDLE,PINPUT_RECORD,DWORD,LPDWORD,USHORT);
    read_input_ex read_nowait=(read_input_ex)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"ReadConsoleInputExW");
    INPUT_RECORD records[64];DWORD count,i,kept,error,pending;
    if(!read_nowait)return ERROR_CALL_NOT_IMPLEMENTED;
    if(!GetNumberOfConsoleInputEvents(frontend->console_input,&pending))return GetLastError();
    while(pending) {
        DWORD amount=pending<ARRAYSIZE(records) ? pending : ARRAYSIZE(records);
        if(!read_nowait(frontend->console_input,records,amount,&count,2))return GetLastError();
        if(!count)break;
        pending-=count;kept=0;
        if(frontend->window_active)continue; /* Inactive physical source. */
        for(i=0;i<count;++i) {
            BOOL keep;
            error=console_input(frontend,&records[i],&keep);if(error)return error;
            if(keep)records[kept++]=records[i];
        }
        error=dos_input_write(frontend,records,kept,FALSE);if(error)return error;
    }
    return ERROR_SUCCESS;
}
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
static DWORD present_loop(run16_native_frontend *frontend)
{
    for(;;) {
        HANDLE waits[10]={frontend->stop,frontend->changed,frontend->refresh,
            frontend->control[0],frontend->control[1],frontend->handoff,frontend_window_wake(frontend->window)};
        DWORD error=ERROR_SUCCESS,wait,count=7,helper_index=MAXDWORD;
        BOOL paused,has_backend,requested=WaitForSingleObject(frontend->refresh,0)==WAIT_OBJECT_0;
        if(WaitForSingleObject(frontend->stop,0)==WAIT_OBJECT_0)return ERROR_OPERATION_ABORTED;
        if(requested)ResetEvent(frontend->refresh);
        EnterCriticalSection(&frontend->io_lock);
        if(WaitForSingleObject(frontend->stop,0)==WAIT_OBJECT_0){
            LeaveCriticalSection(&frontend->io_lock);return ERROR_OPERATION_ABORTED;
        }
        /* A newly selected native reader needs its actual viewport before
         * dequeuing Window mouse input. Keep input queued during lazy helper
         * creation; neither invent geometry nor discard early button events. */
        if(!frontend->dos_owner && frontend->backend && !frontend->native_mouse.ready)
            error=run16_native_view_present(frontend->backend,&frontend->view);
        if(!error && (frontend->dos_owner ? frontend->dos_mouse.width!=0 :
            frontend->backend && frontend->native_mouse.ready))
            error=frontend_window_poll(frontend->window);
        if(!error) {
            LONG display=InterlockedExchange(&frontend->display_request,0);
            if(display)error=frontend_window_select(frontend->window,
                display==2 ? FRONTEND_DISPLAY_WINDOW : FRONTEND_DISPLAY_CONSOLE);
        }
        if(error) { LeaveCriticalSection(&frontend->io_lock);return error; }
        if(WaitForSingleObject(frontend->handoff,0)==WAIT_OBJECT_0) {
            ResetEvent(frontend->handoff);
            frontend->handoff_error=apply_dos_binding(frontend,frontend->handoff_owner,frontend->handoff_active);
            SetEvent(frontend->handoff_done);
        }
        has_backend=frontend->backend!=NULL;
        paused=frontend->dos_owner!=NULL || !has_backend;
        if(frontend->dos_owner || frontend->window_active) {
            error=collect_dos_console(frontend);
            if(error){LeaveCriticalSection(&frontend->io_lock);return error;}
            waits[count++]=frontend->console_input;
        }
        if(has_backend) {
            helper_index=count;
            waits[count++]=run16_native_backend_process(frontend->backend);
            if(!paused && !frontend->window_active)waits[count++]=frontend->view.input;
        }
        if(!paused) {
            if(frontend->dos_input_count) {
                error=run16_native_backend_input(frontend->backend,frontend->dos_input,frontend->dos_input_count);
                if(error){LeaveCriticalSection(&frontend->io_lock);return error;}
                frontend->dos_input_count=0;
                if(!ResetEvent(frontend->dos_input_ready)) {
                    error=GetLastError();LeaveCriticalSection(&frontend->io_lock);return error;
                }
            }
            error=run16_native_view_present(frontend->backend,&frontend->view);
            if(!error && !frontend->window_active && WaitForSingleObject(frontend->view.input,0)==WAIT_OBJECT_0)
                error=run16_native_view_forward_input(frontend->backend,&frontend->view);
        } else if(frontend->dos_owner && frontend->dos_video) {
            const run16_console_video *video=frontend->dos_video;
            /* BEGIN/partial DATA never replace the last completed frame.
             * Only an explicit TEXT publication clears graphics mode. */
            uint32_t serial=video->pixels ? video->published_serial : video->serial;
            if(serial!=frontend->dos_video_serial && (!video->pending || video->pixels)) {
                if(video->pixels) {
                    lib_u32 width,height;
                    error=frontend_window_dos_frame(video,frontend->window_frame);
                    if(!error && !kvm_window_frame_size(frontend->window_frame,&width,&height))
                        error=ERROR_INVALID_DATA;
                    if(!error)error=frontend_dos_mouse_geometry(&frontend->dos_mouse,width,height);
                    if(!error)error=frontend_window_present(frontend->window,frontend->window_frame,
                        video->description.kind==CONSOLE_VIDEO_DIB);
                    if(!error && frontend->window_active)
                        error=frontend_dos_mouse_enter(&frontend->dos_mouse,window_records,frontend);
                } else error=frontend_window_clear(frontend->window);
                if(!error)frontend->dos_video_serial=serial;
            }
        }
        LeaveCriticalSection(&frontend->io_lock);
        if(error && error!=ERROR_RETRY)return error;
        if(requested) {
            if(error==ERROR_RETRY)SetEvent(frontend->refresh);
            else SetEvent(frontend->refreshed); /* No native redraw over active DOS. */
        }
        wait=WaitForMultipleObjects(count,waits,FALSE,paused ? INFINITE : 30);
        if(wait==WAIT_OBJECT_0)return ERROR_OPERATION_ABORTED;
        if(helper_index!=MAXDWORD && wait==WAIT_OBJECT_0+helper_index)return ERROR_BROKEN_PIPE;
        if(wait==WAIT_FAILED)return GetLastError();
        if(has_backend && (wait==WAIT_OBJECT_0+3 || wait==WAIT_OBJECT_0+4)) {
            run16_native_host_request request={RUN16_NATIVE_HOST_VERSION,RUN16_NATIVE_CONTROL,0,0,wait-(WAIT_OBJECT_0+3)};
            run16_native_host_reply reply;
            /* Both Consoles implement one logical user Console. DOS workers
             * physically attached here already receive the original event;
             * forward once to the other Console, even while DOS owns I/O. */
            error=run16_native_backend_call(frontend->backend,&request,NULL,&reply,NULL,0);
            if(error || reply.status)return error ? error : reply.status;
        }
    }
}
static DWORD WINAPI present(void *context)
{
    run16_native_frontend *frontend=context;
    frontend_window_callbacks callbacks={frontend,window_input,window_route};
    DWORD error,ending;
    frontend->window_frame=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*frontend->window_frame));
    if(!frontend->window_frame)return ERROR_NOT_ENOUGH_MEMORY;
    error=frontend_window_create(&frontend->window,&callbacks,"NTVDM");
    if(!error)error=present_loop(frontend);
    ending=frontend_window_destroy(frontend->window);
    if(ending)return ending; /* Callback storage must outlive a failed join. */
    frontend->window=NULL;
    HeapFree(GetProcessHeap(),0,frontend->window_frame);frontend->window_frame=NULL;
    return error;
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
    InitializeCriticalSection(&frontend->handoff_lock);
    frontend->console_input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    frontend->console_output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    if(frontend->console_input==INVALID_HANDLE_VALUE || frontend->console_output==INVALID_HANDLE_VALUE) {
        error=GetLastError();run16_native_frontend_destroy(frontend);return error;
    }
    frontend->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    frontend->refresh=CreateEventW(NULL,TRUE,FALSE,NULL);
    frontend->refreshed=CreateEventW(NULL,TRUE,FALSE,NULL);
    frontend->changed=CreateEventW(NULL,FALSE,FALSE,NULL);
    frontend->control[0]=CreateEventW(NULL,FALSE,FALSE,NULL);
    frontend->control[1]=CreateEventW(NULL,FALSE,FALSE,NULL);
    frontend->handoff=CreateEventW(NULL,TRUE,FALSE,NULL);
    frontend->handoff_done=CreateEventW(NULL,TRUE,FALSE,NULL);
    frontend->dos_input_ready=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!frontend->stop || !frontend->refresh || !frontend->refreshed || !frontend->changed ||
        !frontend->control[0] || !frontend->control[1] || !frontend->handoff || !frontend->handoff_done || !frontend->dos_input_ready) {
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
    /* One I/O owner exists for the whole character session, including DOS-only
     * time before the first native child. Helper creation remains lazy. */
    frontend->thread=CreateThread(NULL,0,present,frontend,0,NULL);
    if(!frontend->thread) { error=GetLastError();run16_native_frontend_destroy(frontend);return error; }
    *output=frontend;return ERROR_SUCCESS;
}
DWORD run16_native_frontend_console(run16_native_frontend *frontend,HANDLE *input,HANDLE *output)
{
    DWORD error;
    if(!frontend || !input || !output || input==output)return ERROR_INVALID_PARAMETER;
    *input=NULL;*output=NULL;
    if(!DuplicateHandle(GetCurrentProcess(),frontend->console_input,GetCurrentProcess(),input,0,FALSE,DUPLICATE_SAME_ACCESS))
        return GetLastError();
    if(!DuplicateHandle(GetCurrentProcess(),frontend->console_output,GetCurrentProcess(),output,0,FALSE,DUPLICATE_SAME_ACCESS)) {
        error=GetLastError();CloseHandle(*input);*input=NULL;return error;
    }
    return ERROR_SUCCESS;
}
DWORD run16_native_frontend_display(run16_native_frontend *frontend,BOOL window)
{
    if(!frontend)return ERROR_INVALID_PARAMETER;
    if(WaitForSingleObject(frontend->stop,0)==WAIT_OBJECT_0)return ERROR_OPERATION_ABORTED;
    InterlockedExchange(&frontend->display_request,window ? 2 : 1);
    return SetEvent(frontend->changed) ? ERROR_SUCCESS : GetLastError();
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
        if(!error)error=run16_native_view_begin_on(frontend->backend,&frontend->view,
            frontend->console_input,frontend->console_output);
        if(!error) {
            frontend->view.window_context=frontend;
            frontend->view.window_frame=native_frame;
            frontend->view.console_input=console_input;
        }
        if(error) {
            run16_native_backend_close(frontend->backend);frontend->backend=NULL;
            LeaveCriticalSection(&frontend->io_lock);goto done;
        }
        LeaveCriticalSection(&frontend->io_lock);
        AcquireSRWLockExclusive(&control_lock);
        frontend->controls_live=TRUE;
        ReleaseSRWLockExclusive(&control_lock);
        SetEvent(frontend->changed);
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
            error=run16_native_view_sync_console(frontend->backend,&frontend->view);
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
static DWORD reclaim_dos_input(void *context,const INPUT_RECORD *records,DWORD count)
{
    INPUT_RECORD *keys;DWORD i,kept=0,error;
    /* A native reader's unconsumed mouse positions belong to its viewport,
     * not the resumed guest. Preserve all non-mouse typeahead in order. */
    if(!count)return ERROR_SUCCESS;
    keys=HeapAlloc(GetProcessHeap(),0,(SIZE_T)count*sizeof(*keys));
    if(!keys)return ERROR_NOT_ENOUGH_MEMORY;
    for(i=0;i<count;++i)if(records[i].EventType!=MOUSE_EVENT)keys[kept++]=records[i];
    error=dos_input_write(context,keys,kept,TRUE);
    HeapFree(GetProcessHeap(),0,keys);return error;
}
static DWORD apply_dos_binding(run16_native_frontend *frontend,const void *owner,BOOL active)
{
    DWORD error=ERROR_SUCCESS,returned;
    if(active && frontend->dos_owner==owner)goto done;
    if(frontend->dos_owner && frontend->dos_owner!=owner) { error=ERROR_BUSY;goto done; }
    if(!active && !frontend->dos_owner)goto done;
    /* The frontend's physical keyboard/window survives program handoff.
     * Drain accepted events to the old owner, but do not manufacture source
     * retirement/key releases merely because DOS yields to a native reader.
     * Console policy still closes its graphics-only Window on native return. */
    if(frontend->dos_owner ? frontend->dos_mouse.width!=0 : frontend->native_mouse.ready)
        error=frontend_window_mode(frontend->window)==FRONTEND_DISPLAY_WINDOW ?
            frontend_window_poll(frontend->window) : frontend_window_clear(frontend->window);
    if(error)goto done;
    if(!active) {
        DWORD i,kept=0;
        /* Original DOS block has already flushed its pending mouse IRQs.
         * Discard only that consumer's copied mouse records, not keyboard
         * typeahead. A native reader must never receive private DOS tags or
         * guest-coordinate mouse positions. */
        for(i=0;i<frontend->dos_input_count;++i) {
            WORD type=frontend->dos_input[i].EventType;
            if(type!=CONSOLE_INPUT_RELATIVE_MOUSE && type!=MOUSE_EVENT)
                frontend->dos_input[kept++]=frontend->dos_input[i];
        }
        frontend->dos_input_count=kept;
        ZeroMemory(&frontend->dos_mouse,sizeof(frontend->dos_mouse));
        if(!kept && !ResetEvent(frontend->dos_input_ready)) { error=GetLastError();goto done; }
    }
    if(active && frontend->backend) {
        /* Queue the release after old native input, then deliver that batch
         * before changing owner. Do not feed native positions to DOS. */
        error=frontend_native_mouse_release(&frontend->native_mouse,TRUE,window_records,frontend);
        if(!error && frontend->dos_input_count) {
            error=run16_native_backend_input(frontend->backend,frontend->dos_input,frontend->dos_input_count);
            if(!error) {
                frontend->dos_input_count=0;
                if(!ResetEvent(frontend->dos_input_ready))error=GetLastError();
            }
        }
        if(error)goto done;
    }
    if(!active && !frontend->window_active && frontend->dos_input_count) {
        typedef BOOL (WINAPI *prepend_input)(HANDLE,PINPUT_RECORD,DWORD,LPDWORD);
        prepend_input prepend=(prepend_input)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"WriteConsoleInputVDMW");
        if(!prepend) { error=ERROR_CALL_NOT_IMPLEMENTED;goto done; }
        if(!prepend(frontend->console_input,frontend->dos_input,frontend->dos_input_count,&returned)) {
            error=GetLastError();goto done;
        }
        if(returned!=frontend->dos_input_count) { error=ERROR_WRITE_FAULT;goto done; }
        frontend->dos_input_count=0;
        if(!ResetEvent(frontend->dos_input_ready)) { error=GetLastError();goto done; }
    }
    if(frontend->backend) {
        if(active) {
            error=run16_native_view_sync_console(frontend->backend,&frontend->view);
            if(!error)error=run16_native_view_reclaim_input_to(frontend->backend,&frontend->view,&returned,
                reclaim_dos_input,frontend);
            if(!error && !SetConsoleMode(frontend->view.input,frontend->view.input_mode))error=GetLastError();
        } else error=run16_native_view_seed(frontend->backend,&frontend->view);
    }
    if(!error) {
        frontend->dos_owner=active ? owner : NULL;
        frontend->native_mouse.ready=FALSE;
        frontend->dos_video=NULL;frontend->dos_video_serial=0;
    }
done:
    return error;
}
DWORD run16_native_frontend_dos_bind(run16_native_frontend *frontend,const void *owner,BOOL active)
{
    HANDLE waits[3];DWORD wait,error;
    if(!frontend || !owner)return ERROR_INVALID_PARAMETER;
    /* Serialize callers, but never hold io_lock while waiting for its owner.
     * Requests alter only input/presentation binding, never execution order. */
    EnterCriticalSection(&frontend->handoff_lock);
    frontend->handoff_owner=owner;frontend->handoff_active=active;
    waits[0]=frontend->handoff_done;waits[1]=frontend->stop;waits[2]=frontend->thread;
    if(!ResetEvent(frontend->handoff_done) || !SetEvent(frontend->handoff))error=GetLastError();
    else {
        wait=WaitForMultipleObjects(3,waits,FALSE,INFINITE);
        if(wait==WAIT_OBJECT_0)error=frontend->handoff_error;
        else if(wait==WAIT_FAILED)error=GetLastError();
        else if(wait==WAIT_OBJECT_0+1)error=ERROR_OPERATION_ABORTED;
        else if(!GetExitCodeThread(frontend->thread,&error))error=GetLastError();
        else if(!error)error=ERROR_BROKEN_PIPE;
    }
    LeaveCriticalSection(&frontend->handoff_lock);
    return error;
}
DWORD run16_native_frontend_dos_enter(run16_native_frontend *frontend,const void *owner)
{
    if(!frontend || !owner)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&frontend->io_lock);
    if(frontend->dos_owner==owner)return ERROR_SUCCESS;
    LeaveCriticalSection(&frontend->io_lock);return ERROR_NOT_READY;
}
DWORD run16_native_frontend_dos_video(run16_native_frontend *frontend,const void *owner,
    const run16_console_video *video)
{
    DWORD error=run16_native_frontend_dos_enter(frontend,owner);
    if(error)return error;
    frontend->dos_video=video;
    error=SetEvent(frontend->changed) ? ERROR_SUCCESS : GetLastError();
    run16_native_frontend_dos_leave(frontend);
    return error;
}
void run16_native_frontend_dos_leave(run16_native_frontend *frontend)
{
    LeaveCriticalSection(&frontend->io_lock);
}
BOOL run16_native_frontend_text_frame_required(run16_native_frontend *frontend)
{
    return frontend_window_mode(frontend->window)==FRONTEND_DISPLAY_WINDOW;
}
HANDLE run16_native_frontend_dos_ready(run16_native_frontend *frontend)
{
    return frontend->dos_input_ready;
}
DWORD run16_native_frontend_dos_read(run16_native_frontend *frontend,BOOL peek,
    INPUT_RECORD *records,DWORD capacity,DWORD *read)
{
    DWORD count=frontend->dos_input_count;
    if(count>capacity)count=capacity;
    if(count)memcpy(records,frontend->dos_input,count*sizeof(*records));
    if(!peek && count) {
        frontend->dos_input_count-=count;
        memmove(frontend->dos_input,frontend->dos_input+count,frontend->dos_input_count*sizeof(*records));
        if(!frontend->dos_input_count && !ResetEvent(frontend->dos_input_ready))return GetLastError();
    }
    *read=count;return ERROR_SUCCESS;
}
DWORD run16_native_frontend_dos_prepend(run16_native_frontend *frontend,const INPUT_RECORD *records,DWORD count)
{
    return dos_input_write(frontend,records,count,TRUE);
}
void run16_native_frontend_dos_forget(run16_native_frontend *frontend,const void *owner)
{
    if(!frontend)return;
    /* Retire Window input against its DOS owner before removing the source.
     * A stopped/failed presentation thread may refuse the handoff; the lock
     * below still detaches its borrowed channel storage before disposal. */
    (void)run16_native_frontend_dos_bind(frontend,owner,FALSE);
    EnterCriticalSection(&frontend->io_lock);
    if(frontend->dos_owner==owner) {
        frontend->dos_owner=NULL;frontend->dos_video=NULL;
        frontend->dos_video_serial=0;
        ZeroMemory(&frontend->dos_mouse,sizeof(frontend->dos_mouse));
    }
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
    if(frontend->window)return; /* Failed Window join: retain all callback context. */
    /* Restore before closing a surface even after a presentation failure. A
     * failed restore retains its handles for terminal process cleanup. */
    if(frontend->console_surface) {
        if(!SetConsoleActiveScreenBuffer(frontend->console_output))return;
        CloseHandle(frontend->console_surface);
    }
    run16_native_view_end(&frontend->view);
    run16_native_backend_close(frontend->backend);
    if(frontend->console_input && frontend->console_input!=INVALID_HANDLE_VALUE)CloseHandle(frontend->console_input);
    if(frontend->console_output && frontend->console_output!=INVALID_HANDLE_VALUE)CloseHandle(frontend->console_output);
    if(frontend->stop)CloseHandle(frontend->stop);
    if(frontend->refresh)CloseHandle(frontend->refresh);
    if(frontend->refreshed)CloseHandle(frontend->refreshed);
    if(frontend->changed)CloseHandle(frontend->changed);
    if(frontend->control[0])CloseHandle(frontend->control[0]);
    if(frontend->control[1])CloseHandle(frontend->control[1]);
    if(frontend->handoff)CloseHandle(frontend->handoff);
    if(frontend->handoff_done)CloseHandle(frontend->handoff_done);
    if(frontend->dos_input_ready)CloseHandle(frontend->dos_input_ready);
    if(frontend->dos_input)HeapFree(GetProcessHeap(),0,frontend->dos_input);
    DeleteCriticalSection(&frontend->handoff_lock);
    DeleteCriticalSection(&frontend->io_lock);
    DeleteCriticalSection(&frontend->lock);
    HeapFree(GetProcessHeap(),0,frontend);
}
