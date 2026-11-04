#include "intercept.h"
#include "common/native-hook/installer.h"
#include "lib-win32/process-hook/helper.h"
#include <rpc.h>
#include <wchar.h>
/* RPC allocation is local and has no initialization/loader-lock activity. */
void *__RPC_USER MIDL_user_allocate(size_t bytes)
{return HeapAlloc(GetProcessHeap(),0,bytes);}
void __RPC_USER MIDL_user_free(void *value)
{if(value)HeapFree(GetProcessHeap(),0,value);}
nthook_context nthook_process_context={0};
void WINAPI NthookAnchor(void) {}
void CALLBACK NthookFinishHelper(HWND window,HINSTANCE instance,LPSTR command,INT show)
{
    (void)window;(void)instance;(void)command;(void)show;
    ExitProcess(process_hook_run_helper(nthook_helper_apply,NULL));
}
DWORD WINAPI NthookContextFlags(void)
{return nthook_process_context.mode | (nthook_process_context.frontend ? 0x100u : 0u);}
/* The helper is an explicit system rundll32 load, without a staged payload.
 * Verify its executable and DLL command shape before allowing that one passive
 * load; the helper itself performs full command and handoff validation. */
static BOOL helper_process(HINSTANCE instance)
{
    WCHAR image[MAX_PATH],module[MAX_PATH],system[MAX_PATH],candidate[MAX_PATH];
    PCWSTR command,end;DWORD n;
    n=GetModuleFileNameW(NULL,image,MAX_PATH);if(!n || n>=MAX_PATH)return FALSE;
    n=GetSystemDirectoryW(system,MAX_PATH);if(!n || n>=MAX_PATH)return FALSE;
    if(swprintf_s(candidate,MAX_PATH,L"%ls\\rundll32.exe",system)<0)return FALSE;
    if(_wcsicmp(image,candidate)) {
        n=GetSystemWow64DirectoryW(system,MAX_PATH);if(!n || n>=MAX_PATH)return FALSE;
        if(swprintf_s(candidate,MAX_PATH,L"%ls\\rundll32.exe",system)<0 || _wcsicmp(image,candidate))return FALSE;
    }
    n=GetModuleFileNameW(instance,module,MAX_PATH);if(!n || n>=MAX_PATH)return FALSE;
    command=GetCommandLineW();while(*command==L' ')++command;
    if(*command++!=L'"')return FALSE;
    end=wcschr(command,L'"');if(!end)return FALSE;
    command=end+1;while(*command==L' ')++command;
    if(*command++!=L'"')return FALSE;
    end=wcschr(command,L'"');if(!end || (size_t)(end-command)!=wcslen(module) ||
       _wcsnicmp(command,module,(size_t)(end-command)))return FALSE;
    return wcsncmp(end+1,L",#1 ",4)==0;
}
BOOL WINAPI DllMain(HINSTANCE instance,DWORD reason,LPVOID reserved)
{
    (void)reserved;
    if(reason==DLL_PROCESS_ATTACH) {
        BOOL found=FALSE;DWORD error;
        DisableThreadLibraryCalls(instance);
        error=nthook_context_read(&nthook_process_context,&found);
        if(error)return FALSE;
        if(!found)return helper_process(instance);
        if(nthook_process_context.mode!=NATIVE_HOOK_INTERCEPT)return FALSE;
        return nthook_attach()==NO_ERROR;
    }
    return TRUE;
}
