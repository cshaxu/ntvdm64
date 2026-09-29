/* Recovered from S8 d253e55af native_console_capture.c; see component README. */
#include "console_state.h"
#include <string.h>

/* Same-owner recovery: S8 native_console_host.c::input_records. Do not use an
 * inherited Ctrl-C ignore flag: each native target retains its own handlers. */
DWORD ntcon_input_write(HANDLE input,const INPUT_RECORD *records,DWORD count,DWORD *written)
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
        } else {
            if(!WriteConsoleInputW(input,record,1,&done))return GetLastError();
            if(done!=1)return ERROR_WRITE_FAULT;
        }
        ++*written;
    }
    return ERROR_SUCCESS;
}

DWORD ntcon_screen_apply(HANDLE output,const CONSOLE_SCREEN_BUFFER_INFOEX *info,
    const CONSOLE_CURSOR_INFO *cursor)
{
    CONSOLE_SCREEN_BUFFER_INFOEX previous={sizeof(previous)},copy;
    CONSOLE_CURSOR_INFO old_cursor;
    CONSOLE_SCREEN_BUFFER_INFO current;
    COORD capacity;
    BOOL moved,colors,cursor_moved;
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
    capacity.X=max(previous.dwSize.X,info->dwSize.X);
    capacity.Y=max(previous.dwSize.Y,info->dwSize.Y);
    moved=memcmp(&previous.srWindow,&info->srWindow,sizeof(info->srWindow))!=0;
    cursor_moved=previous.dwCursorPosition.X!=info->dwCursorPosition.X ||
        previous.dwCursorPosition.Y!=info->dwCursorPosition.Y;
    colors=previous.wPopupAttributes!=info->wPopupAttributes ||
        memcmp(previous.ColorTable,info->ColorTable,sizeof(info->ColorTable))!=0;
    /* Grow before moving the viewport; shrink only after it fits. */
    if((capacity.X!=previous.dwSize.X || capacity.Y!=previous.dwSize.Y) &&
        !SetConsoleScreenBufferSize(output,capacity)) return GetLastError();
    if(moved && !SetConsoleWindowInfo(output,TRUE,&info->srWindow))return GetLastError();
    if((capacity.X!=info->dwSize.X || capacity.Y!=info->dwSize.Y) &&
        !SetConsoleScreenBufferSize(output,info->dwSize)) return GetLastError();
    copy=*info;
    if(colors && !SetConsoleScreenBufferInfoEx(output,&copy))return GetLastError();
    if(previous.wAttributes!=info->wAttributes && !SetConsoleTextAttribute(output,info->wAttributes))return GetLastError();
    if(cursor_moved && !SetConsoleCursorPosition(output,info->dwCursorPosition))return GetLastError();
    /* Moving a cursor may scroll the viewport; restore the source's exact
     * inclusive rectangle afterward, including a user-scrolled history view.
     * Do not resize an unchanged buffer: that creates input-event feedback. */
    if(moved || colors || cursor_moved) {
        if(!GetConsoleScreenBufferInfo(output,&current))return GetLastError();
        if(memcmp(&current.srWindow,&info->srWindow,sizeof(info->srWindow)) &&
            !SetConsoleWindowInfo(output,TRUE,&info->srWindow))return GetLastError();
    }
    if((old_cursor.dwSize!=cursor->dwSize || old_cursor.bVisible!=cursor->bVisible) &&
        !SetConsoleCursorInfo(output,cursor))return GetLastError();
    return ERROR_SUCCESS;
}

DWORD ntcon_cells_write(HANDLE output,DWORD offset,const CHAR_INFO *cells,DWORD count)
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

void ntcon_capture_end(ntcon_capture *capture)
{
    if (!capture) return;
    if (capture->buffer && capture->buffer!=INVALID_HANDLE_VALUE) CloseHandle(capture->buffer);
    ZeroMemory(capture,sizeof(*capture));
}

DWORD ntcon_capture_begin(ntcon_capture *capture)
{
    HANDLE output;
    DWORD error;
    if (!capture) return ERROR_INVALID_PARAMETER;
    ZeroMemory(capture,sizeof(*capture));
    output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    if (output==INVALID_HANDLE_VALUE) return GetLastError();
    error=ntcon_capture_begin_output(capture,output);
    CloseHandle(output);
    return error;
}

DWORD ntcon_capture_begin_output(ntcon_capture *capture,HANDLE output)
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
    ntcon_capture_end(capture);
    return error;
}

DWORD ntcon_capture_read(ntcon_capture *capture,DWORD offset,
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
    if (!ReadConsoleOutputW(capture->buffer,cells,size,origin,&actual) ||
        !GetConsoleScreenBufferInfoEx(capture->buffer,&after)) return GetLastError();
    if (memcmp(&requested,&actual,sizeof(actual)) || !same_geometry(&before,&after))
        return ERROR_RETRY;
    *region=actual;*count=columns*rows;
    return ERROR_SUCCESS;
}

DWORD ntcon_console_close(void)
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

DWORD ntcon_console_members(DWORD **members,DWORD *count)
{
    DWORD capacity=16,attempt;
    if(!members || !count)return ERROR_INVALID_PARAMETER;
    *members=NULL;*count=0;
    for(attempt=0;attempt<8;++attempt) {
        DWORD *ids,total,index,used=0,self=0,error;
        ids=HeapAlloc(GetProcessHeap(),0,capacity*sizeof(*ids));
        if(!ids)return ERROR_NOT_ENOUGH_MEMORY;
        total=GetConsoleProcessList(ids,capacity);
        if(!total) {error=GetLastError();HeapFree(GetProcessHeap(),0,ids);return error ? error : ERROR_INVALID_HANDLE;}
        if(total>capacity) {
            HeapFree(GetProcessHeap(),0,ids);
            if(total>65536)return ERROR_BUFFER_OVERFLOW;
            capacity=total;continue;
        }
        for(index=0;index<total;++index) {
            if(ids[index]==GetCurrentProcessId())++self;
            else ids[used++]=ids[index];
        }
        if(self!=1) {HeapFree(GetProcessHeap(),0,ids);return ERROR_INVALID_DATA;}
        *members=ids;*count=used;return ERROR_SUCCESS;
    }
    return ERROR_RETRY;
}
