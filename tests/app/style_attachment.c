#define NTW16S_STATIC
#include "ntw16s-dll/ntw16s.h"
#include <stdio.h>
#include <stdlib.h>

#define CHECK(x) do{if(!(x)){fprintf(stderr,"ATTACH FAIL line=%d error=%lu: %s\n",__LINE__,GetLastError(),#x);ExitProcess(1);}}while(0)
typedef ntw16s_creation_scope *(__cdecl *begin_fn)(BOOL);
typedef BOOL (__cdecl *end_fn)(ntw16s_creation_scope *,ntw16s_creation_result *);
typedef BOOL (__cdecl *window_fn)(HWND);
typedef BOOL (__cdecl *bind_fn)(const ntw16s_native_api *);
typedef HRESULT (__cdecl *theme_fn)(HWND,LPCWSTR,LPCWSTR);
static theme_fn set_theme;
static HRESULT (WINAPI *native_theme)(HWND,LPCWSTR,LPCWSTR);
static HWND watched;
static BOOL reenter;
static WCHAR last_application[80],last_ids[80];
static unsigned theme_calls;
static HRESULT WINAPI observe_theme(HWND w,LPCWSTR app,LPCWSTR ids)
{
    if(w==watched){
        ++theme_calls;
        wcscpy_s(last_application,80,app?app:L"<null>");
        wcscpy_s(last_ids,80,ids?ids:L"<null>");
        if(reenter){reenter=FALSE;CHECK(SUCCEEDED(set_theme(w,L"Nested",L"NestedIds")));}
    }
    return native_theme(w,app,ids);
}
static BOOL WINAPI alternate_get(HWND w,int b,LPSCROLLINFO i){return GetScrollInfo(w,b,i);}
static begin_fn begin_scope;
static end_fn end_scope;
static window_fn attached,remove_style;
static BOOL CALLBACK detach_child(HWND window,LPARAM unused)
{(void)unused;if(attached(window))CHECK(remove_style(window));return TRUE;}
static HWND button,edit,plain,list,combo;
static LRESULT CALLBACK proc(HWND w,UINT m,WPARAM p,LPARAM l)
{
    if(m==WM_NCCREATE && ((CREATESTRUCTW *)l)->lpCreateParams==(void *)1)return FALSE;
    if(m==WM_CREATE){
        ntw16s_creation_scope *off;ntw16s_creation_result result;
        button=CreateWindowW(L"BUTTON",L"Button",WS_CHILD|BS_PUSHBUTTON,4,4,160,40,w,(HMENU)1,NULL,NULL);
        edit=CreateWindowW(L"EDIT",L"Original",WS_CHILD|WS_BORDER,4,50,240,32,w,(HMENU)2,NULL,NULL);
        CHECK(button && edit);
        list=CreateWindowW(L"LISTBOX",L"",WS_CHILD|LBS_USETABSTOPS|WS_VSCROLL,260,4,200,100,w,(HMENU)5,NULL,NULL);
        combo=CreateWindowW(L"COMBOBOX",L"",WS_CHILD|CBS_DROPDOWNLIST|WS_VSCROLL,260,130,200,150,w,(HMENU)6,NULL,NULL);
        CHECK(list && combo);
        {int stops[2]={32,64};CHECK(SendMessageW(list,LB_SETTABSTOPS,2,(LPARAM)stops));}
        off=begin_scope(FALSE);CHECK(off);
        plain=CreateWindowW(L"BUTTON",L"Off",WS_CHILD,4,90,160,40,w,(HMENU)3,NULL,NULL);
        CHECK(plain && !attached(plain));CHECK(end_scope(off,&result) && !result.failure && !result.installed);
    }
    return DefWindowProcW(w,m,p,l);
}
static HWND create(void *parameter)
{return CreateWindowW(L"StyleAttachment",L"Caption",WS_OVERLAPPEDWINDOW,20,20,600,400,NULL,NULL,GetModuleHandleW(NULL),parameter);}
typedef struct test_input {WCHAR **argv;HDESK desktop;} test_input;
static DWORD WINAPI run_test(void *parameter)
{
    test_input *input=(test_input *)parameter;WCHAR **argv=input->argv;
    HMODULE module,theme_module;WCHAR title[128];WNDCLASSW cls={0};
    bind_fn bind;ntw16s_native_api api;
    ntw16s_creation_scope *scope,*nested;ntw16s_creation_result result;HWND w,late;DWORD before,after;
    printf("ATTACH start tick=%llu\n",GetTickCount64());fflush(stdout);
    CHECK(SetThreadDesktop(input->desktop));
    module=LoadLibraryExW(argv[1],NULL,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);CHECK(module);
    begin_scope=(begin_fn)(void *)GetProcAddress(module,"Ntw16sCreationBegin");
    end_scope=(end_fn)(void *)GetProcAddress(module,"Ntw16sCreationEnd");
    attached=(window_fn)(void *)GetProcAddress(module,"Ntw16sWindowAttached");
    remove_style=(window_fn)(void *)GetProcAddress(module,"Ntw16sWindowRemove");
    CHECK(begin_scope && end_scope && attached && remove_style);
    bind=(bind_fn)(void *)GetProcAddress(module,"Ntw16sBindNativeApi");
    set_theme=(theme_fn)(void *)GetProcAddress(module,"Ntw16sSetWindowTheme");
    theme_module=LoadLibraryExW(L"uxtheme.dll",NULL,LOAD_LIBRARY_SEARCH_SYSTEM32);CHECK(theme_module);
    native_theme=(HRESULT (WINAPI *)(HWND,LPCWSTR,LPCWSTR))(void *)GetProcAddress(theme_module,"SetWindowTheme");
    CHECK(bind && set_theme && native_theme);
    CHECK(!bind(NULL) && GetLastError()==ERROR_INVALID_PARAMETER);
    api.size=sizeof(api);api.get_scroll_info=GetScrollInfo;api.set_scroll_info=SetScrollInfo;api.set_window_theme=observe_theme;
    CHECK(bind(&api) && bind(&api));api.get_scroll_info=alternate_get;
    CHECK(!bind(&api) && GetLastError()==ERROR_INVALID_STATE);
    printf("ATTACH bound tick=%llu\n",GetTickCount64());fflush(stdout);
    cls.hInstance=GetModuleHandleW(NULL);cls.lpfnWndProc=proc;cls.lpszClassName=L"StyleAttachment";CHECK(RegisterClassW(&cls));
    scope=begin_scope(TRUE);CHECK(scope);nested=begin_scope(FALSE);CHECK(nested);
    CHECK(!end_scope(scope,NULL) && GetLastError()==ERROR_INVALID_STATE);CHECK(end_scope(nested,NULL));
    w=create(NULL);CHECK(w);CHECK(end_scope(scope,&result));
    printf("ATTACH first created tick=%llu\n",GetTickCount64());fflush(stdout);
    if(result.failure)fprintf(stderr,"scope failure=%lu installed=%lu unsupported=%lu\n",result.failure,result.installed,result.unsupported);
    CHECK(!result.failure && result.installed==5 && attached(w) && attached(button) && attached(edit) && attached(list) && attached(combo) && !attached(plain));
    CHECK(GetWindowTextW(w,title,128) && !wcscmp(title,L"Caption"));
    CHECK(SendMessageW(edit,WM_GETTEXT,128,(LPARAM)title) && !wcscmp(title,L"Original"));
    watched=list;theme_calls=0;
    CHECK(SUCCEEDED(set_theme(list,L"Application",L"Identifiers")));
    CHECK(theme_calls && !last_application[0] && !last_ids[0]);
    reenter=TRUE;CHECK(SUCCEEDED(set_theme(list,L"Outer",L"OuterIds")));
    CHECK(remove_style(list) && !attached(list));
    CHECK(!wcscmp(last_application,L"Nested") && !wcscmp(last_ids,L"NestedIds"));watched=NULL;
    scope=begin_scope(TRUE);CHECK(scope);
    late=CreateWindowW(L"BUTTON",L"Later",WS_CHILD,4,130,160,40,w,(HMENU)4,NULL,NULL);CHECK(late);
    CHECK(end_scope(scope,&result) && !result.failure && attached(late));
    CHECK(remove_style(w) && !attached(w));CHECK(GetWindowTextW(w,title,128) && !wcscmp(title,L"Caption"));
    CHECK(DestroyWindow(w));
    before=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);
    printf("ATTACH lifetime loop tick=%llu\n",GetTickCount64());fflush(stdout);
    {int i;for(i=0;i<30;++i){
        scope=begin_scope(TRUE);CHECK(scope);w=create(NULL);CHECK(w);
        CHECK(end_scope(scope,&result) && !result.failure && attached(w));CHECK(DestroyWindow(w));
        if(i%10==9){printf("ATTACH lifetime=%d tick=%llu\n",i+1,GetTickCount64());fflush(stdout);}
    }}
    after=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);CHECK(after==before);
    scope=begin_scope(TRUE);CHECK(scope);CHECK(!create((void *)1));CHECK(end_scope(scope,&result));
    CHECK(!result.failure && !result.installed);
    {DWORD flags[2]={WS_MINIMIZE,WS_MAXIMIZE};int i;
        for(i=0;i<2;++i){
            WINDOWPLACEMENT before_place={sizeof(before_place)},after_place={sizeof(after_place)};
            printf("INITIAL STATE begin=%d tick=%llu\n",i,GetTickCount64());fflush(stdout);
            scope=begin_scope(TRUE);CHECK(scope);
            w=CreateWindowW(L"StyleAttachment",L"Initial state",WS_OVERLAPPEDWINDOW|flags[i],
                20,20,400,300,NULL,NULL,GetModuleHandleW(NULL),NULL);CHECK(w);
            CHECK(end_scope(scope,&result) && !result.failure && attached(w));
            printf("INITIAL STATE created=%d tick=%llu\n",i,GetTickCount64());fflush(stdout);
            CHECK(i?IsZoomed(w):IsIconic(w));CHECK(GetWindowPlacement(w,&before_place));
            ShowWindow(w,SW_RESTORE);CHECK(!IsIconic(w) && !IsZoomed(w) && attached(w));
            printf("INITIAL STATE restored=%d tick=%llu\n",i,GetTickCount64());fflush(stdout);
            CHECK(GetWindowPlacement(w,&after_place));
            CHECK(EqualRect(&before_place.rcNormalPosition,&after_place.rcNormalPosition));
            CHECK(GetWindowTextW(w,title,128) && !wcscmp(title,L"Initial state"));
            CHECK(DestroyWindow(w));
        }
    }
    scope=begin_scope(FALSE);CHECK(scope);w=create(NULL);CHECK(w);
    CHECK(end_scope(scope,&result) && !result.failure && !result.installed && !attached(w));CHECK(DestroyWindow(w));
    scope=begin_scope(TRUE);CHECK(scope);w=create(NULL);CHECK(w);
    late=CreateWindowW(L"COMBOBOX",L"",WS_CHILD|CBS_DROPDOWNLIST|CBS_OWNERDRAWFIXED,
        4,250,200,100,w,(HMENU)8,NULL,NULL);
    CHECK(late && !attached(late));
    CHECK(end_scope(scope,&result) && !result.failure);
    CHECK(EnumChildWindows(w,detach_child,0));CHECK(remove_style(w));
    CHECK(FreeLibrary(module) && !GetModuleHandleW(argv[1]));
    /* A successful remove must leave no DLL callback on still-live controls,
     * including internal combo edits and inactive captured theme state. */
    CHECK(DestroyWindow(w));
    CHECK(UnregisterClassW(cls.lpszClassName,cls.hInstance));CHECK(FreeLibrary(theme_module));
    puts("ATTACH PASS explicit removal permits DLL unload before live-window destruction");
    printf("ATTACH PASS actual DLL, native API binding, theme reentry/restore, nested Off, controls in WM_CREATE, later children, detach, failure and 30 lifetimes GDI=%lu/%lu\n",before,after);
    return 0;
}
int wmain(int argc,WCHAR **argv)
{
    test_input input;WCHAR name[80],image[MAX_PATH],line[MAX_PATH*2+40];DWORD code;
    STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION process={0};
    CHECK(argc==2 || (argc==3 && !wcscmp(argv[2],L"--worker")));
    SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    if(argc==3){input.argv=argv;input.desktop=GetThreadDesktop(GetCurrentThreadId());return (int)run_test(&input);}
    swprintf_s(name,80,L"Ntw16sAttachment-%lu",GetCurrentProcessId());input.argv=argv;
    input.desktop=CreateDesktopW(name,NULL,NULL,0,DESKTOP_CREATEWINDOW|DESKTOP_READOBJECTS|DESKTOP_WRITEOBJECTS|DESKTOP_ENUMERATE|DESKTOP_HOOKCONTROL,NULL);
    CHECK(input.desktop);
    /* USER/UxTheme can retain auxiliary GUI threads beyond our test thread.
     * An owned child process bounds their lifetime; the parent never joins
     * that desktop and closes it only after the entire child has retired. */
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));
    swprintf_s(line,ARRAYSIZE(line),L"\"%ls\" \"%ls\" --worker",image,argv[1]);
    startup.lpDesktop=name;startup.dwFlags=STARTF_USESTDHANDLES;
    startup.hStdInput=GetStdHandle(STD_INPUT_HANDLE);startup.hStdOutput=GetStdHandle(STD_OUTPUT_HANDLE);startup.hStdError=GetStdHandle(STD_ERROR_HANDLE);
    CHECK(CreateProcessW(image,line,NULL,NULL,TRUE,0,NULL,NULL,&startup,&process));
    if(WaitForSingleObject(process.hProcess,55000)!=WAIT_OBJECT_0){TerminateProcess(process.hProcess,ERROR_TIMEOUT);CHECK(FALSE);}
    CHECK(GetExitCodeProcess(process.hProcess,&code));
    CHECK(CloseHandle(process.hThread) && CloseHandle(process.hProcess) && CloseDesktop(input.desktop));
    printf("ATTACH child exit=%08lx desktop closed\n",code);return (int)code;
}
