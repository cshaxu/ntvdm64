// Test-only Console text interception. Original calls/results remain unchanged.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include "../../src/nthook32-dll/detours/detours.h"
extern "C" BOOL WINAPI MvdmWriteConsoleA(HANDLE,const VOID*,DWORD,LPDWORD,LPVOID);
typedef BOOL (WINAPI *WriteFn)(HANDLE,const VOID*,DWORD,LPDWORD,LPVOID);
static WriteFn original_write=MvdmWriteConsoleA;
static HANDLE log_handle=INVALID_HANDLE_VALUE;
static CRITICAL_SECTION log_lock;
static BOOL WINAPI observed_write(HANDLE output,const VOID *buffer,DWORD length,LPDWORD written,LPVOID reserved) {
    DWORD previous=GetLastError(),bytes;char header[96];
    EnterCriticalSection(&log_lock);
    int count=_snprintf_s(header,sizeof(header),_TRUNCATE,"\r\n[console-write tick=%llu length=%lu]\r\n",GetTickCount64(),length);
    if(count>0)WriteFile(log_handle,header,(DWORD)count,&bytes,NULL);
    if(buffer&&length)WriteFile(log_handle,buffer,length,&bytes,NULL);
    LeaveCriticalSection(&log_lock);
    SetLastError(previous);
    return original_write(output,buffer,length,written,reserved);
}
static void __cdecl initialize_trace() {
    wchar_t path[32768];DWORD count=GetEnvironmentVariableW(L"WIN31_CONSOLE_TRACE",path,32768);
    if(!count)return;
    if(count>=32768 || !wcsstr(path,L"\\build\\"))ExitProcess(127);
    log_handle=CreateFileW(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,0,NULL);
    if(log_handle==INVALID_HANDLE_VALUE)ExitProcess(127);
    InitializeCriticalSection(&log_lock);
    if(DetourTransactionBegin()!=NO_ERROR || DetourUpdateThread(GetCurrentThread())!=NO_ERROR ||
       DetourAttach((PVOID*)&original_write,observed_write)!=NO_ERROR || DetourTransactionCommit()!=NO_ERROR)ExitProcess(127);
}
#pragma section(".CRT$XCU",read)
extern "C" __declspec(allocate(".CRT$XCU")) void (__cdecl *win31_console_trace_entry)()=initialize_trace;
