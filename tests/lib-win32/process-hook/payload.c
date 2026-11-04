#include "lib-win32/process-hook/staging.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
int main(int argc,char **argv)
{
    if(argc==2){
        BOOL found=FALSE,matched=FALSE;
        MEMORY_BASIC_INFORMATION allocation;
        BYTE *base=(BYTE *)GetModuleHandleW(NULL);
        BYTE *staged=base+headers()->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
        if(GetModuleHandleW(L"ntw16s32.dll") || GetModuleHandleW(L"ntw16s64.dll"))return 10;
        if(process_hook_stage_read(consume,&matched,&found) || !found || !matched)return 11;
        if(process_hook_stage_restore())return 12;
        if(!VirtualQuery(staged,&allocation,sizeof(allocation)) || allocation.State!=MEM_FREE)return 16;
        if(headers()->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress!=strtoul(argv[1],NULL,10))return 13;
        found=TRUE;matched=FALSE;
        if(process_hook_stage_read(consume,&matched,&found) || found)return 14;
        if(GetModuleHandleW(L"ntw16s32.dll") || GetModuleHandleW(L"ntw16s64.dll"))return 15;
        return 42;
    }
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
