/* Selectable standalone HWND gallery. Native references are explicitly not
 * retro-renderer support. Captures use only this process's owned windows. */
#include "win16-style/win16_style.h"
#include "win16-style/layout.h"
#include <stdio.h>
#include <stdlib.h>
#include <windowsx.h>

typedef struct sample {
    const WCHAR *name;
    DWORD style,ex;
    BOOL retro;
    int special;
} sample;
enum {NORMAL,OWNED,MENU,SCROLL,CHILD,MDI,ICONIC,MAXIMIZED};
static const sample samples[]={
    {L"01 Both buttons / sizeable",WS_OVERLAPPEDWINDOW,0,TRUE,NORMAL},
    {L"02 Minimize only / fixed",WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,0,TRUE,NORMAL},
    {L"03 Maximize only / sizeable",WS_CAPTION|WS_SYSMENU|WS_THICKFRAME|WS_MAXIMIZEBOX,0,TRUE,NORMAL},
    {L"04 No buttons / fixed",WS_CAPTION|WS_SYSMENU,0,TRUE,NORMAL},
    {L"05 No buttons / sizeable",WS_CAPTION|WS_SYSMENU|WS_THICKFRAME,0,TRUE,NORMAL},
    {L"06 Dialog frame",WS_CAPTION|WS_SYSMENU,WS_EX_DLGMODALFRAME,TRUE,NORMAL},
    {L"07 No system menu",WS_CAPTION|WS_THICKFRAME,0,TRUE,NORMAL},
    {L"08 Fixed caption, no controls",WS_CAPTION,0,TRUE,NORMAL},
    {L"09 No caption / sizeable",WS_POPUP|WS_THICKFRAME,0,TRUE,NORMAL},
    {L"10 No caption / thin border",WS_POPUP|WS_BORDER,0,TRUE,NORMAL},
    {L"11 Borderless",WS_POPUP,0,TRUE,NORMAL},
    {L"12 Owned popup dialog",WS_POPUP|WS_CAPTION|WS_SYSMENU,WS_EX_DLGMODALFRAME,TRUE,OWNED},
    {L"13 Maximized / restore",WS_OVERLAPPEDWINDOW,0,TRUE,MAXIMIZED},
    {L"14 Minimized (native state)",WS_OVERLAPPEDWINDOW,0,TRUE,ICONIC},
    {L"15 Menu bar [NATIVE reference]",WS_OVERLAPPEDWINDOW,0,FALSE,MENU},
    {L"16 Scrollbars [NATIVE reference]",WS_OVERLAPPEDWINDOW|WS_HSCROLL|WS_VSCROLL,0,FALSE,SCROLL},
    {L"17 Child [NATIVE reference]",WS_OVERLAPPEDWINDOW,0,FALSE,CHILD},
    {L"18 MDI [NATIVE reference]",WS_OVERLAPPEDWINDOW,0,FALSE,MDI},
    {L"19 Ordinary [NATIVE reference]",WS_OVERLAPPEDWINDOW,0,FALSE,NORMAL}
};
#define COUNT ((int)(sizeof(samples)/sizeof(samples[0])))
#define REQUIRE(x) do{if(!(x)){fprintf(stderr,"GALLERY FAIL line %d: %s error=%lu\n",__LINE__,#x,GetLastError());exit(1);}}while(0)
static HWND controller,listbox,opened[COUNT];
static BOOL test_mode;

static LRESULT CALLBACK mdi_proc(HWND w,UINT m,WPARAM a,LPARAM b)
{
    if(m==WM_PAINT){PAINTSTRUCT ps;HDC dc=BeginPaint(w,&ps);TextOutW(dc,12,12,L"Native MDI child - retro pending",31);EndPaint(w,&ps);return 0;}
    return DefMDIChildProcW(w,m,a,b);
}

