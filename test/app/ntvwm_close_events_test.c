#include <windows.h>
#include <stdio.h>
static HWND WINAPI test_window(void);
static HWINEVENTHOOK WINAPI test_hook(DWORD,DWORD,HMODULE,WINEVENTPROC,DWORD,DWORD,DWORD);
static BOOL WINAPI test_free(void);
static BOOL WINAPI test_is_window(HWND);
static BOOL WINAPI test_post(HWND,UINT,WPARAM,LPARAM);
static DWORD WINAPI test_wait(DWORD,const HANDLE *,DWORD,DWORD,DWORD);
static BOOL WINAPI test_peek(MSG *,HWND,UINT,UINT,UINT);
static BOOL WINAPI test_unhook(HWINEVENTHOOK);
#define GetConsoleWindow test_window
#define SetWinEventHook test_hook
#define FreeConsole test_free
#define IsWindow test_is_window
#define PostMessageW test_post
#define MsgWaitForMultipleObjectsEx test_wait
#define PeekMessageW test_peek
#define UnhookWinEvent test_unhook
#include "ntvwm-exe/console_state.c"
#undef GetConsoleWindow
#undef SetWinEventHook
#undef FreeConsole
#undef IsWindow
#undef PostMessageW
#undef MsgWaitForMultipleObjectsEx
#undef PeekMessageW
#undef UnhookWinEvent

static DWORD mode,detaches,posts,waits,unhooks,violations;
static BOOL live,quit;
static WINEVENTPROC callback;
static HWND WINAPI test_window(void) { return mode==9 ? NULL : (HWND)1; }
static HWINEVENTHOOK WINAPI test_hook(DWORD first,DWORD last,HMODULE module,
    WINEVENTPROC proc,DWORD pid,DWORD tid,DWORD flags)
{
    if(first!=EVENT_OBJECT_DESTROY || last!=first || module || pid || tid ||
        flags!=WINEVENT_OUTOFCONTEXT)++violations;
    callback=proc;
    if(mode==1 || mode==2){SetLastError(mode==1 ? ERROR_ACCESS_DENIED : 0);return NULL;}
    return (HWINEVENTHOOK)2;
}
static BOOL WINAPI test_unhook(HWINEVENTHOOK hook)
{ if(hook!=(HWINEVENTHOOK)2)++violations;++unhooks;return TRUE; }
static BOOL WINAPI test_free(void)
{
    ++detaches;
    if(mode==3){SetLastError(ERROR_BUSY);return FALSE;}
    if(mode==4)live=FALSE;
    return TRUE;
}
static BOOL WINAPI test_is_window(HWND window)
{ if(window!=(HWND)1)++violations;return live; }
static BOOL WINAPI test_post(HWND window,UINT message,WPARAM wparam,LPARAM lparam)
{
    if(window!=(HWND)1 || message!=WM_CLOSE || wparam || lparam)++violations;
    ++posts;
    if(mode==5 || mode==6) {
        if(mode==6)live=FALSE;
        SetLastError(ERROR_ACCESS_DENIED);return FALSE;
    }
    return TRUE;
}
static DWORD WINAPI test_wait(DWORD count,const HANDLE *handles,DWORD timeout,DWORD mask,DWORD flags)
{
    if(count || handles || timeout!=INFINITE || mask!=QS_ALLINPUT || flags!=MWMO_INPUTAVAILABLE)++violations;
    ++waits;
    if(mode==7){SetLastError(ERROR_INVALID_HANDLE);return WAIT_FAILED;}
    if(mode==8){quit=TRUE;return WAIT_OBJECT_0;}
    if(mode==10 && waits==1) {
        callback((HWINEVENTHOOK)2,EVENT_OBJECT_DESTROY,(HWND)3,OBJID_WINDOW,0,0,0);
        callback((HWINEVENTHOOK)2,EVENT_OBJECT_DESTROY,(HWND)1,OBJID_CLIENT,0,0,0);
        callback((HWINEVENTHOOK)2,EVENT_OBJECT_DESTROY,(HWND)1,OBJID_WINDOW,1,0,0);
    } else {
        live=FALSE;
        callback((HWINEVENTHOOK)2,EVENT_OBJECT_DESTROY,(HWND)1,OBJID_WINDOW,0,0,0);
    }
    return WAIT_OBJECT_0;
}
static BOOL WINAPI test_peek(MSG *message,HWND window,UINT first,UINT last,UINT flags)
{
    (void)window;(void)first;(void)last;
    if(quit && flags==PM_REMOVE){quit=FALSE;ZeroMemory(message,sizeof(*message));message->message=WM_QUIT;message->wParam=17;return TRUE;}
    return FALSE;
}
int main(void)
{
    static const DWORD expected[]={0,ERROR_ACCESS_DENIED,ERROR_GEN_FAILURE,ERROR_BUSY,0,
        ERROR_ACCESS_DENIED,0,ERROR_INVALID_HANDLE,ERROR_OPERATION_ABORTED,ERROR_INVALID_HANDLE,0};
    ntvwm_console_guard sentinel={0};DWORD round,result;MSG message;
    for(round=0;round<16;++round)for(mode=0;mode<ARRAYSIZE(expected);++mode) {
        detaches=posts=waits=unhooks=violations=0;live=TRUE;quit=FALSE;
        event_guard=&sentinel;
        result=ntvwm_console_close();
        if(result!=expected[mode] || event_guard!=&sentinel || violations ||
            unhooks!=((mode==1 || mode==2 || mode==9) ? 0u : 1u) ||
            detaches!=((mode==1 || mode==2 || mode==9) ? 0u : 1u) ||
            posts!=((mode==1 || mode==2 || mode==3 || mode==4 || mode==9) ? 0u : 1u) ||
            waits!=((mode==0 || mode==7 || mode==8) ? 1u : mode==10 ? 2u : 0u)) {
            printf("FAIL mode=%lu result=%lu detach=%lu post=%lu wait=%lu unhook=%lu\n",
                mode,result,detaches,posts,waits,unhooks);return 1;
        }
        if(mode==8 && (!PeekMessageW(&message,NULL,WM_QUIT,WM_QUIT,PM_REMOVE) || message.wParam!=17))return 2;
    }
    event_guard=NULL;
    puts("NTVWM-CLOSE-EVENTS PASS cases=176 failure-cleanup unrelated-event quit-preserved tls-restored");
    return 0;
}
