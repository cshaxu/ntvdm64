#include <windows.h>
#include <stdio.h>

/* Private desktop only. Never SwitchDesktop, SendInput or show a window. */
typedef struct probe {
    HDESK desktop;
    HANDLE ready,request,reply,stop;
    BOOL ok;
    HWND window,focus,caret,capture;
    volatile LONG prepare_state;
} probe;
static unsigned errors;
static HDESK test_desktop;
static volatile LONG in_attachment,callbacks;
static LRESULT CALLBACK window_proc(HWND w,UINT m,WPARAM a,LPARAM b)
{
    if(InterlockedCompareExchange(&in_attachment,0,0))InterlockedIncrement(&callbacks);
    return DefWindowProcW(w,m,a,b);
}
#define CHECK(x) do { if(!(x)){++errors;printf("FAIL %d error=%lu\n",__LINE__,GetLastError());} } while(0)
static DWORD WINAPI peer_main(void *argument)
{
    probe *p=argument;
    HANDLE events[2]={p->stop,p->request};
    p->ok=SetThreadDesktop(p->desktop);
    if(p->ok)p->window=CreateWindowExW(0,L"S40InputFocus",L"S40 private focus",WS_OVERLAPPED,0,0,80,40,NULL,NULL,GetModuleHandleW(NULL),NULL);
    p->ok=p->ok && p->window!=NULL;SetEvent(p->ready);
    for(;;) {
        DWORD w=WaitForMultipleObjects(2,events,FALSE,5000);
        GUITHREADINFO info={sizeof(info)};
        if(w==WAIT_OBJECT_0)break;
        if(w!=WAIT_OBJECT_0+1)return 2;
        if(InterlockedExchange(&p->prepare_state,0)) {
            SetFocus(p->window);SetCapture(p->window);
            if(!CreateCaret(p->window,NULL,1,12))return 3;
        }
        p->focus=GetFocus();p->capture=GetCapture();p->ok=GetGUIThreadInfo(GetCurrentThreadId(),&info);
        p->caret=info.hwndCaret;SetEvent(p->reply);
    }
    if(p->window)DestroyWindow(p->window);
    return 0;
}
static BOOL read_peer(probe *p)
{
    return SetEvent(p->request) && WaitForSingleObject(p->reply,3000)==WAIT_OBJECT_0 && p->ok;
}
static DWORD WINAPI focus_test(void *unused)
{
    probe p={0};
    WCHAR name[80];
    HANDLE thread=NULL;
    DWORD id=0,code=0,route;
    HWND window=NULL;
    BOOL attached=FALSE,caret=FALSE;
    WNDCLASSW wc={0};
    (void)unused;
    wsprintfW(name,L"S40InputFocus-%lu",GetCurrentProcessId());
    p.desktop=CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL);
    test_desktop=p.desktop;
    CHECK(p.desktop!=NULL);if(!p.desktop)return 1;
    if(!SetThreadDesktop(p.desktop)) {
        CHECK(FALSE);return 1;
    }
    wc.lpfnWndProc=window_proc;wc.hInstance=GetModuleHandleW(NULL);wc.lpszClassName=L"S40InputFocus";
    CHECK(RegisterClassW(&wc));
    window=CreateWindowExW(0,L"S40InputFocus",L"S40 private owner",WS_OVERLAPPED,0,0,80,40,NULL,NULL,wc.hInstance,NULL);
    CHECK(window!=NULL);if(!window)goto done;
    p.ready=CreateEventW(NULL,FALSE,FALSE,NULL);p.request=CreateEventW(NULL,FALSE,FALSE,NULL);
    p.reply=CreateEventW(NULL,FALSE,FALSE,NULL);p.stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(p.ready && p.request && p.reply && p.stop);
    if(!p.ready || !p.request || !p.reply || !p.stop)goto done;
    thread=CreateThread(NULL,0,peer_main,&p,0,&id);CHECK(thread!=NULL);if(!thread)goto done;
    CHECK(WaitForSingleObject(p.ready,3000)==WAIT_OBJECT_0 && p.ok);
    SetFocus(window);CHECK(GetFocus()==window);
    caret=CreateCaret(window,NULL,1,12);CHECK(caret);
    SetCapture(window);CHECK(GetCapture()==window);
    CHECK(read_peer(&p) && p.focus==NULL && p.caret==NULL && p.capture==NULL);
    InterlockedExchange(&in_attachment,1);
    attached=AttachThreadInput(GetCurrentThreadId(),id,TRUE);
    InterlockedExchange(&in_attachment,0);CHECK(attached);
    if(!attached)goto done;
    CHECK(read_peer(&p));
    printf("attached_focus=%u attached_caret=%u\n",(unsigned)(p.focus==window),(unsigned)(p.caret==window));
    CHECK(p.focus==window && p.caret==window && p.capture==window);
    InterlockedExchange(&in_attachment,1);
    CHECK(AttachThreadInput(GetCurrentThreadId(),id,FALSE));attached=FALSE;
    InterlockedExchange(&in_attachment,0);
    CHECK(read_peer(&p));
    CHECK(p.focus==NULL && p.caret==NULL && p.capture==NULL && GetFocus()==window);
    CHECK(callbacks==0);
    /* Competing native state: neither thread is on the visible desktop. */
    for(route=0;route!=2;++route) {
    GUITHREADINFO current={sizeof(current)};
    SetFocus(window);SetCapture(window);
    CHECK(CreateCaret(window,NULL,1,12));
    InterlockedExchange(&p.prepare_state,1);
    CHECK(read_peer(&p) && p.focus==p.window && p.capture==p.window && p.caret==p.window);
    CHECK(GetFocus()==window && GetCapture()==window);
    InterlockedExchange(&in_attachment,1);
    attached=route ? AttachThreadInput(id,GetCurrentThreadId(),TRUE) :
        AttachThreadInput(GetCurrentThreadId(),id,TRUE);
    InterlockedExchange(&in_attachment,0);CHECK(attached);
    if(!attached)goto done;
    CHECK(read_peer(&p));
    CHECK(p.focus==GetFocus() && (p.focus==window || p.focus==p.window));
    CHECK(p.capture==GetCapture() && (p.capture==window || p.capture==p.window));
    CHECK(GetGUIThreadInfo(GetCurrentThreadId(),&current));
    CHECK(p.caret==current.hwndCaret && (p.caret==window || p.caret==p.window));
    printf("reverse=%lu conflict_focus_owner=%u conflict_capture_owner=%u conflict_caret_owner=%u\n",
        route,(unsigned)(p.focus==window),(unsigned)(p.capture==window),(unsigned)(p.caret==window));
    CHECK(callbacks==0);
    InterlockedExchange(&in_attachment,1);
    CHECK(AttachThreadInput(GetCurrentThreadId(),id,FALSE));attached=FALSE;
    InterlockedExchange(&in_attachment,0);
    CHECK(callbacks==0);
    }
