#include "wow_title_policy.h"
#include "wow_user_task_lifecycle.h"
#include "common/protocol/window_style.h"
#include "ntw16s-dll/attachment.h"
#include <wchar.h>
#include <shellapi.h>

static INIT_ONCE title_once=INIT_ONCE_STATIC_INIT;
typedef ntw16s_creation_scope *(__cdecl *creation_begin_fn)(BOOL);
typedef BOOL (__cdecl *creation_end_fn)(ntw16s_creation_scope *,ntw16s_creation_result *);
static creation_begin_fn creation_begin;
static creation_end_fn creation_end;
static BOOL (__cdecl *style_attached)(HWND);
static void (__cdecl *scroll_changed)(HWND);
static LONG style_ready;
static BOOL CALLBACK load_titles(PINIT_ONCE once,void *parameter,void **context)
{
    WCHAR path[MAX_PATH],*name;DWORD length;HMODULE module;
    typedef DWORD (__cdecl *version_fn)(void);
    version_fn version;creation_begin_fn begin;creation_end_fn end;
    FARPROC attached,changed;
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
    version=(version_fn)(void *)GetProcAddress(module,"Ntw16sGetAbiVersion");
    begin=(creation_begin_fn)(void *)GetProcAddress(module,"Ntw16sCreationBegin");
    end=(creation_end_fn)(void *)GetProcAddress(module,"Ntw16sCreationEnd");
    attached=GetProcAddress(module,"Ntw16sWindowAttached");changed=GetProcAddress(module,"Ntw16sScrollChanged");
    if(!version || version()!=NTW16S_ABI_VERSION || !begin || !end || !attached || !changed){
        FreeLibrary(module);SetLastError(ERROR_REVISION_MISMATCH);return FALSE;
    }
    creation_begin=begin;creation_end=end;
    style_attached=(BOOL (__cdecl *)(HWND))(void *)attached;
    scroll_changed=(void (__cdecl *)(HWND))(void *)changed;
    InterlockedExchange(&style_ready,1);
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
BOOL WINAPI wow_style_creation_begin(DWORD policy,void **scope)
{
    if(!scope || !WINDOW_STYLE_VALID(policy)){SetLastError(ERROR_INVALID_PARAMETER);return FALSE;}
    *scope=NULL;
    if(policy!=WINDOW_STYLE_OFF){
        if(!InitOnceExecuteOnce(&title_once,load_titles,NULL,NULL))return FALSE;
    }else if(!InterlockedCompareExchange(&style_ready,0,0)){
        /* Off has no DLL dependency. Once loaded, an explicit Off scope must
         * mask a reentrant enabled task in the same shared WOW process. */
        return TRUE;
    }
    *scope=creation_begin(policy!=WINDOW_STYLE_OFF);return *scope!=NULL;
}
DWORD WINAPI wow_style_creation_end(void *scope)
{
    ntw16s_creation_result result={0};
    if(!scope)return ERROR_SUCCESS;
    if(!creation_end((ntw16s_creation_scope *)scope,&result))return GetLastError();
    return result.failure;
}
INT_PTR WINAPI wow_title_modal(HINSTANCE module,LPCDLGTEMPLATEW resource,HWND owner,
    DLGPROC procedure,LPARAM parameter,BOOL ansi,DWORD policy)
{
    void *scope;INT_PTR result=-1;DWORD error=ERROR_SUCCESS,failure=ERROR_SUCCESS;
    if(!wow_style_creation_begin(policy,&scope))return -1;
    __try {
        result=ansi ? DialogBoxIndirectParamA(module,resource,owner,procedure,parameter) :
            DialogBoxIndirectParamW(module,resource,owner,procedure,parameter);
        error=GetLastError();
    } __finally {
        failure=wow_style_creation_end(scope);
    }
    if(failure){result=-1;error=failure;}
    SetLastError(error);return result;
}
int WINAPI wow_title_message_box_a(HWND owner,LPCSTR text,LPCSTR caption,UINT type)
{
    DWORD policy,error,failure=ERROR_SUCCESS;int result;void *scope;
    if(!wow_title_policy(&policy))return 0;
    if(!wow_style_creation_begin(policy,&scope))return 0;
    __try {
        result=MessageBoxA(owner,text,caption,type);error=GetLastError();
    } __finally {failure=wow_style_creation_end(scope);}
    if(failure){result=0;error=failure;}
    SetLastError(error);return result;
}

/* ShellAbout is a native dialog on behalf of the current Win16 task, not a
 * separately launched native application. Retain the host's version content
 * and return contract while covering its internal dialog/control creation. */
int WINAPI wow_style_shell_about_a(HWND owner,LPCSTR application,LPCSTR text,HICON icon)
{
    DWORD policy,error,failure=ERROR_SUCCESS;int result;void *scope;
    if(!wow_title_policy(&policy) || !wow_style_creation_begin(policy,&scope))return 0;
    __try {
        result=ShellAboutA(owner,application,text,icon);error=GetLastError();
    } __finally {failure=wow_style_creation_end(scope);}
    if(failure){result=0;error=failure;}
    SetLastError(error);return result;
}

/* Finite USER bindings selected only for the original WOW scroll callers.
 * This DLL is not injected with nthook, so these calls reach native USER
 * directly. Window-local attachment, not the current task, selects repaint. */
static BOOL styled_scroll(HWND w)
{
    DWORD saved=GetLastError();BOOL result;
    result=InterlockedCompareExchange(&style_ready,0,0) && style_attached(w);
    SetLastError(saved);return result;
}
int WINAPI wow_style_set_scroll_pos(HWND w,int bar,int position,BOOL redraw)
{
    int result;DWORD error;BOOL styled=styled_scroll(w);
    result=SetScrollPos(w,bar,position,styled?FALSE:redraw);error=GetLastError();
    if(styled && redraw)scroll_changed(w);
    SetLastError(error);return result;
}
BOOL WINAPI wow_style_set_scroll_range(HWND w,int bar,int first,int last,BOOL redraw)
{
    BOOL result,styled=styled_scroll(w);DWORD error;
    result=SetScrollRange(w,bar,first,last,styled?FALSE:redraw);error=GetLastError();
    if(result && styled && redraw)scroll_changed(w);
    SetLastError(error);return result;
}
BOOL WINAPI wow_style_show_scroll_bar(HWND w,int bar,BOOL show)
{
    BOOL result=ShowScrollBar(w,bar,show);DWORD error=GetLastError();
    if(result && styled_scroll(w))scroll_changed(w);
    SetLastError(error);return result;
}
BOOL WINAPI wow_style_enable_scroll_bar(HWND w,UINT bar,UINT arrows)
{
    BOOL result=EnableScrollBar(w,bar,arrows);DWORD error=GetLastError();
    if(styled_scroll(w))scroll_changed(w);
    SetLastError(error);return result;
}
