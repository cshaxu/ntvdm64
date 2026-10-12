/* Independent USER/GDI fixture. --test <bitmap> captures only its owned HWNDs.
 * No input injection, desktop capture, product process or computer-use API. */
#include "win16-style/win16_style.h"
#include "win16-style/layout.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windowsx.h>

static unsigned checks;
#define CHECK(x) do {++checks;if(!(x)){fprintf(stderr,"FAIL %u line %d: %s error=%lu\n",checks,__LINE__,#x,GetLastError());exit(1);}}while(0)
static BOOL testing;
static int live;
static LRESULT CALLBACK window_proc(HWND w,UINT m,WPARAM a,LPARAM b)
{
    if(m==WM_PAINT){
        PAINTSTRUCT ps;RECT r;HDC dc=BeginPaint(w,&ps);
        GetClientRect(w,&r);FillRect(dc,&r,(HBRUSH)GetStockObject(WHITE_BRUSH));
        SetBkMode(dc,TRANSPARENT);SelectObject(dc,GetStockObject(DEFAULT_GUI_FONT));
        r.left+=18;r.top+=22;
        DrawTextW(dc,L"Independent Win32 application\n\nClient painting belongs to the application.\n"
            L"Try: drag, resize, arrows, Alt+Space, Alt+F4.\n"
            L"F2: change title    F3: change caption colour", -1,&r,DT_LEFT);
        EndPaint(w,&ps);return 0;
    }
    if(m==WM_KEYDOWN && a==VK_F2){SetWindowTextW(w,L"Changed title — Win3.1 frame");return 0;}
    if(m==WM_KEYDOWN && a==VK_F3){
        win16_style_palette p;win16_style_default_palette(&p);
        p.active=RGB(0,128,128);p.active_text=RGB(255,255,255);
        win16_style_set_palette(w,&p);return 0;
    }
    if(m==WM_DPICHANGED){
        RECT *r=(RECT *)b;
        SetWindowPos(w,NULL,r->left,r->top,r->right-r->left,r->bottom-r->top,
            SWP_NOZORDER|SWP_NOACTIVATE);return 0;
    }
    if(m==WM_DESTROY){if(--live==0 && !testing)PostQuitMessage(0);return 0;}
    return DefWindowProcW(w,m,a,b);
}

static void pump(void)
{
    MSG m;
    while(PeekMessageW(&m,NULL,0,0,PM_REMOVE)){TranslateMessage(&m);DispatchMessageW(&m);}
}

static void client_rect(HWND w,RECT *r)
{
    POINT p={0,0};GetClientRect(w,r);ClientToScreen(w,&p);OffsetRect(r,p.x,p.y);
}

static HWND make_window(DWORD style,DWORD ex_style,const WCHAR *title,int x,int y,BOOL styled)
{
    HWND w=CreateWindowExW(ex_style,L"Win16StyleTrial",title,style,x,y,430,240,NULL,NULL,
        GetModuleHandleW(NULL),NULL);
    RECT before,after;
    CHECK(w!=NULL);++live;
    client_rect(w,&before);
    if(styled){CHECK(win16_style_install(w,NULL));client_rect(w,&after);
        CHECK(EqualRect(&before,&after));}
    ShowWindow(w,SW_SHOWNOACTIVATE);UpdateWindow(w);return w;
}

