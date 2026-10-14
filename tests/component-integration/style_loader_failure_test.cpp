#include "nthook32-dll/hook.h"
#include <stdio.h>
#include <wchar.h>

#define CHECK(x) do{if(!(x)){fprintf(stderr,"LOADER FAIL line=%d error=%lu: %s\n",__LINE__,GetLastError(),#x);return 1;}}while(0)
int wmain(int argc,WCHAR **argv)
{
    CHECK(argc==2 || argc==3);
    DWORD expected=(DWORD)_wtoi(argv[argc-1]);
    CHECK(expected==ERROR_MOD_NOT_FOUND || expected==ERROR_BAD_EXE_FORMAT);
    if(argc==3){
        nthook_context context;BOOL found=FALSE;
        CHECK(!nthook_context_read(&context,&found) && found && context.window_style==WINDOW_STYLE_ALL);
        HWND window=CreateWindowExW(0,L"STATIC",L"must not fall back",WS_OVERLAPPEDWINDOW,0,0,320,200,NULL,NULL,NULL,NULL);
        DWORD error=GetLastError();
        CHECK(!window && error==expected);
        printf("PASS actual Hook style loader refuses creation error=%lu\n",error);return 0;
    }
    WCHAR self[MAX_PATH],line[MAX_PATH+80],name[80];
    CHECK(GetModuleFileNameW(NULL,self,MAX_PATH));
    swprintf_s(name,80,L"StyleLoader-%lu",GetCurrentProcessId());
    HDESK desktop=CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL);CHECK(desktop);
    nthook_context context;CHECK(!nthook_context_paths(&context));context.window_style=WINDOW_STYLE_ALL;
    STARTUPINFOW start={sizeof(start)};PROCESS_INFORMATION process={0};start.lpDesktop=name;
    swprintf_s(line,ARRAYSIZE(line),L"\"%ls\" child %lu",self,expected);
    CHECK(CreateProcessW(self,line,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&start,&process));
    DWORD error=nthook_install(process.hProcess,&context,NATIVE_HOOK_INTERCEPT);
    if(error){TerminateProcess(process.hProcess,error);SetLastError(error);CHECK(!error);}
    CHECK(ResumeThread(process.hThread)!=(DWORD)-1);
    if(WaitForSingleObject(process.hProcess,20000)!=WAIT_OBJECT_0){TerminateProcess(process.hProcess,ERROR_TIMEOUT);CHECK(FALSE);}
    DWORD code;CHECK(GetExitCodeProcess(process.hProcess,&code) && !code);
    CloseHandle(process.hThread);CloseHandle(process.hProcess);CHECK(CloseDesktop(desktop));return 0;
}
