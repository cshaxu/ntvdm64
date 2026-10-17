/* Real same-width DLL terminal disposal on an owned isolated desktop. */
#define NTW16S_STATIC
#include "ntw16s-dll/ntw16s.h"
#include <commctrl.h>
#include <stdio.h>
#pragma comment(lib,"comctl32.lib")
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define CHECK(x) do{if(!(x)){fprintf(stderr,"TERMINAL FAIL line=%d error=%lu: %s\n",__LINE__,GetLastError(),#x);ExitProcess(1);}}while(0)
typedef ntw16s_creation_scope *(__cdecl *begin_fn)(BOOL);
typedef BOOL (__cdecl *end_fn)(ntw16s_creation_scope *,ntw16s_creation_result *);
typedef BOOL (__cdecl *window_fn)(HWND);
static begin_fn begin_scope;
static end_fn end_scope;
static window_fn terminal,attached;
static HWND windows[8];
static HDESK desktop;
static BOOL in_terminal,forced;
static unsigned terminal_calls;

static LRESULT CALLBACK procedure(HWND w,UINT m,WPARAM p,LPARAM l)
{
    if(in_terminal && (m==WM_SETFONT || m==WM_SIZE || m==WM_THEMECHANGED ||
            m==WM_COMMAND || m==WM_HSCROLL || m==WM_VSCROLL))CHECK(FALSE);
    if(m==WM_APP+3){
        CHECK(!in_terminal);in_terminal=TRUE;
        CHECK(terminal(w));in_terminal=FALSE;
        CHECK(!attached(w));return 0x4473;
    }
    return DefWindowProcW(w,m,p,l);
}
static LRESULT CALLBACK returned(int code,WPARAM p,LPARAM l)
{
    const CWPRETSTRUCT *message=(const CWPRETSTRUCT *)l;
    LRESULT result=CallNextHookEx(NULL,code,p,l);unsigned i;
    if(code<0 || !message || message->message!=WM_NCDESTROY)return result;
    for(i=0;i<ARRAYSIZE(windows);++i)if(windows[i] && windows[i]==message->hwnd){
        WNDPROC previous=(WNDPROC)GetWindowLongPtrW(message->hwnd,GWLP_WNDPROC);
        CHECK(!in_terminal);in_terminal=TRUE;SetLastError(0x44730001);
        CHECK(terminal(message->hwnd) && GetLastError()==0x44730001);
        CHECK(!attached(message->hwnd));
        CHECK(terminal(message->hwnd));in_terminal=FALSE;
        if(forced)CHECK((WNDPROC)GetWindowLongPtrW(message->hwnd,GWLP_WNDPROC)==previous);
        windows[i]=NULL;++terminal_calls;break;
    }
    return result;
}
static DWORD WINAPI wrong_thread(void *parameter)
{
    CHECK(SetThreadDesktop(desktop));
    CHECK(!terminal((HWND)parameter) && GetLastError()==ERROR_INVALID_THREAD_ID);
    return 0;
}
static HWND matrix(void)
{
    static const WCHAR *classes[]={L"BUTTON",L"STATIC",L"EDIT",L"LISTBOX",L"COMBOBOX",L"SCROLLBAR",L"SysLink"};
    static const DWORD styles[]={BS_PUSHBUTTON,SS_LEFT,ES_MULTILINE|WS_BORDER|WS_VSCROLL,
        WS_VSCROLL,WS_VSCROLL|CBS_DROPDOWNLIST,SBS_VERT,0};
    ntw16s_creation_scope *scope=begin_scope(TRUE);ntw16s_creation_result result;
    unsigned i;CHECK(scope);
    windows[0]=CreateWindowW(L"Ntw16sTerminal",L"Terminal",WS_OVERLAPPEDWINDOW|WS_HSCROLL|WS_VSCROLL,
        0,0,500,350,NULL,NULL,GetModuleHandleW(NULL),NULL);CHECK(windows[0]);
    for(i=0;i<ARRAYSIZE(classes);++i){
        windows[i+1]=CreateWindowW(classes[i],i==6?L"<a href=\"test\">Link</a>":L"Item",
            WS_CHILD|styles[i],10,10+(int)i*35,180,100,windows[0],
            (HMENU)(UINT_PTR)(i+1),GetModuleHandleW(NULL),NULL);
        CHECK(windows[i+1]);
    }
    /* Native controls may create additional implementation-private windows.
     * Require every explicit supported window, not a private child count. */
    CHECK(end_scope(scope,&result) && !result.failure && result.installed>=ARRAYSIZE(windows));
    for(i=0;i<ARRAYSIZE(windows);++i)CHECK(attached(windows[i]));
    return windows[0];
}
static int worker(const WCHAR *path)
{
    HMODULE module;WNDCLASSW cls={0};INITCOMMONCONTROLSEX cc={sizeof(cc),ICC_LINK_CLASS};
    HHOOK observer;unsigned round,i;DWORD baseline=0,after=0;
    CHECK(InitCommonControlsEx(&cc));
    desktop=GetThreadDesktop(GetCurrentThreadId());
    module=LoadLibraryExW(path,NULL,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);CHECK(module);
    begin_scope=(begin_fn)(void *)GetProcAddress(module,"Ntw16sCreationBegin");
    end_scope=(end_fn)(void *)GetProcAddress(module,"Ntw16sCreationEnd");
    terminal=(window_fn)(void *)GetProcAddress(module,"Ntw16sWindowTerminal");
    attached=(window_fn)(void *)GetProcAddress(module,"Ntw16sWindowAttached");
    CHECK(begin_scope && end_scope && terminal && attached);
    cls.hInstance=GetModuleHandleW(NULL);cls.lpfnWndProc=procedure;cls.lpszClassName=L"Ntw16sTerminal";
    CHECK(RegisterClassW(&cls));
    observer=SetWindowsHookExW(WH_CALLWNDPROCRET,returned,NULL,GetCurrentThreadId());CHECK(observer);
    for(round=0;round<7;++round){
        HWND root=matrix();unsigned before=terminal_calls;
        forced=round%3==1;
        if(round==0){HANDLE thread=CreateThread(NULL,0,wrong_thread,root,0,NULL);DWORD code;
            CHECK(thread && WaitForSingleObject(thread,10000)==WAIT_OBJECT_0);
            CHECK(GetExitCodeThread(thread,&code) && !code);CloseHandle(thread);CHECK(attached(root));}
        if(forced)for(i=0;i<ARRAYSIZE(windows);++i){
            WNDPROC native=(WNDPROC)GetClassLongPtrW(windows[i],GCLP_WNDPROC);CHECK(native);
            CHECK(SetWindowLongPtrW(windows[i],GWLP_WNDPROC,(LONG_PTR)native));
        }
        if(round%3==2)CHECK(SendMessageW(root,WM_APP+3,0,0)==0x4473);
        CHECK(DestroyWindow(root));
        CHECK(terminal_calls-before==ARRAYSIZE(windows));
        for(i=0;i<ARRAYSIZE(windows);++i)CHECK(!windows[i]);
        GdiFlush();after=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);
        if(!round)baseline=after;else CHECK(after==baseline);
    }
    CHECK(UnhookWindowsHookEx(observer));
    {ntw16s_creation_scope *scope=begin_scope(TRUE);ntw16s_creation_result result;HWND root;
        CHECK(scope);root=CreateWindowW(cls.lpszClassName,L"Unload",WS_OVERLAPPEDWINDOW,
            0,0,300,180,NULL,NULL,cls.hInstance,NULL);
        CHECK(root && end_scope(scope,&result) && !result.failure && attached(root));
        CHECK(SetWindowLongPtrW(root,GWLP_WNDPROC,(LONG_PTR)procedure));
        CHECK(terminal(root) && terminal(root) && !attached(root));
        CHECK(FreeLibrary(module) && !GetModuleHandleW(path));
        CHECK(DestroyWindow(root));
    }
    CHECK(UnregisterClassW(cls.lpszClassName,cls.hInstance));
    fprintf(stderr,"NTW16S_TERMINAL errors=0 rounds=7 callbacks=%u families=7 neutralized=1 reentrant=1 foreign_thread=1 repeat=1 unload_before_destroy=1 gdi=%lu/%lu\n",terminal_calls,baseline,after);
    return 0;
}
int wmain(int argc,WCHAR **argv)
{
    WCHAR name[80],image[MAX_PATH],line[MAX_PATH*2+40];HDESK isolated;
    STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION process={0};DWORD code;
    CHECK(argc==2 || (argc==3 && !wcscmp(argv[2],L"--worker")));
    SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX|SEM_NOOPENFILEERRORBOX);
    CHECK(SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2));
    if(argc==3)return worker(argv[1]);
    swprintf_s(name,ARRAYSIZE(name),L"Ntw16sTerminal-%lu",GetCurrentProcessId());
    isolated=CreateDesktopW(name,NULL,NULL,0,DESKTOP_CREATEWINDOW|DESKTOP_READOBJECTS|
        DESKTOP_WRITEOBJECTS|DESKTOP_ENUMERATE|DESKTOP_HOOKCONTROL,NULL);CHECK(isolated);
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));
    swprintf_s(line,ARRAYSIZE(line),L"\"%ls\" \"%ls\" --worker",image,argv[1]);
    startup.lpDesktop=name;startup.dwFlags=STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;startup.wShowWindow=SW_HIDE;
    startup.hStdInput=GetStdHandle(STD_INPUT_HANDLE);startup.hStdOutput=GetStdHandle(STD_OUTPUT_HANDLE);startup.hStdError=GetStdHandle(STD_ERROR_HANDLE);
    CHECK(CreateProcessW(image,line,NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,NULL,&startup,&process));
    if(WaitForSingleObject(process.hProcess,90000)!=WAIT_OBJECT_0){TerminateProcess(process.hProcess,ERROR_TIMEOUT);CHECK(FALSE);}
    CHECK(GetExitCodeProcess(process.hProcess,&code));
    CHECK(CloseHandle(process.hThread) && CloseHandle(process.hProcess) && CloseDesktop(isolated));
    return (int)code;
}
