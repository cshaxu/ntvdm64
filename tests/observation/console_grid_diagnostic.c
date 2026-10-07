#include <windows.h>
#include <stdio.h>
#include <string.h>
/* Compile the real row-copy primitive; native calls and failures are intact. */
static void grid_note(const char *name,HANDLE h,DWORD error,int x,int y,int right,int bottom)
{
    DWORD saved=GetLastError(),written;CONSOLE_SCREEN_BUFFER_INFO b={0};
    char path[MAX_PATH],line[256];HANDLE file;
    DWORD n=GetEnvironmentVariableA("NTCON_GRID_DIAGNOSTIC",path,sizeof(path));
    if(!n || n>=sizeof(path) || !strstr(path,"\\build\\M0-T436\\S2\\")){SetLastError(saved);return;}
    GetConsoleScreenBufferInfo(h,&b);
    file=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,0,NULL);
    if(file!=INVALID_HANDLE_VALUE) {
        int count=wsprintfA(line,"api=%s error=%lu request=%d,%d,%d,%d buffer=%d,%d view=%d,%d,%d,%d max=%d,%d\r\n",
            name,error,x,y,right,bottom,b.dwSize.X,b.dwSize.Y,b.srWindow.Left,b.srWindow.Top,
            b.srWindow.Right,b.srWindow.Bottom,b.dwMaximumWindowSize.X,b.dwMaximumWindowSize.Y);
        WriteFile(file,line,(DWORD)count,&written,NULL);CloseHandle(file);
    }
    SetLastError(saved);
}
static BOOL grid_window(HANDLE h,BOOL a,const SMALL_RECT *r) {
    BOOL ok=SetConsoleWindowInfo(h,a,r);DWORD e=GetLastError();
    if(!ok)grid_note("window",h,e,r->Left,r->Top,r->Right,r->Bottom);
    SetLastError(e);return ok;
}
static BOOL grid_size(HANDLE h,COORD s) {
    BOOL ok=SetConsoleScreenBufferSize(h,s);DWORD e=GetLastError();
    if(!ok)grid_note("size",h,e,s.X,s.Y,0,0);
    SetLastError(e);return ok;
}
static BOOL grid_read(HANDLE h,CHAR_INFO *c,COORD s,COORD o,SMALL_RECT *r) {
    BOOL ok=ReadConsoleOutputW(h,c,s,o,r);DWORD e=GetLastError();
    if(!ok)grid_note("read",h,e,r->Left,r->Top,r->Right,r->Bottom);
    SetLastError(e);return ok;
}
static BOOL grid_write(HANDLE h,const CHAR_INFO *c,COORD s,COORD o,SMALL_RECT *r) {
    BOOL ok=WriteConsoleOutputW(h,c,s,o,r);DWORD e=GetLastError();
    if(!ok)grid_note("write",h,e,r->Left,r->Top,r->Right,r->Bottom);
    SetLastError(e);return ok;
}
#define SetConsoleWindowInfo grid_window
#define SetConsoleScreenBufferSize grid_size
#define ReadConsoleOutputW grid_read
#define WriteConsoleOutputW grid_write
#include "../../src/opennt-abi/host-compat/console_grid.c"
