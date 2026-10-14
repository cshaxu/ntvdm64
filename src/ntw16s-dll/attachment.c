#include "ntw16s.h"
#include "theme.h"
#include <commctrl.h>
#include <stdlib.h>

/* This coordinator composes the one existing renderer. It observes only
 * creation inside explicit caller scopes; it never scans a desktop or infers
 * task policy from an owner, title, executable name or a process-wide switch. */
typedef struct pending_window {struct pending_window *next;HWND window;DWORD style;} pending_window;
struct ntw16s_creation_scope {
    struct ntw16s_creation_scope *previous;
    DWORD thread;
    BOOL enabled;
    pending_window *pending;
    ntw16s_creation_result result;
};
typedef struct attachment {
    unsigned calls;
    BOOL detached,frame,control,title;
    HWND themed;
} attachment;
static __declspec(thread) ntw16s_creation_scope *current;
static __declspec(thread) HHOOK creation_return_hook;
static __declspec(thread) HHOOK creation_before_hook;
static LRESULT CALLBACK attachment_proc(HWND,UINT,WPARAM,LPARAM,UINT_PTR,DWORD_PTR);

static attachment *lookup(HWND w)
{
    DWORD_PTR value;
    return GetWindowSubclass(w,attachment_proc,(UINT_PTR)attachment_proc,&value)?(attachment *)value:NULL;
}
static BOOL class_is(HWND w,const WCHAR *name)
{
    WCHAR value[64];
    return GetClassNameW(w,value,ARRAYSIZE(value)) && !lstrcmpiW(value,name);
}
static void release(attachment *a)
{if(!--a->calls && a->detached)free(a);}
static LRESULT CALLBACK attachment_proc(HWND w,UINT m,WPARAM p,LPARAM l,UINT_PTR id,DWORD_PTR data)
{
    attachment *a=(attachment *)data;LRESULT result;(void)id;
    ++a->calls;
    if(m==WM_NCDESTROY){
        RemoveWindowSubclass(w,attachment_proc,(UINT_PTR)attachment_proc);
        a->detached=TRUE;
        if(a->themed)ntw16s_theme_remove(a->themed,a);
    }
    result=DefSubclassProc(w,m,p,l);
    release(a);return result;
}
BOOL __cdecl Ntw16sWindowAttached(HWND w)
{
    if(GetWindowThreadProcessId(w,NULL)!=GetCurrentThreadId()){
        SetLastError(ERROR_INVALID_THREAD_ID);return FALSE;
    }
    return lookup(w)!=NULL;
}
void __cdecl Ntw16sScrollChanged(HWND w)
{
    if(GetWindowThreadProcessId(w,NULL)==GetCurrentThreadId() && lookup(w)){
        if(class_is(w,L"SCROLLBAR"))RedrawWindow(w,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW);
        else win16_style_scrollbars_paint(w,NULL,NULL);
    }
}
BOOL __cdecl Ntw16sWindowRemove(HWND w)
{
    attachment *a;
    if(GetWindowThreadProcessId(w,NULL)!=GetCurrentThreadId()){
        SetLastError(ERROR_INVALID_THREAD_ID);return FALSE;
    }
    a=lookup(w);if(!a){SetLastError(ERROR_NOT_FOUND);return FALSE;}
    ++a->calls;
    RemoveWindowSubclass(w,attachment_proc,(UINT_PTR)attachment_proc);
    a->detached=TRUE;
    /* Existing owners handle reentrant destruction/removal independently. */
    if(a->title)Ntw16sTitleRemove(w);
    if(a->frame)win16_style_remove(w);
    if(a->control)win16_style_control_remove(w);
    if(a->themed)ntw16s_theme_remove(a->themed,a);
    ntw16s_theme_remove(w,NULL);
    release(a);return TRUE;
}

