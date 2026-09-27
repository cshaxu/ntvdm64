/* Real frontend helper and Windows Console membership. Private desktop only. */
#include <windows.h>
#include <stdio.h>
#include "frontend-exe/native_console_backend.h"

int wmain(int argc,WCHAR **argv)
{
    WCHAR image[MAX_PATH],frontend[MAX_PATH],cwd[MAX_PATH],command[2048];
    WCHAR ready_name[96],parent_name[96],child_name[96],*slash;
    HANDLE ready=NULL,parent_done=NULL,child_done=NULL,target=NULL;
    run16_native_backend *backend=NULL;run16_native_start start={0};
    LPWCH environment=NULL;DWORD count=99,code=0,error=0,i;int failed=1;
    if(argc==5){
        HANDLE gate=OpenEventW(SYNCHRONIZE,FALSE,argv[!wcscmp(argv[1],L"--parent") ? 3 : 4]);
        if(!gate)return 10;
        if(!wcscmp(argv[1],L"--parent")){
            STARTUPINFOW si={sizeof(si)};PROCESS_INFORMATION pi={0};
            if(!GetModuleFileNameW(NULL,image,MAX_PATH))return 11;
            swprintf_s(command,2048,L"\"%ls\" --child %ls %ls %ls",image,argv[2],argv[3],argv[4]);
            if(!CreateProcessW(image,command,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi))return 12;
            CloseHandle(pi.hThread);CloseHandle(pi.hProcess);
        }else if(!wcscmp(argv[1],L"--child")){
            HANDLE signal=OpenEventW(EVENT_MODIFY_STATE,FALSE,argv[2]);
            if(!signal || !SetEvent(signal))return 13;
            CloseHandle(signal);
        }else return 14;
        code=WaitForSingleObject(gate,15000)==WAIT_OBJECT_0 ? 37 : 15;
        CloseHandle(gate);return (int)code;
    }
    if(!GetModuleFileNameW(NULL,image,MAX_PATH) || !GetCurrentDirectoryW(MAX_PATH,cwd))goto done;
    wcscpy_s(frontend,MAX_PATH,image);slash=wcsrchr(frontend,L'\\');if(!slash)goto done;
    wcscpy_s(slash+1,MAX_PATH-(size_t)(slash+1-frontend),L"frontend.exe");
    swprintf_s(ready_name,96,L"Local\\ntvdm-members-%lu-ready",GetCurrentProcessId());
    swprintf_s(parent_name,96,L"Local\\ntvdm-members-%lu-parent",GetCurrentProcessId());
    swprintf_s(child_name,96,L"Local\\ntvdm-members-%lu-child",GetCurrentProcessId());
    ready=CreateEventW(NULL,TRUE,FALSE,ready_name);parent_done=CreateEventW(NULL,TRUE,FALSE,parent_name);child_done=CreateEventW(NULL,TRUE,FALSE,child_name);
    if(!ready || !parent_done || !child_done)goto done;
    error=run16_native_backend_open(frontend,&backend);if(error)goto done;
    error=run16_native_backend_members(backend,&count);if(error || count)goto done;
    environment=GetEnvironmentStringsW();if(!environment)goto done;
    swprintf_s(command,2048,L"\"%ls\" --parent %ls %ls %ls",image,ready_name,parent_name,child_name);
    start.application=image;start.command=command;start.directory=cwd;start.environment=environment;start.console_mask=7;
    error=run16_native_backend_launch(backend,&start,&target);if(error)goto done;
    if(WaitForSingleObject(ready,5000)!=WAIT_OBJECT_0)goto done;
    error=run16_native_backend_members(backend,&count);if(error || count!=2)goto done;
    SetEvent(parent_done);
    if(WaitForSingleObject(target,5000)!=WAIT_OBJECT_0 || !GetExitCodeProcess(target,&code) || code!=37)goto done;
    error=run16_native_backend_members(backend,&count);if(error || count!=1)goto done;
    SetEvent(child_done);
    for(i=0;i<100;++i){error=run16_native_backend_members(backend,&count);if(error || !count)break;Sleep(10);}
    if(error || count)goto done;
    TerminateProcess(run16_native_backend_process(backend),123);
    WaitForSingleObject(run16_native_backend_process(backend),5000);count=99;
    error=run16_native_backend_members(backend,&count);if(!error || count!=99)goto done;
    puts("PASS native users 0 -> 2 -> 1 after direct parent exit -> 0; helper excluded; helper failure is not empty membership");failed=0;
done:
    if(failed)printf("FAIL membership error=%lu count=%lu exit=%lu last=%lu\n",error,count,code,GetLastError());
    if(parent_done)SetEvent(parent_done);if(child_done)SetEvent(child_done);
    if(target){WaitForSingleObject(target,5000);CloseHandle(target);}
    run16_native_backend_close(backend);
    if(environment)FreeEnvironmentStringsW(environment);
    if(ready)CloseHandle(ready);if(parent_done)CloseHandle(parent_done);if(child_done)CloseHandle(child_done);
    return failed;
}
