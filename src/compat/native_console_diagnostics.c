#include "compat/include/native_console_diagnostics.h"
#include <stdio.h>

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
