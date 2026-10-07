// Test-only failure observation in the exact private NTCON. Calls/results
// and LastError are preserved. No guest writes or product image replacement.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <intrin.h>
#include "../../src/nthook32-dll/detours/detours.h"
static HANDLE output=INVALID_HANDLE_VALUE;
static CRITICAL_SECTION lock;
static decltype(&SetConsoleWindowInfo) window_call=SetConsoleWindowInfo;
static decltype(&SetConsoleScreenBufferSize) size_call=SetConsoleScreenBufferSize;
static decltype(&SetConsoleCursorPosition) position_call=SetConsoleCursorPosition;
static decltype(&SetConsoleCursorInfo) cursor_call=SetConsoleCursorInfo;
static decltype(&WriteConsoleOutputW) cells_call=WriteConsoleOutputW;
struct Before {CONSOLE_SCREEN_BUFFER_INFO info;CONSOLE_FONT_INFOEX font;BOOL valid,font_valid;};
static Before inspect(HANDLE h) {
    DWORD saved=GetLastError();Before b={};b.font.cbSize=sizeof(b.font);
    b.valid=GetConsoleScreenBufferInfo(h,&b.info);
    b.font_valid=GetCurrentConsoleFontEx(h,FALSE,&b.font);
    SetLastError(saved);return b;
}
static void failure(const char *api,DWORD error,void *caller,const Before &b,
    int a,int c,int d,int e) {
    if(output==INVALID_HANDLE_VALUE)return;
    char line[512];DWORD written;
    int count=_snprintf_s(line,sizeof(line),_TRUNCATE,
        "tick=%llu api=%s error=%lu caller-rva=%llx request=%d,%d,%d,%d info-valid=%d buffer=%d,%d viewport=%d,%d,%d,%d maximum=%d,%d font-valid=%d font=%d,%d\r\n",
        GetTickCount64(),api,error,(unsigned long long)((uintptr_t)caller-(uintptr_t)GetModuleHandleW(NULL)),
        a,c,d,e,b.valid,b.info.dwSize.X,b.info.dwSize.Y,b.info.srWindow.Left,b.info.srWindow.Top,
        b.info.srWindow.Right,b.info.srWindow.Bottom,b.info.dwMaximumWindowSize.X,b.info.dwMaximumWindowSize.Y,
        b.font_valid,b.font.dwFontSize.X,b.font.dwFontSize.Y);
    EnterCriticalSection(&lock);
    if(count>0)WriteFile(output,line,(DWORD)count,&written,NULL);
    LeaveCriticalSection(&lock);
}
static BOOL WINAPI observe_window(HANDLE h,BOOL absolute,const SMALL_RECT *r) {
    Before b=inspect(h);BOOL ok=window_call(h,absolute,r);DWORD error=GetLastError();
    if(!ok)failure("SetConsoleWindowInfo",error,_ReturnAddress(),b,r?r->Left:-1,r?r->Top:-1,r?r->Right:-1,r?r->Bottom:-1);
    SetLastError(error);return ok;
}
static BOOL WINAPI observe_size(HANDLE h,COORD s) {
    Before b=inspect(h);BOOL ok=size_call(h,s);DWORD error=GetLastError();
    failure(ok?"SetConsoleScreenBufferSize:OK":"SetConsoleScreenBufferSize",ok?0:error,_ReturnAddress(),b,s.X,s.Y,0,0);
    SetLastError(error);return ok;
}
static BOOL WINAPI observe_position(HANDLE h,COORD s) {
    Before b=inspect(h);BOOL ok=position_call(h,s);DWORD error=GetLastError();
    if(!ok)failure("SetConsoleCursorPosition",error,_ReturnAddress(),b,s.X,s.Y,0,0);
    SetLastError(error);return ok;
}
static BOOL WINAPI observe_cursor(HANDLE h,const CONSOLE_CURSOR_INFO *s) {
    Before b=inspect(h);BOOL ok=cursor_call(h,s);DWORD error=GetLastError();
    if(!ok)failure("SetConsoleCursorInfo",error,_ReturnAddress(),b,s?(int)s->dwSize:-1,s?s->bVisible:-1,0,0);
    SetLastError(error);return ok;
}
static BOOL WINAPI observe_cells(HANDLE h,const CHAR_INFO *p,COORD s,COORD o,SMALL_RECT *r) {
    Before b=inspect(h);BOOL ok=cells_call(h,p,s,o,r);DWORD error=GetLastError();
    if(!ok)failure("WriteConsoleOutputW",error,_ReturnAddress(),b,s.X,s.Y,o.X,o.Y);
    SetLastError(error);return ok;
}
BOOL WINAPI DllMain(HINSTANCE module,DWORD reason,LPVOID) {
    if(reason!=DLL_PROCESS_ATTACH)return TRUE;
    wchar_t image[MAX_PATH],desktop[128];DWORD bytes;
    if(!GetModuleFileNameW(NULL,image,MAX_PATH))return FALSE;
    const wchar_t *name=wcsrchr(image,L'\\');
    if(!name || _wcsicmp(name+1,L"ntcon.exe"))return TRUE; // probe's own LoadLibrary
    if(!GetUserObjectInformationW(GetThreadDesktop(GetCurrentThreadId()),UOI_NAME,
        desktop,sizeof(desktop),&bytes) || wcsncmp(desktop,L"NTVDMConsoleTest-",17))return FALSE;
    wchar_t path[2048];DWORD n=GetEnvironmentVariableW(L"WIN31_CONSOLE_API_TRACE",path,2048);
    if(!n)return TRUE;
    if(n>=2048 || !wcsstr(path,L"\\build\\M0-T436\\S2\\"))return FALSE;
    output=CreateFileW(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,0,NULL);
    if(output==INVALID_HANDLE_VALUE)return FALSE;
    InitializeCriticalSection(&lock);DisableThreadLibraryCalls(module);
    if(DetourTransactionBegin()!=NO_ERROR || DetourUpdateThread(GetCurrentThread())!=NO_ERROR)return FALSE;
    if(DetourAttach((PVOID*)&window_call,observe_window)!=NO_ERROR ||
       DetourAttach((PVOID*)&size_call,observe_size)!=NO_ERROR ||
       DetourAttach((PVOID*)&position_call,observe_position)!=NO_ERROR ||
       DetourAttach((PVOID*)&cursor_call,observe_cursor)!=NO_ERROR ||
       DetourAttach((PVOID*)&cells_call,observe_cells)!=NO_ERROR || DetourTransactionCommit()!=NO_ERROR) {
        DetourTransactionAbort();return FALSE;
    }
    // The input hook is removed after each action, but observation must remain
    // until the final guest return. Pin only this test DLL to process exit.
    HMODULE pinned;
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN|GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        (LPCWSTR)(uintptr_t)&DllMain,&pinned))return FALSE;
    char line[80];DWORD written;
    int length=_snprintf_s(line,sizeof(line),_TRUNCATE,"api=ATTACHED pid=%lu\r\n",GetCurrentProcessId());
    if(length>0)WriteFile(output,line,(DWORD)length,&written,NULL);
    return TRUE;
}
