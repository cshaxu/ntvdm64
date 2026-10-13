#include "wow_title_policy.h"
#include "wow_user_task_lifecycle.h"
#include "common/protocol/window_style.h"
#include <wchar.h>

typedef BOOL (__cdecl *title_install_fn)(HWND);
static INIT_ONCE title_once=INIT_ONCE_STATIC_INIT;
static title_install_fn title_install;
static BOOL CALLBACK load_titles(PINIT_ONCE once,void *parameter,void **context)
{
    WCHAR path[MAX_PATH],*name;DWORD length,error;HMODULE module;
    (void)once;(void)parameter;(void)context;
    length=GetModuleFileNameW(NULL,path,ARRAYSIZE(path));
    if(!length || length>=ARRAYSIZE(path)){SetLastError(ERROR_FILENAME_EXCED_RANGE);return FALSE;}
    name=wcsrchr(path,L'\\');
    if(!name){SetLastError(ERROR_INVALID_NAME);return FALSE;}
    if(wcscpy_s(name+1,ARRAYSIZE(path)-(size_t)(name+1-path),L"ntw16s32.dll")) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);return FALSE;
    }
    module=LoadLibraryExW(path,NULL,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!module)return FALSE;
    title_install=(title_install_fn)(void *)GetProcAddress(module,"Ntw16sTitleInstall");
    if(!title_install){error=GetLastError();FreeLibrary(module);SetLastError(error);return FALSE;}
    /* Module reference only; enablement belongs to each bound WOW task.
     * Retain the DLL until process teardown for surviving window subclasses. */
    return TRUE;
}
BOOL WINAPI wow_title_policy(DWORD *policy)
{
    wow_user_runtime_thread *binding=wow_user_runtime_current();
    if(!policy || !binding || !binding->runtime || !binding->runtime->lifecycle ||
       !wow_user_task_lifecycle_style(binding->runtime->lifecycle,policy)) {
        SetLastError(ERROR_INVALID_STATE);return FALSE;
    }
    return TRUE;
}
BOOL WINAPI wow_title_apply(HWND window,DWORD policy)
{
    if(!WINDOW_STYLE_VALID(policy)){SetLastError(ERROR_INVALID_PARAMETER);return FALSE;}
    if(policy==WINDOW_STYLE_OFF ||
       (GetWindowLongPtrW(window,GWL_STYLE)&WS_CAPTION)!=WS_CAPTION)return TRUE;
    if(!InitOnceExecuteOnce(&title_once,load_titles,NULL,NULL))return FALSE;
    return title_install(window);
}

typedef struct modal_scope {
    struct modal_scope *previous;
    HWND window;
    HHOOK create_hook,return_hook;
    DWORD policy,failure;
} modal_scope;
static __declspec(thread) modal_scope *current_modal;
static LRESULT CALLBACK modal_create(int code,WPARAM wp,LPARAM lp)
{
    modal_scope *scope=current_modal;
    if(code==HCBT_CREATEWND && scope && !scope->window) {
        WCHAR name[16];
        if(GetClassNameW((HWND)wp,name,ARRAYSIZE(name)) && !wcscmp(name,L"#32770"))
            scope->window=(HWND)wp;
    }
    return CallNextHookEx(NULL,code,wp,lp);
}
static LRESULT CALLBACK modal_return(int code,WPARAM wp,LPARAM lp)
{
    modal_scope *scope=current_modal;
    if(code>=0 && scope) {
        const CWPRETSTRUCT *message=(const CWPRETSTRUCT *)lp;
        if(message->hwnd==scope->window) {
            if(message->message==WM_NCDESTROY)scope->window=NULL;
            else if(message->message==WM_INITDIALOG &&
                !wow_title_apply(scope->window,scope->policy)) {
                scope->failure=GetLastError();if(!scope->failure)scope->failure=ERROR_GEN_FAILURE;
                EndDialog(scope->window,-1);
            }
        }
    }
    return CallNextHookEx(NULL,code,wp,lp);
}
static BOOL begin_modal(modal_scope *scope,DWORD policy)
{
    ZeroMemory(scope,sizeof(*scope));scope->previous=current_modal;scope->policy=policy;
    scope->create_hook=SetWindowsHookExW(WH_CBT,modal_create,NULL,GetCurrentThreadId());
    if(scope->create_hook)scope->return_hook=SetWindowsHookExW(WH_CALLWNDPROCRET,modal_return,NULL,GetCurrentThreadId());
    if(!scope->create_hook || !scope->return_hook) {
        DWORD error=GetLastError();if(scope->create_hook)UnhookWindowsHookEx(scope->create_hook);
        SetLastError(error);return FALSE;
    }
    current_modal=scope;return TRUE;
}
static void end_modal(modal_scope *scope)
{
    current_modal=scope->previous;
    UnhookWindowsHookEx(scope->return_hook);UnhookWindowsHookEx(scope->create_hook);
}
INT_PTR WINAPI wow_title_modal(HINSTANCE module,LPCDLGTEMPLATEW resource,HWND owner,
    DLGPROC procedure,LPARAM parameter,BOOL ansi,DWORD policy)
{
    modal_scope scope;INT_PTR result=-1;DWORD error=ERROR_SUCCESS;
    if(!WINDOW_STYLE_VALID(policy)){SetLastError(ERROR_INVALID_PARAMETER);return -1;}
    if(policy==WINDOW_STYLE_OFF)
        return ansi ? DialogBoxIndirectParamA(module,resource,owner,procedure,parameter) :
            DialogBoxIndirectParamW(module,resource,owner,procedure,parameter);
    if(!begin_modal(&scope,policy))return -1;
    __try {
        result=ansi ? DialogBoxIndirectParamA(module,resource,owner,procedure,parameter) :
            DialogBoxIndirectParamW(module,resource,owner,procedure,parameter);
        error=GetLastError();if(scope.failure){result=-1;error=scope.failure;}
    } __finally {
        end_modal(&scope);
    }
    SetLastError(error);return result;
}
int WINAPI wow_title_message_box_a(HWND owner,LPCSTR text,LPCSTR caption,UINT type)
{
    DWORD policy,error;int result;modal_scope scope;
    if(!wow_title_policy(&policy))return 0;
    if(policy==WINDOW_STYLE_OFF)return MessageBoxA(owner,text,caption,type);
    if(!begin_modal(&scope,policy))return 0;
    __try {
        result=MessageBoxA(owner,text,caption,type);error=GetLastError();
        if(scope.failure){result=0;error=scope.failure;}
    } __finally {end_modal(&scope);}
    SetLastError(error);return result;
}
