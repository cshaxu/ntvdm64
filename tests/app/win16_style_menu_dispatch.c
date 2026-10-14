/* Regression for host menu drawing that bypasses WM_NCPAINT on hover.
 * The witness consumes opaque test payloads; it does not emulate UAH. */
#include "ntw16s-dll/win16_style.h"
#include "ntw16s-dll/controls.h"
#include <commctrl.h>
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"MENU DISPATCH FAIL line=%d error=%lu\n",__LINE__,GetLastError());return 1;}}while(0)
static BOOL probing;
static unsigned forwarded;
static BOOL damage_probe;
static unsigned client_paints,erases,ui_updates,idles;
static LRESULT CALLBACK damage(HWND w,UINT m,WPARAM a,LPARAM b,UINT_PTR id,DWORD_PTR data)
{
    (void)id;(void)data;
    if(damage_probe){if(m==WM_PAINT)++client_paints;if(m==WM_ERASEBKGND)++erases;if(m==WM_UPDATEUISTATE)++ui_updates;}
    return DefSubclassProc(w,m,a,b);
}
static LRESULT CALLBACK witness(HWND w,UINT m,WPARAM a,LPARAM b)
{
    if(probing && m>=0x91 && m<=0x95){++forwarded;return 73;}
    if(damage_probe){
        if(m==WM_ENTERIDLE && a==MSGF_MENU){++idles;EndMenu();return 0;}
        if(m==WM_TIMER && a==998){EndMenu();return 0;}
    }
    return DefWindowProcW(w,m,a,b);
}
int main(void)
{
    WCHAR name[80];HDESK desktop;WNDCLASSW cls={0};HWND w;HMENU menu;
    unsigned i,phase,round;UINT draw[]={0x91,0x92};DWORD before,after;
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    swprintf_s(name,80,L"StyleMenuDispatch-%lu",GetCurrentProcessId());
    desktop=CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL);
    CHECK(desktop && SetThreadDesktop(desktop));
    cls.lpfnWndProc=witness;cls.hInstance=GetModuleHandleW(NULL);cls.lpszClassName=L"StyleMenuDispatch";
    CHECK(RegisterClassW(&cls));
    w=CreateWindowW(cls.lpszClassName,L"File hover routing",WS_OVERLAPPEDWINDOW,
        0,0,400,220,NULL,NULL,cls.hInstance,NULL);CHECK(w);
    menu=CreateMenu();CHECK(menu && AppendMenuW(menu,MF_STRING,1,L"&File") && SetMenu(w,menu));
    ShowWindow(w,SW_MINIMIZE);ShowWindow(w,SW_RESTORE);UpdateWindow(w);
    /* Warm every minimize/restore/menu-detach phase before the measured
     * cycles. A lone install/remove does not warm native asynchronous frame
     * state; retain the cold observations, but measure steady-state lifetime. */
    CHECK(win16_style_install(w,NULL) && win16_style_remove(w));
    for(round=0;round<16;++round){
    GdiFlush();before=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);
    CHECK(win16_style_install(w,NULL));
    for(phase=0;phase<4;++phase){
        if(phase==1){ShowWindow(w,SW_MINIMIZE);CHECK(IsIconic(w));}
        if(phase==2){ShowWindow(w,SW_RESTORE);CHECK(!IsIconic(w));CHECK(SetMenu(w,NULL));}
        if(phase==3){CHECK(SetMenu(w,menu) && win16_style_remove(w));}
        probing=TRUE;
        for(i=0;i<2;++i){LRESULT result;forwarded=0;
            result=SendMessageW(w,draw[i],0,(LPARAM)1);
            CHECK(phase==0?(result==0 && forwarded==0):(result==73 && forwarded==1));
        }
        /* Measurement/initialization traffic must not be blanket-suppressed. */
        forwarded=0;CHECK(SendMessageW(w,0x93,0,(LPARAM)1)==73 && forwarded==1);
        probing=FALSE;printf("MENU DISPATCH PASS phase=%u draw ownership and measurement forwarding\n",phase);
    }
    {MSG pending;while(PeekMessageW(&pending,NULL,0,0,PM_REMOVE)){TranslateMessage(&pending);DispatchMessageW(&pending);}}
    GdiFlush();after=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);
    printf("MENU DISPATCH GDI round=%u phase=%s objects=%lu/%lu\n",round,round<8?"warm":"measured",before,after);
    if(round>=8)CHECK(after<=before);
    }
    {HWND button;HMENU popup=CreatePopupMenu();POINT click={12,0};MSG message;RECT bounds;
        HDC dc;COLORREF original,observed;int x,y;
        CHECK(popup && AppendMenuW(popup,MF_STRING,3,L"&Exit test"));
        CHECK(DeleteMenu(menu,0,MF_BYPOSITION) && AppendMenuW(menu,MF_POPUP,(UINT_PTR)popup,L"&File"));
        CHECK(win16_style_install(w,NULL));
        button=CreateWindowW(L"BUTTON",L"Unrelated control",WS_CHILD|WS_VISIBLE,60,80,240,40,w,NULL,cls.hInstance,NULL);
        CHECK(button && win16_style_control_install(button,NULL));
        CHECK(SetWindowSubclass(w,damage,1,0) && SetWindowSubclass(button,damage,1,0));
        SetActiveWindow(w);SetFocus(w);RedrawWindow(w,NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN|RDW_UPDATENOW);
        while(PeekMessageW(&message,NULL,0,0,PM_REMOVE)){TranslateMessage(&message);DispatchMessageW(&message);}
        CHECK(GetWindowRect(w,&bounds));x=bounds.right-bounds.left-2;y=(bounds.bottom-bounds.top)/2;
        dc=GetWindowDC(w);CHECK(dc);original=GetPixel(dc,x,y);
        CHECK(SetPixel(dc,x,y,RGB(37,83,119))!=CLR_INVALID);ReleaseDC(w,dc);
        ClientToScreen(w,&click);click.y-=12;damage_probe=TRUE;CHECK(SetTimer(w,998,1000,NULL));
        {LRESULT hit=SendMessageW(w,WM_NCHITTEST,0,MAKELPARAM(click.x,click.y));
            CHECK(hit==HTBORDER);
            SendMessageW(w,WM_NCLBUTTONDOWN,(WPARAM)hit,MAKELPARAM(click.x,click.y));}
        KillTimer(w,998);
        UpdateWindow(w);UpdateWindow(button);damage_probe=FALSE;
        printf("MENU CLICK DAMAGE idle=%u clientPaint=%u erase=%u updateUIState=%u\n",idles,client_paints,erases,ui_updates);
        dc=GetWindowDC(w);CHECK(dc);observed=GetPixel(dc,x,y);SetPixel(dc,x,y,original);ReleaseDC(w,dc);
        printf("MENU CLICK FRAME WITNESS=%06lx expected=%06lx\n",observed,(COLORREF)RGB(37,83,119));
        CHECK(idles && !client_paints && !erases && !ui_updates && observed==RGB(37,83,119));
        dc=GetWindowDC(w);CHECK(dc);CHECK(SetPixel(dc,x,y,RGB(37,83,119))!=CLR_INVALID);ReleaseDC(w,dc);
        idles=client_paints=erases=ui_updates=0;damage_probe=TRUE;CHECK(SetTimer(w,998,1000,NULL));
        SendMessageW(w,WM_SYSCOMMAND,SC_KEYMENU,L' ');KillTimer(w,998);damage_probe=FALSE;
        dc=GetWindowDC(w);CHECK(dc);observed=GetPixel(dc,x,y);SetPixel(dc,x,y,original);ReleaseDC(w,dc);
        printf("SYSTEM MENU DAMAGE idle=%u clientPaint=%u erase=%u frame=%06lx\n",idles,client_paints,erases,observed);
        CHECK(idles && !client_paints && !erases && observed==RGB(37,83,119));
    }
    CHECK(DestroyWindow(w));CHECK(after<=before);return 0;
}
