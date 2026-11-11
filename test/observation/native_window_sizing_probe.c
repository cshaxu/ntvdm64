/* Native USER sizing control. No WOW code and no hardcoded expected metric.
 * All windows live on this process's disposable desktop and are never shown. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
static const char *label;
static LRESULT CALLBACK procedure(HWND w, UINT m, WPARAM wp, LPARAM lp)
{
    if(m==WM_GETMINMAXINFO) {
        MINMAXINFO *p=(MINMAXINFO*)lp;
        printf("%s minmax-in min=%ld,%ld max=%ld,%ld\n",label,
            p->ptMinTrackSize.x,p->ptMinTrackSize.y,p->ptMaxTrackSize.x,p->ptMaxTrackSize.y);
    }
    if(m==WM_WINDOWPOSCHANGING || m==WM_WINDOWPOSCHANGED) {
        WINDOWPOS *p=(WINDOWPOS*)lp;
        printf("%s position message=%04X size=%d,%d flags=%04X\n",label,m,p->cx,p->cy,p->flags);
    }
    return DefWindowProcA(w,m,wp,lp);
}
int main(void)
{
    HDESK previous=GetThreadDesktop(GetCurrentThreadId()),desktop;
    char name[80]; WNDCLASSA wc={0}; unsigned i; int failed=0;
    const DWORD styles[]={WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_CLIPSIBLINGS,
        WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_CLIPSIBLINGS|WS_THICKFRAME,
        WS_POPUP|WS_BORDER, WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MINIMIZE};
    const char *names[]={"fixed-caption","sizeable-caption","popup-border","minimized-caption"};
    sprintf_s(name,sizeof(name),"NativeSizing-%lu",GetCurrentProcessId());
    desktop=CreateDesktopA(name,NULL,NULL,0,GENERIC_ALL,NULL);
    if(!desktop || !SetThreadDesktop(desktop))return 2;
    wc.lpfnWndProc=procedure;wc.hInstance=GetModuleHandleA(NULL);wc.lpszClassName="NativeSizingControl";
    if(!RegisterClassA(&wc))return 3;
    printf("metrics mintrack=%d,%d size=%d,%d frame=%d,%d dpi=%u\n",
        GetSystemMetrics(SM_CXMINTRACK),GetSystemMetrics(SM_CYMINTRACK),
        GetSystemMetrics(SM_CXSIZE),GetSystemMetrics(SM_CYSIZE),
        GetSystemMetrics(SM_CXFRAME),GetSystemMetrics(SM_CYFRAME),GetDpiForSystem());
    printf("minimized-metrics=%d,%d\n",GetSystemMetrics(SM_CXMINIMIZED),GetSystemMetrics(SM_CYMINIMIZED));
    for(i=0;i<sizeof(styles)/sizeof(styles[0]);++i) {
        HWND w; RECT r={0}; label=names[i];
        w=CreateWindowExA(0,wc.lpszClassName,"Sizing control",styles[i],0,0,154,240,NULL,NULL,wc.hInstance,NULL);
        if(!w || !GetWindowRect(w,&r)){failed=1;break;}
        printf("%s request=154,240 actual=%ld,%ld style=%08lX\n",label,r.right-r.left,r.bottom-r.top,styles[i]);
        if(styles[i]&WS_MINIMIZE){
            if(!MoveWindow(w,10,10,154,240,FALSE))failed=1;
            ShowWindow(w,SW_RESTORE);
            GetWindowRect(w,&r);
            printf("%s after-move154-and-restore=%ld,%ld iconic=%d\n",label,r.right-r.left,r.bottom-r.top,IsIconic(w));
        }
        if(!DestroyWindow(w)){failed=1;break;}
    }
    if(!UnregisterClassA(wc.lpszClassName,wc.hInstance))failed=1;
    if(!SetThreadDesktop(previous))failed=1;
    if(!CloseDesktop(desktop))failed=1;
    return failed;
}
