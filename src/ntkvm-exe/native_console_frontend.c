#include "native_console_frontend.h"
#include "console_frontend.h"
#include "window_controller.h"
#include "window_keyboard.h"
#include "window_mouse.h"
#include "window_frame.h"
#include "lib/kvm-window/render.h"
#include "opennt-abi/host-compat/include/console_grid.h"
#include <stdint.h>

/* Visible presentation and copied input only. DOS temporarily owns the I/O
 * route; the attached native worker is the return route, not a local backend. */
typedef struct binding_waiter {
    struct binding_waiter *next;
    HANDLE changed;
} binding_waiter;
struct run16_native_frontend {
    CRITICAL_SECTION lock,io_lock,handoff_lock;
    binding_waiter *binding_waiters; /* guarded by io_lock; each waiter owns its event */
    DWORD original_input_mode;
    BOOL input_mode_saved;
    DWORD parked_input_mode;
    BOOL parked;
    HANDLE stop,refresh,refreshed,thread,changed,park,park_done,control[2];
    DWORD park_error;
    HANDLE console_input,console_output,console_surface,dos_surface;
    SMALL_RECT logical_window;
    /* Root teardown returns the canonical buffer without undoing the DOS
     * worker's final cell grid, viewport, or cursor position. */
    CONSOLE_CURSOR_INFO original_cursor;
    BOOL original_cursor_saved;
    BOOL native_geometry_pending;
    HANDLE handoff,handoff_done;
    const void *handoff_owner;
    BOOL handoff_active,handoff_native;
    DWORD handoff_error;
    const void *dos_owner,*native_owner;
    const void *title_owner;
    char worker_title[CONSOLE_IO_TITLE_BYTES];
    BOOL worker_title_valid;
    const void *dos_pending;
    const run16_console_video *dos_video,*native_video;
    uint32_t dos_video_serial,native_video_serial;
    console_text_configuration text_configuration;
    uint32_t text_revision;
    BOOL controls_live;
    frontend_window_controller *window;
    frontend_keyboard_delivery keyboard;
    frontend_dos_mouse dos_mouse;
    DWORD pointer_control;
    const frontend_window_input *pointer_input;
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
/* Called only with io_lock held. Per-waiter manual events avoid both lost
 * transitions and competition over a shared auto-reset notification. */
static void signal_binding_waiters(run16_native_frontend *frontend)
{
    binding_waiter *waiter;
    for(waiter=frontend->binding_waiters;waiter;waiter=waiter->next)
        SetEvent(waiter->changed);
}
static DWORD apply_binding(run16_native_frontend *,const void *,BOOL,BOOL);
static DWORD collect_dos_console(run16_native_frontend *);
static DWORD refresh_window_title(run16_native_frontend *frontend)
{
    char title[KVM_WINDOW_TITLE_CAPACITY]={0};
    DWORD length;
    const void *active=frontend->dos_owner ? frontend->dos_owner : frontend->native_owner;
    if(active && frontend->worker_title_valid && frontend->title_owner==active)
        return frontend_window_set_title(frontend->window,frontend->worker_title);
    SetLastError(ERROR_SUCCESS);
    length=GetConsoleTitleA(title,sizeof(title));
    /* A disappearing root Console is handled by the existing lifetime path;
     * a cosmetic title read must not block a pending worker handoff. */
    if(!length && GetLastError()!=ERROR_SUCCESS)return ERROR_SUCCESS;
    /* The local Window control payload has a fixed capacity. Keep its
     * termination valid even when the Console title is longer. */
    title[sizeof(title)-1]=0;
    return frontend_window_set_title(frontend->window,title);
}
static DWORD restore_cursor_shape(run16_native_frontend *frontend)
{
    if(!frontend)return ERROR_INVALID_PARAMETER;
    if(frontend->original_cursor_saved &&
        !SetConsoleCursorInfo(frontend->console_output,&frontend->original_cursor))return GetLastError();
    return ERROR_SUCCESS;
}
/* DOS's OpenNT Console API calls operate on an inactive buffer. In a
 * multi-tab terminal, changing the active Conhost buffer to 28 rows need not
 * change the terminal's 30-row display. Keep the physical buffer unchanged
 * and project complete logical cells and cursor after each DOS mutation. */
static DWORD copy_grid(HANDLE source,HANDLE target,SMALL_RECT source_rect,COORD target_origin)
{
    COORD origin={0,0},size;
    SMALL_RECT target_rect,actual;
    CHAR_INFO *cells;
    SIZE_T count;
    BOOL ok;
    DWORD error;
    size.X=source_rect.Right-source_rect.Left+1;
    size.Y=source_rect.Bottom-source_rect.Top+1;
    if(size.X<=0 || size.Y<=0 || (SIZE_T)size.X>1000000u/(SIZE_T)size.Y)
        return ERROR_INVALID_DATA;
    count=(SIZE_T)size.X*(SIZE_T)size.Y;
    cells=HeapAlloc(GetProcessHeap(),0,count*sizeof(*cells));
    if(!cells)return ERROR_NOT_ENOUGH_MEMORY;
    actual=source_rect;
    ok=ReadConsoleOutputW(source,cells,size,origin,&actual) &&
        actual.Left==source_rect.Left && actual.Top==source_rect.Top &&
        actual.Right==source_rect.Right && actual.Bottom==source_rect.Bottom;
    if(ok) {
        target_rect=(SMALL_RECT){target_origin.X,target_origin.Y,
            target_origin.X+size.X-1,target_origin.Y+size.Y-1};
        actual=target_rect;
        ok=WriteConsoleOutputW(target,cells,size,origin,&actual) &&
            actual.Left==target_rect.Left && actual.Top==target_rect.Top &&
            actual.Right==target_rect.Right && actual.Bottom==target_rect.Bottom;
    }
    error=ok ? ERROR_SUCCESS : GetLastError();
    HeapFree(GetProcessHeap(),0,cells);
    return error;
}
static DWORD prepare_dos_surface(run16_native_frontend *frontend)
{
    CONSOLE_SCREEN_BUFFER_INFO visible,shadow;
    SMALL_RECT shrink,window;
    COORD cursor,viewport_size;
    DWORD error;
    if(!GetConsoleScreenBufferInfo(frontend->console_output,&visible))return GetLastError();
    if(!frontend->dos_surface) {
        frontend->dos_surface=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
        if(frontend->dos_surface==INVALID_HANDLE_VALUE) {
            frontend->dos_surface=NULL;return GetLastError();
        }
    }
    if(!GetConsoleScreenBufferInfo(frontend->dos_surface,&shadow))return GetLastError();
    viewport_size=(COORD){visible.srWindow.Right-visible.srWindow.Left+1,
        visible.srWindow.Bottom-visible.srWindow.Top+1};
    shrink=(SMALL_RECT){0,0,min(shadow.dwSize.X,viewport_size.X)-1,
        min(shadow.dwSize.Y,viewport_size.Y)-1};
    if(!SetConsoleWindowInfo(frontend->dos_surface,TRUE,&shrink) ||
        !SetConsoleScreenBufferSize(frontend->dos_surface,viewport_size))return GetLastError();
    window=(SMALL_RECT){0,0,viewport_size.X-1,viewport_size.Y-1};
    if(!SetConsoleWindowInfo(frontend->dos_surface,TRUE,&window))return GetLastError();
    error=copy_grid(frontend->console_output,frontend->dos_surface,
        visible.srWindow,(COORD){0,0});
    if(error)return error;
    cursor=(COORD){visible.dwCursorPosition.X-visible.srWindow.Left,
        visible.dwCursorPosition.Y-visible.srWindow.Top};
    if(cursor.X<0 || cursor.Y<0 || cursor.X>=viewport_size.X ||
        cursor.Y>=viewport_size.Y)cursor=(COORD){0,0};
    if(!SetConsoleCursorPosition(frontend->dos_surface,cursor))return GetLastError();
    frontend->logical_window=window;
    error=run16_console_prepare_dos(frontend->dos_surface,&frontend->logical_window);
    if(error)return error;
    frontend->native_geometry_pending=FALSE;
    return run16_native_frontend_project_dos(frontend);
}
DWORD run16_native_frontend_project_dos(run16_native_frontend *frontend)
{
    CONSOLE_SCREEN_BUFFER_INFO logical,visible;
    DWORD count,error;
    COORD cursor;
    if(!frontend || !frontend->dos_surface)return ERROR_INVALID_STATE;
    if(!GetConsoleScreenBufferInfo(frontend->dos_surface,&logical) ||
        !GetConsoleScreenBufferInfo(frontend->console_output,&visible))return GetLastError();
    /* The host viewport may be one row shorter than OpenNT's selected VGA
     * mode (for example, 24 visible rows select DOS 25). Keep the complete
     * logical page in the canonical buffer; the cursor lets Console choose
     * which portion is physically visible. Never reject the DOS activation. */
    if((LONG)visible.srWindow.Left+logical.dwSize.X>SHRT_MAX ||
        (LONG)visible.srWindow.Top+logical.dwSize.Y>SHRT_MAX)
        return ERROR_ARITHMETIC_OVERFLOW;
    if(visible.dwSize.X<visible.srWindow.Left+logical.dwSize.X ||
        visible.dwSize.Y<visible.srWindow.Top+logical.dwSize.Y) {
        COORD size={max(visible.dwSize.X,(SHORT)(visible.srWindow.Left+logical.dwSize.X)),
            max(visible.dwSize.Y,(SHORT)(visible.srWindow.Top+logical.dwSize.Y))};
        if(!SetConsoleScreenBufferSize(frontend->console_output,size))return GetLastError();
    }
    error=copy_grid(frontend->dos_surface,frontend->console_output,
        (SMALL_RECT){0,0,logical.dwSize.X-1,logical.dwSize.Y-1},
        (COORD){visible.srWindow.Left,visible.srWindow.Top});
    if(error)return error;
    if(visible.srWindow.Right-visible.srWindow.Left+1>logical.dwSize.X ||
        visible.srWindow.Bottom-visible.srWindow.Top+1>logical.dwSize.Y) {
        SHORT row;
        for(row=visible.srWindow.Top;row<=visible.srWindow.Bottom;++row) {
            COORD start;
            DWORD excess;
            if(row<visible.srWindow.Top+logical.dwSize.Y) {
                start=(COORD){visible.srWindow.Left+logical.dwSize.X,row};
                excess=visible.srWindow.Right-start.X+1;
            } else {
                start=(COORD){visible.srWindow.Left,row};
                excess=visible.srWindow.Right-start.X+1;
            }
            if(!excess)continue;
            if(!FillConsoleOutputCharacterW(frontend->console_output,L' ',excess,start,&count))
                return GetLastError();
            if(count!=excess)return ERROR_WRITE_FAULT;
            if(!FillConsoleOutputAttribute(frontend->console_output,logical.wAttributes,
                excess,start,&count))return GetLastError();
            if(count!=excess)return ERROR_WRITE_FAULT;
        }
    }
    cursor=(COORD){visible.srWindow.Left+logical.dwCursorPosition.X,
        visible.srWindow.Top+logical.dwCursorPosition.Y};
    if(!SetConsoleCursorPosition(frontend->console_output,cursor))return GetLastError();
    return ERROR_SUCCESS;
}
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
    if(!frontend->dos_owner && count && records[0].EventType==CONSOLE_INPUT_RELATIVE_MOUSE) {
        INPUT_RECORD translated[2];DWORD i;
        if(count>ARRAYSIZE(translated))return ERROR_INVALID_DATA;
        memcpy(translated,records,count*sizeof(*records));
        for(i=0;i<count;++i)if(records[i].EventType==CONSOLE_INPUT_RELATIVE_MOUSE) {
            console_mouse_input motion;console_pointer_input pointer;
            memcpy(&motion,&records[i].Event,sizeof(motion));
            pointer.dx=motion.dx;pointer.dy=motion.dy;pointer.control=frontend->pointer_control;
            pointer.buttons=motion.buttons;pointer.action=motion.action;
            if(motion.action==CONSOLE_MOUSE_MOVE && frontend->pointer_input &&
                frontend->window_frame && frontend->window_frame->valid &&
                !frontend->window_frame->graphics &&
                frontend_native_pointer_position(frontend->pointer_input,
                    frontend->window_frame->text.base.text_columns*8u,
                    frontend->window_frame->text.base.text_rows*
                        (frontend->window_frame->text.base.font_height ?
                         frontend->window_frame->text.base.font_height : 16u),
                    &pointer.dx,&pointer.dy))pointer.action=CONSOLE_MOUSE_POSITION;
            translated[i].EventType=CONSOLE_INPUT_POINTER;
            memcpy(&translated[i].Event,&pointer,sizeof(pointer));
        }
        return dos_input_write(frontend,translated,count,FALSE);
    }
    /* Both workers consume this copied queue through the same channel. */
    return dos_input_write(frontend,records,count,FALSE);
}
static lib_bool window_input(void *context,const frontend_window_input *input)
{
    run16_native_frontend *frontend=context;
    DWORD error;
    frontend->pointer_control=input->control_state;
    frontend->pointer_input=input;
    error=frontend_dos_mouse_dispatch(&frontend->dos_mouse,input,window_records,frontend);
    frontend->pointer_input=NULL;
    if(error)return FALSE;
    return frontend_keyboard_dispatch(&frontend->keyboard,input,!frontend->dos_owner,
        window_records,frontend)==ERROR_SUCCESS;
}
static DWORD window_route(void *context,BOOL window,BOOL graphics)
{
    run16_native_frontend *frontend=context;
    (void)graphics;
    if(!window) {
        DWORD error=frontend_dos_mouse_leave(&frontend->dos_mouse,window_records,frontend);
        if(error)return error;
    }
    /* Drain records with their old source policy before changing visibility.
     * Guest prepend never uses this physical Console queue. */
    if((frontend->dos_owner || frontend->native_owner) && frontend->window_active!=window) {
        DWORD error=collect_dos_console(frontend);if(error)return error;
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
    if(window && frontend->dos_mouse.width)
        return frontend_dos_mouse_enter(&frontend->dos_mouse,window_records,frontend);
    return ERROR_SUCCESS;
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
        HANDLE waits[8]={frontend->stop,frontend->changed,frontend->refresh,
            frontend->handoff,frontend->park,frontend_window_wake(frontend->window)};
        DWORD error=0,wait,count=6;
        BOOL requested=WaitForSingleObject(frontend->refresh,0)==WAIT_OBJECT_0;
        const run16_console_video *video;
        uint32_t *published;
        if(WaitForSingleObject(frontend->stop,0)==WAIT_OBJECT_0)return ERROR_OPERATION_ABORTED;
        if(requested)ResetEvent(frontend->refresh);
        EnterCriticalSection(&frontend->io_lock);
        if(WaitForSingleObject(frontend->handoff,0)==WAIT_OBJECT_0) {
            ResetEvent(frontend->handoff);
            frontend->handoff_error=apply_binding(frontend,frontend->handoff_owner,
                frontend->handoff_active,frontend->handoff_native);
            SetEvent(frontend->handoff_done);
        }
        if(WaitForSingleObject(frontend->park,0)==WAIT_OBJECT_0) {
            ResetEvent(frontend->park);
            frontend->park_error=(frontend->dos_owner || frontend->native_owner) ? ERROR_BUSY :
                frontend_window_clear(frontend->window);
            if(!frontend->park_error)
                frontend->park_error=frontend_window_select(frontend->window,FRONTEND_DISPLAY_CONSOLE);
            if(!frontend->park_error &&
                !GetConsoleMode(frontend->console_input,&frontend->parked_input_mode))
                frontend->park_error=GetLastError();
            if(!frontend->park_error && frontend->input_mode_saved &&
                !SetConsoleMode(frontend->console_input,frontend->original_input_mode))
                frontend->park_error=GetLastError();
            if(!frontend->park_error) {
                frontend->parked=TRUE;
                frontend->console_f_down=frontend->console_shortcut=FALSE;
            }
            SetEvent(frontend->park_done);
        }
        (void)refresh_window_title(frontend);
        if(frontend->dos_owner || frontend->native_owner || frontend->window_active) {
            error=collect_dos_console(frontend);
            waits[count++]=frontend->console_input;
        }
        if(!error && frontend->dos_mouse.width)
            error=frontend_window_poll(frontend->window);
        if(!error) {
            LONG display=InterlockedExchange(&frontend->display_request,0);
            if(display)error=frontend_window_select(frontend->window,
                display==2 ? FRONTEND_DISPLAY_WINDOW : FRONTEND_DISPLAY_CONSOLE);
        }
        video=frontend->dos_owner ? frontend->dos_video : frontend->native_video;
        published=frontend->dos_owner ? &frontend->dos_video_serial : &frontend->native_video_serial;
        if(!error && video) {
            uint32_t serial=video->published_serial;
            if(serial!=*published && (!video->pending || video->pixels)) {
                if(video->pixels) {
                    lib_u32 width,height;
                    error=frontend_window_dos_frame(video,frontend->window_frame);
                    if(!error && !kvm_window_frame_size(frontend->window_frame,&width,&height))
                        error=ERROR_INVALID_DATA;
                    if(!error)
                        error=frontend_dos_mouse_geometry(&frontend->dos_mouse,width,height);
                    if(!error)error=frontend_window_present(frontend->window,frontend->window_frame,
                        video->description.kind==CONSOLE_VIDEO_DIB);
                    if(!error && frontend->window_active)
                        error=frontend_dos_mouse_enter(&frontend->dos_mouse,window_records,frontend);
                } else error=frontend_window_clear(frontend->window);
                if(!error)*published=serial;
            }
        }
        LeaveCriticalSection(&frontend->io_lock);
        if(error && error!=ERROR_RETRY)return error;
        if(requested) {
            if(error==ERROR_RETRY)SetEvent(frontend->refresh);
            else SetEvent(frontend->refreshed);
        }
        wait=WaitForMultipleObjects(count,waits,FALSE,INFINITE);
        if(wait==WAIT_OBJECT_0)return ERROR_OPERATION_ABORTED;
        if(wait==WAIT_FAILED)return GetLastError();
    }
}
static DWORD WINAPI present(void *context)
{
    run16_native_frontend *frontend=context;
    frontend_window_callbacks callbacks={frontend,window_input,window_route};
    DWORD error,ending;
    frontend->window_frame=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*frontend->window_frame));
    if(!frontend->window_frame)return ERROR_NOT_ENOUGH_MEMORY;
    error=frontend_window_create(&frontend->window,&callbacks,"");
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
    {
        CONSOLE_SCREEN_BUFFER_INFO info;
        /* Preserve the Console that the launcher handed over.  A worker's
         * first explicit frame/geometry request owns any conversion; forcing
         * 80x25 here loses scrollback and can form an invalid rectangle when
         * the current viewport is offset. */
        if(!GetConsoleScreenBufferInfo(frontend->console_output,&info)) {
            error=GetLastError();run16_native_frontend_destroy(frontend);return error;
        }
        frontend->logical_window=info.srWindow;
        if(!GetConsoleCursorInfo(frontend->console_output,&frontend->original_cursor)) {
            error=GetLastError();run16_native_frontend_destroy(frontend);return error;
        }
        frontend->original_cursor_saved=TRUE;
    }
    if(!GetConsoleMode(frontend->console_input,&frontend->original_input_mode)) {
        error=GetLastError();run16_native_frontend_destroy(frontend);return error;
    }
    frontend->input_mode_saved=TRUE;
    frontend->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    frontend->refresh=CreateEventW(NULL,TRUE,FALSE,NULL);
    frontend->refreshed=CreateEventW(NULL,TRUE,FALSE,NULL);
    frontend->changed=CreateEventW(NULL,FALSE,FALSE,NULL);
    frontend->park=CreateEventW(NULL,TRUE,FALSE,NULL);
    frontend->park_done=CreateEventW(NULL,TRUE,FALSE,NULL);
    frontend->control[0]=CreateEventW(NULL,FALSE,FALSE,NULL);
    frontend->control[1]=CreateEventW(NULL,FALSE,FALSE,NULL);
    frontend->handoff=CreateEventW(NULL,TRUE,FALSE,NULL);
    frontend->handoff_done=CreateEventW(NULL,TRUE,FALSE,NULL);
    frontend->dos_input_ready=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!frontend->stop || !frontend->refresh || !frontend->refreshed || !frontend->changed ||
        !frontend->park || !frontend->park_done ||
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
    /* One visible I/O owner exists before either kind of worker attaches. */
    frontend->thread=CreateThread(NULL,0,present,frontend,0,NULL);
    if(!frontend->thread) { error=GetLastError();run16_native_frontend_destroy(frontend);return error; }
    *output=frontend;return ERROR_SUCCESS;
}
SMALL_RECT *run16_native_frontend_text_region(run16_native_frontend *frontend)
{ return frontend ? &frontend->logical_window : NULL; }
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
DWORD run16_native_frontend_dos_console(run16_native_frontend *frontend,HANDLE *output)
{
    if(!frontend || !output || !frontend->dos_surface)return ERROR_INVALID_PARAMETER;
    *output=NULL;
    return DuplicateHandle(GetCurrentProcess(),frontend->dos_surface,GetCurrentProcess(),
        output,0,FALSE,DUPLICATE_SAME_ACCESS) ? ERROR_SUCCESS : GetLastError();
}
DWORD run16_native_frontend_display(run16_native_frontend *frontend,BOOL window)
{
    if(!frontend)return ERROR_INVALID_PARAMETER;
    if(WaitForSingleObject(frontend->stop,0)==WAIT_OBJECT_0)return ERROR_OPERATION_ABORTED;
    InterlockedExchange(&frontend->display_request,window ? 2 : 1);
    return SetEvent(frontend->changed) ? ERROR_SUCCESS : GetLastError();
}
void run16_native_frontend_console_title_changed(run16_native_frontend *frontend)
{
    if(frontend && frontend->changed)SetEvent(frontend->changed);
}
void run16_native_frontend_worker_title(run16_native_frontend *frontend,const void *owner,const char *title)
{
    if(!frontend || !owner || !title ||
        (frontend->dos_owner ? frontend->dos_owner : frontend->native_owner)!=owner)return;
    strcpy_s(frontend->worker_title,sizeof(frontend->worker_title),title);
    frontend->title_owner=owner;frontend->worker_title_valid=TRUE;
    SetEvent(frontend->changed);
}
void run16_native_frontend_cancel(run16_native_frontend *frontend)
{
    if(frontend && frontend->stop) {
        SetEvent(frontend->stop);
    }
}
DWORD run16_native_frontend_drain(run16_native_frontend *frontend)
{
    DWORD error=ERROR_SUCCESS;
    if(!frontend)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&frontend->lock);
    EnterCriticalSection(&frontend->io_lock);
    if(frontend->dos_owner)error=ERROR_BUSY;
    else {
        /* Delivered native input remains backend-owned; final presentation
         * must not reclaim or replay it into another execution consumer. */
        SetEvent(frontend->stop);
    }
    LeaveCriticalSection(&frontend->io_lock);
    LeaveCriticalSection(&frontend->lock);
    return error;
}
DWORD run16_native_frontend_park(run16_native_frontend *frontend)
{
    HANDLE waits[3];DWORD wait,error;
    if(!frontend)return ERROR_INVALID_PARAMETER;
    waits[0]=frontend->park_done;waits[1]=frontend->stop;waits[2]=frontend->thread;
    if(!ResetEvent(frontend->park_done) || !SetEvent(frontend->park))return GetLastError();
    wait=WaitForMultipleObjects(3,waits,FALSE,INFINITE);
    if(wait==WAIT_OBJECT_0)return frontend->park_error;
    if(wait==WAIT_OBJECT_0+1)return ERROR_OPERATION_ABORTED;
    if(wait==WAIT_FAILED)return GetLastError();
    if(!GetExitCodeThread(frontend->thread,&error))return GetLastError();
    return error ? error : ERROR_BROKEN_PIPE;
}
/* Only the original worker's block/resume edges select this binding. The
 * owner is a root-local channel address, never a process/task scheduler ID. */
