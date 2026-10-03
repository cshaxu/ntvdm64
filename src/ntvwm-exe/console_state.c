/* Recovered from S8 d253e55af native_console_capture.c; see component README. */
#include "console_state.h"
#include "common/console/members.h"
#include <string.h>
#include <stdio.h>

BOOL ntvwm_console_quiescent(DWORD completed_target)
{
    DWORD count,index,*members=NULL;
    BOOL self=FALSE,empty=FALSE;
    if(common_console_members_read(16,65536,0,&members,&count))return FALSE;
    for(index=0;index<count;++index) {
        HANDLE process;DWORD wait,error;
        if(members[index]==GetCurrentProcessId()){self=TRUE;continue;}
        if(members[index]==completed_target)continue;
        process=OpenProcess(SYNCHRONIZE,FALSE,members[index]);
        if(!process) {
            error=GetLastError();
            if(error==ERROR_INVALID_PARAMETER)continue; /* Gone before pin. */
            goto done;
        }
        wait=WaitForSingleObject(process,0);CloseHandle(process);
        if(wait!=WAIT_OBJECT_0)goto done;
    }
    empty=self;
done:
    common_console_members_release(members);
    return empty;
}

/* A VT-input native client consumes character records, not MOUSE_EVENTs.
 * Keep the ordinary Console path unchanged for classic clients. */
static DWORD write_vt_mouse(HANDLE input,const MOUSE_EVENT_RECORD *mouse)
{
    CONSOLE_SCREEN_BUFFER_INFO screen;
    INPUT_RECORD keys[48]={0};
    char sequence[48];
    DWORD code,button=mouse->dwButtonState,flags=mouse->dwEventFlags;
    DWORD count,index,done;
    int length;
    if(!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&screen))
        return GetLastError();
    if(flags&MOUSE_WHEELED)code=64+((SHORT)HIWORD(button)<0 ? 1 : 0);
    else if(flags&MOUSE_HWHEELED)code=64+((SHORT)HIWORD(button)<0 ? 7 : 6);
    else if(button&FROM_LEFT_1ST_BUTTON_PRESSED)code=0;
    else if(button&RIGHTMOST_BUTTON_PRESSED)code=2;
    else code=3;
    if(flags&MOUSE_MOVED)code|=32;
    if(mouse->dwControlKeyState&SHIFT_PRESSED)code|=4;
    if(mouse->dwControlKeyState&(LEFT_ALT_PRESSED|RIGHT_ALT_PRESSED))code|=8;
    if(mouse->dwControlKeyState&(LEFT_CTRL_PRESSED|RIGHT_CTRL_PRESSED))code|=16;
    length=sprintf_s(sequence,sizeof(sequence),"\x1b[<%lu;%u;%u%c",code,
        (unsigned)(mouse->dwMousePosition.X-screen.srWindow.Left+1),
        (unsigned)(mouse->dwMousePosition.Y-screen.srWindow.Top+1),
        !(flags&(MOUSE_MOVED|MOUSE_WHEELED|MOUSE_HWHEELED)) && !
            (button&(FROM_LEFT_1ST_BUTTON_PRESSED|RIGHTMOST_BUTTON_PRESSED)) ? 'm' : 'M');
    if(length<=0 || length>(int)(sizeof(keys)/sizeof(keys[0])))return ERROR_INVALID_DATA;
    count=(DWORD)length;
    for(index=0;index<count;++index) {
        keys[index].EventType=KEY_EVENT;
        keys[index].Event.KeyEvent.bKeyDown=TRUE;
        keys[index].Event.KeyEvent.wRepeatCount=1;
        keys[index].Event.KeyEvent.uChar.UnicodeChar=(WCHAR)(unsigned char)sequence[index];
    }
    if(!WriteConsoleInputW(input,keys,count,&done))return GetLastError();
    return done==count ? ERROR_SUCCESS : ERROR_WRITE_FAULT;
}

