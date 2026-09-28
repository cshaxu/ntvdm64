#include "native_console_view.h"
#include "native_terminal.h"
#include <string.h>

static DWORD visible_geometry(HANDLE output,run16_native_geometry *geometry)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    ZeroMemory(geometry,sizeof(*geometry));geometry->font.cbSize=sizeof(geometry->font);
    if(!GetConsoleScreenBufferInfo(output,&info) ||
        !GetCurrentConsoleFontEx(output,FALSE,&geometry->font))return GetLastError();
    geometry->size=info.dwSize;geometry->window=info.srWindow;
    return ERROR_SUCCESS;
}

void run16_native_view_end(run16_native_console_view *view)
{
    if(!view)return;
    if(view->input && view->input!=INVALID_HANDLE_VALUE) {
        if(view->mode_saved)SetConsoleMode(view->input,view->input_mode);
        CloseHandle(view->input);
    }
    if(view->output && view->output!=INVALID_HANDLE_VALUE)CloseHandle(view->output);
    ZeroMemory(view,sizeof(*view));
}

DWORD run16_native_view_begin(run16_native_backend *backend,run16_native_console_view *view)
{
    HANDLE input,output;
    DWORD error;
    if(!backend || !view)return ERROR_INVALID_PARAMETER;
    ZeroMemory(view,sizeof(*view));
    input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    if(input==INVALID_HANDLE_VALUE)return GetLastError();
    output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    error=output==INVALID_HANDLE_VALUE ? GetLastError() :
        run16_native_view_begin_on(backend,view,input,output);
    if(output!=INVALID_HANDLE_VALUE)CloseHandle(output);
    CloseHandle(input);
    return error;
}

DWORD run16_native_view_begin_on(run16_native_backend *backend,
    run16_native_console_view *view,HANDLE input,HANDLE output)
{
    DWORD error;
    if(!backend || !view)return ERROR_INVALID_PARAMETER;
    ZeroMemory(view,sizeof(*view));
    if(!input || input==INVALID_HANDLE_VALUE || !output || output==INVALID_HANDLE_VALUE)
        return ERROR_INVALID_HANDLE;
    if(!DuplicateHandle(GetCurrentProcess(),input,GetCurrentProcess(),&view->input,0,FALSE,DUPLICATE_SAME_ACCESS) ||
        !DuplicateHandle(GetCurrentProcess(),output,GetCurrentProcess(),&view->output,0,FALSE,DUPLICATE_SAME_ACCESS) ||
        !GetConsoleMode(view->input,&view->input_mode))error=GetLastError();
    else error=run16_native_view_seed(backend,view);
    if(error)run16_native_view_end(view);
    return error;
}

static DWORD configure(run16_native_backend *backend,const run16_native_geometry *geometry)
{
    COORD size={geometry->size.X,(SHORT)(geometry->window.Bottom-geometry->window.Top+1)};
    unsigned history=geometry->size.Y>size.Y ? geometry->size.Y-size.Y : 0;
    return run16_native_backend_configure(backend,size,history);
}

