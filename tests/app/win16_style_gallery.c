/* Selectable standalone HWND gallery. Native references are explicitly not
 * retro-renderer support. Captures use only this process's owned windows. */
#include "win16-style/win16_style.h"
#include "win16-style/layout.h"
#include "win16-style/menu_paint.h"
#include "win16-style/menu_bar.h"
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
    {L"15 Retro menu bar and popups",WS_OVERLAPPEDWINDOW,0,TRUE,MENU},
    {L"16 Scrollbars [NATIVE reference]",WS_OVERLAPPEDWINDOW|WS_HSCROLL|WS_VSCROLL,0,FALSE,SCROLL},
    {L"17 Child [NATIVE reference]",WS_OVERLAPPEDWINDOW,0,FALSE,CHILD},
    {L"18 MDI frames and merged menus",WS_OVERLAPPEDWINDOW,0,TRUE,MDI},
    {L"19 Ordinary [NATIVE reference]",WS_OVERLAPPEDWINDOW,0,FALSE,NORMAL}
};
#define COUNT ((int)(sizeof(samples)/sizeof(samples[0])))
#define REQUIRE(x) do{if(!(x)){fprintf(stderr,"GALLERY FAIL line %d: %s error=%lu\n",__LINE__,#x,GetLastError());exit(1);}}while(0)
static HWND controller,listbox,opened[COUNT];
static BOOL test_mode;
static const WCHAR *menu_labels[]={L"&Example command\tCtrl+E",L"&Disabled command",L"",L"&Close\tAlt+F4",L"&More",L"&Nested command",L"&File"};
#define MENU_LABEL_COUNT (sizeof(menu_labels)/sizeof(menu_labels[0]))
static win16_style_menu_kind menu_kind(ULONG_PTR data)
{
    return data==2?WIN16_STYLE_MENU_SEPARATOR:data==6?WIN16_STYLE_MENU_BAR:WIN16_STYLE_MENU_ROW;
}
static unsigned menu_measure_count,menu_draw_count,menu_command_count;
static int menu_test_action;
static BOOL menu_test_timeout;
static unsigned menu_test_step;
static const WCHAR *menu_capture_path;
static BOOL menu_captured;
static BOOL menu_test_bar;
static BOOL menu_test_system;
static POINT menu_test_double_point;
static BOOL menu_boundary_system;
static HWND menu_route_parent,menu_route_child;
static void route_idle(HWND w)
{
    BOOL child=menu_test_step==1 || (menu_test_action==12 && menu_test_step==3);
    UINT key=menu_test_action==12?(menu_test_step<2?VK_LEFT:menu_test_step<4?VK_RIGHT:VK_ESCAPE):
        (menu_test_step==0?VK_LEFT:menu_test_step==1?VK_RIGHT:VK_ESCAPE);
    REQUIRE(w==(child?menu_route_child:menu_route_parent));
    PostMessageW(w,WM_KEYDOWN,key,0);
    ++menu_test_step;
}

static void check_bar_selection(HWND w,BOOL selected)
{
    RECT r;win16_style_layout base;win16_style_palette palette;
    HDC dc=CreateCompatibleDC(NULL);HBITMAP bitmap,old;
    REQUIRE(dc && GetWindowRect(w,&r));
    REQUIRE(win16_style_measure(r.right-r.left,r.bottom-r.top,GetDpiForWindow(w),
        (DWORD)GetWindowLongPtrW(w,GWL_STYLE),(DWORD)GetWindowLongPtrW(w,GWL_EXSTYLE),FALSE,&base));
    {HDC screen=GetDC(w);bitmap=CreateCompatibleBitmap(screen,r.right-r.left,r.bottom-r.top);ReleaseDC(w,screen);}
    REQUIRE(bitmap);old=SelectObject(dc,bitmap);REQUIRE(PrintWindow(w,dc,0));
    win16_style_default_palette(&palette);
    REQUIRE(GetPixel(dc,base.client.left+1,base.client.top+1)==(selected?palette.active:palette.light));
    SelectObject(dc,old);DeleteObject(bitmap);DeleteDC(dc);
}

/* Resolve only this fixture's owned items in the menu Windows actually names.
 * Native tracking still selects/executes; no command IDs or positions are
 * inferred from a particular key. General application-menu adaptation remains
 * a separate component boundary. */
static LRESULT menu_mnemonic(HMENU menu,WCHAR key)
{
    int count=GetMenuItemCount(menu),i,selected=-1,first=-1,next=-1,matches=0;
    WCHAR folded=key;
    CharUpperBuffW(&folded,1);
    for(i=0;i<count;++i){
        MENUITEMINFOW item={sizeof(item)};
        item.fMask=MIIM_FTYPE|MIIM_STATE|MIIM_DATA;
        if(!GetMenuItemInfoW(menu,(UINT)i,TRUE,&item))continue;
        if(item.fState&MFS_HILITE)selected=i;
    }
    for(i=0;i<count;++i){
        MENUITEMINFOW item={sizeof(item)};const WCHAR *text;
        item.fMask=MIIM_FTYPE|MIIM_STATE|MIIM_DATA;
        if(!GetMenuItemInfoW(menu,(UINT)i,TRUE,&item) ||
            !(item.fType&MFT_OWNERDRAW) || (item.fType&MFT_SEPARATOR) ||
            (item.fState&(MFS_DISABLED|MFS_GRAYED)) || item.dwItemData>=MENU_LABEL_COUNT)continue;
        text=menu_labels[item.dwItemData];
        for(;*text && *text!=L'\t';++text){
            WCHAR letter;
            if(*text!=L'&' || !text[1])continue;
            ++text;if(*text==L'&')continue;
            letter=*text;CharUpperBuffW(&letter,1);
            if(letter!=folded)continue;
            if(first<0)first=i;
            if(i>selected && next<0)next=i;
            ++matches;break;
        }
    }
    if(!matches)return MAKELRESULT(0,MNC_IGNORE);
    return MAKELRESULT(matches==1?first:(next<0?first:next),matches==1?MNC_EXECUTE:MNC_SELECT);
}

