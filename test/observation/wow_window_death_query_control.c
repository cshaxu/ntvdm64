/* Native-only source-predicate diagnostic, not a product/window substitute.
 * Compare public logical life with queries used by capture_window/sample_links.
 * Only our own test windows are created/destroyed; no private USER is read. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

static unsigned mode, rows, dead_queryable;
static BOOL self_destroyed;

static void observe(const char *phase, HWND window)
{
    WINDOWINFO info={sizeof(info)}, link_info={sizeof(link_info)};
    DWORD pid=0, pid_after=0, tid, tid_after, instance_error, style_error, extra_error;
    LONG instance;
    DWORD style, extra;
    char name[256];
    int length;
    BOOL alive=IsWindow(window), valid_info, valid_links, alive_after, capture;
    HWND next, parent, owner, child;
    tid=GetWindowThreadProcessId(window,&pid);
    valid_info=GetWindowInfo(window,&info);
    valid_links=GetWindowInfo(window,&link_info);
    next=GetWindow(window,GW_HWNDNEXT); parent=GetAncestor(window,GA_PARENT);
    owner=GetWindow(window,GW_OWNER); child=GetWindow(window,GW_CHILD);
    SetLastError(ERROR_SUCCESS); instance=GetWindowLongA(window,GWL_HINSTANCE);
    instance_error=GetLastError();
    (void)GetWindowContextHelpId(window);
    length=GetClassNameA(window,name,sizeof(name));
    SetLastError(ERROR_SUCCESS); style=GetClassLongA(window,GCL_STYLE); style_error=GetLastError();
    SetLastError(ERROR_SUCCESS); extra=GetClassLongA(window,GCL_CBWNDEXTRA); extra_error=GetLastError();
    tid_after=GetWindowThreadProcessId(window,&pid_after);
    alive_after=IsWindow(window);
    capture=tid && valid_info && valid_links && (instance || !instance_error) &&
        length > 0 && length < (int)sizeof(name)-1 && (style || !style_error) &&
        (extra || !extra_error) && tid_after == tid && pid_after == pid;
    if (!alive && !alive_after && capture) ++dead_queryable;
    ++rows;
    printf("%u,%s,%08lX,%u,%u,%u,%u,%lu,%lu,%d,%08lX,%lu,%08lX,%lu,%lu,%lu,%u,%08lX,%08lX,%08lX,%08lX\n",
        mode,phase,(DWORD)(ULONG_PTR)window,(unsigned)alive,(unsigned)alive_after,
        (unsigned)valid_info,(unsigned)valid_links,tid,tid_after,length,
        (DWORD)instance,instance_error,style,style_error,extra,extra_error,
        (unsigned)capture,(DWORD)(ULONG_PTR)next,(DWORD)(ULONG_PTR)parent,
        (DWORD)(ULONG_PTR)owner,(DWORD)(ULONG_PTR)child);
}

static LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    LRESULT result;
    if (message == WM_APP+7) {
        self_destroyed=DestroyWindow(window);
        observe("self-after-destroy",window);
        return self_destroyed;
    }
    if (message == WM_DESTROY) observe("destroy",window);
    if (message == WM_NCDESTROY) {
        observe("nc-before-default",window);
        result=DefWindowProcA(window,message,wp,lp);
        observe("nc-after-default",window);
        return result;
    }
    return DefWindowProcA(window,message,wp,lp);
}

int main(void)
{
    WNDCLASSA cls={0}; HWND window;
    cls.lpfnWndProc=window_proc; cls.hInstance=GetModuleHandleA(NULL);
    cls.lpszClassName="T447_NATIVE_DEATH_QUERY";
    if (!RegisterClassA(&cls)) return 1;
    puts("mode,phase,hwnd,aliveBefore,aliveAfter,windowInfo,linkInfo,tidBefore,tidAfter,classBytes,instance,instanceError,classStyle,styleError,classExtra,extraError,capturePredicate,next,parent,owner,child");
    for(mode=0;mode<2;++mode){
        window=CreateWindowA(cls.lpszClassName,"Owned death query",WS_POPUP,
            0,0,1,1,NULL,NULL,cls.hInstance,NULL);
        if (!window) return 2;
        observe("before",window);
        if (!mode) {
            if (!DestroyWindow(window)) return 3;
            observe("direct-after",window);
        }else{
            if (!SendMessageA(window,WM_APP+7,0,0) || !self_destroyed) return 4;
            observe("self-after-send",window);
        }
    }
    if (!UnregisterClassA(cls.lpszClassName,cls.hInstance) || rows != 11) return 5;
    fprintf(stderr,"NATIVE_DEATH_QUERY rows=%u dead_queryable=%u diagnostic_only=1\n",rows,dead_queryable);
    return 0;
}