/* Optional failure evidence, never a geometry selector or recovery policy. */
void ntvwm_trace_error(const char *stage,DWORD operation,DWORD error)
{
    WCHAR path[MAX_PATH];char line[320];DWORD length,written;
    HANDLE file,output;
    CONSOLE_SCREEN_BUFFER_INFO info={0};
    CONSOLE_FONT_INFOEX font={sizeof(font)};
    if(!error || error==ERROR_NOT_READY || error==ERROR_BUSY)return;
    length=GetEnvironmentVariableW(L"NTVWM_GEOMETRY_ERROR_LOG",path,MAX_PATH);
    if(!length || length>=MAX_PATH)return;
    output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    if(output!=INVALID_HANDLE_VALUE) {
        GetConsoleScreenBufferInfo(output,&info);GetCurrentConsoleFontEx(output,FALSE,&font);
        CloseHandle(output);
    }
    length=(DWORD)sprintf_s(line,sizeof(line),"pid=%lu stage=%s operation=%lu error=%lu buffer=%dx%d rect=%d,%d,%d,%d cursor=%d,%d carrier=%dx%d\r\n",
        GetCurrentProcessId(),stage,operation,error,info.dwSize.X,info.dwSize.Y,
        info.srWindow.Left,info.srWindow.Top,info.srWindow.Right,info.srWindow.Bottom,
        info.dwCursorPosition.X,info.dwCursorPosition.Y,font.dwFontSize.X,font.dwFontSize.Y);
    file=CreateFileW(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,0,NULL);
    if(file!=INVALID_HANDLE_VALUE){WriteFile(file,line,length,&written,NULL);CloseHandle(file);}
}

/* Same-owner recovery: S8 native_console_host.c::input_records. Do not use an
 * inherited Ctrl-C ignore flag: each native target retains its own handlers. */
DWORD ntvwm_input_write(HANDLE input,const INPUT_RECORD *records,DWORD count,DWORD *written)
{
    DWORD mode,done;
    if(!written)return ERROR_INVALID_PARAMETER;
    *written=0;
    if(!records && count)return ERROR_INVALID_PARAMETER;
    while(*written<count) {
        const INPUT_RECORD *record=records+*written;
        const KEY_EVENT_RECORD *key=&record->Event.KeyEvent;
        BOOL control=record->EventType==KEY_EVENT && key->bKeyDown &&
            (key->dwControlKeyState&(LEFT_CTRL_PRESSED|RIGHT_CTRL_PRESSED)) &&
            !(key->dwControlKeyState&(LEFT_ALT_PRESSED|RIGHT_ALT_PRESSED));
        if(!GetConsoleMode(input,&mode))return GetLastError();
        /* WriteConsoleInput does not run the Console UI's Ctrl-C/Break path. */
        if(control && ((key->wVirtualKeyCode=='C' && (mode&ENABLE_PROCESSED_INPUT)) ||
            key->wVirtualKeyCode==VK_CANCEL)) {
            DWORD event=key->wVirtualKeyCode==VK_CANCEL ? CTRL_BREAK_EVENT : CTRL_C_EVENT;
            if(event==CTRL_BREAK_EVENT && !FlushConsoleInputBuffer(input))return GetLastError();
            if(!GenerateConsoleCtrlEvent(event,0))return GetLastError();
        } else if(record->EventType==MOUSE_EVENT &&
            (mode&ENABLE_VIRTUAL_TERMINAL_INPUT) && !(mode&ENABLE_MOUSE_INPUT)) {
            DWORD error=write_vt_mouse(input,&record->Event.MouseEvent);
            if(error)return error;
        } else {
            if(!WriteConsoleInputW(input,record,1,&done))return GetLastError();
            if(done!=1)return ERROR_WRITE_FAULT;
        }
        ++*written;
    }
    return ERROR_SUCCESS;
}

