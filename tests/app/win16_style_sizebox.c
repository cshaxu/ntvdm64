/* Native size-box command oracle; no desktop switching or cursor injection. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "win16-style/controls.h"
#define CHECK(x) do{if(!(x)){fprintf(stderr,"SIZEBOX FAIL line=%d error=%lu\n",__LINE__,GetLastError());return 1;}}while(0)
static UINT commands,last_command;
static LPARAM last_point;
static LRESULT CALLBACK parent(HWND w,UINT m,WPARAM a,LPARAM b)
{
    if(m==WM_SYSCOMMAND){++commands;last_command=(UINT)a;last_point=b;return 0;}
    return DefWindowProcW(w,m,a,b);
}
static void drain(void)
{MSG msg;while(PeekMessageW(&msg,NULL,0,0,PM_REMOVE)){TranslateMessage(&msg);DispatchMessageW(&msg);}}
static int row(HWND root,DWORD flags,BOOL styled)
{
    HWND box;RECT client,window,ink={1000,1000,-1,-1};HDC dc;HBITMAP bitmap,old;
    int x,y;COLORREF sentinel=RGB(197,31,143);unsigned changed=0;UINT before;
    box=CreateWindowW(L"SCROLLBAR",L"",WS_CHILD|WS_VISIBLE|SBS_SIZEBOX|flags,
        50,50,60,60,root,NULL,GetModuleHandleW(NULL),NULL);CHECK(box);
    CHECK(GetClientRect(box,&client) && GetWindowRect(box,&window));
    if(styled)CHECK(win16_style_control_install(box,NULL));
    dc=CreateCompatibleDC(NULL);CHECK(dc);bitmap=CreateBitmap(100,100,1,32,NULL);CHECK(bitmap);
    old=SelectObject(dc,bitmap);CHECK(old);
    {RECT whole={0,0,100,100};SetDCBrushColor(dc,sentinel);FillRect(dc,&whole,GetStockObject(DC_BRUSH));}
    SendMessageW(box,WM_PRINTCLIENT,(WPARAM)dc,PRF_CLIENT);
    for(y=0;y<100;++y)for(x=0;x<100;++x)if(GetPixel(dc,x,y)!=sentinel){
        ++changed;ink.left=min(ink.left,x);ink.top=min(ink.top,y);ink.right=max(ink.right,x+1);ink.bottom=max(ink.bottom,y+1);}
    printf("SIZEBOX pixels styled=%d flags=%lx client=%ld,%ld window=%ld,%ld ink=%ld,%ld,%ld,%ld count=%u center=%06lx scrollbar=%06lx face=%06lx\n",
        styled,flags,client.right,client.bottom,window.right-window.left,window.bottom-window.top,
        ink.left,ink.top,ink.right,ink.bottom,changed,GetPixel(dc,client.right/2,client.bottom/2),GetSysColor(COLOR_SCROLLBAR),GetSysColor(COLOR_BTNFACE));
    if(styled){UINT dpi=GetDpiForWindow(box);int left=window.right-window.left-GetSystemMetricsForDpi(SM_CXVSCROLL,dpi)+GetSystemMetricsForDpi(SM_CXBORDER,dpi);
        int top=window.bottom-window.top-GetSystemMetricsForDpi(SM_CYHSCROLL,dpi)+GetSystemMetricsForDpi(SM_CYBORDER,dpi);
        CHECK(ink.left==max(0,left) && ink.top==max(0,top) && ink.right==client.right && ink.bottom==client.bottom);
        for(y=0;y<100;++y)for(x=0;x<100;++x)
            CHECK(GetPixel(dc,x,y)==((x>=ink.left && x<ink.right && y>=ink.top && y<ink.bottom)?GetSysColor(COLOR_SCROLLBAR):sentinel));}
    SelectObject(dc,old);DeleteObject(bitmap);DeleteDC(dc);
    before=commands;SendMessageW(box,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(2,3));drain();
    printf("SIZEBOX click styled=%d count=%u command=%x point=%lx\n",styled,commands-before,last_command,(DWORD)last_point);
    CHECK(commands==before+1 && last_command==(SC_SIZE|8));
    before=commands;SendMessageW(box,WM_LBUTTONDBLCLK,MK_LBUTTON,MAKELPARAM(2,3));drain();
    CHECK(commands==before+1 && (last_command&0xfff0)==SC_MAXIMIZE);
    if(styled)CHECK(win16_style_control_remove(box));
    CHECK(DestroyWindow(box));return 0;
}
int main(int argc,char **argv)
{
    HDESK desktop;WNDCLASSW cls={0};HWND root;unsigned i;DWORD forms[]={0,SBS_SIZEBOXTOPLEFTALIGN,SBS_SIZEBOXBOTTOMRIGHTALIGN};
    BOOL styled=argc>1 && !strcmp(argv[1],"--styled");
    desktop=CreateDesktopW(L"StyleSizeBox",NULL,NULL,0,GENERIC_ALL,NULL);CHECK(desktop && SetThreadDesktop(desktop));
    cls.lpfnWndProc=parent;cls.lpszClassName=L"StyleSizeBoxOwner";cls.hInstance=GetModuleHandleW(NULL);CHECK(RegisterClassW(&cls));
    root=CreateWindowW(cls.lpszClassName,L"Size box oracle",WS_OVERLAPPEDWINDOW,0,0,400,300,NULL,NULL,cls.hInstance,NULL);CHECK(root);
    ShowWindow(root,SW_SHOWNOACTIVATE);UpdateWindow(root);
    for(i=0;i<3;++i)CHECK(!row(root,forms[i],styled));
    CHECK(DestroyWindow(root));puts("SIZEBOX PASS native command routes");return 0;
}