static void capture_menu(HWND popup,const WCHAR *name,BOOL check_border)
{
    RECT r;BITMAPINFO bi={0};BITMAPFILEHEADER fh={0};HDC dc;HBITMAP bm,old;
    void *pixels;DWORD bytes;HANDLE file;WCHAR path[MAX_PATH];
    REQUIRE(GetWindowRect(popup,&r));
    dc=CreateCompatibleDC(NULL);REQUIRE(dc);
    bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bi.bmiHeader.biWidth=r.right-r.left;
    bi.bmiHeader.biHeight=r.top-r.bottom;bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;
    bm=CreateDIBSection(dc,&bi,DIB_RGB_COLORS,&pixels,NULL,0);REQUIRE(bm);old=SelectObject(dc,bm);
    /* Private-desktop print, not a physical-screen first-paint observation. */
    REQUIRE(PrintWindow(popup,dc,0));GdiFlush();
    /* Verify the actual scoped popup painter, not only the paint primitive. */
    if(check_border){int x,width=r.right-r.left,height=r.bottom-r.top;DWORD *p=(DWORD *)pixels;
        for(x=0;x<width;++x){REQUIRE((p[x]&0xffffff)==0);
            REQUIRE((p[(height-1)*width+x]&0xffffff)==0);}}
    REQUIRE(swprintf_s(path,MAX_PATH,L"%ls-%ls-popup.bmp",menu_capture_path,name)>0);
    file=CreateFileW(path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);REQUIRE(file!=INVALID_HANDLE_VALUE);
    fh.bfType=0x4d42;fh.bfOffBits=sizeof(fh)+sizeof(bi.bmiHeader);
    fh.bfSize=fh.bfOffBits+(r.right-r.left)*(r.bottom-r.top)*4;
    REQUIRE(WriteFile(file,&fh,sizeof(fh),&bytes,NULL));
    REQUIRE(WriteFile(file,&bi.bmiHeader,sizeof(bi.bmiHeader),&bytes,NULL));
    REQUIRE(WriteFile(file,pixels,fh.bfSize-fh.bfOffBits,&bytes,NULL));CloseHandle(file);
    SelectObject(dc,old);DeleteObject(bm);DeleteDC(dc);menu_captured=TRUE;
}

static LRESULT CALLBACK mdi_proc(HWND w,UINT m,WPARAM a,LPARAM b)
{
    if((menu_test_action==11 || menu_test_action==12) && m==WM_ENTERIDLE && a==MSGF_MENU){route_idle(w);return 0;}
    if(m==WM_PAINT){PAINTSTRUCT ps;RECT r;HDC dc=BeginPaint(w,&ps);const WCHAR *text=L"Retro child; native MDI management";
        GetClientRect(w,&r);FillRect(dc,&r,(HBRUSH)GetStockObject(WHITE_BRUSH));
        TextOutW(dc,12,12,text,lstrlenW(text));EndPaint(w,&ps);return 0;}
    return DefMDIChildProcW(w,m,a,b);
}