/* A hidden Console still enforces pixel-window limits. Use a fixed carrier
 * font only when that blocks the requested logical region; never derive the
 * region from those limits. This is not the copied glyph/Window font. */
static DWORD prepare_hidden_geometry(HANDLE output)
{
    CONSOLE_FONT_INFOEX font={sizeof(font)},actual={sizeof(actual)};
    if(!GetCurrentConsoleFontEx(output,FALSE,&font))return GetLastError();
    if(font.dwFontSize.X==2 && font.dwFontSize.Y==4)return ERROR_SUCCESS;
    font.dwFontSize.X=2;font.dwFontSize.Y=4;
    font.FontFamily=FF_MODERN; font.FontWeight=FW_NORMAL;
    wcscpy_s(font.FaceName,LF_FACESIZE,L"Consolas");
    if(!SetCurrentConsoleFontEx(output,FALSE,&font) ||
        !GetCurrentConsoleFontEx(output,FALSE,&actual))return GetLastError();
    return actual.dwFontSize.X==2 && actual.dwFontSize.Y==4 ? ERROR_SUCCESS : ERROR_NOT_SUPPORTED;
}

DWORD ntvwm_console_initialize(void)
{
    DWORD error;
    HANDLE output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    if(output==INVALID_HANDLE_VALUE)return GetLastError();
    error=prepare_hidden_geometry(output);
    CloseHandle(output);return error;
}

static BOOL set_hidden_size(HANDLE output,COORD size)
{
    CONSOLE_SCREEN_BUFFER_INFOEX info={sizeof(info)};
    if(SetConsoleScreenBufferSize(output,size))return TRUE;
    if(GetLastError()!=ERROR_INVALID_PARAMETER)return FALSE;
    if(!GetConsoleScreenBufferInfoEx(output,&info))return FALSE;
    info.dwSize=size;
    ++info.srWindow.Right;++info.srWindow.Bottom;
    return SetConsoleScreenBufferInfoEx(output,&info);
}

static BOOL set_hidden_window(HANDLE output,const SMALL_RECT *window)
{
    CONSOLE_SCREEN_BUFFER_INFOEX info={sizeof(info)};
    CONSOLE_SCREEN_BUFFER_INFO actual;
    DWORD error;
    if(SetConsoleWindowInfo(output,TRUE,window))return TRUE;
    if(GetLastError()!=ERROR_INVALID_PARAMETER)return FALSE;
    if(!GetConsoleScreenBufferInfoEx(output,&info))return FALSE;
    info.srWindow=*window;
    ++info.srWindow.Right;++info.srWindow.Bottom;
    if(!SetConsoleScreenBufferInfoEx(output,&info) ||
        !GetConsoleScreenBufferInfo(output,&actual))return FALSE;
    if(!memcmp(&actual.srWindow,window,sizeof(*window)))return TRUE;
    error=prepare_hidden_geometry(output);
    if(error) {SetLastError(error);return FALSE;}
    return SetConsoleWindowInfo(output,TRUE,window);
}