static DWORD import_console(run16_native_backend *backend,run16_native_console_view *view,BOOL update)
{
    CONSOLE_SCREEN_BUFFER_INFOEX info={sizeof(info)};
    CHAR_INFO *cells=NULL;DWORD error,members=0;int rows,top,row;
    COORD size,origin={0,0};SMALL_RECT rectangle;
    if(!backend || !view)return ERROR_INVALID_PARAMETER;
    error=update ? 0 : run16_native_backend_members(backend,&members);
    if(error)return error;
    if(members && !update) {view->retired=FALSE;return 0;}
    if(!GetConsoleScreenBufferInfoEx(view->output,&info))return GetLastError();
    rows=info.srWindow.Bottom-info.srWindow.Top+1;
    top=min(info.srWindow.Top,info.dwCursorPosition.Y);
    top=max(top,info.dwCursorPosition.Y-rows+1);
    size.X=info.dwSize.X;size.Y=(SHORT)rows;
    error=update ? 0 : run16_native_backend_configure(backend,size,info.dwSize.Y-rows);
    if(error)return error;
    cells=HeapAlloc(GetProcessHeap(),0,(SIZE_T)(top+rows)*size.X*sizeof(*cells));
    if(!cells)return ERROR_NOT_ENOUGH_MEMORY;
    /* Read bounded rows: ReadConsoleOutput has transfer-size limits. Keep
     * scrollback and the cursor's active viewport, not the unused tail. */
    size.Y=1;
    for(row=0;row<top+rows;++row) {
        rectangle.Left=0;rectangle.Top=(SHORT)row;
        rectangle.Right=size.X-1;rectangle.Bottom=(SHORT)row;
        if(!ReadConsoleOutputW(view->output,cells+(SIZE_T)row*size.X,size,origin,&rectangle)) {
            error=GetLastError();break;
        }
        if(rectangle.Left!=0 || rectangle.Right!=size.X-1 || rectangle.Top!=row || rectangle.Bottom!=row) {
            error=ERROR_RETRY;break;
        }
    }
    if(!error)error=run16_native_backend_seed(backend,&info,cells,(unsigned)top,&view->revision);
    if(!error)view->retired=update; /* These cells already exist in Console. */
    HeapFree(GetProcessHeap(),0,cells);return error;
}

DWORD run16_native_view_prepare_launch(run16_native_backend *backend,run16_native_console_view *view)
{
    return import_console(backend,view,FALSE);
}

DWORD run16_native_view_import_console(run16_native_backend *backend,run16_native_console_view *view)
{
    return import_console(backend,view,TRUE);
}

DWORD run16_native_view_seed(run16_native_backend *backend,run16_native_console_view *view)
{
    DWORD error,mode;
    if(!backend || !view)return ERROR_INVALID_PARAMETER;
    error=visible_geometry(view->output,&view->geometry);
    if(!error)error=configure(backend,&view->geometry);
    if(error)return error;
    /* Geometry belongs to this view; remote native screen state belongs to
     * ConPTY. Never pretend that painting a local parser seeds its Windows
     * Console buffer or overwrite target output during DOS/native handoff. */
    mode=(view->input_mode|ENABLE_EXTENDED_FLAGS|ENABLE_WINDOW_INPUT|ENABLE_MOUSE_INPUT)&
        ~(ENABLE_LINE_INPUT|ENABLE_ECHO_INPUT|ENABLE_PROCESSED_INPUT|ENABLE_QUICK_EDIT_MODE|ENABLE_VIRTUAL_TERMINAL_INPUT);
    if(!SetConsoleMode(view->input,mode))return GetLastError();
    view->mode_saved=TRUE;view->geometry_saved=TRUE;
    return 0;
}

DWORD run16_native_view_prepare_screen(run16_native_backend *backend,run16_native_console_view *view)
{
    run16_native_geometry geometry;DWORD error=visible_geometry(view->output,&geometry);
    if(!error)error=configure(backend,&geometry);
    if(!error)error=run16_native_backend_pump(backend);
    return error;
}