static LRESULT CALLBACK sample_proc(HWND w,UINT m,WPARAM a,LPARAM b)
{
    const sample *s=(const sample *)GetWindowLongPtrW(w,GWLP_USERDATA);
    if(m==WM_NCCREATE){s=(const sample *)((CREATESTRUCTW *)b)->lpCreateParams;
        SetWindowLongPtrW(w,GWLP_USERDATA,(LONG_PTR)s);}
    if(menu_test_action>=5 && m==WM_INITMENUPOPUP && (HMENU)a==GetSystemMenu(w,FALSE))
        menu_boundary_system=TRUE;
    if(s && (s->special==MENU || s->special==OWNED || s->special==MDI) && menu_test_action && m==WM_ENTERIDLE && a==MSGF_MENU){
        if(menu_test_action==11 || menu_test_action==12){route_idle(w);return 0;}
        if(menu_test_action==4){PostMessageW(w,WM_NCLBUTTONDBLCLK,HTMENU,
            MAKELPARAM(menu_test_double_point.x,menu_test_double_point.y));return 0;}
        if(menu_test_action==5){
            if(menu_test_step++<2)PostMessageW(w,WM_KEYDOWN,VK_LEFT,0);
            else PostMessageW(w,WM_CHAR,L'n',0);
            return 0;}
        if(menu_test_action==6){
            if(menu_test_step++<2)PostMessageW(w,WM_KEYDOWN,VK_RIGHT,0);
            else PostMessageW(w,WM_CHAR,L'e',0);
            return 0;}
        if(menu_test_action==7){
            PostMessageW(w,WM_KEYDOWN,menu_test_step++?VK_ESCAPE:VK_LEFT,0);return 0;}
        if(menu_test_action==8 || menu_test_action==9){
            if(!menu_test_step++)PostMessageW(w,WM_MOUSEMOVE,0,
                MAKELPARAM(menu_test_double_point.x,menu_test_double_point.y));
            else PostMessageW(w,WM_CHAR,L'n',0);
            return 0;}
        if(menu_test_action==10){
            if(!menu_test_step){
                HMENU outer=GetSubMenu(GetMenu(w),3),inner=CreatePopupMenu();
                MENUITEMINFOW info={sizeof(info)};RECT bounds;UINT command;
                info.fMask=MIIM_FTYPE;
                REQUIRE(GetMenuItemInfoW(outer,0,TRUE,&info) && (info.fType&MFT_OWNERDRAW));
                REQUIRE(inner && AppendMenuW(inner,MF_STRING,400,L"&Inner command"));
                REQUIRE(GetWindowRect(w,&bounds));menu_test_step=1;
                command=(UINT)TrackPopupMenuEx(inner,TPM_RECURSE|TPM_RETURNCMD|TPM_NOANIMATION,
                    bounds.left+30,bounds.top+80,w,NULL);
                menu_test_step=2;REQUIRE(command==400);
                REQUIRE(GetMenuItemInfoW(inner,0,TRUE,&info) && !(info.fType&MFT_OWNERDRAW));
                REQUIRE(GetMenuItemInfoW(outer,0,TRUE,&info) && (info.fType&MFT_OWNERDRAW));
                REQUIRE(DestroyMenu(inner));PostMessageW(w,WM_CHAR,L'n',0);
            }else PostMessageW(w,WM_CHAR,menu_test_step==1?L'i':L'n',0);
            return 0;}
        if(menu_test_system){MENUITEMINFOW info={sizeof(info)};info.fMask=MIIM_FTYPE;
            REQUIRE(GetMenuItemInfoW(GetSystemMenu(w,FALSE),SC_CLOSE,FALSE,&info));
            REQUIRE(info.fType&MFT_OWNERDRAW);}
        if(menu_test_bar)check_bar_selection(w,TRUE);
        if(menu_capture_path && !menu_captured)capture_menu((HWND)b,menu_test_system?L"system":L"menu",TRUE);
        if(menu_test_action==3)PostMessageW(w,WM_CHAR,menu_test_step++?L'n':L'm',0);
        else if(menu_test_action==1)PostMessageW(w,WM_CHAR,L'e',0);
        else PostMessageW(w,WM_KEYDOWN,VK_ESCAPE,0);
        return 0;
    }
    if(s && s->special==MENU && m==WM_MENUCHAR){
        return menu_mnemonic((HMENU)b,(WCHAR)LOWORD(a));
    }
    if(s && (s->special==MENU || s->special==OWNED || s->special==MDI) && m==WM_TIMER && a==97){menu_test_timeout=TRUE;EndMenu();return 0;}
    if(s && s->special==MENU && m==WM_MEASUREITEM){
        MEASUREITEMSTRUCT *item=(MEASUREITEMSTRUCT *)b;SIZE size;HDC dc;
        if(item->CtlType==ODT_MENU && item->itemData<MENU_LABEL_COUNT){
            dc=GetDC(w);REQUIRE(win16_style_menu_measure(dc,(HFONT)GetStockObject(SYSTEM_FONT),
                GetDpiForWindow(w),menu_labels[item->itemData],menu_kind(item->itemData),&size));ReleaseDC(w,dc);
            item->itemWidth=size.cx;item->itemHeight=size.cy;++menu_measure_count;return TRUE;
        }
    }
    if(s && s->special==MENU && m==WM_DRAWITEM){
        DRAWITEMSTRUCT *item=(DRAWITEMSTRUCT *)b;win16_style_palette palette;
        if(item->CtlType==ODT_MENU && item->itemData<MENU_LABEL_COUNT){
            win16_style_default_palette(&palette);++menu_draw_count;
            return win16_style_menu_text(item->hDC,&item->rcItem,(HFONT)GetStockObject(SYSTEM_FONT),
                GetDpiForWindow(w),&palette,menu_labels[item->itemData],item->itemState,menu_kind(item->itemData),item->itemData==4);
        }
    }
    if(s && s->special==MENU && m==WM_COMMAND){
        if(LOWORD(a)==100){HMENU menu=GetSubMenu(GetMenu(w),0);UINT flags=GetMenuState(menu,100,MF_BYCOMMAND);
            CheckMenuItem(menu,100,MF_BYCOMMAND|((flags&MF_CHECKED)?MF_UNCHECKED:MF_CHECKED));
            ++menu_command_count;SetWindowTextW(w,L"Example command received");return 0;}
        if(LOWORD(a)==200){++menu_command_count;SetWindowTextW(w,L"Nested command received");return 0;}
        if(LOWORD(a)==300){++menu_command_count;SetWindowTextW(w,L"Bar action received");return 0;}
        if(LOWORD(a)==103){SendMessageW(w,WM_CLOSE,0,0);return 0;}
    }
    if(m==WM_PAINT){
        PAINTSTRUCT ps;RECT r;HDC dc=BeginPaint(w,&ps);
        if(s && s->special==MDI){EndPaint(w,&ps);return 0;}
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
            SetWindowPos(client,NULL,0,0,max(1,r.right),max(1,r.bottom),SWP_NOZORDER);}
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

static INT_PTR CALLBACK dialog_proc(HWND w,UINT m,WPARAM a,LPARAM b)
{
    if(m==WM_INITDIALOG){SetWindowLongPtrW(w,GWLP_USERDATA,b);return TRUE;}
    if(m==WM_PAINT || m==WM_CLOSE || m==WM_KEYDOWN || m==WM_DPICHANGED ||
        m==WM_ENTERIDLE || (m==WM_TIMER && a==97)){
        sample_proc(w,m,a,b);return TRUE;
    }
    return FALSE; /* DefDlgProc owns the actual dialog/system-menu contract. */
}

static HWND create_sample(int index,HWND owner,BOOL state)
{
    const sample *s=&samples[index];RECT before,after;POINT p={0,0};HWND w;
    if(s->special==OWNED){
        struct {DLGTEMPLATE dialog;WORD menu,window_class,title;} resource={0};
        resource.dialog.style=s->style|DS_MODALFRAME;resource.dialog.dwExtendedStyle=s->ex;
        resource.dialog.cx=240;resource.dialog.cy=115;
        w=CreateDialogIndirectParamW(GetModuleHandleW(NULL),&resource.dialog,owner,dialog_proc,(LPARAM)s);
        REQUIRE(w);SetWindowTextW(w,s->name);
        if(test_mode){HMENU system=GetSystemMenu(w,FALSE);WCHAR name[32];
            REQUIRE(GetClassNameW(w,name,32) && !lstrcmpW(name,L"#32770"));
            REQUIRE(GetMenuState(system,SC_MOVE,MF_BYCOMMAND)!=(UINT)-1);
            REQUIRE(GetMenuState(system,SC_CLOSE,MF_BYCOMMAND)!=(UINT)-1);
            REQUIRE(GetMenuState(system,SC_RESTORE,MF_BYCOMMAND)==(UINT)-1);
            REQUIRE(GetMenuState(system,SC_SIZE,MF_BYCOMMAND)==(UINT)-1);
            REQUIRE(GetMenuState(system,SC_MINIMIZE,MF_BYCOMMAND)==(UINT)-1);
            REQUIRE(GetMenuState(system,SC_MAXIMIZE,MF_BYCOMMAND)==(UINT)-1);}
        SetWindowPos(w,NULL,80+(index%4)*40,80+(index%4)*35,480,230,SWP_NOZORDER|SWP_NOACTIVATE);
    }else w=CreateWindowExW(s->ex,L"StyleGallerySample",s->name,s->style,
        80+(index%4)*40,80+(index%4)*35,480,230,s->special==OWNED?owner:NULL,
        NULL,GetModuleHandleW(NULL),(void *)s);REQUIRE(w);
    GetClientRect(w,&before);ClientToScreen(w,&p);OffsetRect(&before,p.x,p.y);
    if(s->retro){REQUIRE(win16_style_install(w,NULL));
        p.x=p.y=0;GetClientRect(w,&after);ClientToScreen(w,&p);OffsetRect(&after,p.x,p.y);
        REQUIRE(EqualRect(&before,&after));}
    if(s->special==MENU){HMENU menu=CreateMenu(),file=CreatePopupMenu(),more=CreatePopupMenu();int n;
        REQUIRE(menu&&file&&more);
        {MENUITEMINFOW item={sizeof(item)};item.fMask=MIIM_FTYPE|MIIM_ID|MIIM_DATA;
            item.fType=MFT_OWNERDRAW;item.wID=200;item.dwItemData=5;REQUIRE(InsertMenuItemW(more,0,TRUE,&item));}
        for(n=0;n<5;++n){MENUITEMINFOW item={sizeof(item)};
            item.fMask=MIIM_FTYPE|MIIM_ID|MIIM_DATA|MIIM_STATE;
            item.fType=MFT_OWNERDRAW|(n==2?MFT_SEPARATOR:0);item.wID=100+n;item.dwItemData=n;
            item.fState=n==1?MFS_DISABLED:n==0?MFS_CHECKED:0;
            if(n==4){item.fMask|=MIIM_SUBMENU;item.hSubMenu=more;}
            REQUIRE(InsertMenuItemW(file,n,TRUE,&item));}
        REQUIRE(AppendMenuW(menu,MF_POPUP,(UINT_PTR)file,L"&File"));
        REQUIRE(AppendMenuW(menu,MF_STRING|MF_DISABLED,301,L"&Disabled"));
        REQUIRE(AppendMenuW(menu,MF_STRING,300,L"&Action"));
        {HMENU extra=CreatePopupMenu();REQUIRE(extra);
            REQUIRE(AppendMenuW(extra,MF_STRING,200,L"&Next command"));
            REQUIRE(AppendMenuW(menu,MF_POPUP,(UINT_PTR)extra,L"&Extra"));}
        REQUIRE(SetMenu(w,menu));}
    if(s->special==SCROLL){SetScrollRange(w,SB_HORZ,0,100,FALSE);SetScrollRange(w,SB_VERT,0,100,FALSE);}
    if(s->special==CHILD)REQUIRE(CreateWindowW(L"STATIC",L"Native child window",WS_CHILD|WS_VISIBLE|WS_BORDER,
        20,115,280,45,w,NULL,GetModuleHandleW(NULL),NULL));
    if(s->special==MDI){CLIENTCREATESTRUCT cc={0};HWND client,child,second;RECT r;
        HMENU menu=CreateMenu(),window=CreatePopupMenu();REQUIRE(menu && window);
        REQUIRE(AppendMenuW(menu,MF_POPUP,(UINT_PTR)window,L"&Window"));
        REQUIRE(SetMenu(w,menu));cc.hWindowMenu=window;cc.idFirstChild=5000;GetClientRect(w,&r);
        client=CreateWindowW(L"MDICLIENT",NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,
            0,0,r.right,r.bottom,w,(HMENU)2,GetModuleHandleW(NULL),&cc);REQUIRE(client);
        child=CreateMDIWindowW(L"StyleGalleryMDI",L"MDI child 1",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
            0,0,330,120,client,GetModuleHandleW(NULL),0);REQUIRE(child);
        GetClientRect(child,&before);p.x=p.y=0;ClientToScreen(child,&p);OffsetRect(&before,p.x,p.y);
        REQUIRE(win16_style_install(child,NULL));
        GetClientRect(child,&after);p.x=p.y=0;ClientToScreen(child,&p);OffsetRect(&after,p.x,p.y);
        REQUIRE(EqualRect(&before,&after));
        second=CreateMDIWindowW(L"StyleGalleryMDI",L"MDI child 2",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
            90,35,330,120,client,GetModuleHandleW(NULL),0);REQUIRE(second);
        REQUIRE(win16_style_install(second,NULL));
        if(test_mode){
            SendMessageW(client,WM_MDIACTIVATE,(WPARAM)child,0);
            REQUIRE((HWND)SendMessageW(client,WM_MDIGETACTIVE,0,0)==child);
            {MDINEXTMENU next={0};
                next.hmenuIn=GetMenu(w);SendMessageW(w,WM_NEXTMENU,VK_LEFT,(LPARAM)&next);
                REQUIRE(next.hwndNext==child && next.hmenuNext==GetSystemMenu(child,FALSE));
                next.hmenuIn=GetSystemMenu(w,FALSE);next.hmenuNext=NULL;next.hwndNext=NULL;
                SendMessageW(w,WM_NEXTMENU,VK_RIGHT,(LPARAM)&next);
                REQUIRE(next.hwndNext==child && next.hmenuNext==GetSystemMenu(child,FALSE));
                next.hmenuIn=GetSystemMenu(child,FALSE);next.hmenuNext=NULL;next.hwndNext=NULL;
                SendMessageW(child,WM_NEXTMENU,VK_LEFT,(LPARAM)&next);
                REQUIRE(next.hwndNext==w && next.hmenuNext==GetSystemMenu(w,FALSE));
                next.hmenuNext=NULL;next.hwndNext=NULL;
                SendMessageW(child,WM_NEXTMENU,VK_RIGHT,(LPARAM)&next);
                REQUIRE(next.hwndNext==w && next.hmenuNext==GetMenu(w));
                printf("MDI CONTRACT PASS native frame/child WM_NEXTMENU identities in both directions\n");}
            ShowWindow(w,SW_SHOWNOACTIVATE);UpdateWindow(w);SetFocus(child);
            menu_route_parent=w;menu_route_child=child;menu_test_action=11;menu_test_step=0;menu_test_timeout=FALSE;
            REQUIRE(SetTimer(w,97,2000,NULL));
            SendMessageW(w,WM_SYSCOMMAND,SC_KEYMENU,L'w');
            KillTimer(w,97);menu_test_action=0;
            REQUIRE(!menu_test_timeout && menu_test_step==3 && IsWindow(child) && IsWindow(w));
            SendMessageW(w,WM_KEYDOWN,VK_ESCAPE,0);REQUIRE(GetFocus()==child);
            printf("MDI PASS real frame popup -> child system popup -> frame popup -> Escape\n");
            menu_test_action=12;menu_test_step=0;menu_test_timeout=FALSE;
            REQUIRE(SetTimer(w,97,2000,NULL));
            SendMessageW(w,WM_SYSCOMMAND,SC_KEYMENU,L'w');
            KillTimer(w,97);menu_test_action=0;
            REQUIRE(!menu_test_timeout && menu_test_step==5 && IsWindow(child) && IsWindow(w));
            SendMessageW(w,WM_KEYDOWN,VK_ESCAPE,0);REQUIRE(GetFocus()==child);
            printf("MDI PASS child system <-> frame system <-> child system round trip\n");
            SendMessageW(client,WM_MDIMAXIMIZE,(WPARAM)child,0);REQUIRE(IsZoomed(child));
            {MDINEXTMENU next={0};next.hmenuIn=GetMenu(w);
                SendMessageW(w,WM_NEXTMENU,VK_LEFT,(LPARAM)&next);
                REQUIRE(!next.hwndNext && !next.hmenuNext);
                printf("MDI CONTRACT PASS maximized frame does not redirect to separate child menu\n");}
            {win16_style_bar bar;win16_style_layout base;RECT wr;POINT point;MSG msg;
                REQUIRE(GetWindowRect(w,&wr));
                REQUIRE(win16_style_measure(wr.right-wr.left,wr.bottom-wr.top,GetDpiForWindow(w),
                    (DWORD)GetWindowLongPtrW(w,GWL_STYLE),(DWORD)GetWindowLongPtrW(w,GWL_EXSTYLE),FALSE,&base));
                point.x=base.client.left;point.y=base.client.top;
                {int k;for(k=0;k<GetMenuItemCount(GetMenu(w));++k){MENUITEMINFOW mi={sizeof(mi)};
                    mi.fMask=MIIM_FTYPE|MIIM_BITMAP|MIIM_ID|MIIM_SUBMENU;
                    REQUIRE(GetMenuItemInfoW(GetMenu(w),k,TRUE,&mi));
                    fprintf(stderr,"MDI merged item %d type=%x bitmap=%p id=%x submenu=%p\n",k,mi.fType,mi.hbmpItem,mi.wID,mi.hSubMenu);}}
                REQUIRE(win16_style_bar_read(GetMenu(w),(HFONT)GetStockObject(SYSTEM_FONT),GetDpiForWindow(w),
                    point,base.client.right-base.client.left,&bar));
                REQUIRE(bar.count==3 && bar.items[0].control==HBMMENU_SYSTEM &&
                    bar.items[2].control==HBMMENU_MBAR_RESTORE);
                if(menu_capture_path){ShowWindow(w,SW_SHOWNOACTIVATE);
                    SetWindowPos(w,NULL,0,0,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
                    UpdateWindow(w);capture_menu(w,L"mdi-maximized",FALSE);
                    REQUIRE(GetWindowRect(w,&wr));}
                point.x=wr.left+(bar.rects[2].left+bar.rects[2].right)/2;
                point.y=wr.top+(bar.rects[2].top+bar.rects[2].bottom)/2;
                win16_style_bar_free(&bar);
                REQUIRE(SendMessageW(w,WM_NCHITTEST,0,MAKELPARAM(point.x,point.y))==HTMENU);
                SendMessageW(w,WM_NCLBUTTONDOWN,HTMENU,MAKELPARAM(point.x,point.y));
                while(PeekMessageW(&msg,w,WM_COMMAND,WM_COMMAND,PM_REMOVE))DispatchMessageW(&msg);
                REQUIRE(!IsZoomed(child) && !IsZoomed(w));
            }
            GetClientRect(child,&before);p.x=p.y=0;ClientToScreen(child,&p);OffsetRect(&before,p.x,p.y);
            REQUIRE(win16_style_remove(child));
            GetClientRect(child,&after);p.x=p.y=0;ClientToScreen(child,&p);OffsetRect(&after,p.x,p.y);
            REQUIRE(EqualRect(&before,&after));REQUIRE(win16_style_install(child,NULL));
            SendMessageW(client,WM_MDIMAXIMIZE,(WPARAM)second,0);REQUIRE(IsZoomed(second));
            {win16_style_bar bar;win16_style_layout base;RECT wr;POINT point;MSG msg;HMENU sys=GetSystemMenu(second,FALSE);
                REQUIRE(GetWindowRect(w,&wr));
                REQUIRE(win16_style_measure(wr.right-wr.left,wr.bottom-wr.top,GetDpiForWindow(w),
                    (DWORD)GetWindowLongPtrW(w,GWL_STYLE),(DWORD)GetWindowLongPtrW(w,GWL_EXSTYLE),FALSE,&base));
                point.x=base.client.left;point.y=base.client.top;
                REQUIRE(win16_style_bar_read(GetMenu(w),(HFONT)GetStockObject(SYSTEM_FONT),GetDpiForWindow(w),
                    point,base.client.right-base.client.left,&bar));
                REQUIRE(bar.items[0].control==HBMMENU_SYSTEM);
                point.x=wr.left+(bar.rects[0].left+bar.rects[0].right)/2;
                point.y=wr.top+(bar.rects[0].top+bar.rects[0].bottom)/2;win16_style_bar_free(&bar);
                EnableMenuItem(sys,SC_CLOSE,MF_BYCOMMAND|MF_GRAYED);
                SendMessageW(w,WM_NCLBUTTONDBLCLK,HTMENU,MAKELPARAM(point.x,point.y));
                while(PeekMessageW(&msg,w,WM_COMMAND,WM_COMMAND,PM_REMOVE))DispatchMessageW(&msg);
                REQUIRE(IsWindow(second) && IsWindow(w));
                EnableMenuItem(sys,SC_CLOSE,MF_BYCOMMAND|MF_ENABLED);
                menu_test_double_point=point;menu_test_action=4;menu_test_timeout=FALSE;
                REQUIRE(SetTimer(w,97,2000,NULL));
                SendMessageW(w,WM_NCLBUTTONDOWN,HTMENU,MAKELPARAM(point.x,point.y));
                KillTimer(w,97);menu_test_action=0;REQUIRE(!menu_test_timeout);
                while(PeekMessageW(&msg,w,WM_NCLBUTTONDBLCLK,WM_NCLBUTTONDBLCLK,PM_REMOVE))DispatchMessageW(&msg);
                while(PeekMessageW(&msg,w,WM_COMMAND,WM_COMMAND,PM_REMOVE))DispatchMessageW(&msg);
                REQUIRE(!IsWindow(second) && IsWindow(w));
                printf("MDI PASS merged system popup queued double-click closes child only\n");
            }
            REQUIRE((HWND)SendMessageW(client,WM_MDIGETACTIVE,0,0)==child);
            printf("MDI PASS child client preservation, activation, maximize/restore, destroy\n");
        }
    }
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
    int page,i;test_mode=TRUE;menu_capture_path=path;create_controller();
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
            if(samples[i].special==OWNED){
                MENUITEMINFOW before={sizeof(before)},after={sizeof(after)};
                before.fMask=after.fMask=MIIM_FTYPE|MIIM_DATA|MIIM_BITMAP;
                REQUIRE(GetMenuItemInfoW(GetSystemMenu(w,FALSE),SC_CLOSE,FALSE,&before));
                menu_captured=FALSE;menu_test_system=TRUE;menu_test_action=2;menu_test_timeout=FALSE;
                SetActiveWindow(w);SetFocus(w);REQUIRE(SetTimer(w,97,1000,NULL));
                SendMessageW(w,WM_SYSCOMMAND,SC_KEYMENU,L' ');
                KillTimer(w,97);menu_test_action=0;menu_test_system=FALSE;
                REQUIRE(!menu_test_timeout && menu_captured && IsWindow(w));menu_captured=FALSE;
                REQUIRE(GetMenuItemInfoW(GetSystemMenu(w,FALSE),SC_CLOSE,FALSE,&after));
                REQUIRE(before.fType==after.fType && before.dwItemData==after.dwItemData && before.hbmpItem==after.hbmpItem);
                printf("MENU PASS actual dialog system menu, retro capture, Escape and item restoration\n");
            }
            GetWindowRect(w,&r);width=r.right-r.left;height=r.bottom-r.top;
            if(samples[i].retro){win16_style_layout l;POINT p;RECT actual;
                REQUIRE(win16_style_measure(width,height,GetDpiForWindow(w),samples[i].style,samples[i].ex,FALSE,&l));
                p.x=r.left+l.client.left+10;p.y=r.top+l.client.top+10;
                if(samples[i].special==MENU || samples[i].special==MDI){p.x=10;p.y=10;ClientToScreen(w,&p);}
                REQUIRE(SendMessageW(w,WM_NCHITTEST,0,MAKELPARAM(p.x,p.y))==HTCLIENT);
                GetClientRect(w,&actual);REQUIRE(actual.right==l.client.right-l.client.left);
                if(samples[i].special!=MENU && samples[i].special!=MDI)REQUIRE(actual.bottom==l.client.bottom-l.client.top);
                SendMessageW(w,WM_NCACTIVATE,i%2,0);
                if(!(samples[i].style&WS_CAPTION))REQUIRE(IsRectEmpty(&l.caption));
            }else REQUIRE(!win16_style_set_palette(w,NULL));
            if(samples[i].special==MENU){
                UINT command;HMENU popup=GetSubMenu(GetMenu(w),0);
                HMENU nested=GetSubMenu(popup,4);MENUITEMINFOW duplicate={sizeof(duplicate)};
                REQUIRE(menu_mnemonic(popup,L'E')==MAKELRESULT(0,MNC_EXECUTE));
                REQUIRE(menu_mnemonic(popup,L'd')==MAKELRESULT(0,MNC_IGNORE));
                REQUIRE(menu_mnemonic(popup,L'n')==MAKELRESULT(0,MNC_IGNORE));
                REQUIRE(menu_mnemonic(nested,L'e')==MAKELRESULT(0,MNC_IGNORE));
                REQUIRE(menu_mnemonic(nested,L'n')==MAKELRESULT(0,MNC_EXECUTE));
                REQUIRE(EnableMenuItem(popup,100,MF_BYCOMMAND|MF_DISABLED)!=(UINT)-1);
                REQUIRE(menu_mnemonic(popup,L'e')==MAKELRESULT(0,MNC_IGNORE));
                REQUIRE(EnableMenuItem(popup,100,MF_BYCOMMAND|MF_ENABLED)!=(UINT)-1);
                duplicate.fMask=MIIM_FTYPE|MIIM_DATA|MIIM_ID;duplicate.fType=MFT_OWNERDRAW;
                duplicate.dwItemData=0;duplicate.wID=201;
                REQUIRE(InsertMenuItemW(popup,5,TRUE,&duplicate));
                REQUIRE(menu_mnemonic(popup,L'e')==MAKELRESULT(0,MNC_SELECT));
                REQUIRE(HiliteMenuItem(w,popup,0,MF_BYPOSITION|MF_HILITE));
                REQUIRE(menu_mnemonic(popup,L'e')==MAKELRESULT(5,MNC_SELECT));
                REQUIRE(HiliteMenuItem(w,popup,0,MF_BYPOSITION|MF_UNHILITE));
                REQUIRE(HiliteMenuItem(w,popup,5,MF_BYPOSITION|MF_HILITE));
                REQUIRE(menu_mnemonic(popup,L'e')==MAKELRESULT(0,MNC_SELECT));
                REQUIRE(RemoveMenu(popup,5,MF_BYPOSITION));
                printf("MENU PASS mnemonic menu identity, disabled state and duplicate cycling\n");
                menu_measure_count=menu_draw_count=menu_command_count=0;
                SetActiveWindow(w);SetFocus(w);
                REQUIRE(SetTimer(w,97,1000,NULL));menu_test_action=1;menu_test_timeout=FALSE;
                command=(UINT)TrackPopupMenuEx(popup,TPM_RETURNCMD|TPM_NOANIMATION,r.left+8,r.top+35,w,NULL);
                menu_test_action=0;KillTimer(w,97);
                printf("MENU control command=%u measured=%u drawn=%u\n",command,menu_measure_count,menu_draw_count);
                REQUIRE(command==100 && !menu_test_timeout);
                SendMessageW(w,WM_COMMAND,command,0);REQUIRE(menu_command_count==1);
                REQUIRE(!(GetMenuState(popup,100,MF_BYCOMMAND)&MF_CHECKED));
                REQUIRE(menu_measure_count>0 && menu_draw_count>0);
                REQUIRE(SetTimer(w,97,1000,NULL));menu_test_action=2;menu_test_timeout=FALSE;
                command=(UINT)TrackPopupMenuEx(popup,TPM_RETURNCMD|TPM_NOANIMATION,r.left+8,r.top+35,w,NULL);
                menu_test_action=0;KillTimer(w,97);REQUIRE(command==0 && !menu_test_timeout);
                REQUIRE(menu_command_count==1);
                REQUIRE(SetTimer(w,97,1000,NULL));menu_test_action=3;menu_test_step=0;menu_test_timeout=FALSE;
                command=(UINT)TrackPopupMenuEx(popup,TPM_RETURNCMD|TPM_NOANIMATION,r.left+8,r.top+35,w,NULL);
                menu_test_action=0;KillTimer(w,97);REQUIRE(command==200 && !menu_test_timeout);
                SendMessageW(w,WM_COMMAND,command,0);REQUIRE(menu_command_count==2);
                {POINT hit={10,0};RECT outer;win16_style_layout base;
                    GetWindowRect(w,&outer);
                    REQUIRE(win16_style_measure(outer.right-outer.left,outer.bottom-outer.top,
                        GetDpiForWindow(w),samples[i].style,samples[i].ex,FALSE,&base));
                    hit.x=outer.left+base.client.left+10;hit.y=outer.top+base.client.top+2;
                    REQUIRE(SendMessageW(w,WM_NCHITTEST,0,MAKELPARAM(hit.x,hit.y))==HTMENU);
                    REQUIRE(SetTimer(w,97,1000,NULL));menu_test_action=1;menu_test_timeout=FALSE;
                    menu_test_bar=TRUE;
                    SendMessageW(w,WM_NCLBUTTONDOWN,HTMENU,MAKELPARAM(hit.x,hit.y));
                    menu_test_action=0;KillTimer(w,97);
                    menu_test_bar=FALSE;check_bar_selection(w,FALSE);
                    /* Native TrackPopupMenu queues the selected command. */
                    {MSG command_message;while(PeekMessageW(&command_message,w,WM_COMMAND,WM_COMMAND,PM_REMOVE))
                        DispatchMessageW(&command_message);}
                    printf("MENU bar timeout=%d commands=%u\n",menu_test_timeout,menu_command_count);
                    REQUIRE(!menu_test_timeout && menu_command_count==3);
                    REQUIRE(GetMenuState(popup,100,MF_BYCOMMAND)&MF_CHECKED);
                    printf("MENU PASS custom bar hit -> native popup -> actual WM_COMMAND\n");}
                REQUIRE(SetTimer(w,97,1000,NULL));menu_test_action=1;menu_test_timeout=FALSE;
                menu_test_bar=TRUE;
                SendMessageW(w,WM_SYSCOMMAND,SC_KEYMENU,L'f');
                menu_test_action=0;KillTimer(w,97);
                menu_test_bar=FALSE;check_bar_selection(w,FALSE);
                {MSG command_message;while(PeekMessageW(&command_message,w,WM_COMMAND,WM_COMMAND,PM_REMOVE))
                    DispatchMessageW(&command_message);}
                REQUIRE(!menu_test_timeout && menu_command_count==4);
                REQUIRE(!(GetMenuState(popup,100,MF_BYCOMMAND)&MF_CHECKED));
                printf("MENU PASS Alt-letter entry -> native popup -> one command\n");
                {HWND focus=CreateWindowW(L"StyleGallerySample",L"focus witness",WS_CHILD,0,0,10,10,w,NULL,GetModuleHandleW(NULL),(void *)&samples[0]);
                    REQUIRE(focus);SetFocus(focus);REQUIRE(GetFocus()==focus);
                    SendMessageW(w,WM_SYSCOMMAND,SC_KEYMENU,0);REQUIRE(GetFocus()==w);
                    check_bar_selection(w,TRUE);
                    SendMessageW(w,WM_KEYDOWN,VK_RIGHT,0); /* disabled: selected, not executable */
                    SendMessageW(w,WM_KEYDOWN,VK_RETURN,0);REQUIRE(menu_command_count==4);
                    SendMessageW(w,WM_KEYDOWN,VK_RIGHT,0);
                    SendMessageW(w,WM_KEYDOWN,VK_RETURN,0);
                    {MSG command_message;while(PeekMessageW(&command_message,w,WM_COMMAND,WM_COMMAND,PM_REMOVE))
                        DispatchMessageW(&command_message);}
                    REQUIRE(menu_command_count==5 && GetFocus()==focus);check_bar_selection(w,FALSE);
                    menu_test_action=5;menu_test_step=0;menu_test_timeout=FALSE;menu_boundary_system=FALSE;
                    REQUIRE(SetTimer(w,97,2000,NULL));
                    SendMessageW(w,WM_SYSCOMMAND,SC_KEYMENU,L'f');
                    KillTimer(w,97);menu_test_action=0;
                    {MSG command_message;while(PeekMessageW(&command_message,w,WM_COMMAND,WM_COMMAND,PM_REMOVE))
                        DispatchMessageW(&command_message);}
                    REQUIRE(!menu_test_timeout && menu_test_step>=3 && menu_boundary_system && menu_command_count==6);
                    REQUIRE(GetFocus()==focus);
                    printf("MENU PASS open File -> system menu -> Extra popup -> command\n");
                    menu_test_action=6;menu_test_step=0;menu_test_timeout=FALSE;menu_boundary_system=FALSE;
                    REQUIRE(SetTimer(w,97,2000,NULL));
                    SendMessageW(w,WM_SYSCOMMAND,SC_KEYMENU,L'e');
                    KillTimer(w,97);menu_test_action=0;
                    {MSG command_message;while(PeekMessageW(&command_message,w,WM_COMMAND,WM_COMMAND,PM_REMOVE))
                        DispatchMessageW(&command_message);}
                    REQUIRE(!menu_test_timeout && menu_test_step>=3 && menu_boundary_system && menu_command_count==7);
                    REQUIRE(GetFocus()==focus);
                    menu_test_action=7;menu_test_step=0;menu_test_timeout=FALSE;menu_boundary_system=FALSE;
                    REQUIRE(SetTimer(w,97,2000,NULL));
                    SendMessageW(w,WM_SYSCOMMAND,SC_KEYMENU,L'f');
                    KillTimer(w,97);menu_test_action=0;
                    REQUIRE(!menu_test_timeout && menu_boundary_system && menu_command_count==7);
                    SendMessageW(w,WM_KEYDOWN,VK_ESCAPE,0);REQUIRE(GetFocus()==focus);
                    printf("MENU PASS reverse system boundary and Escape restores focus without command\n");
                    {win16_style_bar bar;win16_style_layout base;RECT outer;POINT origin;int target;
                        HFONT font=CreateFontW(16,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
                            OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,DEFAULT_PITCH,L"System");
                        REQUIRE(font);
                        REQUIRE(GetWindowRect(w,&outer));
                        REQUIRE(win16_style_measure(outer.right-outer.left,outer.bottom-outer.top,GetDpiForWindow(w),
                            (DWORD)GetWindowLongPtrW(w,GWL_STYLE),(DWORD)GetWindowLongPtrW(w,GWL_EXSTYLE),FALSE,&base));
                        origin.x=base.client.left;origin.y=base.client.top;
                        REQUIRE(win16_style_bar_read(GetMenu(w),font,GetDpiForWindow(w),
                            origin,base.client.right-base.client.left,&bar));
                        DeleteObject(font);
                        for(target=1;target<=3;++target){
                            menu_test_double_point.x=outer.left+(bar.rects[target].left+bar.rects[target].right)/2;
                            menu_test_double_point.y=outer.top+(bar.rects[target].top+bar.rects[target].bottom)/2;
                            menu_test_action=target==3?8:9;menu_test_step=0;menu_test_timeout=FALSE;
                            REQUIRE(SetTimer(w,97,2000,NULL));
                            SendMessageW(w,WM_SYSCOMMAND,SC_KEYMENU,L'f');
                            KillTimer(w,97);menu_test_action=0;
                            {MSG command_message;while(PeekMessageW(&command_message,w,WM_COMMAND,WM_COMMAND,PM_REMOVE))
                                DispatchMessageW(&command_message);}
                            fprintf(stderr,"HOVER target=%d point=%ld,%ld timeout=%d steps=%u commands=%u\n",target,
                                menu_test_double_point.x,menu_test_double_point.y,menu_test_timeout,menu_test_step,menu_command_count);
                            REQUIRE(!menu_test_timeout && menu_command_count==(target==3?8u:7u));
                            if(target<3)SendMessageW(w,WM_KEYDOWN,VK_ESCAPE,0);
                            REQUIRE(GetFocus()==focus);
                        }
                        win16_style_bar_free(&bar);
                        printf("MENU PASS hover switches popup; disabled/direct bar items do not execute\n");}
                    menu_test_action=10;menu_test_step=0;menu_test_timeout=FALSE;
                    REQUIRE(SetTimer(w,97,2000,NULL));
                    SendMessageW(w,WM_SYSCOMMAND,SC_KEYMENU,L'e');
                    KillTimer(w,97);menu_test_action=0;
                    {MSG command_message;MENUITEMINFOW info={sizeof(info)};
                        while(PeekMessageW(&command_message,w,WM_COMMAND,WM_COMMAND,PM_REMOVE))DispatchMessageW(&command_message);
                        info.fMask=MIIM_FTYPE;
                        REQUIRE(GetMenuItemInfoW(GetSubMenu(GetMenu(w),3),0,TRUE,&info) && !(info.fType&MFT_OWNERDRAW));}
                    REQUIRE(!menu_test_timeout && menu_test_step==2 && menu_command_count==9 && GetFocus()==focus);
                    printf("MENU PASS real recursive popup restores inner only then resumes outer command\n");
                    SendMessageW(w,WM_SYSCOMMAND,SC_KEYMENU,0);check_bar_selection(w,TRUE);
                    SendMessageW(w,WM_KEYDOWN,VK_ESCAPE,0);REQUIRE(GetFocus()==focus);check_bar_selection(w,FALSE);
                    SendMessageW(w,WM_SYSCOMMAND,SC_KEYMENU,0);REQUIRE(GetFocus()==w);
                    REQUIRE(win16_style_remove(w));REQUIRE(GetFocus()==focus);
                    REQUIRE(win16_style_install(w,NULL));
                    REQUIRE(DestroyWindow(focus));
                    printf("MENU PASS F10 entry, arrows, disabled refusal, Enter, Escape and focus return\n");}
                printf("MENU PASS owner draw, check toggle, nested selection and Escape (no-animation control)\n");
            }
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