static void capture(HWND *windows,const WCHAR *path,BOOL pressed)
{
    BITMAPINFO bi={0};BITMAPFILEHEADER file={0};
    HDC dc=CreateCompatibleDC(NULL),part=CreateCompatibleDC(NULL);
    HBITMAP bitmap,old;void *pixels;HANDLE out;DWORD wrote;
    int i,width=1000,height=650;
    bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bi.bmiHeader.biWidth=width;
    bi.bmiHeader.biHeight=-height;bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;
    bitmap=CreateDIBSection(dc,&bi,DIB_RGB_COLORS,&pixels,NULL,0);CHECK(bitmap!=NULL);
    old=(HBITMAP)SelectObject(dc,bitmap);
    {RECT r={0,0,1000,650};HBRUSH brush=CreateSolidBrush(RGB(160,160,164));
    FillRect(dc,&r,brush);DeleteObject(brush);}
    for(i=0;i<4;++i){
        RECT r;HBITMAP shot,prior;int w,h;
        GetWindowRect(windows[i],&r);w=r.right-r.left;h=r.bottom-r.top;
        shot=CreateCompatibleBitmap(dc,w,h);CHECK(shot!=NULL);
        prior=(HBITMAP)SelectObject(part,shot);
        CHECK(PrintWindow(windows[i],part,0));
        if(i==0){
            win16_style_layout l;RECT b;UINT dpi=GetDpiForWindow(windows[i]);
            int bright=max(1,MulDiv(1,(int)dpi,96));
            int shade=max(1,MulDiv(2,(int)dpi,96));
            CHECK(win16_style_measure(w,h,dpi,
                (DWORD)GetWindowLongPtr(windows[i],GWL_STYLE),
                (DWORD)GetWindowLongPtr(windows[i],GWL_EXSTYLE),FALSE,&l));
            b=l.maximize;
            CHECK(GetPixel(part,b.left+l.unit,b.top+l.unit+bright+1)==
                (pressed?RGB(128,128,128):RGB(255,255,255)));
            CHECK(GetPixel(part,(b.left+b.right)/2,b.bottom-l.unit-shade)==
                (pressed?RGB(192,192,192):RGB(128,128,128)));
        }
        if(i==1){
            win16_style_layout l;RECT b;UINT dpi=GetDpiForWindow(windows[i]);
            int bright=max(1,MulDiv(1,(int)dpi,96));
            int shade=max(1,MulDiv(2,(int)dpi,96));
            CHECK(win16_style_measure(w,h,dpi,
                (DWORD)GetWindowLongPtr(windows[i],GWL_STYLE),
                (DWORD)GetWindowLongPtr(windows[i],GWL_EXSTYLE),FALSE,&l));
            CHECK(GetPixel(part,l.unit,l.client.top+8)==RGB(192,192,192));
            /* Verify actual painted layers, not just geometry. These are
             * the reference-derived flat control box and asymmetric button
             * contracts, not a claim of whole-window pixel equivalence. */
            b=l.system;
            CHECK(GetPixel(part,b.left+l.unit,b.top+l.unit)==RGB(192,192,192));
            b=l.minimize;
            CHECK(GetPixel(part,b.left+l.unit,b.top+l.unit+bright+1)==RGB(255,255,255));
            CHECK(GetPixel(part,b.left+l.unit+bright,b.top+l.unit+bright)==RGB(192,192,192));
            CHECK(GetPixel(part,(b.left+b.right)/2,b.bottom-l.unit-shade)==RGB(128,128,128));
            CHECK(GetPixel(part,(b.left+b.right)/2,b.bottom-l.unit-shade-1)==RGB(192,192,192));
        }
        if(i==2){
            win16_style_layout l;
            CHECK(win16_style_measure(w,h,GetDpiForWindow(windows[i]),
                (DWORD)GetWindowLongPtr(windows[i],GWL_STYLE),
                (DWORD)GetWindowLongPtr(windows[i],GWL_EXSTYLE),FALSE,&l));
            CHECK(l.dialog_frame);
            CHECK(GetPixel(part,0,l.client.top+8)==RGB(0,0,0));
            CHECK(GetPixel(part,l.unit,l.client.top+8)==RGB(164,200,240));
            CHECK(GetPixel(part,l.border-l.unit,l.client.top+8)==RGB(255,255,255));
        }
        CHECK(BitBlt(dc,30+(i%2)*490,30+(i/2)*310,w,h,part,0,0,SRCCOPY));
        SelectObject(part,prior);DeleteObject(shot);
    }
    GdiFlush();
    file.bfType=0x4d42;file.bfOffBits=sizeof(file)+sizeof(BITMAPINFOHEADER);
    file.bfSize=file.bfOffBits+(DWORD)(width*height*4);
    out=CreateFileW(path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    CHECK(out!=INVALID_HANDLE_VALUE);
    CHECK(WriteFile(out,&file,sizeof(file),&wrote,NULL)&&wrote==sizeof(file));
    CHECK(WriteFile(out,&bi.bmiHeader,sizeof(bi.bmiHeader),&wrote,NULL));
    CHECK(WriteFile(out,pixels,(DWORD)(width*height*4),&wrote,NULL));
    CloseHandle(out);SelectObject(dc,old);DeleteObject(bitmap);DeleteDC(part);DeleteDC(dc);
}

static void layout_tests(void)
{
    UINT dpi;DWORD kinds[]={WS_OVERLAPPEDWINDOW,WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
        WS_CAPTION|WS_SYSMENU};
    size_t i;
    for(dpi=96;dpi<=192;dpi+=48)for(i=0;i<3;++i){
        win16_style_layout l;POINT p;
        CHECK(win16_style_measure(600,400,dpi,kinds[i],i==2?WS_EX_DLGMODALFRAME:0,FALSE,&l));
        CHECK(l.dialog_frame==(i==2));
        p.x=l.system.left+2;p.y=l.system.top+2;CHECK(win16_style_hit(&l,p)==HTSYSMENU);
        p.x=l.title.left+2;CHECK(win16_style_hit(&l,p)==HTCAPTION);
        p.x=l.client.left+20;p.y=l.client.top+20;CHECK(win16_style_hit(&l,p)==HTCLIENT);
        CHECK(IsRectEmpty(&l.maximize)==(i!=0));CHECK(IsRectEmpty(&l.minimize)==(i==2));
        if(i==0){p.x=0;p.y=0;CHECK(win16_style_hit(&l,p)==HTTOPLEFT);}
    }
    CHECK(!win16_style_measure(0,400,96,WS_OVERLAPPEDWINDOW,0,FALSE,NULL));
}

static void font_probe(void)
{
    HDC dc=CreateCompatibleDC(NULL);int height;
    for(height=16;height<=26;height+=2){
        HFONT f=CreateFontW(height,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,DEFAULT_PITCH,L"System");
        HGDIOBJ old=SelectObject(dc,f);TEXTMETRICW tm;WCHAR name[80];SIZE size;
        GetTextMetricsW(dc,&tm);GetTextFaceW(dc,80,name);
        GetTextExtentPoint32W(dc,L"Control Panel",13,&size);
        printf("FONT request=%d cell=%ld leading=%ld weight=%ld title=%ldx%ld face=%ls\n",
            height,tm.tmHeight,tm.tmInternalLeading,tm.tmWeight,size.cx,size.cy,name);
        SelectObject(dc,old);DeleteObject(f);
    }
    DeleteDC(dc);
}

static void button_test(HWND w,int hit,UINT command)
{
    win16_style_layout l;RECT outer,button;POINT p;LPARAM screen,client;
    CHECK(GetWindowRect(w,&outer));
    CHECK(win16_style_measure(outer.right-outer.left,outer.bottom-outer.top,
        GetDpiForWindow(w),(DWORD)GetWindowLongPtr(w,GWL_STYLE),
        (DWORD)GetWindowLongPtr(w,GWL_EXSTYLE),FALSE,&l));
    button=hit==HTMAXBUTTON?l.maximize:l.minimize;
    p.x=outer.left+(button.left+button.right)/2;
    p.y=outer.top+(button.top+button.bottom)/2;
    screen=MAKELPARAM(p.x,p.y);
    CHECK(SendMessageW(w,WM_NCHITTEST,0,screen)==hit);
    SendMessageW(w,WM_NCLBUTTONDOWN,hit,screen);CHECK(GetCapture()==w);
    SendMessageW(w,WM_CANCELMODE,0,0);CHECK(GetCapture()!=w);
    CHECK(!IsZoomed(w)&&!IsIconic(w));
    SendMessageW(w,WM_NCLBUTTONDOWN,hit,screen);
    CHECK(ScreenToClient(w,&p));client=MAKELPARAM(p.x,p.y);
    SendMessageW(w,WM_LBUTTONUP,0,client);pump();
    CHECK(command==SC_MAXIMIZE?IsZoomed(w):IsIconic(w));
    SendMessageW(w,WM_SYSCOMMAND,SC_RESTORE,0);pump();
    CHECK(!IsZoomed(w)&&!IsIconic(w));
}

int wmain(int argc,WCHAR **argv)
{
    WNDCLASSW cls={0};HWND w[4];MSG msg;int i;HDESK desktop=NULL;
    DWORD initial_gdi;
    testing=argc==3 && !wcscmp(argv[1],L"--test");
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    if(testing){
        WCHAR name[80];swprintf_s(name,80,L"Win16StyleTest-%lu",GetCurrentProcessId());
        desktop=CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL);
        CHECK(desktop!=NULL);CHECK(SetThreadDesktop(desktop));
    }
    cls.lpfnWndProc=window_proc;cls.hInstance=GetModuleHandleW(NULL);
    cls.hCursor=LoadCursorW(NULL,IDC_ARROW);cls.lpszClassName=L"Win16StyleTrial";
    cls.style=CS_DBLCLKS;CHECK(RegisterClassW(&cls)!=0);
    layout_tests();
    font_probe();
    w[0]=make_window(WS_OVERLAPPEDWINDOW,0,L"Program Manager",60,60,TRUE);
    w[1]=make_window(WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,0,L"Control Panel",550,60,TRUE);
    w[2]=make_window(WS_CAPTION|WS_SYSMENU,WS_EX_DLGMODALFRAME,L"Color",60,380,TRUE);
    w[3]=make_window(WS_OVERLAPPEDWINDOW,0,L"Native Windows — comparison",550,380,FALSE);
    pump();
    if(!testing){
        SetForegroundWindow(w[0]);
        while(GetMessageW(&msg,NULL,0,0)>0){TranslateMessage(&msg);DispatchMessageW(&msg);}
        return 0;
    }
    CHECK(!win16_style_install(w[0],NULL)&&GetLastError()==ERROR_ALREADY_EXISTS);
    CHECK(!win16_style_set_palette(w[3],NULL));
    SendMessageW(w[0],WM_NCACTIVATE,FALSE,0);SendMessageW(w[1],WM_NCACTIVATE,TRUE,0);
    SendMessageW(w[2],WM_NCACTIVATE,TRUE,0);
    capture(w,argv[2],FALSE);
    {
        win16_style_layout l;RECT r;POINT p;WCHAR path[MAX_PATH];
        CHECK(GetWindowRect(w[0],&r));
        CHECK(win16_style_measure(r.right-r.left,r.bottom-r.top,GetDpiForWindow(w[0]),
            (DWORD)GetWindowLongPtr(w[0],GWL_STYLE),
            (DWORD)GetWindowLongPtr(w[0],GWL_EXSTYLE),FALSE,&l));
        p.x=r.left+(l.maximize.left+l.maximize.right)/2;
        p.y=r.top+(l.maximize.top+l.maximize.bottom)/2;
        SendMessageW(w[0],WM_NCLBUTTONDOWN,HTMAXBUTTON,MAKELPARAM(p.x,p.y));
        CHECK(GetCapture()==w[0]);
        CHECK(swprintf_s(path,MAX_PATH,L"%ls-pressed.bmp",argv[2])>0);
        capture(w,path,TRUE);
        SendMessageW(w[0],WM_CANCELMODE,0,0);
        CHECK(GetCapture()!=w[0]);CHECK(!IsZoomed(w[0]));
        /* Cancellation must restore the original pixels, not just the
         * capture/command state. Preserve this second observation too. */
        CHECK(swprintf_s(path,MAX_PATH,L"%ls-canceled.bmp",argv[2])>0);
        capture(w,path,FALSE);
    }
    button_test(w[0],HTMAXBUTTON,SC_MAXIMIZE);
    button_test(w[1],HTMINBUTTON,SC_MINIMIZE);
    {HMENU menu=CreateMenu();CHECK(menu!=NULL);CHECK(SetMenu(w[3],menu));
        CHECK(!win16_style_install(w[3],NULL)&&GetLastError()==ERROR_NOT_SUPPORTED);
        CHECK(SetMenu(w[3],NULL));CHECK(DestroyMenu(menu));}
    CHECK(SetWindowTextW(w[0],L"Changed title"));
    {WCHAR title[64];CHECK(GetWindowTextW(w[0],title,64)>0);
        CHECK(!wcscmp(title,L"Changed title"));}
    {win16_style_palette p;win16_style_default_palette(&p);p.active=RGB(0,128,128);
        CHECK(win16_style_set_palette(w[0],&p));}
    SendMessageW(w[0],WM_SYSCOMMAND,SC_MAXIMIZE,0);pump();CHECK(IsZoomed(w[0]));
    SendMessageW(w[0],WM_SYSCOMMAND,SC_RESTORE,0);pump();CHECK(!IsZoomed(w[0]));
    SendMessageW(w[1],WM_SYSCOMMAND,SC_MINIMIZE,0);pump();CHECK(IsIconic(w[1]));
    SendMessageW(w[1],WM_SYSCOMMAND,SC_RESTORE,0);pump();CHECK(!IsIconic(w[1]));
    {RECT before,after;client_rect(w[0],&before);CHECK(win16_style_remove(w[0]));
        client_rect(w[0],&after);CHECK(EqualRect(&before,&after));
        CHECK(win16_style_install(w[0],NULL));}
    pump();GdiFlush();initial_gdi=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);
    for(i=0;i<80;++i){
        HWND temp=make_window(WS_OVERLAPPEDWINDOW,0,L"Lifetime",0,0,TRUE);
        CHECK(win16_style_remove(temp));CHECK(win16_style_install(temp,NULL));
        CHECK(DestroyWindow(temp));
        pump();GdiFlush();
        printf("GDI cycle=%d baseline=%lu current=%lu\n",i,initial_gdi,
            GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS));
        /* Retain whole-process counts as diagnostics; they do not isolate
         * this library's allocations from USER's own drawing resources. */
        if(i==39)initial_gdi=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);
    }
    /* Measure attachment lifetime separately on one surviving HWND, without
     * conflating its resources with repeated creation of native windows. */
    for(i=0;i<120;++i){
        CHECK(win16_style_remove(w[0]));CHECK(win16_style_install(w[0],NULL));
        pump();GdiFlush();
        if(i==19)initial_gdi=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);
    }
    printf("GDI attachment baseline=%lu final=%lu\n",initial_gdi,
        GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS));
    CHECK(GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS)<=initial_gdi);
    for(i=0;i<4;++i){SendMessageW(w[i],WM_CLOSE,0,0);CHECK(!IsWindow(w[i]));}
    /* Desktop handle closes on process exit; never switch user's desktop. */
    printf("PASS %u checks: layout, real HWND geometry, title, palette, commands, cleanup\n",checks);
    return 0;
}
