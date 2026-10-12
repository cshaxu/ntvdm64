/* Independent USER/GDI fixture. --test <bitmap> captures only its owned HWNDs.
 * No input injection, desktop capture, product process or computer-use API. */
#include "win16-style/win16_style.h"
#include "win16-style/layout.h"
#include "win16-style/frame_buttons.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windowsx.h>

static unsigned checks;
#define CHECK(x) do {++checks;if(!(x)){fprintf(stderr,"FAIL %u line %d: %s error=%lu\n",checks,__LINE__,#x,GetLastError());exit(1);}}while(0)
static BOOL testing,menu_desktop_test;
static const WCHAR *capture_path;
static int live;
static HWND menu_probe;
static unsigned menu_entries,close_commands,menu_popups,key_menu_entries;
static void menu_pixels(HWND w,BOOL selected);
static HWND cursor_probe;
static unsigned client_cursor_calls;
static unsigned native_ncactivate_calls;
void win16_style_gallery_run(void);
void win16_style_gallery_test(const WCHAR *path);
static LRESULT CALLBACK window_proc(HWND w,UINT m,WPARAM a,LPARAM b)
{
    if(w==cursor_probe && m==WM_NCACTIVATE)++native_ncactivate_calls;
    if(w==cursor_probe && m==WM_SETCURSOR && LOWORD(b)==HTCLIENT){
        ++client_cursor_calls;SetCursor(LoadCursorW(NULL,IDC_IBEAM));return TRUE;
    }
    if(w==menu_probe){
        if(m==WM_INITMENUPOPUP){
            if(HIWORD(b))++menu_popups;
            if(menu_desktop_test)printf("MENU init system=%u matches=%d\n",HIWORD(b),(HMENU)a==GetSystemMenu(w,FALSE));
        }
        if(m==WM_SYSCOMMAND && (a&0xfff0)==SC_MOUSEMENU)++menu_entries;
        if(m==WM_SYSCOMMAND && (a&0xfff0)==SC_KEYMENU){
            ++key_menu_entries;
            /* A private non-input desktop cannot exercise keyboard menu
             * tracking. --menu-test separately requires the real popup. */
            if(!menu_desktop_test)return 0;
        }
        if(m==WM_SYSCOMMAND && (a&0xfff0)==SC_CLOSE)++close_commands;
        if(m==WM_ENTERIDLE && a==MSGF_MENU){
            menu_pixels(w,TRUE);
            EndMenu();return 0;
        }
    }
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

static void oem_pixels(HDC dc,RECT b,int u,int index)
{
    static const COLORREF palette[]={RGB(0,0,0),RGB(255,255,255),RGB(128,128,128),RGB(192,192,192)};
    int x,y,width=index>=6?18:19,dw,dh=b.bottom-b.top;
    dw=b.right-b.left-(index>=6?u:0);
    for(y=0;y<18;++y)for(x=0;x<width;++x){
        int px=b.left+((2*x+1)*dw)/(2*width);
        int py=b.top+((2*y+1)*dh)/36;
        int role=(index>=6?system_menu_bitmap[y][x]:frame_buttons[index][y][x])-'0';
        COLORREF actual=GetPixel(dc,px,py),expected=palette[index==7?role^1:role];
        if(actual!=expected){
            BITMAPINFO bi={0};BITMAPFILEHEADER fh={0};BITMAP bm;DWORD written;HANDLE out;void *data;WCHAR path[MAX_PATH];
            HBITMAP image=(HBITMAP)GetCurrentObject(dc,OBJ_BITMAP);
            GetObjectW(image,sizeof(bm),&bm);
            bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bi.bmiHeader.biWidth=bm.bmWidth;
            bi.bmiHeader.biHeight=bm.bmHeight;bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;
            data=calloc((size_t)bm.bmWidth*bm.bmHeight,4);
            if(data && GetDIBits(dc,image,0,(UINT)bm.bmHeight,data,&bi,DIB_RGB_COLORS)){
                swprintf_s(path,MAX_PATH,L"%ls-failure.bmp",capture_path);
                out=CreateFileW(path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
                if(out!=INVALID_HANDLE_VALUE){fh.bfType=0x4d42;fh.bfOffBits=sizeof(fh)+sizeof(bi.bmiHeader);fh.bfSize=fh.bfOffBits+(DWORD)(bm.bmWidth*bm.bmHeight*4);
                    WriteFile(out,&fh,sizeof(fh),&written,NULL);WriteFile(out,&bi.bmiHeader,sizeof(bi.bmiHeader),&written,NULL);WriteFile(out,data,fh.bfSize-fh.bfOffBits,&written,NULL);CloseHandle(out);}
            }
            free(data);
            fprintf(stderr,"OEM index=%d source=%d,%d dest=%d,%d size=%d,%d actual=%06lx expected=%06lx\n",index,x,y,px,py,dw,dh,actual,expected);
        }
        CHECK(actual==expected);
    }
}

static void menu_pixels(HWND w,BOOL selected)
{
    RECT r;win16_style_layout l;HDC dc;
    CHECK(GetWindowRect(w,&r));
    CHECK(win16_style_measure(r.right-r.left,r.bottom-r.top,GetDpiForWindow(w),
        (DWORD)GetWindowLongPtr(w,GWL_STYLE),(DWORD)GetWindowLongPtr(w,GWL_EXSTYLE),FALSE,&l));
    dc=GetWindowDC(w);CHECK(dc!=NULL);
    oem_pixels(dc,l.system,l.unit,selected?7:6);
    ReleaseDC(w,dc);
}

static void sizing_strokes(HDC dc,const win16_style_layout *l,UINT dpi)
{
    int corner=l->border+MulDiv(19,(int)dpi,96)-l->unit,i;
    COLORREF expected=l->sizeable?RGB(0,0,0):RGB(192,192,192);
    for(i=0;i<2;++i){
        int x=i?l->outer.right-corner-l->unit:corner;
        int y=i?l->outer.bottom-l->caption.bottom-l->unit:l->caption.bottom;
        CHECK(GetPixel(dc,x,l->unit)==expected);
        CHECK(GetPixel(dc,x,l->outer.bottom-l->border+l->unit)==expected);
        CHECK(GetPixel(dc,l->unit,y)==expected);
        CHECK(GetPixel(dc,l->outer.right-l->border+l->unit,y)==expected);
    }
    if(l->sizeable){
        int raster_width=0,scan_y=(l->maximize.top+l->maximize.bottom)/2,j,k;
        RECT strokes[8];
        /* Independent oracle: read the actual stretched OEM black separator,
         * not another copy of the layout's rounding expression. */
        while(raster_width<l->maximize.right-l->maximize.left &&
            GetPixel(dc,l->maximize.left+raster_width,scan_y)==RGB(0,0,0))++raster_width;
        CHECK(raster_width>0);CHECK(l->unit==raster_width);
        CHECK(corner==l->system.right-l->unit);
        CHECK(l->outer.right-corner-l->unit==l->maximize.left);
        for(i=0;i<2;++i){
            int x=i?l->maximize.left:l->system.right-raster_width;
            int y=i?l->outer.bottom-l->caption.bottom-raster_width:l->caption.bottom;
            SetRect(&strokes[i*4],x,raster_width,x+raster_width,l->border-raster_width);
            strokes[i*4+1]=strokes[i*4];OffsetRect(&strokes[i*4+1],0,l->outer.bottom-l->border);
            SetRect(&strokes[i*4+2],raster_width,y,l->border-raster_width,y+raster_width);
            strokes[i*4+3]=strokes[i*4+2];OffsetRect(&strokes[i*4+3],l->outer.right-l->border,0);
        }
        for(i=0;i<8;++i){
            RECT r=strokes[i];BOOL vertical=i%4<2;
            CHECK(!IsRectEmpty(&r));
            printf("SIZING stroke=%d rect=%ld,%ld,%ld,%ld OEM-width=%d\n",
                i,r.left,r.top,r.right,r.bottom,raster_width);
            for(j=r.top;j<r.bottom;++j)for(k=r.left;k<r.right;++k)
                CHECK(GetPixel(dc,k,j)==RGB(0,0,0));
            /* Grey neighbours prove exact extent, not merely that one sampled
             * point happens to lie somewhere on a longer/wider black stroke. */
            if(vertical){
                for(j=r.top;j<r.bottom;++j){
                    CHECK(GetPixel(dc,r.left-1,j)==RGB(192,192,192));
                    CHECK(GetPixel(dc,r.right,j)==RGB(192,192,192));
                }
            }else{
                for(k=r.left;k<r.right;++k){
                    CHECK(GetPixel(dc,k,r.top-1)==RGB(192,192,192));
                    CHECK(GetPixel(dc,k,r.bottom)==RGB(192,192,192));
                }
            }
        }
    }
}

static void raster_scale_tests(void)
{
    HDC dc=CreateCompatibleDC(NULL);BITMAPINFO target={0},source={0};
    HBITMAP bitmap,old;void *pixels;DWORD row[19];int i;UINT dpi;
    target.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);target.bmiHeader.biWidth=152;
    target.bmiHeader.biHeight=-1;target.bmiHeader.biPlanes=1;target.bmiHeader.biBitCount=32;
    bitmap=CreateDIBSection(dc,&target,DIB_RGB_COLORS,&pixels,NULL,0);
    CHECK(dc!=NULL && bitmap!=NULL);old=(HBITMAP)SelectObject(dc,bitmap);
    source=target;source.bmiHeader.biWidth=19;
    for(i=0;i<19;++i)row[i]=i?0xffffff:0;
    SetStretchBltMode(dc,COLORONCOLOR);
    for(dpi=96;dpi<=768;++dpi){
        win16_style_layout l;int width=MulDiv(19,(int)dpi,96),observed=0;
        CHECK(win16_style_measure(2000,2000,dpi,WS_OVERLAPPEDWINDOW,0,FALSE,&l));
        CHECK(StretchDIBits(dc,0,0,width,1,0,0,19,1,row,&source,DIB_RGB_COLORS,SRCCOPY)!=GDI_ERROR);
        while(observed<width && GetPixel(dc,observed,0)==RGB(0,0,0))++observed;
        if(l.unit!=observed)fprintf(stderr,"RASTER dpi=%u width=%d expected=%d observed=%d\n",dpi,width,l.unit,observed);
        CHECK(l.unit==observed);
    }
    printf("RASTER DPI=96..768 (673 scales) matches actual OEM separator stretch\n");
    SelectObject(dc,old);DeleteObject(bitmap);DeleteDC(dc);
}

static void title_clip_test(void)
{
    WCHAR title[121];HWND w;RECT r;win16_style_layout l;int i,x,y,width,height,logical_width;
    HDC actual,base=CreateCompatibleDC(NULL),expected=CreateCompatibleDC(NULL);
    HBITMAP a,b,old_a,old_b;HFONT font;RECT logical;
    for(i=0;i<120;++i)title[i]=L'W';title[120]=0;
    w=make_window(WS_OVERLAPPEDWINDOW,0,title,20,20,TRUE);
    SendMessageW(w,WM_NCACTIVATE,FALSE,0);
    CHECK(GetWindowRect(w,&r));
    CHECK(win16_style_measure(r.right-r.left,r.bottom-r.top,GetDpiForWindow(w),WS_OVERLAPPEDWINDOW,0,FALSE,&l));
    width=l.title.right-l.title.left;height=l.title.bottom-l.title.top;
    logical_width=MulDiv(width,96,(int)GetDpiForWindow(w));
    actual=GetWindowDC(w);CHECK(actual && base && expected);
    a=CreateCompatibleBitmap(actual,logical_width,20);b=CreateCompatibleBitmap(actual,width,height);
    CHECK(a && b);old_a=SelectObject(base,a);old_b=SelectObject(expected,b);
    SetRect(&logical,0,0,logical_width,20);FillRect(base,&logical,(HBRUSH)GetStockObject(WHITE_BRUSH));
    font=CreateFontW(16,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,DEFAULT_PITCH,L"System");
    CHECK(font);SelectObject(base,font);SetBkMode(base,TRANSPARENT);SetTextColor(base,RGB(0,0,0));
    /* Oracle: a deliberately overflowing stream of original W glyphs starts
     * one logical pixel inside the left edge. No fitting/ellipsis algorithm.
     * The bitmap boundary clips its tail. Compare the whole title, including
     * the right edge where a modern ellipsis used to replace original glyphs. */
    CHECK(TextOutW(base,1,2,title,120));SetStretchBltMode(expected,COLORONCOLOR);
    {int scaled=MulDiv(20,(int)GetDpiForWindow(w),96);
    CHECK(StretchBlt(expected,0,(height-scaled)/2,width,scaled,base,0,0,logical_width,20,SRCCOPY));}
    for(y=0;y<height;++y)for(x=0;x<width;++x)
        CHECK(GetPixel(actual,l.title.left+x,l.title.top+y)==GetPixel(expected,x,y));
    printf("TITLE overflowing original glyph stream clipped without ellipsis: %dx%d pixels\n",width,height);
    ReleaseDC(w,actual);SelectObject(base,old_a);SelectObject(expected,old_b);
    DeleteDC(base);DeleteDC(expected);DeleteObject(a);DeleteObject(b);DeleteObject(font);
    CHECK(DestroyWindow(w));
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
            CHECK(win16_style_measure(w,h,dpi,
                (DWORD)GetWindowLongPtr(windows[i],GWL_STYLE),
                (DWORD)GetWindowLongPtr(windows[i],GWL_EXSTYLE),FALSE,&l));
            b=l.maximize;
            oem_pixels(part,b,l.unit,pressed?4:1);
            sizing_strokes(part,&l,dpi);
        }
        if(i==1){
            win16_style_layout l;RECT b;UINT dpi=GetDpiForWindow(windows[i]);
            CHECK(win16_style_measure(w,h,dpi,
                (DWORD)GetWindowLongPtr(windows[i],GWL_STYLE),
                (DWORD)GetWindowLongPtr(windows[i],GWL_EXSTYLE),FALSE,&l));
            CHECK(GetPixel(part,l.unit,l.client.top+8)==RGB(192,192,192));
            /* Verify actual painted layers, not just geometry. These are
             * the reference-derived flat control box and asymmetric button
             * contracts, not a claim of whole-window pixel equivalence. */
            b=l.system;
            CHECK(GetPixel(part,b.left+l.unit,b.top+l.unit)==RGB(192,192,192));
            oem_pixels(part,b,l.unit,6);
            b=l.minimize;
            oem_pixels(part,b,l.unit,0);
            sizing_strokes(part,&l,dpi);
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

static void oem_window_tests(HWND w)
{
    int zoom,pressed,hit;
    for(zoom=0;zoom<2;++zoom){
        RECT r;win16_style_layout l;HDC dc=CreateCompatibleDC(NULL);
        HBITMAP bitmap,old;BITMAPINFO bi={0};void *pixels;
        if(zoom){SendMessageW(w,WM_SYSCOMMAND,SC_MAXIMIZE,0);pump();CHECK(IsZoomed(w));}
        CHECK(GetWindowRect(w,&r));
        fprintf(stderr,"OEM window zoom=%d rect=%ld,%ld,%ld,%ld dpi=%u\n",zoom,r.left,r.top,r.right,r.bottom,GetDpiForWindow(w));
        {MONITORINFO mi={sizeof(mi)};RECT work={0};MINMAXINFO mm={0};
            HMONITOR monitor=MonitorFromWindow(w,MONITOR_DEFAULTTONEAREST);
            BOOL ok=GetMonitorInfoW(monitor,&mi),spi=SystemParametersInfoW(SPI_GETWORKAREA,0,&work,0);
            SendMessageW(w,WM_GETMINMAXINFO,0,(LPARAM)&mm);
            fprintf(stderr,"GEOMETRY monitor=%p valid=%d work=%ld,%ld,%ld,%ld spi=%d primary=%ld,%ld,%ld,%ld maxpos=%ld,%ld maxsize=%ld,%ld\n",
                monitor,ok,mi.rcWork.left,mi.rcWork.top,mi.rcWork.right,mi.rcWork.bottom,spi,work.left,work.top,work.right,work.bottom,
                mm.ptMaxPosition.x,mm.ptMaxPosition.y,mm.ptMaxSize.x,mm.ptMaxSize.y);
            CHECK(ok);
        }
        CHECK(win16_style_measure(r.right-r.left,r.bottom-r.top,GetDpiForWindow(w),
            (DWORD)GetWindowLongPtr(w,GWL_STYLE),(DWORD)GetWindowLongPtr(w,GWL_EXSTYLE),IsZoomed(w),&l));
        if(zoom){
            MONITORINFO mi={sizeof(mi)};RECT visible,actual,expected;POINT p={0,0};int dx,dy;
            CHECK(GetMonitorInfoW(MonitorFromWindow(w,MONITOR_DEFAULTTONEAREST),&mi));
            CHECK(IntersectRect(&visible,&r,&mi.rcWork));
            CHECK(win16_style_measure(visible.right-visible.left,visible.bottom-visible.top,GetDpiForWindow(w),
                (DWORD)GetWindowLongPtr(w,GWL_STYLE),(DWORD)GetWindowLongPtr(w,GWL_EXSTYLE),TRUE,&l));
            expected=l.client;OffsetRect(&expected,visible.left,visible.top);
            CHECK(GetClientRect(w,&actual));CHECK(ClientToScreen(w,&p));OffsetRect(&actual,p.x,p.y);
            CHECK(EqualRect(&actual,&expected));
            dx=visible.left-r.left;dy=visible.top-r.top;
            OffsetRect(&l.minimize,dx,dy);OffsetRect(&l.maximize,dx,dy);
        }
        bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bi.bmiHeader.biWidth=r.right-r.left;
        bi.bmiHeader.biHeight=r.top-r.bottom;bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;
        bitmap=CreateDIBSection(dc,&bi,DIB_RGB_COLORS,&pixels,NULL,0);
        CHECK(bitmap!=NULL);old=(HBITMAP)SelectObject(dc,bitmap);
        for(hit=HTMINBUTTON;hit<=HTMAXBUTTON;++hit)for(pressed=0;pressed<2;++pressed){
            RECT b=hit==HTMINBUTTON?l.minimize:l.maximize;
            if(pressed){SendMessageW(w,WM_NCLBUTTONDOWN,hit,MAKELPARAM(r.left+b.left+3,r.top+b.top+3));CHECK(GetCapture()==w);}
            CHECK(PrintWindow(w,dc,0));
            oem_pixels(dc,b,l.unit,(hit==HTMINBUTTON?0:zoom?2:1)+(pressed?3:0));
            if(zoom && hit==HTMAXBUTTON && !pressed){
                WCHAR path[MAX_PATH];BITMAPFILEHEADER fh={0};DWORD written;HANDLE out;
                GdiFlush();CHECK(swprintf_s(path,MAX_PATH,L"%ls-maximized.bmp",capture_path)>0);
                fh.bfType=0x4d42;fh.bfOffBits=sizeof(fh)+sizeof(bi.bmiHeader);
                fh.bfSize=fh.bfOffBits+(DWORD)((r.right-r.left)*(r.bottom-r.top)*4);
                out=CreateFileW(path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);CHECK(out!=INVALID_HANDLE_VALUE);
                CHECK(WriteFile(out,&fh,sizeof(fh),&written,NULL)&&written==sizeof(fh));
                CHECK(WriteFile(out,&bi.bmiHeader,sizeof(bi.bmiHeader),&written,NULL)&&written==sizeof(bi.bmiHeader));
                CHECK(WriteFile(out,pixels,fh.bfSize-fh.bfOffBits,&written,NULL)&&written==fh.bfSize-fh.bfOffBits);
                CloseHandle(out);
            }
            SendMessageW(w,WM_CANCELMODE,0,0);CHECK(GetCapture()!=w);
        }
        SelectObject(dc,old);DeleteObject(bitmap);DeleteDC(dc);
    }
    /* Click the actual maximized window's restore slot, not only SC_RESTORE. */
    {RECT r;win16_style_layout l;POINT p;
        CHECK(GetWindowRect(w,&r));
        {MONITORINFO mi={sizeof(mi)};RECT visible;
        CHECK(GetMonitorInfoW(MonitorFromWindow(w,MONITOR_DEFAULTTONEAREST),&mi));CHECK(IntersectRect(&visible,&r,&mi.rcWork));
        CHECK(win16_style_measure(visible.right-visible.left,visible.bottom-visible.top,GetDpiForWindow(w),
            (DWORD)GetWindowLongPtr(w,GWL_STYLE),(DWORD)GetWindowLongPtr(w,GWL_EXSTYLE),TRUE,&l));
        OffsetRect(&l.maximize,visible.left-r.left,visible.top-r.top);}
        p.x=r.left+(l.maximize.left+l.maximize.right)/2;p.y=r.top+(l.maximize.top+l.maximize.bottom)/2;
        CHECK(SendMessageW(w,WM_NCHITTEST,0,MAKELPARAM(p.x,p.y))==HTMAXBUTTON);
        SendMessageW(w,WM_NCLBUTTONDOWN,HTMAXBUTTON,MAKELPARAM(p.x,p.y));
        CHECK(ScreenToClient(w,&p));SendMessageW(w,WM_LBUTTONUP,0,MAKELPARAM(p.x,p.y));pump();
        CHECK(!IsZoomed(w));CHECK(GetCapture()!=w);
    }
}

static void layout_tests(void)
{
    UINT dpi;DWORD kinds[]={WS_OVERLAPPEDWINDOW,WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
        WS_CAPTION|WS_SYSMENU};
    size_t i;
    /* Original USER's inclusive corner comparisons at VGA 96 DPI.
     * Test both sides of each of the eight dividing strokes. */
    {static const struct {LONG x,y;int hit;} edges[]={
        {22,0,HTTOPLEFT},{23,0,HTTOP},{577,0,HTTOP},{578,0,HTTOPRIGHT},
        {22,399,HTBOTTOMLEFT},{23,399,HTBOTTOM},{577,399,HTBOTTOM},{578,399,HTBOTTOMRIGHT},
        {0,22,HTTOPLEFT},{0,23,HTLEFT},{0,377,HTLEFT},{0,378,HTBOTTOMLEFT},
        {599,22,HTTOPRIGHT},{599,23,HTRIGHT},{599,377,HTRIGHT},{599,378,HTBOTTOMRIGHT}};
        win16_style_layout l;unsigned caption;
        for(caption=0;caption<2;++caption){
            CHECK(win16_style_measure(600,400,96,caption?WS_OVERLAPPEDWINDOW:WS_POPUP|WS_THICKFRAME,0,FALSE,&l));
            for(i=0;i<sizeof(edges)/sizeof(edges[0]);++i){POINT p={edges[i].x,edges[i].y};
                CHECK(win16_style_hit(&l,p)==edges[i].hit);}
        }
    }
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
    for(dpi=96;dpi<=192;dpi+=48){
        unsigned bits;
        for(bits=0;bits<8;++bits){
            DWORD style=WS_CAPTION|((bits&1)?WS_SYSMENU:0)|
                ((bits&2)?WS_MINIMIZEBOX:0)|((bits&4)?WS_MAXIMIZEBOX:0);
            win16_style_layout l;
            CHECK(win16_style_measure(600,400,dpi,style,0,FALSE,&l));
            CHECK(IsRectEmpty(&l.system)==!(bits&1));
            CHECK(IsRectEmpty(&l.minimize)==!(bits&2));
            CHECK(IsRectEmpty(&l.maximize)==!(bits&4));
            CHECK(l.title.left>=l.caption.left && l.title.right<=l.caption.right);
        }
        {DWORD frames[]={0,WS_BORDER,WS_DLGFRAME,WS_THICKFRAME};
        for(i=0;i<4;++i){win16_style_layout l;POINT p={100,100};
            CHECK(win16_style_measure(600,400,dpi,WS_POPUP|frames[i],0,FALSE,&l));
            CHECK(IsRectEmpty(&l.caption)&&IsRectEmpty(&l.system));
            CHECK(win16_style_hit(&l,p)==HTCLIENT);
            CHECK((l.border==0)==(i==0));
        }}
    }
}

static void menu_tests(void)
{
    RECT r;win16_style_layout l;LPARAM p;unsigned before;int variant;
    HWND owner=make_window(WS_OVERLAPPEDWINDOW,0,L"Menu test owner",20,20,FALSE);
    for(variant=0;variant<3;++variant){
    DWORD style=variant==0?WS_OVERLAPPEDWINDOW:WS_CAPTION|WS_SYSMENU|(variant==2?WS_POPUP:0);
    DWORD ex=variant?WS_EX_DLGMODALFRAME:0;
    menu_entries=0;menu_popups=0;key_menu_entries=0;
    menu_probe=make_window(style,ex,L"System menu contract",20,20,TRUE);
    if(variant==2){SetWindowLongPtrW(menu_probe,GWLP_HWNDPARENT,(LONG_PTR)owner);
        CHECK(GetWindow(menu_probe,GW_OWNER)==owner);}
    if(menu_desktop_test){
        ShowWindow(menu_probe,SW_SHOW);SetForegroundWindow(menu_probe);SetActiveWindow(menu_probe);pump();
        printf("MENU foreground=%d active=%d\n",GetForegroundWindow()==menu_probe,GetActiveWindow()==menu_probe);
    }
    CHECK(GetWindowRect(menu_probe,&r));
    CHECK(win16_style_measure(r.right-r.left,r.bottom-r.top,GetDpiForWindow(menu_probe),
        style,ex,FALSE,&l));
    p=MAKELPARAM(r.left+(l.system.left+l.system.right)/2,r.top+(l.system.top+l.system.bottom)/2);
    CHECK(SendMessageW(menu_probe,WM_NCHITTEST,0,p)==HTSYSMENU);
    {MENUBARINFO info={sizeof(info)};BOOL ok=GetMenuBarInfo(menu_probe,OBJID_SYSMENU,0,&info);
        printf("MENU geometry variant=%d native=%d rect=%ld,%ld,%ld,%ld custom=%d,%d\n",
            variant,ok,info.rcBar.left,info.rcBar.top,info.rcBar.right,info.rcBar.bottom,
            GET_X_LPARAM(p),GET_Y_LPARAM(p));fflush(stdout);}
    /* Enter the real native menu loop and cancel on idle, without input
     * injection. A generic TrackPopupMenu loop bypasses SC_MOUSEMENU. */
    if(variant){
        SendMessageW(menu_probe,WM_NCLBUTTONDOWN,HTSYSMENU,p);
        CHECK(GetCapture()==menu_probe);menu_pixels(menu_probe,TRUE);
        SendMessageW(menu_probe,WM_CANCELMODE,0,0);pump();
        CHECK(GetCapture()!=menu_probe);CHECK(key_menu_entries==0);
        menu_pixels(menu_probe,FALSE);
    }
    SendMessageW(menu_probe,WM_NCLBUTTONDOWN,HTSYSMENU,p);
    if(variant){
        POINT release={GET_X_LPARAM(p),GET_Y_LPARAM(p)};
        CHECK(GetCapture()==menu_probe);CHECK(menu_popups==0);CHECK(key_menu_entries==0);
        CHECK(ScreenToClient(menu_probe,&release));
        SendMessageW(menu_probe,WM_LBUTTONUP,0,MAKELPARAM(release.x,release.y));
        CHECK(GetCapture()!=menu_probe);pump();
    }
    printf("MENU variant=%d mouse_commands=%u popups=%u\n",variant,menu_entries,menu_popups);fflush(stdout);
    CHECK(menu_entries+key_menu_entries==1);CHECK(IsWindow(menu_probe));
    if(variant)CHECK(key_menu_entries==1);
    if(menu_desktop_test && variant)CHECK(menu_popups==1);
    if(menu_desktop_test){
        unsigned clicks=menu_popups;
        SendMessageW(menu_probe,WM_SYSCOMMAND,SC_KEYMENU,L' ');
        printf("MENU desktop variant=%d click_popups=%u keyboard_popups=%u\n",
            variant,clicks,menu_popups-clicks);fflush(stdout);
    }
    menu_pixels(menu_probe,FALSE);
    /* A non-input private desktop may cancel before entering the popup loop.
     * Explicit notification tests prove painting/state, not physical tracking. */
    SendMessageW(menu_probe,WM_INITMENUPOPUP,(WPARAM)GetSystemMenu(menu_probe,FALSE),MAKELPARAM(0,TRUE));
    menu_pixels(menu_probe,TRUE);
    SendMessageW(menu_probe,WM_NCPAINT,1,0);menu_pixels(menu_probe,TRUE);
    SendMessageW(menu_probe,WM_EXITMENULOOP,FALSE,0);menu_pixels(menu_probe,FALSE);
    SendMessageW(menu_probe,WM_INITMENUPOPUP,(WPARAM)GetSystemMenu(menu_probe,FALSE),0);
    menu_pixels(menu_probe,FALSE);
    EnableMenuItem(GetSystemMenu(menu_probe,FALSE),SC_CLOSE,MF_BYCOMMAND|MF_GRAYED);
    before=close_commands;
    SendMessageW(menu_probe,WM_NCLBUTTONDBLCLK,HTSYSMENU,p);pump();
    CHECK(IsWindow(menu_probe));CHECK(close_commands==before);
    EnableMenuItem(GetSystemMenu(menu_probe,FALSE),SC_CLOSE,MF_BYCOMMAND|MF_ENABLED);
    SendMessageW(menu_probe,WM_NCLBUTTONDBLCLK,HTSYSMENU,p);pump();
    CHECK(!IsWindow(menu_probe));CHECK(close_commands==before+1);
    menu_probe=NULL;
    }
    CHECK(DestroyWindow(owner));
}

static void nonclient_surface_probe(void)
{
    HWND w=make_window(WS_OVERLAPPEDWINDOW,0,L"Non-client repaint probe",20,20,TRUE);
    RECT r,client;win16_style_layout l;HDC dc=CreateCompatibleDC(NULL),source;
    BITMAPINFO bi={0};HBITMAP bm,old;void *pixels;DWORD *baseline;int width,height=80,i;
    const int hits[]={HTMINBUTTON,HTMAXBUTTON,HTCAPTION};
    static const struct {int hit;LPCWSTR cursor;} cursors[]={
        {HTCAPTION,IDC_ARROW},{HTSYSMENU,IDC_ARROW},{HTMINBUTTON,IDC_ARROW},{HTMAXBUTTON,IDC_ARROW},
        {HTLEFT,IDC_SIZEWE},{HTRIGHT,IDC_SIZEWE},{HTTOP,IDC_SIZENS},{HTBOTTOM,IDC_SIZENS},
        {HTTOPLEFT,IDC_SIZENWSE},{HTBOTTOMRIGHT,IDC_SIZENWSE},
        {HTTOPRIGHT,IDC_SIZENESW},{HTBOTTOMLEFT,IDC_SIZENESW}};
    GetWindowRect(w,&r);width=r.right-r.left;
    CHECK(win16_style_measure(width,r.bottom-r.top,GetDpiForWindow(w),WS_OVERLAPPEDWINDOW,0,FALSE,&l));
    client_rect(w,&client);OffsetRect(&client,-r.left,-r.top);
    printf("NC CLIENT actual=%ld,%ld,%ld,%ld layout=%ld,%ld,%ld,%ld dpi=%u\n",
        client.left,client.top,client.right,client.bottom,l.client.left,l.client.top,
        l.client.right,l.client.bottom,GetDpiForWindow(w));
    bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bi.bmiHeader.biWidth=width;bi.bmiHeader.biHeight=-height;
    bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;
    bm=CreateDIBSection(dc,&bi,DIB_RGB_COLORS,&pixels,NULL,0);CHECK(bm);old=SelectObject(dc,bm);
    baseline=malloc((size_t)width*height*4);CHECK(baseline);
    SetActiveWindow(w);RedrawWindow(w,NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);pump();
    /* The private desktop is not the input desktop: establish the tested
     * active presentation explicitly, including before the activation cycle. */
    SendMessageW(w,WM_NCACTIVATE,TRUE,0);
    for(i=0;i<9;++i){
        int n,changed=0,changed_inside=0,nonblack=0;WCHAR path[MAX_PATH];HANDLE out;DWORD written;BITMAPFILEHEADER fh={0};
        if(i>0 && i<4){RECT b=hits[i-1]==HTMINBUTTON?l.minimize:hits[i-1]==HTMAXBUTTON?l.maximize:l.title;
            /* Physical movement reaches WM_SETCURSOR before NC mouse input.
             * Sending only WM_NCMOUSEMOVE missed the native redraw route. */
            CHECK(SendMessageW(w,WM_SETCURSOR,(WPARAM)w,MAKELPARAM(hits[i-1],WM_MOUSEMOVE))==TRUE);
            SendMessageW(w,WM_NCMOUSEMOVE,hits[i-1],MAKELPARAM(r.left+(b.left+b.right)/2,r.top+(b.top+b.bottom)/2));
            SendMessageW(w,WM_NCMOUSEHOVER,hits[i-1],MAKELPARAM(r.left+(b.left+b.right)/2,r.top+(b.top+b.bottom)/2));
            SendMessageW(w,WM_NCMOUSELEAVE,0,0);pump();}
        if(i==4)CHECK(SetWindowTextW(w,L"Non-client repaint probe"));
        if(i==5)SendMessageW(w,WM_SETICON,ICON_SMALL,(LPARAM)LoadIconW(NULL,IDI_APPLICATION));
        if(i==6){SendMessageW(w,WM_NCACTIVATE,FALSE,0);SendMessageW(w,WM_NCACTIVATE,TRUE,0);}
        if(i==7)SendMessageW(w,WM_THEMECHANGED,0,0);
        if(i==8){
            ShowWindow(w,SW_MINIMIZE);pump();CHECK(IsIconic(w));
            cursor_probe=w;native_ncactivate_calls=0;
            SendMessageW(w,WM_NCACTIVATE,TRUE,0);CHECK(native_ncactivate_calls==1);
            cursor_probe=NULL;
            ShowWindow(w,SW_RESTORE);pump();CHECK(!IsIconic(w));
            SendMessageW(w,WM_NCACTIVATE,TRUE,0);
        }
        source=GetWindowDC(w);CHECK(source);CHECK(BitBlt(dc,0,0,width,height,source,0,0,SRCCOPY));ReleaseDC(w,source);GdiFlush();
        if(!i)memcpy(baseline,pixels,(size_t)width*height*4);
        for(n=0;n<width*height;++n){DWORD c=((DWORD *)pixels)[n]&0xffffff;
            if(c)++nonblack;if(c!=(baseline[n]&0xffffff)){
                if(changed<8)printf("NC PIXEL step=%d x=%d y=%d before=%06lx after=%06lx\n",
                    i,n%width,n/width,baseline[n]&0xffffff,c);
                if(n%width>=l.border && n%width<width-l.border && n/width>=l.border)++changed_inside;
                ++changed;}}
        printf("NC SURFACE step=%d nonblack=%d changed=%d\n",i,nonblack,changed);
        CHECK(nonblack>width*height/2);
        CHECK(swprintf_s(path,MAX_PATH,L"%ls-nc-%d.bmp",capture_path,i)>0);
        fh.bfType=0x4d42;fh.bfOffBits=sizeof(fh)+sizeof(bi.bmiHeader);fh.bfSize=fh.bfOffBits+(DWORD)(width*height*4);
        out=CreateFileW(path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);CHECK(out!=INVALID_HANDLE_VALUE);
        CHECK(WriteFile(out,&fh,sizeof(fh),&written,NULL));CHECK(WriteFile(out,&bi.bmiHeader,sizeof(bi.bmiHeader),&written,NULL));
        CHECK(WriteFile(out,pixels,(DWORD)(width*height*4),&written,NULL));CloseHandle(out);
        {int x,y,dirty=0;
            for(y=client.top;y<min(client.top+12,height);++y)
                for(x=client.left;x<client.right;++x)
                    if((((DWORD *)pixels)[y*width+x]&0xffffff)!=0xffffff)++dirty;
            printf("NC CLIENT strip dirty=%d step=%d\n",dirty,i);
            CHECK(dirty==0);
        }
        /* Native restore can change the two outermost clipped corner pixels.
         * Keep the full equality assertion for non-geometry events; after
         * restore require identical caption/buttons/client, excluding only
         * the outer frame band. Client-white checks remain unconditional. */
        if(i==8)CHECK(changed_inside==0);else CHECK(changed==0);
    }
    SendMessageW(w,WM_NCMOUSELEAVE,0,0);
    cursor_probe=w;client_cursor_calls=0;
    CHECK(SendMessageW(w,WM_SETCURSOR,(WPARAM)w,MAKELPARAM(HTCLIENT,WM_MOUSEMOVE))==TRUE);
    CHECK(client_cursor_calls==1);CHECK(GetCursor()==LoadCursorW(NULL,IDC_IBEAM));
    for(i=0;i<(int)(sizeof(cursors)/sizeof(cursors[0]));++i){
        CHECK(SendMessageW(w,WM_SETCURSOR,(WPARAM)w,MAKELPARAM(cursors[i].hit,WM_MOUSEMOVE))==TRUE);
        CHECK(GetCursor()==LoadCursorW(NULL,cursors[i].cursor));
    }
    CHECK(client_cursor_calls==1);cursor_probe=NULL;
    free(baseline);SelectObject(dc,old);DeleteObject(bm);DeleteDC(dc);CHECK(DestroyWindow(w));
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
    menu_desktop_test=argc==3 && !wcscmp(argv[1],L"--menu-test");
    testing=menu_desktop_test || (argc==3 && !wcscmp(argv[1],L"--test"));
    if(testing)capture_path=argv[2];
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    if(!testing){win16_style_gallery_run();return 0;}
    if(testing && !menu_desktop_test){
        WCHAR name[80];swprintf_s(name,80,L"Win16StyleTest-%lu",GetCurrentProcessId());
        desktop=CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL);
        CHECK(desktop!=NULL);CHECK(SetThreadDesktop(desktop));
    }
    cls.lpfnWndProc=window_proc;cls.hInstance=GetModuleHandleW(NULL);
    cls.hCursor=LoadCursorW(NULL,IDC_ARROW);cls.lpszClassName=L"Win16StyleTrial";
    cls.style=CS_DBLCLKS;CHECK(RegisterClassW(&cls)!=0);
    if(menu_desktop_test){menu_tests();return 0;}
    layout_tests();
    raster_scale_tests();
    font_probe();
    title_clip_test();
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
        WCHAR path[MAX_PATH];
        SendMessageW(w[0],WM_INITMENUPOPUP,(WPARAM)GetSystemMenu(w[0],FALSE),MAKELPARAM(0,TRUE));
        menu_pixels(w[0],TRUE);
        CHECK(swprintf_s(path,MAX_PATH,L"%ls-system-selected.bmp",argv[2])>0);
        capture(w,path,FALSE);
        SendMessageW(w[0],WM_EXITMENULOOP,FALSE,0);menu_pixels(w[0],FALSE);
        CHECK(swprintf_s(path,MAX_PATH,L"%ls-system-restored.bmp",argv[2])>0);
        capture(w,path,FALSE);
    }
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
    oem_window_tests(w[0]);
    menu_tests();
    nonclient_surface_probe();
    win16_style_gallery_test(argv[2]);
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
