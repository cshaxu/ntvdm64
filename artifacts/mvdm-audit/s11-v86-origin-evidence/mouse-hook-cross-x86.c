#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv) {
    if(argc==3 && !strcmp(argv[1],"child")) {
        MSG msg; HANDLE ready=OpenEventA(EVENT_MODIFY_STATE,FALSE,argv[2]);
        if(!ready)return 11; PeekMessageW(&msg,NULL,0,0,PM_NOREMOVE); SetEvent(ready); CloseHandle(ready);
        while(GetMessageW(&msg,NULL,0,0)>0){} return 0;
    }
    char image[MAX_PATH], event[96], cmd[MAX_PATH+160]; STARTUPINFOA si={0}; PROCESS_INFORMATION pi={0};
    HANDLE ready; HMODULE dll; HOOKPROC proc; HHOOK hook; DWORD err; int result=1;
    GetModuleFileNameA(NULL,image,MAX_PATH); sprintf_s(event,sizeof(event),"Local\\T473-Hook-Control-%lu",GetCurrentProcessId());
    ready=CreateEventA(NULL,TRUE,FALSE,event); if(!ready || GetLastError()==ERROR_ALREADY_EXISTS)return 12;
    sprintf_s(cmd,sizeof(cmd),"\"%s\" child %s",image,event); si.cb=sizeof(si);
    if(!CreateProcessA(NULL,cmd,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&si,&pi))return 13;
    if(WaitForSingleObject(ready,5000)!=WAIT_OBJECT_0)goto done;
    dll=LoadLibraryA("O:\\repos.hobby\\ntvdm64\\build\\t473-s11\\v86-hook-cross-x86.dll");
    if(!dll){printf("load-error=%lu\n",GetLastError());goto done;}
    proc=(HOOKPROC)GetProcAddress(dll,"MouseInputHook"); if(!proc){printf("export-error=%lu\n",GetLastError());FreeLibrary(dll);goto done;}
    SetLastError(0); hook=SetWindowsHookExW(WH_GETMESSAGE,proc,dll,pi.dwThreadId);err=GetLastError();
    printf("parent=%lu child=%lu thread=%lu hook=%p error=%lu\n",GetCurrentProcessId(),pi.dwProcessId,pi.dwThreadId,hook,err);
    if(hook){UnhookWindowsHookEx(hook);result=0;} FreeLibrary(dll);
 done:
    PostThreadMessageW(pi.dwThreadId,WM_QUIT,0,0); if(WaitForSingleObject(pi.hProcess,5000)!=WAIT_OBJECT_0){TerminateProcess(pi.hProcess,99);WaitForSingleObject(pi.hProcess,INFINITE);} CloseHandle(pi.hThread);CloseHandle(pi.hProcess);CloseHandle(ready);return result;
}
