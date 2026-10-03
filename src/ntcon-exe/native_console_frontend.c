#include "native_console_frontend.h"
#include "console_frontend.h"
#include "window_controller.h"
#include "window_keyboard.h"
#include "window_mouse.h"
#include "window_frame.h"
#include "lib/kvm-window/render.h"
#include "opennt-abi/host-compat/include/console_grid.h"
#include <stdint.h>

/* Visible presentation and copied input only. The active identity is a local
 * authenticated channel, never a worker kind, process tree or task record. */
typedef struct binding_waiter {
    struct binding_waiter *next;
    HANDLE changed;
} binding_waiter;
typedef struct pending_binding {
    struct pending_binding *next;
    const void *owner;
} pending_binding;
struct run16_native_frontend {
    CRITICAL_SECTION io_lock,handoff_lock;
    binding_waiter *binding_waiters; /* guarded by io_lock; each waiter owns its event */
    DWORD original_input_mode;
    BOOL input_mode_saved;
    DWORD parked_input_mode;
    BOOL parked;
    HANDLE stop,refresh,refreshed,thread,changed,park,park_done,control[2];
    DWORD park_error;
    HANDLE console_input,console_output,console_surface,logical_surface;
    SMALL_RECT logical_window;
    COORD attempted_canvas;
    /* Root teardown returns the canonical buffer without undoing the DOS
     * worker's final cell grid, viewport, or cursor position. */
    CONSOLE_CURSOR_INFO original_cursor;
    BOOL original_cursor_saved;
    HANDLE handoff,handoff_done;
    const void *handoff_owner;
    BOOL handoff_active,handoff_prepare_vga;
    DWORD handoff_error;
    const void *owner;
    const void *title_owner;
    char worker_title[CONSOLE_IO_TITLE_BYTES];
    BOOL worker_title_valid;
    pending_binding *pending; /* request order, guarded by io_lock */
    const run16_console_video *video;
    uint32_t video_serial;
    console_text_configuration text_configuration;
    uint32_t text_revision;
    BOOL controls_live;
    frontend_window_controller *window;
    frontend_keyboard_delivery keyboard;
    frontend_window_mouse window_mouse;
    kvm_window_frame *window_frame;
    BOOL window_active;
    BOOL console_f_down,console_shortcut;
    INPUT_RECORD *input;
    DWORD input_count,input_capacity;
    HANDLE input_ready;
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
/* Local I/O acquisition requests only. These nodes own no task, process,
 * completion or borrowed frame; channel teardown removes its own request. */
static DWORD append_pending(run16_native_frontend *frontend,const void *owner)
{
    pending_binding **link=&frontend->pending,*entry;
    while(*link) {
        if((*link)->owner==owner)return ERROR_SUCCESS;
        link=&(*link)->next;
    }
    entry=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*entry));
    if(!entry)return ERROR_NOT_ENOUGH_MEMORY;
    entry->owner=owner;*link=entry;
    signal_binding_waiters(frontend);
    return ERROR_SUCCESS;
}
static void remove_pending(run16_native_frontend *frontend,const void *owner)
{
    pending_binding **link=&frontend->pending,*entry;
    while(*link && (*link)->owner!=owner)link=&(*link)->next;
    if(!*link)return;
    entry=*link;*link=entry->next;
    HeapFree(GetProcessHeap(),0,entry);
    signal_binding_waiters(frontend);
}
static DWORD apply_binding(run16_native_frontend *,const void *,BOOL,BOOL);
static DWORD collect_console(run16_native_frontend *);
static DWORD refresh_window_title(run16_native_frontend *frontend)
{
    char title[KVM_WINDOW_TITLE_CAPACITY]={0};
    DWORD length;
    const void *active=frontend->owner;
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
    if(!ok && !error)error=ERROR_INVALID_DATA;
    HeapFree(GetProcessHeap(),0,cells);
    return error;
}
static DWORD clone_grid(HANDLE source,HANDLE *output)
{
    CONSOLE_SCREEN_BUFFER_INFO info,target;
    CONSOLE_CURSOR_INFO cursor;
    HANDLE copy;
    SMALL_RECT fit;
    DWORD error=ERROR_SUCCESS;
    LONG row;
    *output=NULL;
    if(!GetConsoleScreenBufferInfo(source,&info) || !GetConsoleCursorInfo(source,&cursor))
        return GetLastError();
    copy=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
    if(copy==INVALID_HANDLE_VALUE)return GetLastError();
    if(!GetConsoleScreenBufferInfo(copy,&target)){error=GetLastError();goto fail;}
    fit=(SMALL_RECT){0,0,min(info.dwSize.X,target.dwSize.X)-1,
        min(info.dwSize.Y,target.dwSize.Y)-1};
    if(!SetConsoleWindowInfo(copy,TRUE,&fit) || !SetConsoleScreenBufferSize(copy,info.dwSize)) {
        error=GetLastError();goto fail;
    }
    for(row=0;row<info.dwSize.Y;) {
        SHORT rows=(SHORT)min(info.dwSize.Y-row,max(1,4096/info.dwSize.X));
        error=copy_grid(source,copy,(SMALL_RECT){0,(SHORT)row,info.dwSize.X-1,(SHORT)(row+rows-1)},(COORD){0,(SHORT)row});
        if(error)goto fail;
        row+=rows;
    }
    if(!SetConsoleCursorPosition(copy,info.dwCursorPosition) ||
        !SetConsoleCursorInfo(copy,&cursor) || !SetConsoleTextAttribute(copy,info.wAttributes)) {
        error=GetLastError();goto fail;
    }
    *output=copy;return ERROR_SUCCESS;
fail:
    CloseHandle(copy);return error;
}
DWORD run16_native_frontend_clone_text(run16_native_frontend *frontend,HANDLE *output,SMALL_RECT *window)
{
    DWORD error;
    if(!frontend || !output || !window)return ERROR_INVALID_PARAMETER;
    *output=NULL;
    error=clone_grid(frontend->logical_surface,output);
    if(!error)*window=frontend->logical_window;
    return error;
}
DWORD run16_native_frontend_commit_text(run16_native_frontend *frontend,HANDLE surface,SMALL_RECT window)
{
    HANDLE previous=frontend->logical_surface;
    frontend->logical_surface=surface;frontend->logical_window=window;
    CloseHandle(previous);
    return run16_native_frontend_project_text(frontend);
}
/* A VGA text frame is a complete cell publication, not a bitmap. Decode only
 * the worker's byte glyphs into the same Console cell storage used by stream
 * operations. Fonts/palette remain the worker's copied presentation metadata. */