static DWORD present_snapshot(run16_native_backend *backend,run16_native_console_view *view,BOOL window,BOOL locked)
{
    ntkvm_terminal_frame snapshot={0};
    run16_native_frame_info frame={0};
    run16_native_geometry before,after;
    CHAR_INFO *cells=NULL;DWORD error,total,i;
    if(!backend || !view)return ERROR_INVALID_PARAMETER;
    error=visible_geometry(view->output,&before);
    if(error)return error;
    if(!locked && (!view->geometry_saved || memcmp(&before,&view->geometry,sizeof(before)))) {
        error=configure(backend,&before);
        if(error)return error;
    }
    error=locked ? 0 : run16_native_backend_pump(backend);
    if(!error)error=run16_native_backend_capture(backend,&snapshot);
    if(error==ERROR_NO_DATA)return 0; /* Backend is lazy until first launch. */
    if(error)return error;
    /* DOS already received this native frame. A retained ConPTY is not
     * permission to repaint old output over the resumed DOS prompt. */
    if(view->retired && snapshot.revision==view->revision)goto done;
    frame.screen.cbSize=sizeof(frame.screen);
    if(!GetConsoleScreenBufferInfoEx(view->output,&frame.screen)) {error=GetLastError();goto done;}
    if(snapshot.rows<=0 || snapshot.rows>32767 || snapshot.columns<=0 || snapshot.columns>32767) {
        error=ERROR_INVALID_DATA;goto done;
    }
    frame.screen.dwSize.X=(SHORT)snapshot.columns;
    /* Retain the user's scrollback capacity even when the terminal has not
     * produced that many lines yet; a first paint must not collapse it. */
    frame.screen.dwSize.Y=(SHORT)max(snapshot.rows,before.size.Y);
    frame.screen.dwCursorPosition.X=(SHORT)snapshot.cursor.col;
    frame.screen.dwCursorPosition.Y=(SHORT)snapshot.cursor.row;
    frame.screen.srWindow.Left=0;frame.screen.srWindow.Top=(SHORT)snapshot.history_rows;
    frame.screen.srWindow.Right=(SHORT)(min(snapshot.columns,before.window.Right-before.window.Left+1)-1);
    frame.screen.srWindow.Bottom=(SHORT)(snapshot.rows-1);
    frame.cursor.bVisible=snapshot.cursor_visible;frame.cursor.dwSize=25;
    total=(DWORD)frame.screen.dwSize.Y*snapshot.columns;
    if((uint64_t)total*sizeof(*cells)>(SIZE_T)-1) {error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
    cells=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,(SIZE_T)total*sizeof(*cells));
    if(!cells) {error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
    for(i=0;i<total;++i) {
        cells[i].Char.UnicodeChar=L' ';cells[i].Attributes=frame.screen.wAttributes;
    }
    for(i=0;i<(DWORD)snapshot.rows*snapshot.columns;++i) {
        const VTermScreenCell *cell=snapshot.cells+i;
        uint32_t ch=cell->chars[0];
        WORD foreground=ntkvm_terminal_console_colour(&cell->fg,frame.screen.ColorTable);
        WORD background=ntkvm_terminal_console_colour(&cell->bg,frame.screen.ColorTable);
        if(cell->attrs.reverse) {WORD swap=foreground;foreground=background;background=swap;}
        cells[i].Attributes=foreground|(background<<4);
        if(cell->attrs.underline)cells[i].Attributes|=COMMON_LVB_UNDERSCORE;
        cells[i].Char.UnicodeChar=ch && ch<=0xffff ? (WCHAR)ch : ch ? L'?' : L' ';
        if(cell->width==2 && i+1<total) {
            cells[i].Attributes|=COMMON_LVB_LEADING_BYTE;
            cells[i+1].Attributes=cells[i].Attributes&~COMMON_LVB_LEADING_BYTE;
            cells[i+1].Attributes|=COMMON_LVB_TRAILING_BYTE;
            if(ch>0xffff && ch<=0x10ffff) {
                ch-=0x10000;cells[i].Char.UnicodeChar=(WCHAR)(0xd800+(ch>>10));
                cells[i+1].Char.UnicodeChar=(WCHAR)(0xdc00+(ch&0x3ff));
            } else cells[i+1].Char.UnicodeChar=cells[i].Char.UnicodeChar;
            ++i;
        }
    }
    error=visible_geometry(view->output,&after);
    if(!error && memcmp(&before,&after,sizeof(before)))error=ERROR_RETRY;
    if(!error && window && view->window_frame)
        error=view->window_frame(view->window_context,&frame,cells,total);
    else if(!error) {
        error=run16_native_screen_apply(view->output,&frame.screen,&frame.cursor);
        if(!error)error=run16_native_cells_write(view->output,0,cells,total);
    }
    if(!error) {
        view->revision=snapshot.revision;view->retired=FALSE;
        view->geometry=before;
        if(!window || !view->window_frame) {
            view->geometry.size=frame.screen.dwSize;view->geometry.window=frame.screen.srWindow;
        }
        view->geometry_saved=TRUE;
    }
done:
    if(cells)HeapFree(GetProcessHeap(),0,cells);
    ntkvm_terminal_frame_free(&snapshot);
    return error;
}

DWORD run16_native_view_present(run16_native_backend *backend,run16_native_console_view *view)
{
    return present_snapshot(backend,view,TRUE,FALSE);
}

DWORD run16_native_view_sync_console(run16_native_backend *backend,run16_native_console_view *view)
{
    DWORD error=present_snapshot(backend,view,FALSE,FALSE);
    if(!error)view->retired=TRUE;
    return error;
}

DWORD run16_native_view_sync_locked(run16_native_backend *backend,run16_native_console_view *view)
{
    DWORD error=present_snapshot(backend,view,FALSE,TRUE);
    if(!error)view->retired=TRUE;
    return error;
}

DWORD run16_native_view_forward_input(run16_native_backend *backend,run16_native_console_view *view)
{
    INPUT_RECORD records[64];
    DWORD available=0,count=0,i,kept=0,consumed=0,error;
    if(!PeekConsoleInputW(view->input,records,ARRAYSIZE(records),&available))return GetLastError();
    if(!available)return ERROR_SUCCESS;
    if(!ReadConsoleInputW(view->input,records,available,&count))return GetLastError();
    /* The geometry exchange invokes the real native resize, which generates
     * its own mode-dependent notification. Do not inject a duplicate/stale
     * visible-buffer WINDOW_BUFFER_SIZE_EVENT into the ConPTY input stream. */
    for(i=0;i<count;++i) {
        BOOL keep=TRUE;
        if(view->console_input) {
            error=view->console_input(view->window_context,&records[i],&keep);
            if(error)return error;
        }
        if(keep && records[i].EventType!=WINDOW_BUFFER_SIZE_EVENT)records[kept++]=records[i];
    }
    count=kept;
    error=run16_native_backend_input_some(backend,records,count,&consumed);
    if(error==ERROR_NO_MORE_ITEMS) {
        typedef BOOL (WINAPI *prepend_input)(HANDLE,PINPUT_RECORD,DWORD,LPDWORD);
        prepend_input prepend=(prepend_input)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"WriteConsoleInputVDMW");
        DWORD restored=0;
        /* These records never entered ConPTY. Restore only that suffix ahead
         * of newer physical input; never reclaim a delivered native prefix. */
        if(consumed<count) {
            if(!prepend)return ERROR_CALL_NOT_IMPLEMENTED;
            if(!prepend(view->input,records+consumed,count-consumed,&restored))return GetLastError();
            if(restored!=count-consumed)return ERROR_WRITE_FAULT;
        }
        return 0;
    }
    return error;
}

