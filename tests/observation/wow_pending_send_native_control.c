/* Native USER control for destruction during SendMessageTimeout. No WOW,
 * Hook, provider substitute or private USER layout participates. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

static HWND target;
static DWORD status, error;
static DWORD_PTR value;
static BOOL destroyed;
static const char class_name[]="T447_PENDING_NATIVE_CONTROL";

static LRESULT CALLBACK receiver(HWND window,UINT message,WPARAM wp,LPARAM lp)
{
    if(message==0x47c){
        destroyed=DestroyWindow(window);
        return 0x447a007c;
    }
    if(message==WM_DESTROY){PostQuitMessage(0);return 0;}
    return DefWindowProcA(window,message,wp,lp);
}

static BOOL create_target(const char *caption)
{
    WNDCLASSA cls={0};
    cls.hInstance=GetModuleHandleA(NULL);cls.lpfnWndProc=receiver;
    cls.lpszClassName=class_name;
    if(!RegisterClassA(&cls))return FALSE;
    target=CreateWindowA(class_name,caption,WS_POPUP,0,0,100,60,
        NULL,NULL,cls.hInstance,NULL);
    return target!=NULL;
}

static DWORD WINAPI send_destroy(PVOID ignored)
{
    (void)ignored;
    value=0;
    SetLastError(ERROR_SUCCESS);
    status=(DWORD)SendMessageTimeoutA(target,0x47c,0,0,SMTO_ERRORONEXIT,5000,&value);
    error=GetLastError();
    return 0;
}

static BOOL pump(void)
{
    MSG msg;BOOL got;
    while((got=GetMessageA(&msg,NULL,0,0))>0)DispatchMessageA(&msg);
    return got==0;
}

int main(int argc,char **argv)
{
    HANDLE sender,ready;DWORD exit_code=1;
    char executable[MAX_PATH],name[120],command[1024];
    STARTUPINFOA startup={0};PROCESS_INFORMATION child={0};
    DWORD child_owner=0;int result=12;
    if(argc==3 && !strcmp(argv[1],"--receiver")){
        ready=OpenEventA(EVENT_MODIFY_STATE,FALSE,argv[2]);
        if(!ready || !create_target(argv[2]) || !SetEvent(ready))return 2;
        CloseHandle(ready);
        if(!pump() || !destroyed || IsWindow(target))return 3;
        return 0;
    }
    if(argc!=1 || !create_target("same-process"))return 4;
    sender=CreateThread(NULL,0,send_destroy,NULL,0,NULL);
    if(!sender || !pump() || WaitForSingleObject(sender,6000)!=WAIT_OBJECT_0 ||
        !destroyed || IsWindow(target))return 5;
    CloseHandle(sender);
    if(status && value!=0x447a007c)return 6;
    printf("case=same bits=%u status=%lu error=%lu result=%llX destroyed=1\n",
        (unsigned)(sizeof(void *)*8),status,error,(unsigned long long)value);
    if(!UnregisterClassA(class_name,GetModuleHandleA(NULL)))return 7;
    if(!GetModuleFileNameA(NULL,executable,MAX_PATH) ||
        sprintf_s(name,sizeof(name),"Local\\T447-death-%lu-%llu",
            GetCurrentProcessId(),GetTickCount64())<0)return 8;
    ready=CreateEventA(NULL,TRUE,FALSE,name);
    if(!ready || GetLastError()==ERROR_ALREADY_EXISTS)return 9;
    if(sprintf_s(command,sizeof(command),"\"%s\" --receiver \"%s\"",executable,name)<0)return 10;
    startup.cb=sizeof(startup);
    if(!CreateProcessA(executable,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,
        NULL,NULL,&startup,&child))return 11;
    CloseHandle(child.hThread);
    if(WaitForSingleObject(ready,6000)!=WAIT_OBJECT_0)goto child_done;
    target=FindWindowA(class_name,name);
    if(!target || !GetWindowThreadProcessId(target,&child_owner) ||
        child_owner!=child.dwProcessId){result=13;goto child_done;}
    (void)send_destroy(NULL);
    if(WaitForSingleObject(child.hProcess,6000)!=WAIT_OBJECT_0 ||
        !GetExitCodeProcess(child.hProcess,&exit_code) || exit_code || IsWindow(target)){
        result=14;goto child_done;
    }
    if(status && value!=0x447a007c){result=15;goto child_done;}
    printf("case=cross bits=%u status=%lu error=%lu result=%llX destroyed=1\n",
        (unsigned)(sizeof(void *)*8),status,error,(unsigned long long)value);
    result=0;
child_done:
    if(result && WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT){
        TerminateProcess(child.hProcess,1);WaitForSingleObject(child.hProcess,5000);
    }
    CloseHandle(ready);CloseHandle(child.hProcess);
    return result;
}
