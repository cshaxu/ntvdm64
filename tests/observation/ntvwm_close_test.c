#include "ntvwm-exe/console_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
static HANDLE close_seen;
static FILE *observation;
static HWND observed_window;
static HWINEVENTHOOK filtered_hook;
static BOOL live_after_close;
static ULONGLONG closing_thread_cpu(void)
{
    FILETIME created,exited,kernel,user;
    if(!GetThreadTimes(GetCurrentThread(),&created,&exited,&kernel,&user))return ~0ULL;
    return (((ULONGLONG)kernel.dwHighDateTime<<32)|kernel.dwLowDateTime)+
        (((ULONGLONG)user.dwHighDateTime<<32)|user.dwLowDateTime);
}
static void CALLBACK observe_event(HWINEVENTHOOK hook,DWORD event,HWND window,
    LONG object,LONG child,DWORD thread,DWORD time)
{
    (void)time;
    if(observation && window==observed_window) {
        HANDLE source=OpenThread(THREAD_QUERY_LIMITED_INFORMATION,FALSE,thread);
        DWORD pid=source ? GetProcessIdOfThread(source) : 0;
        if(source)CloseHandle(source);
        fprintf(observation,"event=%lx filtered=%d object=%ld child=%ld thread=%lu source-pid=%lu\n",
            event,hook==filtered_hook,object,child,thread,pid);
        fflush(observation);
    }
}
static DWORD observed_close(void)
{
    const WCHAR *path=_wgetenv(L"NTVWM_CLOSE_OBSERVATION");
    HANDLE owner,thread_handle;DWORD pid=0,tid,error;
    HWINEVENTHOOK all_hook;MSG message;ULONGLONG end,cpu,start,elapsed,used;
    if(!path || !*path)return ntvwm_console_close();
    if(_wfopen_s(&observation,path,L"a") || !observation)return ERROR_OPEN_FAILED;
    observed_window=GetConsoleWindow();tid=GetWindowThreadProcessId(observed_window,&pid);
    owner=OpenProcess(SYNCHRONIZE,FALSE,pid);
    thread_handle=OpenThread(SYNCHRONIZE,FALSE,tid);
    PeekMessageW(&message,NULL,0,0,PM_NOREMOVE);
    filtered_hook=SetWinEventHook(EVENT_MIN,EVENT_MAX,NULL,observe_event,pid,0,WINEVENT_OUTOFCONTEXT);
    all_hook=SetWinEventHook(EVENT_MIN,EVENT_MAX,NULL,observe_event,0,0,WINEVENT_OUTOFCONTEXT);
    fprintf(observation,"begin hwnd=%p pid=%lu tid=%lu self=%lu/%lu handles=%d/%d hooks=%d/%d\n",
        observed_window,pid,tid,GetCurrentProcessId(),GetCurrentThreadId(),
        owner!=NULL,thread_handle!=NULL,filtered_hook!=NULL,all_hook!=NULL);
    fflush(observation);
    cpu=closing_thread_cpu();start=GetTickCount64();
    error=ntvwm_console_close();
    elapsed=GetTickCount64()-start;used=closing_thread_cpu();
    if(cpu==~0ULL || used==~0ULL || used<cpu)error=ERROR_INVALID_DATA;
    else {
        used=(used-cpu)/10000;
        fprintf(observation,"close-wait wall-ms=%llu cpu-ms=%llu delayed-client=%d\n",
            elapsed,used,close_seen!=NULL);
        /* A real client deliberately holds CTRL_CLOSE below. The owner must
         * wait for actual destruction, without burning a core while waiting. */
        if(!error && close_seen && (elapsed<250 || used>elapsed/2))error=ERROR_INVALID_DATA;
    }
    live_after_close=IsWindow(observed_window);
    fprintf(observation,"closed error=%lu window=%d process=%lu thread=%lu\n",error,
        live_after_close,WaitForSingleObject(owner,0),WaitForSingleObject(thread_handle,0));
    /* Diagnostic-only drain, not a production timeout or completion rule. */
    end=GetTickCount64()+500;
    for(;;) {
        ULONGLONG now=GetTickCount64();
        if(now>=end)break;
        MsgWaitForMultipleObjectsEx(0,NULL,(DWORD)(end-now),QS_ALLINPUT,MWMO_INPUTAVAILABLE);
        while(PeekMessageW(&message,NULL,0,0,PM_REMOVE)) {
            TranslateMessage(&message);DispatchMessageW(&message);
        }
    }
    fprintf(observation,"drained process=%lu thread=%lu\n",WaitForSingleObject(owner,0),WaitForSingleObject(thread_handle,0));
    if(all_hook)UnhookWinEvent(all_hook);
    if(filtered_hook)UnhookWinEvent(filtered_hook);
    if(owner)CloseHandle(owner);if(thread_handle)CloseHandle(thread_handle);
    fclose(observation);observation=NULL;return error;
}
static BOOL WINAPI control(DWORD event)
{
    if(event==CTRL_CLOSE_EVENT) {
        SetEvent(close_seen);
        Sleep(300); /* Test stimulus only: delay actual carrier destruction. */
    }
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
    if(argc==2 && !wcscmp(argv[1],L"--empty-owner")) {
        HWND window=GetConsoleWindow();
        if(!window || IsWindowVisible(window))return 16;
        error=observed_close();
        if(!error && (live_after_close || IsWindow(window) || GetConsoleWindow()))error=17;
        if(!error && ntvwm_console_close()!=ERROR_INVALID_HANDLE)error=18;
    } else if(argc==2 && !wcscmp(argv[1],L"--owner")) {
        HWND window=GetConsoleWindow();
        if(!window || IsWindowVisible(window))return 4;
        ready=CreateEventW(&security,TRUE,FALSE,NULL);
        close_seen=CreateEventW(&security,TRUE,FALSE,NULL);
        if(!ready || !close_seen)return 5;
        swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" --client %llu %llu",image,
            (unsigned long long)(ULONG_PTR)ready,(unsigned long long)(ULONG_PTR)close_seen);
        if(!CreateProcessW(image,command,NULL,NULL,TRUE,0,NULL,NULL,&startup,&process))return 6;
        if(WaitForSingleObject(ready,5000)!=WAIT_OBJECT_0)error=7;
        if(!error)error=observed_close();
        if(!error && (live_after_close || IsWindow(window) || GetConsoleWindow()))error=8;
        if(!error && WaitForSingleObject(close_seen,1000)!=WAIT_OBJECT_0)error=9;
        if(!error && WaitForSingleObject(process.hProcess,5000)!=WAIT_OBJECT_0)error=10;
        if(!error && ntvwm_console_close()!=ERROR_INVALID_HANDLE)error=11;
    } else if(argc==1) {
        swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" --owner",image);
        startup.dwFlags=STARTF_USESHOWWINDOW;startup.wShowWindow=SW_HIDE;
        if(!CreateProcessW(image,command,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&startup,&process))return 12;
        if(WaitForSingleObject(process.hProcess,15000)!=WAIT_OBJECT_0)error=13;
        else if(!GetExitCodeProcess(process.hProcess,&code))error=14;
        else error=code;
        printf("NTVWM-CLOSE error=%lu target-image-and-normal-console-close=%s\n",error,error ? "FAIL" : "PASS");
        if(!error) {
            CloseHandle(process.hProcess);CloseHandle(process.hThread);
            ZeroMemory(&process,sizeof(process));
            swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" --empty-owner",image);
            if(!CreateProcessW(image,command,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&startup,&process))error=19;
            else if(WaitForSingleObject(process.hProcess,15000)!=WAIT_OBJECT_0)error=20;
            else if(!GetExitCodeProcess(process.hProcess,&code))error=21;
            else error=code;
            printf("NTVWM-CLOSE error=%lu empty-console-close=%s\n",error,error ? "FAIL" : "PASS");
        }
    } else return 15;
    if(process.hProcess) {
        if(WaitForSingleObject(process.hProcess,0)==WAIT_TIMEOUT)TerminateProcess(process.hProcess,99);
        CloseHandle(process.hProcess);CloseHandle(process.hThread);
    }
    if(ready)CloseHandle(ready);if(close_seen)CloseHandle(close_seen);
    return (int)error;
}