static BOOL install_created(HWND w,DWORD original_style)
{
    BOOL control,edit,installed;HWND parent;attachment *a;DWORD error;
    if(lookup(w))return TRUE;
    /* COMBOBOX installs its own internal list when construction finishes.
     * The internal EDIT also belongs to that native composite, not a frame. */
    for(parent=GetParent(w);parent;parent=GetParent(parent))
        if(class_is(parent,L"COMBOBOX"))return TRUE;
    if(class_is(w,L"ComboLBox") || class_is(w,L"#32768"))return TRUE;
    edit=(original_style&WS_CHILD) && class_is(w,L"EDIT");
    control=(original_style&WS_CHILD) && (edit || class_is(w,L"BUTTON") || class_is(w,L"STATIC") ||
        class_is(w,L"LISTBOX") || class_is(w,L"COMBOBOX") || class_is(w,L"SCROLLBAR"));
    /* A custom client-only child and MDICLIENT keep application painting.
     * MDI child frames have WS_CAPTION and take the ordinary frame route. */
    if(!control && (original_style&WS_CHILD) && !(original_style&(WS_CAPTION|WS_THICKFRAME))){
        SetLastError(ERROR_NOT_SUPPORTED);return FALSE;
    }
    a=calloc(1,sizeof(*a));
    if(!a){SetLastError(ERROR_NOT_ENOUGH_MEMORY);return FALSE;}
    a->calls=1;
    if(!SetWindowSubclass(w,attachment_proc,(UINT_PTR)attachment_proc,(DWORD_PTR)a)){
        free(a);return FALSE;
    }
    SetLastError(ERROR_SUCCESS);
    if(edit)installed=win16_style_edit_install(w,(original_style&WS_BORDER)!=0,NULL);
    else if(control && class_is(w,L"LISTBOX") && (original_style&LBS_USETABSTOPS))
        installed=win16_style_tabbed_list_install(w,NULL,0,NULL);
    else installed=control?win16_style_control_install(w,NULL):win16_style_install(w,NULL);
    error=GetLastError();
    if(installed && !a->detached){
        a->control=control;a->frame=!control;
        if(control){
            HWND themed=NULL;BOOL list=FALSE;
            if(class_is(w,L"COMBOBOX")){
                COMBOBOXINFO info={sizeof(info)};
                if(GetComboBoxInfo(w,&info)){themed=info.hwndList;list=TRUE;}
            }else if(class_is(w,L"LISTBOX")){themed=w;list=TRUE;}
            else if(edit && (original_style&ES_MULTILINE))themed=w;
            if(themed){
                a->themed=themed;
                if(!ntw16s_theme_enable(themed,a))installed=FALSE;
                else {
                    installed=list?win16_style_list_scrollbar_enable(themed):win16_style_edit_scrollbars_enable(themed);}
                error=GetLastError();
            }
        }
        if(installed && !control && (original_style&WS_CAPTION)==WS_CAPTION){
            installed=Ntw16sTitleInstall(w);error=GetLastError();
            if(installed && !a->detached)a->title=TRUE;
        }
    }
    if(a->detached){installed=FALSE;error=ERROR_CANCELLED;}
    if(!installed && !a->detached)Ntw16sWindowRemove(w);
    release(a);
    if(!installed)SetLastError(error?error:ERROR_GEN_FAILURE);
    return installed;
}
static LRESULT CALLBACK creation_before(int code,WPARAM p,LPARAM l)
{
    ntw16s_creation_scope *scope=current;DWORD saved=GetLastError();
    if(code==HCBT_CREATEWND && scope && scope->enabled){
        const CBT_CREATEWNDW *create=(const CBT_CREATEWNDW *)l;
        pending_window *pending=calloc(1,sizeof(*pending));
        if(!pending){scope->result.failure=ERROR_NOT_ENOUGH_MEMORY;return 1;}
        pending->window=(HWND)p;pending->style=(DWORD)create->lpcs->style;
        pending->next=scope->pending;scope->pending=pending;
        if(class_is((HWND)p,L"LISTBOX") || class_is((HWND)p,L"ComboLBox") ||
           (class_is((HWND)p,L"EDIT") && (pending->style&ES_MULTILINE))){
            if(!ntw16s_theme_capture((HWND)p)){
                scope->result.failure=GetLastError();return 1;
            }
        }
    }
    SetLastError(saved);return CallNextHookEx(NULL,code,p,l);
}
static pending_window *take_pending(ntw16s_creation_scope *scope,HWND w)
{
    pending_window **link=&scope->pending,*found;
    while(*link && (*link)->window!=w)link=&(*link)->next;
    found=*link;if(found)*link=found->next;return found;
}
static LRESULT CALLBACK creation_return(int code,WPARAM p,LPARAM l)
{
    LRESULT result=CallNextHookEx(NULL,code,p,l);DWORD saved=GetLastError();
    ntw16s_creation_scope *scope=current;
    if(code>=0 && scope && scope->enabled){
        const CWPRETSTRUCT *m=(const CWPRETSTRUCT *)l;
        BOOL created=m->message==WM_CREATE && m->lResult!=-1;
        if(m->message==WM_NCDESTROY){pending_window *pending=take_pending(scope,m->hwnd);free(pending);}
        if((created || m->message==WM_INITDIALOG) && IsWindow(m->hwnd)){
            pending_window *pending=take_pending(scope,m->hwnd);
            DWORD style=created?(DWORD)((const CREATESTRUCTW *)m->lParam)->style:
                (DWORD)GetWindowLongPtrW(m->hwnd,GWL_STYLE);
            if(pending){style=pending->style;free(pending);}
            BOOL already=lookup(m->hwnd)!=NULL;
            if(install_created(m->hwnd,style)){
                if(!already && lookup(m->hwnd))++scope->result.installed;
            }else{
                DWORD error=GetLastError();
                if(error==ERROR_NOT_SUPPORTED){
                    ++scope->result.unsupported;ntw16s_theme_remove(m->hwnd,NULL);
                    if(class_is(m->hwnd,L"COMBOBOX")){
                        COMBOBOXINFO info={sizeof(info)};
                        if(GetComboBoxInfo(m->hwnd,&info))ntw16s_theme_remove(info.hwndList,NULL);
                    }
                }
                else if(!scope->result.failure)scope->result.failure=error?error:ERROR_GEN_FAILURE;
            }
            if(scope->result.failure && m->message==WM_INITDIALOG)
                EndDialog(m->hwnd,-1);
        }
    }
    SetLastError(saved);return result;
}
ntw16s_creation_scope *__cdecl Ntw16sCreationBegin(BOOL enabled)
{
    ntw16s_creation_scope *scope=calloc(1,sizeof(*scope));
    if(!scope){SetLastError(ERROR_NOT_ENOUGH_MEMORY);return NULL;}
    if(!current){
        creation_before_hook=SetWindowsHookExW(WH_CBT,creation_before,NULL,GetCurrentThreadId());
        if(!creation_before_hook){free(scope);return NULL;}
        creation_return_hook=SetWindowsHookExW(WH_CALLWNDPROCRET,creation_return,NULL,GetCurrentThreadId());
        if(!creation_return_hook){DWORD error=GetLastError();UnhookWindowsHookEx(creation_before_hook);creation_before_hook=NULL;free(scope);SetLastError(error);return NULL;}
    }
    scope->thread=GetCurrentThreadId();scope->enabled=enabled;
    scope->previous=current;current=scope;return scope;
}
BOOL __cdecl Ntw16sCreationEnd(ntw16s_creation_scope *scope,ntw16s_creation_result *result)
{
    if(!scope || scope!=current){SetLastError(ERROR_INVALID_STATE);return FALSE;}
    if(scope->thread!=GetCurrentThreadId()){SetLastError(ERROR_INVALID_THREAD_ID);return FALSE;}
    current=scope->previous;if(result)*result=scope->result;
    while(scope->pending){pending_window *pending=scope->pending;scope->pending=pending->next;free(pending);}
    if(!current){UnhookWindowsHookEx(creation_return_hook);creation_return_hook=NULL;
        UnhookWindowsHookEx(creation_before_hook);creation_before_hook=NULL;}
    free(scope);return TRUE;
}
