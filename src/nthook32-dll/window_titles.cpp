#include "intercept.h"
#include "detours/detours.h"
#include "ntw16s-dll/attachment.h"
#include "ntw16s-dll/native_api.h"
#include <wchar.h>
#include <uxtheme.h>

/* Native processes share their copied launch policy; WOW selects per task at
 * its own creation binding. No USER operation or DLL load runs in DllMain. */
typedef ntw16s_creation_scope *(__cdecl *creation_begin_fn)(BOOL);
typedef BOOL (__cdecl *creation_end_fn)(ntw16s_creation_scope *,ntw16s_creation_result *);
static INIT_ONCE title_once=INIT_ONCE_STATIC_INIT;
static creation_begin_fn creation_begin;
static creation_end_fn creation_end;
static BOOL (__cdecl *style_attached)(HWND);
static void (__cdecl *scroll_changed)(HWND);
static BOOL (__cdecl *style_get_scroll)(HWND,int,LPSCROLLINFO);
static int (__cdecl *style_set_scroll)(HWND,int,LPCSCROLLINFO,BOOL);
static HRESULT (__cdecl *style_set_theme)(HWND,LPCWSTR,LPCWSTR);
static LONG style_ready;
static decltype(&GetScrollInfo) real_get_scroll=GetScrollInfo;
static decltype(&SetScrollInfo) real_set_scroll=SetScrollInfo;
static decltype(&SetWindowTheme) real_set_theme=SetWindowTheme;
static BOOL CALLBACK load_titles(PINIT_ONCE,void *,void **)
{
    WCHAR path[MAX_PATH],*name;HMODULE module;
    typedef DWORD (__cdecl *version_fn)(void);
    version_fn version;creation_begin_fn begin;creation_end_fn end;
    FARPROC attached,changed,get_scroll,set_scroll,set_theme;
    typedef BOOL (__cdecl *bind_fn)(const ntw16s_native_api *);
    bind_fn bind;
    wcscpy_s(path,sizeof(void *)==8 ? nthook_process_context.hook64 : nthook_process_context.hook);
    name=wcsrchr(path,L'\\');if(!name){SetLastError(ERROR_INVALID_NAME);return FALSE;}
    if(wcscpy_s(name+1,ARRAYSIZE(path)-(size_t)(name+1-path),
        sizeof(void *)==8 ? L"ntw16s64.dll" : L"ntw16s32.dll"))return FALSE;
    module=LoadLibraryExW(path,NULL,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!module)return FALSE;
    version=(version_fn)(void *)GetProcAddress(module,"Ntw16sGetAbiVersion");
    begin=(creation_begin_fn)(void *)GetProcAddress(module,"Ntw16sCreationBegin");
    end=(creation_end_fn)(void *)GetProcAddress(module,"Ntw16sCreationEnd");
    attached=GetProcAddress(module,"Ntw16sWindowAttached");changed=GetProcAddress(module,"Ntw16sScrollChanged");
    get_scroll=GetProcAddress(module,"win16_style_get_scroll_info");set_scroll=GetProcAddress(module,"win16_style_set_scroll_info");
    set_theme=GetProcAddress(module,"Ntw16sSetWindowTheme");
    bind=(bind_fn)(void *)GetProcAddress(module,"Ntw16sBindNativeApi");
    if(!version || version()!=NTW16S_ABI_VERSION || !begin || !end || !attached || !changed || !get_scroll || !set_scroll || !set_theme || !bind){
        FreeLibrary(module);SetLastError(ERROR_REVISION_MISMATCH);return FALSE;
    }
    {ntw16s_native_api api={sizeof(api),real_get_scroll,real_set_scroll,real_set_theme};
        if(!bind(&api)){DWORD error=GetLastError();FreeLibrary(module);SetLastError(error);return FALSE;}}
    creation_begin=begin;creation_end=end;
    style_attached=(decltype(style_attached))(void *)attached;
    scroll_changed=(decltype(scroll_changed))(void *)changed;
    style_get_scroll=(decltype(style_get_scroll))(void *)get_scroll;
    style_set_scroll=(decltype(style_set_scroll))(void *)set_scroll;
    style_set_theme=(decltype(style_set_theme))(void *)set_theme;
    InterlockedExchange(&style_ready,1);
    /* Process lifetime reference: live HWND subclasses may outlive any launch
     * wrapper. Windows releases it at process exit; never unload under them. */
    return TRUE;
}
struct title_scope { ntw16s_creation_scope *creation;DWORD failure; };
static BOOL begin_titles(title_scope *scope,BOOL modal)
{
    (void)modal;ZeroMemory(scope,sizeof(*scope));
    if(!InitOnceExecuteOnce(&title_once,load_titles,NULL,NULL))return FALSE;
    scope->creation=creation_begin(TRUE);return scope->creation!=NULL;
}
static void end_titles(title_scope *scope)
{
    ntw16s_creation_result result={0};
    if(!creation_end(scope->creation,&result))scope->failure=GetLastError();
    else scope->failure=result.failure;
}