static DWORD import_dos_text(run16_native_frontend *frontend,const run16_console_video *video)
{
    const console_text_style *style=(const console_text_style *)video->pixels;
    const BYTE *text=video->pixels+sizeof(*style);
    COORD size={(SHORT)video->description.width,(SHORT)video->description.height},origin={0,0};
    SMALL_RECT rect={0,0,size.X-1,size.Y-1};
    CONSOLE_SCREEN_BUFFER_INFO info;
    CONSOLE_CURSOR_INFO cursor;
    HANDLE incoming=NULL;
    CHAR_INFO *cells;
    DWORD error,index,count=(DWORD)size.X*size.Y,step=video->description.stride/size.X;
    error=clone_grid(frontend->logical_surface,&incoming);if(error)return error;
    cells=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,count*sizeof(*cells));
    if(!cells){CloseHandle(incoming);return ERROR_NOT_ENOUGH_MEMORY;}
    for(index=0;index<count;++index) {
        char glyph=(char)text[index*step];
        if(!MultiByteToWideChar(GetConsoleOutputCP(),0,&glyph,1,&cells[index].Char.UnicodeChar,1)) {
            error=GetLastError();goto done;
        }
        cells[index].Attributes=text[index*step+1];
    }
    if(!GetConsoleScreenBufferInfo(incoming,&info)){error=GetLastError();goto done;}
    {
        SMALL_RECT fit={0,0,min(size.X,info.dwSize.X)-1,min(size.Y,info.dwSize.Y)-1};
        if(!SetConsoleWindowInfo(incoming,TRUE,&fit) || !SetConsoleScreenBufferSize(incoming,size) ||
            !WriteConsoleOutputW(incoming,cells,size,origin,&rect)) {error=GetLastError();goto done;}
    }
    cursor.dwSize=max(1,min(100,style->cursor_height*100/(int)style->font_height));
    cursor.bVisible=style->cursor_visible && style->cursor_column>=0 && style->cursor_row>=0 &&
        style->cursor_column<size.X && style->cursor_row<size.Y;
    if(!SetConsoleCursorInfo(incoming,&cursor) || (cursor.bVisible &&
        !SetConsoleCursorPosition(incoming,(COORD){(SHORT)style->cursor_column,(SHORT)style->cursor_row}))) {
        error=GetLastError();goto done;
    }
    error=run16_native_frontend_commit_text(frontend,incoming,(SMALL_RECT){0,0,size.X-1,size.Y-1});
    incoming=NULL;
