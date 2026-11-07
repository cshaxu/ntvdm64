/* Diagnostic-only pass-through at the native Console input boundary. */
#include <windows.h>
#include <stdio.h>
BOOL WINAPI observed_read_input(HANDLE,PINPUT_RECORD,DWORD,LPDWORD);
#define ReadConsoleInputW observed_read_input
#define ntvwm_input_write observed_write_input
#include "ntvwm-exe/presentation.c"
#undef ReadConsoleInputW
#undef ntvwm_input_write
extern DWORD ntvwm_input_write(HANDLE,const INPUT_RECORD *,DWORD,DWORD *);
static void observe_records(char operation,const INPUT_RECORD *records,DWORD count)
{
    DWORD error=GetLastError(),i,written;
    HANDLE file=CreateFileA(TEST_NATIVE_TRACE_PATH,FILE_APPEND_DATA,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,0,NULL);
    if(file==INVALID_HANDLE_VALUE)ExitProcess(GetLastError());
    for(i=0;i<count;++i)if(records[i].EventType==KEY_EVENT) {
        const KEY_EVENT_RECORD *key=&records[i].Event.KeyEvent;
        char line[160];
        int length=sprintf_s(line,sizeof(line),"%lu %c %lu %lu %u %u %u %u\n",
            GetCurrentProcessId(),operation,i,count,key->bKeyDown,
            key->uChar.UnicodeChar,key->wRepeatCount,key->wVirtualScanCode);
        if(!WriteFile(file,line,length,&written,NULL) || written!=(DWORD)length)
            ExitProcess(ERROR_WRITE_FAULT);
    }
    CloseHandle(file);SetLastError(error);
}
BOOL WINAPI observed_read_input(HANDLE input,PINPUT_RECORD records,DWORD count,LPDWORD read)
{
    BOOL result=ReadConsoleInputW(input,records,count,read);
    if(result)observe_records('R',records,*read);
    return result;
}
DWORD observed_write_input(HANDLE input,const INPUT_RECORD *records,DWORD count,DWORD *written)
{
    DWORD error=ntvwm_input_write(input,records,count,written);
    if(!error)observe_records('W',records,*written);
    return error;
}
