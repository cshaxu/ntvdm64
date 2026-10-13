/* Real DLL boundary and shipped thin-demo checks, on an owned private desktop. */
#define NTW16S_STATIC
#include "ntw16s-dll/ntw16s.h"
#include <stdio.h>
#include <stdlib.h>

#define CHECK(x) do{if(!(x)){fprintf(stderr,"DLL FAIL line=%d error=%lu: %s\n",__LINE__,GetLastError(),#x);ExitProcess(1);}}while(0)
typedef BOOL (__cdecl *install_fn)(HWND,const win16_style_palette *);
typedef BOOL (__cdecl *remove_fn)(HWND);
typedef HFONT (__cdecl *font_fn)(void);
typedef void (__cdecl *release_fn)(HFONT);
typedef DWORD (__cdecl *version_fn)(void);
static install_fn install_window,install_control;
static remove_fn remove_window,remove_control;
static font_fn acquire_font;
static release_fn release_font;
static HWND parent_window;
static HFONT other_font;
static HANDLE ready,finish;
static HDESK desktop;
static LRESULT CALLBACK window_proc(HWND w,UINT m,WPARAM a,LPARAM b)
{if(m==WM_APP+7)return 456;return DefWindowProcW(w,m,a,b);}
static DWORD WINAPI other_thread(void *unused)
{
    HWND w;(void)unused;
    CHECK(SetThreadDesktop(desktop));
    CHECK(!install_window(parent_window,NULL) && GetLastError()==ERROR_INVALID_THREAD_ID);
    w=CreateWindowW(L"DllBoundary",L"second thread",WS_OVERLAPPEDWINDOW,0,0,400,240,NULL,NULL,GetModuleHandleW(NULL),NULL);
    CHECK(w && install_window(w,NULL));other_font=acquire_font();CHECK(other_font);
    CHECK(SetEvent(ready));CHECK(WaitForSingleObject(finish,10000)==WAIT_OBJECT_0);
    CHECK(remove_window(w));CHECK(DestroyWindow(w));release_font(other_font);return 0;
}
static void load_boundary(const WCHAR *path)
{
    HMODULE dll;version_fn version;HWND child;HFONT font,original;HANDLE thread;DWORD code;
    dll=LoadLibraryExW(path,NULL,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);CHECK(dll);
    version=(version_fn)(void *)GetProcAddress(dll,"Ntw16sGetAbiVersion");
    CHECK(version && version()==NTW16S_ABI_VERSION && version()!=NTW16S_ABI_VERSION+1);
    install_window=(install_fn)(void *)GetProcAddress(dll,"win16_style_install");
    remove_window=(remove_fn)(void *)GetProcAddress(dll,"win16_style_remove");
    install_control=(install_fn)(void *)GetProcAddress(dll,"win16_style_control_install");
    remove_control=(remove_fn)(void *)GetProcAddress(dll,"win16_style_control_remove");
    acquire_font=(font_fn)(void *)GetProcAddress(dll,"win16_style_font_acquire");
    release_font=(release_fn)(void *)GetProcAddress(dll,"win16_style_font_release");
    CHECK(install_window && remove_window && install_control && remove_control && acquire_font && release_font);
    parent_window=CreateWindowW(L"DllBoundary",L"DLL test",WS_OVERLAPPEDWINDOW,0,0,400,240,NULL,NULL,GetModuleHandleW(NULL),NULL);
    CHECK(parent_window && install_window(parent_window,NULL));
    CHECK(!install_window(parent_window,NULL) && GetLastError()==ERROR_ALREADY_EXISTS);
    child=CreateWindowW(L"BUTTON",L"Button",WS_CHILD|BS_PUSHBUTTON,10,10,100,32,parent_window,NULL,GetModuleHandleW(NULL),NULL);
    CHECK(child);original=(HFONT)GetStockObject(SYSTEM_FONT);SendMessageW(child,WM_SETFONT,(WPARAM)original,0);
    CHECK(install_control(child,NULL));font=acquire_font();CHECK(font);
    CHECK((HFONT)SendMessageW(child,WM_GETFONT,0,0)==font);
    ready=CreateEventW(NULL,TRUE,FALSE,NULL);finish=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(ready && finish);
    thread=CreateThread(NULL,0,other_thread,NULL,0,NULL);CHECK(thread);
    CHECK(WaitForSingleObject(ready,10000)==WAIT_OBJECT_0 && other_font!=font);
    CHECK(SetEvent(finish) && WaitForSingleObject(thread,10000)==WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(thread,&code) && code==0);CloseHandle(thread);CloseHandle(ready);CloseHandle(finish);
    CHECK(remove_control(child) && (HFONT)SendMessageW(child,WM_GETFONT,0,0)==original);
    CHECK(remove_window(parent_window));CHECK(SendMessageW(parent_window,WM_APP+7,0,0)==456);
    CHECK(DestroyWindow(parent_window));release_font(font);
    /* No remaining HWND subclasses, creation scopes, font leases or active
     * callbacks: the caller's explicit unload precondition is now satisfied. */
    CHECK(FreeLibrary(dll));CHECK(!GetModuleHandleW(path));
    puts("DLL PASS version, real imports, duplicate/foreign-thread rejection, thread isolation, font restore and unload");
}
static DWORD demo_pid;
static HWND found_controller,found_sample;
static BOOL CALLBACK find_demo(HWND w,LPARAM unused)
{
    DWORD pid;WCHAR name[64];(void)unused;GetWindowThreadProcessId(w,&pid);if(pid!=demo_pid)return TRUE;
    GetClassNameW(w,name,64);
    if(!lstrcmpW(name,L"StyleGalleryControl"))found_controller=w;
    else if(!lstrcmpW(name,L"StyleGallerySample") || !lstrcmpW(name,L"#32770"))found_sample=w;
    return TRUE;
}
static LRESULT send_owned(HWND w,UINT m,WPARAM a,LPARAM b)
{DWORD_PTR value=0;CHECK(SendMessageTimeoutW(w,m,a,b,SMTO_ABORTIFHUNG,5000,&value));return (LRESULT)value;}
static void demo_boundary(const WCHAR *path,WCHAR *desktop_name)
{
    STARTUPINFOW start={sizeof(start)};PROCESS_INFORMATION process={0};HWND list;int n;DWORD code;
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits={0};HANDLE job=CreateJobObjectW(NULL,NULL);
    limits.BasicLimitInformation.LimitFlags=JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    CHECK(job && SetInformationJobObject(job,JobObjectExtendedLimitInformation,&limits,sizeof(limits)));
    start.lpDesktop=desktop_name;
    CHECK(CreateProcessW(path,NULL,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&start,&process));demo_pid=process.dwProcessId;
    if(!AssignProcessToJobObject(job,process.hProcess)){TerminateProcess(process.hProcess,1);CHECK(FALSE);}
    CHECK(ResumeThread(process.hThread)!=(DWORD)-1);
    for(n=0;n<250 && !found_controller;++n){EnumDesktopWindows(desktop,find_demo,0);Sleep(20);}
    CHECK(found_controller);list=GetDlgItem(found_controller,10);CHECK(list);
    CHECK(send_owned(list,LB_GETCOUNT,0,0)==27);
    for(n=0;n<27;++n){
        CHECK(send_owned(list,LB_SETCURSEL,n,0)==n);send_owned(found_controller,WM_COMMAND,1,0);
        found_sample=NULL;EnumDesktopWindows(desktop,find_demo,0);CHECK(found_sample);
        if(n==0){HWND button=GetDlgItem(found_sample,1201);RECT before,after;SCROLLINFO info={sizeof(info),SIF_ALL};
            CHECK(button && GetWindowRect(button,&before));
            CHECK(SetWindowPos(found_sample,NULL,0,0,620,380,SWP_NOZORDER|SWP_NOACTIVATE));
            CHECK(GetWindowRect(button,&before));send_owned(found_sample,WM_VSCROLL,SB_LINEDOWN,0);
            CHECK(GetScrollInfo(found_sample,SB_VERT,&info) && info.nPos==24);
            CHECK(GetWindowRect(button,&after) && after.top==before.top-24);
        }
        send_owned(found_controller,WM_COMMAND,2,0);found_sample=NULL;EnumDesktopWindows(desktop,find_demo,0);CHECK(!found_sample);
    }
    send_owned(found_controller,WM_CLOSE,0,0);
    CHECK(WaitForSingleObject(process.hProcess,10000)==WAIT_OBJECT_0);
    CHECK(GetExitCodeProcess(process.hProcess,&code) && code==0);
    CloseHandle(process.hThread);CloseHandle(process.hProcess);CloseHandle(job);
    puts("DEMO PASS shipped W16Style: 27 pages, real viewport movement, close and normal process exit");
}
int wmain(int argc,WCHAR **argv)
{
    WCHAR name[64];WNDCLASSW cls={0};HDESK previous;HMODULE wrong;
    CHECK(argc==4);SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);
    CHECK(SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2));
    swprintf_s(name,64,L"Ntw16sTest-%lu",GetCurrentProcessId());previous=GetThreadDesktop(GetCurrentThreadId());
    desktop=CreateDesktopW(name,NULL,NULL,0,DESKTOP_CREATEWINDOW|DESKTOP_READOBJECTS|DESKTOP_WRITEOBJECTS|DESKTOP_ENUMERATE,NULL);
    CHECK(desktop && SetThreadDesktop(desktop));
    cls.hInstance=GetModuleHandleW(NULL);cls.lpfnWndProc=window_proc;cls.lpszClassName=L"DllBoundary";CHECK(RegisterClassW(&cls));
    load_boundary(argv[1]);load_boundary(argv[1]);
    wrong=LoadLibraryExW(argv[2],NULL,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
    CHECK(!wrong && GetLastError()==ERROR_BAD_EXE_FORMAT);
    puts("DLL PASS opposite-width rejected");
    demo_boundary(argv[3],name);
    CHECK(UnregisterClassW(cls.lpszClassName,cls.hInstance));
    CHECK(SetThreadDesktop(previous));CHECK(CloseDesktop(desktop));return 0;
}
