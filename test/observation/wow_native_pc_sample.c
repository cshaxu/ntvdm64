/* Diagnostic only: sample exact caller-owned x86 worker threads. Each
 * successful suspension is paired with resume; no guest memory is changed. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    DWORD target,round,n=0,i,rounds=100;entry threads[64];HANDLE snapshot,process;
    ULONGLONG started;
    DWORD word_rva=0,word_address=0;
    THREADENTRY32 thread={sizeof(thread)};MODULEENTRY32 module={sizeof(module)};
    if((argc<2 || argc>4) || !(target=strtoul(argv[1],NULL,10)))return 2;
    if(argc>=3 && (!(rounds=strtoul(argv[2],NULL,10)) || rounds>2000))return 2;
    if(argc==4){char *end;word_rva=strtoul(argv[3],&end,16);if(!word_rva || *end)return 2;}
    process=OpenProcess(PROCESS_VM_READ|PROCESS_QUERY_INFORMATION,FALSE,target);
    if(!process)return 3;
    snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,target);
    if(snapshot!=INVALID_HANDLE_VALUE){
        if(Module32First(snapshot,&module))do{
            printf("module=%s base=%08lX size=%08lX\n",module.szModule,(DWORD)(ULONG_PTR)module.modBaseAddr,module.modBaseSize);
            if(word_rva && !_stricmp(module.szModule,"ntvdm.exe") &&
                    module.modBaseSize>=sizeof(DWORD) && word_rva<=module.modBaseSize-sizeof(DWORD))
                word_address=(DWORD)(ULONG_PTR)module.modBaseAddr+word_rva;
        }while(Module32Next(snapshot,&module));
        CloseHandle(snapshot);
    }
    if(word_rva){
        DWORD value;SIZE_T bytes=0;LARGE_INTEGER at;
        if(!word_address || !ReadProcessMemory(process,(PVOID)(ULONG_PTR)word_address,&value,sizeof(value),&bytes) || bytes!=sizeof(value)){CloseHandle(process);return 6;}
        QueryPerformanceCounter(&at);
        printf("host-word rva=%08lX address=%08lX value=%08lX qpc=%lld non-atomic=1\n",word_rva,word_address,value,at.QuadPart);
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
    started=GetTickCount64();
    for(round=0;round<rounds;++round){
        for(i=0;i<n;++i){
            CONTEXT context={0};DWORD stack[8]={0};SIZE_T bytes=0;BOOL ok=FALSE;
            LARGE_INTEGER timestamp;
            context.ContextFlags=CONTEXT_CONTROL;
            if(SuspendThread(threads[i].handle)==(DWORD)-1)continue;
            ok=GetThreadContext(threads[i].handle,&context);
            QueryPerformanceCounter(&timestamp);
            if(ok)ReadProcessMemory(process,(PVOID)(ULONG_PTR)context.Esp,stack,sizeof(stack),&bytes);
            ResumeThread(threads[i].handle);
            if(ok)printf("sample=%lu tid=%lu pc=%08lX stack=%08lX,%08lX,%08lX,%08lX,%08lX,%08lX,%08lX,%08lX qpc=%lld\n",round,threads[i].id,context.Eip,stack[0],stack[1],stack[2],stack[3],stack[4],stack[5],stack[6],stack[7],timestamp.QuadPart);
        }
        Sleep(10);
    }
    printf("sampling rounds=%lu elapsed-ms=%llu\n",rounds,GetTickCount64()-started);
    for(i=0;i<n;++i){printf("cpu tid=%lu microseconds=%llu\n",threads[i].id,(cpu(threads[i].handle)-threads[i].cpu)/10);CloseHandle(threads[i].handle);}
    CloseHandle(process);return 0;
}