static DWORD apply_binding(run16_native_frontend *frontend,const void *owner,BOOL active,BOOL native)
{
    const void **slot=native ? &frontend->native_owner : &frontend->dos_owner;
    DWORD error=0,i,kept=0;
    /* The current native producer publishes its final screen before DOS is
     * allowed to import it. This is I/O ownership, not task scheduling. */
    if(active && !native && frontend->native_owner) {
        if(frontend->dos_pending && frontend->dos_pending!=owner)return ERROR_BUSY;
        frontend->dos_pending=owner;
        signal_binding_waiters(frontend);
        return ERROR_BUSY;
    }
    if(active && !native && frontend->dos_pending && frontend->dos_pending!=owner)return ERROR_BUSY;
    if(active && native && (frontend->dos_owner || frontend->dos_pending))return ERROR_NOT_READY;
    if(active && *slot==owner)return 0;
    if(*slot && *slot!=owner)return ERROR_BUSY;
    if(!active && !*slot)return 0;
    /* A borrowed root restores the outer CMD's cooked mode while idle. Its
     * resident worker resumes the mode it had before that temporary return. */
    if(active && frontend->parked) {
        if(!SetConsoleMode(frontend->console_input,frontend->parked_input_mode))
            return GetLastError();
        frontend->parked=FALSE;
    }
    if(!native && active) {
        error=prepare_dos_surface(frontend);
        if(error)return error;
    }
    /* Initial DOS startup retains the original Console scrollback path.
     * Only a published native page needs conversion before DOS resumes. */
    if(native && !active && frontend->native_video && frontend->native_video->pixels)
        frontend->native_geometry_pending=TRUE;
    if(frontend->dos_mouse.width)
        error=frontend_window_mode(frontend->window)==FRONTEND_DISPLAY_WINDOW ?
            frontend_window_poll(frontend->window) : frontend_window_clear(frontend->window);
    if(error)return error;
    /* Consumer-specific mouse coordinates cannot cross worker boundaries.
     * Keep unsent keyboard/focus typeahead in the one frontend queue. */
    for(i=0;i<frontend->dos_input_count;++i) {
        WORD type=frontend->dos_input[i].EventType;
        if(type!=CONSOLE_INPUT_RELATIVE_MOUSE && type!=CONSOLE_INPUT_POINTER && type!=MOUSE_EVENT)
            frontend->dos_input[kept++]=frontend->dos_input[i];
    }
    frontend->dos_input_count=kept;
    if(!kept && !ResetEvent(frontend->dos_input_ready))return GetLastError();
    ZeroMemory(&frontend->dos_mouse,sizeof(frontend->dos_mouse));
    *slot=active ? owner : NULL;
    frontend->title_owner=NULL;frontend->worker_title_valid=FALSE;
    if(!native && frontend->dos_pending==owner)frontend->dos_pending=NULL;
    if(native) {
        frontend->native_video=NULL;frontend->native_video_serial=0;
    } else {
        frontend->dos_video=NULL;frontend->dos_video_serial=0;
        frontend->native_video=NULL;
        frontend->native_video_serial=0;
    }
    signal_binding_waiters(frontend);
    return 0;
}
static DWORD bind_worker(run16_native_frontend *frontend,const void *owner,BOOL active,BOOL native)
{
    HANDLE waits[3];DWORD wait,error;
    if(!frontend || !owner)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&frontend->handoff_lock);
    frontend->handoff_owner=owner;frontend->handoff_active=active;frontend->handoff_native=native;
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
DWORD run16_native_frontend_dos_bind(run16_native_frontend *frontend,const void *owner,BOOL active)
{ return bind_worker(frontend,owner,active,FALSE); }
DWORD run16_native_frontend_wait_dos_ready(run16_native_frontend *frontend,const void *owner,
    HANDLE cancel,DWORD timeout)
{
    ULONGLONG deadline=GetTickCount64()+timeout;
    binding_waiter waiter={0},**link;
    HANDLE waits[3];
    DWORD error=ERROR_SUCCESS;
    if(!frontend || !owner || !cancel || !timeout || timeout>10000)return ERROR_INVALID_PARAMETER;
    waiter.changed=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!waiter.changed)return GetLastError();
    waits[0]=waiter.changed;waits[1]=cancel;waits[2]=frontend->stop;
    EnterCriticalSection(&frontend->io_lock);
    waiter.next=frontend->binding_waiters;
    frontend->binding_waiters=&waiter;
    for(;;) {
        ULONGLONG now;DWORD wait,wait_error;
        if(WaitForSingleObject(frontend->stop,0)!=WAIT_TIMEOUT ||
            WaitForSingleObject(cancel,0)!=WAIT_TIMEOUT){error=ERROR_OPERATION_ABORTED;break;}
        if(!frontend->native_owner && (!frontend->dos_owner || frontend->dos_owner==owner) &&
            (!frontend->dos_pending || frontend->dos_pending==owner))break;
        now=GetTickCount64();
        if(now>=deadline){error=ERROR_TIMEOUT;break;}
        /* Reset under the predicate lock, then wait outside it. A later
         * ownership transition sets this private manual event; cancel/stop
         * have their own persistent handles in the same wait set. */
        if(!ResetEvent(waiter.changed)){error=GetLastError();break;}
        LeaveCriticalSection(&frontend->io_lock);
        wait=WaitForMultipleObjects(3,waits,FALSE,(DWORD)(deadline-now));
        wait_error=wait==WAIT_FAILED ? GetLastError() : ERROR_SUCCESS;
        EnterCriticalSection(&frontend->io_lock);
        if(wait==WAIT_FAILED){error=wait_error;break;}
        if(wait==WAIT_TIMEOUT){error=ERROR_TIMEOUT;break;}
    }
    for(link=&frontend->binding_waiters;*link && *link!=&waiter;link=&(*link)->next){}
    if(*link==&waiter)*link=waiter.next;
    LeaveCriticalSection(&frontend->io_lock);
    CloseHandle(waiter.changed);
    return error;
}
void run16_native_frontend_cancel_dos_pending(run16_native_frontend *frontend,const void *owner)
{
    if(!frontend || !owner)return;
    EnterCriticalSection(&frontend->io_lock);
    if(frontend->dos_pending==owner) {
        frontend->dos_pending=NULL;
        signal_binding_waiters(frontend);
    }
    LeaveCriticalSection(&frontend->io_lock);
}
DWORD run16_native_frontend_native_bind(run16_native_frontend *frontend,const void *owner,BOOL active)
{ return bind_worker(frontend,owner,active,TRUE); }
DWORD run16_native_frontend_dos_enter(run16_native_frontend *frontend,const void *owner)
{
    if(!frontend || !owner)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&frontend->io_lock);
    if((frontend->dos_owner ? frontend->dos_owner : frontend->native_owner)==owner)return ERROR_SUCCESS;
    LeaveCriticalSection(&frontend->io_lock);return ERROR_NOT_READY;
}
DWORD run16_native_frontend_dos_video(run16_native_frontend *frontend,const void *owner,
    const run16_console_video *video)
{
    DWORD error=run16_native_frontend_dos_enter(frontend,owner);
    if(error)return error;
    if(!video->pending && (video->configuration_serial>video->published_serial ||
        (video->pixels && video->description.kind==CONSOLE_VIDEO_TEXT_FRAME))) {
        console_text_configuration copy;
        if(video->configuration_serial>video->published_serial)copy=video->configuration;
        else {
            memcpy(&copy.style,video->pixels,sizeof(copy.style));
            memcpy(copy.palette,video->description.palette,sizeof(copy.palette));
        }
        /* Cursor movement is not a new font/palette configuration. */
        copy.style.cursor_column=copy.style.cursor_row=0;
        copy.style.cursor_start=copy.style.cursor_height=0;
        copy.style.cursor_start1=copy.style.cursor_height1=0;copy.style.cursor_visible=0;
        if(!frontend->text_revision || memcmp(&copy,&frontend->text_configuration,sizeof(copy))) {
            if(frontend->text_revision==UINT32_MAX) {
                run16_native_frontend_dos_leave(frontend);return ERROR_ARITHMETIC_OVERFLOW;
            }
            frontend->text_configuration=copy;++frontend->text_revision;
        }
    }
    if(frontend->dos_owner==owner)frontend->dos_video=video;
    else frontend->native_video=video;
    error=SetEvent(frontend->changed) ? ERROR_SUCCESS : GetLastError();
    run16_native_frontend_dos_leave(frontend);
    return error;
}
void run16_native_frontend_dos_leave(run16_native_frontend *frontend)
{
    LeaveCriticalSection(&frontend->io_lock);
}
DWORD run16_native_frontend_screen_begin(run16_native_frontend *frontend)
{ (void)frontend;return 0; }
DWORD run16_native_frontend_screen_end(run16_native_frontend *frontend,BOOL write)
{ (void)frontend;(void)write;return 0; }
void run16_native_frontend_snapshot_begin(run16_native_frontend *frontend)
{ EnterCriticalSection(&frontend->io_lock); }
void run16_native_frontend_snapshot_end(run16_native_frontend *frontend)
{ LeaveCriticalSection(&frontend->io_lock); }
BOOL run16_native_frontend_text_frame_required(run16_native_frontend *frontend)
{
    /* This request disables the original DOS stream path. Font handoff must
     * not request a display transition while the user remains in Console. */
    return frontend_window_mode(frontend->window)==FRONTEND_DISPLAY_WINDOW;
}
BOOL run16_native_frontend_window_clip_owned(run16_native_frontend *frontend)
{ return frontend->window_active; }
DWORD run16_native_frontend_read_text_configuration(run16_native_frontend *frontend,
    DWORD offset,DWORD revision,console_io_reply *reply)
{
    DWORD total=sizeof(frontend->text_configuration),count;
    if(offset>=total || (!revision && offset))return ERROR_INVALID_PARAMETER;
    if(!frontend->text_revision)return ERROR_NOT_FOUND;
    if(revision && revision!=frontend->text_revision)return ERROR_RETRY;
    count=min(total-offset,CONSOLE_IO_DATA_BYTES);
    memcpy(reply->data,(BYTE *)&frontend->text_configuration+offset,count);
    reply->bytes=count;reply->state.count=total;reply->state.mode=frontend->text_revision;
    return ERROR_SUCCESS;
}
HANDLE run16_native_frontend_dos_ready(run16_native_frontend *frontend)
{
    return frontend->dos_input_ready;
}
DWORD run16_native_frontend_dos_read(run16_native_frontend *frontend,BOOL peek,
    INPUT_RECORD *records,DWORD capacity,DWORD *read)
{
    DWORD count=frontend->dos_input_count;
    *read=0;
    if(!peek && !frontend->dos_owner && frontend->dos_pending)return ERROR_BUSY;
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
    (void)run16_native_frontend_native_bind(frontend,owner,FALSE);
    EnterCriticalSection(&frontend->io_lock);
    if(frontend->dos_pending==owner)frontend->dos_pending=NULL;
    if(frontend->dos_owner==owner) {
        frontend->dos_owner=NULL;frontend->dos_video=NULL;
        frontend->dos_video_serial=0;
        ZeroMemory(&frontend->dos_mouse,sizeof(frontend->dos_mouse));
    }
    if(frontend->native_owner==owner) {
        frontend->native_owner=NULL;frontend->native_video=NULL;frontend->native_video_serial=0;
    }
    SetEvent(frontend->changed);
    signal_binding_waiters(frontend);
    LeaveCriticalSection(&frontend->io_lock);
}
DWORD run16_native_frontend_destroy(run16_native_frontend *frontend)
{
    DWORD error=ERROR_SUCCESS;
    if(!frontend)return ERROR_SUCCESS;
    AcquireSRWLockExclusive(&control_lock);
    if(control_owner==frontend) {
        control_owner=NULL;
        SetConsoleCtrlHandler(frontend_control,FALSE);
    }
    ReleaseSRWLockExclusive(&control_lock);
    if(frontend->stop)SetEvent(frontend->stop);
    /* All resources observe the frontend stop, but closing the native Console must
     * never signal that session-wide event. */
    if(frontend->thread) { WaitForSingleObject(frontend->thread,INFINITE);CloseHandle(frontend->thread); }
    if(frontend->window)return ERROR_BUSY; /* Failed Window join: retain all callback context. */
    /* Reselect the canonical buffer even after a presentation failure. A
     * failed cleanup retains its handles for terminal process cleanup. */
    if(frontend->console_surface) {
        if(!SetConsoleActiveScreenBuffer(frontend->console_output))return GetLastError();
        CloseHandle(frontend->console_surface);
        frontend->console_surface=NULL;
    }
    if(frontend->dos_surface) {CloseHandle(frontend->dos_surface);frontend->dos_surface=NULL;}
    error=restore_cursor_shape(frontend);
    if(error)return error;
    if(frontend->input_mode_saved) {
        if(!SetConsoleMode(frontend->console_input,frontend->original_input_mode))return GetLastError();
        frontend->input_mode_saved=FALSE;
    }
    if(frontend->console_input && frontend->console_input!=INVALID_HANDLE_VALUE)CloseHandle(frontend->console_input);
    if(frontend->console_output && frontend->console_output!=INVALID_HANDLE_VALUE)CloseHandle(frontend->console_output);
    if(frontend->stop)CloseHandle(frontend->stop);
    if(frontend->refresh)CloseHandle(frontend->refresh);
    if(frontend->refreshed)CloseHandle(frontend->refreshed);
    if(frontend->changed)CloseHandle(frontend->changed);
    if(frontend->park)CloseHandle(frontend->park);
    if(frontend->park_done)CloseHandle(frontend->park_done);
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
    return error;
}
