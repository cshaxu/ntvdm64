#include "intercept.h"
#include "detours/detours.h"
#include <wchar.h>

/* Native processes share their copied launch policy; WOW selects per task at
 * its own creation binding. No USER operation or DLL load runs in DllMain. */
typedef BOOL (__cdecl *title_install_fn)(HWND);
static INIT_ONCE title_once=INIT_ONCE_STATIC_INIT;
static title_install_fn title_install;
static BOOL CALLBACK load_titles(PINIT_ONCE,void *,void **)
{
    WCHAR path[MAX_PATH],*name;HMODULE module;DWORD error;
    wcscpy_s(path,sizeof(void *)==8 ? nthook_process_context.hook64 : nthook_process_context.hook);
    name=wcsrchr(path,L'\\');if(!name){SetLastError(ERROR_INVALID_NAME);return FALSE;}
    if(wcscpy_s(name+1,ARRAYSIZE(path)-(size_t)(name+1-path),
        sizeof(void *)==8 ? L"ntw16s64.dll" : L"ntw16s32.dll"))return FALSE;
    module=LoadLibraryExW(path,NULL,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!module)return FALSE;
    title_install=(title_install_fn)(void *)GetProcAddress(module,"Ntw16sTitleInstall");
    if(!title_install){error=GetLastError();FreeLibrary(module);SetLastError(error);return FALSE;}
    /* Process lifetime reference: live HWND subclasses may outlive any launch
     * wrapper. Windows releases it at process exit; never unload under them. */
    return TRUE;
}
struct title_scope { title_scope *previous;HHOOK hook;DWORD failure;BOOL modal; };
static __declspec(thread) title_scope *current_scope;
static LRESULT CALLBACK creation_return(int code,WPARAM wp,LPARAM lp)
{
    LRESULT result=CallNextHookEx(NULL,code,wp,lp);DWORD saved=GetLastError();
    title_scope *scope=current_scope;
    if(code>=0 && scope) {
        const CWPRETSTRUCT *message=(const CWPRETSTRUCT *)lp;
        if(((message->message==WM_CREATE && message->lResult!=-1) ||
            message->message==WM_INITDIALOG) &&
           (GetWindowLongPtrW(message->hwnd,GWL_STYLE)&WS_CAPTION)==WS_CAPTION &&
           !title_install(message->hwnd)) {
            scope->failure=GetLastError();if(!scope->failure)scope->failure=ERROR_GEN_FAILURE;
        }
        if(scope->failure && scope->modal && message->message==WM_INITDIALOG)
            EndDialog(message->hwnd,-1);
    }
    SetLastError(saved);return result;
}
static BOOL begin_titles(title_scope *scope,BOOL modal)
{
    ZeroMemory(scope,sizeof(*scope));scope->modal=modal;
    if(!InitOnceExecuteOnce(&title_once,load_titles,NULL,NULL))return FALSE;
    scope->previous=current_scope;
    /* One hook per nested API call; only the innermost callback handles the
     * creation. Earlier frames see the same callback too, so idempotent
     * installation remains required. No persistent thread hook or registry. */
    scope->hook=SetWindowsHookExW(WH_CALLWNDPROCRET,creation_return,NULL,GetCurrentThreadId());
    if(!scope->hook)return FALSE;
    current_scope=scope;return TRUE;
}
static void end_titles(title_scope *scope)
{current_scope=scope->previous;UnhookWindowsHookEx(scope->hook);}

#define WINDOW_HOOK(S,T) \
static decltype(&CreateWindowEx##S) create_##S=CreateWindowEx##S; \
static HWND WINAPI window_##S(DWORD ex,T cls,T text,DWORD style,int x,int y,int cx,int cy,HWND parent,HMENU menu,HINSTANCE instance,void *param) \
{ title_scope scope;HWND result=NULL;DWORD error=ERROR_SUCCESS; \
  if(!begin_titles(&scope,FALSE))return NULL; \
  __try { result=create_##S(ex,cls,text,style,x,y,cx,cy,parent,menu,instance,param);error=GetLastError(); \
    if(scope.failure){if(result)DestroyWindow(result);result=NULL;error=scope.failure;} \
  } __finally { end_titles(&scope); } SetLastError(error);return result; }
WINDOW_HOOK(A,LPCSTR)
WINDOW_HOOK(W,LPCWSTR)

#define DIALOG_HOOK(NAME,TYPE,RET,MODAL,FAIL) \
static decltype(&NAME) real_##NAME=NAME; \
static RET WINAPI hook_##NAME(HINSTANCE instance,TYPE name,HWND parent,DLGPROC proc,LPARAM param) \
{ title_scope scope;RET result=FAIL;DWORD error=ERROR_SUCCESS; \
  if(!begin_titles(&scope,MODAL))return FAIL; \
  __try { result=real_##NAME(instance,name,parent,proc,param);error=GetLastError(); \
    if(scope.failure){if(!MODAL && result)DestroyWindow((HWND)result);result=FAIL;error=scope.failure;} \
  } __finally { end_titles(&scope); } SetLastError(error);return result; }
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
    if(scope.failure){result=0;error=scope.failure;} \
  } __finally { end_titles(&scope); } SetLastError(error);return result; }
MESSAGE_HOOK(MessageBoxA,(HWND owner,LPCSTR text,LPCSTR caption,UINT type),(owner,text,caption,type))
MESSAGE_HOOK(MessageBoxW,(HWND owner,LPCWSTR text,LPCWSTR caption,UINT type),(owner,text,caption,type))
MESSAGE_HOOK(MessageBoxExA,(HWND owner,LPCSTR text,LPCSTR caption,UINT type,WORD language),(owner,text,caption,type,language))
MESSAGE_HOOK(MessageBoxExW,(HWND owner,LPCWSTR text,LPCWSTR caption,UINT type,WORD language),(owner,text,caption,type,language))
MESSAGE_HOOK(MessageBoxIndirectA,(const MSGBOXPARAMSA *parameters),(parameters))
MESSAGE_HOOK(MessageBoxIndirectW,(const MSGBOXPARAMSW *parameters),(parameters))
#undef MESSAGE_HOOK

LONG nthook_attach_titles(void)
{
    LONG error=NO_ERROR;
    if(nthook_process_context.window_style!=WINDOW_STYLE_ALL)return error;
    error=DetourAttach(&(PVOID &)create_A,window_A);
    if(!error)error=DetourAttach(&(PVOID &)create_W,window_W);
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
