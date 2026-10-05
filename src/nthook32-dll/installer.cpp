#include "hook.h"
#include "detours/detours.h"
#include "common/image_classification.h"
#include <wchar.h>
extern const GUID nthook_payload_guid;
DWORD nthook_launcher_target(HANDLE child,const nthook_context *context,BOOL *launcher)
{
    WCHAR actual[MAX_PATH+32];DWORD error=ERROR_SUCCESS;
    HANDLE files[2]={INVALID_HANDLE_VALUE,INVALID_HANDLE_VALUE};BY_HANDLE_FILE_INFORMATION info[2];
    if(!child || !context || !launcher)return ERROR_INVALID_PARAMETER;
    *launcher=FALSE;
    error=common_process_image_path(child,actual,ARRAYSIZE(actual));
    if(error)return error;
    PCWSTR names[2]={actual,context->launcher};
    for(unsigned i=0;i<2;++i) {
        files[i]=CreateFileW(names[i],FILE_READ_ATTRIBUTES,FILE_SHARE_READ|FILE_SHARE_DELETE,
            NULL,OPEN_EXISTING,0,NULL);
        if(files[i]==INVALID_HANDLE_VALUE || !GetFileInformationByHandle(files[i],&info[i])) {
            error=GetLastError();break;
        }
    }
    if(!error)*launcher=info[0].dwVolumeSerialNumber==info[1].dwVolumeSerialNumber &&
        info[0].nFileIndexHigh==info[1].nFileIndexHigh && info[0].nFileIndexLow==info[1].nFileIndexLow;
    for(unsigned i=0;i<2;++i)if(files[i]!=INVALID_HANDLE_VALUE)CloseHandle(files[i]);
    return error;
}
static void undo_attachment(HANDLE child,HANDLE value)
{
    HANDLE local=NULL;
    if(value && DuplicateHandle(child,value,GetCurrentProcess(),&local,0,FALSE,
        DUPLICATE_CLOSE_SOURCE|DUPLICATE_SAME_ACCESS))CloseHandle(local);
}
DWORD nthook_install(HANDLE child,const nthook_context *context,DWORD mode)
{
    BYTE data[sizeof(native_hook_packet)+3*MAX_PATH*sizeof(WCHAR)]={0};
    native_hook_packet *header=(native_hook_packet *)data;
    HANDLE attached[2]={NULL,NULL};DWORD error=ERROR_SUCCESS,flags,machine;
    PCWSTR hook;
    BOOL opposite=FALSE;
    char dll[MAX_PATH*2];BOOL default_char=FALSE;
    if(!child || !context || (mode!=NATIVE_HOOK_INTERCEPT && mode!=NATIVE_HOOK_LAUNCHER) ||
        !!context->frontend!=!!context->execution)return ERROR_INVALID_PARAMETER;
    error=common_process_machine(child,&machine);if(error)return error;
    if(!context->launcher[0] || !context->hook[0] || !context->hook64[0] ||
       wcsnlen_s(context->launcher,MAX_PATH)==MAX_PATH || wcsnlen_s(context->hook,MAX_PATH)==MAX_PATH ||
       wcsnlen_s(context->hook64,MAX_PATH)==MAX_PATH)
        return ERROR_INVALID_DATA;
    if(mode==NATIVE_HOOK_INTERCEPT) {
        BOOL launcher=FALSE;
        error=nthook_launcher_target(child,context,&launcher);
        if(error)return error;
        /* The actual pinned launcher receives context only, including when
         * NTVWM itself creates it. Never install its own interception DLL. */
        if(launcher)mode=NATIVE_HOOK_LAUNCHER;
    }
    /* Context-only mode is exclusively the pinned x86 launcher. Ordinary
     * opposite-width native targets keep their real Windows process. */
    if(mode==NATIVE_HOOK_LAUNCHER) {
        BOOL launcher=FALSE;
        error=nthook_launcher_target(child,context,&launcher);
        if(error)return error;
        if(!launcher || machine!=IMAGE_FILE_MACHINE_I386)return ERROR_NOT_SUPPORTED;
    } else {
#if defined(_WIN64)
        opposite=machine==IMAGE_FILE_MACHINE_I386;
#else
        opposite=machine==IMAGE_FILE_MACHINE_AMD64;
#endif
        if(machine!=IMAGE_FILE_MACHINE_I386 && machine!=IMAGE_FILE_MACHINE_AMD64)
            return ERROR_NOT_SUPPORTED;
    }
    hook=machine==IMAGE_FILE_MACHINE_AMD64 ? context->hook64 : context->hook;
    if(mode==NATIVE_HOOK_INTERCEPT) {
        flags=GetFileAttributesW(hook);
        if(flags==INVALID_FILE_ATTRIBUTES)return GetLastError();
        if(flags&FILE_ATTRIBUTE_DIRECTORY)return ERROR_BAD_EXE_FORMAT;
        if(!WideCharToMultiByte(CP_ACP,WC_NO_BEST_FIT_CHARS,hook,-1,dll,sizeof(dll),NULL,&default_char) ||
            default_char)return ERROR_NO_UNICODE_TRANSLATION;
    }
    for(DWORD i=0;i<2;++i) {
        HANDLE source=i?context->execution:context->frontend;
        if(source && !DuplicateHandle(GetCurrentProcess(),source,child,&attached[i],
            0,FALSE,DUPLICATE_SAME_ACCESS)){error=GetLastError();goto failed;}
    }
    header->version=NATIVE_HOOK_VERSION;header->mode=mode;header->machine=machine;
    header->frontend=(ULONG_PTR)attached[0];header->execution=(ULONG_PTR)attached[1];
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
    if(!DetourCopyPayloadToProcess(child,nthook_payload_guid,data,header->bytes)) {error=GetLastError();goto failed;}
    if(mode==NATIVE_HOOK_INTERCEPT) {
        LPCSTR imports[1]={dll};
        /* The finite reviewed helper modifies this same suspended target;
         * no DetourCreateProcessWithDll wrapper or replacement child. */
        BOOL installed=opposite ? DetourProcessViaHelperDllsW(GetProcessId(child),1,
            imports,CreateProcessW) : DetourUpdateProcessWithDll(child,imports,1);
        if(!installed){error=GetLastError();goto failed;}
    }
    return ERROR_SUCCESS;
failed:
    for(DWORD i=0;i<2;++i)undo_attachment(child,attached[i]);
    return error?error:ERROR_DLL_INIT_FAILED;
}