static LRESULT CALLBACK sample_proc(HWND w,UINT m,WPARAM a,LPARAM b)
{
    const sample *s=(const sample *)GetWindowLongPtrW(w,GWLP_USERDATA);
    if(m==WM_NCCREATE){s=(const sample *)((CREATESTRUCTW *)b)->lpCreateParams;
        SetWindowLongPtrW(w,GWLP_USERDATA,(LONG_PTR)s);}
    if(m==WM_PAINT){
        PAINTSTRUCT ps;RECT r;HDC dc=BeginPaint(w,&ps);
        GetClientRect(w,&r);FillRect(dc,&r,(HBRUSH)GetStockObject(WHITE_BRUSH));
        SelectObject(dc,GetStockObject(DEFAULT_GUI_FONT));SetBkMode(dc,TRANSPARENT);
        r.left+=14;r.top+=14;
        if(s)DrawTextW(dc,s->name,-1,&r,DT_LEFT|DT_SINGLELINE);
        r.top+=28;
        DrawTextW(dc,L"F2: title   F3: colour   Esc: close\n"
            L"Activate/deactivate by clicking another window.\n"
            L"System box: click menu, double-click close.\n"
            L"Application client area is not restyled.",-1,&r,DT_LEFT);
        EndPaint(w,&ps);return 0;
    }
    if(m==WM_KEYDOWN){
        if(a==VK_ESCAPE){SendMessageW(w,WM_CLOSE,0,0);return 0;}
        if(a==VK_F2){SetWindowTextW(w,L"Changed title");return 0;}
        if(a==VK_F3){win16_style_palette p;win16_style_default_palette(&p);
            p.active=RGB(0,128,128);p.active_text=RGB(255,255,255);
            win16_style_set_palette(w,&p);return 0;}
    }
    if(m==WM_DPICHANGED){RECT *r=(RECT *)b;SetWindowPos(w,NULL,r->left,r->top,
        r->right-r->left,r->bottom-r->top,SWP_NOZORDER|SWP_NOACTIVATE);return 0;}
    if(m==WM_CLOSE){DestroyWindow(w);return 0;}
    if(s && s->special==MDI){
        HWND client=GetDlgItem(w,2);
        if(m==WM_SIZE && client){RECT r;GetClientRect(w,&r);
            SetWindowPos(client,NULL,8,105,max(1,r.right-16),max(1,r.bottom-110),SWP_NOZORDER);}
        return DefFrameProcW(w,client,m,a,b);
    }
    return DefWindowProcW(w,m,a,b);
}

static void register_classes(void)
{
    WNDCLASSW c={0};c.hInstance=GetModuleHandleW(NULL);c.hCursor=LoadCursorW(NULL,IDC_ARROW);
    c.style=CS_DBLCLKS;c.lpfnWndProc=sample_proc;c.lpszClassName=L"StyleGallerySample";
    REQUIRE(RegisterClassW(&c));c.lpfnWndProc=mdi_proc;c.lpszClassName=L"StyleGalleryMDI";
    REQUIRE(RegisterClassW(&c));
}

static HWND create_sample(int index,HWND owner,BOOL state)
{
    const sample *s=&samples[index];RECT before,after;POINT p={0,0};HWND w;
    w=CreateWindowExW(s->ex,L"StyleGallerySample",s->name,s->style,
        80+(index%4)*40,80+(index%4)*35,480,230,s->special==OWNED?owner:NULL,
        NULL,GetModuleHandleW(NULL),(void *)s);REQUIRE(w);
    GetClientRect(w,&before);ClientToScreen(w,&p);OffsetRect(&before,p.x,p.y);
    if(s->retro){REQUIRE(win16_style_install(w,NULL));
        p.x=p.y=0;GetClientRect(w,&after);ClientToScreen(w,&p);OffsetRect(&after,p.x,p.y);
        REQUIRE(EqualRect(&before,&after));}
    if(s->special==MENU){HMENU menu=CreateMenu(),file=CreatePopupMenu();
        REQUIRE(menu&&file);AppendMenuW(file,MF_STRING,1,L"Example command");
        AppendMenuW(menu,MF_POPUP,(UINT_PTR)file,L"File (native)");REQUIRE(SetMenu(w,menu));}
    if(s->special==SCROLL){SetScrollRange(w,SB_HORZ,0,100,FALSE);SetScrollRange(w,SB_VERT,0,100,FALSE);}
    if(s->special==CHILD)REQUIRE(CreateWindowW(L"STATIC",L"Native child window",WS_CHILD|WS_VISIBLE|WS_BORDER,
        20,115,280,45,w,NULL,GetModuleHandleW(NULL),NULL));
    if(s->special==MDI){CLIENTCREATESTRUCT cc={0};HWND client,child;RECT r;GetClientRect(w,&r);
        client=CreateWindowW(L"MDICLIENT",NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,
            8,105,r.right-16,max(60,r.bottom-110),w,(HMENU)2,GetModuleHandleW(NULL),&cc);REQUIRE(client);
        child=CreateMDIWindowW(L"StyleGalleryMDI",L"MDI child (native)",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
            0,0,300,80,client,GetModuleHandleW(NULL),0);REQUIRE(child);}
    ShowWindow(w,state&&s->special==MAXIMIZED?SW_MAXIMIZE:state&&s->special==ICONIC?SW_MINIMIZE:SW_SHOWNOACTIVATE);
    UpdateWindow(w);return w;
}

