#include "title_marker.h"
#include <commctrl.h>
#include <wchar.h>
#include <string.h>
#include <limits.h>

typedef struct title_marker {
    unsigned references;
    BOOL detached;
    WCHAR *source,*display;
} title_marker;
static const WCHAR prefix[]=L"(Win16 Style) ";
static LRESULT CALLBACK title_proc(HWND,UINT,WPARAM,LPARAM,UINT_PTR,DWORD_PTR);

static void release(title_marker *state)
{
    if(--state->references)return;
    if(state->source)HeapFree(GetProcessHeap(),0,state->source);
    if(state->display)HeapFree(GetProcessHeap(),0,state->display);
    HeapFree(GetProcessHeap(),0,state);
}
static BOOL local_window(HWND window)
{
    DWORD process=0,thread=GetWindowThreadProcessId(window,&process);
    if(!thread){SetLastError(ERROR_INVALID_WINDOW_HANDLE);return FALSE;}
    if(process!=GetCurrentProcessId() || thread!=GetCurrentThreadId()) {
        SetLastError(ERROR_INVALID_THREAD_ID);return FALSE;
    }
    return TRUE;
}
static WCHAR *copy_text(HWND window,LPARAM text)
{
    int count;WCHAR *copy;
    if(!text)text=(LPARAM)(IsWindowUnicode(window) ? (const void *)L"" : (const void *)"");
    if(IsWindowUnicode(window)) {
        size_t bytes=(wcslen((PCWSTR)text)+1)*sizeof(WCHAR);
        copy=HeapAlloc(GetProcessHeap(),0,bytes);
        if(copy)memcpy(copy,(const void *)text,bytes);
        return copy;
    }
    count=MultiByteToWideChar(CP_ACP,0,(PCSTR)text,-1,NULL,0);
    if(!count)return NULL;
    copy=HeapAlloc(GetProcessHeap(),0,(size_t)count*sizeof(WCHAR));
    if(copy && !MultiByteToWideChar(CP_ACP,0,(PCSTR)text,-1,copy,count)) {
        HeapFree(GetProcessHeap(),0,copy);return NULL;
    }
    return copy;
}
static WCHAR *read_text(HWND window)
{
    int length=GetWindowTextLengthW(window);WCHAR *text;
    if(length==INT_MAX)return NULL;
    text=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,((size_t)length+1)*sizeof(WCHAR));
    if(text)GetWindowTextW(window,text,length+1);
    return text;
}
static void detach(HWND window,title_marker *state)
{
    if(state->detached)return;
    RemoveWindowSubclass(window,title_proc,(UINT_PTR)title_proc);
    state->detached=TRUE;release(state);
}
static LRESULT CALLBACK title_proc(HWND window,UINT message,WPARAM wp,LPARAM lp,
    UINT_PTR id,DWORD_PTR data)
{
    title_marker *state=(title_marker *)data;LRESULT result;
    (void)id;++state->references;
    if(message==WM_NCDESTROY)detach(window,state);
    if(message==WM_SETTEXT && !state->detached) {
        WCHAR *source=copy_text(window,lp),*display=NULL,*actual=NULL;
        char *ansi=NULL;size_t count;
        if(!source){SetLastError(ERROR_NOT_ENOUGH_MEMORY);release(state);return FALSE;}
        /* A GetWindowText/SetWindowText round trip must not add another
         * library prefix. Do not strip arbitrary user-supplied prefix text. */
        if(state->display && !wcscmp(source,state->display)) {
            HeapFree(GetProcessHeap(),0,source);
            result=DefSubclassProc(window,message,wp,lp);release(state);return result;
        }
        count=wcslen(source)+ARRAYSIZE(prefix);
        display=HeapAlloc(GetProcessHeap(),0,count*sizeof(WCHAR));
        if(!display){HeapFree(GetProcessHeap(),0,source);release(state);return FALSE;}
        wcscpy_s(display,count,prefix);wcscat_s(display,count,source);
        if(!IsWindowUnicode(window)) {
            int bytes=WideCharToMultiByte(CP_ACP,0,display,-1,NULL,0,NULL,NULL);
            if(bytes)ansi=HeapAlloc(GetProcessHeap(),0,(size_t)bytes);
            if(!ansi || !WideCharToMultiByte(CP_ACP,0,display,-1,ansi,bytes,NULL,NULL)) {
                if(ansi)HeapFree(GetProcessHeap(),0,ansi);
                HeapFree(GetProcessHeap(),0,source);HeapFree(GetProcessHeap(),0,display);
                release(state);return FALSE;
            }
        }
        result=DefSubclassProc(window,message,wp,(LPARAM)(ansi ? (void *)ansi : display));
        if(ansi)HeapFree(GetProcessHeap(),0,ansi);
        if(IsWindow(window))actual=read_text(window);
        if(result && actual && !wcscmp(actual,display)) {
            if(state->detached)SetWindowTextW(window,source);
            else {
                if(state->source)HeapFree(GetProcessHeap(),0,state->source);
                if(state->display)HeapFree(GetProcessHeap(),0,state->display);
                state->source=source;state->display=display;source=display=NULL;
            }
        }
        if(actual)HeapFree(GetProcessHeap(),0,actual);
        if(source)HeapFree(GetProcessHeap(),0,source);
        if(display)HeapFree(GetProcessHeap(),0,display);
    } else result=DefSubclassProc(window,message,wp,lp);
    release(state);return result;
}
BOOL __cdecl Ntw16sTitleInstall(HWND window)
{
    title_marker *state;DWORD_PTR existing;WCHAR *text;BOOL result;
    if(!local_window(window))return FALSE;
    if(GetWindowSubclass(window,title_proc,(UINT_PTR)title_proc,&existing))return TRUE;
    if((GetWindowLongPtrW(window,GWL_STYLE)&WS_CAPTION)!=WS_CAPTION) {
        SetLastError(ERROR_NOT_SUPPORTED);return FALSE;
    }
    text=read_text(window);
    if(!text)return FALSE;
    state=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*state));
    if(!state){HeapFree(GetProcessHeap(),0,text);return FALSE;}
    state->references=2; /* installed ownership and this reentrant call */
    if(!SetWindowSubclass(window,title_proc,(UINT_PTR)title_proc,(DWORD_PTR)state)) {
        release(state);release(state);HeapFree(GetProcessHeap(),0,text);return FALSE;
    }
    result=(BOOL)SendMessageW(window,WM_SETTEXT,0,(LPARAM)text);
    HeapFree(GetProcessHeap(),0,text);
    if(!result || state->detached || !state->display){detach(window,state);result=FALSE;}
    release(state);return result;
}
BOOL __cdecl Ntw16sTitleRemove(HWND window)
{
    DWORD_PTR value;title_marker *state;WCHAR *actual;BOOL result=TRUE;
    if(!local_window(window))return FALSE;
    if(!GetWindowSubclass(window,title_proc,(UINT_PTR)title_proc,&value))return TRUE;
    state=(title_marker *)value;++state->references;
    actual=read_text(window);detach(window,state);
    if(actual && state->display && !wcscmp(actual,state->display))
        result=(BOOL)SendMessageW(window,WM_SETTEXT,0,(LPARAM)state->source);
    if(actual)HeapFree(GetProcessHeap(),0,actual);
    release(state);return result;
}
