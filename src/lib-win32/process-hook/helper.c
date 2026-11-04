/* Project-authored bounded OS rundll32 handoff. The target handle is borrowed;
 * only our transfer allocation and freshly created helper are owned here. */
#include "helper.h"
#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <errno.h>
#define HANDOFF_MAGIC UINT64_C(0x31464f444e414857)
#define PROCESS_HOOK_PATH_CAP 32752u
typedef struct handoff_header {
    uint64_t magic,creation;
    uint32_t version,bytes,packet_bytes,pid,dll_chars,reserved;
} handoff_header;
_Static_assert(sizeof(handoff_header)==40,"handoff header is width independent");
static uint64_t creation_time(HANDLE process)
{
    FILETIME creation,exit,kernel,user;
    if(!GetProcessTimes(process,&creation,&exit,&kernel,&user))return 0;
    return ((uint64_t)creation.dwHighDateTime<<32)|creation.dwLowDateTime;
}
static DWORD remaining(ULONGLONG deadline)
{ULONGLONG now=GetTickCount64();return now>=deadline?0:(DWORD)(deadline-now);}
static void *allocate_low(HANDLE process,SIZE_T bytes)
{
    uintptr_t cursor=0x10000,limit=0x7ffe0000;MEMORY_BASIC_INFORMATION region;
    while(cursor<limit && bytes<=limit-cursor){
        if(!VirtualQueryEx(process,(void *)cursor,&region,sizeof(region)))return NULL;
        uintptr_t end=(uintptr_t)region.BaseAddress+region.RegionSize;
        if(end<=cursor)break;
        if(region.State==MEM_FREE && bytes<=end-cursor){
            void *allocation=VirtualAllocEx(process,(void *)cursor,bytes,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
            if(allocation)return allocation;
        }
        if(end>=limit)break;
        cursor=(end+0xffffu)&~(uintptr_t)0xffffu;
    }
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);return NULL;
}
DWORD process_hook_install_helper(HANDLE child,PCWSTR dll,const void *packet,DWORD bytes,process_hook_create_w create)
{
    DWORD error=ERROR_SUCCESS,length,pid;WCHAR application[MAX_PATH],*command=NULL;
    STARTUPINFOW startup={0};PROCESS_INFORMATION helper={0};
    handoff_header *header=NULL;void *remote=NULL;SIZE_T total,written=0;
    ULONGLONG deadline=GetTickCount64()+10000;uint64_t birth;
    if(!child || !dll || !packet || !create || !bytes || bytes>65536)return ERROR_INVALID_PARAMETER;
    SIZE_T dll_chars=wcsnlen_s(dll,PROCESS_HOOK_PATH_CAP)+1;
    if(dll_chars>PROCESS_HOOK_PATH_CAP)return ERROR_FILENAME_EXCED_RANGE;
    pid=GetProcessId(child);if(!pid)return GetLastError();
    birth=creation_time(child);if(!birth)return GetLastError();
    total=sizeof(*header)+bytes+dll_chars*sizeof(WCHAR);
    header=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,total);if(!header)return ERROR_NOT_ENOUGH_MEMORY;
    command=HeapAlloc(GetProcessHeap(),0,32768*sizeof(WCHAR));if(!command){error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
    header->magic=HANDOFF_MAGIC;header->creation=birth;header->version=1;header->bytes=(DWORD)total;
    header->packet_bytes=bytes;header->pid=pid;header->dll_chars=(DWORD)dll_chars;
    memcpy(header+1,packet,bytes);memcpy((BYTE *)(header+1)+bytes,dll,dll_chars*sizeof(WCHAR));
    /* This must succeed through the borrowed capability, before opening a PID
     * in the helper. A limited query handle must never gain staging authority. */
    remote=allocate_low(child,total);if(!remote){error=GetLastError();goto done;}
    if(!WriteProcessMemory(child,remote,header,total,&written)){error=GetLastError();goto done;}
    if(written!=total){error=ERROR_PARTIAL_COPY;goto done;}
#ifdef _WIN64
    length=GetSystemWow64DirectoryW(application,MAX_PATH);
    if(!length || length>=MAX_PATH-14){error=length?ERROR_INSUFFICIENT_BUFFER:GetLastError();goto done;}
    memcpy(application+length,L"\\rundll32.exe",14*sizeof(WCHAR));
#else
    length=GetWindowsDirectoryW(application,MAX_PATH);
    if(!length || length>=MAX_PATH-24){error=length?ERROR_INSUFFICIENT_BUFFER:GetLastError();goto done;}
    memcpy(application+length,L"\\Sysnative\\rundll32.exe",24*sizeof(WCHAR));
#endif
    if(swprintf_s(command,32768,L"\"%ls\" \"%ls\",#1 %lu %llx %llx",application,dll,pid,
        (unsigned long long)(uintptr_t)remote,(unsigned long long)birth)<0){error=ERROR_INSUFFICIENT_BUFFER;goto done;}
    startup.cb=sizeof(startup);
    if(!remaining(deadline)){error=ERROR_TIMEOUT;goto done;}
    if(!create(application,command,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,NULL,NULL,&startup,&helper)){
        error=GetLastError();goto done;
    }
    if(WaitForSingleObject(child,0)==WAIT_OBJECT_0){error=ERROR_PROCESS_ABORTED;goto done;}
    if(ResumeThread(helper.hThread)!=1){error=GetLastError();if(!error)error=ERROR_INVALID_STATE;goto done;}
    HANDLE waits[2]={child,helper.hProcess};
    DWORD wait=WaitForMultipleObjects(2,waits,FALSE,remaining(deadline));
    if(wait==WAIT_OBJECT_0)error=ERROR_PROCESS_ABORTED;
    else if(wait==WAIT_OBJECT_0+1){if(!GetExitCodeProcess(helper.hProcess,&error))error=GetLastError();}
    else error=wait==WAIT_TIMEOUT?ERROR_TIMEOUT:GetLastError();
done:
    if(helper.hProcess){
        if(WaitForSingleObject(helper.hProcess,0)==WAIT_TIMEOUT){
            DWORD joined;
            TerminateProcess(helper.hProcess,ERROR_CANCELLED);
            /* The deadline may already be exhausted. Termination is async;
             * join before releasing the target memory the helper can read. */
            joined=WaitForSingleObject(helper.hProcess,5000);
            if(joined!=WAIT_OBJECT_0){
                /* The creator must abort this uncommitted target. Keep its
                 * transfer allocation until then if the helper may still run. */
                remote=NULL;
                if(!error)error=joined==WAIT_TIMEOUT?ERROR_TIMEOUT:GetLastError();
            }
        }
        CloseHandle(helper.hProcess);
    }
    if(helper.hThread)CloseHandle(helper.hThread);
    if(remote)VirtualFreeEx(child,remote,0,MEM_RELEASE);
    if(header)HeapFree(GetProcessHeap(),0,header);
    if(command)HeapFree(GetProcessHeap(),0,command);
    return error;
}
static BOOL number(const char **input,unsigned base,uint64_t *value)
{
    char *end;const char *start=*input;
    while(*start==' ')++start;
    if(!*start || *start=='-' || *start=='+')return FALSE;
    errno=0;*value=strtoull(start,&end,base);
    if(errno || end==start || (*end && *end!=' '))return FALSE;
    *input=end;return TRUE;
}
DWORD process_hook_run_helper(process_hook_helper_apply apply,void *context)
{
    uint64_t pid,address,birth;DWORD error=ERROR_INVALID_PARAMETER;HANDLE child=NULL;
    handoff_header header;BYTE *data=NULL;SIZE_T read=0;char arguments[128];
    if(!apply)return ERROR_INVALID_PARAMETER;
    /* Ordinal callbacks have no A/W suffix contract. Read the Unicode process
     * command line rather than guessing the encoding of rundll32's argument. */
    PCWSTR raw=GetCommandLineW();
    while(*raw==L' ')++raw;
    if(*raw++!=L'"')goto done;
    raw=wcschr(raw,L'"');if(!raw)goto done;++raw;
    while(*raw==L' ')++raw;
    if(*raw++!=L'"')goto done;
    raw=wcschr(raw,L'"');if(!raw || wcsncmp(raw+1,L",#1 ",4))goto done;
    raw+=5;
    SIZE_T used=0;
    while(*raw && used<sizeof(arguments)-1){if(*raw>127)goto done;arguments[used++]=(char)*raw++;}
    if(*raw)goto done;
    arguments[used]=0;const char *command=arguments;
    if(!number(&command,10,&pid) || !pid || pid>MAXDWORD ||
       !number(&command,16,&address) || !address || address>0x7ffe0000 ||
       !number(&command,16,&birth) || !birth)goto done;
    while(*command==' ')++command;
    if(*command)goto done;
    child=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_VM_WRITE|PROCESS_VM_OPERATION|SYNCHRONIZE,FALSE,(DWORD)pid);
    if(!child){error=GetLastError();goto done;}
    if(creation_time(child)!=birth){error=ERROR_INVALID_DATA;goto done;}
    if(WaitForSingleObject(child,0)==WAIT_OBJECT_0){error=ERROR_PROCESS_ABORTED;goto done;}
    if(!ReadProcessMemory(child,(void *)(uintptr_t)address,&header,sizeof(header),&read)){
        error=GetLastError();goto done;
    }
    error=ERROR_INVALID_DATA;
    if(read!=sizeof(header) || header.magic!=HANDOFF_MAGIC || header.creation!=birth || header.pid!=pid ||
       header.version!=1 || header.reserved || !header.packet_bytes || header.packet_bytes>65536 ||
       header.packet_bytes%sizeof(WCHAR) ||
       header.dll_chars<4 || header.dll_chars>PROCESS_HOOK_PATH_CAP ||
       header.bytes!=sizeof(header)+header.packet_bytes+header.dll_chars*sizeof(WCHAR))goto done;
    data=HeapAlloc(GetProcessHeap(),0,header.bytes);if(!data){error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
    if(!ReadProcessMemory(child,(void *)(uintptr_t)address,data,header.bytes,&read)){error=GetLastError();goto done;}
    if(read!=header.bytes || memcmp(data,&header,sizeof(header)))goto done;
    BYTE *packet=data+sizeof(header);PCWSTR dll=(PCWSTR)(packet+header.packet_bytes);
    if(dll[header.dll_chars-1] || wcsnlen_s(dll,header.dll_chars)!=header.dll_chars-1)goto done;
    error=apply(child,packet,header.packet_bytes,dll,context);
done:
    if(data)HeapFree(GetProcessHeap(),0,data);
    if(child)CloseHandle(child);
    return error;
}
