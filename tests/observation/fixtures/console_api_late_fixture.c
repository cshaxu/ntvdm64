#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
typedef LRESULT (CALLBACK *HookFn)(int,WPARAM,LPARAM);
typedef struct result {BOOL value;DWORD error;} result;
static volatile LONG stop_workers,progress;
static DWORD WINAPI worker(void *unused) {(void)unused;while(!InterlockedCompareExchange(&stop_workers,0,0)){InterlockedIncrement(&progress);Sleep(1);}return 0;}
static void observe(result *r) {
 SMALL_RECT rect={0,0,1,1};COORD point={1,1},zero={0,0};CONSOLE_CURSOR_INFO ci={25,TRUE};CHAR_INFO cell={0};
 SetLastError(0x1234);r[0].value=SetConsoleWindowInfo(INVALID_HANDLE_VALUE,TRUE,&rect);r[0].error=GetLastError();
 SetLastError(0x1234);r[1].value=SetConsoleScreenBufferSize(INVALID_HANDLE_VALUE,point);r[1].error=GetLastError();
 SetLastError(0x1234);r[2].value=SetConsoleCursorPosition(INVALID_HANDLE_VALUE,point);r[2].error=GetLastError();
 SetLastError(0x1234);r[3].value=SetConsoleCursorInfo(INVALID_HANDLE_VALUE,&ci);r[3].error=GetLastError();
 SetLastError(0x1234);r[4].value=WriteConsoleOutputW(INVALID_HANDLE_VALUE,&cell,point,zero,&rect);r[4].error=GetLastError();
}
#define CHECK(x) do{if(!(x)){fprintf(stderr,"line=%d error=%lu\n",__LINE__,GetLastError());return 1;}}while(0)
int wmain(int argc,WCHAR **argv) {
 WCHAR desktop_name[96];HDESK desktop;HANDLE workers[2];HMODULE dll;HookFn hook;result before[5],after[5];LONG initial;BOOL fault;HANDLE ack;WCHAR ack_name[96];MSG msg={0};
 CHECK(argc==4);fault=!wcscmp(argv[3],L"fail");CHECK(swprintf_s(desktop_name,96,L"NTVDMConsoleTest-%lu",GetCurrentProcessId())>0);
 desktop=CreateDesktopW(desktop_name,NULL,NULL,0,DESKTOP_CREATEWINDOW|DESKTOP_READOBJECTS|DESKTOP_WRITEOBJECTS|DESKTOP_HOOKCONTROL,NULL);CHECK(desktop);CHECK(SetThreadDesktop(desktop));
 CHECK(SetEnvironmentVariableW(L"WIN31_CONSOLE_API_TRACE",argv[2]));
 for(int i=0;i<2;++i){workers[i]=CreateThread(NULL,0,worker,NULL,0,NULL);CHECK(workers[i]);}
 observe(before);dll=LoadLibraryW(argv[1]);CHECK(dll);hook=(HookFn)GetProcAddress(dll,"MouseInputHook");CHECK(hook);
  SetLastError(0x5678);hook(0,PM_NOREMOVE,0);CHECK(GetLastError()==0x5678);
 CHECK(swprintf_s(ack_name,96,L"Local\\NTVDM-Mouse-Probe-%lu-%lu",GetCurrentProcessId(),GetCurrentThreadId())>0);
 ack=CreateEventW(NULL,TRUE,FALSE,ack_name);CHECK(ack && GetLastError()!=ERROR_ALREADY_EXISTS);msg.message=WM_APP+0x5f1;msg.wParam=0;hook(HC_ACTION,PM_REMOVE,(LPARAM)&msg);
 CHECK(WaitForSingleObject(ack,100)==(fault?WAIT_TIMEOUT:WAIT_OBJECT_0));CloseHandle(ack);observe(after);
 for(int i=0;i<5;++i)CHECK(before[i].value==after[i].value && before[i].error==after[i].error);
 CHECK(FreeLibrary(dll));CHECK(GetModuleHandleW(L"mouse-hook.dll")!=NULL);observe(after);
 for(int i=0;i<5;++i)CHECK(before[i].value==after[i].value && before[i].error==after[i].error);
 initial=InterlockedCompareExchange(&progress,0,0);Sleep(100);CHECK(InterlockedCompareExchange(&progress,0,0)>initial);
 InterlockedExchange(&stop_workers,1);CHECK(WaitForMultipleObjects(2,workers,TRUE,5000)==WAIT_OBJECT_0);for(int i=0;i<2;++i)CloseHandle(workers[i]);
 puts("PASS late hooks preserve calls and errors, pin module, resume other threads");return 0;
}
