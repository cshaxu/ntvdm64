/* Diagnostic only: sample exact caller-owned x86 worker threads. Each
 * successful suspension is paired with resume; no guest memory is changed. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct entry { DWORD id; HANDLE handle; ULONGLONG cpu; } entry;
static ULONGLONG cpu(HANDLE thread) {
    FILETIME create, exit, kernel, user;
    ULARGE_INTEGER k,u;
    if(!GetThreadTimes(thread,&create,&exit,&kernel,&user))return 0;
    k.LowPart=kernel.dwLowDateTime;k.HighPart=kernel.dwHighDateTime;
    u.LowPart=user.dwLowDateTime;u.HighPart=user.dwHighDateTime;
    return k.QuadPart+u.QuadPart;
}
int main(int argc,char **argv) {
    DWORD target,round,n=0,i;entry threads[64];HANDLE snapshot,process;
    THREADENTRY32 thread={sizeof(thread)};MODULEENTRY32 module={sizeof(module)};
    if(argc!=2 || !(target=strtoul(argv[1],NULL,10)))return 2;
    process=OpenProcess(PROCESS_VM_READ|PROCESS_QUERY_INFORMATION,FALSE,target);
    if(!process)return 3;
    snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,target);
    if(snapshot!=INVALID_HANDLE_VALUE){
        if(Module32First(snapshot,&module))do{printf("module=%s base=%08lX size=%08lX\n",module.szModule,(DWORD)(ULONG_PTR)module.modBaseAddr,module.modBaseSize);}while(Module32Next(snapshot,&module));
        CloseHandle(snapshot);
    }
    snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);
    if(snapshot==INVALID_HANDLE_VALUE){CloseHandle(process);return 4;}
    if(Thread32First(snapshot,&thread))do{
        if(thread.th32OwnerProcessID==target && n<64){
            HANDLE h=OpenThread(THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT|THREAD_QUERY_INFORMATION,FALSE,thread.th32ThreadID);
            if(h){threads[n].id=thread.th32ThreadID;threads[n].handle=h;threads[n].cpu=cpu(h);++n;}
        }
    }while(Thread32Next(snapshot,&thread));
    CloseHandle(snapshot);
    for(round=0;round<100;++round){
        for(i=0;i<n;++i){
            CONTEXT context={0};DWORD stack[8]={0};SIZE_T bytes=0;BOOL ok=FALSE;
            context.ContextFlags=CONTEXT_CONTROL;
            if(SuspendThread(threads[i].handle)==(DWORD)-1)continue;
            ok=GetThreadContext(threads[i].handle,&context);
            if(ok)ReadProcessMemory(process,(PVOID)(ULONG_PTR)context.Esp,stack,sizeof(stack),&bytes);
            ResumeThread(threads[i].handle);
            if(ok)printf("sample=%lu tid=%lu pc=%08lX stack=%08lX,%08lX,%08lX,%08lX,%08lX,%08lX,%08lX,%08lX\n",round,threads[i].id,context.Eip,stack[0],stack[1],stack[2],stack[3],stack[4],stack[5],stack[6],stack[7]);
        }
        Sleep(10);
    }
    for(i=0;i<n;++i){printf("cpu tid=%lu microseconds=%llu\n",threads[i].id,(cpu(threads[i].handle)-threads[i].cpu)/10);CloseHandle(threads[i].handle);}
    CloseHandle(process);return 0;
}