done:
    ReleaseCapture();
    if(attached)AttachThreadInput(GetCurrentThreadId(),id,FALSE);
    if(caret)DestroyCaret();
    if(thread) {
        SetEvent(p.stop);CHECK(WaitForSingleObject(thread,3000)==WAIT_OBJECT_0);
        CHECK(GetExitCodeThread(thread,&code) && code==0);CloseHandle(thread);
    }
    if(window)DestroyWindow(window);
    CHECK(UnregisterClassW(L"S40InputFocus",wc.hInstance));
    if(p.ready)CloseHandle(p.ready);if(p.request)CloseHandle(p.request);
    if(p.reply)CloseHandle(p.reply);if(p.stop)CloseHandle(p.stop);
    /* Native USER finishes GUI-thread cleanup on normal thread exit. */
    return errors!=0;
}
int __cdecl main(void)
{
    DWORD code=0;
    HANDLE thread=CreateThread(NULL,0,focus_test,NULL,0,NULL);
    CHECK(thread!=NULL);if(!thread)return 1;
    if(WaitForSingleObject(thread,12000)!=WAIT_OBJECT_0) {
        CHECK(FALSE);return 1; /* Parent runner bounds this private process. */
    }
    CHECK(GetExitCodeThread(thread,&code) && code==0);
    CloseHandle(thread);
    if(test_desktop)CHECK(CloseDesktop(test_desktop));
    printf("WOW_NATIVE_INPUT_FOCUS errors=%u private_desktop=1 capture=1 callbacks=%ld\n",errors,callbacks);
    return errors!=0;
}