DWORD ntvwm_screen_apply(HANDLE output,const CONSOLE_SCREEN_BUFFER_INFOEX *info,
    const CONSOLE_CURSOR_INFO *cursor)
{
    CONSOLE_SCREEN_BUFFER_INFOEX previous={sizeof(previous)},copy;
    CONSOLE_CURSOR_INFO old_cursor;
    CONSOLE_SCREEN_BUFFER_INFO current;
    COORD capacity;
    BOOL moved,resized,colors,cursor_moved;
    if(!info || !cursor || info->cbSize!=sizeof(*info) ||
        info->dwSize.X<=0 || info->dwSize.Y<=0 ||
        info->srWindow.Left<0 || info->srWindow.Top<0 ||
        info->srWindow.Right<info->srWindow.Left || info->srWindow.Bottom<info->srWindow.Top ||
        info->srWindow.Right>=info->dwSize.X || info->srWindow.Bottom>=info->dwSize.Y ||
        info->dwCursorPosition.X<0 || info->dwCursorPosition.Y<0 ||
        info->dwCursorPosition.X>=info->dwSize.X || info->dwCursorPosition.Y>=info->dwSize.Y ||
        !cursor->dwSize || cursor->dwSize>100) return ERROR_INVALID_DATA;
    if(!GetConsoleScreenBufferInfoEx(output,&previous)) return GetLastError();
    if(!GetConsoleCursorInfo(output,&old_cursor))return GetLastError();
    resized=previous.dwSize.X!=info->dwSize.X || previous.dwSize.Y!=info->dwSize.Y;
    moved=memcmp(&previous.srWindow,&info->srWindow,sizeof(info->srWindow))!=0;
    cursor_moved=previous.dwCursorPosition.X!=info->dwCursorPosition.X ||
        previous.dwCursorPosition.Y!=info->dwCursorPosition.Y;
    colors=previous.wPopupAttributes!=info->wPopupAttributes ||
        memcmp(previous.ColorTable,info->ColorTable,sizeof(info->ColorTable))!=0;
    /* Grow before moving the viewport; shrink only after it fits. InfoEx is
     * only the fallback for the invisible carrier's pixel-window limits.
     * Its setter consumes exclusive right/bottom, unlike its getter. */
    capacity.X=max(previous.dwSize.X,info->dwSize.X);
    capacity.Y=max(previous.dwSize.Y,info->dwSize.Y);
    if((capacity.X!=previous.dwSize.X || capacity.Y!=previous.dwSize.Y) &&
        !set_hidden_size(output,capacity))return GetLastError();
    if(moved && !set_hidden_window(output,&info->srWindow))return GetLastError();
    if((capacity.X!=info->dwSize.X || capacity.Y!=info->dwSize.Y) &&
        !set_hidden_size(output,info->dwSize))return GetLastError();
    copy=*info;
    if(colors && !SetConsoleScreenBufferInfoEx(output,&copy))return GetLastError();
    if(previous.wAttributes!=info->wAttributes && !SetConsoleTextAttribute(output,info->wAttributes))return GetLastError();
    if(cursor_moved && !SetConsoleCursorPosition(output,info->dwCursorPosition))return GetLastError();
    /* Moving a cursor may scroll the viewport; restore the source's exact
     * inclusive rectangle afterward, including a user-scrolled history view.
     * Do not resize an unchanged buffer: that creates input-event feedback. */
    if(resized || moved || colors || cursor_moved) {
        if(!GetConsoleScreenBufferInfo(output,&current))return GetLastError();
        if(memcmp(&current.srWindow,&info->srWindow,sizeof(info->srWindow)) &&
            !set_hidden_window(output,&info->srWindow))return GetLastError();
    }
    if((old_cursor.dwSize!=cursor->dwSize || old_cursor.bVisible!=cursor->bVisible) &&
        !SetConsoleCursorInfo(output,cursor))return GetLastError();
    /* API success alone is not an application acknowledgment: the backend
     * must really expose the inherited buffer, region and cursor to targets. */
    if(!GetConsoleScreenBufferInfo(output,&current))return GetLastError();
    if(current.dwSize.X!=info->dwSize.X || current.dwSize.Y!=info->dwSize.Y ||
        memcmp(&current.srWindow,&info->srWindow,sizeof(info->srWindow)) ||
        current.dwCursorPosition.X!=info->dwCursorPosition.X ||
        current.dwCursorPosition.Y!=info->dwCursorPosition.Y)return ERROR_RETRY;
    return ERROR_SUCCESS;
}

