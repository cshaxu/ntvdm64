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
    HMODULE hook=GetModuleHandleW(
#if defined(_WIN64)
        L"nthook64.dll"
#else
        L"nthook32.dll"
#endif
    );
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
    if(!wcscmp(mode,L"exit37"))return 37;
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
    if(!wcscmp(mode,L"attributes") || !wcscmp(mode,L"cross-attributes")) {
        if(!wcscmp(mode,L"cross-attributes") &&
            !GetEnvironmentVariableW(L"NTHOOK_OPPOSITE",application,MAX_PATH))return 29;
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
        swprintf_s(command,L"\"%ls\" attributes-leaf",application);
        BOOL initialized=extended.lpAttributeList && InitializeProcThreadAttributeList(extended.lpAttributeList,1,0,&bytes);
        BOOL created=initialized && UpdateProcThreadAttribute(extended.lpAttributeList,0,PROC_THREAD_ATTRIBUTE_HANDLE_LIST,list,sizeof(list),NULL,NULL) &&
            CreateProcessW(application,command,NULL,NULL,TRUE,CREATE_SUSPENDED|CREATE_UNICODE_ENVIRONMENT|EXTENDED_STARTUPINFO_PRESENT,
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
        swprintf_s(application,L"%ls\\%ls\\cmd.exe",windows,
#if defined(_WIN64)
            L"System32"
#else
            L"SysWOW64"
#endif
        );
        swprintf_s(command,L"\"%ls\" /d /s /c \"\"%ls\" grandchild\"",application,image);
    } else if(!wcscmp(mode,L"gui-to-cui") || !wcscmp(mode,L"cui-to-gui")) {
        WCHAR *slash=wcsrchr(application,L'\\');
        wcscpy_s(slash+1,MAX_PATH-(slash+1-application),
            !wcscmp(mode,L"gui-to-cui") ? L"nthook-install-test.exe" : L"nthook-gui-test.exe");
        swprintf_s(command,L"\"%ls\" %ls",application,
            !wcscmp(mode,L"cui-to-gui") ? L"gui" : L"grandchild");
    }
    if(!wcsncmp(mode,L"cross",5)) {
        BOOL returning=!wcscmp(mode,L"cross-return");
        if(!GetEnvironmentVariableW(returning ? L"NTHOOK_RETURN" : L"NTHOOK_OPPOSITE",
            application,MAX_PATH))return 29;
        if(!wcscmp(mode,L"cross-gui")) {
            WCHAR *slash=wcsrchr(application,L'\\');
            wcscpy_s(slash+1,MAX_PATH-(slash+1-application),L"nthook-gui-test.exe");
        }
        swprintf_s(command,L"\"%ls\" %ls",application,
            returning ? L"grandchild" : (!wcscmp(mode,L"cross-gui") ? L"gui" :
                (!wcscmp(mode,L"cross-exit") ? L"exit37" : L"cross-return")));
    }
    if(!wcscmp(mode,L"ansi") || !wcscmp(mode,L"cross-ansi")) {
        char ansi_image[MAX_PATH*2],ansi_command[MAX_PATH*4];STARTUPINFOA ansi_startup={sizeof(ansi_startup)};
        if(!WideCharToMultiByte(CP_ACP,0,application,-1,ansi_image,sizeof(ansi_image),NULL,NULL) ||
           !WideCharToMultiByte(CP_ACP,0,command,-1,ansi_command,sizeof(ansi_command),NULL,NULL))return 20;
        if(!CreateProcessA(ansi_image,ansi_command,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&ansi_startup,&process))return 16;
    } else if(!CreateProcessW(application,command,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&startup,&process))return 16;
    if(WaitForSingleObject(process.hProcess,0)!=WAIT_TIMEOUT)return 17;
    if(ResumeThread(process.hThread)!=1){TerminateProcess(process.hProcess,17);
        CloseHandle(process.hThread);CloseHandle(process.hProcess);return 17;}
    CloseHandle(process.hThread);
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
/* Callback-only fault injection at the existing Detours creation seam.
 * Production gets no test flag, scheduler or substitute helper executable. */
static unsigned helper_fault;
static HANDLE helper_target,helper_observed;
static BOOL WINAPI helper_create(LPCWSTR application,LPWSTR command,
    LPSECURITY_ATTRIBUTES process_attributes,LPSECURITY_ATTRIBUTES thread_attributes,
    BOOL inherit,DWORD flags,LPVOID environment,LPCWSTR directory,
    LPSTARTUPINFOW startup,LPPROCESS_INFORMATION process)
{
    check(!inherit && flags==CREATE_SUSPENDED &&
        startup->dwFlags&STARTF_USESHOWWINDOW && startup->wShowWindow==SW_HIDE,
        "helper is hidden suspended/no inherited authority");
    WCHAR windows[MAX_PATH],expected[MAX_PATH];GetWindowsDirectoryW(windows,MAX_PATH);
    swprintf_s(expected,L"%ls\\%ls\\rundll32.exe",windows,
#if defined(_WIN64)
        L"syswow64"
#else
        L"sysnative"
#endif
    );
    check(!_wcsicmp(application,expected),"matching real Windows loader, not WINDIR input");
    if(helper_fault==0){SetLastError(ERROR_ACCESS_DENIED);return FALSE;}
    BOOL created;
    if(helper_fault==3) {
        WCHAR image[MAX_PATH],line[2*MAX_PATH];GetModuleFileNameW(NULL,image,MAX_PATH);
        swprintf_s(line,L"\"%ls\" grandchild",image);
        created=CreateProcessW(image,line,process_attributes,thread_attributes,inherit,
            flags,environment,directory,startup,process);
    }else created=CreateProcessW(application,command,process_attributes,thread_attributes,inherit,
        flags,environment,directory,startup,process);
    if(!created)return FALSE;
    check(DuplicateHandle(GetCurrentProcess(),process->hProcess,GetCurrentProcess(),
        &helper_observed,SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,0),
        "pin exactly this test-owned helper");
    if(helper_fault==1)check(SuspendThread(process->hThread)==1,"hold helper across single resume");
    if(helper_fault==2)check(TerminateProcess(helper_target,41) &&
        WaitForSingleObject(helper_target,5000)==WAIT_OBJECT_0,"target death before helper wait");
    return TRUE;
}
static void helper_failures(PCWSTR opposite,const nthook_context *context)
{
    WCHAR saved[MAX_PATH];DWORD length=GetEnvironmentVariableW(L"WINDIR",saved,MAX_PATH);
    check(length<MAX_PATH && SetEnvironmentVariableW(L"WINDIR",L"Z:\\not-the-host"),
        "poison only fixture WINDIR");
    LPCSTR dlls[1];char dll[MAX_PATH*2];
#if defined(_WIN64)
    PCWSTR hook=context->hook;
#else
    PCWSTR hook=context->hook64;
#endif
    check(WideCharToMultiByte(CP_ACP,0,hook,-1,dll,sizeof(dll),NULL,NULL)!=0,"helper fixture DLL path");
    dlls[0]=dll;
    for(helper_fault=0;helper_fault<4;++helper_fault) {
        STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION target={0};
        WCHAR command[2*MAX_PATH];swprintf_s(command,L"\"%ls\" grandchild",opposite);
        check(CreateProcessW(opposite,command,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,
            &startup,&target),"fault owns unpublished suspended target");
        helper_target=target.hProcess;helper_observed=NULL;
        DWORD before=0,after=0;GetProcessHandleCount(GetCurrentProcess(),&before);
        ULONGLONG started=GetTickCount64();
        BOOL installed=DetourProcessViaHelperDllsW(target.dwProcessId,1,dlls,helper_create);
        DWORD error=GetLastError();ULONGLONG elapsed=GetTickCount64()-started;
        check(!installed && error==(helper_fault==0 ? ERROR_ACCESS_DENIED :
            (helper_fault==1 ? ERROR_TIMEOUT : (helper_fault==2 ? ERROR_PROCESS_ABORTED : ERROR_DLL_INIT_FAILED))),
            "concrete helper failure, never successful unhooked target");
        check(elapsed<15000,"helper installation failure has finite budget");
        if(helper_fault==1)check(elapsed>=9000,"timeout uses actual production ten-second deadline");
        if(helper_observed) {
            check(WaitForSingleObject(helper_observed,0)==WAIT_OBJECT_0,"failed helper joined before return");
            CloseHandle(helper_observed);helper_observed=NULL;
        }
        GetProcessHandleCount(GetCurrentProcess(),&after);
        check(before==after,"helper failure leaves no local handle growth");
        if(helper_fault!=2)check(WaitForSingleObject(target.hProcess,0)==WAIT_TIMEOUT &&
            ResumeThread(target.hThread)==1,"helper does not terminate/resume target on failure");
        TerminateProcess(target.hProcess,41);check(WaitForSingleObject(target.hProcess,5000)==WAIT_OBJECT_0,
            "creator rolls back its own unpublished target");
        CloseHandle(target.hThread);CloseHandle(target.hProcess);
        printf("helper-fault=%u error=%lu elapsed=%llu\n",helper_fault,error,(unsigned long long)elapsed);
    }
    check(SetEnvironmentVariableW(L"WINDIR",length ? saved : NULL),"restore fixture WINDIR");
}
static void helper_exact_path(PCWSTR opposite,const nthook_context *context)
{
    WCHAR directory[MAX_PATH],dll[MAX_PATH];nthook_context selected=*context;
    GetModuleFileNameW(NULL,directory,MAX_PATH);
    check(wcsstr(directory,L"\\build\\")!=NULL,"alternate DLL fixture remains build-only");
    WCHAR *slash=wcsrchr(directory,L'\\');
    wcscpy_s(slash+1,MAX_PATH-(slash+1-directory),
#if defined(_WIN64)
        L"helper64.path"
#else
        L"helper32.path"
#endif
    );
    check(CreateDirectoryW(directory,NULL),"fresh explicit DLL directory with caller-width digits");
    swprintf_s(dll,L"%ls\\%ls",directory,
#if defined(_WIN64)
        L"nthook32.dll"
#else
        L"nthook64.dll"
#endif
    );
#if defined(_WIN64)
    check(CopyFileW(context->hook,dll,TRUE),"copy actual opposite-width Hook");
    wcscpy_s(selected.hook,dll);
#else
    check(CopyFileW(context->hook64,dll,TRUE),"copy actual opposite-width Hook");
    wcscpy_s(selected.hook64,dll);
#endif
    DWORD result=run(opposite,L"nested",&selected,NATIVE_HOOK_INTERCEPT);
    BOOL removed=DeleteFileW(dll) && RemoveDirectoryW(directory);
    check(result==0 && removed,"helper preserves explicit matching DLL path and releases its files");
}
static void attachment_rollback(PCWSTR image,const nthook_context *context,BOOL wrong_dll)
{
    STARTUPINFOW start={sizeof(start)};PROCESS_INFORMATION process={0};
    nthook_context selected=*context;DWORD before=0,after=0,result=0;
    selected.frontend=CreateEventW(NULL,TRUE,FALSE,NULL);
    selected.execution=wrong_dll ? CreateEventW(NULL,TRUE,FALSE,NULL) : (HANDLE)(ULONG_PTR)0x7ffffffd;
    check(selected.frontend && selected.execution,"rollback fixture capability pair");
    if(wrong_dll) {
#if defined(_WIN64)
        wcscpy_s(selected.hook,context->hook64);
#else
        wcscpy_s(selected.hook64,context->hook);
#endif
    }
    check(CreateProcessW(image,NULL,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&start,&process),
        "rollback owns actual suspended target");
    check(GetProcessHandleCount(process.hProcess,&before),"target initial handle count");
    DWORD error=nthook_install(process.hProcess,&selected,NATIVE_HOOK_INTERCEPT);
    check(wrong_dll ? error!=ERROR_SUCCESS : error==ERROR_INVALID_HANDLE,
        "wrong matching DLL or second capability fails installation");
    check(GetProcessHandleCount(process.hProcess,&after) && before==after,
        "partially duplicated recipient capabilities rolled back");
    check(GetExitCodeProcess(process.hProcess,&result) && result==STILL_ACTIVE,
        "installer does not kill caller-owned target");
    check(SuspendThread(process.hThread)==1 && ResumeThread(process.hThread)==2,
        "failure retains original suspended target");
    check(TerminateProcess(process.hProcess,error) &&
        WaitForSingleObject(process.hProcess,10000)==WAIT_OBJECT_0,"creator rolls back unpublished target");
    CloseHandle(process.hThread);CloseHandle(process.hProcess);
    CloseHandle(selected.frontend);if(wrong_dll)CloseHandle(selected.execution);
}
static void malformed(PCWSTR image,const nthook_context *context,unsigned mutation)
{
    BYTE data[sizeof(native_hook_packet)+3*MAX_PATH*sizeof(WCHAR)]={0};
    native_hook_packet *header=(native_hook_packet *)data;
    header->version=NATIVE_HOOK_VERSION;header->mode=NATIVE_HOOK_LAUNCHER;
    header->machine=
#if defined(_WIN64)
        IMAGE_FILE_MACHINE_AMD64;
#else
        IMAGE_FILE_MACHINE_I386;
#endif
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
    case 6:data[header->hook_offset+header->hook_bytes-2]=1;break;
    case 7:bytes=sizeof(*header)-1;break;
    case 8:header->hook64_offset=MAXDWORD;break;
    case 9:header->hook64_bytes=MAXDWORD;break;
    case 10:header->machine=0;break;
    case 11:header->reserved=1;break;
    case 12:data[bytes-2]=1;break;
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
    PCWSTR launcher=NULL,opposite=NULL;
    for(int i=1;i<argc;i+=2) {
        check(i+1<argc,"fixture option value supplied");
        if(!wcscmp(argv[i],L"--launcher-fixture"))launcher=argv[i+1];
        else if(!wcscmp(argv[i],L"--opposite-fixture"))opposite=argv[i+1];
        else check(FALSE,"known fixture option");
    }
    check(!nthook_context_paths(&context),"own-package pinned paths");
    GetModuleFileNameW(NULL,image,MAX_PATH);wcscpy_s(gui,image);
    slash=wcsrchr(gui,L'\\');wcscpy_s(slash+1,MAX_PATH-(slash+1-gui),L"nthook-gui-test.exe");
    context.frontend=CreateEventW(NULL,TRUE,FALSE,NULL);
    context.execution=CreateEventW(NULL,TRUE,FALSE,NULL);
    check(context.frontend && context.execution,"private fixture events");
    nthook_context launcher_context=context;
#if defined(_WIN64)
    check(launcher!=NULL,"x86 context-only fixture supplied");
    wcscpy_s(launcher_context.launcher,launcher);
#else
    wcscpy_s(launcher_context.launcher,image);
#endif
    if(opposite) {
        WCHAR sibling[MAX_PATH];wcscpy_s(sibling,opposite);slash=wcsrchr(sibling,L'\\');
        check(slash!=NULL,"opposite fixture absolute path");
#if defined(_WIN64)
        wcscpy_s(slash+1,MAX_PATH-(slash+1-sibling),L"nthook32.dll");
        wcscpy_s(context.hook,sibling);
#else
        wcscpy_s(slash+1,MAX_PATH-(slash+1-sibling),L"nthook64.dll");
        wcscpy_s(context.hook64,sibling);
#endif
        launcher_context=context;
#if defined(_WIN64)
        wcscpy_s(launcher_context.launcher,launcher);
#else
        wcscpy_s(launcher_context.launcher,image);
#endif
        check(SetEnvironmentVariableW(L"NTHOOK_OPPOSITE",opposite) &&
            SetEnvironmentVariableW(L"NTHOOK_RETURN",image),"cross-width chain identities supplied");
    }
    check(run(launcher_context.launcher,L"context",&launcher_context,NATIVE_HOOK_LAUNCHER)==0,"context-only pinned x86 payload/no DLL");
    check(WaitForSingleObject(context.frontend,0)==WAIT_OBJECT_0 &&
          WaitForSingleObject(context.execution,0)==WAIT_OBJECT_0,"recipient-local duplicated events");
    check(run(image,L"nested",&context,NATIVE_HOOK_INTERCEPT)==0,"CUI immediate descendant propagated");
    check(run(image,L"ansi",&context,NATIVE_HOOK_INTERCEPT)==0,"ANSI native child propagated");
    check(run(image,L"cmd",&context,NATIVE_HOOK_INTERCEPT)==0,"actual matching-width CMD propagated immediate child");
    check(run(image,L"attributes",&context,NATIVE_HOOK_INTERCEPT)==0,"explicit handle list/Unicode environment/CWD/streams preserved");
    check(run(image,L"cui-to-gui",&context,NATIVE_HOOK_INTERCEPT)==0,"CUI to GUI strips text authority");
    if(opposite) {
        DWORD before=0,after=0;
        check(GetProcessHandleCount(GetCurrentProcess(),&before),"cross-width steady-state handle count");
        check(run(opposite,L"nested",&context,NATIVE_HOOK_INTERCEPT)==0,"opposite matching DLL/immediate same-width descendant");
        check(run(image,L"cross",&context,NATIVE_HOOK_INTERCEPT)==0,"real alternating-width child/grandchild and waits");
        check(run(image,L"cross-ansi",&context,NATIVE_HOOK_INTERCEPT)==0,"ANSI opposite-width child and return");
        check(run(image,L"cross-gui",&context,NATIVE_HOOK_INTERCEPT)==0,"opposite GUI strips text authority");
        check(run(image,L"cross-attributes",&context,NATIVE_HOOK_INTERCEPT)==0,"opposite child handle list/Unicode/CWD/streams preserved");
        check(run(image,L"cross-exit",&context,NATIVE_HOOK_INTERCEPT)==37,"opposite actual Windows nonzero completion retained");
        check(GetProcessHandleCount(GetCurrentProcess(),&after) && before==after,
            "successful cross-width helpers leave no local handle growth");
        helper_exact_path(opposite,&context);
        attachment_rollback(opposite,&context,FALSE);
        attachment_rollback(opposite,&context,TRUE);
    }
    CloseHandle(context.frontend);CloseHandle(context.execution);
    context.frontend=context.execution=NULL;
    check(run(gui,L"nested",&context,NATIVE_HOOK_INTERCEPT)==0,"GUI immediate descendant propagated");
    check(run(gui,L"gui-to-cui",&context,NATIVE_HOOK_INTERCEPT)==0,"GUI to CUI propagates compatibility");
    check(run(gui,L"gui",&context,NATIVE_HOOK_INTERCEPT)==0,"GUI carries no text capability");
    if(opposite)check(run(gui,L"cross",&context,NATIVE_HOOK_INTERCEPT)==0,"GUI to opposite CUI/return preserves compatibility without authority");
    if(opposite)helper_failures(opposite,&context);
    for(unsigned i=0;i<13;++i)malformed(image,&context,i);
    launcher_context.frontend=(HANDLE)(ULONG_PTR)0x7ffffffe;launcher_context.execution=(HANDLE)(ULONG_PTR)0x7ffffffd;
    check(run(launcher_context.launcher,L"grandchild",&launcher_context,NATIVE_HOOK_LAUNCHER)==ERROR_INVALID_HANDLE,"invalid capability fails before handoff");
    context.frontend=context.execution=NULL;
    wcscat_s(
#if defined(_WIN64)
        context.hook64,
#else
        context.hook,
#endif
        L".missing");
    check(run(image,L"grandchild",&context,NATIVE_HOOK_INTERCEPT)==ERROR_FILE_NOT_FOUND,"missing DLL fails before handoff");
    printf("NTHOOK-INSTALL-PASS assertions=%u\n",assertions);return 0;
}
