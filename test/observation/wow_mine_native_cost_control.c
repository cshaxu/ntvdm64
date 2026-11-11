/* Independently authored native reference for the observer, not a WINMINE
 * implementation or a substitute for actual WOW gameplay. Only the sampled
 * small rectangles and posted-pair/paint/normal-exit protocol are comparable. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
static unsigned downs,ups,paints,errors;
static BOOL held,flag;
static void paint_board(HWND window)
{
    PAINTSTRUCT ps;
    HDC dc=BeginPaint(window,&ps);
    HBRUSH light=CreateSolidBrush(RGB(192,192,192)),dark=CreateSolidBrush(RGB(0,0,0));
    RECT area;
    int i;
    GetClientRect(window,&area);FillRect(dc,&area,light);
    for(i=0;i<3;++i){
        RECT cell={12+16*i,54,26+16*i,68};
        FillRect(dc,&cell,dark);InflateRect(&cell,-2,-2);FillRect(dc,&cell,light);
    }
    if(flag){RECT mark={17,58,23,64};FillRect(dc,&mark,dark);}
    area.left=16;area.top=16;area.right=56;area.bottom=40;FillRect(dc,&area,dark);
    area.left=20;area.top=20;area.right=25;area.bottom=35;FillRect(dc,&area,light);
    DeleteObject(dark);DeleteObject(light);EndPaint(window,&ps);++paints;
}
static LRESULT CALLBACK window_proc(HWND window,UINT message,WPARAM wp,LPARAM lp)
{
    switch(message){
    case WM_PAINT:paint_board(window);return 0;
    case WM_RBUTTONDOWN:
        if(held || LOWORD(lp)!=19 || HIWORD(lp)!=61 || wp!=MK_RBUTTON)++errors;
        held=TRUE;++downs;return 0;
    case WM_RBUTTONUP:
        if(!held || LOWORD(lp)!=19 || HIWORD(lp)!=61 || wp)++errors;
        held=FALSE;flag=!flag;++ups;InvalidateRect(window,NULL,FALSE);return 0;
    case WM_CLOSE:DestroyWindow(window);return 0;
    case WM_DESTROY:PostQuitMessage(0);return 0;
    }
    return DefWindowProcA(window,message,wp,lp);
}
int main(int argc,char **argv)
{
    WNDCLASSA cls={0};
    HWND window;
    HDESK original=GetThreadDesktop(GetCurrentThreadId()),desktop=NULL;
    MSG message;
    BOOL result;
    STARTUPINFOA startup={sizeof(startup)};
    BOOL visible_before;
    if(argc!=2)return 64;
    if(strcmp(argv[1],"-")){
        if(strncmp(argv[1],"NTVDMObserverCost-",18))return 65;
        desktop=CreateDesktopA(argv[1],NULL,NULL,0,DESKTOP_CREATEWINDOW|
            DESKTOP_ENUMERATE|DESKTOP_READOBJECTS|DESKTOP_WRITEOBJECTS,NULL);
        if(!desktop || !SetThreadDesktop(desktop))return 66;
    }
    cls.lpfnWndProc=window_proc;cls.hInstance=GetModuleHandleA(NULL);
    cls.lpszClassName="Minesweeper";cls.hCursor=LoadCursor(NULL,IDC_ARROW);
    if(!RegisterClassA(&cls))return 67;
    window=CreateWindowExA(0,cls.lpszClassName,"Native observer-cost reference",
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,0,0,200,240,NULL,NULL,cls.hInstance,NULL);
    if(!window)return 68;
    /* The owning background runner hides its startup console. Make only
     * this test window visible explicitly, without activating another app;
     * do not depend on the inherited first-ShowWindow startup policy. */
    GetStartupInfoA(&startup);visible_before=IsWindowVisible(window);
    if(!SetWindowPos(window,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|
            SWP_NOACTIVATE|SWP_SHOWWINDOW))return 69;
    printf("native-start pid=%lu window=%p visible-before=%u visible-after=%u startup-flags=%lu startup-show=%u\n",
        GetCurrentProcessId(),window,visible_before,IsWindowVisible(window),
        startup.dwFlags,startup.wShowWindow);fflush(stdout);
    while((result=GetMessageA(&message,NULL,0,0))>0){TranslateMessage(&message);DispatchMessageA(&message);}
    if(result==(BOOL)-1)++errors;
    if(downs!=12 || ups!=12 || held || flag || paints<13)++errors;
    UnregisterClassA(cls.lpszClassName,cls.hInstance);
    if(desktop){if(!SetThreadDesktop(original) || !CloseDesktop(desktop))++errors;}
    printf("NATIVE_OBSERVER_CONTROL errors=%u down=%u up=%u paints=%u flag=%u\n",errors,downs,ups,paints,flag);
    return errors?1:0;
}
