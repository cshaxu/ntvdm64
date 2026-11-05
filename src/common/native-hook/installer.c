#include "common/native-hook/installer.h"
#include "lib-win32/handle/duplicate.h"
#include "lib-win32/file/identity.h"
#include "lib-win32/process-hook/staging.h"
#include "lib-win32/process-hook/payload.h"
#include "lib-win32/process-hook/helper.h"
#include "common/native_image.h"
#include "lib-win32/process-image/image.h"
#include <wchar.h>
DWORD nthook_launcher_target(HANDLE child,const nthook_context *context,BOOL *launcher)
{
    WCHAR actual[MAX_PATH+32];DWORD error=ERROR_SUCCESS;
    if(!child || !context || !launcher)return ERROR_INVALID_PARAMETER;
    *launcher=FALSE;
    error=common_native_process_path(child,actual,MAX_PATH+32);
    if(error)return error;
    return win32_file_same(actual,context->launcher,launcher);
}
/* Runs inside the target-width helper. The process handle is borrowed from the
 * generic handoff and must already identify the still-suspended child. */
DWORD nthook_helper_apply(HANDLE child,const void *packet,DWORD bytes,PCWSTR dll,void *unused)
{
    nthook_context context;WORD machine=0;DWORD error;char path[MAX_PATH*2];BOOL lossy=FALSE;
    PCWSTR expected;(void)unused;
    error=win32_process_image_machine(child,&machine);if(error)return error;
    if(machine!=(sizeof(void *)==8?IMAGE_FILE_MACHINE_AMD64:IMAGE_FILE_MACHINE_I386))return ERROR_BAD_EXE_FORMAT;
    error=nthook_context_parse(&context,packet,bytes,machine);if(error)return error;
    expected=machine==IMAGE_FILE_MACHINE_AMD64?context.hook64:context.hook;
    if(!dll || _wcsicmp(expected,dll))return ERROR_INVALID_NAME;
    if(context.mode!=NATIVE_HOOK_INTERCEPT)return ERROR_INVALID_DATA;
    if(!WideCharToMultiByte(CP_ACP,WC_NO_BEST_FIT_CHARS,expected,-1,path,sizeof(path),NULL,&lossy) || lossy)
        return ERROR_NO_UNICODE_TRANSLATION;
    return process_hook_stage_process(child,path,packet,bytes);
}
DWORD nthook_install(HANDLE child,const nthook_context *context,DWORD mode)
{
    BYTE data[sizeof(native_hook_packet)+3*MAX_PATH*sizeof(WCHAR)]={0};
    native_hook_packet *header=(native_hook_packet *)data;
    HANDLE attached[2]={NULL,NULL};DWORD error=ERROR_SUCCESS,flags;
    char dll[MAX_PATH*2];BOOL default_char=FALSE;WORD machine=0;PCWSTR hook;
    if(!child || !context || (mode!=NATIVE_HOOK_INTERCEPT && mode!=NATIVE_HOOK_LAUNCHER) ||
        !!context->frontend!=!!context->execution ||
        (context->creation_flags & ~NATIVE_HOOK_CREATION_FLAGS))return ERROR_INVALID_PARAMETER;
    error=win32_process_image_machine(child,&machine);
    if(error)return error;
    hook=machine==IMAGE_FILE_MACHINE_AMD64 ? context->hook64 : context->hook;
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
    if(context->creation_flags && mode!=NATIVE_HOOK_LAUNCHER)return ERROR_INVALID_PARAMETER;
    if(mode==NATIVE_HOOK_INTERCEPT) {
        flags=GetFileAttributesW(hook);
        if(flags==INVALID_FILE_ATTRIBUTES)return GetLastError();
        if(flags&FILE_ATTRIBUTE_DIRECTORY)return ERROR_BAD_EXE_FORMAT;
        if(!WideCharToMultiByte(CP_ACP,WC_NO_BEST_FIT_CHARS,hook,-1,dll,sizeof(dll),NULL,&default_char) ||
            default_char)return ERROR_NO_UNICODE_TRANSLATION;
    }
    for(DWORD i=0;i<2;++i) {
        HANDLE source=i?context->execution:context->frontend;
        if(source && (error=win32_handle_duplicate(GetCurrentProcess(),source,child,&attached[i])))goto failed;
    }
    header->version=NATIVE_HOOK_VERSION;header->mode=mode;header->machine=machine;
    header->flags=context->creation_flags;
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
    if(mode==NATIVE_HOOK_LAUNCHER)
        error=process_hook_payload_write(child,data,header->bytes);
    else if(machine==(sizeof(void *)==8?IMAGE_FILE_MACHINE_AMD64:IMAGE_FILE_MACHINE_I386))
        error=process_hook_stage_process(child,dll,data,header->bytes);
    else
        error=process_hook_install_helper(child,hook,data,header->bytes,CreateProcessW);
    if(error)goto failed;
    return ERROR_SUCCESS;
failed:
    for(DWORD i=0;i<2;++i)win32_handle_close_remote(child,attached[i]);
    return error?error:ERROR_DLL_INIT_FAILED;
}
