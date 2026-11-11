#include <windows.h>
#include <stdio.h>
#include <string.h>
static volatile LONG calls;
__declspec(noinline) BOOL WINAPI MvdmWriteConsoleA(HANDLE output,const VOID *data,DWORD length,LPDWORD written,LPVOID reserved)
{
    (void)output;(void)data;(void)reserved;
    InterlockedIncrement(&calls);
    if(written)*written=length;
    SetLastError(4321);return TRUE;
}
int main(void)
{
    DWORD written=0;const char text[]="C-TRACE-WITNESS";
    SetLastError(1234);
    if(!MvdmWriteConsoleA(NULL,text,sizeof(text)-1,&written,NULL) || calls!=1 ||
       written!=sizeof(text)-1 || GetLastError()!=4321)return 1;
    puts("CONSOLE_TRACE_ORIGINAL_PASS");return 0;
}
