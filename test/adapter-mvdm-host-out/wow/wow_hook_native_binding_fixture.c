#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

/* Argument-boundary unit fixture only. Real USER16/Hook execution is a
 * separate mandatory group; this seam never implements a native hook. */
static HINSTANCE observed_module;
static DWORD observed_thread, observed_entry_error;
static HOOKPROC observed_callback;
static int observed_kind;
static unsigned calls, errors;
static HHOOK WINAPI observed_native_registration(int kind, HOOKPROC callback,
    HINSTANCE module, DWORD thread)
{
    observed_kind=kind;observed_callback=callback;observed_module=module;
    observed_thread=thread;observed_entry_error=GetLastError();++calls;
    SetLastError(0x447090u);
    return (HHOOK)(ULONG_PTR)0x1234u;
}
#define SetWindowsHookExA observed_native_registration
#include "wow32-dll/source/wow_hook_native_binding.c"
#undef SetWindowsHookExA

static LRESULT CALLBACK callback(int code,WPARAM wp,LPARAM lp)
{(void)code;(void)wp;(void)lp;return 0;}
static DWORD WINAPI wait_thread(void *event)
{return WaitForSingleObject((HANDLE)event,10000)==WAIT_OBJECT_0?0:1;}
#define CHECK(value) do {if(!(value)){fprintf(stderr,"hook-boundary failed line=%u error=%lu\n",__LINE__,GetLastError());++errors;}}while(0)
static void check(HINSTANCE input,HOOKPROC proc,DWORD thread,HINSTANCE expected)
{
    unsigned before=calls;
    SetLastError(0x447123u);
    CHECK(wow_native_SetWindowsHookExA(WH_CBT,proc,input,thread)==(HHOOK)(ULONG_PTR)0x1234u);
    CHECK(GetLastError()==0x447090u && calls==before+1 &&
        observed_entry_error==0x447123u && observed_module==expected &&
        observed_thread==thread && observed_callback==proc && observed_kind==WH_CBT);
}
int main(int argc,char **argv)
{
    HINSTANCE self=GetModuleHandleA(NULL);
    HANDLE event,thread;
    DWORD tid,before=0,after=0,index;
    STARTUPINFOA startup={sizeof(startup)};
    PROCESS_INFORMATION child={0};
    char path[MAX_PATH],command[2*MAX_PATH];
    if(argc==2 && !strcmp(argv[1],"--child"))return 0;
    event=CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(event!=NULL);if(!event)return 1;
    thread=CreateThread(NULL,0,wait_thread,event,0,&tid);
    CHECK(thread!=NULL);if(!thread){CloseHandle(event);return 1;}
    check(self,callback,GetCurrentThreadId(),NULL);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
    for(index=0;index<16;index++)check(self,callback,tid,NULL);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after) && before==after);
    check(self,callback,0,self);
    check(self,callback,MAXDWORD,self);
    check(NULL,callback,tid,NULL);
    check((HINSTANCE)(ULONG_PTR)1,callback,tid,(HINSTANCE)(ULONG_PTR)1);
    check(self,NULL,tid,self);
    SetEvent(event);CHECK(WaitForSingleObject(thread,10000)==WAIT_OBJECT_0);
    check(self,callback,tid,self); /* Still-held dead native thread. */
    CHECK(GetModuleFileNameA(NULL,path,sizeof(path))>0);
    sprintf_s(command,sizeof(command),"\"%s\" --child",path);
    CHECK(CreateProcessA(path,command,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,
        NULL,NULL,&startup,&child));
    if(child.hProcess){
        check(self,callback,child.dwThreadId,self);
        CHECK(ResumeThread(child.hThread)!=(DWORD)-1);
        CHECK(WaitForSingleObject(child.hProcess,10000)==WAIT_OBJECT_0);
        CloseHandle(child.hThread);CloseHandle(child.hProcess);
    }
    CloseHandle(thread);CloseHandle(event);
    printf("WOW_HOOK_NATIVE_BOUNDARY errors=%u calls=%u local=17 global=1 invalid=1 null=1 mismatch=1 callback=1 dead=1 foreign=1 handles=%lu/%lu\n",errors,calls,before,after);
    return errors?1:0;
}