DWORD run16_native_view_wait(run16_native_backend *backend,run16_native_console_view *view,
    HANDLE target,DWORD *result)
{
    DWORD error=ERROR_SUCCESS,wait;
    HANDLE waits[2];
    if(!backend || !view || !target || !result)return ERROR_INVALID_PARAMETER;
    view->presentation_error=ERROR_SUCCESS;
    waits[0]=target;waits[1]=view->input;
    for(;;) {
        BOOL ended=WaitForSingleObject(target,0)==WAIT_OBJECT_0;
        error=run16_native_view_present(backend,view);
        if(error && error!=ERROR_RETRY)break;
        if(ended)break;
        wait=WaitForMultipleObjects(2,waits,FALSE,30);
        if(wait==WAIT_OBJECT_0 || wait==WAIT_TIMEOUT)continue;
        if(wait!=WAIT_OBJECT_0+1) {
            error=GetLastError();
            break;
        }
        error=run16_native_view_forward_input(backend,view);
        if(error)break;
    }
    view->presentation_error=error;
    /* Completion is authoritative even if the last frame/input exchange lost
     * its peer. Recheck here to cover completion during a failed exchange.
     * A still-running target is neither success nor forcibly terminated. */
    wait=WaitForSingleObject(target,0);
    if(wait==WAIT_OBJECT_0) {
        if(!GetExitCodeProcess(target,result))return GetLastError();
        return ERROR_SUCCESS;
    }
    if(wait==WAIT_FAILED)return GetLastError();
    return error;
}
