/* Actual DLL/title subclass behavior; no renderer or foreground desktop. */
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"title FAIL %d error=%lu: %s\n",__LINE__,GetLastError(),#x); ExitProcess(1); } } while(0)
typedef BOOL (__cdecl *marker_fn)(HWND);
static marker_fn install,remove_marker;
static int action;
static BOOL unicode_window;
static LRESULT CALLBACK procedure(HWND w,UINT m,WPARAM a,LPARAM b)
{
    if(m==WM_SETTEXT && action) {
        int pending=action;action=0;
        if(pending==1) { SetWindowTextW(w,L"inner");return TRUE; }
        if(pending==2) { CHECK(remove_marker(w)); }
        if(pending==3) { CHECK(DestroyWindow(w));return FALSE; }
        if(pending==4)return FALSE;
        if(pending==5)return TRUE; /* claims success without accepting text */
    }
    return unicode_window ? DefWindowProcW(w,m,a,b) : DefWindowProcA(w,m,a,b);
}
static void caption(HWND w,PCWSTR expected)
{ WCHAR text[256];CHECK(GetWindowTextW(w,text,256)>=0);if(wcscmp(text,expected))fwprintf(stderr,L"actual=[%s] expected=[%s]\n",text,expected);CHECK(!wcscmp(text,expected)); }
static DWORD WINAPI foreign_thread(void *window)
{ CHECK(!install((HWND)window) && GetLastError()==ERROR_INVALID_THREAD_ID);CHECK(!remove_marker((HWND)window));return 0; }
static void cases(BOOL wide)
{
    HWND w;HANDLE thread;DWORD code;WCHAR text[256];WNDCLASSW wcw={0};WNDCLASSA wca={0};
    unicode_window=wide;
    if(wide) { wcw.lpfnWndProc=procedure;wcw.lpszClassName=L"TitleWide";CHECK(RegisterClassW(&wcw)); }
    else { wca.lpfnWndProc=procedure;wca.lpszClassName="TitleAnsi";CHECK(RegisterClassA(&wca)); }
    w=wide ? CreateWindowW(L"TitleWide",L"initial",WS_OVERLAPPEDWINDOW,0,0,200,100,NULL,NULL,NULL,NULL) :
        CreateWindowA("TitleAnsi","initial",WS_OVERLAPPEDWINDOW,0,0,200,100,NULL,NULL,NULL,NULL);
    CHECK(w && install(w));caption(w,L"(Win16 Style) initial");
    CHECK(install(w));caption(w,L"(Win16 Style) initial");
    CHECK(SetWindowTextA(w,"ANSI update"));caption(w,L"(Win16 Style) ANSI update");
    CHECK(SetWindowTextW(w,L"wide update"));caption(w,L"(Win16 Style) wide update");
    CHECK(GetWindowTextW(w,text,256));CHECK(SetWindowTextW(w,text));caption(w,L"(Win16 Style) wide update");
    CHECK(SetWindowTextW(w,L"(Win16 Style) user text"));caption(w,L"(Win16 Style) (Win16 Style) user text");
    CHECK(SetWindowTextW(w,L""));caption(w,L"(Win16 Style) ");
    thread=CreateThread(NULL,0,foreign_thread,w,0,NULL);CHECK(thread);
    CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0 && GetExitCodeThread(thread,&code) && !code);CloseHandle(thread);
    action=1;CHECK(SetWindowTextW(w,L"outer"));caption(w,L"(Win16 Style) inner");
    action=4;CHECK(!SendMessageW(w,WM_SETTEXT,0,(LPARAM)L"rejected"));caption(w,L"(Win16 Style) inner");
    CHECK(remove_marker(w));caption(w,L"inner");CHECK(remove_marker(w));
    CHECK(install(w));action=2;CHECK(SetWindowTextW(w,L"removed during set"));caption(w,L"removed during set");
    CHECK(install(w));action=3;SendMessageW(w,WM_SETTEXT,0,(LPARAM)L"destroy during set");CHECK(!IsWindow(w));
    w=wide ? CreateWindowW(L"TitleWide",L"plain",WS_OVERLAPPEDWINDOW,0,0,200,100,NULL,NULL,NULL,NULL) :
        CreateWindowA("TitleAnsi","plain",WS_OVERLAPPEDWINDOW,0,0,200,100,NULL,NULL,NULL,NULL);
    CHECK(w);action=4;CHECK(!install(w));caption(w,L"plain");
    action=5;CHECK(!install(w));caption(w,L"plain");
    action=3;CHECK(!install(w));CHECK(!IsWindow(w));
    if(wide)CHECK(UnregisterClassW(L"TitleWide",NULL));else CHECK(UnregisterClassA("TitleAnsi",NULL));
    printf("PASS %s titles: update, round-trip, explicit prefix, empty, wrong thread, reentrant set/remove/destroy, rejection\n",wide?"W":"A");
}
int wmain(int argc,WCHAR **argv)
{
    HMODULE module;HDESK desktop,previous;WCHAR name[80];
    CHECK(argc==2);swprintf_s(name,80,L"TitleMarker-%lu",GetCurrentProcessId());
    previous=GetThreadDesktop(GetCurrentThreadId());
    desktop=CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL);CHECK(desktop && SetThreadDesktop(desktop));
    module=LoadLibraryExW(argv[1],NULL,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);CHECK(module);
    install=(marker_fn)(void *)GetProcAddress(module,"Ntw16sTitleInstall");
    remove_marker=(marker_fn)(void *)GetProcAddress(module,"Ntw16sTitleRemove");CHECK(install && remove_marker);
    CHECK(!install(NULL));cases(TRUE);cases(FALSE);CHECK(FreeLibrary(module));
    CHECK(SetThreadDesktop(previous) && CloseDesktop(desktop));return 0;
}