done:
    HeapFree(GetProcessHeap(),0,cells);if(incoming)CloseHandle(incoming);return error;
}
static DWORD prepare_logical_surface(run16_native_frontend *frontend)
{
    CONSOLE_SCREEN_BUFFER_INFO visible;
    SMALL_RECT shrink,window;
    COORD cursor,viewport_size;
    DWORD error;
    HANDLE incoming=NULL;
    if(!GetConsoleScreenBufferInfo(frontend->logical_surface,&visible))return GetLastError();
    viewport_size=(COORD){frontend->logical_window.Right-frontend->logical_window.Left+1,
        frontend->logical_window.Bottom-frontend->logical_window.Top+1};
    /* Capability conversion operates on the current logical viewport, never
     * the host canvas. Copy before shrinking so offset viewports survive. */
    error=clone_grid(frontend->logical_surface,&incoming);if(error)return error;
    error=copy_grid(frontend->logical_surface,incoming,frontend->logical_window,(COORD){0,0});
    if(error)goto fail;
    cursor=(COORD){visible.dwCursorPosition.X-frontend->logical_window.Left,
        visible.dwCursorPosition.Y-frontend->logical_window.Top};
    if(cursor.X<0 || cursor.Y<0 || cursor.X>=viewport_size.X ||
        cursor.Y>=viewport_size.Y)cursor=(COORD){0,0};
    if(!SetConsoleCursorPosition(incoming,cursor)){error=GetLastError();goto fail;}
    shrink=(SMALL_RECT){0,0,min(visible.dwSize.X,viewport_size.X)-1,
        min(visible.dwSize.Y,viewport_size.Y)-1};
    /* Window changes can resize/reflow even an inactive ConPTY buffer.
     * Preserve the copied cells at both calls, before VGA conversion. */
    if(!opennt_console_resize_grid(incoming,NULL,TRUE,&shrink) ||
        !opennt_console_resize_grid(incoming,&viewport_size,FALSE,NULL)){error=GetLastError();goto fail;}
    window=(SMALL_RECT){0,0,viewport_size.X-1,viewport_size.Y-1};
    error=run16_console_prepare_dos(incoming,&window);
    if(error)goto fail;
    return run16_native_frontend_commit_text(frontend,incoming,window);
fail:
    CloseHandle(incoming);return error;
}
DWORD run16_native_frontend_project_text(run16_native_frontend *frontend)
{
    CONSOLE_SCREEN_BUFFER_INFO logical,visible;
    CONSOLE_CURSOR_INFO shape,visible_shape;
    DWORD count,error;
    COORD cursor;
    SHORT width,height,canvas_width,canvas_height;
    SMALL_RECT source;
    if(!frontend || !frontend->logical_surface)return ERROR_INVALID_STATE;
    if(!GetConsoleScreenBufferInfo(frontend->logical_surface,&logical) ||
        !GetConsoleScreenBufferInfo(frontend->console_output,&visible) ||
        !GetConsoleCursorInfo(frontend->logical_surface,&shape) ||
        !GetConsoleCursorInfo(frontend->console_output,&visible_shape))return GetLastError();
    {
        COORD requested={frontend->logical_window.Right-frontend->logical_window.Left+1,
            frontend->logical_window.Bottom-frontend->logical_window.Top+1};
        if(frontend->attempted_canvas.X!=requested.X || frontend->attempted_canvas.Y!=requested.Y) {
            SMALL_RECT fit=visible.srWindow;
            frontend->attempted_canvas=requested;
            /* A refused/unapplied host resize is not a logical resize. Bound
             * the request to representable host storage and query its result. */
            /* Do not shrink the host canvas: an embedded terminal can retain
             * its outer rows despite that API succeeding. Unused rows belong
             * to projection clearing, not a fabricated smaller host page. */
            fit.Right=fit.Left+max(visible.srWindow.Right-visible.srWindow.Left+1,
                min(requested.X,min(visible.dwMaximumWindowSize.X,visible.dwSize.X-fit.Left)))-1;
            fit.Bottom=fit.Top+max(visible.srWindow.Bottom-visible.srWindow.Top+1,
                min(requested.Y,min(visible.dwMaximumWindowSize.Y,visible.dwSize.Y-fit.Top)))-1;
            if(memcmp(&fit,&visible.srWindow,sizeof(fit)))
                (void)SetConsoleWindowInfo(frontend->console_output,TRUE,&fit);
            if(!GetConsoleScreenBufferInfo(frontend->console_output,&visible))return GetLastError();
        }
    }
    canvas_width=visible.srWindow.Right-visible.srWindow.Left+1;
    canvas_height=visible.srWindow.Bottom-visible.srWindow.Top+1;
    width=min(canvas_width,frontend->logical_window.Right-frontend->logical_window.Left+1);
    height=min(canvas_height,frontend->logical_window.Bottom-frontend->logical_window.Top+1);
    if(width<=0 || height<=0)return ERROR_INVALID_DATA;
    source=(SMALL_RECT){frontend->logical_window.Left,frontend->logical_window.Top,
        frontend->logical_window.Left+width-1,frontend->logical_window.Top+height-1};
    /* Presentation never resizes storage to force a cursor into view. The
     * queried physical canvas only clips this copy; logical data survives. */
    error=copy_grid(frontend->logical_surface,frontend->console_output,
        source,
        (COORD){visible.srWindow.Left,visible.srWindow.Top});
    if(error)return error;
    if(canvas_width>width || canvas_height>height) {
        SHORT row;
        for(row=visible.srWindow.Top;row<=visible.srWindow.Bottom;++row) {
            COORD start;
            DWORD excess;
            if(row<visible.srWindow.Top+height) {
                start=(COORD){visible.srWindow.Left+width,row};
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
    cursor=(COORD){logical.dwCursorPosition.X-frontend->logical_window.Left,
        logical.dwCursorPosition.Y-frontend->logical_window.Top};
    if(cursor.X<0 || cursor.Y<0 || cursor.X>=width || cursor.Y>=height)shape.bVisible=FALSE;
    else {
        cursor.X+=visible.srWindow.Left;cursor.Y+=visible.srWindow.Top;
        if((cursor.X!=visible.dwCursorPosition.X || cursor.Y!=visible.dwCursorPosition.Y) &&
            !SetConsoleCursorPosition(frontend->console_output,cursor))return GetLastError();
    }
    if((shape.dwSize!=visible_shape.dwSize || shape.bVisible!=visible_shape.bVisible) &&
        !SetConsoleCursorInfo(frontend->console_output,&shape))return GetLastError();
    return ERROR_SUCCESS;
}
/* Copied frontend transport records, serialized by io_lock. Windows still
 * owns Console processing and the guest owns its keyboard device. Grow before
 * mutation; no truncation or partial prepend on allocation failure. */
static DWORD input_write(run16_native_frontend *frontend,
    const INPUT_RECORD *records,DWORD count,BOOL prepend)
{
    DWORD total;SIZE_T capacity;INPUT_RECORD *grown;
    if(count>MAXDWORD-frontend->input_count)return ERROR_ARITHMETIC_OVERFLOW;
    total=frontend->input_count+count;
    if(total>frontend->input_capacity) {
        capacity=(SIZE_T)total;
        if(capacity>((SIZE_T)-1)/sizeof(*grown)-64)return ERROR_NOT_ENOUGH_MEMORY;
        capacity+=64;
        grown=frontend->input ? HeapReAlloc(GetProcessHeap(),0,frontend->input,capacity*sizeof(*grown)) :
            HeapAlloc(GetProcessHeap(),0,capacity*sizeof(*grown));
        if(!grown)return ERROR_NOT_ENOUGH_MEMORY;
        frontend->input=grown;frontend->input_capacity=(DWORD)capacity;
    }
    if(count) {
        if(prepend)memmove(frontend->input+count,frontend->input,
            frontend->input_count*sizeof(*records));
        memcpy(frontend->input+(prepend ? 0 : frontend->input_count),records,count*sizeof(*records));
        frontend->input_count=total;
    }
    return !total || SetEvent(frontend->input_ready) ? ERROR_SUCCESS : GetLastError();
}
static DWORD window_records(void *context,const INPUT_RECORD *records,DWORD count)
{
    run16_native_frontend *frontend=context;
    /* Both workers consume this copied queue through the same channel. */
    return input_write(frontend,records,count,FALSE);
}
static lib_bool window_input(void *context,const frontend_window_input *input)
{
    run16_native_frontend *frontend=context;
    DWORD error;
    error=frontend_window_mouse_dispatch(&frontend->window_mouse,input,window_records,frontend);
    if(error)return FALSE;
    return frontend_keyboard_dispatch(&frontend->keyboard,input,
        window_records,frontend)==ERROR_SUCCESS;
}
static DWORD window_route(void *context,BOOL window,BOOL graphics)
{
    run16_native_frontend *frontend=context;
    (void)graphics;
    if(!window) {
        DWORD error=frontend_window_mouse_leave(&frontend->window_mouse,window_records,frontend);
        if(error)return error;
    }
    /* Drain records with their old source policy before changing visibility.
     * Guest prepend never uses this physical Console queue. */
    if(frontend->owner && frontend->window_active!=window) {
        DWORD error=collect_console(frontend);if(error)return error;
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
    if(window && frontend->window_mouse.width)
        return frontend_window_mouse_enter(&frontend->window_mouse,window_records,frontend);
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
    if(record->EventType==WINDOW_BUFFER_SIZE_EVENT) {
        /* A visible-canvas event is not a resize of either worker's execution
         * Console. Reproject committed state; do not forward host geometry. */
        *keep=FALSE;
        return run16_native_frontend_project_text(frontend);
    }
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
static DWORD collect_console(run16_native_frontend *frontend)
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
            if(records[i].EventType==MOUSE_EVENT) {
                CONSOLE_SCREEN_BUFFER_INFO canvas;
                COORD *point=&records[i].Event.MouseEvent.dwMousePosition;
                SHORT width=frontend->logical_window.Right-frontend->logical_window.Left+1;
                SHORT height=frontend->logical_window.Bottom-frontend->logical_window.Top+1;
                if(!GetConsoleScreenBufferInfo(frontend->console_output,&canvas))return GetLastError();
                point->X-=canvas.srWindow.Left;point->Y-=canvas.srWindow.Top;
                if(point->X<0 || point->Y<0 || point->X>=width || point->Y>=height ||
                    point->X>canvas.srWindow.Right-canvas.srWindow.Left ||
                    point->Y>canvas.srWindow.Bottom-canvas.srWindow.Top)continue;
                point->X+=frontend->logical_window.Left;point->Y+=frontend->logical_window.Top;
            }
            error=console_input(frontend,&records[i],&keep);if(error)return error;
            if(keep)records[kept++]=records[i];
        }
        error=input_write(frontend,records,kept,FALSE);if(error)return error;
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
                frontend->handoff_active,frontend->handoff_prepare_vga);
            SetEvent(frontend->handoff_done);
        }
        if(WaitForSingleObject(frontend->park,0)==WAIT_OBJECT_0) {
            ResetEvent(frontend->park);
            frontend->park_error=(frontend->owner || frontend->pending) ? ERROR_BUSY :
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
        if(frontend->owner || frontend->window_active) {
            error=collect_console(frontend);
            waits[count++]=frontend->console_input;
        }
        if(!error && frontend->window_mouse.width)
            error=frontend_window_poll(frontend->window);
        if(!error) {
            LONG display=InterlockedExchange(&frontend->display_request,0);
            if(display)error=frontend_window_select(frontend->window,
                display==2 ? FRONTEND_DISPLAY_WINDOW : FRONTEND_DISPLAY_CONSOLE);
        }
        video=frontend->video;
        published=&frontend->video_serial;
        if(!error && video) {
            uint32_t serial=video->published_serial;
            if(serial!=*published && (!video->pending || video->pixels)) {
                if(video->pixels) {
                    lib_u32 width=0,height=0;
                    error=frontend_window_dos_frame(video,frontend->window_frame);
                    if(!error && !kvm_window_frame_size(frontend->window_frame,&width,&height))
                        error=ERROR_INVALID_DATA;
                    if(!error)
                        error=frontend_window_mouse_geometry(&frontend->window_mouse,width,height);
                    if(!error)error=frontend_window_present(frontend->window,frontend->window_frame,
                        video->description.kind==CONSOLE_VIDEO_DIB);
                    if(!error && frontend->window_active)
                        error=frontend_window_mouse_enter(&frontend->window_mouse,window_records,frontend);
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
        frontend->attempted_canvas=(COORD){info.srWindow.Right-info.srWindow.Left+1,
            info.srWindow.Bottom-info.srWindow.Top+1};
        if(!GetConsoleCursorInfo(frontend->console_output,&frontend->original_cursor)) {
            error=GetLastError();run16_native_frontend_destroy(frontend);return error;
        }
        frontend->original_cursor_saved=TRUE;
        error=clone_grid(frontend->console_output,&frontend->logical_surface);
        if(error){run16_native_frontend_destroy(frontend);return error;}
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
    frontend->input_ready=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!frontend->stop || !frontend->refresh || !frontend->refreshed || !frontend->changed ||
        !frontend->park || !frontend->park_done ||
        !frontend->control[0] || !frontend->control[1] || !frontend->handoff || !frontend->handoff_done || !frontend->input_ready) {
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
DWORD run16_native_frontend_logical_console(run16_native_frontend *frontend,HANDLE *output)
{
    if(!frontend || !output || !frontend->logical_surface)return ERROR_INVALID_PARAMETER;
    *output=NULL;
    return DuplicateHandle(GetCurrentProcess(),frontend->logical_surface,GetCurrentProcess(),
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
        frontend->owner!=owner)return;
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
static DWORD apply_binding(run16_native_frontend *frontend,const void *owner,BOOL active,BOOL prepare_vga)
{
    DWORD error=0,i,kept=0;
    /* Every incoming channel waits for the previous owner's final publication
     * and release. Preserve request order across wakeups, without classifying
     * workers or deciding which tasks execute. One pending node per channel. */
    if(active && frontend->owner==owner)return ERROR_SUCCESS;
    if(active && (frontend->owner ||
        (frontend->pending && frontend->pending->owner!=owner))) {
        error=append_pending(frontend,owner);
        return error ? error : ERROR_BUSY;
    }
    if(!active && frontend->owner && frontend->owner!=owner)return ERROR_BUSY;
    if(!active && !frontend->owner)return ERROR_SUCCESS;
    /* A borrowed root restores the outer CMD's cooked mode while idle. Its
     * resident worker resumes the mode it had before that temporary return. */
    if(active && frontend->parked) {
        HANDLE incoming=NULL;
        CONSOLE_SCREEN_BUFFER_INFO current;
        /* The outer shell legitimately owned the canonical page while parked.
         * Import that new page once at reacquisition, never during handoff. */
        error=GetConsoleScreenBufferInfo(frontend->console_output,&current) ?
            clone_grid(frontend->console_output,&incoming) : GetLastError();
        if(error)return error;
        CloseHandle(frontend->logical_surface);frontend->logical_surface=incoming;
        frontend->logical_window=current.srWindow;
        if(!SetConsoleMode(frontend->console_input,frontend->parked_input_mode))
            return GetLastError();
        frontend->parked=FALSE;
    }
    if(prepare_vga && active) {
        error=prepare_logical_surface(frontend);
        if(error)return error;
    }
    if(frontend->window_mouse.width)
        error=frontend_window_mode(frontend->window)==FRONTEND_DISPLAY_WINDOW ?
            frontend_window_poll(frontend->window) : frontend_window_clear(frontend->window);
    if(error)return error;
    /* Consumer-specific mouse coordinates cannot cross worker boundaries.
     * Keep unsent keyboard/focus typeahead in the one frontend queue. */
    for(i=0;i<frontend->input_count;++i) {
        WORD type=frontend->input[i].EventType;
        if(type!=CONSOLE_INPUT_FRAME_MOUSE && type!=MOUSE_EVENT)
            frontend->input[kept++]=frontend->input[i];
    }
    frontend->input_count=kept;
    if(!kept && !ResetEvent(frontend->input_ready))return GetLastError();
    ZeroMemory(&frontend->window_mouse,sizeof(frontend->window_mouse));
    frontend->owner=active ? owner : NULL;
    frontend->title_owner=NULL;frontend->worker_title_valid=FALSE;
    remove_pending(frontend,owner);
    frontend->video=NULL;frontend->video_serial=0;
    signal_binding_waiters(frontend);
    return 0;
}
DWORD run16_native_frontend_bind(run16_native_frontend *frontend,const void *owner,BOOL active,BOOL prepare_vga)
{
    HANDLE waits[3];DWORD wait,error;
    if(!frontend || !owner)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&frontend->handoff_lock);
    frontend->handoff_owner=owner;frontend->handoff_active=active;frontend->handoff_prepare_vga=prepare_vga;
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
DWORD run16_native_frontend_wait_ready(run16_native_frontend *frontend,const void *owner,
    HANDLE cancel,HANDLE peer,DWORD timeout)
{
    ULONGLONG deadline=GetTickCount64()+timeout;
    binding_waiter waiter={0},**link;
    HANDLE waits[5];
    DWORD error=ERROR_SUCCESS;
    if(!frontend || !owner || !cancel || !peer || !timeout || timeout>10000)return ERROR_INVALID_PARAMETER;
    waiter.changed=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!waiter.changed)return GetLastError();
    waits[0]=frontend->stop;waits[1]=cancel;waits[2]=peer;
    waits[3]=frontend->thread;waits[4]=waiter.changed;
    EnterCriticalSection(&frontend->io_lock);
    waiter.next=frontend->binding_waiters;
    frontend->binding_waiters=&waiter;
    for(;;) {
        ULONGLONG now;DWORD wait,wait_error;
        if(WaitForSingleObject(frontend->stop,0)!=WAIT_TIMEOUT ||
            WaitForSingleObject(cancel,0)!=WAIT_TIMEOUT){error=ERROR_OPERATION_ABORTED;break;}
        if(WaitForSingleObject(peer,0)!=WAIT_TIMEOUT){error=ERROR_BROKEN_PIPE;break;}
        if(WaitForSingleObject(frontend->thread,0)!=WAIT_TIMEOUT) {
            if(!GetExitCodeThread(frontend->thread,&error))error=GetLastError();
            else if(!error)error=ERROR_BROKEN_PIPE;
            break;
        }
        if(frontend->owner==owner || (!frontend->owner &&
            (!frontend->pending || frontend->pending->owner==owner)))break;
        now=GetTickCount64();
        if(now>=deadline){error=ERROR_TIMEOUT;break;}
        /* Reset under the predicate lock, then wait outside it. A later
         * ownership transition sets this private manual event; cancel/stop
         * have their own persistent handles in the same wait set. */
        if(!ResetEvent(waiter.changed)){error=GetLastError();break;}
        LeaveCriticalSection(&frontend->io_lock);
        wait=WaitForMultipleObjects(5,waits,FALSE,(DWORD)(deadline-now));
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
void run16_native_frontend_cancel_pending(run16_native_frontend *frontend,const void *owner)
{
    if(!frontend || !owner)return;
    EnterCriticalSection(&frontend->io_lock);
    remove_pending(frontend,owner);
    LeaveCriticalSection(&frontend->io_lock);
}
DWORD run16_native_frontend_enter(run16_native_frontend *frontend,const void *owner)
{
    if(!frontend || !owner)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&frontend->io_lock);
    if(frontend->owner==owner)return ERROR_SUCCESS;
    LeaveCriticalSection(&frontend->io_lock);return ERROR_NOT_READY;
}
DWORD run16_native_frontend_video(run16_native_frontend *frontend,const void *owner,
    const run16_console_video *video,BOOL import_text)
{
    DWORD error=run16_native_frontend_enter(frontend,owner);
    if(error)return error;
    if(import_text && !video->pending && video->pixels &&
        video->description.kind==CONSOLE_VIDEO_TEXT_FRAME) {
        error=import_dos_text(frontend,video);
        if(error){run16_native_frontend_leave(frontend);return error;}
    }
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
                run16_native_frontend_leave(frontend);return ERROR_ARITHMETIC_OVERFLOW;
            }
            frontend->text_configuration=copy;++frontend->text_revision;
        }
    }
    frontend->video=video;
    error=SetEvent(frontend->changed) ? ERROR_SUCCESS : GetLastError();
    run16_native_frontend_leave(frontend);
    return error;
}
void run16_native_frontend_leave(run16_native_frontend *frontend)
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
HANDLE run16_native_frontend_ready(run16_native_frontend *frontend)
{
    return frontend->input_ready;
}
DWORD run16_native_frontend_read(run16_native_frontend *frontend,BOOL peek,
    INPUT_RECORD *records,DWORD capacity,DWORD *read)
{
    DWORD count=frontend->input_count;
    *read=0;
    if(!peek && frontend->pending)return ERROR_BUSY;
    if(count>capacity)count=capacity;
    if(count)memcpy(records,frontend->input,count*sizeof(*records));
    if(!peek && count) {
        frontend->input_count-=count;
        memmove(frontend->input,frontend->input+count,frontend->input_count*sizeof(*records));
        if(!frontend->input_count && !ResetEvent(frontend->input_ready))return GetLastError();
    }
    *read=count;return ERROR_SUCCESS;
}
DWORD run16_native_frontend_prepend(run16_native_frontend *frontend,const INPUT_RECORD *records,DWORD count)
{
    return input_write(frontend,records,count,TRUE);
}
void run16_native_frontend_forget(run16_native_frontend *frontend,const void *owner)
{
    if(!frontend)return;
    /* Retire Window input against its current owner before removing the source.
     * A stopped/failed presentation thread may refuse the handoff; the lock
     * below still detaches its borrowed channel storage before disposal. */
    (void)run16_native_frontend_bind(frontend,owner,FALSE,FALSE);
    EnterCriticalSection(&frontend->io_lock);
    remove_pending(frontend,owner);
    if(frontend->owner==owner) {
        frontend->owner=NULL;frontend->video=NULL;
        frontend->video_serial=0;
        ZeroMemory(&frontend->window_mouse,sizeof(frontend->window_mouse));
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
    while(frontend->pending)remove_pending(frontend,frontend->pending->owner);
    /* Reselect the canonical buffer even after a presentation failure. A
     * failed cleanup retains its handles for terminal process cleanup. */
    if(frontend->console_surface) {
        if(!SetConsoleActiveScreenBuffer(frontend->console_output))return GetLastError();
        CloseHandle(frontend->console_surface);
        frontend->console_surface=NULL;
    }
    if(frontend->logical_surface) {CloseHandle(frontend->logical_surface);frontend->logical_surface=NULL;}
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
    if(frontend->input_ready)CloseHandle(frontend->input_ready);
    if(frontend->input)HeapFree(GetProcessHeap(),0,frontend->input);
    DeleteCriticalSection(&frontend->handoff_lock);
    DeleteCriticalSection(&frontend->io_lock);
    HeapFree(GetProcessHeap(),0,frontend);
    return error;
}