static void close_samples(void)
{
    int i;for(i=0;i<COUNT;++i){if(IsWindow(opened[i]))DestroyWindow(opened[i]);opened[i]=NULL;}
}

static LRESULT CALLBACK controller_proc(HWND w,UINT m,WPARAM a,LPARAM b)
{
    if(m==WM_COMMAND){
        int id=LOWORD(a),index=(int)SendMessageW(listbox,LB_GETCURSEL,0,0);
        if((id==1 || (id==10&&HIWORD(a)==LBN_DBLCLK)) && index>=0 && index<COUNT){
            if(!IsWindow(opened[index]))opened[index]=create_sample(index,w,TRUE);
            else {ShowWindow(opened[index],SW_RESTORE);SetForegroundWindow(opened[index]);}
        }
        if(id==2)close_samples();return 0;
    }
    if(m==WM_CLOSE){close_samples();DestroyWindow(w);return 0;}
    if(m==WM_DESTROY){if(!test_mode)PostQuitMessage(0);return 0;}
    return DefWindowProcW(w,m,a,b);
}

static void create_controller(void)
{
    WNDCLASSW c={0};int i;HWND button;
    register_classes();c.lpfnWndProc=controller_proc;c.hInstance=GetModuleHandleW(NULL);
    c.hCursor=LoadCursorW(NULL,IDC_ARROW);c.hbrBackground=(HBRUSH)(COLOR_BTNFACE+1);
    c.lpszClassName=L"StyleGalleryControl";REQUIRE(RegisterClassW(&c));
    controller=CreateWindowW(c.lpszClassName,L"Win3.1 frame gallery - select a window to inspect",
        WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,40,40,650,620,NULL,NULL,c.hInstance,NULL);REQUIRE(controller);
    listbox=CreateWindowW(L"LISTBOX",NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_BORDER|LBS_NOTIFY,
        16,16,595,490,controller,(HMENU)10,c.hInstance,NULL);REQUIRE(listbox);
    SendMessageW(listbox,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),TRUE);
    for(i=0;i<COUNT;++i)SendMessageW(listbox,LB_ADDSTRING,0,(LPARAM)samples[i].name);
    SendMessageW(listbox,LB_SETCURSEL,0,0);
    button=CreateWindowW(L"BUTTON",L"Open selected",WS_CHILD|WS_VISIBLE|WS_TABSTOP,
        16,520,180,30,controller,(HMENU)1,c.hInstance,NULL);REQUIRE(button);
    button=CreateWindowW(L"BUTTON",L"Close samples",WS_CHILD|WS_VISIBLE|WS_TABSTOP,
        215,520,180,30,controller,(HMENU)2,c.hInstance,NULL);REQUIRE(button);
}

void win16_style_gallery_run(void)
{
    MSG m;create_controller();
    ShowWindow(controller,SW_SHOW);UpdateWindow(controller);
    while(GetMessageW(&m,NULL,0,0)>0){TranslateMessage(&m);DispatchMessageW(&m);}
}

