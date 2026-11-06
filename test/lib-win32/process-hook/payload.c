#include "lib-win32/process-hook/staging.h"
#include "lib-win32/process-hook/helper.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

static const BYTE expected[]={0,1,2,0xfe,0xff,42,0};
static DWORD consume(const void *data,DWORD bytes,DWORD machine,void *context)
{
    BOOL *matched=context;
    *matched=bytes==sizeof(expected) && !memcmp(data,expected,bytes) &&
        machine==(sizeof(void *)==8?IMAGE_FILE_MACHINE_AMD64:IMAGE_FILE_MACHINE_I386);
    return *matched?ERROR_SUCCESS:ERROR_INVALID_DATA;
}
static IMAGE_NT_HEADERS *headers(void)
{
    BYTE *base=(BYTE *)GetModuleHandleW(NULL);
    IMAGE_DOS_HEADER *dos=(IMAGE_DOS_HEADER *)base;
    return (IMAGE_NT_HEADERS *)(base+dos->e_lfanew);
}
static BOOL WINAPI reject_create(LPCWSTR a,LPWSTR b,LPSECURITY_ATTRIBUTES c,LPSECURITY_ATTRIBUTES d,BOOL e,DWORD f,LPVOID g,LPCWSTR h,LPSTARTUPINFOW i,LPPROCESS_INFORMATION j)
{
    (void)a;(void)b;(void)c;(void)d;(void)e;(void)f;(void)g;(void)h;(void)i;(void)j;
    SetLastError(ERROR_ACCESS_DENIED);return FALSE;
}
static int dll_contract(void)
{
    WCHAR self[MAX_PATH],command[MAX_PATH+8];STARTUPINFOW si={.cb=sizeof(si)};PROCESS_INFORMATION pi={0};
    if(!GetModuleFileNameW(NULL,self,MAX_PATH) || swprintf_s(command,ARRAYSIZE(command),L"\"%ls\"",self)<0)return 1;
    if(!CreateProcessW(self,command,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,NULL,NULL,&si,&pi))return 2;
    int result=3;SIZE_T read=0;IMAGE_DOS_HEADER dos;IMAGE_NT_HEADERS nt;IMAGE_IMPORT_DESCRIPTOR descriptor;char name[32];
    BYTE *base=(BYTE *)GetModuleHandleW(NULL);
    /* Same executable, but ASLR may choose a different base: find its image allocation. */
    MEMORY_BASIC_INFORMATION region;uintptr_t cursor=0;
    while(VirtualQueryEx(pi.hProcess,(void *)cursor,&region,sizeof(region))){
        if(region.Type==MEM_IMAGE && region.BaseAddress==region.AllocationBase &&
           ReadProcessMemory(pi.hProcess,region.BaseAddress,&dos,sizeof(dos),&read) && dos.e_magic==IMAGE_DOS_SIGNATURE &&
           ReadProcessMemory(pi.hProcess,(BYTE *)region.BaseAddress+dos.e_lfanew,&nt,sizeof(nt),&read) && !(nt.FileHeader.Characteristics&IMAGE_FILE_DLL)){
            base=region.BaseAddress;break;
        }
        uintptr_t next=(uintptr_t)region.BaseAddress+region.RegionSize;if(next<=cursor)goto done;cursor=next;
    }
    if(process_hook_stage_process(pi.hProcess,"first.dll",expected,sizeof(expected)) ||
       process_hook_stage_process(pi.hProcess,"second.dll",expected,sizeof(expected)))goto done;
    if(!ReadProcessMemory(pi.hProcess,base+dos.e_lfanew,&nt,sizeof(nt),&read) ||
       !ReadProcessMemory(pi.hProcess,base+nt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress,&descriptor,sizeof(descriptor),&read) ||
       !ReadProcessMemory(pi.hProcess,base+descriptor.Name,name,sizeof("second.dll"),&read) || strcmp(name,"second.dll"))goto done;
    if(process_hook_stage_process(pi.hProcess,"second.dll",expected,sizeof(expected)))goto done;
    result=0;
done:
    TerminateProcess(pi.hProcess,0);WaitForSingleObject(pi.hProcess,2000);CloseHandle(pi.hThread);CloseHandle(pi.hProcess);
    printf("STAGING_DLL_%s\n",result?"FAIL":"PASS");return result;
}
int main(int argc,char **argv)
{
    if(argc==2 && !strcmp(argv[1],"dll-contract"))return dll_contract();
    if(argc==2 && !strcmp(argv[1],"helper-contract")){
        DWORD e=process_hook_install_helper(GetCurrentProcess(),L"unused.dll",expected,sizeof(expected),reject_create);
        printf("HELPER_LENGTH error=%lu\n",e);return e==ERROR_INVALID_PARAMETER?0:1;
    }
    if(argc==2){
        BOOL found=FALSE,matched=FALSE;
        MEMORY_BASIC_INFORMATION allocation;
        BYTE *base=(BYTE *)GetModuleHandleW(NULL);
        BYTE *staged=base+headers()->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
        if(GetModuleHandleW(L"first.dll") || GetModuleHandleW(L"second.dll"))return 10;
        if(process_hook_stage_read(consume,&matched,&found) || !found || !matched)return 11;
        if(process_hook_stage_restore())return 12;
        if(!VirtualQuery(staged,&allocation,sizeof(allocation)) || allocation.State!=MEM_FREE)return 16;
        if(headers()->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress!=strtoul(argv[1],NULL,10))return 13;
        found=TRUE;matched=FALSE;
        if(process_hook_stage_read(consume,&matched,&found) || found)return 14;
        if(GetModuleHandleW(L"first.dll") || GetModuleHandleW(L"second.dll"))return 15;
        return 42;
    }
    if(dll_contract())return 17;
    if(process_hook_install_helper(GetCurrentProcess(),L"unused.dll",expected,sizeof(expected),reject_create)!=ERROR_INVALID_PARAMETER)return 18;
    WCHAR self[MAX_PATH],command[MAX_PATH+32];
    STARTUPINFOW startup={.cb=sizeof(startup)};PROCESS_INFORMATION child={0};
    DWORD result=1,exit_code=0,original=headers()->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if(process_hook_stage_payload(NULL,expected,sizeof(expected))!=ERROR_INVALID_PARAMETER)return 1;
    if(!GetModuleFileNameW(NULL,self,MAX_PATH) || swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" %lu",self,original)<0)return 2;
    if(!CreateProcessW(self,command,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,NULL,NULL,&startup,&child))return 3;
    if(process_hook_stage_payload(child.hProcess,expected,0)!=ERROR_INVALID_PARAMETER)goto done;
    static const BYTE superseded[]={9,8,7};
    if(process_hook_stage_payload(child.hProcess,superseded,sizeof(superseded)))goto done;
    DWORD error=process_hook_stage_payload(child.hProcess,expected,sizeof(expected));
    if(error){fprintf(stderr,"payload staging error=%lu\n",error);goto done;}
    if(process_hook_stage_payload(child.hProcess,expected,sizeof(expected)))goto done;
    if(ResumeThread(child.hThread)!=1 || WaitForSingleObject(child.hProcess,10000)!=WAIT_OBJECT_0 ||
       !GetExitCodeProcess(child.hProcess,&exit_code) || exit_code!=42){fprintf(stderr,"payload child exit=%lu error=%lu\n",exit_code,GetLastError());goto done;}
    printf("PAYLOAD_ONLY_PASS bits=%u restored=1 released=1 dll_absent=1 repeated=1\n",(unsigned)(sizeof(void *)*8));result=0;
done:
    if(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT){TerminateProcess(child.hProcess,1);WaitForSingleObject(child.hProcess,2000);}
    CloseHandle(child.hThread);CloseHandle(child.hProcess);return (int)result;
}
