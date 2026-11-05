#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <wchar.h>
#include "lib-win32/process-hook/payload.h"
static const BYTE payload[9]={0,0xff,1,4,8,16,32,64,128};
static DWORD consume(const void *data,DWORD bytes,void *context)
{
    BOOL *matched=(BOOL *)context;
    *matched=bytes==sizeof(payload) && !memcmp(data,payload,bytes);
    return *matched?ERROR_SUCCESS:ERROR_INVALID_DATA;
}
static DWORD reject(const void *data,DWORD bytes,void *context)
{(void)data;(void)bytes;(void)context;return ERROR_CANCELLED;}
static void *find_payload_allocation(void)
{
    uintptr_t cursor=0x10000,limit=(uintptr_t)0x80000000u;MEMORY_BASIC_INFORMATION region;
#ifdef _WIN64
    SYSTEM_INFO info;GetSystemInfo(&info);limit=(uintptr_t)info.lpMaximumApplicationAddress;
#endif
    while(cursor<limit && VirtualQuery((void *)cursor,&region,sizeof(region))) {
        uintptr_t end=(uintptr_t)region.BaseAddress+region.RegionSize;
        if(end<=cursor)break;
        if(region.State==MEM_COMMIT && region.Type==MEM_PRIVATE &&
           region.BaseAddress==region.AllocationBase && !(region.Protect&PAGE_GUARD) &&
           (region.Protect&0xff)==PAGE_READWRITE && region.RegionSize>=sizeof(payload)) {
            BYTE bytes[512];SIZE_T count=region.RegionSize<sizeof(bytes)?region.RegionSize:sizeof(bytes),read=0;
            if(ReadProcessMemory(GetCurrentProcess(),region.BaseAddress,bytes,count,&read) && read==count) {
                for(SIZE_T i=0;i+sizeof(payload)<=count;++i)
                    if(!memcmp(bytes+i,payload,sizeof(payload)))return region.BaseAddress;
            }
        }
        cursor=end;
    }
    return NULL;
}
static BOOL original_import_unchanged(void)
{
    WCHAR path[MAX_PATH];HANDLE file=INVALID_HANDLE_VALUE,mapping=NULL;BYTE *view=NULL,*base=(BYTE *)GetModuleHandleW(NULL);
    BOOL result=FALSE;IMAGE_DOS_HEADER *memoryDos=(IMAGE_DOS_HEADER *)base;
    if(!GetModuleFileNameW(NULL,path,MAX_PATH))return FALSE;
    file=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,0,NULL);
    if(file==INVALID_HANDLE_VALUE)goto done;
    mapping=CreateFileMappingW(file,NULL,PAGE_READONLY,0,0,NULL);if(!mapping)goto done;
    view=MapViewOfFile(mapping,FILE_MAP_READ,0,0,0);if(!view)goto done;
    IMAGE_DOS_HEADER *diskDos=(IMAGE_DOS_HEADER *)view;
    if(diskDos->e_magic!=IMAGE_DOS_SIGNATURE || memoryDos->e_magic!=IMAGE_DOS_SIGNATURE)goto done;
    IMAGE_NT_HEADERS *disk=(IMAGE_NT_HEADERS *)(view+diskDos->e_lfanew);
    IMAGE_NT_HEADERS *memory=(IMAGE_NT_HEADERS *)(base+memoryDos->e_lfanew);
    if(disk->Signature!=IMAGE_NT_SIGNATURE || memory->Signature!=IMAGE_NT_SIGNATURE)goto done;
    result=!memcmp(&disk->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT],
        &memory->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT],sizeof(IMAGE_DATA_DIRECTORY));