#define WINDOW_HOOK(S,T) \
static decltype(&CreateWindowEx##S) create_##S=CreateWindowEx##S; \
static HWND WINAPI window_##S(DWORD ex,T cls,T text,DWORD style,int x,int y,int cx,int cy,HWND parent,HMENU menu,HINSTANCE instance,void *param) \
{ title_scope scope;HWND result=NULL;DWORD error=ERROR_SUCCESS; \
  if(!begin_titles(&scope,FALSE))return NULL; \
  __try { result=create_##S(ex,cls,text,style,x,y,cx,cy,parent,menu,instance,param);error=GetLastError(); \
  } __finally { end_titles(&scope); } \
  if(scope.failure){if(result)DestroyWindow(result);result=NULL;error=scope.failure;} \
  SetLastError(error);return result; }
WINDOW_HOOK(A,LPCSTR)
WINDOW_HOOK(W,LPCWSTR)

#define DIALOG_HOOK(NAME,TYPE,RET,MODAL,FAIL) \
static decltype(&NAME) real_##NAME=NAME; \
static RET WINAPI hook_##NAME(HINSTANCE instance,TYPE name,HWND parent,DLGPROC proc,LPARAM param) \
{ title_scope scope;RET result=FAIL;DWORD error=ERROR_SUCCESS; \
  if(!begin_titles(&scope,MODAL))return FAIL; \
  __try { result=real_##NAME(instance,name,parent,proc,param);error=GetLastError(); \
  } __finally { end_titles(&scope); } \
  if(scope.failure){if(!MODAL && result)DestroyWindow((HWND)result);result=FAIL;error=scope.failure;} \
  SetLastError(error);return result; }
DIALOG_HOOK(CreateDialogParamA,LPCSTR,HWND,FALSE,NULL)
DIALOG_HOOK(CreateDialogParamW,LPCWSTR,HWND,FALSE,NULL)
DIALOG_HOOK(CreateDialogIndirectParamA,LPCDLGTEMPLATEA,HWND,FALSE,NULL)
DIALOG_HOOK(CreateDialogIndirectParamW,LPCDLGTEMPLATEW,HWND,FALSE,NULL)
DIALOG_HOOK(DialogBoxParamA,LPCSTR,INT_PTR,TRUE,-1)
DIALOG_HOOK(DialogBoxParamW,LPCWSTR,INT_PTR,TRUE,-1)
DIALOG_HOOK(DialogBoxIndirectParamA,LPCDLGTEMPLATEA,INT_PTR,TRUE,-1)
DIALOG_HOOK(DialogBoxIndirectParamW,LPCDLGTEMPLATEW,INT_PTR,TRUE,-1)

/* MessageBox creates its own dialog inside USER rather than necessarily
 * calling an exported DialogBox entry. Keep its API/return contract and use
 * the same bounded creation scope as the other modal entry points. */
#define MESSAGE_HOOK(NAME,PARAMETERS,ARGUMENTS) \
static decltype(&NAME) real_##NAME=NAME; \
static int WINAPI hook_##NAME PARAMETERS \
{ title_scope scope;int result=0;DWORD error=ERROR_SUCCESS; \
  if(!begin_titles(&scope,TRUE))return 0; \
  __try { result=real_##NAME ARGUMENTS;error=GetLastError(); \
  } __finally { end_titles(&scope); } \
  if(scope.failure){result=0;error=scope.failure;} SetLastError(error);return result; }
MESSAGE_HOOK(MessageBoxA,(HWND owner,LPCSTR text,LPCSTR caption,UINT type),(owner,text,caption,type))
MESSAGE_HOOK(MessageBoxW,(HWND owner,LPCWSTR text,LPCWSTR caption,UINT type),(owner,text,caption,type))
MESSAGE_HOOK(MessageBoxExA,(HWND owner,LPCSTR text,LPCSTR caption,UINT type,WORD language),(owner,text,caption,type,language))
MESSAGE_HOOK(MessageBoxExW,(HWND owner,LPCWSTR text,LPCWSTR caption,UINT type,WORD language),(owner,text,caption,type,language))
MESSAGE_HOOK(MessageBoxIndirectA,(const MSGBOXPARAMSA *parameters),(parameters))
MESSAGE_HOOK(MessageBoxIndirectW,(const MSGBOXPARAMSW *parameters),(parameters))
#undef MESSAGE_HOOK

/* The DLL uses the three original native leaves bound at load, so application
 * reentry is never accidentally bypassed by an ambient recursion guard. */
static decltype(&SetScrollPos) real_set_pos=SetScrollPos;
static decltype(&SetScrollRange) real_set_range=SetScrollRange;
static decltype(&ShowScrollBar) real_show_scroll=ShowScrollBar;
static decltype(&EnableScrollBar) real_enable_scroll=EnableScrollBar;
static HRESULT WINAPI hook_set_theme(HWND w,LPCWSTR application,LPCWSTR ids)
{
    if(!InterlockedCompareExchange(&style_ready,0,0))return real_set_theme(w,application,ids);
    return style_set_theme(w,application,ids);
}
static BOOL eligible_scroll(HWND w)
{
    DWORD saved=GetLastError();
    BOOL eligible=InterlockedCompareExchange(&style_ready,0,0) && style_attached(w);
    SetLastError(saved);return eligible;
}
static BOOL WINAPI hook_get_scroll(HWND w,int bar,LPSCROLLINFO info)
{
    if(!eligible_scroll(w))return real_get_scroll(w,bar,info);
    return style_get_scroll(w,bar,info);
}
static int WINAPI hook_set_scroll(HWND w,int bar,LPCSCROLLINFO info,BOOL redraw)
{
    if(!eligible_scroll(w))return real_set_scroll(w,bar,info,redraw);
    return style_set_scroll(w,bar,info,redraw);
}
static int WINAPI hook_set_pos(HWND w,int bar,int position,BOOL redraw)
{
    int result;DWORD error;
    if(!eligible_scroll(w))return real_set_pos(w,bar,position,redraw);
    result=real_set_pos(w,bar,position,FALSE);error=GetLastError();if(redraw)scroll_changed(w);
    SetLastError(error);return result;
}
static BOOL WINAPI hook_set_range(HWND w,int bar,int first,int last,BOOL redraw)
{
    BOOL result;DWORD error;
    if(!eligible_scroll(w))return real_set_range(w,bar,first,last,redraw);
    result=real_set_range(w,bar,first,last,FALSE);error=GetLastError();if(result && redraw)scroll_changed(w);
    SetLastError(error);return result;
}
static BOOL WINAPI hook_show_scroll(HWND w,int bar,BOOL show)
{
    BOOL result;DWORD error;
    if(!eligible_scroll(w))return real_show_scroll(w,bar,show);
    result=real_show_scroll(w,bar,show);error=GetLastError();if(result)scroll_changed(w);
    SetLastError(error);return result;
}
static BOOL WINAPI hook_enable_scroll(HWND w,UINT bar,UINT arrows)
{
    BOOL result;DWORD error;
    if(!eligible_scroll(w))return real_enable_scroll(w,bar,arrows);
    result=real_enable_scroll(w,bar,arrows);error=GetLastError();scroll_changed(w);
    SetLastError(error);return result;
}

LONG nthook_attach_titles(void)
{
    LONG error=NO_ERROR;
    if(nthook_process_context.window_style!=WINDOW_STYLE_ALL)return error;
    error=DetourAttach(&(PVOID &)create_A,window_A);
    if(!error)error=DetourAttach(&(PVOID &)create_W,window_W);
    if(!error)error=DetourAttach(&(PVOID &)real_get_scroll,hook_get_scroll);
    if(!error)error=DetourAttach(&(PVOID &)real_set_scroll,hook_set_scroll);
    if(!error)error=DetourAttach(&(PVOID &)real_set_pos,hook_set_pos);
    if(!error)error=DetourAttach(&(PVOID &)real_set_range,hook_set_range);
    if(!error)error=DetourAttach(&(PVOID &)real_show_scroll,hook_show_scroll);
    if(!error)error=DetourAttach(&(PVOID &)real_enable_scroll,hook_enable_scroll);
    if(!error)error=DetourAttach(&(PVOID &)real_set_theme,hook_set_theme);
#define ATTACH(NAME) if(!error)error=DetourAttach(&(PVOID &)real_##NAME,hook_##NAME)
    ATTACH(CreateDialogParamA);ATTACH(CreateDialogParamW);
    ATTACH(CreateDialogIndirectParamA);ATTACH(CreateDialogIndirectParamW);
    ATTACH(DialogBoxParamA);ATTACH(DialogBoxParamW);
    ATTACH(DialogBoxIndirectParamA);ATTACH(DialogBoxIndirectParamW);
    ATTACH(MessageBoxA);ATTACH(MessageBoxW);
    ATTACH(MessageBoxExA);ATTACH(MessageBoxExW);
    ATTACH(MessageBoxIndirectA);ATTACH(MessageBoxIndirectW);
#undef ATTACH
    return error;
}
