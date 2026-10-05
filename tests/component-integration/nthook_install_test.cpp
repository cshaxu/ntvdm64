#include "nthook32-dll/hook.h"
#include "nthook32-dll/detours/detours.h"
#include <stdio.h>
#include <wchar.h>
static unsigned assertions;
extern const GUID nthook_payload_guid;
static void check(BOOL condition,PCSTR message)
{
    ++assertions;
    if(!condition){fprintf(stderr,"FAIL: %s (%lu)\n",message,GetLastError());ExitProcess(1);}
}
static DWORD child(PCWSTR mode)
{
    nthook_context context;BOOL found=FALSE;
    DWORD context_error=nthook_context_read(&context,&found);
    if(!wcscmp(mode,L"invalid"))return found && context_error==ERROR_INVALID_DATA ? 0 : 28;
    if(context_error || !found)return 10;
    HMODULE hook=GetModuleHandleW(L"nthook32.dll");
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
           !SetEvent((HANDLE)(ULONG_PTR)wcstoul(value,NULL,10)))return 23;
        if(GetEnvironmentVariableW(L"NTHOOK_EXCLUDED",value,128))
            SetEvent((HANDLE)(ULONG_PTR)wcstoul(value,NULL,10));
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
        length+=1+swprintf_s(environment+length+1,256-length-1,L"NTHOOK_INCLUDED=%lu",(DWORD)(ULONG_PTR)included);
        length+=1+swprintf_s(environment+length+1,256-length-1,L"NTHOOK_EXCLUDED=%lu",(DWORD)(ULONG_PTR)excluded);
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
    if(!wcscmp(mode,L"cmd")) {
        WCHAR windows[MAX_PATH];if(!GetWindowsDirectoryW(windows,MAX_PATH))return 19;
        swprintf_s(application,L"%ls\\SysWOW64\\cmd.exe",windows);
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
    if(error)TerminateProcess(process.hProcess,error);
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
static void malformed(PCWSTR image,const nthook_context *context,unsigned mutation)
{
    BYTE data[sizeof(native_hook_packet)+2*MAX_PATH*sizeof(WCHAR)]={0};
    native_hook_packet *header=(native_hook_packet *)data;
    header->version=NATIVE_HOOK_VERSION;header->mode=NATIVE_HOOK_LAUNCHER;
    header->machine=IMAGE_FILE_MACHINE_I386;header->launcher_offset=sizeof(*header);
    header->launcher_bytes=(DWORD)((wcslen(context->launcher)+1)*sizeof(WCHAR));
    header->hook_offset=header->launcher_offset+header->launcher_bytes;
    header->hook_bytes=(DWORD)((wcslen(context->hook)+1)*sizeof(WCHAR));
    header->bytes=header->hook_offset+header->hook_bytes;
    memcpy(data+header->launcher_offset,context->launcher,header->launcher_bytes);
    memcpy(data+header->hook_offset,context->hook,header->hook_bytes);
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
    check(run(image,L"cmd",&context,NATIVE_HOOK_INTERCEPT)==0,"actual x86 CMD propagated immediate child");
    check(run(image,L"attributes",&context,NATIVE_HOOK_INTERCEPT)==0,"explicit handle list/Unicode environment/CWD/streams preserved");
    check(run(image,L"cui-to-gui",&context,NATIVE_HOOK_INTERCEPT)==0,"CUI to GUI strips text authority");
    CloseHandle(context.frontend);CloseHandle(context.execution);
    context.frontend=context.execution=NULL;
    check(run(gui,L"nested",&context,NATIVE_HOOK_INTERCEPT)==0,"GUI immediate descendant propagated");
    check(run(gui,L"gui-to-cui",&context,NATIVE_HOOK_INTERCEPT)==0,"GUI to CUI propagates compatibility");
    check(run(gui,L"gui",&context,NATIVE_HOOK_INTERCEPT)==0,"GUI carries no text capability");
    for(unsigned i=0;i<8;++i)malformed(image,&context,i);
    context.frontend=(HANDLE)(ULONG_PTR)0x7ffffffe;context.execution=(HANDLE)(ULONG_PTR)0x7ffffffd;
    check(run(image,L"grandchild",&context,NATIVE_HOOK_LAUNCHER)==ERROR_INVALID_HANDLE,"invalid capability fails before handoff");
    context.frontend=context.execution=NULL;
    wcscat_s(context.hook,L".missing");
    check(run(image,L"grandchild",&context,NATIVE_HOOK_INTERCEPT)==ERROR_FILE_NOT_FOUND,"missing DLL fails before handoff");
    printf("NTHOOK-INSTALL-PASS assertions=%u\n",assertions);return 0;
}