void win16_style_gallery_test(const WCHAR *path)
{
    int page,i;test_mode=TRUE;create_controller();
    for(page=0;page<(COUNT+3)/4;++page){
        BITMAPINFO bi={0};BITMAPFILEHEADER fh={0};HDC dc=CreateCompatibleDC(NULL),part=CreateCompatibleDC(NULL);
        HBITMAP bm,old;void *pixels;RECT bg={0,0,1060,610};WCHAR name[MAX_PATH];DWORD bytes;HANDLE out;
        bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bi.bmiHeader.biWidth=1060;bi.bmiHeader.biHeight=-610;
        bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;
        bm=CreateDIBSection(dc,&bi,DIB_RGB_COLORS,&pixels,NULL,0);REQUIRE(bm);old=SelectObject(dc,bm);
        FillRect(dc,&bg,(HBRUSH)GetStockObject(LTGRAY_BRUSH));
        for(i=page*4;i<min(COUNT,page*4+4);++i){
            HWND w=create_sample(i,controller,FALSE);RECT r;HBITMAP shot,prior;int width,height,x,y;
            if(samples[i].special==OWNED)REQUIRE(GetWindow(w,GW_OWNER)==controller);
            GetWindowRect(w,&r);width=r.right-r.left;height=r.bottom-r.top;
            if(samples[i].retro){win16_style_layout l;POINT p;RECT actual;
                REQUIRE(win16_style_measure(width,height,GetDpiForWindow(w),samples[i].style,samples[i].ex,FALSE,&l));
                p.x=r.left+l.client.left+10;p.y=r.top+l.client.top+10;
                REQUIRE(SendMessageW(w,WM_NCHITTEST,0,MAKELPARAM(p.x,p.y))==HTCLIENT);
                GetClientRect(w,&actual);REQUIRE(actual.right==l.client.right-l.client.left);
                REQUIRE(actual.bottom==l.client.bottom-l.client.top);
                SendMessageW(w,WM_NCACTIVATE,i%2,0);
                if(!(samples[i].style&WS_CAPTION))REQUIRE(IsRectEmpty(&l.caption));
            }else REQUIRE(!win16_style_set_palette(w,NULL));
            shot=CreateCompatibleBitmap(dc,width,height);REQUIRE(shot);prior=SelectObject(part,shot);
            REQUIRE(PrintWindow(w,part,0));x=20+(i%2)*525;y=30+((i%4)/2)*295;
            SetBkMode(dc,TRANSPARENT);TextOutW(dc,x,y-20,samples[i].name,lstrlenW(samples[i].name));
            REQUIRE(BitBlt(dc,x,y,width,height,part,0,0,SRCCOPY));
            SelectObject(part,prior);DeleteObject(shot);
            /* The maximized/minimized state is exercised on the same gallery
             * HWND after its readable normal-state capture. */
            if(samples[i].special==MAXIMIZED){ShowWindow(w,SW_MAXIMIZE);REQUIRE(IsZoomed(w));ShowWindow(w,SW_RESTORE);REQUIRE(!IsZoomed(w));}
            if(samples[i].special==ICONIC){ShowWindow(w,SW_MINIMIZE);REQUIRE(IsIconic(w));ShowWindow(w,SW_RESTORE);REQUIRE(!IsIconic(w));}
            if(samples[i].retro){REQUIRE(win16_style_remove(w));REQUIRE(win16_style_install(w,NULL));}
            REQUIRE(DestroyWindow(w));printf("GALLERY PASS %02d retro=%d special=%d\n",i+1,samples[i].retro,samples[i].special);
        }
        GdiFlush();REQUIRE(swprintf_s(name,MAX_PATH,L"%ls-gallery-%d.bmp",path,page+1)>0);
        fh.bfType=0x4d42;fh.bfOffBits=sizeof(fh)+sizeof(bi.bmiHeader);fh.bfSize=fh.bfOffBits+1060*610*4;
        out=CreateFileW(name,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);REQUIRE(out!=INVALID_HANDLE_VALUE);
        REQUIRE(WriteFile(out,&fh,sizeof(fh),&bytes,NULL));REQUIRE(WriteFile(out,&bi.bmiHeader,sizeof(bi.bmiHeader),&bytes,NULL));
        REQUIRE(WriteFile(out,pixels,1060*610*4,&bytes,NULL));CloseHandle(out);
        SelectObject(dc,old);DeleteObject(bm);DeleteDC(part);DeleteDC(dc);
    }
    /* Exercise the actual user entrypoint, selection and Close samples button. */
    for(i=0;i<COUNT;++i){
        REQUIRE(SendMessageW(listbox,LB_SETCURSEL,i,0)!=LB_ERR);
        SendMessageW(controller,WM_COMMAND,1,0);REQUIRE(IsWindow(opened[i]));
        SendMessageW(controller,WM_COMMAND,2,0);REQUIRE(!IsWindow(opened[i]));
    }
    SendMessageW(controller,WM_CLOSE,0,0);REQUIRE(!IsWindow(controller));
    printf("GALLERY PASS selector open/close: %d cases\n",COUNT);
}
