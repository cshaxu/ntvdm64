#include "nthook32-dll/hook.h"
#include "nthook32-dll/detours/detours.h"
#include <stdio.h>
#include <wchar.h>
static unsigned assertions;
static HANDLE fault_target,fault_helper;
static unsigned helper_fault;
extern const GUID nthook_payload_guid;
static void check(BOOL condition,PCSTR message)
{
    ++assertions;
    if(!condition){fprintf(stderr,"FAIL: %s (%lu)\n",message,GetLastError());ExitProcess(1);}
}
static DWORD child(PCWSTR mode,PCWSTR next=NULL,PCWSTR final=NULL)
{
    if(!wcscmp(mode,L"held-helper")) {
        HANDLE never=CreateEventW(NULL,TRUE,FALSE,NULL);
        if(!never)return 31;
        WaitForSingleObject(never,INFINITE);CloseHandle(never);return 32;
    }
    nthook_context context;BOOL found=FALSE;
    DWORD context_error=nthook_context_read(&context,&found);
    if(!wcscmp(mode,L"invalid"))return found && context_error==ERROR_INVALID_DATA ? 0 : 28;
    if(context_error || !found)return 10;
    HMODULE hook=GetModuleHandleW(sizeof(void *)==8 ? L"nthook64.dll" : L"nthook32.dll");
    if(!wcscmp(mode,L"context")) {
        if(hook || context.mode!=NATIVE_HOOK_LAUNCHER)return 11;
        DWORD flags;
        if(!GetHandleInformation(context.frontend,&flags) || flags&HANDLE_FLAG_INHERIT ||
           !SetEvent(context.frontend) || !SetEvent(context.execution))return 12;
        return 0;
    }
    if(!hook || context.mode!=NATIVE_HOOK_INTERCEPT)return 13;
    auto read=(DWORD (WINAPI *)(void))GetProcAddress(hook,"NthookContextFlags");
    if(!read || (read()&0xff)!=NATIVE_HOOK_INTERCEPT)return 14;
    if(!wcscmp(mode,L"gui") && context.frontend)return 15;
    if(!wcscmp(mode,L"grandchild"))return 0;
    if(!wcscmp(mode,L"attributes-leaf")) {
        WCHAR value[128],directory[MAX_PATH],windows[MAX_PATH];DWORD written;
        if(!GetEnvironmentVariableW(L"NTHOOK_TEST",value,128) || wcscmp(value,L"\x4e2d\x6587"))return 21;
        GetCurrentDirectoryW(MAX_PATH,directory);GetWindowsDirectoryW(windows,MAX_PATH);
        if(_wcsicmp(directory,windows))return 22;
        if(!GetEnvironmentVariableW(L"NTHOOK_INCLUDED",value,128) ||
           !SetEvent((HANDLE)(ULONG_PTR)_wcstoui64(value,NULL,10)))return 23;
        if(GetEnvironmentVariableW(L"NTHOOK_EXCLUDED",value,128))
            SetEvent((HANDLE)(ULONG_PTR)_wcstoui64(value,NULL,10));
        return WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),"UNICODE-ATTRIBUTES",18,&written,NULL) && written==18 ? 0 : 24;
    }
    WCHAR image[MAX_PATH],command[2*MAX_PATH];PROCESS_INFORMATION process={0};
    STARTUPINFOW startup={sizeof(startup)};
    GetModuleFileNameW(NULL,image,MAX_PATH);
    WCHAR application[MAX_PATH];wcscpy_s(application,image);
    swprintf_s(command,L"\"%ls\" grandchild",image);
    if(!wcscmp(mode,L"attributes")) {
        SECURITY_ATTRIBUTES security={sizeof(security),NULL,TRUE};
        HANDLE included=CreateEventW(&security,TRUE,FALSE,NULL),excluded=CreateEventW(&security,TRUE,FALSE,NULL);
        HANDLE input=CreateFileW(L"NUL",GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,&security,OPEN_EXISTING,0,NULL);
        HANDLE output=NULL,reader=NULL;SIZE_T bytes=0;DWORD result=25,count=0;
        if(!included || !excluded || input==INVALID_HANDLE_VALUE || !CreatePipe(&reader,&output,&security,0))return 25;
        SetHandleInformation(reader,HANDLE_FLAG_INHERIT,0);
        STARTUPINFOEXW extended={0};extended.StartupInfo.cb=sizeof(extended);
        InitializeProcThreadAttributeList(NULL,1,0,&bytes);
        extended.lpAttributeList=(LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(),0,bytes);
        HANDLE list[3]={included,input,output};
        WCHAR environment[256],windows[MAX_PATH];
        int length=swprintf_s(environment,L"NTHOOK_TEST=\x4e2d\x6587");
        length+=1+swprintf_s(environment+length+1,256-length-1,L"NTHOOK_INCLUDED=%llu",(unsigned long long)(ULONG_PTR)included);
        length+=1+swprintf_s(environment+length+1,256-length-1,L"NTHOOK_EXCLUDED=%llu",(unsigned long long)(ULONG_PTR)excluded);
        environment[length+1]=0;GetWindowsDirectoryW(windows,MAX_PATH);
        extended.StartupInfo.dwFlags=STARTF_USESTDHANDLES;
        extended.StartupInfo.hStdInput=input;extended.StartupInfo.hStdOutput=extended.StartupInfo.hStdError=output;
        swprintf_s(command,L"\"%ls\" attributes-leaf",image);
        BOOL initialized=extended.lpAttributeList && InitializeProcThreadAttributeList(extended.lpAttributeList,1,0,&bytes);
        BOOL created=initialized && UpdateProcThreadAttribute(extended.lpAttributeList,0,PROC_THREAD_ATTRIBUTE_HANDLE_LIST,list,sizeof(list),NULL,NULL) &&
            CreateProcessW(image,command,NULL,NULL,TRUE,CREATE_SUSPENDED|CREATE_UNICODE_ENVIRONMENT|EXTENDED_STARTUPINFO_PRESENT,
                environment,windows,&extended.StartupInfo,&process);
        if(initialized)DeleteProcThreadAttributeList(extended.lpAttributeList);
        if(extended.lpAttributeList)HeapFree(GetProcessHeap(),0,extended.lpAttributeList);
        CloseHandle(output);CloseHandle(input);
        if(created) {
            if(ResumeThread(process.hThread)!=1)result=26;
            else if(WaitForSingleObject(process.hProcess,10000)==WAIT_OBJECT_0)GetExitCodeProcess(process.hProcess,&result);
            else TerminateProcess(process.hProcess,25);
            CloseHandle(process.hThread);CloseHandle(process.hProcess);
            char text[32]={0};ReadFile(reader,text,sizeof(text),&count,NULL);
            if(!result && (count!=18 || memcmp(text,"UNICODE-ATTRIBUTES",18) ||
                WaitForSingleObject(included,0)!=WAIT_OBJECT_0 || WaitForSingleObject(excluded,0)!=WAIT_TIMEOUT))result=27;
        }
        CloseHandle(reader);CloseHandle(included);CloseHandle(excluded);return result;
    }
    if(!wcscmp(mode,L"alternate")) {
        if(!next || !final)return 33;
        wcscpy_s(application,next);
        swprintf_s(command,L"\"%ls\" alternate-leaf \"%ls\"",next,final);
    } else if(!wcscmp(mode,L"alternate-leaf")) {
        if(!next)return 34;
        wcscpy_s(application,next);
        swprintf_s(command,L"\"%ls\" grandchild",next);
    } else if(!wcscmp(mode,L"cmd")) {
        WCHAR windows[MAX_PATH];if(!GetWindowsDirectoryW(windows,MAX_PATH))return 19;
        swprintf_s(application,L"%ls\\%ls\\cmd.exe",windows,sizeof(void *)==8 ? L"System32" : L"SysWOW64");
        swprintf_s(command,L"\"%ls\" /d /s /c \"\"%ls\" grandchild\"",application,image);
    } else if(!wcscmp(mode,L"gui-to-cui") || !wcscmp(mode,L"cui-to-gui")) {
        WCHAR *slash=wcsrchr(application,L'\\');
        wcscpy_s(slash+1,MAX_PATH-(slash+1-application),
            !wcscmp(mode,L"gui-to-cui") ? L"nthook-install-test.exe" : L"nthook-gui-test.exe");
        swprintf_s(command,L"\"%ls\" %ls",application,
            !wcscmp(mode,L"cui-to-gui") ? L"gui" : L"grandchild");
    }
    if(!wcscmp(mode,L"ansi")) {
        char ansi_image[MAX_PATH*2],ansi_command[MAX_PATH*4];STARTUPINFOA ansi_startup={sizeof(ansi_startup)};
        if(!WideCharToMultiByte(CP_ACP,0,application,-1,ansi_image,sizeof(ansi_image),NULL,NULL) ||
           !WideCharToMultiByte(CP_ACP,0,command,-1,ansi_command,sizeof(ansi_command),NULL,NULL))return 20;
        if(!CreateProcessA(ansi_image,ansi_command,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&ansi_startup,&process))return 16;
    } else if(!CreateProcessW(application,command,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&startup,&process))return 16;
    if(WaitForSingleObject(process.hProcess,0)!=WAIT_TIMEOUT)return 17;
    ResumeThread(process.hThread);CloseHandle(process.hThread);
    DWORD result=18;
    if(WaitForSingleObject(process.hProcess,10000)==WAIT_OBJECT_0)GetExitCodeProcess(process.hProcess,&result);
    else TerminateProcess(process.hProcess,18);
    CloseHandle(process.hProcess);return result;
}
static DWORD run(PCWSTR image,PCWSTR mode,nthook_context *context,DWORD install_mode)
{
    WCHAR command[2*MAX_PATH];PROCESS_INFORMATION process={0};STARTUPINFOW startup={sizeof(startup)};
    swprintf_s(command,L"\"%ls\" %ls",image,mode);
    check(CreateProcessW(image,command,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&startup,&process),"create suspended no-inherit child");
    DWORD error=nthook_install(process.hProcess,context,install_mode),result=MAXDWORD;
    if(error) {
        check(TerminateProcess(process.hProcess,error),"abort exact unpublished fixture child");
        check(WaitForSingleObject(process.hProcess,10000)==WAIT_OBJECT_0,"join fixture rollback");
    }
    else {
        check(WaitForSingleObject(process.hProcess,0)==WAIT_TIMEOUT,"installer did not resume child");
        check(ResumeThread(process.hThread)==1,"caller suspension preserved");
        DWORD wait=WaitForSingleObject(process.hProcess,10000);
        if(wait!=WAIT_OBJECT_0)TerminateProcess(process.hProcess,ERROR_TIMEOUT);
        check(wait==WAIT_OBJECT_0,"child completed without timed retry");
        check(GetExitCodeProcess(process.hProcess,&result),"real Windows exit code");
    }
    CloseHandle(process.hThread);CloseHandle(process.hProcess);
    printf("case=%ls install=%lu exit=%lu\n",mode,error,result);fflush(stdout);
    return error ? error : result;
}
static BOOL WINAPI helper_create(LPCWSTR,LPWSTR,LPSECURITY_ATTRIBUTES pa,
    LPSECURITY_ATTRIBUTES ta,BOOL inherit,DWORD flags,LPVOID environment,
    LPCWSTR directory,LPSTARTUPINFOW startup,LPPROCESS_INFORMATION process)
{
    if(helper_fault==0){SetLastError(ERROR_ACCESS_DENIED);return FALSE;}
    WCHAR image[MAX_PATH],command[MAX_PATH+32];
    if(helper_fault==2 && !TerminateProcess(fault_target,37))return FALSE;
    GetModuleFileNameW(NULL,image,MAX_PATH);
    swprintf_s(command,L"\"%ls\" held-helper",image);
    BOOL result=CreateProcessW(image,command,pa,ta,inherit,flags,environment,directory,startup,process);
    if(result)check(DuplicateHandle(GetCurrentProcess(),process->hProcess,GetCurrentProcess(),
        &fault_helper,SYNCHRONIZE,FALSE,0),"pin exact injected installer for cleanup proof");
    return result;
}
static void helper_failures(PCWSTR image,const nthook_context *context)
{
    char dll[MAX_PATH*2];LPCSTR imports[1]={dll};
    check(WideCharToMultiByte(CP_ACP,0,sizeof(void *)==8 ? context->hook : context->hook64,
        -1,dll,sizeof(dll),NULL,NULL),"encode opposite-width installer path");
    for(helper_fault=0;helper_fault<3;++helper_fault) {
        STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION target={0};
        WCHAR command[MAX_PATH+32];swprintf_s(command,L"\"%ls\" grandchild",image);
        check(CreateProcessW(image,command,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&startup,&target),
            "create exact suspended target for installer failure");
        fault_target=target.hProcess;fault_helper=NULL;
        ULONGLONG started=GetTickCount64();
        check(!DetourProcessViaHelperDllsW(target.dwProcessId,1,imports,helper_create),
            "installer create/timeout/dead-target must fail");
        DWORD error=GetLastError();ULONGLONG elapsed=GetTickCount64()-started;
        check(error==(helper_fault==0 ? ERROR_ACCESS_DENIED :
            helper_fault==1 ? ERROR_TIMEOUT : ERROR_PROCESS_ABORTED),"preserve concrete installer failure");
        if(helper_fault==1)check(elapsed>=9900 && elapsed<15000,"real ten-second finite installer deadline");
        if(fault_helper) {
            check(WaitForSingleObject(fault_helper,0)==WAIT_OBJECT_0,"exact owned helper joined before return");
            CloseHandle(fault_helper);fault_helper=NULL;
        }
        if(helper_fault!=2) {
            check(WaitForSingleObject(target.hProcess,0)==WAIT_TIMEOUT,"installer failure does not kill target");
            check(SuspendThread(target.hThread)==1,"installer failure leaves target suspension unchanged");
            check(ResumeThread(target.hThread)==2,"undo fixture inspection without starting target");
            check(TerminateProcess(target.hProcess,37),"creator aborts exact unpublished target");
        }
        check(WaitForSingleObject(target.hProcess,10000)==WAIT_OBJECT_0,"join exact fixture target");
        CloseHandle(target.hThread);CloseHandle(target.hProcess);fault_target=NULL;
        printf("helper-fault=%u error=%lu elapsed=%llu\n",helper_fault,error,elapsed);
    }
}
static void malformed(PCWSTR image,const nthook_context *context,unsigned mutation)
{
    BYTE data[sizeof(native_hook_packet)+3*MAX_PATH*sizeof(WCHAR)]={0};
    native_hook_packet *header=(native_hook_packet *)data;
    header->version=NATIVE_HOOK_VERSION;header->mode=NATIVE_HOOK_LAUNCHER;
    header->machine=sizeof(void *)==8 ? IMAGE_FILE_MACHINE_AMD64 : IMAGE_FILE_MACHINE_I386;
    header->launcher_offset=sizeof(*header);
    header->launcher_bytes=(DWORD)((wcslen(context->launcher)+1)*sizeof(WCHAR));
    header->hook_offset=header->launcher_offset+header->launcher_bytes;
    header->hook_bytes=(DWORD)((wcslen(context->hook)+1)*sizeof(WCHAR));
    header->hook64_offset=header->hook_offset+header->hook_bytes;
    header->hook64_bytes=(DWORD)((wcslen(context->hook64)+1)*sizeof(WCHAR));
    header->bytes=header->hook64_offset+header->hook64_bytes;
    memcpy(data+header->launcher_offset,context->launcher,header->launcher_bytes);
    memcpy(data+header->hook_offset,context->hook,header->hook_bytes);
    memcpy(data+header->hook64_offset,context->hook64,header->hook64_bytes);
    DWORD bytes=header->bytes;
    switch(mutation) {
    case 0:++header->version;break;
    case 1:header->mode=99;break;
    case 2:header->flags=1;break;
    case 3:header->frontend=1;break;
    case 4:header->hook_offset=MAXDWORD;break;
    case 5:header->launcher_bytes=MAXDWORD;break;
    case 6:data[bytes-2]=1;break;
    case 7:bytes=sizeof(*header)-1;break;
    }
    WCHAR command[2*MAX_PATH];STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION process={0};
    swprintf_s(command,L"\"%ls\" invalid",image);
    check(CreateProcessW(image,command,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&startup,&process),"malformed context isolated suspended child");
    check(DetourCopyPayloadToProcess(process.hProcess,nthook_payload_guid,data,bytes),"copy malformed fixture payload");
    check(ResumeThread(process.hThread)==1,"resume malformed fixture");
    DWORD wait=WaitForSingleObject(process.hProcess,10000),result=MAXDWORD;
    if(wait!=WAIT_OBJECT_0)TerminateProcess(process.hProcess,ERROR_TIMEOUT);
    GetExitCodeProcess(process.hProcess,&result);CloseHandle(process.hThread);CloseHandle(process.hProcess);
    check(wait==WAIT_OBJECT_0 && result==0,"malformed payload rejected, not interpreted as absent");
}
int wmain(int argc,WCHAR **argv)
{
    if(argc==4 && !wcscmp(argv[1],L"alternate"))return (int)child(argv[1],argv[2],argv[3]);
    if(argc==3 && !wcscmp(argv[1],L"alternate-leaf"))return (int)child(argv[1],argv[2]);
    if(argc==2)return (int)child(argv[1]);
    nthook_context context;WCHAR image[MAX_PATH],gui[MAX_PATH],*slash;
    check(!nthook_context_paths(&context),"own-package pinned paths");
    GetModuleFileNameW(NULL,image,MAX_PATH);wcscpy_s(gui,image);
    slash=wcsrchr(gui,L'\\');wcscpy_s(slash+1,MAX_PATH-(slash+1-gui),L"nthook-gui-test.exe");
    context.frontend=CreateEventW(NULL,TRUE,FALSE,NULL);
    context.execution=CreateEventW(NULL,TRUE,FALSE,NULL);
    check(context.frontend && context.execution,"private fixture events");
    check(run(image,L"context",&context,NATIVE_HOOK_LAUNCHER)==0,"context-only payload/no DLL");
    check(WaitForSingleObject(context.frontend,0)==WAIT_OBJECT_0 &&
          WaitForSingleObject(context.execution,0)==WAIT_OBJECT_0,"recipient-local duplicated events");
    check(run(image,L"nested",&context,NATIVE_HOOK_INTERCEPT)==0,"CUI immediate descendant propagated");
    check(run(image,L"ansi",&context,NATIVE_HOOK_INTERCEPT)==0,"ANSI native child propagated");
    check(run(image,L"cmd",&context,NATIVE_HOOK_INTERCEPT)==0,"actual matching-width CMD propagated immediate child");
    check(run(image,L"attributes",&context,NATIVE_HOOK_INTERCEPT)==0,"explicit handle list/Unicode environment/CWD/streams preserved");
    check(run(image,L"cui-to-gui",&context,NATIVE_HOOK_INTERCEPT)==0,"CUI to GUI strips text authority");
    CloseHandle(context.frontend);CloseHandle(context.execution);
    context.frontend=context.execution=NULL;
    check(run(gui,L"nested",&context,NATIVE_HOOK_INTERCEPT)==0,"GUI immediate descendant propagated");
    check(run(gui,L"gui-to-cui",&context,NATIVE_HOOK_INTERCEPT)==0,"GUI to CUI propagates compatibility");
    check(run(gui,L"gui",&context,NATIVE_HOOK_INTERCEPT)==0,"GUI carries no text capability");
    if(argc==3) {
        WCHAR alternating[2*MAX_PATH];
        swprintf_s(alternating,L"alternate \"%ls\" \"%ls\"",argv[1],image);
        check(run(image,alternating,&context,NATIVE_HOOK_INTERCEPT)==0,
            "ordinary alternating native-width descendants retain actual handles, suspension and matching Hooks");
        check(run(argv[1],L"nested",&context,NATIVE_HOOK_INTERCEPT)==0,
            "actual opposite-width target loads matching Hook before its immediate child");
        check(run(argv[2],L"nested",&context,NATIVE_HOOK_INTERCEPT)==0,
            "actual opposite-width GUI target propagates matching Hook");
        check(run(argv[2],L"gui",&context,NATIVE_HOOK_INTERCEPT)==0,
            "opposite-width GUI has no character frontend authority");
        helper_failures(image,&context);
    }
    for(unsigned i=0;i<8;++i)malformed(image,&context,i);
    context.frontend=(HANDLE)(ULONG_PTR)0x7ffffffe;context.execution=(HANDLE)(ULONG_PTR)0x7ffffffd;
    check(run(image,L"grandchild",&context,NATIVE_HOOK_LAUNCHER)==ERROR_INVALID_HANDLE,"invalid capability fails before handoff");
    context.frontend=context.execution=NULL;
    if(sizeof(void *)==8)wcscat_s(context.hook64,L".missing");
    else wcscat_s(context.hook,L".missing");
    check(run(image,L"grandchild",&context,NATIVE_HOOK_INTERCEPT)==ERROR_FILE_NOT_FOUND,"missing DLL fails before handoff");
    printf("NTHOOK-INSTALL-PASS assertions=%u\n",assertions);return 0;
}
