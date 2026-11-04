#include "payload.h"
#include <stdint.h>
#include <string.h>
#define PAYLOAD_MAGIC0 UINT64_C(0x3144414f4c594150)
#define PAYLOAD_MAGIC1 UINT64_C(0xb14fa381268b19c7)
#define PAYLOAD_LIMIT 65536u
#define PAYLOAD_LOW_LIMIT ((uintptr_t)0x80000000u)
typedef struct payload_header {
    uint64_t magic0,magic1,base;
    uint32_t version,bytes;
    uint64_t reserved;
} payload_header;
_Static_assert(sizeof(payload_header)==40,"cross-width payload header");
static DWORD find_payload(HANDLE process,void **address,payload_header *header,BOOL *found)
{
    uintptr_t cursor=0x10000;MEMORY_BASIC_INFORMATION region;
    *found=FALSE;*address=NULL;
    while(cursor<PAYLOAD_LOW_LIMIT) {
        SIZE_T queried=VirtualQueryEx(process,(void *)cursor,&region,sizeof(region));
        if(!queried) {
            DWORD error=GetLastError();
            if(error==ERROR_INVALID_PARAMETER)return ERROR_SUCCESS;
            return error?error:ERROR_INVALID_DATA;
        }
        uintptr_t base=(uintptr_t)region.BaseAddress;
        uintptr_t end=base+region.RegionSize;
        if(end<=cursor)return ERROR_INVALID_DATA;
        if(region.State==MEM_COMMIT && region.Type==MEM_PRIVATE &&
           region.BaseAddress==region.AllocationBase && region.RegionSize>=sizeof(*header) &&
           !(region.Protect&PAGE_GUARD) && (region.Protect&0xff)==PAGE_READWRITE) {
            payload_header candidate;SIZE_T read=0;
            if(ReadProcessMemory(process,region.BaseAddress,&candidate,sizeof(candidate),&read) &&
               read==sizeof(candidate) && candidate.magic0==PAYLOAD_MAGIC0 && candidate.magic1==PAYLOAD_MAGIC1) {
                *found=TRUE;
                if(candidate.version!=1 || candidate.reserved || candidate.base!=(uint64_t)base ||
                   !candidate.bytes || candidate.bytes>PAYLOAD_LIMIT ||
                   candidate.bytes>region.RegionSize-sizeof(candidate))return ERROR_INVALID_DATA;
                *address=region.BaseAddress;*header=candidate;return ERROR_SUCCESS;
            }
        }
        cursor=end;
    }
    return ERROR_SUCCESS;
}
static void *allocate_low(HANDLE process,SIZE_T bytes)
{
    uintptr_t cursor=0x10000;MEMORY_BASIC_INFORMATION region;SYSTEM_INFO info;
    GetSystemInfo(&info);
    while(cursor<PAYLOAD_LOW_LIMIT && bytes<=PAYLOAD_LOW_LIMIT-cursor) {
        if(!VirtualQueryEx(process,(void *)cursor,&region,sizeof(region)))return NULL;
        uintptr_t end=(uintptr_t)region.BaseAddress+region.RegionSize;
        if(end<=cursor)break;
        if(region.State==MEM_FREE && bytes<=end-cursor) {
            void *p=VirtualAllocEx(process,(void *)cursor,bytes,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
            if(p)return p;
            if(GetLastError()==ERROR_ACCESS_DENIED)return NULL;
        }
        if(end>=PAYLOAD_LOW_LIMIT)break;
        cursor=(end+info.dwAllocationGranularity-1u)&~((uintptr_t)info.dwAllocationGranularity-1u);
    }
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);return NULL;
}
DWORD process_hook_payload_write(HANDLE child,const void *payload,DWORD bytes)
{
    payload_header header,old;void *remote=NULL,*existing=NULL;BOOL found=FALSE;
    SIZE_T wrote=0,read=0,total;DWORD error;
    if(!child || !payload || !bytes || bytes>PAYLOAD_LIMIT)return ERROR_INVALID_PARAMETER;
    error=find_payload(child,&existing,&old,&found);if(error)return error;
    if(found)return ERROR_ALREADY_EXISTS;
    total=sizeof(header)+(SIZE_T)bytes;remote=allocate_low(child,total);
    if(!remote){error=GetLastError();return error?error:ERROR_NOT_ENOUGH_MEMORY;}
    ZeroMemory(&header,sizeof(header));header.base=(uint64_t)(uintptr_t)remote;header.version=1;header.bytes=bytes;
    if(!ReadProcessMemory(child,remote,&old,sizeof(old),&read) || read!=sizeof(old)) {
        error=GetLastError();if(!error)error=ERROR_PARTIAL_COPY;goto failed;
    }
    if(!WriteProcessMemory(child,remote,&header,sizeof(header),&wrote) || wrote!=sizeof(header)) {
        error=GetLastError();if(!error)error=ERROR_PARTIAL_COPY;goto failed;
    }
    if(!WriteProcessMemory(child,(BYTE *)remote+sizeof(header),payload,bytes,&wrote) || wrote!=bytes) {
        error=GetLastError();if(!error)error=ERROR_PARTIAL_COPY;goto failed;
    }
    header.magic0=PAYLOAD_MAGIC0;header.magic1=PAYLOAD_MAGIC1;
    if(!WriteProcessMemory(child,remote,&header,sizeof(header),&wrote) || wrote!=sizeof(header)) {
        error=GetLastError();if(!error)error=ERROR_PARTIAL_COPY;goto failed;
    }
    return ERROR_SUCCESS;
failed:
    VirtualFreeEx(child,remote,0,MEM_RELEASE);return error;
}
DWORD process_hook_payload_take(process_hook_payload_reader consume,void *context,BOOL *found)
{
    HANDLE self=GetCurrentProcess();payload_header header;void *remote,*copy;SIZE_T read=0;DWORD error;
    if(!consume || !context || !found)return ERROR_INVALID_PARAMETER;
    *found=FALSE;error=find_payload(self,&remote,&header,found);if(error || !*found)return error;
    copy=HeapAlloc(GetProcessHeap(),0,header.bytes);if(!copy)return ERROR_NOT_ENOUGH_MEMORY;
    if(!ReadProcessMemory(self,(BYTE *)remote+sizeof(header),copy,header.bytes,&read) || read!=header.bytes) {
        error=GetLastError();if(!error)error=ERROR_PARTIAL_COPY;
    } else error=consume(copy,header.bytes,context);
    HeapFree(GetProcessHeap(),0,copy);
    if(!error && !VirtualFree(remote,0,MEM_RELEASE))error=GetLastError();
    return error;
}
