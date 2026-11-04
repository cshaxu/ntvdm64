#include "install.h"
#include "detours/detours.h"

DWORD win32_process_hook_apply(HANDLE child,DWORD machine,const GUID *payload_id,
    const void *payload,DWORD bytes,const char *dll)
{
    if(!DetourCopyPayloadToProcess(child,*payload_id,(void *)payload,bytes))
        { DWORD error=GetLastError(); return error ? error : ERROR_DLL_INIT_FAILED; }
    if(dll) {
        LPCSTR imports[1]={dll};
        BOOL installed=machine==(sizeof(void *)==8 ? IMAGE_FILE_MACHINE_AMD64 : IMAGE_FILE_MACHINE_I386) ?
            DetourUpdateProcessWithDll(child,imports,1) :
            DetourProcessViaHelperDllsW(GetProcessId(child),1,imports,CreateProcessW);
        if(!installed){ DWORD error=GetLastError(); return error ? error : ERROR_DLL_INIT_FAILED; }
    }
    return ERROR_SUCCESS;
}
