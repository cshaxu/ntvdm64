#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "lib-win32/process-hook/helper.h"
#include "lib-win32/process-hook/staging.h"
static const BYTE payload[8]={0x19,0x81,0x03,0x57,0x44,0x35,0x7a,0x21};
static DWORD consume(const void *data,DWORD size,DWORD machine,void *context)
{
    BOOL *good=(BOOL *)context;
    *good=size==sizeof(payload) && !memcmp(data,payload,sizeof(payload)) &&
        machine==(sizeof(void *)==8?IMAGE_FILE_MACHINE_AMD64:IMAGE_FILE_MACHINE_I386);
    return *good?ERROR_SUCCESS:ERROR_INVALID_DATA;
}
int wmain(int argc,wchar_t **argv)
{
    if(argc==2 && !wcscmp(argv[1],L"--child")) {
        BOOL found=FALSE,good=FALSE;
        if(GetModuleHandleW(L"cross-helper.dll"))return 10;
        if(process_hook_stage_read(consume,&good,&found)||!found||!good)return 11;
        if(process_hook_stage_restore())return 12;
        found=TRUE;good=FALSE;
        if(process_hook_stage_read(consume,&good,&found)||found)return 13;
        return 42;
    }
    if(argc!=4)return 2;
    WCHAR line[32768];STARTUPINFOW si={0};PROCESS_INFORMATION pi={0};DWORD error=0,code=0;
    if(swprintf_s(line,32768,L"\"%ls\" --child",argv[1])<0)return 3;
    si.cb=sizeof(si);
    if(!CreateProcessW(argv[1],line,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,NULL,NULL,&si,&pi))return 4;
    if(!wcscmp(argv[3],L"same"))error=process_hook_stage_payload(pi.hProcess,payload,sizeof(payload));
    else if(!wcscmp(argv[3],L"reject")) {
        BYTE invalid[sizeof(payload)];memcpy(invalid,payload,sizeof(invalid));invalid[0]^=1;
        error=process_hook_install_helper(pi.hProcess,argv[2],invalid,sizeof(invalid),CreateProcessW);
        if(error!=ERROR_INVALID_DATA || !GetExitCodeProcess(pi.hProcess,&code) || code!=STILL_ACTIVE) {
            error=ERROR_INVALID_DATA;goto done;
        }
        printf("CROSS_REJECT_PASS source=%u target_alive=1\n",(unsigned)(sizeof(void*)*8));
        error=0;goto done;
    } else error=process_hook_install_helper(pi.hProcess,argv[2],payload,sizeof(payload),CreateProcessW);
    if(error){fwprintf(stderr,L"stage error=%lu source=%u\n",error,(unsigned)(sizeof(void*)*8));goto done;}
    if(ResumeThread(pi.hThread)!=1 || WaitForSingleObject(pi.hProcess,15000)!=WAIT_OBJECT_0 ||
       !GetExitCodeProcess(pi.hProcess,&code) || code!=42){error=ERROR_INVALID_DATA;goto done;}
    printf("CROSS_PAYLOAD_PASS source=%u mode=%ls\n",(unsigned)(sizeof(void*)*8),argv[3]);
done:
    if(WaitForSingleObject(pi.hProcess,0)==WAIT_TIMEOUT){TerminateProcess(pi.hProcess,1);WaitForSingleObject(pi.hProcess,2000);}
    CloseHandle(pi.hThread);CloseHandle(pi.hProcess);return error?1:0;
}