DWORD ntvwm_cells_write(HANDLE output,DWORD offset,const CHAR_INFO *cells,DWORD count)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    SMALL_RECT requested,actual;
    COORD size,origin={0,0};
    DWORD width,total,x,y,rows,columns;
    if(!cells || !count) return ERROR_INVALID_PARAMETER;
    if(!GetConsoleScreenBufferInfo(output,&info)) return GetLastError();
    width=(DWORD)info.dwSize.X;total=width*(DWORD)info.dwSize.Y;
    if(!width || offset>=total || count>total-offset) return ERROR_INVALID_PARAMETER;
    x=offset%width;y=offset/width;
    columns=count;rows=1;
    if(count>width-x) {
        if(x || count%width) return ERROR_INVALID_PARAMETER;
        columns=width;rows=count/width;
    }
    size.X=(SHORT)columns;size.Y=(SHORT)rows;
    requested.Left=(SHORT)x;requested.Top=(SHORT)y;
    requested.Right=(SHORT)(x+columns-1);requested.Bottom=(SHORT)(y+rows-1);
    actual=requested;
    if(!WriteConsoleOutputW(output,cells,size,origin,&actual)) return GetLastError();
    return memcmp(&actual,&requested,sizeof(actual)) ? ERROR_RETRY : ERROR_SUCCESS;
}

/* Same-owner prototype recovery: retain the current CONOUT$ open and exact
 * rectangle/geometry checks, not its fixed 160x54 viewport-only frame. The
 * modern Console owns storage and scrolling; this adapter only copies cells. */
static BOOL same_geometry(const CONSOLE_SCREEN_BUFFER_INFOEX *left,
    const CONSOLE_SCREEN_BUFFER_INFOEX *right)
{
    return left->dwSize.X==right->dwSize.X && left->dwSize.Y==right->dwSize.Y &&
        !memcmp(&left->srWindow,&right->srWindow,sizeof(left->srWindow));
}

void ntvwm_capture_end(ntvwm_capture *capture)
{
    if (!capture) return;
    if (capture->buffer && capture->buffer!=INVALID_HANDLE_VALUE) CloseHandle(capture->buffer);
    ZeroMemory(capture,sizeof(*capture));
}

DWORD ntvwm_capture_begin(ntvwm_capture *capture)
{
    HANDLE output;
    DWORD error;
    if (!capture) return ERROR_INVALID_PARAMETER;
    ZeroMemory(capture,sizeof(*capture));
    output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    if (output==INVALID_HANDLE_VALUE) return GetLastError();
    error=ntvwm_capture_begin_output(capture,output);
    CloseHandle(output);
    return error;
}

DWORD ntvwm_capture_begin_output(ntvwm_capture *capture,HANDLE output)
{
    CONSOLE_SCREEN_BUFFER_INFOEX after={sizeof(after)};
    DWORD error;
    if (!capture) return ERROR_INVALID_PARAMETER;
    ZeroMemory(capture,sizeof(*capture));
    if (!output || output==INVALID_HANDLE_VALUE) return ERROR_INVALID_HANDLE;
    if (!DuplicateHandle(GetCurrentProcess(),output,GetCurrentProcess(),
        &capture->buffer,0,FALSE,DUPLICATE_SAME_ACCESS)) return GetLastError();
    capture->info.cbSize=sizeof(capture->info);
    if (!GetConsoleScreenBufferInfoEx(capture->buffer,&capture->info) ||
        !GetConsoleCursorInfo(capture->buffer,&capture->cursor) ||
        !GetConsoleMode(capture->buffer,&capture->output_mode)) { error=GetLastError();goto fail; }
    capture->input_codepage=GetConsoleCP();
    if (!capture->input_codepage) { error=GetLastError();goto fail; }
    capture->output_codepage=GetConsoleOutputCP();
    if (!capture->output_codepage) { error=GetLastError();goto fail; }
    if (!GetConsoleScreenBufferInfoEx(capture->buffer,&after)) { error=GetLastError();goto fail; }
    if (capture->info.dwSize.X<=0 || capture->info.dwSize.Y<=0 ||
        !same_geometry(&capture->info,&after)) { error=ERROR_RETRY;goto fail; }
    return ERROR_SUCCESS;
fail:
    ntvwm_capture_end(capture);
    return error;
}

