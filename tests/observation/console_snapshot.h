#ifndef OBSERVER_CONSOLE_SNAPSHOT_H
#define OBSERVER_CONSOLE_SNAPSHOT_H
#include <windows.h>
#include <stdlib.h>
#include <string.h>

#ifdef OBSERVER_RECTANGLE_CAPTURE
static BOOL observer_read_cells(HANDLE output,char *copy,COORD size,DWORD *read)
{
    CHAR_INFO *tile;
    unsigned y,x,rows;
    *read=0;
    tile=(CHAR_INFO*)malloc((size_t)size.X*32*sizeof(*tile));
    if(!tile){SetLastError(ERROR_NOT_ENOUGH_MEMORY);return FALSE;}
    for(y=0;y<(unsigned)size.Y;y+=rows) {
        SMALL_RECT requested,actual;
        COORD dimensions;
        rows=(unsigned)size.Y-y;if(rows>32)rows=32;
        dimensions.X=size.X;dimensions.Y=(SHORT)rows;
        requested.Left=0;requested.Top=(SHORT)y;
        requested.Right=size.X-1;requested.Bottom=(SHORT)(y+rows-1);
        actual=requested;
        if(!ReadConsoleOutputA(output,tile,dimensions,(COORD){0,0},&actual)) {
            DWORD error=GetLastError();free(tile);SetLastError(error);return FALSE;
        }
        if(memcmp(&actual,&requested,sizeof(actual))) {
            free(tile);return TRUE; /* Short read: caller rejects/retries. */
        }
        for(x=0;x<(unsigned)size.X*rows;++x)copy[(*read)++]=tile[x].Char.AsciiChar;
    }
    free(tile);return TRUE;
}
#endif

/* Test observer only. A linear read must not be indexed with stale geometry.
 * Resize and repaint are separate operations: require repeated content too.
 * Retry bounded instability; report failure rather than inventing rows. */
static DWORD observer_console_snapshot(HANDLE output,
    CONSOLE_SCREEN_BUFFER_INFO *info,char **screen,DWORD *count)
{
    unsigned attempt;
    *screen=NULL;*count=0;
    for(attempt=0;attempt<4;++attempt) {
        CONSOLE_SCREEN_BUFFER_INFO after;
        DWORD cells,read=0,verified=0,error;
        char *copy;
        if(!GetConsoleScreenBufferInfo(output,info))return GetLastError();
        if(info->dwSize.X<=0 || info->dwSize.Y<=0)return ERROR_INVALID_DATA;
        cells=(DWORD)info->dwSize.X*(DWORD)info->dwSize.Y;
        if(cells>4u*1024u*1024u)return ERROR_NOT_SUPPORTED;
        copy=(char*)malloc((size_t)cells*2);
        if(!copy)return ERROR_NOT_ENOUGH_MEMORY;
#ifdef OBSERVER_RECTANGLE_CAPTURE
        if(!observer_read_cells(output,copy,info->dwSize,&read) ||
           !observer_read_cells(output,copy+cells,info->dwSize,&verified) ||
#else
        if(!ReadConsoleOutputCharacterA(output,copy,cells,(COORD){0,0},&read) ||
           !ReadConsoleOutputCharacterA(output,copy+cells,cells,(COORD){0,0},&verified) ||
#endif
           !GetConsoleScreenBufferInfo(output,&after)) {
            error=GetLastError();free(copy);return error;
        }
        if(read==cells && verified==cells && !memcmp(copy,copy+cells,cells) &&
           info->dwSize.X==after.dwSize.X &&
           info->dwSize.Y==after.dwSize.Y &&
           !memcmp(&info->srWindow,&after.srWindow,sizeof(info->srWindow))) {
            *screen=copy;*count=read;return ERROR_SUCCESS;
        }
        free(copy);
    }
    return ERROR_RETRY;
}
#endif
