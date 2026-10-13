#include "controls.h"
#include <stdlib.h>

struct win16_style_control_scope {
    struct win16_style_control_scope *next;
    HWND root;
    win16_style_palette palette;
    win16_style_control_counts counts;
    unsigned calls;
    BOOL detached;
};
static __declspec(thread) win16_style_control_scope *scopes;
static __declspec(thread) HHOOK creation_hook;

static BOOL class_is(HWND w,const WCHAR *expected)
{WCHAR name[32];return GetClassNameW(w,name,32) && !lstrcmpiW(name,expected);}

static LRESULT CALLBACK created(int code,WPARAM a,LPARAM b)
{
    LRESULT result=CallNextHookEx(NULL,code,a,b);DWORD error=GetLastError();
    const CWPRETSTRUCT *m=(const CWPRETSTRUCT *)b;win16_style_control_scope *s;
    if(code<0) return result;
    if(m->message==WM_NCDESTROY){
        for(s=scopes;s;s=s->next)if(s->root==m->hwnd)s->root=NULL;
    }else if(m->message==WM_CREATE && m->lResult!=-1){
        HWND ancestor;const CREATESTRUCTW *create=(const CREATESTRUCTW *)m->lParam;
        BOOL edit=class_is(m->hwnd,L"EDIT"),internal=FALSE;
        if(!edit && !class_is(m->hwnd,L"BUTTON") && !class_is(m->hwnd,L"STATIC") &&
            !class_is(m->hwnd,L"LISTBOX") && !class_is(m->hwnd,L"COMBOBOX") &&
            !class_is(m->hwnd,L"SCROLLBAR"))goto done;
        /* A combo owns installation of its internal list; do not attach it
         * prematurely while the native combo is still constructing itself. */
        for(ancestor=GetParent(m->hwnd);ancestor;ancestor=GetParent(ancestor))
            if(class_is(ancestor,L"COMBOBOX")){internal=TRUE;break;}
        if(internal)goto done;
        for(s=scopes;s;s=s->next)if(s->root && IsChild(s->root,m->hwnd))break;
        if(s){BOOL installed;DWORD failure;++s->calls;
            installed=edit?win16_style_edit_install(m->hwnd,(create->style&WS_BORDER)!=0,&s->palette):
                win16_style_control_install(m->hwnd,&s->palette);
            failure=GetLastError();
            if(installed)++s->counts.installed;
            else if(failure==ERROR_NOT_SUPPORTED)++s->counts.unsupported;
            else ++s->counts.failed;
            if(!--s->calls && s->detached)free(s);
        }
    }
done:
    SetLastError(error);return result;
}

win16_style_control_scope *win16_style_controls_begin(HWND root,const win16_style_palette *palette)
{
    win16_style_control_scope *s;
    if(GetWindowThreadProcessId(root,NULL)!=GetCurrentThreadId()){SetLastError(ERROR_INVALID_THREAD_ID);return NULL;}
    s=calloc(1,sizeof(*s));if(!s){SetLastError(ERROR_NOT_ENOUGH_MEMORY);return NULL;}
    if(!scopes){creation_hook=SetWindowsHookExW(WH_CALLWNDPROCRET,created,NULL,GetCurrentThreadId());
        if(!creation_hook){free(s);return NULL;}}
    s->root=root;if(palette)s->palette=*palette;else win16_style_default_palette(&s->palette);
    s->next=scopes;scopes=s;return s;
}

BOOL win16_style_controls_end(win16_style_control_scope *s,win16_style_control_counts *counts)
{
    win16_style_control_scope **link=&scopes;
    while(*link && *link!=s)link=&(*link)->next;
    if(!s || !*link){SetLastError(ERROR_INVALID_PARAMETER);return FALSE;}
    *link=s->next;s->detached=TRUE;if(counts)*counts=s->counts;
    if(!scopes){UnhookWindowsHookEx(creation_hook);creation_hook=NULL;}
    if(!s->calls)free(s);return TRUE;
}
