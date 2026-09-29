#include "ntcon-exe/console_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
static HANDLE close_seen;
static BOOL WINAPI control(DWORD event)
{
    if(event==CTRL_CLOSE_EVENT)SetEvent(close_seen);
    return FALSE;
}
int wmain(int argc,WCHAR **argv)
{
    WCHAR image[MAX_PATH],command[MAX_PATH+120];
    STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION process={0};
    DWORD error=0,code=0;HANDLE ready=NULL;
    SECURITY_ATTRIBUTES security={sizeof(security),NULL,TRUE};
    if(!GetModuleFileNameW(NULL,image,MAX_PATH))return 1;
    if(argc==4 && !wcscmp(argv[1],L"--client")) {
        ready=(HANDLE)(ULONG_PTR)_wcstoui64(argv[2],NULL,10);
        close_seen=(HANDLE)(ULONG_PTR)_wcstoui64(argv[3],NULL,10);
        if(!SetConsoleCtrlHandler(control,TRUE) || !SetEvent(ready))return 2;
        Sleep(20000);return 3;
    }
    if(argc==2 && !wcscmp(argv[1],L"--owner")) {
        HWND window=GetConsoleWindow();
        if(!window || IsWindowVisible(window))return 4;
        ready=CreateEventW(&security,TRUE,FALSE,NULL);
        close_seen=CreateEventW(&security,TRUE,FALSE,NULL);
        if(!ready || !close_seen)return 5;
        swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" --client %llu %llu",image,
            (unsigned long long)(ULONG_PTR)ready,(unsigned long long)(ULONG_PTR)close_seen);
        if(!CreateProcessW(image,command,NULL,NULL,TRUE,0,NULL,NULL,&startup,&process))return 6;
        if(WaitForSingleObject(ready,5000)!=WAIT_OBJECT_0)error=7;
        if(!error)error=ntcon_console_close();
        if(!error && (IsWindow(window) || GetConsoleWindow()))error=8;
        if(!error && WaitForSingleObject(close_seen,1000)!=WAIT_OBJECT_0)error=9;
        if(!error && WaitForSingleObject(process.hProcess,5000)!=WAIT_OBJECT_0)error=10;
        if(!error && ntcon_console_close()!=ERROR_INVALID_HANDLE)error=11;
    } else if(argc==1) {
        swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" --owner",image);
        startup.dwFlags=STARTF_USESHOWWINDOW;startup.wShowWindow=SW_HIDE;
        if(!CreateProcessW(image,command,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&startup,&process))return 12;
        if(WaitForSingleObject(process.hProcess,15000)!=WAIT_OBJECT_0)error=13;
        else if(!GetExitCodeProcess(process.hProcess,&code))error=14;
        else error=code;
        printf("NTCON-CLOSE error=%lu normal-console-close-and-client-signal=%s\n",error,error ? "FAIL" : "PASS");
    } else return 15;
    if(process.hProcess) {
        if(WaitForSingleObject(process.hProcess,0)==WAIT_TIMEOUT)TerminateProcess(process.hProcess,99);
        CloseHandle(process.hProcess);CloseHandle(process.hThread);
    }
    if(ready)CloseHandle(ready);if(close_seen)CloseHandle(close_seen);
    return (int)error;
}