DWORD ntvwm_capture_read(ntvwm_capture *capture,DWORD offset,
    CHAR_INFO *cells,DWORD capacity,SMALL_RECT *region,DWORD *count)
{
    CONSOLE_SCREEN_BUFFER_INFOEX before={sizeof(before)},after={sizeof(after)};
    SMALL_RECT requested,actual;
    COORD size,origin={0,0};
    DWORD width,height,x,y,columns,rows;
    if (!count) return ERROR_INVALID_PARAMETER;
    *count=0;
    if (!capture || !capture->buffer || capture->buffer==INVALID_HANDLE_VALUE ||
        !cells || !capacity || !region) return ERROR_INVALID_PARAMETER;
    width=(DWORD)capture->info.dwSize.X;height=(DWORD)capture->info.dwSize.Y;
    if (!width || !height || width>32767 || height>32767) return ERROR_INVALID_DATA;
    if (offset>=width*height) return ERROR_NO_MORE_ITEMS;
    x=offset%width;y=offset/width;
    columns=width-x;
    if (columns>capacity) columns=capacity;
    rows=(!x && columns==width) ? capacity/width : 1;
    if (rows>height-y) rows=height-y;
    size.X=(SHORT)columns;size.Y=(SHORT)rows;
    requested.Left=(SHORT)x;requested.Top=(SHORT)y;
    requested.Right=(SHORT)(x+columns-1);requested.Bottom=(SHORT)(y+rows-1);
    actual=requested;
    if (!GetConsoleScreenBufferInfoEx(capture->buffer,&before)) return GetLastError();
    if (!same_geometry(&capture->info,&before)) return ERROR_RETRY;
    if (!ReadConsoleOutputW(capture->buffer,cells,size,origin,&actual)) {
        DWORD error=GetLastError();
        /* A target can shrink the buffer after our preceding geometry read,
         * making a formerly valid tile invalid. Prove that change before
         * treating the API failure as a torn snapshot; do not mask real I/O
         * errors or publish cells from two different geometries. */
        if(GetConsoleScreenBufferInfoEx(capture->buffer,&after) &&
            !same_geometry(&before,&after)) {
            ntvwm_trace_error("resized-during-read",0,error);
            return ERROR_RETRY;
        }
        return error;
    }
    if (!GetConsoleScreenBufferInfoEx(capture->buffer,&after)) return GetLastError();
    if (memcmp(&requested,&actual,sizeof(actual)) || !same_geometry(&before,&after))
        return ERROR_RETRY;
    *region=actual;*count=columns*rows;
    return ERROR_SUCCESS;
}

DWORD ntvwm_console_close(void)
{
    HWND window=GetConsoleWindow();DWORD process=0,thread,current_process=0;
    ULONGLONG deadline;
    if(!window)return ERROR_INVALID_HANDLE;
    thread=GetWindowThreadProcessId(window,&process);
    if(!thread || !process)return ERROR_INVALID_HANDLE;
    /* Keep the worker outside the Console's CTRL_CLOSE broadcast, so it can
     * confirm closure. FreeConsole alone is insufficient with live clients. */
    if(!FreeConsole())return GetLastError();
    if(!IsWindow(window))return ERROR_SUCCESS;
    if(GetWindowThreadProcessId(window,&current_process)!=thread || current_process!=process)
        return ERROR_INVALID_HANDLE;
    if(!PostMessageW(window,WM_CLOSE,0,0))return GetLastError();
    deadline=GetTickCount64()+8000;
    while(IsWindow(window)) {
        if(GetWindowThreadProcessId(window,&current_process)!=thread || current_process!=process)
            return ERROR_INVALID_HANDLE;
        if(GetTickCount64()>=deadline)return ERROR_TIMEOUT;
        Sleep(10);
    }
    return ERROR_SUCCESS;
}
