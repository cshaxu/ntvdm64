/* One retained ConPTY survives direct-target and descendant completion. */
#include <windows.h>
#include <stdio.h>
#include "ntkvm-exe/native_console_backend.h"

int wmain(int argc,WCHAR **argv)
{
    WCHAR image[MAX_PATH],cwd[MAX_PATH],command[2048];
    WCHAR ready_name[96],parent_name[96],child_name[96],pid_name[112];
    HANDLE ready=NULL,parent_done=NULL,child_done=NULL,target=NULL;
    HANDLE mapping=NULL,descendant=NULL;DWORD *child_pid=NULL;
    run16_native_backend *backend=NULL;run16_native_start start={0};
    LPWCH environment=NULL;DWORD count=99,code=0,error=0;int failed=1;
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
            swprintf_s(pid_name,112,L"%ls-pid",argv[2]);
            mapping=OpenFileMappingW(FILE_MAP_WRITE,FALSE,pid_name);
            child_pid=mapping ? MapViewOfFile(mapping,FILE_MAP_WRITE,0,0,sizeof(DWORD)) : NULL;
            if(!child_pid)return 16;
            *child_pid=GetCurrentProcessId();UnmapViewOfFile(child_pid);CloseHandle(mapping);
            if(!signal || !SetEvent(signal))return 13;
            CloseHandle(signal);
        }else return 14;
        code=WaitForSingleObject(gate,15000)==WAIT_OBJECT_0 ? 37 : 15;
        CloseHandle(gate);return (int)code;
    }
    if(!GetModuleFileNameW(NULL,image,MAX_PATH) || !GetCurrentDirectoryW(MAX_PATH,cwd))goto done;
    swprintf_s(ready_name,96,L"Local\\ntvdm-members-%lu-ready",GetCurrentProcessId());
    swprintf_s(parent_name,96,L"Local\\ntvdm-members-%lu-parent",GetCurrentProcessId());
    swprintf_s(child_name,96,L"Local\\ntvdm-members-%lu-child",GetCurrentProcessId());
    swprintf_s(pid_name,112,L"%ls-pid",ready_name);
    mapping=CreateFileMappingW(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,sizeof(DWORD),pid_name);
    child_pid=mapping ? MapViewOfFile(mapping,FILE_MAP_READ,0,0,sizeof(DWORD)) : NULL;
    if(!child_pid)goto done;
    ready=CreateEventW(NULL,TRUE,FALSE,ready_name);parent_done=CreateEventW(NULL,TRUE,FALSE,parent_name);child_done=CreateEventW(NULL,TRUE,FALSE,child_name);
    if(!ready || !parent_done || !child_done)goto done;
    error=run16_native_backend_open(&backend);if(error)goto done;
    error=run16_native_backend_members(backend,&count);if(error || count)goto done;
    environment=GetEnvironmentStringsW();if(!environment)goto done;
    swprintf_s(command,2048,L"\"%ls\" --parent %ls %ls %ls",image,ready_name,parent_name,child_name);
    start.application=image;start.command=command;start.directory=cwd;start.environment=environment;start.console_mask=7;
    error=run16_native_backend_launch(backend,&start,&target);if(error)goto done;
    if(WaitForSingleObject(ready,5000)!=WAIT_OBJECT_0)goto done;
    descendant=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,*child_pid);
    if(!descendant)goto done;
    error=run16_native_backend_members(backend,&count);if(error || count!=1)goto done;
    SetEvent(parent_done);
    if(WaitForSingleObject(target,5000)!=WAIT_OBJECT_0 || !GetExitCodeProcess(target,&code) || code!=37)goto done;
    error=run16_native_backend_members(backend,&count);if(error || count!=1)goto done;
    SetEvent(child_done);
    if(WaitForSingleObject(descendant,5000)!=WAIT_OBJECT_0 ||
        !GetExitCodeProcess(descendant,&code) || code!=37)goto done;
    error=run16_native_backend_members(backend,&count);if(error || count!=1)goto done;
    run16_native_backend_cancel(backend);count=99;
    error=run16_native_backend_members(backend,&count);if(error!=ERROR_OPERATION_ABORTED || count!=99)goto done;
    puts("PASS ConPTY retained after verified parent and descendant exit; cancellation is not empty membership");failed=0;
done:
    if(failed)printf("FAIL membership error=%lu count=%lu exit=%lu last=%lu\n",error,count,code,GetLastError());
    if(parent_done)SetEvent(parent_done);if(child_done)SetEvent(child_done);
    if(target){WaitForSingleObject(target,5000);CloseHandle(target);}
    if(descendant)CloseHandle(descendant);
    if(child_pid)UnmapViewOfFile(child_pid);if(mapping)CloseHandle(mapping);
    run16_native_backend_close(backend);
    if(environment)FreeEnvironmentStringsW(environment);
    if(ready)CloseHandle(ready);if(parent_done)CloseHandle(parent_done);if(child_done)CloseHandle(child_done);
    return failed;
}