done:
    if(view)UnmapViewOfFile(view);
    if(mapping)CloseHandle(mapping);
    if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);
    return result;
}
#ifdef _WIN64
static BOOL reserve_low_regions(HANDLE process)
{
    uintptr_t cursor=0x10000;MEMORY_BASIC_INFORMATION region;SYSTEM_INFO info;
    GetSystemInfo(&info);
    while(cursor<(uintptr_t)0x80000000u && VirtualQueryEx(process,(void *)cursor,&region,sizeof(region))) {
        uintptr_t end=(uintptr_t)region.BaseAddress+region.RegionSize;
        if(end<=cursor)return FALSE;
        if(end>(uintptr_t)0x80000000u)end=(uintptr_t)0x80000000u;
        if(region.State==MEM_FREE && end>cursor) {
            SIZE_T length=(SIZE_T)(end-cursor);
            length&=~((SIZE_T)info.dwAllocationGranularity-1u);
            if(length && !VirtualAllocEx(process,(void *)cursor,length,MEM_RESERVE,PAGE_NOACCESS))return FALSE;
        }
        if(end>=(uintptr_t)0x80000000u)break;
        cursor=(end+info.dwAllocationGranularity-1u)&~((uintptr_t)info.dwAllocationGranularity-1u);
    }
    return TRUE;
}
#endif
int wmain(int argc,wchar_t **argv)
{
    if((argc==2 || argc==3) && !wcscmp(argv[1],L"--child")) {
        BOOL found=FALSE,matched=FALSE;MEMORY_BASIC_INFORMATION allocation;
        void *address=find_payload_allocation();if(!address)return 15;
        if(argc==3 && (!wcscmp(argv[2],L"--expect-high")) && (uintptr_t)address<(uintptr_t)0x80000000u)return 18;
        if(GetModuleHandleW(L"cross-helper.dll") || !original_import_unchanged())return 10;
        /* A damaged matched header must be rejected and never consumed. */
        DWORD *version=(DWORD *)((BYTE *)address+24),saved=*version;
        *version=99;
        if(process_hook_payload_take(consume,&matched,&found)!=ERROR_INVALID_DATA || !found || matched)return 17;
        *version=saved;found=FALSE;
        if(process_hook_payload_take(reject,&matched,&found)!=ERROR_CANCELLED||!found)return 11;
        found=FALSE;
        if(process_hook_payload_take(consume,&matched,&found)||!found||!matched)return 14;
        if(!VirtualQuery(address,&allocation,sizeof(allocation)) || allocation.State!=MEM_FREE)return 16;
        found=TRUE;matched=FALSE;
        if(process_hook_payload_take(consume,&matched,&found)||found)return 12;
        if(!original_import_unchanged())return 13;
        return 42;
    }
    if(argc!=2 && argc!=3)return 2;
    WCHAR line[32768];STARTUPINFOW si={0};PROCESS_INFORMATION pi={0};HANDLE limited=NULL;
    DWORD error=0,code=0;
    if(swprintf_s(line,32768,L"\"%ls\" --child%ls",argv[1],argc==3?L" --expect-high":L"")<0)return 3;
    si.cb=sizeof(si);
    if(!CreateProcessW(argv[1],line,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,NULL,NULL,&si,&pi))return 4;
    if(argc==3) {
#ifdef _WIN64
        if(wcscmp(argv[2],L"--exhaust-low") || !reserve_low_regions(pi.hProcess)){error=9;goto done;}
#else
        error=9;goto done;
#endif
    }
    if(process_hook_payload_write(NULL,payload,sizeof(payload))!=ERROR_INVALID_PARAMETER ||
       process_hook_payload_write(pi.hProcess,payload,0)!=ERROR_INVALID_PARAMETER ||
       process_hook_payload_write(pi.hProcess,payload,65537)!=ERROR_INVALID_PARAMETER) {error=1;goto done;}
    if(!DuplicateHandle(GetCurrentProcess(),pi.hProcess,GetCurrentProcess(),&limited,
       PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,0)){error=2;goto done;}
    {DWORD limitedError=process_hook_payload_write(limited,payload,sizeof(payload));
    if(limitedError!=ERROR_ACCESS_DENIED){fprintf(stderr,"limited handle returned %lu\n",limitedError);error=3;goto done;}}
    CloseHandle(limited);limited=NULL;
    if(!DuplicateHandle(GetCurrentProcess(),pi.hProcess,GetCurrentProcess(),&limited,
       PROCESS_QUERY_INFORMATION|PROCESS_VM_WRITE|PROCESS_VM_OPERATION,FALSE,0)){error=6;goto done;}
    if(process_hook_payload_write(limited,payload,sizeof(payload))!=ERROR_ACCESS_DENIED){error=7;goto done;}
    CloseHandle(limited);limited=NULL;
    {DWORD invalid=process_hook_payload_write(pi.hProcess,(const void *)(ULONG_PTR)1,sizeof(payload));
    if(!invalid || invalid==ERROR_ALREADY_EXISTS){error=8;goto done;}}
    error=process_hook_payload_write(pi.hProcess,payload,sizeof(payload));if(error)goto done;
    if(process_hook_payload_write(pi.hProcess,payload,sizeof(payload))!=ERROR_ALREADY_EXISTS){error=4;goto done;}
    if(ResumeThread(pi.hThread)!=1 || WaitForSingleObject(pi.hProcess,15000)!=WAIT_OBJECT_0 ||
       !GetExitCodeProcess(pi.hProcess,&code) || code!=42){error=5;goto done;}
    printf("DIRECT_PAYLOAD_PASS source=%u duplicate=1 rights=1 corrupt_rejected=1 write_failure_cleanup=1 retry=1 released=1 no_dll=1 imports_unchanged=1\n",(unsigned)(sizeof(void*)*8));
done:
    if(error)fprintf(stderr,"direct payload error=%lu target_exit=%lu\n",error,code);
    if(limited)CloseHandle(limited);
    if(WaitForSingleObject(pi.hProcess,0)==WAIT_TIMEOUT){TerminateProcess(pi.hProcess,1);WaitForSingleObject(pi.hProcess,2000);}
    CloseHandle(pi.hThread);CloseHandle(pi.hProcess);return error?1:0;
}
