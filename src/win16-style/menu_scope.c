#include "menu_scope.h"
#include <commctrl.h>
#include <stdlib.h>
#include <wchar.h>

typedef struct popup popup;
struct win16_style_menu_scope {
    win16_style_palette palette;
    struct win16_style_menu_scope *previous;
    DWORD thread;
    popup *windows;
};
struct popup {
    HWND window;popup *next;win16_style_menu_scope *scope;
    win16_style_palette palette;
    unsigned calls;
    BOOL detached;
};
static __declspec(thread) win16_style_menu_scope *current;
static __declspec(thread) HHOOK thread_hook;
static LRESULT CALLBACK popup_proc(HWND,UINT,WPARAM,LPARAM,UINT_PTR,DWORD_PTR);

static void paint_border(HWND w,popup *p,HDC target)
{
    RECT outer,client,r;HDC dc=target?target:GetWindowDC(w);int saved,u;
    if(!dc)return;
    saved=SaveDC(dc);if(!saved){if(!target)ReleaseDC(w,dc);return;}
    GetWindowRect(w,&outer);GetClientRect(w,&client);MapWindowPoints(w,NULL,(POINT *)&client,2);
    OffsetRect(&client,-outer.left,-outer.top);
    SetRect(&r,0,0,outer.right-outer.left,outer.bottom-outer.top);
    ExcludeClipRect(dc,client.left,client.top,client.right,client.bottom);
    SetDCBrushColor(dc,p->palette.light);FillRect(dc,&r,(HBRUSH)GetStockObject(DC_BRUSH));
    u=max(1,MulDiv(1,(int)GetDpiForWindow(w),96));
    SetDCBrushColor(dc,p->palette.edge);
    {RECT edge=r;edge.bottom=u;FillRect(dc,&edge,(HBRUSH)GetStockObject(DC_BRUSH));
     edge=r;edge.top=edge.bottom-u;FillRect(dc,&edge,(HBRUSH)GetStockObject(DC_BRUSH));
     edge=r;edge.right=u;FillRect(dc,&edge,(HBRUSH)GetStockObject(DC_BRUSH));
     edge=r;edge.left=edge.right-u;FillRect(dc,&edge,(HBRUSH)GetStockObject(DC_BRUSH));}
    RestoreDC(dc,saved);if(!target)ReleaseDC(w,dc);
}

static void detach(popup *p)
{
    popup **link=&p->scope->windows;
    while(*link && *link!=p)link=&(*link)->next;
    if(*link)*link=p->next;
    RemoveWindowSubclass(p->window,popup_proc,(UINT_PTR)popup_proc);
    p->detached=TRUE;p->scope=NULL;
    if(!p->calls)free(p);
}

static LRESULT CALLBACK popup_proc(HWND w,UINT m,WPARAM a,LPARAM b,UINT_PTR id,DWORD_PTR data)
{
    popup *p=(popup *)data;LRESULT result=0;(void)id;
    ++p->calls;
    if(m==WM_NCDESTROY){detach(p);result=DefSubclassProc(w,m,a,b);}
    else if(m==WM_NCPAINT)paint_border(w,p,NULL);
    else if(m==WM_PRINT){result=DefSubclassProc(w,m,a,b&~PRF_NONCLIENT);
        /* Native printing may reenter the owner, end the scope or destroy
         * this HWND. Keep the node alive but never paint after detachment. */
        if(!p->detached && (b&PRF_NONCLIENT))paint_border(w,p,(HDC)a);
    }else result=DefSubclassProc(w,m,a,b);
    if(!--p->calls && p->detached)free(p);
    return result;
}

static LRESULT CALLBACK popup_message(int code,WPARAM a,LPARAM b)
{
    const CWPSTRUCT *message=(const CWPSTRUCT *)b;
    if(code>=0 && current && (message->message==WM_WINDOWPOSCHANGING ||
        message->message==WM_NCPAINT || message->message==WM_PRINT)){
        HWND w=message->hwnd;WCHAR name[32];DWORD_PTR existing;
        if(!GetWindowSubclass(w,popup_proc,(UINT_PTR)popup_proc,&existing) &&
            GetClassNameW(w,name,32) && !wcscmp(name,L"#32768")){
            popup *p=(popup *)calloc(1,sizeof(*p));
            if(p){p->window=w;p->scope=current;p->palette=current->palette;
                if(SetWindowSubclass(w,popup_proc,(UINT_PTR)popup_proc,(DWORD_PTR)p)){
                    p->next=current->windows;current->windows=p;
                }else free(p);
            }
        }
    }
    return CallNextHookEx(NULL,code,a,b);
}

win16_style_menu_scope *win16_style_menu_begin(const win16_style_palette *palette)
{
    win16_style_menu_scope *s=(win16_style_menu_scope *)calloc(1,sizeof(*s));
    if(!s)return NULL;
    if(palette)s->palette=*palette;else win16_style_default_palette(&s->palette);
    s->thread=GetCurrentThreadId();s->previous=current;
    /* One hook for the nested scope stack, never a desktop/global hook. */
    if(!current){thread_hook=SetWindowsHookExW(WH_CALLWNDPROC,popup_message,NULL,s->thread);
        if(!thread_hook){free(s);return NULL;}}
    current=s;return s;
}

BOOL win16_style_menu_end(win16_style_menu_scope *s)
{
    win16_style_menu_scope **link=&current;
    while(*link && *link!=s)link=&(*link)->previous;
    if(!s || !*link || s->thread!=GetCurrentThreadId()){
        SetLastError(ERROR_INVALID_PARAMETER);return FALSE;}
    while(s->windows)detach(s->windows);
    *link=s->previous;
    if(!current){UnhookWindowsHookEx(thread_hook);thread_hook=NULL;}
    free(s);return TRUE;
}
