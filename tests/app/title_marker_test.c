/* The historical marker test now verifies transparent application captions. */
#include "ntw16s-dll/attachment.h"
#include <stdio.h>
#include <wchar.h>
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"caption FAIL %d error=%lu: %s\n",__LINE__,GetLastError(),#x); ExitProcess(1); } } while(0)
typedef ntw16s_creation_scope *(__cdecl *begin_fn)(BOOL);
typedef BOOL (__cdecl *end_fn)(ntw16s_creation_scope *,ntw16s_creation_result *);
typedef BOOL (__cdecl *window_fn)(HWND);
static begin_fn begin_scope;
static end_fn end_scope;
static window_fn attached,remove_style;
static int action;
static BOOL wide_window;
static LRESULT CALLBACK procedure(HWND w,UINT m,WPARAM a,LPARAM b)
{
    if(m==WM_SETTEXT && action){
        int pending=action;action=0;
        if(pending==1){SetWindowTextW(w,L"inner");return TRUE;}
        if(pending==2){CHECK(remove_style(w));}
        if(pending==3){CHECK(DestroyWindow(w));return FALSE;}
        if(pending==4)return FALSE;
    }
    return wide_window?DefWindowProcW(w,m,a,b):DefWindowProcA(w,m,a,b);
}
static void caption(HWND w,PCWSTR expected)
{WCHAR text[256];CHECK(GetWindowTextW(w,text,256)>=0);CHECK(!wcscmp(text,expected));}
static void cases(BOOL wide)
{
    WNDCLASSW wcw={0};WNDCLASSA wca={0};HWND w;WCHAR text[256];
    ntw16s_creation_scope *scope;ntw16s_creation_result result;
    wide_window=wide;
    if(wide){wcw.lpfnWndProc=procedure;wcw.lpszClassName=L"CaptionW";CHECK(RegisterClassW(&wcw));}
    else{wca.lpfnWndProc=procedure;wca.lpszClassName="CaptionA";CHECK(RegisterClassA(&wca));}
    scope=begin_scope(TRUE);CHECK(scope);
    w=wide?CreateWindowW(L"CaptionW",L"initial",WS_OVERLAPPEDWINDOW,0,0,300,180,NULL,NULL,NULL,NULL):
        CreateWindowA("CaptionA","initial",WS_OVERLAPPEDWINDOW,0,0,300,180,NULL,NULL,NULL,NULL);
    CHECK(w && end_scope(scope,&result) && !result.failure && attached(w));caption(w,L"initial");
    CHECK(SetWindowTextA(w,"ANSI update"));caption(w,L"ANSI update");
    CHECK(SetWindowTextW(w,L"wide update"));caption(w,L"wide update");
    CHECK(GetWindowTextW(w,text,256) && SetWindowTextW(w,text));caption(w,L"wide update");
    /* Never strip matching text genuinely supplied by the application. */
    CHECK(SetWindowTextW(w,L"(Win16 Style) user text"));caption(w,L"(Win16 Style) user text");
    CHECK(SetWindowTextW(w,L""));caption(w,L"");
    action=1;CHECK(SetWindowTextW(w,L"outer"));caption(w,L"inner");
    action=4;CHECK(!SendMessageW(w,WM_SETTEXT,0,(LPARAM)L"rejected"));caption(w,L"inner");
    action=2;CHECK(SetWindowTextW(w,L"remove during set"));caption(w,L"remove during set");CHECK(!attached(w));
    action=3;SetWindowTextW(w,L"destroy during set");CHECK(!IsWindow(w));
    if(wide)CHECK(UnregisterClassW(L"CaptionW",NULL));else CHECK(UnregisterClassA("CaptionA",NULL));
    printf("PASS %s unchanged caption: initial, update, roundtrip, literal prefix, empty, reentry, reject, detach, destroy\n",wide?"W":"A");
}
int wmain(int argc,WCHAR **argv)
{
    HMODULE module;HDESK desktop,previous;WCHAR name[80];CHECK(argc==2);
    swprintf_s(name,80,L"CaptionPreservation-%lu",GetCurrentProcessId());
    previous=GetThreadDesktop(GetCurrentThreadId());
    desktop=CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL);CHECK(desktop && SetThreadDesktop(desktop));
    module=LoadLibraryExW(argv[1],NULL,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);CHECK(module);
    begin_scope=(begin_fn)(void *)GetProcAddress(module,"Ntw16sCreationBegin");
    end_scope=(end_fn)(void *)GetProcAddress(module,"Ntw16sCreationEnd");
    attached=(window_fn)(void *)GetProcAddress(module,"Ntw16sWindowAttached");
    remove_style=(window_fn)(void *)GetProcAddress(module,"Ntw16sWindowRemove");
    CHECK(begin_scope && end_scope && attached && remove_style);
    CHECK(!GetProcAddress(module,"Ntw16sTitleInstall") && !GetProcAddress(module,"Ntw16sTitleRemove"));
    cases(TRUE);cases(FALSE);CHECK(FreeLibrary(module));
    CHECK(SetThreadDesktop(previous) && CloseDesktop(desktop));return 0;
}
