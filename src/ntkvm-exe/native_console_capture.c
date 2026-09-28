#include "native_console_capture.h"
#include <string.h>

DWORD run16_native_screen_apply(HANDLE output,const CONSOLE_SCREEN_BUFFER_INFOEX *info,
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

DWORD run16_native_cells_write(HANDLE output,DWORD offset,const CHAR_INFO *cells,DWORD count)
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
