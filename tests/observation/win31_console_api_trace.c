// Test-only failure observation in the exact private NTCON. Calls/results
// and LastError are preserved. No guest writes or product image replacement.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <intrin.h>
#include "../../src/lib-win32/process-hook/late_patch.h"
#include "../../src/lib-win32/process-hook/minhook/src/buffer.h"
#include <wchar.h>
static HANDLE output=INVALID_HANDLE_VALUE;
static CRITICAL_SECTION lock;
static BOOL (WINAPI *window_call)(HANDLE,BOOL,const SMALL_RECT *);
static BOOL (WINAPI *size_call)(HANDLE,COORD);
static BOOL (WINAPI *position_call)(HANDLE,COORD);
static BOOL (WINAPI *cursor_call)(HANDLE,const CONSOLE_CURSOR_INFO *);
static BOOL (WINAPI *cells_call)(HANDLE,const CHAR_INFO *,COORD,COORD,SMALL_RECT *);
typedef struct Before {CONSOLE_SCREEN_BUFFER_INFO info;CONSOLE_FONT_INFOEX font;BOOL valid,font_valid;} Before;
static Before inspect(HANDLE h) {
    DWORD saved=GetLastError();Before b={0};b.font.cbSize=sizeof(b.font);
    b.valid=GetConsoleScreenBufferInfo(h,&b.info);
    b.font_valid=GetCurrentConsoleFontEx(h,FALSE,&b.font);
    SetLastError(saved);return b;
}
static void failure(const char *api,DWORD error,void *caller,const Before b,
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
/* The system invokes MouseInputHook after the DLL load has returned.  Never
 * suspend other threads or take a process snapshot under the loader lock. */
static INIT_ONCE attached_once=INIT_ONCE_STATIC_INIT;
static BOOL allow_input;
static process_hook_late_patch patches[5];
static process_hook_late_patch *batch[5];
static process_hook_late_guard guard;
static BOOL CALLBACK attach_once(PINIT_ONCE once,PVOID unused,PVOID *context) {
    WCHAR image[MAX_PATH],desktop[128],path[2048];DWORD bytes,n,error=ERROR_SUCCESS,prepared=0;
    const WCHAR *name;HMODULE pinned;BOOL any=FALSE;(void)once;(void)unused;(void)context;
    n=GetModuleFileNameW(NULL,image,MAX_PATH);if(!n || n>=MAX_PATH)return TRUE;
    name=wcsrchr(image,L'\\');if(!name || _wcsicmp(name+1,L"ntcon.exe"))return TRUE;
    if(!GetUserObjectInformationW(GetThreadDesktop(GetCurrentThreadId()),UOI_NAME,desktop,sizeof(desktop),&bytes) || wcsncmp(desktop,L"NTVDMConsoleTest-",17))return TRUE;
    n=GetEnvironmentVariableW(L"WIN31_CONSOLE_API_TRACE",path,2048);if(!n){allow_input=TRUE;return TRUE;}if(n>=2048 || !wcsstr(path,L"\\build\\M0-T436\\S2\\"))return TRUE;
    output=CreateFileW(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,0,NULL);if(output==INVALID_HANDLE_VALUE)return TRUE;
    InitializeCriticalSection(&lock);
    /* The callback and its retained trampolines may outlive the input hook. */
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN|GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,(LPCWSTR)(uintptr_t)&attach_once,&pinned))goto fail;
    window_call=SetConsoleWindowInfo;size_call=SetConsoleScreenBufferSize;position_call=SetConsoleCursorPosition;cursor_call=SetConsoleCursorInfo;cells_call=WriteConsoleOutputW;
    InitializeBuffer();
    {void *slots[]={&window_call,&size_call,&position_call,&cursor_call,&cells_call};
     void *hooks[]={observe_window,observe_size,observe_position,observe_cursor,observe_cells};
     for(DWORD i=0;i<5;++i){batch[i]=&patches[i];error=process_hook_late_patch_prepare(batch[i],slots[i],hooks[i]);if(error)break;++prepared;}}
#ifdef PROCESS_HOOK_LATE_GUARD_TEST
    process_hook_late_patch_test_fail_protect_after(2);
#endif
    if(!error)error=process_hook_late_patch_install_batch(&guard,batch,5);
    /* A failed release retains its handles. Keep retrying before returning to
     * the target message loop; never unload installed trampolines. */
    while(guard.threads && guard.suspended){DWORD release=process_hook_late_guard_end(&guard);if(release)Sleep(10);}
    /* Snapshot-disposal failure after all threads resumed does not justify
     * blocking the UI thread. Keep the pinned owner's retained resources. */
    if(guard.threads || guard.snapshots){DWORD release=process_hook_late_guard_end(&guard);if(release && !error)error=release;}
    for(DWORD i=0;i<prepared;++i)if(patches[i].installed)any=TRUE;
    if(!any && error){while(prepared)process_hook_late_patch_discard(&patches[--prepared]);UninitializeBuffer();}
    {char line[100];DWORD written;int length=_snprintf_s(line,sizeof(line),_TRUNCATE,
        error?"api=ATTACH_ERROR pid=%lu error=%lu installed=%d\r\n":"api=ATTACHED pid=%lu\r\n",GetCurrentProcessId(),error,any);
     if(length>0)WriteFile(output,line,(DWORD)length,&written,NULL);}
    if(!error)allow_input=TRUE;return TRUE;
fail:
    CloseHandle(output);output=INVALID_HANDLE_VALUE;DeleteCriticalSection(&lock);return TRUE;
}
BOOL win31_console_api_on_message(void) {
    DWORD saved=GetLastError();BOOL ready=InitOnceExecuteOnce(&attached_once,attach_once,NULL,NULL) && allow_input;SetLastError(saved);return ready;
}
BOOL WINAPI DllMain(HINSTANCE module,DWORD reason,LPVOID reserved) {
    (void)reserved;
    if(reason==DLL_PROCESS_ATTACH)DisableThreadLibraryCalls(module);return TRUE;
}
