/* Selectable standalone HWND gallery. Native references are explicitly not
 * retro-renderer support. Captures use only this process's owned windows. */
#include "win16-style/win16_style.h"
#include "win16-style/layout.h"
#include "win16-style/menu_paint.h"
#include "win16-style/menu_bar.h"
#include "win16-style/controls.h"
#include "win16-style/font.h"
#include "win16-style/scroll_glyphs.h"
#include "win16-style/scroll_layout.h"
#include <stdio.h>
#include <stdlib.h>
#include <windowsx.h>
#include <commctrl.h>
#include <oleacc.h>

typedef struct sample {
    const WCHAR *name;
    DWORD style,ex;
    BOOL retro;
    int special;
} sample;
enum {NORMAL,OWNED,MENU,SCROLL,CHILD,MDI,ICONIC,MAXIMIZED,CHECKS,BASICS,INPUTS,LISTS,COMBOS,SCROLLCONTROLS,AUTOCONTROLS,SHOWCASE,MULTILIST};
static const sample samples[]={
    {L"01 Win3.x controls showcase",WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_HSCROLL|WS_VSCROLL,0,TRUE,SHOWCASE},
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
    {L"16 Window scrollbars [trial]",WS_OVERLAPPEDWINDOW|WS_HSCROLL|WS_VSCROLL,0,TRUE,SCROLL},
    {L"17 Child [NATIVE reference]",WS_OVERLAPPEDWINDOW,0,FALSE,CHILD},
    {L"18 MDI frames and merged menus",WS_OVERLAPPEDWINDOW,0,TRUE,MDI},
    {L"19 Ordinary [NATIVE reference]",WS_OVERLAPPEDWINDOW,0,FALSE,NORMAL},
    {L"20 Checkbox / Radio [trial]",WS_OVERLAPPEDWINDOW,0,TRUE,CHECKS},
    {L"21 Buttons / Labels / Boxes [trial]",WS_OVERLAPPEDWINDOW,0,TRUE,BASICS},
    {L"22 Edit / Password / Multiline [trial]",WS_OVERLAPPEDWINDOW,0,TRUE,INPUTS},
    {L"23 List selection [trial]",WS_OVERLAPPEDWINDOW,0,TRUE,LISTS},
    {L"24 Combobox forms [trial]",WS_OVERLAPPEDWINDOW,0,TRUE,COMBOS},
    {L"25 Scrollbar controls [trial]",WS_OVERLAPPEDWINDOW,0,TRUE,SCROLLCONTROLS},
    {L"26 Automatic controls [trial]",WS_OVERLAPPEDWINDOW,0,TRUE,AUTOCONTROLS},
    {L"27 Multicolumn list [trial]",WS_OVERLAPPEDWINDOW,0,TRUE,MULTILIST}
};
#define COUNT ((int)(sizeof(samples)/sizeof(samples[0])))
#define REQUIRE(x) do{if(!(x)){fprintf(stderr,"GALLERY FAIL line %d: %s error=%lu\n",__LINE__,#x,GetLastError());exit(1);}}while(0)
static HWND controller,listbox,opened[COUNT];
static HFONT gallery_font;
static BOOL test_mode;
static BOOL scroll_test_mode;
static const WCHAR *menu_labels[]={L"&Example command\tCtrl+E",L"&Disabled command",L"",L"&Close\tAlt+F4",L"&More",L"&Nested command",L"&File"};
#define MENU_LABEL_COUNT (sizeof(menu_labels)/sizeof(menu_labels[0]))
static win16_style_menu_kind menu_kind(ULONG_PTR data)
{
    return data==2?WIN16_STYLE_MENU_SEPARATOR:data==6?WIN16_STYLE_MENU_BAR:WIN16_STYLE_MENU_ROW;
}
static unsigned menu_measure_count,menu_draw_count,menu_command_count;
static unsigned control_commands;
static int menu_test_action;
static BOOL menu_test_timeout;
static BOOL menu_test_mouse;
static unsigned menu_test_step;
static const WCHAR *menu_capture_path;
static BOOL menu_captured;
static ULONGLONG outer_menu_started,outer_menu_capture_ms;
static unsigned outer_menu_idle_count,outer_menu_init_count,outer_menu_exit_count;
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

static void print_dc_state(const char *phase,HDC dc)
{
    RECT clip;POINT origin={0,0};BITMAP bm={0};int kind=GetClipBox(dc,&clip);
    LPtoDP(dc,&origin,1);GetObjectW(GetCurrentObject(dc,OBJ_BITMAP),sizeof(bm),&bm);
    printf("PRINT DC %s clip=%d:%ld,%ld,%ld,%ld origin=%ld,%ld bitmap=%ldx%ld\n",
        phase,kind,clip.left,clip.top,clip.right,clip.bottom,origin.x,origin.y,bm.bmWidth,bm.bmHeight);
}
static LRESULT CALLBACK print_observer(HWND w,UINT msg,WPARAM wp,LPARAM lp,UINT_PTR id,DWORD_PTR data)
{
    LRESULT result;(void)id;(void)data;
    if(msg==WM_PRINT || msg==WM_PRINTCLIENT){printf("PRINT MESSAGE %04x flags=%Ix\n",msg,(UINT_PTR)lp);print_dc_state("enter",(HDC)wp);}
    if(msg==WM_PRINTCLIENT){RECT outer;POINT client={0,0},actual={0,0};
        REQUIRE(GetWindowRect(w,&outer) && ClientToScreen(w,&client) && LPtoDP((HDC)wp,&actual,1));
        REQUIRE(actual.x==client.x-outer.left && actual.y==client.y-outer.top);}
    result=DefSubclassProc(w,msg,wp,lp);
    if(msg==WM_PRINT || msg==WM_PRINTCLIENT)print_dc_state("leave",(HDC)wp);
    return result;
}
static void capture_menu(HWND popup,const WCHAR *name,BOOL check_border)
{
    RECT r;BITMAPINFO bi={0};BITMAPFILEHEADER fh={0};HDC dc;HBITMAP bm,old;
    void *pixels;DWORD bytes;HANDLE file;WCHAR path[MAX_PATH];ULONGLONG started=GetTickCount64();
    REQUIRE(GetWindowRect(popup,&r));
    dc=CreateCompatibleDC(NULL);REQUIRE(dc);
    bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bi.bmiHeader.biWidth=r.right-r.left;
    bi.bmiHeader.biHeight=r.top-r.bottom;bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;
    bm=CreateDIBSection(dc,&bi,DIB_RGB_COLORS,&pixels,NULL,0);REQUIRE(bm);old=SelectObject(dc,bm);
    /* Private-desktop print, not a physical-screen first-paint observation. */
    if(!wcsncmp(name,L"all-controls",12)){
        RECT full={0,0,r.right-r.left,r.bottom-r.top};
        FillRect(dc,&full,(HBRUSH)GetStockObject(WHITE_BRUSH));
        REQUIRE(SetWindowSubclass(popup,print_observer,(UINT_PTR)print_observer,0));print_dc_state("before",dc);
    }
    if(!lstrcmpW(name,L"all-controls-screen")){
        HDC screen=GetWindowDC(popup);REQUIRE(screen);
        REQUIRE(BitBlt(dc,0,0,r.right-r.left,r.bottom-r.top,screen,0,0,SRCCOPY));ReleaseDC(popup,screen);
    }
    else if(!lstrcmpW(name,L"all-controls-children"))
        SendMessageW(popup,WM_PRINT,(WPARAM)dc,PRF_CHILDREN);
    else if(!lstrcmpW(name,L"all-controls-message"))
        SendMessageW(popup,WM_PRINT,(WPARAM)dc,PRF_NONCLIENT|PRF_CLIENT|PRF_CHILDREN|PRF_ERASEBKGND);
    else REQUIRE(PrintWindow(popup,dc,0));GdiFlush();
    if(!wcsncmp(name,L"all-controls",12)){
        print_dc_state("after",dc);RemoveWindowSubclass(popup,print_observer,(UINT_PTR)print_observer);
    }
    if(!lstrcmpW(name,L"all-controls-message")){
        POINT client={0,0};RECT area;int x,y,width=r.right-r.left;DWORD *p=(DWORD *)pixels;
        REQUIRE(ClientToScreen(popup,&client) && GetClientRect(popup,&area));
        client.x-=r.left;client.y-=r.top;
        /* This demo deliberately leaves a blank band before its controls.
         * Catch native menu text leaking below the custom bar, and an
         * unpainted bottom band from an incorrect client origin. */
        for(y=0;y<10;++y)for(x=4;x<150;++x)
            REQUIRE((p[(client.y+y)*width+client.x+x]&0xffffff)==0xffffff);
        for(x=4;x<150;++x)
            REQUIRE((p[(client.y+area.bottom-1)*width+client.x+x]&0xffffff)==0xffffff);
        printf("PRINT PIXELS PASS client origin, blank top/bottom bands and no native menu residue\n");
    }
    /* Verify the actual scoped popup painter, not only the paint primitive. */
    if(check_border){int x,width=r.right-r.left,height=r.bottom-r.top;DWORD *p=(DWORD *)pixels;
        for(x=0;x<width;++x){REQUIRE((p[x]&0xffffff)==0);
            REQUIRE((p[(height-1)*width+x]&0xffffff)==0);}}
    REQUIRE(swprintf_s(path,MAX_PATH,L"%ls-%ls-popup.bmp",menu_capture_path,name)>0);
    file=CreateFileW(path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);REQUIRE(file!=INVALID_HANDLE_VALUE);
    fh.bfType=0x4d42;fh.bfOffBits=sizeof(fh)+sizeof(bi.bmiHeader);
    fh.bfSize=fh.bfOffBits+(r.right-r.left)*(r.bottom-r.top)*4;
    /* BI_RGB's high byte is not alpha. Normalize only the serialized image
     * after RGB assertions so viewers do not hide GDI-rendered pixels. */
    {DWORD index,count=(fh.bfSize-fh.bfOffBits)/4;DWORD *p=(DWORD *)pixels;
        for(index=0;index<count;++index)p[index]|=0xff000000;}
    REQUIRE(WriteFile(file,&fh,sizeof(fh),&bytes,NULL));
    REQUIRE(WriteFile(file,&bi.bmiHeader,sizeof(bi.bmiHeader),&bytes,NULL));
    REQUIRE(WriteFile(file,pixels,fh.bfSize-fh.bfOffBits,&bytes,NULL));CloseHandle(file);
    SelectObject(dc,old);DeleteObject(bm);DeleteDC(dc);menu_captured=TRUE;
    if(menu_test_action==13)outer_menu_capture_ms=GetTickCount64()-started;
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
    if(s && s->special==SHOWCASE && m==DM_GETDEFID)return MAKELRESULT(1202,DC_HASDEFID);
    if(s && s->special>=CHECKS && m==WM_COMMAND && LOWORD(a)==IDCANCEL && !b){SendMessageW(w,WM_CLOSE,0,0);return 0;}
    if(m==WM_NCDESTROY){
        HICON icon=(HICON)RemovePropW(w,L"ShowcaseIcon");HBITMAP bitmap=(HBITMAP)RemovePropW(w,L"ShowcaseBitmap");
        if(icon)DestroyIcon(icon);if(bitmap)DeleteObject(bitmap);
    }
    if(m==WM_NCCREATE){s=(const sample *)((CREATESTRUCTW *)b)->lpCreateParams;
        SetWindowLongPtrW(w,GWLP_USERDATA,(LONG_PTR)s);}
    if(menu_test_action>=5 && m==WM_INITMENUPOPUP && (HMENU)a==GetSystemMenu(w,FALSE))
        menu_boundary_system=TRUE;
    if(menu_test_action==13 && m==WM_INITMENUPOPUP)++outer_menu_init_count;
    if(menu_test_action==13 && m==WM_EXITMENULOOP)++outer_menu_exit_count;
    if(s && menu_test_action==13 && m==WM_ENTERIDLE && a==MSGF_MENU){
        HMENU menu=GetSystemMenu(w,FALSE);int i,count=GetMenuItemCount(menu);WCHAR name[40];
        ++outer_menu_idle_count;
        for(i=0;i<count;++i){MENUITEMINFOW info={sizeof(info)};
            info.fMask=MIIM_FTYPE|MIIM_DATA|MIIM_ID;
            REQUIRE(GetMenuItemInfoW(menu,(UINT)i,TRUE,&info));
            printf("OUTER MENU sample=%d item=%d id=%u type=%x adapted=%d\n",
                menu_test_step,i,info.wID,info.fType,(info.fType&MFT_OWNERDRAW)!=0);
        }
        REQUIRE(swprintf_s(name,40,L"outer-system-%02d-%ls",menu_test_step,menu_test_mouse?L"mouse":L"key")>0);
        if(!menu_captured)capture_menu((HWND)b,name,TRUE);
        PostMessageW(w,WM_KEYDOWN,VK_ESCAPE,0);return 0;
    }
    if(s && menu_test_action==13 && m==WM_TIMER && a==97){menu_test_timeout=TRUE;EndMenu();return 0;}
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
    if(s && s->special==BASICS && m==WM_COMMAND && LOWORD(a)==600 && HIWORD(a)==BN_CLICKED){++control_commands;return 0;}
    if(s && s->special==LISTS && m==WM_COMMAND && HIWORD(a)==LBN_SELCHANGE){++control_commands;return 0;}
    if(s && (s->special==SCROLL || s->special==SHOWCASE) && (m==WM_HSCROLL || m==WM_VSCROLL) && !b){
        int bar=m==WM_HSCROLL?SB_HORZ:SB_VERT;SCROLLINFO info={sizeof(info),SIF_ALL};
        REQUIRE(win16_style_get_scroll_info(w,bar,&info));
        switch(LOWORD(a)){
        case SB_LINEUP:--info.nPos;break;case SB_LINEDOWN:++info.nPos;break;
        case SB_PAGEUP:info.nPos-=(int)info.nPage;break;case SB_PAGEDOWN:info.nPos+=(int)info.nPage;break;
        case SB_TOP:info.nPos=info.nMin;break;case SB_BOTTOM:info.nPos=info.nMax;break;
        case SB_THUMBTRACK:case SB_THUMBPOSITION:info.nPos=info.nTrackPos;break;
        default:return 0;}
        ++control_commands;info.fMask=SIF_POS;win16_style_set_scroll_info(w,bar,&info,TRUE);return 0;
    }
    if(s && (s->special==SCROLLCONTROLS || s->special==SHOWCASE) && (m==WM_HSCROLL || m==WM_VSCROLL) && b){
        SCROLLINFO info={sizeof(info),SIF_ALL};REQUIRE(GetScrollInfo((HWND)b,SB_CTL,&info));
        switch(LOWORD(a)){
        case SB_LINEUP:--info.nPos;break;case SB_LINEDOWN:++info.nPos;break;
        case SB_PAGEUP:info.nPos-=(int)info.nPage;break;case SB_PAGEDOWN:info.nPos+=(int)info.nPage;break;
        case SB_TOP:info.nPos=info.nMin;break;case SB_BOTTOM:info.nPos=info.nMax;break;
        case SB_THUMBTRACK:case SB_THUMBPOSITION:info.nPos=info.nTrackPos;break;
        default:return 0;}
        ++control_commands;info.fMask=SIF_POS;SetScrollInfo((HWND)b,SB_CTL,&info,TRUE);return 0;
    }
    if(m==WM_PAINT || m==WM_PRINTCLIENT){
        PAINTSTRUCT ps;RECT r;HDC dc=m==WM_PAINT?BeginPaint(w,&ps):(HDC)a;int saved=SaveDC(dc);
        if(s && s->special==MDI){if(saved)RestoreDC(dc,saved);if(m==WM_PAINT)EndPaint(w,&ps);return 0;}
        GetClientRect(w,&r);FillRect(dc,&r,(HBRUSH)GetStockObject(WHITE_BRUSH));
        if(s && (s->special==CHECKS || s->special==BASICS || s->special==INPUTS || s->special==LISTS || s->special==COMBOS || s->special==SCROLLCONTROLS || s->special==AUTOCONTROLS || s->special==SHOWCASE || s->special==MULTILIST)){if(saved)RestoreDC(dc,saved);if(m==WM_PAINT)EndPaint(w,&ps);return 0;}
        SelectObject(dc,GetStockObject(DEFAULT_GUI_FONT));SetBkMode(dc,TRANSPARENT);
        r.left+=14;r.top+=14;
        if(s)DrawTextW(dc,s->name,-1,&r,DT_LEFT|DT_SINGLELINE);
        r.top+=28;
        DrawTextW(dc,L"F2: title   F3: colour   Esc: close\n"
            L"Activate/deactivate by clicking another window.\n"
            L"System box: click menu, double-click close.\n"
            L"Application client area is not restyled.",-1,&r,DT_LEFT);
        if(saved)RestoreDC(dc,saved);if(m==WM_PAINT)EndPaint(w,&ps);return 0;
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

/* One review surface, built through the same future-control attachment API
 * as an application. Detailed pages below retain their independent tests. */
static LRESULT CALLBACK count_control_paints(HWND w,UINT m,WPARAM a,LPARAM b,UINT_PTR id,DWORD_PTR data)
{
    (void)id;if(m==WM_PAINT)++*(unsigned *)data;
    return DefSubclassProc(w,m,a,b);
}
static void test_idle_control_damage(HWND w)
{
    const int ids[]={1200,1201,1218};unsigned paints[3]={0};int n,k;
    HDC dc=CreateCompatibleDC(NULL);HBITMAP bitmap,old;BITMAPINFO info={0};void *pixels;
    COLORREF interior;RECT fill={0,0,278,260};
    for(n=0;n<3;++n){HWND control=GetDlgItem(w,ids[n]);REQUIRE(control);
        ValidateRect(control,NULL);REQUIRE(SetWindowSubclass(control,count_control_paints,(UINT_PTR)count_control_paints,(DWORD_PTR)&paints[n]));
        for(k=0;k<100;++k)SendMessageW(control,WM_MOUSEMOVE,0,MAKELPARAM(8+k%20,10));
        UpdateWindow(control);REQUIRE(RemoveWindowSubclass(control,count_control_paints,(UINT_PTR)count_control_paints));
    }
    info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=278;info.bmiHeader.biHeight=-260;
    info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;
    bitmap=CreateDIBSection(dc,&info,DIB_RGB_COLORS,&pixels,NULL,0);REQUIRE(bitmap);old=SelectObject(dc,bitmap);
    SetDCBrushColor(dc,RGB(37,83,119));FillRect(dc,&fill,(HBRUSH)GetStockObject(DC_BRUSH));
    SendMessageW(GetDlgItem(w,1200),WM_PRINTCLIENT,(WPARAM)dc,PRF_CLIENT);
    interior=GetPixel(dc,100,120);SelectObject(dc,old);DeleteObject(bitmap);DeleteDC(dc);
    printf("IDLE DAMAGE group=%u button=%u list=%u per 100 moves; group interior=%06lx expected=%06lx\n",
        paints[0],paints[1],paints[2],(unsigned long)interior,(unsigned long)RGB(37,83,119));fflush(stdout);
    REQUIRE(!paints[0] && !paints[1] && !paints[2]);REQUIRE(interior==RGB(37,83,119));
}

static HICON fixture_icon(BOOL alternate)
{
    BYTE mask[32],image[32];int row;
    /* Authored monochrome test image: a framed cross, not extracted artwork. */
    for(row=0;row<16;++row){
        mask[row*2]=mask[row*2+1]=0;
        image[row*2]=image[row*2+1]=(BYTE)((row==0 || row==15 || row==7 || row==8)?0:0x7e);
        if(alternate){image[row*2]^=0xff;image[row*2+1]^=0xff;}
    }
    return CreateIcon(GetModuleHandleW(NULL),16,16,1,1,mask,image);
}

static void test_static_icon(HWND parent)
{
    unsigned variant;
    for(variant=0;variant<8;++variant){
        DWORD flags=SS_ICON|((variant&1)?SS_CENTERIMAGE:0)|((variant&2)?WS_DISABLED:0);
        HWND w=CreateWindowW(L"STATIC",NULL,WS_CHILD|flags,0,0,40,36,parent,NULL,GetModuleHandleW(NULL),NULL);
        HICON first=fixture_icon(FALSE),second=fixture_icon(TRUE);int state,pixel;
        HDC expected=CreateCompatibleDC(NULL),actual=CreateCompatibleDC(NULL);BITMAPINFO bi={0};
        HBITMAP bm1,bm2,old1,old2;DWORD *want,*got;RECT r={0,0,40,36};LONG_PTR style;
        int width=(variant&4)?8:40,height=(variant&4)?6:36,saved;
        win16_style_palette palette;ICONINFO icon_info;
        REQUIRE(w && first && second && expected && actual);
        REQUIRE(!SendMessageW(w,STM_SETICON,(WPARAM)first,0));
        REQUIRE(SetWindowPos(w,NULL,0,0,width,height,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE));
        style=GetWindowLongPtrW(w,GWL_STYLE);REQUIRE(win16_style_control_install(w,NULL));
        bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bi.bmiHeader.biWidth=40;bi.bmiHeader.biHeight=-36;
        bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;
        bm1=CreateDIBSection(expected,&bi,DIB_RGB_COLORS,(void **)&want,NULL,0);
        bm2=CreateDIBSection(actual,&bi,DIB_RGB_COLORS,(void **)&got,NULL,0);REQUIRE(bm1 && bm2);
        old1=SelectObject(expected,bm1);old2=SelectObject(actual,bm2);win16_style_default_palette(&palette);
        for(state=0;state<3;++state){
            HICON icon=state==0?first:state==1?second:NULL;
            if(state)REQUIRE((HICON)SendMessageW(w,STM_SETICON,(WPARAM)icon,0)==(state==1?first:second));
            REQUIRE((HICON)SendMessageW(w,STM_GETICON,0,0)==icon);
            REQUIRE(SetWindowPos(w,NULL,0,0,width,height,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE));
            SetRect(&r,0,0,40,36);
            SetDCBrushColor(expected,RGB(21,43,65));FillRect(expected,&r,(HBRUSH)GetStockObject(DC_BRUSH));
            SetDCBrushColor(actual,RGB(21,43,65));FillRect(actual,&r,(HBRUSH)GetStockObject(DC_BRUSH));
            GetClientRect(w,&r);REQUIRE(r.right==width && r.bottom==height);
            saved=SaveDC(expected);REQUIRE(saved);IntersectClipRect(expected,0,0,width,height);
            SetDCBrushColor(expected,palette.light);FillRect(expected,&r,(HBRUSH)GetStockObject(DC_BRUSH));
            if(icon)REQUIRE(DrawIconEx(expected,(variant&1)?(width-16)/2:0,(variant&1)?(height-16)/2:0,
                icon,(variant&1)?16:width,(variant&1)?16:height,0,NULL,DI_NORMAL));
            RestoreDC(expected,saved);
            SendMessageW(w,WM_PRINTCLIENT,(WPARAM)actual,PRF_CLIENT);GdiFlush();
            for(pixel=0;pixel<40*36;++pixel)REQUIRE((want[pixel]&0xffffff)==(got[pixel]&0xffffff));
        }
        SendMessageW(w,STM_SETICON,(WPARAM)first,0);
        REQUIRE(win16_style_control_remove(w));REQUIRE(GetWindowLongPtrW(w,GWL_STYLE)==style);
        REQUIRE((HICON)SendMessageW(w,STM_GETICON,0,0)==first);REQUIRE(DestroyWindow(w));
        REQUIRE(GetIconInfo(first,&icon_info));
        if(icon_info.hbmColor)DeleteObject(icon_info.hbmColor);DeleteObject(icon_info.hbmMask);
        REQUIRE(DestroyIcon(first));REQUIRE(DestroyIcon(second));
        SelectObject(expected,old1);SelectObject(actual,old2);DeleteObject(bm1);DeleteObject(bm2);
        DeleteDC(expected);DeleteDC(actual);
        printf("STATIC ICON PASS variant=%u clipped pixels, replacement, clearing and borrowed handle lifetime\n",variant);
    }
}

static HBITMAP fixture_bitmap(BOOL alternate)
{
    BITMAPINFO info={0};BYTE *pixels;HBITMAP image;int x,y;
    info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=16;info.bmiHeader.biHeight=-16;
    info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=24;
    image=CreateDIBSection(NULL,&info,DIB_RGB_COLORS,(void **)&pixels,NULL,0);REQUIRE(image);
    for(y=0;y<16;++y)for(x=0;x<16;++x){
        BYTE *pixel=pixels+(y*16+x)*3;BOOL dark=((x/4+y/4)&1)!=alternate;
        pixel[0]=dark?128:192;pixel[1]=dark?0:192;pixel[2]=dark?0:192;
    }
    return image;
}

static void test_static_bitmap(HWND parent)
{
    unsigned variant;
    for(variant=0;variant<8;++variant){
        DWORD flags=SS_BITMAP|((variant&1)?SS_CENTERIMAGE:0)|((variant&2)?WS_DISABLED:0);
        HWND w=CreateWindowW(L"STATIC",NULL,WS_CHILD|flags,0,0,40,36,parent,NULL,GetModuleHandleW(NULL),NULL);
        HBITMAP first=fixture_bitmap(FALSE),second=fixture_bitmap(TRUE),bm1,bm2,old1,old2;
        HDC expected=CreateCompatibleDC(NULL),actual=CreateCompatibleDC(NULL),source=CreateCompatibleDC(NULL);
        BITMAPINFO info={0};BITMAP bits;DWORD *want,*got;LONG_PTR style;
        int width=(variant&4)?8:40,height=(variant&4)?6:36,state,pixel;
        REQUIRE(w && expected && actual && source);style=GetWindowLongPtrW(w,GWL_STYLE);
        REQUIRE(win16_style_control_install(w,NULL));
        info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=40;info.bmiHeader.biHeight=-36;
        info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;
        bm1=CreateDIBSection(expected,&info,DIB_RGB_COLORS,(void **)&want,NULL,0);
        bm2=CreateDIBSection(actual,&info,DIB_RGB_COLORS,(void **)&got,NULL,0);REQUIRE(bm1 && bm2);
        old1=SelectObject(expected,bm1);old2=SelectObject(actual,bm2);
        for(state=0;state<3;++state){
            HBITMAP image=state==0?first:state==1?second:NULL,prior;RECT r={0,0,40,36};int saved;
            REQUIRE((HBITMAP)SendMessageW(w,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)image)==(state==0?NULL:state==1?first:second));
            REQUIRE((HBITMAP)SendMessageW(w,STM_GETIMAGE,IMAGE_BITMAP,0)==image);
            REQUIRE(SetWindowPos(w,NULL,0,0,width,height,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE));
            SetDCBrushColor(expected,RGB(21,43,65));FillRect(expected,&r,(HBRUSH)GetStockObject(DC_BRUSH));
            SetDCBrushColor(actual,RGB(21,43,65));FillRect(actual,&r,(HBRUSH)GetStockObject(DC_BRUSH));
            saved=SaveDC(expected);REQUIRE(saved);IntersectClipRect(expected,0,0,width,height);
            if(image){
                prior=SelectObject(source,image);REQUIRE(prior && prior!=HGDI_ERROR);
                if(variant&1){SetRect(&r,0,0,width,height);SetDCBrushColor(expected,GetPixel(source,0,0));
                    FillRect(expected,&r,(HBRUSH)GetStockObject(DC_BRUSH));}
                REQUIRE(StretchBlt(expected,(variant&1)?(width-16)/2:0,(variant&1)?(height-16)/2:0,
                    (variant&1)?16:width,(variant&1)?16:height,source,0,0,16,16,SRCCOPY));
                SelectObject(source,prior);
            }
            RestoreDC(expected,saved);
            {BOOL before=PtVisible(actual,width/2,height/2),after;
                SendMessageW(w,WM_PRINTCLIENT,(WPARAM)actual,PRF_CLIENT);GdiFlush();after=PtVisible(actual,width/2,height/2);
                printf("BITMAP DC variant=%u state=%d center-visible=%d -> %d\n",variant,state,before,after);fflush(stdout);
                REQUIRE(before==after);}
            for(pixel=0;pixel<40*36;++pixel)if((want[pixel]&0xffffff)!=(got[pixel]&0xffffff)){
                printf("BITMAP MISMATCH variant=%u state=%d xy=%d,%d expected=%06lx actual=%06lx\n",
                    variant,state,pixel%40,pixel/40,want[pixel]&0xffffff,got[pixel]&0xffffff);fflush(stdout);REQUIRE(FALSE);}
        }
        SendMessageW(w,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)first);
        REQUIRE(win16_style_control_remove(w));REQUIRE(GetWindowLongPtrW(w,GWL_STYLE)==style);
        REQUIRE((HBITMAP)SendMessageW(w,STM_GETIMAGE,IMAGE_BITMAP,0)==first);REQUIRE(DestroyWindow(w));
        REQUIRE(GetObjectW(first,sizeof(bits),&bits)==sizeof(bits));REQUIRE(DeleteObject(first));REQUIRE(DeleteObject(second));
        SelectObject(expected,old1);SelectObject(actual,old2);DeleteObject(bm1);DeleteObject(bm2);
        DeleteDC(expected);DeleteDC(actual);DeleteDC(source);
        printf("STATIC BITMAP PASS variant=%u native pixels, replacement, clearing and borrowed lifetime\n",variant);
    }
}

static void test_static_text(HWND parent)
{
    const struct {DWORD style;UINT flags;const WCHAR *text;} cases[]={
        {SS_LEFTNOWORDWRAP,DT_LEFT|DT_EXPANDTABS,L"First\tcolumn\r\nSecond line"},
        {SS_LEFT,DT_LEFT|DT_EXPANDTABS|DT_WORDBREAK,L"First\tcolumn and more words than one row can hold"},
        {SS_CENTER,DT_CENTER|DT_EXPANDTABS|DT_WORDBREAK,L"First\tcolumn\r\nSecond line"},
        {SS_RIGHT,DT_RIGHT|DT_EXPANDTABS|DT_WORDBREAK,L"First\tcolumn\r\nSecond line"},
        {SS_SIMPLE,DT_LEFT|DT_SINGLELINE,L"&Simple label"},
        {SS_SIMPLE|SS_NOPREFIX,DT_LEFT|DT_SINGLELINE|DT_NOPREFIX,L"&Simple label"}
    };
    unsigned n;
    for(n=0;n<sizeof(cases)/sizeof(cases[0]);++n){
        HWND w=CreateWindowW(L"STATIC",cases[n].text,WS_CHILD|cases[n].style,0,0,220,64,parent,NULL,GetModuleHandleW(NULL),NULL);
        HDC expected=CreateCompatibleDC(NULL),actual=CreateCompatibleDC(NULL);BITMAPINFO bi={0};
        HBITMAP bm1,bm2,old1,old2;DWORD *want,*got;HFONT font=(HFONT)GetStockObject(SYSTEM_FONT);
        RECT r={0,0,220,64};win16_style_palette palette;LONG_PTR style;int pixel;WCHAR text[100];
        REQUIRE(w && expected && actual);style=GetWindowLongPtrW(w,GWL_STYLE);
        SendMessageW(w,WM_SETFONT,(WPARAM)font,FALSE);REQUIRE(win16_style_control_install(w,NULL));
        bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bi.bmiHeader.biWidth=220;bi.bmiHeader.biHeight=-64;
        bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;
        bm1=CreateDIBSection(expected,&bi,DIB_RGB_COLORS,(void **)&want,NULL,0);
        bm2=CreateDIBSection(actual,&bi,DIB_RGB_COLORS,(void **)&got,NULL,0);REQUIRE(bm1 && bm2);
        old1=SelectObject(expected,bm1);old2=SelectObject(actual,bm2);win16_style_default_palette(&palette);
        SetDCBrushColor(expected,palette.light);FillRect(expected,&r,(HBRUSH)GetStockObject(DC_BRUSH));
        SelectObject(expected,(HFONT)SendMessageW(w,WM_GETFONT,0,0));SetBkMode(expected,TRANSPARENT);SetTextColor(expected,palette.edge);
        DrawTextW(expected,cases[n].text,-1,&r,cases[n].flags);
        SendMessageW(w,WM_PRINTCLIENT,(WPARAM)actual,PRF_CLIENT);GdiFlush();
        for(pixel=0;pixel<220*64;++pixel)REQUIRE((want[pixel]&0xffffff)==(got[pixel]&0xffffff));
        SelectObject(expected,GetStockObject(SYSTEM_FONT));
        REQUIRE(win16_style_control_remove(w));REQUIRE(GetWindowLongPtrW(w,GWL_STYLE)==style);
        REQUIRE(GetWindowTextW(w,text,100)>0 && !lstrcmpW(text,cases[n].text));
        REQUIRE((HFONT)SendMessageW(w,WM_GETFONT,0,0)==font);
        SelectObject(expected,old1);SelectObject(actual,old2);DeleteObject(bm1);DeleteObject(bm2);
        DeleteDC(expected);DeleteDC(actual);DestroyWindow(w);
        printf("STATIC TEXT PASS variant=%u original formatting flags, pixels and native state preserved\n",n);
    }
}

static void test_disabled_scroll_pixels(HWND w)
{
    BOOL vertical=(GetWindowLongPtrW(w,GWL_STYLE)&SBS_VERT)!=0;SCROLLBARINFO bar={sizeof(bar)};
    RECT r;HDC dc=CreateCompatibleDC(NULL);HBITMAP bitmap,old;BITMAPINFO info={0};void *pixels;
    win16_style_palette palette;COLORREF colors[4];int end,x,y;
    REQUIRE(GetScrollBarInfo(w,OBJID_CLIENT,&bar));GetClientRect(w,&r);
    REQUIRE((bar.rgstate[1]&STATE_SYSTEM_UNAVAILABLE) && (bar.rgstate[5]&STATE_SYSTEM_UNAVAILABLE));
    win16_style_default_palette(&palette);colors[0]=palette.edge;colors[1]=palette.light;
    colors[2]=palette.shadow;colors[3]=palette.face;
    info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=r.right;info.bmiHeader.biHeight=-r.bottom;
    info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;
    bitmap=CreateDIBSection(dc,&info,DIB_RGB_COLORS,&pixels,NULL,0);REQUIRE(bitmap);old=SelectObject(dc,bitmap);
    SendMessageW(w,WM_PRINTCLIENT,(WPARAM)dc,PRF_CLIENT);
    for(end=0;end<2;++end){
        const char (*bits)[18]=vertical?(end?scroll_dnarrowi:scroll_uparrowi):(end?scroll_rgarrowi:scroll_lfarrowi);
        int arrow=MulDiv(17,(int)GetDpiForWindow(w),96);
        int width=vertical?r.right:arrow,height=vertical?arrow:r.bottom;
        int left=(!vertical && end)?r.right-width:0,top=(vertical && end)?r.bottom-height:0;
        for(y=0;y<17;++y)for(x=0;x<17;++x){
            int px=left+(MulDiv(x,width,17)+MulDiv(x+1,width,17)-1)/2;
            int py=top+(MulDiv(y,height,17)+MulDiv(y+1,height,17)-1)/2;
            REQUIRE(GetPixel(dc,px,py)==colors[bits[y][x]-'0']);
        }
    }
    SelectObject(dc,old);DeleteObject(bitmap);DeleteDC(dc);
    printf("SCROLL OEM PASS disabled %s arrows: 578 cell samples, original border and emboss\n",vertical?"vertical":"horizontal");
}

typedef struct scroll_input_trace {
    HWND child;UINT notices[32];unsigned count;LONG position;BOOL remove;
} scroll_input_trace;
static LRESULT CALLBACK record_scroll_input(HWND w,UINT m,WPARAM a,LPARAM b,UINT_PTR id,DWORD_PTR data)
{
    scroll_input_trace *trace=(scroll_input_trace *)data;(void)id;
    if((m==WM_HSCROLL || m==WM_VSCROLL) && (HWND)b==trace->child){
        SCROLLINFO info={sizeof(info),SIF_ALL};UINT code=LOWORD(a);
        REQUIRE(trace->count<32);trace->notices[trace->count++]=code;
        REQUIRE(GetScrollInfo(trace->child,SB_CTL,&info));
        if(code==SB_THUMBTRACK || code==SB_THUMBPOSITION){
            trace->position=info.nTrackPos;info.nPos=info.nTrackPos;
            if(trace->remove){REQUIRE(win16_style_control_remove(trace->child));return 0;}
        }else if(code==SB_LINEDOWN)++info.nPos;
        else if(code==SB_LINEUP)--info.nPos;
        info.fMask=SIF_POS;SetScrollInfo(trace->child,SB_CTL,&info,TRUE);return 0;
    }
    return DefSubclassProc(w,m,a,b);
}
static void test_scroll_input(HWND scroll)
{
    HWND parent=GetParent(scroll);RECT r;POINT p;int dpi=(int)GetDpiForWindow(scroll);
    SCROLLINFO original={sizeof(original),SIF_ALL},info={sizeof(info),SIF_RANGE|SIF_PAGE|SIF_POS,0,200000,20,70000,0};
    win16_scroll_metrics metrics={MulDiv(17,dpi,96),MulDiv(17,dpi,96),max(1,MulDiv(1,dpi,96))};
    win16_scroll_layout layout;scroll_input_trace trace={0};BOOL vertical=(GetWindowLongPtrW(scroll,GWL_STYLE)&SBS_VERT)!=0;
    REQUIRE(GetScrollInfo(scroll,SB_CTL,&original));trace.child=scroll;
    REQUIRE(SetWindowSubclass(parent,record_scroll_input,(UINT_PTR)record_scroll_input,(DWORD_PTR)&trace));
    SetScrollInfo(scroll,SB_CTL,&info,TRUE);GetClientRect(scroll,&r);
    p.x=vertical?r.right/2:r.right-4;p.y=vertical?r.bottom-4:r.bottom/2;
    SendMessageW(scroll,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(p.x,p.y));
    REQUIRE(GetCapture()==scroll && GetScrollPos(scroll,SB_CTL)==70001);
    SendMessageW(scroll,WM_LBUTTONUP,0,MAKELPARAM(p.x,p.y));
    REQUIRE(GetCapture()!=scroll && trace.count==2 && trace.notices[0]==SB_LINEDOWN && trace.notices[1]==SB_ENDSCROLL);
    REQUIRE(win16_scroll_measure(&r,vertical,&metrics,0,199981,70001,&layout));
    p.x=(layout.thumb.left+layout.thumb.right)/2;p.y=(layout.thumb.top+layout.thumb.bottom)/2;
    trace.count=0;SendMessageW(scroll,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(p.x,p.y));
    REQUIRE(GetCapture()==scroll && trace.count==1 && trace.notices[0]==SB_THUMBTRACK && trace.position==70001);
    if(vertical)p.y+=20;else p.x+=20;
    SendMessageW(scroll,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(p.x,p.y));
    REQUIRE(trace.position>70001 && GetScrollPos(scroll,SB_CTL)==trace.position);
    SendMessageW(scroll,WM_LBUTTONUP,0,MAKELPARAM(p.x,p.y));
    REQUIRE(GetCapture()!=scroll && trace.count==4 && trace.notices[2]==SB_THUMBPOSITION && trace.notices[3]==SB_ENDSCROLL);
    info.nPos=70000;SetScrollInfo(scroll,SB_CTL,&info,TRUE);
    REQUIRE(win16_scroll_measure(&r,vertical,&metrics,0,199981,70000,&layout));
    p.x=(layout.thumb.left+layout.thumb.right)/2;p.y=(layout.thumb.top+layout.thumb.bottom)/2;
    trace.count=0;trace.remove=TRUE;
    SendMessageW(scroll,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(p.x,p.y));
    REQUIRE(trace.count==3 && trace.notices[0]==SB_THUMBTRACK &&
        trace.notices[1]==SB_THUMBPOSITION && trace.notices[2]==SB_ENDSCROLL);
    REQUIRE(trace.position==70000 && GetCapture()!=scroll && IsWindow(scroll));
    REQUIRE(RemoveWindowSubclass(parent,record_scroll_input,(UINT_PTR)record_scroll_input));
    REQUIRE(win16_style_control_install(scroll,NULL));original.fMask=SIF_RANGE|SIF_PAGE|SIF_POS;
    SetScrollInfo(scroll,SB_CTL,&original,TRUE);
    printf("SCROLL INPUT PASS %s: arrow capture, fixed thumb drag, 32-bit track position, final notices, callback removal\n",vertical?"vertical":"horizontal");
}

static void check_scroll_caret(HWND scroll,BOOL tracking)
{
    GUITHREADINFO gui={sizeof(gui)};SCROLLINFO info={sizeof(info),SIF_ALL};RECT r,expected;
    int dpi=(int)GetDpiForWindow(scroll),inset=max(1,MulDiv(2,dpi,96));
    win16_scroll_metrics metrics={MulDiv(17,dpi,96),MulDiv(17,dpi,96),max(1,MulDiv(1,dpi,96))};
    win16_scroll_layout layout;BOOL vertical=(GetWindowLongPtrW(scroll,GWL_STYLE)&SBS_VERT)!=0;
    GetClientRect(scroll,&r);REQUIRE(GetScrollInfo(scroll,SB_CTL,&info));
    REQUIRE(win16_scroll_measure(&r,vertical,&metrics,info.nMin,info.nMax-(int)info.nPage+1,info.nPos,&layout));
    expected=layout.thumb;InflateRect(&expected,-inset,-inset);
    REQUIRE(GetGUIThreadInfo(GetCurrentThreadId(),&gui));REQUIRE(gui.hwndCaret==scroll);
    REQUIRE(EqualRect(&expected,&gui.rcCaret));
    if(tracking)REQUIRE(!(gui.flags&GUI_CARETBLINKING));
}
static void test_scroll_focus(HWND parent)
{
    int vertical;HWND scroll;scroll_input_trace trace;GUITHREADINFO gui={sizeof(gui)};
    SCROLLINFO info={sizeof(info),SIF_RANGE|SIF_PAGE|SIF_POS,0,100,20,30,0};
    for(vertical=0;vertical<2;++vertical){POINT old_position,new_position;
        ZeroMemory(&trace,sizeof(trace));
        scroll=CreateWindowW(L"SCROLLBAR",L"",WS_CHILD|WS_VISIBLE|WS_TABSTOP|(vertical?SBS_VERT:0),
            16,16,vertical?26:240,vertical?240:26,parent,(HMENU)1742,GetModuleHandleW(NULL),NULL);REQUIRE(scroll);
        trace.child=scroll;SetScrollInfo(scroll,SB_CTL,&info,TRUE);
        REQUIRE(win16_style_control_install(scroll,NULL));
        REQUIRE(SetWindowSubclass(parent,record_scroll_input,(UINT_PTR)record_scroll_input,(DWORD_PTR)&trace));
        SetFocus(scroll);REQUIRE(GetFocus()==scroll);check_scroll_caret(scroll,FALSE);REQUIRE(GetCaretPos(&old_position));
        SendMessageW(scroll,WM_KEYDOWN,vertical?VK_DOWN:VK_RIGHT,0);
        SendMessageW(scroll,WM_KEYUP,vertical?VK_DOWN:VK_RIGHT,0);
        check_scroll_caret(scroll,FALSE);
        SetScrollPos(scroll,SB_CTL,65,TRUE);check_scroll_caret(scroll,FALSE);REQUIRE(GetCaretPos(&new_position));
        REQUIRE(vertical?new_position.y>old_position.y:new_position.x>old_position.x);
        SendMessageW(scroll,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(vertical?13:235,vertical?235:13));
        REQUIRE(GetCapture()==scroll);check_scroll_caret(scroll,TRUE);
        SendMessageW(scroll,WM_LBUTTONUP,0,0);check_scroll_caret(scroll,FALSE);
        SetFocus(parent);REQUIRE(GetGUIThreadInfo(GetCurrentThreadId(),&gui));REQUIRE(gui.hwndCaret!=scroll);
        REQUIRE(RemoveWindowSubclass(parent,record_scroll_input,(UINT_PTR)record_scroll_input));
        REQUIRE(win16_style_control_remove(scroll));DestroyWindow(scroll);
    }
    puts("SCROLL FOCUS PASS both axes: fixed-thumb caret geometry, key/programmatic movement, hidden during capture, focus cleanup");
}

/* Read the visible window DC after each input message: WM_PRINT would itself
 * repaint the retro image and hide a native nonclient-paint regression. */
static HBITMAP read_window_pixels(HWND w,DWORD **pixels,int *width)
{
    RECT r;BITMAPINFO info={0};HDC window=GetWindowDC(w),memory;HBITMAP bitmap,old;
    REQUIRE(window);GetWindowRect(w,&r);*width=r.right-r.left;
    info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=*width;
    info.bmiHeader.biHeight=-(r.bottom-r.top);info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;
    memory=CreateCompatibleDC(window);REQUIRE(memory);
    bitmap=CreateDIBSection(window,&info,DIB_RGB_COLORS,(void **)pixels,NULL,0);REQUIRE(bitmap);
    old=SelectObject(memory,bitmap);REQUIRE(BitBlt(memory,0,0,*width,r.bottom-r.top,window,0,0,SRCCOPY));GdiFlush();
    SelectObject(memory,old);DeleteDC(memory);ReleaseDC(w,window);return bitmap;
}
static COLORREF pixel_color(DWORD pixel)
{return RGB((pixel>>16)&255,(pixel>>8)&255,pixel&255);}

static unsigned visible_list_arrow_errors(HWND list)
{
    SCROLLBARINFO bar={sizeof(bar)};RECT outer;win16_style_palette palette;
    COLORREF colors[4];HBITMAP bitmap;DWORD *pixels;int image_width,end,x,y,width,height;unsigned errors=0;
    REQUIRE(GetScrollBarInfo(list,OBJID_VSCROLL,&bar));GetWindowRect(list,&outer);
    REQUIRE(IsWindowVisible(list));
    REQUIRE(!(bar.rgstate[0]&STATE_SYSTEM_INVISIBLE));
    width=bar.rcScrollBar.right-bar.rcScrollBar.left;height=MulDiv(17,(int)GetDpiForWindow(list),96);
    /* USER18:07AA shortens both arrows before laying out a short bar. A
     * second axis can reduce its length below two full arrows plus borders. */
    height=min(height,(bar.rcScrollBar.bottom-bar.rcScrollBar.top)/2-max(1,MulDiv(1,(int)GetDpiForWindow(list),96)));
    REQUIRE(width>0 && height>0);
    win16_style_default_palette(&palette);colors[0]=palette.edge;colors[1]=palette.light;
    colors[2]=palette.shadow;colors[3]=palette.face;bitmap=read_window_pixels(list,&pixels,&image_width);
    for(end=0;end<2;++end){
        unsigned before_errors=errors;
        LONG state=bar.rgstate[end?5:1];
        const char (*bits)[18]=(state&STATE_SYSTEM_UNAVAILABLE)?(end?scroll_dnarrowi:scroll_uparrowi):
            (state&STATE_SYSTEM_PRESSED)?(end?scroll_dnarrowd:scroll_uparrowd):(end?scroll_dnarrow:scroll_uparrow);
        int left=bar.rcScrollBar.left-outer.left;
        int top=(end?bar.rcScrollBar.bottom-height:bar.rcScrollBar.top)-outer.top;
        for(y=0;y<17;++y)for(x=0;x<17;++x){
            int px=left+(MulDiv(x,width,17)+MulDiv(x+1,width,17)-1)/2;
            int py=top+(MulDiv(y,height,17)+MulDiv(y+1,height,17)-1)/2;
            if(pixel_color(pixels[py*image_width+px])!=colors[bits[y][x]-'0'])++errors;
        }
        if(errors!=before_errors)printf("ARROW PIXEL DIFFERENCE end=%d errors=%u rect=%ld,%ld,%ld,%ld metric=%d width=%d state=%lx\n",
            end,errors-before_errors,bar.rcScrollBar.left,bar.rcScrollBar.top,bar.rcScrollBar.right,bar.rcScrollBar.bottom,height,width,state);
    }
    DeleteObject(bitmap);return errors;
}

static unsigned visible_bar_errors(HWND list,LONG object,unsigned *foreign)
{
    SCROLLBARINFO bar={sizeof(bar)};RECT wr,visible;HDC screen,expected,actual;HBITMAP bitmap,old,capture,old_capture;
    BITMAPINFO info={0};DWORD *wanted,*observed;
    unsigned errors=0;int x,y,width,height;win16_style_palette palette;
    if(foreign)*foreign=0;win16_style_default_palette(&palette);
    REQUIRE(IsWindowVisible(list));REQUIRE(GetScrollBarInfo(list,object,&bar));GetWindowRect(list,&wr);
    width=wr.right-wr.left;height=wr.bottom-wr.top;
    visible=bar.rcScrollBar;
    {HWND child=list;
        /* Native simple-combo integral rows may extend its child list below
         * the parent's client. Pixels outside ancestor clipping are not a
         * visible surface. Compare every visible bar pixel, not inaccessible
         * rows against a memory DC which has no native ancestor clipping. */
        while(GetWindowLongPtrW(child,GWL_STYLE)&WS_CHILD){RECT parent;
            child=GetParent(child);REQUIRE(child && GetClientRect(child,&parent));
            MapWindowPoints(child,NULL,(POINT *)&parent,2);
            REQUIRE(IntersectRect(&visible,&visible,&parent));}
        OffsetRect(&visible,-wr.left,-wr.top);}
    screen=GetWindowDC(list);REQUIRE(screen);expected=CreateCompatibleDC(screen);REQUIRE(expected);
    actual=CreateCompatibleDC(screen);REQUIRE(actual);
    info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=width;info.bmiHeader.biHeight=-height;
    info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;
    bitmap=CreateDIBSection(screen,&info,DIB_RGB_COLORS,(void **)&wanted,NULL,0);REQUIRE(bitmap);
    capture=CreateDIBSection(screen,&info,DIB_RGB_COLORS,(void **)&observed,NULL,0);REQUIRE(capture);
    old=SelectObject(expected,bitmap);old_capture=SelectObject(actual,capture);
    /* One existing-surface read, not thousands of synchronous GetPixel calls.
     * This still never asks the control to paint itself for the observation. */
    REQUIRE(BitBlt(actual,0,0,width,height,screen,0,0,SRCCOPY));
    win16_style_scrollbars_paint(list,expected,NULL);
    GdiFlush();
    for(y=bar.rcScrollBar.top-wr.top;y<bar.rcScrollBar.bottom-wr.top;++y)
        for(x=bar.rcScrollBar.left-wr.left;x<bar.rcScrollBar.right-wr.left;++x){
            if(x<visible.left || x>=visible.right || y<visible.top || y>=visible.bottom)continue;
            DWORD pixel=observed[y*width+x]&0xffffff;
            COLORREF color=RGB((pixel>>16)&255,(pixel>>8)&255,pixel&255);
            if(pixel!=(wanted[y*width+x]&0xffffff)){
                if(!errors)fprintf(stderr,"BAR FIRST MISMATCH local=%d,%d actual=%06lx expected=%06lx\n",
                    x,y,(unsigned long)pixel,(unsigned long)(wanted[y*width+x]&0xffffff));
                ++errors;}
            if(foreign && color!=palette.edge && color!=palette.light && color!=palette.face && color!=palette.shadow)++*foreign;
        }
    SelectObject(actual,old_capture);DeleteObject(capture);DeleteDC(actual);
    SelectObject(expected,old);DeleteObject(bitmap);DeleteDC(expected);ReleaseDC(list,screen);return errors;
}

static unsigned visible_list_bar_errors(HWND list,unsigned *foreign)
{return visible_bar_errors(list,(GetWindowLongPtrW(list,GWL_STYLE)&LBS_MULTICOLUMN)?OBJID_HSCROLL:OBJID_VSCROLL,foreign);}

static DWORD visible_bar_hash(HWND list,LONG object)
{
    SCROLLBARINFO bar={sizeof(bar)};RECT outer;DWORD hash=2166136261u,*pixels;
    HBITMAP bitmap;int width,x,y;
    REQUIRE(GetScrollBarInfo(list,object,&bar));
    REQUIRE(GetWindowRect(list,&outer));
    bitmap=read_window_pixels(list,&pixels,&width);
    for(y=bar.rcScrollBar.top-outer.top;y<bar.rcScrollBar.bottom-outer.top;++y)
        for(x=bar.rcScrollBar.left-outer.left;x<bar.rcScrollBar.right-outer.left;++x)
            hash=(hash^(pixels[y*width+x]&0xffffff))*16777619u;
    DeleteObject(bitmap);return hash;
}

static DWORD visible_list_bar_hash(HWND list)
{return visible_bar_hash(list,(GetWindowLongPtrW(list,GWL_STYLE)&LBS_MULTICOLUMN)?OBJID_HSCROLL:OBJID_VSCROLL);}

static unsigned native_tracking_samples;
static BOOL native_tracking_active;
static int native_tracking_part;
static void CALLBACK sample_native_tracking(HWND w,UINT message,UINT_PTR timer,DWORD time)
{
    unsigned foreign,errors;
    (void)message;(void)time;
    errors=visible_list_bar_errors(w,&foreign);++native_tracking_samples;
    printf("NATIVE TRACK LOOP part=%d samples=%u capture=%d errors=%u foreign=%u\n",
        native_tracking_part,native_tracking_samples,GetCapture()==w,errors,foreign);fflush(stdout);
    if(native_tracking_samples==1){
        if(native_tracking_part){
            SCROLLBARINFO bar={sizeof(bar)};POINT move;
            REQUIRE(GetScrollBarInfo(w,OBJID_VSCROLL,&bar));
            move.x=(bar.rcScrollBar.left+bar.rcScrollBar.right)/2;
            move.y=min(bar.rcScrollBar.bottom-bar.dxyLineButton-1,
                bar.rcScrollBar.top+(bar.xyThumbTop+bar.xyThumbBottom)/2+25);
            REQUIRE(ScreenToClient(w,&move));
            REQUIRE(PostMessageW(w,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(move.x,move.y)));
        }
        return;
    }
    KillTimer(w,timer);
    /* End only this diagnostic control's tracking. No physical input or
     * current-desktop cursor manipulation is performed. */
    SendMessageW(w,WM_CANCELMODE,0,0);
    if(GetCapture()==w)ReleaseCapture();
}

static void test_native_tracking_boundary(HWND list,int mode,int part)
{
    SCROLLBARINFO bar={sizeof(bar)};BYTE keyboard[256],pressed[256];UINT_PTR timer;
    REQUIRE(GetScrollBarInfo(list,OBJID_VSCROLL,&bar));
    REQUIRE(GetKeyboardState(keyboard));CopyMemory(pressed,keyboard,sizeof(pressed));
    pressed[VK_LBUTTON]|=0x80;REQUIRE(SetKeyboardState(pressed));
    native_tracking_samples=0;native_tracking_part=part;native_tracking_active=TRUE;
    timer=SetTimer(list,0,100,sample_native_tracking);REQUIRE(timer);
    if(mode&1)SendMessageW(list,WM_SETREDRAW,FALSE,0);
    SendMessageW(list,WM_NCLBUTTONDOWN,HTVSCROLL,
        MAKELPARAM((bar.rcScrollBar.left+bar.rcScrollBar.right)/2,part?
            bar.rcScrollBar.top+(bar.xyThumbTop+bar.xyThumbBottom)/2:
            bar.rcScrollBar.bottom-bar.dxyLineButton/2));
    if(mode&1)SendMessageW(list,WM_SETREDRAW,TRUE,0);
    KillTimer(list,timer);REQUIRE(SetKeyboardState(keyboard));
    native_tracking_active=FALSE;
    printf("NATIVE TRACK BOUNDARY mode=%d part=%d loopSamples=%u; synthetic queue state, not physical held-input acceptance\n",
        mode,part,native_tracking_samples);fflush(stdout);
    RedrawWindow(list,NULL,NULL,RDW_INVALIDATE|RDW_FRAME|RDW_UPDATENOW);
}

static LRESULT CALLBACK native_list_pixels(HWND w,UINT m,WPARAM a,LPARAM b,UINT_PTR id,DWORD_PTR data)
{
    LRESULT result;BOOL observe=IsWindowVisible(w) && (m==WM_MOUSEWHEEL || m==WM_VSCROLL || m==WM_MOUSEMOVE ||
        ((data&4) && (m==LB_SETTOPINDEX || m==LB_SETCURSEL || m==LB_ADDSTRING || m==LB_DELETESTRING)));
    SCROLLINFO before={sizeof(before),SIF_ALL},after={sizeof(after),SIF_ALL};
    DWORD pixels_before=0;
    LONG_PTR style=GetWindowLongPtrW(w,GWL_STYLE);(void)id;
    if(observe)REQUIRE(GetScrollInfo(w,SB_VERT,&before));
    if((data&1) && observe && !native_tracking_active)DefSubclassProc(w,WM_SETREDRAW,FALSE,0);
    if((data==3 || (data&4)) && observe)pixels_before=visible_list_bar_hash(w);
    result=DefSubclassProc(w,m,a,b);
    if(observe){
        unsigned foreign,errors;
        REQUIRE(GetWindowLongPtrW(w,GWL_STYLE)==style);
        errors=visible_list_bar_errors(w,&foreign);
        REQUIRE(GetScrollInfo(w,SB_VERT,&after));
        printf("SCROLL STATE mode=%u before=%d/%u/%d after=%d/%u/%d (pos/page/max)\n",(unsigned)data,
            before.nPos,before.nPage,before.nMax,after.nPos,after.nPage,after.nMax);
        printf("OPEN SCROLL DEFECT mode=%u message=%04x top=%ld barErrors=%u foreign=%u BEFORE redraw restore\n",(unsigned)data,m,
            (long)SendMessageW(w,LB_GETTOPINDEX,0,0),errors,foreign);fflush(stdout);
        if((data==3 || (data&4)) && !native_tracking_active){
            REQUIRE(!foreign);
            REQUIRE(visible_list_bar_hash(w)==pixels_before);
            printf("SCROLL SUPPRESSION surface fingerprint unchanged\n");
        }
        if(data&4){
            RECT client;LRESULT count=SendMessageW(w,LB_GETCOUNT,0,0);
            LRESULT height=SendMessageW(w,LB_GETITEMHEIGHT,0,0),top=SendMessageW(w,LB_GETTOPINDEX,0,0);
            SCROLLINFO sync={sizeof(sync),SIF_RANGE|SIF_PAGE|SIF_POS},readback={sizeof(readback),SIF_ALL};
            HDC dc;DWORD position_pixels,all_pixels;SCROLLINFO position={sizeof(position),SIF_POS};
            REQUIRE(count>0 && height>0 && top>=0);REQUIRE(GetClientRect(w,&client));
            sync.nMax=(int)count-1;sync.nPage=(UINT)(client.bottom/height);sync.nPos=(int)top;
            position.nPos=(int)top;SetScrollInfo(w,SB_VERT,&position,FALSE);
            position_pixels=visible_list_bar_hash(w);
            errors=visible_list_bar_errors(w,&foreign);
            printf("EXTERNAL POSITION ONLY top=%ld surfaceChanged=%d errors=%u foreign=%u\n",
                (LONG)top,position_pixels!=pixels_before,errors,foreign);
            SetScrollInfo(w,SB_VERT,&sync,FALSE);
            REQUIRE(GetScrollInfo(w,SB_VERT,&readback));
            REQUIRE(readback.nPos==top && readback.nMax==count-1 && readback.nPage==sync.nPage);
            /* State must update without an intermediate native repaint. The
             * old surface is deliberately sampled before painting the new
             * position. An expected-image comparison alone would miss this. */
            all_pixels=visible_list_bar_hash(w);errors=visible_list_bar_errors(w,&foreign);
            printf("EXTERNAL ALL FIELDS top=%ld page=%u surfaceChanged=%d changedAfterPosition=%d errors=%u foreign=%u\n",
                (LONG)top,sync.nPage,all_pixels!=pixels_before,all_pixels!=position_pixels,errors,foreign);
            if(data==6){REQUIRE(position_pixels==pixels_before);REQUIRE(all_pixels==pixels_before);}
            dc=GetWindowDC(w);REQUIRE(dc);win16_style_scrollbars_paint(w,dc,NULL);ReleaseDC(w,dc);
            REQUIRE(!visible_list_bar_errors(w,&foreign) && !foreign);
            printf("EXTERNAL SCROLL SYNC top=%ld pos=%d page=%u count=%ld; repaint observations above, not acceptance\n",
                (LONG)top,readback.nPos,readback.nPage,(LONG)count);
        }
        if((data&1) && !native_tracking_active){
            DefSubclassProc(w,WM_SETREDRAW,TRUE,0);errors=visible_list_bar_errors(w,&foreign);
            printf("REDRAW RESTORE OBSERVATION barErrors=%u foreign=%u (not intermediate acceptance)\n",errors,foreign);
        }
    }
    return result;
}

static void test_native_list_paint_boundary(HWND parent)
{
    typedef HRESULT (WINAPI *set_window_theme_fn)(HWND,LPCWSTR,LPCWSTR);
    HMODULE theme=LoadLibraryExW(L"uxtheme.dll",NULL,LOAD_LIBRARY_SEARCH_SYSTEM32);
    set_window_theme_fn set_theme;
    HWND list;int n,mode,case_index;static const int list_modes[]={0,1,2,3,4,6};
    REQUIRE(theme);
    set_theme=(set_window_theme_fn)(void *)GetProcAddress(theme,"SetWindowTheme");REQUIRE(set_theme);
    /* Diagnostic-only per-window theme control: no global setting, no warm-up
     * scroll. Every mode starts with its own new native list and same range. */
    for(case_index=0;case_index<6;++case_index){mode=list_modes[case_index];
    list=CreateWindowW(L"LISTBOX",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|((mode&4)?0:WS_VSCROLL),
        16,16,200,160,parent,(HMENU)1500,GetModuleHandleW(NULL),NULL);
    REQUIRE(list);
    if(mode&2)REQUIRE(SUCCEEDED(set_theme(list,L"",L"")));
    for(n=0;n<32;++n)SendMessageW(list,LB_ADDSTRING,0,(LPARAM)L"Scrolling boundary item");
    if(mode&4){SCROLLINFO info={sizeof(info),SIF_RANGE|SIF_PAGE|SIF_POS,0,31,8,0,0};
        SetScrollInfo(list,SB_VERT,&info,FALSE);}
    REQUIRE(SetWindowSubclass(list,native_list_pixels,(UINT_PTR)native_list_pixels,(DWORD_PTR)mode));
    REQUIRE(win16_style_control_install(list,NULL));
    RedrawWindow(list,NULL,NULL,RDW_INVALIDATE|RDW_FRAME|RDW_UPDATENOW);
    REQUIRE(!visible_list_bar_errors(list,NULL));
    for(n=0;n<3;++n){
        SendMessageW(list,WM_MOUSEWHEEL,MAKEWPARAM(0,(WORD)-WHEEL_DELTA),0);
        printf("STYLED LIST RETURN top=%ld barErrors=%u\n",(long)SendMessageW(list,LB_GETTOPINDEX,0,0),visible_list_bar_errors(list,NULL));
        REQUIRE(!visible_list_bar_errors(list,NULL));
    }
    if(mode&4){
        SendMessageW(list,LB_SETTOPINDEX,2,0);SendMessageW(list,LB_SETCURSEL,20,0);
        SendMessageW(list,LB_ADDSTRING,0,(LPARAM)L"Added externally owned bar item");
        SendMessageW(list,LB_DELETESTRING,0,0);
    }
    if(mode==0 || mode==3){test_native_tracking_boundary(list,mode,0);test_native_tracking_boundary(list,mode,1);}
    REQUIRE(DestroyWindow(list));
    }
    {int kind;static const int modes[]={0,3,4,6};
        for(kind=0;kind<3;++kind)for(case_index=0;case_index<4;++case_index){
            HWND combo;mode=modes[case_index];
            combo=CreateWindowW(L"COMBOBOX",L"",WS_CHILD|WS_VISIBLE|((mode&4)?0:WS_VSCROLL)|
                (kind==0?CBS_DROPDOWNLIST:kind==1?CBS_DROPDOWN:CBS_SIMPLE),
                16,16,200,160,parent,(HMENU)(INT_PTR)(1501+kind),GetModuleHandleW(NULL),NULL);
            COMBOBOXINFO info={sizeof(info)};REQUIRE(combo);REQUIRE(GetComboBoxInfo(combo,&info));
            if(mode&2)REQUIRE(SUCCEEDED(set_theme(info.hwndList,L"",L"")));
            for(n=0;n<32;++n)SendMessageW(combo,CB_ADDSTRING,0,(LPARAM)L"Scrolling dropdown item");
            if(mode&4){SCROLLINFO scroll={sizeof(scroll),SIF_RANGE|SIF_PAGE|SIF_POS,0,31,8,0,0};
                SetScrollInfo(info.hwndList,SB_VERT,&scroll,FALSE);}
            REQUIRE(SetWindowSubclass(info.hwndList,native_list_pixels,(UINT_PTR)native_list_pixels,(DWORD_PTR)mode));
            REQUIRE(win16_style_control_install(combo,NULL));SendMessageW(combo,CB_SHOWDROPDOWN,TRUE,0);
            RedrawWindow(info.hwndList,NULL,NULL,RDW_INVALIDATE|RDW_FRAME|RDW_UPDATENOW);
            for(n=0;n<3;++n){
                SendMessageW(info.hwndList,WM_MOUSEWHEEL,MAKEWPARAM(0,(WORD)-WHEEL_DELTA),0);
                REQUIRE(!visible_list_bar_errors(info.hwndList,NULL));
            }
            for(n=0;n<4;++n){
                SendMessageW(info.hwndList,WM_MOUSEMOVE,0,MAKELPARAM(8,8+n*12));
                REQUIRE(!visible_list_bar_errors(info.hwndList,NULL));
            }
            printf("COMBO SCROLL OBSERVATION kind=%d mode=%d wheel and row motion sampled\n",kind,mode);
            SendMessageW(combo,CB_SHOWDROPDOWN,FALSE,0);
            REQUIRE(DestroyWindow(combo));
        }
    }
    FreeLibrary(theme);
}

static LRESULT CALLBACK integrated_list_boundary(HWND w,UINT m,WPARAM a,LPARAM b,UINT_PTR id,DWORD_PTR data)
{
    BOOL observe=IsWindowVisible(w) && (m==WM_MOUSEWHEEL || m==WM_MOUSEMOVE || m==WM_VSCROLL || m==WM_HSCROLL ||
        m==LB_SETTOPINDEX || m==LB_SETCURSEL || m==LB_ADDSTRING || m==LB_DELETESTRING);
    LONG object=m==WM_HSCROLL?OBJID_HSCROLL:
        (GetWindowLongPtrW(w,GWL_STYLE)&LBS_MULTICOLUMN)?OBJID_HSCROLL:OBJID_VSCROLL;
    DWORD before=observe?visible_bar_hash(w,object):0;LRESULT result;(void)id;
    result=DefSubclassProc(w,m,a,b);
    if(observe && IsWindowVisible(w)){
        DWORD after=visible_bar_hash(w,object);
        printf("INTEGRATED NATIVE BOUNDARY message=%04x surfaceChanged=%d\n",m,after!=before);fflush(stdout);
        REQUIRE(after==before);++*(unsigned *)data;
    }
    return result;
}
static LRESULT CALLBACK combo_capture_trace(HWND w,UINT m,WPARAM a,LPARAM b,UINT_PTR id,DWORD_PTR data)
{
    (void)id;(void)data;
    if(m==WM_CAPTURECHANGED || m==WM_NCLBUTTONDOWN){
        printf("COMBO NATIVE BOUNDARY message=%04x capture=%p lparam=%p\n",m,(void *)GetCapture(),(void *)b);fflush(stdout);}
    return DefSubclassProc(w,m,a,b);
}
static void test_combo_scroll_release(HWND parent)
{
    int kind,n;
    for(kind=0;kind<2;++kind){
        HWND combo,list;COMBOBOXINFO cb={sizeof(cb)};SCROLLBARINFO bar={sizeof(bar)};
        SCROLLINFO si={sizeof(si),SIF_ALL};win16_scroll_layout layout;win16_scroll_metrics metrics;
        RECT rect;POINT p;LRESULT top;int dpi;
        combo=CreateWindowW(L"COMBOBOX",L"",WS_CHILD|WS_VISIBLE|WS_VSCROLL|
            (kind?CBS_DROPDOWN:CBS_DROPDOWNLIST),16,16,220,200,parent,(HMENU)1790,GetModuleHandleW(NULL),NULL);
        REQUIRE(combo && GetComboBoxInfo(combo,&cb));list=cb.hwndList;
        for(n=0;n<60;++n)SendMessageW(combo,CB_ADDSTRING,0,(LPARAM)L"Dropdown release contract");
        REQUIRE(SetWindowSubclass(list,combo_capture_trace,(UINT_PTR)combo_capture_trace,0));
        REQUIRE(win16_style_control_install(combo,NULL));REQUIRE(win16_style_list_scrollbar_enable(list));
        SendMessageW(combo,CB_SETCURSEL,0,0);
        /* Open through the button, not CB_SHOWDROPDOWN: native combo tracking
         * owns capture and subsequent bar clicks arrive as CLIENT messages. */
        p.x=(cb.rcButton.left+cb.rcButton.right)/2;p.y=(cb.rcButton.top+cb.rcButton.bottom)/2;
        SendMessageW(combo,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(p.x,p.y));
        {HWND captured=GetCapture();REQUIRE(captured);
            ClientToScreen(combo,&p);ScreenToClient(captured,&p);
            SendMessageW(captured,WM_LBUTTONUP,0,MAKELPARAM(p.x,p.y));}
        printf("COMBO RELEASE OPEN kind=%d dropped=%lld capture=%p list=%p\n",kind,
            (long long)SendMessageW(combo,CB_GETDROPPEDSTATE,0,0),(void *)GetCapture(),(void *)list);fflush(stdout);
        REQUIRE(SendMessageW(combo,CB_GETDROPPEDSTATE,0,0));REQUIRE(GetCapture()==list);
        REQUIRE(GetScrollBarInfo(list,OBJID_VSCROLL,&bar));REQUIRE(GetScrollInfo(list,SB_VERT,&si));
        SetRect(&rect,0,0,bar.rcScrollBar.right-bar.rcScrollBar.left,bar.rcScrollBar.bottom-bar.rcScrollBar.top);
        dpi=(int)GetDpiForWindow(list);metrics.arrow=metrics.thumb=MulDiv(17,dpi,96);metrics.border=max(1,MulDiv(1,dpi,96));
        REQUIRE(win16_scroll_measure(&rect,TRUE,&metrics,si.nMin,si.nMax-(int)si.nPage+1,si.nPos,&layout));
        p.x=bar.rcScrollBar.left+rect.right/2;p.y=bar.rcScrollBar.top+(layout.thumb.top+layout.thumb.bottom)/2;
        ScreenToClient(list,&p);top=SendMessageW(list,LB_GETTOPINDEX,0,0);
        SendMessageW(list,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(p.x,p.y));
        p.y+=25;SendMessageW(list,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(p.x,p.y));
        SendMessageW(list,WM_LBUTTONUP,0,MAKELPARAM(p.x,p.y));
        printf("COMBO RELEASE RESULT kind=%d dropped=%lld visible=%d top=%lld->%lld capture=%p\n",kind,
            (long long)SendMessageW(combo,CB_GETDROPPEDSTATE,0,0),IsWindowVisible(list),(long long)top,
            (long long)SendMessageW(list,LB_GETTOPINDEX,0,0),(void *)GetCapture());fflush(stdout);
        REQUIRE(SendMessageW(combo,CB_GETDROPPEDSTATE,0,0));REQUIRE(IsWindowVisible(list));
        REQUIRE(GetCapture()==list);REQUIRE(SendMessageW(list,LB_GETTOPINDEX,0,0)>top);
        REQUIRE(SendMessageW(combo,CB_GETCURSEL,0,0)==0);
        /* Releasing outside the bar must still finish only the scroll gesture,
         * not accidentally turn the release into an item/outside click. */
        REQUIRE(GetScrollInfo(list,SB_VERT,&si));
        REQUIRE(win16_scroll_measure(&rect,TRUE,&metrics,si.nMin,si.nMax-(int)si.nPage+1,si.nPos,&layout));
        p.x=bar.rcScrollBar.left+rect.right/2;p.y=bar.rcScrollBar.top+(layout.thumb.top+layout.thumb.bottom)/2;
        ScreenToClient(list,&p);SendMessageW(list,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(p.x,p.y));
        p.x=-40;SendMessageW(list,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(p.x,p.y));
        SendMessageW(list,WM_LBUTTONUP,0,MAKELPARAM(p.x,p.y));
        REQUIRE(SendMessageW(combo,CB_GETDROPPEDSTATE,0,0) && GetCapture()==list);
        /* Arrows/page areas use the same captured-client entry, with repeat
         * timers cleaned on release. Native item choice remains native. */
        for(n=0;n<2;++n){
            p.x=bar.rcScrollBar.left+rect.right/2;
            p.y=bar.rcScrollBar.bottom-(n?metrics.arrow+2:metrics.arrow/2);
            ScreenToClient(list,&p);top=SendMessageW(list,LB_GETTOPINDEX,0,0);
            SendMessageW(list,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(p.x,p.y));
            SendMessageW(list,WM_LBUTTONUP,0,MAKELPARAM(p.x,p.y));
            REQUIRE(SendMessageW(combo,CB_GETDROPPEDSTATE,0,0) && GetCapture()==list);
            REQUIRE(SendMessageW(list,LB_GETTOPINDEX,0,0)>top);
        }
        top=SendMessageW(list,LB_GETTOPINDEX,0,0);
        SendMessageW(list,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(8,8));
        SendMessageW(list,WM_LBUTTONUP,0,MAKELPARAM(8,8));
        REQUIRE(!SendMessageW(combo,CB_GETDROPPEDSTATE,0,0));REQUIRE(GetCapture()!=list);
        REQUIRE(SendMessageW(combo,CB_GETCURSEL,0,0)==top);
        SendMessageW(combo,CB_SHOWDROPDOWN,TRUE,0);
        REQUIRE(SendMessageW(combo,CB_GETDROPPEDSTATE,0,0));
        SendMessageW(combo,WM_KEYDOWN,VK_ESCAPE,0);
        REQUIRE(!SendMessageW(combo,CB_GETDROPPEDSTATE,0,0));
        SendMessageW(combo,CB_SHOWDROPDOWN,TRUE,0);
        REQUIRE(SendMessageW(combo,CB_GETDROPPEDSTATE,0,0));
        SendMessageW(list,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(-40,-40));
        SendMessageW(list,WM_LBUTTONUP,0,MAKELPARAM(-40,-40));
        REQUIRE(!SendMessageW(combo,CB_GETDROPPEDSTATE,0,0));REQUIRE(GetCapture()!=list);
        printf("COMBO RELEASE PASS kind=%d thumb/outside release, arrow/page, selection, Escape and outside close\n",kind);
        REQUIRE(RemoveWindowSubclass(list,combo_capture_trace,(UINT_PTR)combo_capture_trace));DestroyWindow(combo);
    }
}

static void test_integrated_list_scroll(HWND parent)
{
    typedef HRESULT (WINAPI *theme_fn)(HWND,LPCWSTR,LPCWSTR);
    HMODULE module=LoadLibraryExW(L"uxtheme.dll",NULL,LOAD_LIBRARY_SEARCH_SYSTEM32);
    theme_fn theme;int kind,n;REQUIRE(module);theme=(theme_fn)(void *)GetProcAddress(module,"SetWindowTheme");REQUIRE(theme);
    for(kind=0;kind<4;++kind){
        HWND combo=NULL,list,previous_capture;COMBOBOXINFO cb={sizeof(cb)};SCROLLBARINFO bar={sizeof(bar)};
        SCROLLINFO info={sizeof(info),SIF_ALL};RECT rect;POINT origin,p;win16_scroll_layout layout;
        int dpi;win16_scroll_metrics metrics;unsigned foreign,boundary_samples=0;
        if(kind){combo=CreateWindowW(L"COMBOBOX",L"",WS_CHILD|WS_VISIBLE|WS_VSCROLL|
                (kind==1?CBS_DROPDOWNLIST:kind==2?CBS_DROPDOWN:CBS_SIMPLE),
                16,16,220,200,parent,(HMENU)(INT_PTR)(1700+kind),GetModuleHandleW(NULL),NULL);
            REQUIRE(combo && GetComboBoxInfo(combo,&cb));list=cb.hwndList;
        }else list=CreateWindowW(L"LISTBOX",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL,
                16,16,220,200,parent,(HMENU)1700,GetModuleHandleW(NULL),NULL);
        REQUIRE(list);REQUIRE(SUCCEEDED(theme(list,L"",L"")));
        for(n=0;n<60;++n)SendMessageW(combo?combo:list,combo?CB_ADDSTRING:LB_ADDSTRING,0,(LPARAM)L"Integrated native list data");
        REQUIRE(SetWindowSubclass(list,integrated_list_boundary,(UINT_PTR)integrated_list_boundary,(DWORD_PTR)&boundary_samples));
        REQUIRE(win16_style_control_install(combo?combo:list,NULL));
        REQUIRE(win16_style_list_scrollbar_enable(list));
        if(combo)SendMessageW(combo,CB_SHOWDROPDOWN,TRUE,0);
        RedrawWindow(list,NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
        for(n=0;n<3;++n){SendMessageW(list,WM_MOUSEWHEEL,MAKEWPARAM(0,(WORD)-WHEEL_DELTA),0);
            REQUIRE(GetScrollInfo(list,SB_VERT,&info));REQUIRE(info.nPos==SendMessageW(list,LB_GETTOPINDEX,0,0));
            {unsigned errors=visible_list_bar_errors(list,&foreign);
                if(errors || foreign){RECT wr,cr,parentrect;GetWindowRect(list,&wr);GetClientRect(list,&cr);
                    GetClientRect(GetParent(list),&parentrect);MapWindowPoints(GetParent(list),NULL,(POINT *)&parentrect,2);
                    fprintf(stderr,"FONT LIST boundary kind=%d wheel=%d errors=%u foreign=%u window=%ld,%ld,%ld,%ld client=%ld,%ld\n",
                        kind,n,errors,foreign,wr.left,wr.top,wr.right,wr.bottom,cr.right,cr.bottom);
                    fprintf(stderr,"FONT LIST parent client=%ld,%ld,%ld,%ld\n",parentrect.left,parentrect.top,parentrect.right,parentrect.bottom);}
                REQUIRE(!errors && !foreign);}}
        REQUIRE(info.nPos>0);REQUIRE(GetScrollBarInfo(list,OBJID_VSCROLL,&bar));
        origin.x=bar.rcScrollBar.left;origin.y=bar.rcScrollBar.top;
        SetRect(&rect,0,0,bar.rcScrollBar.right-origin.x,bar.rcScrollBar.bottom-origin.y);
        dpi=(int)GetDpiForWindow(list);metrics.arrow=metrics.thumb=MulDiv(17,dpi,96);metrics.border=max(1,MulDiv(1,dpi,96));
        REQUIRE(win16_scroll_measure(&rect,TRUE,&metrics,info.nMin,info.nMax-(int)info.nPage+1,info.nPos,&layout));
        p.x=origin.x+rect.right/2;p.y=origin.y+(layout.thumb.top+layout.thumb.bottom)/2;
        previous_capture=GetCapture();
        SendMessageW(list,WM_NCLBUTTONDOWN,HTVSCROLL,MAKELPARAM(p.x,p.y));
        printf("INTEGRATED CAPTURE kind=%d before=%p list=%p after=%p visible=%d\n",kind,
            (void *)previous_capture,(void *)list,(void *)GetCapture(),IsWindowVisible(list));fflush(stdout);
        REQUIRE(GetCapture()==list);
        p.y+=25;ScreenToClient(list,&p);SendMessageW(list,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(p.x,p.y));
        REQUIRE(SendMessageW(list,LB_GETTOPINDEX,0,0)>info.nPos);
        REQUIRE(!visible_list_bar_errors(list,&foreign) && !foreign);
        SendMessageW(list,WM_LBUTTONUP,0,MAKELPARAM(p.x,p.y));REQUIRE(GetCapture()==previous_capture);
        REQUIRE(IsWindowVisible(list));
        REQUIRE(!visible_list_bar_errors(list,&foreign) && !foreign);
        for(n=0;n<16;++n){SendMessageW(list,WM_MOUSEMOVE,0,MAKELPARAM(8,8+(n%4)*12));
            REQUIRE(!visible_list_bar_errors(list,&foreign) && !foreign);}
        /* Explicit per-arrow state is independent of the range endpoints.
         * A disabled down arrow must neither scroll nor steal popup capture. */
        REQUIRE(GetScrollBarInfo(list,OBJID_VSCROLL,&bar));
        p.x=(bar.rcScrollBar.left+bar.rcScrollBar.right)/2;
        p.y=bar.rcScrollBar.bottom-metrics.arrow/2;
        {LRESULT top=SendMessageW(list,LB_GETTOPINDEX,0,0);
            previous_capture=GetCapture();
            EnableScrollBar(list,SB_VERT,ESB_DISABLE_RTDN);
            REQUIRE(GetScrollBarInfo(list,OBJID_VSCROLL,&bar));
            REQUIRE(bar.rgstate[5]&STATE_SYSTEM_UNAVAILABLE);
            SendMessageW(list,WM_NCLBUTTONDOWN,HTVSCROLL,MAKELPARAM(p.x,p.y));
            REQUIRE(SendMessageW(list,LB_GETTOPINDEX,0,0)==top);
            REQUIRE(GetCapture()==previous_capture);
            EnableScrollBar(list,SB_VERT,ESB_ENABLE_BOTH);
            SendMessageW(list,WM_NCLBUTTONDOWN,HTVSCROLL,MAKELPARAM(p.x,p.y));
            REQUIRE(SendMessageW(list,LB_GETTOPINDEX,0,0)>top);
            REQUIRE(GetCapture()==list);
            ScreenToClient(list,&p);SendMessageW(list,WM_LBUTTONUP,0,MAKELPARAM(p.x,p.y));
            REQUIRE(GetCapture()==previous_capture);
            printf("INTEGRATED ARROW STATE PASS kind=%d disabled inert, enabled scrolls, capture preserved\n",kind);}
        REQUIRE(boundary_samples>=5);
        REQUIRE(RemoveWindowSubclass(list,integrated_list_boundary,(UINT_PTR)integrated_list_boundary));
        /* Cancel our gesture without ending the combo's borrowed capture;
         * then verify a genuine capture transfer is never stolen back. */
        REQUIRE(GetScrollBarInfo(list,OBJID_VSCROLL,&bar));
        p.x=(bar.rcScrollBar.left+bar.rcScrollBar.right)/2;
        p.y=bar.rcScrollBar.top+metrics.arrow/2;
        previous_capture=GetCapture();
        SendMessageW(list,WM_NCLBUTTONDOWN,HTVSCROLL,MAKELPARAM(p.x,p.y));
        REQUIRE(GetCapture()==list);
        SendMessageW(list,WM_CANCELMODE,0,0);REQUIRE(GetCapture()==previous_capture);
        SendMessageW(list,WM_NCLBUTTONDOWN,HTVSCROLL,MAKELPARAM(p.x,p.y));
        REQUIRE(GetCapture()==list);SetCapture(parent);REQUIRE(GetCapture()==parent);
        ReleaseCapture();
        if(combo)SendMessageW(combo,CB_SHOWDROPDOWN,TRUE,0);
        REQUIRE(GetScrollBarInfo(list,OBJID_VSCROLL,&bar));
        p.x=(bar.rcScrollBar.left+bar.rcScrollBar.right)/2;
        p.y=bar.rcScrollBar.top+metrics.arrow/2;
        previous_capture=GetCapture();
        SendMessageW(list,WM_NCLBUTTONDOWN,HTVSCROLL,MAKELPARAM(p.x,p.y));
        REQUIRE(GetCapture()==list);
        REQUIRE(win16_style_control_remove(combo?combo:list));
        REQUIRE(GetCapture()==previous_capture);
        printf("INTEGRATED LIFETIME PASS kind=%d cancel, capture transfer and active removal\n",kind);
        REQUIRE(SUCCEEDED(theme(list,NULL,NULL)));
        SendMessageW(list,LB_SETTOPINDEX,2,0);REQUIRE(SendMessageW(list,LB_GETTOPINDEX,0,0)==2);
        DestroyWindow(combo?combo:list);
        printf("INTEGRATED LIST SCROLL PASS kind=%d native data, wheel/position, fixed-thumb capture/drag, %u intermediate boundaries and removal\n",kind,boundary_samples);fflush(stdout);
    }
    FreeLibrary(module);
}

static void test_multicolumn_scroll(HWND parent)
{
    typedef HRESULT (WINAPI *theme_fn)(HWND,LPCWSTR,LPCWSTR);
    HMODULE module=LoadLibraryExW(L"uxtheme.dll",NULL,LOAD_LIBRARY_SEARCH_SYSTEM32);
    theme_fn theme;HWND list;int n,rows,height,dpi;unsigned boundary_count=0,foreign;
    SCROLLINFO info={sizeof(info),SIF_ALL};SCROLLBARINFO bar={sizeof(bar)};
    RECT client,rect;POINT p,origin;win16_scroll_layout layout;win16_scroll_metrics metrics;
    REQUIRE(module);theme=(theme_fn)(void *)GetProcAddress(module,"SetWindowTheme");REQUIRE(theme);
    list=CreateWindowW(L"LISTBOX",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|WS_HSCROLL|LBS_MULTICOLUMN,
        16,16,280,180,parent,(HMENU)1740,GetModuleHandleW(NULL),NULL);REQUIRE(list);
    SendMessageW(list,WM_SETFONT,(WPARAM)GetStockObject(SYSTEM_FONT),FALSE);
    SendMessageW(list,LB_SETCOLUMNWIDTH,90,0);
    for(n=0;n<200;++n)REQUIRE(SendMessageW(list,LB_ADDSTRING,0,(LPARAM)L"Column item")==n);
    REQUIRE(SUCCEEDED(theme(list,L"",L"")));
    REQUIRE(SetWindowSubclass(list,integrated_list_boundary,(UINT_PTR)integrated_list_boundary,(DWORD_PTR)&boundary_count));
    REQUIRE(win16_style_control_install(list,NULL));REQUIRE(win16_style_list_scrollbar_enable(list));
    GetClientRect(list,&client);height=(int)SendMessageW(list,LB_GETITEMHEIGHT,0,0);REQUIRE(height>0);
    rows=max(1,client.bottom/height);
    RedrawWindow(list,NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
    SendMessageW(list,WM_HSCROLL,SB_LINERIGHT,0);
    REQUIRE(SendMessageW(list,LB_GETTOPINDEX,0,0)==rows);
    REQUIRE(!visible_list_bar_errors(list,&foreign) && !foreign);
    REQUIRE(GetScrollInfo(list,SB_HORZ,&info));REQUIRE(GetScrollBarInfo(list,OBJID_HSCROLL,&bar));
    origin.x=bar.rcScrollBar.left;origin.y=bar.rcScrollBar.top;
    SetRect(&rect,0,0,bar.rcScrollBar.right-origin.x,bar.rcScrollBar.bottom-origin.y);
    dpi=(int)GetDpiForWindow(list);metrics.arrow=metrics.thumb=MulDiv(17,dpi,96);metrics.border=max(1,MulDiv(1,dpi,96));
    REQUIRE(win16_scroll_measure(&rect,FALSE,&metrics,info.nMin,info.nMax-(int)info.nPage+1,info.nPos,&layout));
    p.x=origin.x+(layout.thumb.left+layout.thumb.right)/2;p.y=origin.y+rect.bottom/2;
    SendMessageW(list,WM_NCLBUTTONDOWN,HTHSCROLL,MAKELPARAM(p.x,p.y));REQUIRE(GetCapture()==list);
    p.x+=40;ScreenToClient(list,&p);SendMessageW(list,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(p.x,p.y));
    REQUIRE(GetScrollInfo(list,SB_HORZ,&info));REQUIRE(info.nPos>1);
    REQUIRE(SendMessageW(list,LB_GETTOPINDEX,0,0)==info.nPos*rows);
    REQUIRE(!visible_list_bar_errors(list,&foreign) && !foreign);
    SendMessageW(list,WM_LBUTTONUP,0,MAKELPARAM(p.x,p.y));REQUIRE(GetCapture()!=list);
    SendMessageW(list,WM_MOUSEWHEEL,MAKEWPARAM(0,(WORD)-WHEEL_DELTA),0);
    REQUIRE(!visible_list_bar_errors(list,&foreign) && !foreign);
    for(n=0;n<8;++n){SendMessageW(list,WM_MOUSEMOVE,0,MAKELPARAM(8,8+n*5));
        REQUIRE(!visible_list_bar_errors(list,&foreign) && !foreign);}
    REQUIRE(boundary_count>=5);REQUIRE(RemoveWindowSubclass(list,integrated_list_boundary,(UINT_PTR)integrated_list_boundary));
    REQUIRE(win16_style_control_remove(list));REQUIRE(SUCCEEDED(theme(list,NULL,NULL)));
    SendMessageW(list,WM_HSCROLL,SB_LEFT,0);REQUIRE(!SendMessageW(list,LB_GETTOPINDEX,0,0));
    DestroyWindow(list);FreeLibrary(module);
    printf("MULTICOLUMN SCROLL PASS columns-to-items, fixed thumb, capture, wheel/motion, %u native boundaries and removal\n",boundary_count);
}

static void test_list_horizontal_content(HWND parent)
{
    typedef HRESULT (WINAPI *theme_fn)(HWND,LPCWSTR,LPCWSTR);
    HMODULE module=LoadLibraryExW(L"uxtheme.dll",NULL,LOAD_LIBRARY_SEARCH_SYSTEM32);
    theme_fn theme;
    HWND list;RECT client,outer,item;POINT origin={0,0};DWORD *before,*after;
    HBITMAP first,second;int width,second_width,x,y;unsigned compared=0,ink=0;
    SCROLLINFO info={sizeof(info),SIF_ALL};SCROLLBARINFO bar={sizeof(bar)};
    win16_scroll_layout layout;win16_scroll_metrics metrics;RECT rect;POINT p;
    unsigned boundaries=0,foreign;int n,dpi;LRESULT top;
    REQUIRE(module);theme=(theme_fn)(void *)GetProcAddress(module,"SetWindowTheme");REQUIRE(theme);
    list=CreateWindowW(L"LISTBOX",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|WS_HSCROLL|WS_VSCROLL,
        16,16,280,100,parent,(HMENU)1741,GetModuleHandleW(NULL),NULL);REQUIRE(list);
    SendMessageW(list,WM_SETFONT,(WPARAM)GetStockObject(SYSTEM_FONT),FALSE);
    REQUIRE(SendMessageW(list,LB_ADDSTRING,0,(LPARAM)L"ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 0123456789")==0);
    for(n=1;n<40;++n)REQUIRE(SendMessageW(list,LB_ADDSTRING,0,(LPARAM)L"Other horizontal content row")==n);
    SendMessageW(list,LB_SETHORIZONTALEXTENT,1200,0);
    REQUIRE(SetWindowSubclass(list,integrated_list_boundary,(UINT_PTR)integrated_list_boundary,(DWORD_PTR)&boundaries));
    REQUIRE(win16_style_control_install(list,NULL));
    REQUIRE(SUCCEEDED(theme(list,L"",L"")));REQUIRE(win16_style_list_scrollbar_enable(list));
    RedrawWindow(list,NULL,NULL,RDW_INVALIDATE|RDW_FRAME|RDW_UPDATENOW);
    GetClientRect(list,&client);GetWindowRect(list,&outer);ClientToScreen(list,&origin);
    origin.x-=outer.left;origin.y-=outer.top;
    REQUIRE(SendMessageW(list,LB_GETITEMRECT,0,(LPARAM)&item)!=LB_ERR);
    first=read_window_pixels(list,&before,&width);REQUIRE(first);
    SendMessageW(list,WM_HSCROLL,MAKEWPARAM(SB_THUMBPOSITION,24),0);
    REQUIRE(GetScrollInfo(list,SB_HORZ,&info) && info.nPos==24);
    REQUIRE(!SendMessageW(list,LB_GETTOPINDEX,0,0));
    RedrawWindow(list,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW);
    second=read_window_pixels(list,&after,&second_width);REQUIRE(second && second_width==width);
    for(y=2;y<item.bottom-2;++y)for(x=4;x<client.right-30;++x){
        DWORD expected=before[(origin.y+y)*width+origin.x+x+24]&0xffffff;
        REQUIRE((after[(origin.y+y)*width+origin.x+x]&0xffffff)==expected);
        ++compared;if(expected!=0xffffff)++ink;
    }
    REQUIRE(compared>1000 && ink>50);DeleteObject(second);DeleteObject(first);
    SendMessageW(list,LB_SETTOPINDEX,3,0);top=SendMessageW(list,LB_GETTOPINDEX,0,0);REQUIRE(top==3);
    REQUIRE(GetScrollBarInfo(list,OBJID_HSCROLL,&bar));REQUIRE(GetScrollInfo(list,SB_HORZ,&info));
    SetRect(&rect,0,0,bar.rcScrollBar.right-bar.rcScrollBar.left,bar.rcScrollBar.bottom-bar.rcScrollBar.top);
    dpi=(int)GetDpiForWindow(list);metrics.arrow=metrics.thumb=MulDiv(17,dpi,96);metrics.border=max(1,MulDiv(1,dpi,96));
    REQUIRE(win16_scroll_measure(&rect,FALSE,&metrics,info.nMin,info.nMax-(int)info.nPage+1,info.nPos,&layout));
    p.x=bar.rcScrollBar.left+(layout.thumb.left+layout.thumb.right)/2;p.y=bar.rcScrollBar.top+rect.bottom/2;
    SendMessageW(list,WM_NCLBUTTONDOWN,HTHSCROLL,MAKELPARAM(p.x,p.y));REQUIRE(GetCapture()==list);
    p.x+=25;ScreenToClient(list,&p);SendMessageW(list,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(p.x,p.y));
    REQUIRE(GetScrollInfo(list,SB_HORZ,&info) && info.nPos>24);
    REQUIRE(SendMessageW(list,LB_GETTOPINDEX,0,0)==top);
    REQUIRE(SendMessageW(list,LB_GETITEMRECT,top,(LPARAM)&item)!=LB_ERR);
    REQUIRE(item.right-client.right==info.nPos);
    REQUIRE(!visible_bar_errors(list,OBJID_HSCROLL,&foreign) && !foreign);
    REQUIRE(!visible_bar_errors(list,OBJID_VSCROLL,&foreign) && !foreign);
    SendMessageW(list,WM_LBUTTONUP,0,MAKELPARAM(p.x,p.y));REQUIRE(GetCapture()!=list);
    REQUIRE(boundaries>=4);REQUIRE(RemoveWindowSubclass(list,integrated_list_boundary,(UINT_PTR)integrated_list_boundary));
    REQUIRE(win16_style_control_remove(list));DestroyWindow(list);FreeLibrary(module);
    printf("LIST HORIZONTAL CONTENT PASS 24px native origin, unchanged top item, %u translated pixels with %u ink pixels\n",compared,ink);
    printf("LIST HORIZONTAL TRACK PASS content origin, unchanged vertical position, both bar pixels and %u native boundaries\n",boundaries);
}

static LRESULT CALLBACK edit_scroll_boundary(HWND w,UINT m,WPARAM a,LPARAM b,UINT_PTR id,DWORD_PTR data)
{
    BOOL observe=m==WM_VSCROLL || m==WM_HSCROLL || m==WM_MOUSEWHEEL || m==EM_LINESCROLL;
    LONG object=m==WM_HSCROLL?OBJID_HSCROLL:OBJID_VSCROLL;
    DWORD before=observe?visible_bar_hash(w,object):0;LRESULT result;(void)id;
    result=DefSubclassProc(w,m,a,b);
    if(observe){REQUIRE(visible_bar_hash(w,object)==before);++*(unsigned *)data;}
    return result;
}
typedef struct edit_notifications {HWND edit;unsigned vertical,horizontal;} edit_notifications;
static LRESULT CALLBACK observe_edit_notifications(HWND w,UINT m,WPARAM a,LPARAM b,UINT_PTR id,DWORD_PTR data)
{
    edit_notifications *notices=(edit_notifications *)data;(void)id;
    if(m==WM_COMMAND && (HWND)b==notices->edit && HIWORD(a)==EN_VSCROLL)++notices->vertical;
    if(m==WM_COMMAND && (HWND)b==notices->edit && HIWORD(a)==EN_HSCROLL)++notices->horizontal;
    return DefSubclassProc(w,m,a,b);
}
static void test_edit_scroll(HWND parent)
{
    typedef HRESULT (WINAPI *theme_fn)(HWND,LPCWSTR,LPCWSTR);
    HMODULE module=LoadLibraryExW(L"uxtheme.dll",NULL,LOAD_LIBRARY_SEARCH_SYSTEM32);
    theme_fn theme;HWND edit;WCHAR text[12000];int n,offset=0,dpi,vertical_position;unsigned count=0,foreign;
    DWORD first,last;SCROLLINFO info={sizeof(info),SIF_ALL};SCROLLBARINFO bar={sizeof(bar)};
    RECT rect;POINT p,origin;win16_scroll_layout layout;win16_scroll_metrics metrics;
    edit_notifications notices={0};
    REQUIRE(module);theme=(theme_fn)(void *)GetProcAddress(module,"SetWindowTheme");REQUIRE(theme);
    for(n=0;n<100;++n)offset+=swprintf_s(text+offset,12000-offset,L"Native edit line %d: abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ\r\n",n);
    edit=CreateWindowW(L"EDIT",text,WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL|ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL,
        16,16,280,180,parent,(HMENU)1750,GetModuleHandleW(NULL),NULL);REQUIRE(edit);
    notices.edit=edit;
    REQUIRE(SetWindowSubclass(parent,observe_edit_notifications,(UINT_PTR)observe_edit_notifications,(DWORD_PTR)&notices));
    REQUIRE(SUCCEEDED(theme(edit,L"",L"")));
    SendMessageW(edit,EM_SETSEL,4,9);
    REQUIRE(SetWindowSubclass(edit,edit_scroll_boundary,(UINT_PTR)edit_scroll_boundary,(DWORD_PTR)&count));
    REQUIRE(win16_style_edit_install(edit,FALSE,NULL));REQUIRE(win16_style_edit_scrollbars_enable(edit));
    RedrawWindow(edit,NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
    SendMessageW(edit,WM_MOUSEWHEEL,MAKEWPARAM(0,(WORD)-WHEEL_DELTA),0);
    REQUIRE(GetScrollInfo(edit,SB_VERT,&info));REQUIRE(info.nPos>0);
    REQUIRE(!visible_list_bar_errors(edit,&foreign) && !foreign);
    REQUIRE(GetScrollBarInfo(edit,OBJID_VSCROLL,&bar));
    origin.x=bar.rcScrollBar.left;origin.y=bar.rcScrollBar.top;
    SetRect(&rect,0,0,bar.rcScrollBar.right-origin.x,bar.rcScrollBar.bottom-origin.y);
    dpi=(int)GetDpiForWindow(edit);metrics.arrow=metrics.thumb=MulDiv(17,dpi,96);metrics.border=max(1,MulDiv(1,dpi,96));
    REQUIRE(win16_scroll_measure(&rect,TRUE,&metrics,info.nMin,info.nMax-(int)info.nPage+1,info.nPos,&layout));
    p.x=origin.x+rect.right/2;p.y=origin.y+(layout.thumb.top+layout.thumb.bottom)/2;
    notices.vertical=0;
    SendMessageW(edit,WM_NCLBUTTONDOWN,HTVSCROLL,MAKELPARAM(p.x,p.y));REQUIRE(GetCapture()==edit);
    p.y+=25;ScreenToClient(edit,&p);SendMessageW(edit,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(p.x,p.y));
    REQUIRE(GetScrollInfo(edit,SB_VERT,&info));REQUIRE(info.nPos>layout.position);
    REQUIRE(SendMessageW(edit,EM_GETFIRSTVISIBLELINE,0,0)==info.nPos);
    REQUIRE(notices.vertical==0); /* Native SB_THUMBTRACK is silent. */
    REQUIRE(!visible_list_bar_errors(edit,&foreign) && !foreign);
    SendMessageW(edit,WM_LBUTTONUP,0,MAKELPARAM(p.x,p.y));REQUIRE(GetCapture()!=edit);
    REQUIRE(notices.vertical==0); /* Same-position completion makes no new move. */
    SendMessageW(edit,WM_VSCROLL,SB_LINEDOWN,0);REQUIRE(notices.vertical==1);
    vertical_position=(int)SendMessageW(edit,EM_GETFIRSTVISIBLELINE,0,0);
    REQUIRE(GetScrollInfo(edit,SB_HORZ,&info));REQUIRE(info.nMax>(int)info.nPage);
    REQUIRE(GetScrollBarInfo(edit,OBJID_HSCROLL,&bar));
    origin.x=bar.rcScrollBar.left;origin.y=bar.rcScrollBar.top;
    SetRect(&rect,0,0,bar.rcScrollBar.right-origin.x,bar.rcScrollBar.bottom-origin.y);
    REQUIRE(win16_scroll_measure(&rect,FALSE,&metrics,info.nMin,info.nMax-(int)info.nPage+1,info.nPos,&layout));
    p.x=origin.x+(layout.thumb.left+layout.thumb.right)/2;p.y=origin.y+rect.bottom/2;
    notices.vertical=notices.horizontal=0;
    SendMessageW(edit,WM_NCLBUTTONDOWN,HTHSCROLL,MAKELPARAM(p.x,p.y));REQUIRE(GetCapture()==edit);
    p.x+=40;ScreenToClient(edit,&p);SendMessageW(edit,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(p.x,p.y));
    REQUIRE(GetScrollInfo(edit,SB_HORZ,&info));REQUIRE(info.nPos>layout.position);
    REQUIRE(SendMessageW(edit,EM_GETFIRSTVISIBLELINE,0,0)==vertical_position);
    REQUIRE(!notices.horizontal && !notices.vertical);
    REQUIRE(!visible_bar_errors(edit,OBJID_HSCROLL,&foreign) && !foreign);
    REQUIRE(!visible_bar_errors(edit,OBJID_VSCROLL,&foreign) && !foreign);
    SendMessageW(edit,WM_LBUTTONUP,0,MAKELPARAM(p.x,p.y));REQUIRE(GetCapture()!=edit);
    REQUIRE(!notices.horizontal && !notices.vertical);
    SendMessageW(edit,WM_HSCROLL,SB_LINERIGHT,0);REQUIRE(notices.horizontal==1 && !notices.vertical);
    SendMessageW(edit,EM_GETSEL,(WPARAM)&first,(LPARAM)&last);REQUIRE(first==4 && last==9);
    REQUIRE(GetWindowTextLengthW(edit)==offset);REQUIRE(count>=4);
    REQUIRE(RemoveWindowSubclass(edit,edit_scroll_boundary,(UINT_PTR)edit_scroll_boundary));
    REQUIRE(win16_style_control_remove(edit));REQUIRE(SUCCEEDED(theme(edit,NULL,NULL)));
    SendMessageW(edit,EM_REPLACESEL,TRUE,(LPARAM)L"changed");REQUIRE(SendMessageW(edit,EM_UNDO,0,0));
    REQUIRE(GetWindowTextLengthW(edit)==offset);
    REQUIRE(RemoveWindowSubclass(parent,observe_edit_notifications,(UINT_PTR)observe_edit_notifications));
    DestroyWindow(edit);FreeLibrary(module);
    printf("EDIT BOTH AXES SCROLL PASS independent positions, fixed thumbs, native parent notifications, wheel/capture, selection/text/undo, %u native boundaries\n",count);
}

static void test_list_scroll_stability(HWND parent)
{
    int n,k;unsigned failures=0;
    for(n=0;n<5;++n){
        HWND combo=n>=2?GetDlgItem(parent,1230+n-2):NULL;
        COMBOBOXINFO ci={sizeof(ci)};HWND list;SCROLLBARINFO bar={sizeof(bar)};
        RECT outer;POINT pt;HBITMAP bitmap;DWORD *pixels;COLORREF *before;int image_width,width,height,x,y;unsigned changed=0,paints=0;
        if(combo){SendMessageW(combo,CB_SHOWDROPDOWN,TRUE,0);REQUIRE(GetComboBoxInfo(combo,&ci));list=ci.hwndList;}
        else list=GetDlgItem(parent,1218+n);
        REQUIRE(IsWindowVisible(list));
        RedrawWindow(list,NULL,NULL,RDW_INVALIDATE|RDW_FRAME|RDW_UPDATENOW);
        {unsigned errors=visible_list_arrow_errors(list);
            printf("LIST LIVE OEM kind=%d baseline errors=%u /578\n",n,errors);fflush(stdout);REQUIRE(!errors);}
        REQUIRE(GetScrollBarInfo(list,OBJID_VSCROLL,&bar));REQUIRE(!(bar.rgstate[0]&STATE_SYSTEM_INVISIBLE));
        GetWindowRect(list,&outer);width=bar.rcScrollBar.right-bar.rcScrollBar.left;height=bar.dxyLineButton;
        REQUIRE(width>0 && height>0);before=calloc((size_t)width*height,sizeof(*before));REQUIRE(before);
        pt.x=bar.rcScrollBar.left-outer.left;pt.y=bar.rcScrollBar.top-outer.top;
        bitmap=read_window_pixels(list,&pixels,&image_width);
        for(y=0;y<height;++y)for(x=0;x<width;++x)before[y*width+x]=pixel_color(pixels[(pt.y+y)*image_width+pt.x+x]);
        DeleteObject(bitmap);
        for(k=0;k<8;++k){
            SendMessageW(list,WM_NCMOUSEMOVE,HTVSCROLL,MAKELPARAM(bar.rcScrollBar.left+width/2,bar.rcScrollBar.top+height/2));
            bitmap=read_window_pixels(list,&pixels,&image_width);
            for(y=0;y<height;++y)for(x=0;x<width;++x)if(pixel_color(pixels[(pt.y+y)*image_width+pt.x+x])!=before[y*width+x])++changed;
            DeleteObject(bitmap);SendMessageW(list,WM_NCMOUSELEAVE,0,0);
        }
        free(before);
        /* Repeating motion over one selected row must not repaint a captured
         * dropdown/list once native selection and top index are stable. */
        SetCapture(list);SendMessageW(list,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(8,8));
        ValidateRect(list,NULL);REQUIRE(SetWindowSubclass(list,count_control_paints,(UINT_PTR)count_control_paints,(DWORD_PTR)&paints));
        for(k=0;k<100;++k)SendMessageW(list,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(8,8));
        UpdateWindow(list);RemoveWindowSubclass(list,count_control_paints,(UINT_PTR)count_control_paints);ReleaseCapture();
        printf("LIST SCROLL STABILITY kind=%d hover changed pixels=%u captured repeated-motion paints=%u\n",n,changed,paints);fflush(stdout);
        if(changed || paints)++failures;
        /* Releasing popup capture may legitimately close a native dropdown.
         * Never read a hidden window DC as evidence of corrupt painting. */
        if(combo && !IsWindowVisible(list))SendMessageW(combo,CB_SHOWDROPDOWN,TRUE,0);
        for(k=0;k<7;++k){unsigned errors;
            switch(k){
            case 0:SendMessageW(list,WM_MOUSEWHEEL,MAKEWPARAM(0,(WORD)-WHEEL_DELTA),0);break;
            case 1:SendMessageW(list,LB_SETTOPINDEX,4,0);break;
            case 2:SendMessageW(list,LB_SETCURSEL,5,0);break;
            case 3:EnableWindow(combo?combo:list,FALSE);break;
            case 4:EnableWindow(combo?combo:list,TRUE);if(combo)SendMessageW(combo,CB_SHOWDROPDOWN,TRUE,0);break;
            case 5:SendMessageW(list,WM_THEMECHANGED,0,0);break;
            case 6:if(combo){SendMessageW(combo,CB_SHOWDROPDOWN,FALSE,0);SendMessageW(combo,CB_SHOWDROPDOWN,TRUE,0);}break;
            }
            if(!IsWindowVisible(list)){
                REQUIRE(combo && k==3 && !IsWindowEnabled(combo));
                printf("LIST LIVE OEM kind=%d transition=%d native disabled popup closed\n",n,k);continue;
            }
            UpdateWindow(list);errors=visible_list_arrow_errors(list);
            printf("LIST LIVE OEM kind=%d transition=%d errors=%u /578\n",n,k,errors);fflush(stdout);
            if(errors)++failures;
        }
        if(combo)SendMessageW(combo,CB_SHOWDROPDOWN,FALSE,0);
    }
    REQUIRE(!failures);
}

static BOOL CALLBACK prepare_owned_list(HWND control,LPARAM parameter)
{
    typedef HRESULT (WINAPI *theme_fn)(HWND,LPCWSTR,LPCWSTR);
    theme_fn theme=(theme_fn)(void *)parameter;WCHAR name[32];HWND list=NULL;
    if(!GetClassNameW(control,name,32))return TRUE;
    if(!lstrcmpiW(name,L"EDIT") && (GetWindowLongPtrW(control,GWL_STYLE)&ES_MULTILINE) &&
        (GetWindowLongPtrW(control,GWL_STYLE)&(WS_HSCROLL|WS_VSCROLL))){
        REQUIRE(SUCCEEDED(theme(control,L"",L"")));
        REQUIRE(win16_style_edit_scrollbars_enable(control));return TRUE;}
    if(!lstrcmpiW(name,L"LISTBOX"))list=control;
    else if(!lstrcmpiW(name,L"COMBOBOX")){COMBOBOXINFO info={sizeof(info)};
        REQUIRE(GetComboBoxInfo(control,&info));list=info.hwndList;}
    if(list){
        /* These are fresh demo-owned windows with default theme association,
         * not arbitrary applications. Their setting dies with the window. */
        REQUIRE(SUCCEEDED(theme(list,L"",L"")));
        REQUIRE(win16_style_list_scrollbar_enable(list));
    }
    return TRUE;
}
static void prepare_owned_lists(HWND root)
{
    HMODULE module=LoadLibraryExW(L"uxtheme.dll",NULL,LOAD_LIBRARY_SEARCH_SYSTEM32);FARPROC theme;
    REQUIRE(module);theme=GetProcAddress(module,"SetWindowTheme");REQUIRE(theme);
    EnumChildWindows(root,prepare_owned_list,(LPARAM)(void *)theme);FreeLibrary(module);
}

static void create_showcase(HWND w)
{
    static const struct {const WCHAR *cls,*text;DWORD style;int x,y,cx,cy;} items[]={
        {L"BUTTON",L"Buttons and selection",BS_GROUPBOX,12,10,278,260},
        {L"BUTTON",L"Push button",BS_PUSHBUTTON,26,32,116,40},
        {L"BUTTON",L"Default",BS_DEFPUSHBUTTON,154,32,120,40},
        {L"BUTTON",L"Disabled",BS_PUSHBUTTON|WS_DISABLED,26,76,116,36},
        {L"BUTTON",L"Checked",BS_AUTOCHECKBOX,26,116,120,24},
        {L"BUTTON",L"Unchecked",BS_AUTOCHECKBOX|BS_LEFTTEXT,154,116,122,24},
        {L"BUTTON",L"Mixed",BS_AUTO3STATE,26,148,120,24},
        {L"BUTTON",L"Disabled",BS_AUTOCHECKBOX|WS_DISABLED,154,148,122,24},
        {L"BUTTON",L"Radio A",BS_AUTORADIOBUTTON|WS_GROUP,26,184,116,24},
        {L"BUTTON",L"Radio B",BS_AUTORADIOBUTTON|BS_LEFTTEXT,154,184,120,24},
        {L"BUTTON",L"Disabled radio",BS_AUTORADIOBUTTON|WS_DISABLED,26,220,230,24},
        {L"BUTTON",L"Text entry",BS_GROUPBOX,302,10,278,260},
        {L"EDIT",L"Single-line input",WS_BORDER|ES_AUTOHSCROLL,316,36,248,26},
        {L"EDIT",L"Read-only input",WS_BORDER|ES_READONLY,316,72,248,26},
        {L"EDIT",L"Disabled input",WS_BORDER|WS_DISABLED,316,108,248,26},
        {L"EDIT",L"Password",WS_BORDER|ES_PASSWORD,316,144,248,26},
        {L"EDIT",L"Multiline edit with scrollbars\r\nSecond line\r\nThird line\r\nFourth line\r\nFifth line",WS_BORDER|ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL|WS_VSCROLL|WS_HSCROLL,316,180,248,76},
        {L"BUTTON",L"Lists",BS_GROUPBOX,592,10,278,260},
        {L"LISTBOX",L"",WS_BORDER|WS_VSCROLL|LBS_NOTIFY,606,36,120,100},
        {L"LISTBOX",L"",WS_BORDER|WS_VSCROLL|WS_HSCROLL|LBS_EXTENDEDSEL|LBS_NOTIFY,738,36,116,100},
        {L"LISTBOX",L"",WS_BORDER|WS_HSCROLL|LBS_MULTICOLUMN|LBS_NOTIFY,606,146,248,60},
        {L"BUTTON",L"Labels, boxes and images",BS_GROUPBOX,12,284,278,238},
        {L"STATIC",L"Left aligned label",SS_LEFT,26,310,246,24},
        {L"STATIC",L"Centered label",SS_CENTER,26,342,246,24},
        {L"STATIC",L"Right aligned label",SS_RIGHT,26,374,246,24},
        {L"STATIC",L"Disabled label",SS_LEFT|WS_DISABLED,26,406,124,24},
        {L"STATIC",L"",SS_BLACKFRAME,26,486,64,28},
        {L"STATIC",L"",SS_GRAYRECT,112,486,64,28},
        {L"STATIC",L"",SS_BLACKRECT,198,486,28,28},
        {L"BUTTON",L"Combobox forms",BS_GROUPBOX,302,284,278,238},
        {L"COMBOBOX",L"",CBS_DROPDOWNLIST|WS_VSCROLL,316,310,248,132},
        {L"COMBOBOX",L"",CBS_DROPDOWN|WS_VSCROLL,316,348,248,132},
        {L"COMBOBOX",L"",CBS_SIMPLE|WS_VSCROLL,316,386,248,116},
        {L"BUTTON",L"Scrollbars",BS_GROUPBOX,592,284,278,238},
        {L"SCROLLBAR",L"",SBS_HORZ,606,320,190,17},
        {L"SCROLLBAR",L"",SBS_HORZ|WS_DISABLED,606,358,190,17},
        {L"SCROLLBAR",L"",SBS_VERT,834,310,17,186},
        {L"SCROLLBAR",L"",SBS_VERT|WS_DISABLED,808,310,17,186},
        {L"STATIC",L"Enabled / disabled\nHorizontal / vertical\nArrow, track, thumb",SS_LEFT,606,400,190,80},
        {L"STATIC",L"Simple &text",SS_SIMPLE,152,406,120,24},
        {L"STATIC",L"Tab\tcolumn\nNo automatic wrapping",SS_LEFTNOWORDWRAP,26,436,246,44},
        {L"STATIC",L"",SS_ICON,158,80,32,32},
        {L"STATIC",L"App icon",SS_LEFT,198,84,76,24},
        {L"STATIC",L"",SS_BITMAP,242,492,16,16},
        {L"SCROLLBAR",L"",SBS_SIZEBOX|SBS_SIZEBOXTOPLEFTALIGN,606,492,17,17},
        {L"STATIC",L"Size box",SS_LEFT,636,488,160,24}
    };
    win16_style_control_scope *scope=win16_style_controls_begin(w,NULL);
    win16_style_control_counts counts;unsigned n;HMENU bar=CreateMenu(),file=CreatePopupMenu();
    HICON icon=fixture_icon(FALSE);REQUIRE(icon && SetPropW(w,L"ShowcaseIcon",icon));
    HBITMAP bitmap=fixture_bitmap(FALSE);REQUIRE(SetPropW(w,L"ShowcaseBitmap",bitmap));
    REQUIRE(scope && bar && file);
    REQUIRE(AppendMenuW(file,MF_STRING,1400,L"&Example command"));
    REQUIRE(AppendMenuW(file,MF_STRING|MF_CHECKED,1401,L"&Checked command"));
    REQUIRE(AppendMenuW(file,MF_STRING|MF_DISABLED,1402,L"&Disabled command"));
    REQUIRE(AppendMenuW(bar,MF_POPUP,(UINT_PTR)file,L"&File"));REQUIRE(SetMenu(w,bar));
    for(n=0;n<sizeof(items)/sizeof(items[0]);++n){
        BOOL group=!lstrcmpW(items[n].cls,L"BUTTON") && (items[n].style&BS_TYPEMASK)==BS_GROUPBOX;
        BOOL sizebox=!lstrcmpW(items[n].cls,L"SCROLLBAR") && (items[n].style&SBS_SIZEBOX);
        BOOL tab=lstrcmpW(items[n].cls,L"STATIC") && !group && !sizebox;
        HWND control=CreateWindowW(items[n].cls,items[n].text,WS_CHILD|WS_VISIBLE|(tab?WS_TABSTOP:0)|(group?WS_GROUP:0)|items[n].style,
            MulDiv(items[n].x,3,2),MulDiv(items[n].y,3,2),MulDiv(items[n].cx,3,2),MulDiv(items[n].cy,3,2),
            w,(HMENU)(INT_PTR)(1200+n),GetModuleHandleW(NULL),NULL);
        REQUIRE(control);SendMessageW(control,WM_SETFONT,(WPARAM)GetStockObject(SYSTEM_FONT),FALSE);
        if(!lstrcmpW(items[n].cls,L"STATIC") && (items[n].style&SS_TYPEMASK)==SS_ICON)
            SendMessageW(control,STM_SETICON,(WPARAM)icon,0);
        if(!lstrcmpW(items[n].cls,L"STATIC") && (items[n].style&SS_TYPEMASK)==SS_BITMAP)
            SendMessageW(control,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)bitmap);
        if(!lstrcmpW(items[n].cls,L"LISTBOX") || !lstrcmpW(items[n].cls,L"COMBOBOX")){
            BOOL list=!lstrcmpW(items[n].cls,L"LISTBOX");int k;
            if(list && (items[n].style&LBS_MULTICOLUMN))SendMessageW(control,LB_SETCOLUMNWIDTH,168,0);
            if(list && (items[n].style&WS_HSCROLL) && !(items[n].style&LBS_MULTICOLUMN))
                SendMessageW(control,LB_SETHORIZONTALEXTENT,240,0);
            for(k=0;k<16;++k){WCHAR label[32];swprintf_s(label,32,L"Item %02d",k+1);
                REQUIRE(SendMessageW(control,list?LB_ADDSTRING:CB_ADDSTRING,0,(LPARAM)label)>=0);}
            SendMessageW(control,list?LB_SETCURSEL:CB_SETCURSEL,1,0);
            if(list && (items[n].style&LBS_EXTENDEDSEL))SendMessageW(control,LB_SETSEL,TRUE,2);
        }
        if(!lstrcmpW(items[n].cls,L"SCROLLBAR") && !sizebox){
            SCROLLINFO info={sizeof(info),SIF_RANGE|SIF_PAGE|SIF_POS,0,100,20,30,0};SetScrollInfo(control,SB_CTL,&info,TRUE);
        }
    }
    SendMessageW(GetDlgItem(w,1204),BM_SETCHECK,BST_CHECKED,0);
    SendMessageW(GetDlgItem(w,1206),BM_SETCHECK,BST_INDETERMINATE,0);
    SendMessageW(GetDlgItem(w,1208),BM_SETCHECK,BST_CHECKED,0);
    win16_style_controls_end(scope,&counts);
    REQUIRE(counts.installed==sizeof(items)/sizeof(items[0]) && !counts.failed && !counts.unsupported);
    if(test_mode)printf("SHOWCASE PASS %u automatic standard controls, six families in sample 01\n",counts.installed);
    /* Fresh demo-owned list: initial native tabs are known to be default.
     * Explicit attachment does not guess opaque state in arbitrary lists. */
    {HWND list;int item,stop=40;
        list=CreateWindowW(L"LISTBOX",L"",WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER|WS_VSCROLL|LBS_USETABSTOPS|LBS_NOINTEGRALHEIGHT,
            909,324,372,54,w,(HMENU)(INT_PTR)1510,GetModuleHandleW(NULL),NULL);REQUIRE(list);
        REQUIRE(win16_style_tabbed_list_install(list,NULL,0,NULL));
        REQUIRE(SendMessageW(list,LB_SETTABSTOPS,1,(LPARAM)&stop));
        for(item=0;item<16;++item){WCHAR text[40];swprintf_s(text,40,L"Item %02d\tValue %02d",item+1,item+1);
            REQUIRE(SendMessageW(list,LB_ADDSTRING,0,(LPARAM)text)==item);}
        REQUIRE(SendMessageW(list,LB_SETCURSEL,1,0)==1);
        if(test_mode)printf("SHOWCASE PASS explicit known-state tabbed list, two text columns\n");}
}

/* Demo-owned coordinates were authored for 16-row text. This is fixture
 * layout, never an automatic resize policy for attached application HWNDs. */
static void layout_focused_typography(HWND w)
{
    HWND child;RECT outer;REQUIRE(GetWindowRect(w,&outer));
    REQUIRE(SetWindowPos(w,NULL,0,0,MulDiv(outer.right-outer.left,3,2),MulDiv(outer.bottom-outer.top,3,2),
        SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE));
    for(child=GetWindow(w,GW_CHILD);child;child=GetWindow(child,GW_HWNDNEXT)){
        WCHAR cls[32];RECT r;REQUIRE(GetClassNameW(child,cls,32));
        if(!lstrcmpiW(cls,L"ComboLBox"))continue;
        REQUIRE(GetWindowRect(child,&r));
        /* Closed dropdown height excludes its retained list area. Preserve
         * that area when passing a new height to native COMBOBOX. */
        if(!lstrcmpiW(cls,L"COMBOBOX") && (GetWindowLongPtrW(child,GWL_STYLE)&3)!=CBS_SIMPLE)
            REQUIRE(SendMessageW(child,CB_GETDROPPEDCONTROLRECT,0,(LPARAM)&r));
        MapWindowPoints(NULL,w,(POINT *)&r,2);
        REQUIRE(SetWindowPos(child,NULL,MulDiv(r.left,3,2),MulDiv(r.top,3,2),
            MulDiv(r.right-r.left,3,2),MulDiv(r.bottom-r.top,3,2),SWP_NOZORDER|SWP_NOACTIVATE));
    }
}

static void test_focused_typography(HWND w,int special)
{
    HWND child;RECT parent;unsigned checked=0;REQUIRE(GetClientRect(w,&parent));
    for(child=GetWindow(w,GW_CHILD);child;child=GetWindow(child,GW_HWNDNEXT)){
        WCHAR cls[32],label[128];RECT bounds,client,text={0};HDC dc;HGDIOBJ old;TEXTMETRICW tm;
        UINT kind;int padding=0;BOOL button;
        REQUIRE(GetClassNameW(child,cls,32));
        button=!lstrcmpiW(cls,L"BUTTON");
        if(!button && lstrcmpiW(cls,L"STATIC"))continue;
        kind=(UINT)GetWindowLongPtrW(child,GWL_STYLE)&(button?BS_TYPEMASK:SS_TYPEMASK);
        if(!button && kind>SS_RIGHT)continue;
        if(!GetWindowTextW(child,label,128))continue;
        REQUIRE(GetWindowRect(child,&bounds));MapWindowPoints(NULL,w,(POINT *)&bounds,2);
        REQUIRE(bounds.left>=0 && bounds.top>=0 && bounds.right<=parent.right && bounds.bottom<=parent.bottom);
        REQUIRE(GetClientRect(child,&client));dc=GetDC(child);REQUIRE(dc);
        old=SelectObject(dc,(HFONT)SendMessageW(child,WM_GETFONT,0,0));
        REQUIRE(GetTextMetricsW(dc,&tm) && tm.tmHeight==24);
        DrawTextW(dc,label,-1,&text,DT_CALCRECT|DT_SINGLELINE);
        if(button)padding=MulDiv(kind==BS_PUSHBUTTON || kind==BS_DEFPUSHBUTTON || kind==BS_GROUPBOX?8:18,(int)GetDpiForWindow(child),96);
        printf("FONT24 FIT sample=%d id=%d text=%ld available=%ld height=%ld\n",special,GetDlgCtrlID(child),text.right,client.right-padding,client.bottom);
        SelectObject(dc,old);ReleaseDC(child,dc);
        REQUIRE(text.right+padding<=client.right && tm.tmHeight<=client.bottom);++checked;
    }
    printf("FONT24 FOCUSED PASS special=%d labels=%u bounds and actual text extents\n",special,checked);
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
    if(s->special==SHOWCASE){
        REQUIRE(SetWindowPos(w,NULL,40,40,1395,945,SWP_NOZORDER|SWP_NOACTIVATE));
        if(test_mode){test_static_text(w);test_static_icon(w);test_static_bitmap(w);}
        create_showcase(w);
    }
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
    if(s->special==SCROLL || s->special==SHOWCASE){int n;SCROLLINFO info={sizeof(info),SIF_RANGE|SIF_PAGE|SIF_POS,0,100,20,30,0};
        SetScrollInfo(w,SB_HORZ,&info,TRUE);SetScrollInfo(w,SB_VERT,&info,TRUE);
        if(test_mode){
            for(n=0;n<2;++n){SCROLLBARINFO bar={sizeof(bar)};POINT center;RECT client;int which=n?SB_VERT:SB_HORZ;
                REQUIRE(GetScrollBarInfo(w,n?OBJID_VSCROLL:OBJID_HSCROLL,&bar));
                center.x=(bar.rcScrollBar.left+bar.rcScrollBar.right)/2;center.y=(bar.rcScrollBar.top+bar.rcScrollBar.bottom)/2;
                REQUIRE(SendMessageW(w,WM_NCHITTEST,0,MAKELPARAM(center.x,center.y))==(n?HTVSCROLL:HTHSCROLL));
                GetClientRect(w,&client);MapWindowPoints(w,NULL,(POINT *)&client,2);
                REQUIRE(n?bar.rcScrollBar.left>=client.right:bar.rcScrollBar.top>=client.bottom);
                SendMessageW(w,n?WM_VSCROLL:WM_HSCROLL,SB_LINEDOWN,0);info.fMask=SIF_ALL;
                REQUIRE(GetScrollInfo(w,which,&info) && info.nPos==31 && info.nPage==20);
                SendMessageW(w,n?WM_VSCROLL:WM_HSCROLL,SB_LINEUP,0);
            }
            printf("FRAME SCROLL PASS native rectangles, nonclient hit tests, client exclusion and range/position\n");
            if(s->special==SCROLL){
                UINT dpi=GetDpiForWindow(w);int cx=GetSystemMetricsForDpi(SM_CXVSCROLL,dpi);
                int cy=GetSystemMetricsForDpi(SM_CYHSCROLL,dpi),k;win16_style_layout base;
                REQUIRE(win16_style_measure(480,230,dpi,s->style,s->ex,FALSE,&base));
                if(base.sizeable && !base.dialog_frame && !(s->ex&WS_EX_LEFTSCROLLBAR)){
                    cx-=base.unit;cy-=base.unit;}
                for(k=-1;k<=1;++k){RECT calculated;win16_style_layout expected;int width=2*base.border+cx+k;
                    int height=base.client.top+base.border+cy+k;
                    REQUIRE(win16_style_measure(width,height,dpi,s->style,s->ex,FALSE,&expected));
                    SetRect(&calculated,0,0,width,height);SendMessageW(w,WM_NCCALCSIZE,FALSE,(LPARAM)&calculated);
                    REQUIRE(calculated.right-calculated.left==(k>=0?k:cx+k));
                    REQUIRE(calculated.bottom-calculated.top==(k>0?k:cy+k));
                    REQUIRE(calculated.left==expected.client.left && calculated.top==expected.client.top);
                }
                {RECT client_before,client_after;LONG_PTR ex=GetWindowLongPtrW(w,GWL_EXSTYLE);SCROLLBARINFO bar={sizeof(bar)};
                    SetWindowLongPtrW(w,GWL_EXSTYLE,ex|WS_EX_LEFTSCROLLBAR);
                    REQUIRE(SetWindowPos(w,NULL,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED));
                    GetClientRect(w,&client_before);MapWindowPoints(w,NULL,(POINT *)&client_before,2);
                    REQUIRE(GetScrollBarInfo(w,OBJID_VSCROLL,&bar) && bar.rcScrollBar.right<=client_before.left);
                    REQUIRE(win16_style_remove(w));GetClientRect(w,&client_after);MapWindowPoints(w,NULL,(POINT *)&client_after,2);
                    REQUIRE(EqualRect(&client_before,&client_after));REQUIRE(win16_style_install(w,NULL));
                    GetClientRect(w,&client_after);MapWindowPoints(w,NULL,(POINT *)&client_after,2);
                    REQUIRE(EqualRect(&client_before,&client_after));
                    SetWindowLongPtrW(w,GWL_EXSTYLE,ex);
                    REQUIRE(SetWindowPos(w,NULL,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED));
                }
                printf("FRAME SCROLL PASS tiny client thresholds, left bar and remove/reinstall client preservation\n");
            }
        }
    }
    if(s->special==AUTOCONTROLS){
        const WCHAR *classes[]={L"STATIC",L"BUTTON",L"BUTTON",L"BUTTON",L"EDIT",L"COMBOBOX"};
        const WCHAR *labels[]={L"Auto-attached label",L"Auto button",L"Auto check",L"Auto radio",L"Auto edit",L""};
        DWORD styles[]={SS_LEFT,BS_PUSHBUTTON,BS_AUTOCHECKBOX,BS_AUTORADIOBUTTON,WS_BORDER|ES_AUTOHSCROLL,CBS_DROPDOWNLIST};
        win16_style_control_scope *scope=win16_style_controls_begin(w,NULL);win16_style_control_counts counts;int n;
        REQUIRE(scope);
        for(n=0;n<6;++n){HWND control=CreateWindowW(classes[n],labels[n],WS_CHILD|WS_VISIBLE|styles[n],
                n<3?16:240,16+(n%3)*48,200,n==5?100:30,w,(HMENU)(INT_PTR)(1100+n),GetModuleHandleW(NULL),NULL);
            REQUIRE(control);SendMessageW(control,WM_SETFONT,(WPARAM)GetStockObject(SYSTEM_FONT),FALSE);
            if(n==5){SendMessageW(control,CB_ADDSTRING,0,(LPARAM)L"Auto combo");SendMessageW(control,CB_SETCURSEL,0,0);}
            if(test_mode)REQUIRE(!win16_style_control_install(control,NULL) && GetLastError()==ERROR_ALREADY_EXISTS);
        }
        REQUIRE(win16_style_controls_end(scope,&counts));
        if(test_mode){HWND plain;
            printf("AUTO counts installed=%u unsupported=%u failed=%u\n",counts.installed,counts.unsupported,counts.failed);
            REQUIRE(counts.installed==6 && !counts.unsupported && !counts.failed);
            plain=CreateWindowW(L"BUTTON",L"Unobserved",WS_CHILD,0,0,10,10,w,NULL,GetModuleHandleW(NULL),NULL);
            REQUIRE(plain && !win16_style_control_remove(plain));DestroyWindow(plain);
            {HWND root=CreateWindowW(L"STATIC",L"",WS_CHILD,0,0,10,10,w,NULL,GetModuleHandleW(NULL),NULL);
                win16_style_control_scope *outer=win16_style_controls_begin(root,NULL),*inner=win16_style_controls_begin(root,NULL);
                REQUIRE(root && outer && inner);
                plain=CreateWindowW(L"BUTTON",L"",WS_CHILD,0,0,5,5,root,NULL,GetModuleHandleW(NULL),NULL);REQUIRE(plain);
                REQUIRE(win16_style_controls_end(inner,&counts) && counts.installed==1);
                DestroyWindow(root);
                REQUIRE(win16_style_controls_end(outer,&counts) && counts.installed==0);
            }
            printf("CONTROLS PASS automatic creation, scope end, nested precedence and root teardown\n");
        }
    }
    if(s->special==SCROLLCONTROLS){int n;
        for(n=0;n<4;++n){SCROLLINFO info={sizeof(info),SIF_RANGE|SIF_PAGE|SIF_POS,0,100,20,30,0};BOOL vertical=(n&1)!=0;
            HWND scroll=CreateWindowW(L"SCROLLBAR",L"",WS_CHILD|WS_VISIBLE|WS_TABSTOP|(vertical?SBS_VERT:SBS_HORZ),
                vertical?400+(n/2)*36:16,vertical?16:16+n*36,vertical?24:360,vertical?142:24,w,
                (HMENU)(INT_PTR)(1000+n),GetModuleHandleW(NULL),NULL);
            REQUIRE(scroll);SetScrollInfo(scroll,SB_CTL,&info,TRUE);if(n>=2)EnableWindow(scroll,FALSE);
            REQUIRE(win16_style_control_install(scroll,NULL));
            if(test_mode){unsigned command_before=control_commands;
                if(n>=2)test_disabled_scroll_pixels(scroll);
                REQUIRE(GetScrollPos(scroll,SB_CTL)==30);
                if(n==0){SendMessageW(scroll,WM_KEYDOWN,VK_RIGHT,0);
                    REQUIRE(GetScrollPos(scroll,SB_CTL)==31 && control_commands==command_before+1);}
                REQUIRE(win16_style_control_remove(scroll));
                REQUIRE(GetScrollPos(scroll,SB_CTL)==(n==0?31:30));
                REQUIRE(win16_style_control_install(scroll,NULL));}
            if(test_mode && n<2)test_scroll_input(scroll);
        }
        if(test_mode)printf("CONTROLS PASS native scrollbar range/position, keyboard notification and attachment removal\n");
    }
    if(s->special==COMBOS){int n;
        for(n=0;n<4;++n){HWND combo=CreateWindowW(L"COMBOBOX",L"",WS_CHILD|WS_VISIBLE|WS_TABSTOP|
                (n==3?CBS_SIMPLE:n==2?CBS_DROPDOWN:CBS_DROPDOWNLIST),
                n==3?320:16,n==3?16:16+n*50,n==3?130:290,n==3?160:120,w,
                (HMENU)(INT_PTR)(900+n),GetModuleHandleW(NULL),NULL);
            REQUIRE(combo);SendMessageW(combo,WM_SETFONT,(WPARAM)GetStockObject(SYSTEM_FONT),FALSE);
            REQUIRE(SendMessageW(combo,CB_ADDSTRING,0,(LPARAM)L"First selection")==0);
            REQUIRE(SendMessageW(combo,CB_ADDSTRING,0,(LPARAM)L"Second selection")==1);
            SendMessageW(combo,CB_SETCURSEL,1,0);if(n==1)EnableWindow(combo,FALSE);
            REQUIRE(win16_style_control_install(combo,NULL));
            if(test_mode){REQUIRE(SendMessageW(combo,CB_GETCURSEL,0,0)==1);
                REQUIRE(win16_style_control_remove(combo));
                REQUIRE(SendMessageW(combo,CB_GETCOUNT,0,0)==2);
                REQUIRE(win16_style_control_install(combo,NULL));
                if(n>=2){COMBOBOXINFO info={sizeof(info)};WCHAR text[32];DWORD first,last;
                    REQUIRE(GetComboBoxInfo(combo,&info) && info.hwndItem);
                    SetWindowTextW(info.hwndItem,L"abcdef");SendMessageW(info.hwndItem,EM_SETSEL,2,4);
                    SendMessageW(info.hwndItem,EM_REPLACESEL,TRUE,(LPARAM)L"XY");
                    GetWindowTextW(info.hwndItem,text,32);REQUIRE(!lstrcmpW(text,L"abXYef"));
                    REQUIRE(SendMessageW(info.hwndItem,EM_UNDO,0,0));
                    SendMessageW(info.hwndItem,EM_SETSEL,1,3);REQUIRE(win16_style_control_remove(combo));
                    SendMessageW(info.hwndItem,EM_GETSEL,(WPARAM)&first,(LPARAM)&last);
                    REQUIRE(first==1 && last==3);GetWindowTextW(info.hwndItem,text,32);REQUIRE(!lstrcmpW(text,L"abcdef"));
                    REQUIRE(win16_style_control_install(combo,NULL));SetWindowTextW(info.hwndItem,L"Editable text");
                }
                if(n==0){COMBOBOXINFO info={sizeof(info)};REQUIRE(GetComboBoxInfo(combo,&info));
                    REQUIRE(win16_style_control_remove(info.hwndList));
                    REQUIRE(win16_style_control_install(info.hwndList,NULL));
                    REQUIRE(win16_style_control_remove(combo));
                    REQUIRE(!win16_style_control_install(combo,NULL));
                    REQUIRE(win16_style_control_remove(info.hwndList));
                    REQUIRE(win16_style_control_install(combo,NULL));
                }
            }
        }
        if(test_mode)printf("CONTROLS PASS editable combo native replace/undo/selection and independent child attachment ownership\n");
    }
    if(s->special==MULTILIST){
        HWND list=CreateWindowW(L"LISTBOX",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|WS_HSCROLL|WS_TABSTOP|
            LBS_MULTICOLUMN|LBS_NOTIFY,12,16,440,142,w,(HMENU)(INT_PTR)1500,GetModuleHandleW(NULL),NULL);
        RECT item_rects[30],styled_rects[30],item_after;int n,second=-1;LONG_PTR style;LRESULT top,selection;
        REQUIRE(list);SendMessageW(list,WM_SETFONT,(WPARAM)GetStockObject(SYSTEM_FONT),FALSE);
        SendMessageW(list,LB_SETCOLUMNWIDTH,140,0);
        for(n=0;n<30;++n){WCHAR label[80];swprintf_s(label,80,L"Item %02d: long label clipped to its native column",n+1);
            REQUIRE(SendMessageW(list,LB_ADDSTRING,0,(LPARAM)label)==n);}
        for(n=0;n<30;++n){
            REQUIRE(SendMessageW(list,LB_GETITEMRECT,n,(LPARAM)&item_rects[n])!=LB_ERR);
            if(second<0 && item_rects[n].left>0)second=n;}
        REQUIRE(second>0);SendMessageW(list,LB_SETCURSEL,second,0);
        if(test_mode){
            /* Native integral-height LISTBOX may resize on WM_SETFONT. The
             * round trip must match a native oracle with the same font
             * sequence, not assume every old item rectangle stays fixed. */
            HWND oracle=CreateWindowW(L"LISTBOX",L"",WS_CHILD|WS_BORDER|WS_HSCROLL|LBS_MULTICOLUMN|LBS_NOTIFY,
                12,16,440,142,w,NULL,GetModuleHandleW(NULL),NULL);
            REQUIRE(oracle);SendMessageW(oracle,WM_SETFONT,(WPARAM)GetStockObject(SYSTEM_FONT),FALSE);
            SendMessageW(oracle,LB_SETCOLUMNWIDTH,140,0);
            for(n=0;n<30;++n)REQUIRE(SendMessageW(oracle,LB_ADDSTRING,0,(LPARAM)L"Native font oracle")==n);
            SendMessageW(oracle,LB_SETCURSEL,second,0);
            SendMessageW(oracle,WM_SETFONT,(WPARAM)gallery_font,FALSE);
            SendMessageW(oracle,WM_SETFONT,(WPARAM)GetStockObject(SYSTEM_FONT),FALSE);
            for(n=0;n<30;++n)REQUIRE(SendMessageW(oracle,LB_GETITEMRECT,n,(LPARAM)&item_rects[n])!=LB_ERR);
            DestroyWindow(oracle);
        }
        style=GetWindowLongPtrW(list,GWL_STYLE);top=SendMessageW(list,LB_GETTOPINDEX,0,0);
        selection=SendMessageW(list,LB_GETCURSEL,0,0);REQUIRE(win16_style_control_install(list,NULL));
        if(test_mode){
            REQUIRE((HFONT)SendMessageW(list,WM_GETFONT,0,0)==gallery_font);
            for(n=0;n<30;++n){REQUIRE(SendMessageW(list,LB_GETITEMRECT,n,(LPARAM)&styled_rects[n])!=LB_ERR);
                REQUIRE(styled_rects[n].bottom-styled_rects[n].top==24);}
            {HDC dc=CreateCompatibleDC(NULL);BITMAPINFO info={0};HBITMAP bitmap,old;void *pixels;
                win16_style_palette palette;win16_style_default_palette(&palette);
                info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=440;
                info.bmiHeader.biHeight=-142;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;
                bitmap=CreateDIBSection(dc,&info,DIB_RGB_COLORS,&pixels,NULL,0);REQUIRE(bitmap);old=SelectObject(dc,bitmap);
                SendMessageW(list,WM_PRINTCLIENT,(WPARAM)dc,PRF_CLIENT);
                /* The second column must be painted, not skipped after column
                 * one. Its selected background must not spill into column 3. */
                REQUIRE(GetPixel(dc,styled_rects[second].right-3,styled_rects[second].top+1)==palette.active);
                REQUIRE(GetPixel(dc,styled_rects[second].right+3,styled_rects[second].top+1)==palette.light);
                SelectObject(dc,old);DeleteObject(bitmap);DeleteDC(dc);}
            REQUIRE(win16_style_control_remove(list));
            for(n=0;n<30;++n){REQUIRE(SendMessageW(list,LB_GETITEMRECT,n,(LPARAM)&item_after)!=LB_ERR);
                REQUIRE(EqualRect(&item_rects[n],&item_after));}
            REQUIRE(GetWindowLongPtrW(list,GWL_STYLE)==style && SendMessageW(list,LB_GETTOPINDEX,0,0)==top &&
                SendMessageW(list,LB_GETCURSEL,0,0)==selection);
            REQUIRE(win16_style_control_install(list,NULL));
            SendMessageW(list,WM_HSCROLL,SB_LINERIGHT,0);REQUIRE(SendMessageW(list,LB_GETTOPINDEX,0,0)>top);
            REQUIRE(SendMessageW(list,LB_GETCURSEL,0,0)==selection);
            SendMessageW(list,WM_HSCROLL,SB_LEFT,0);REQUIRE(SendMessageW(list,LB_GETTOPINDEX,0,0)==top);
            printf("CONTROLS PASS multicolumn geometry, column painting/clipping, native scroll and attachment state\n");
        }
    }
    if(s->special==LISTS){
        int n,item;const WCHAR *labels[]={L"First",L"Second",L"Third",L"Fourth"};
        for(n=0;n<3;++n){HWND list=CreateWindowW(L"LISTBOX",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP|WS_VSCROLL|
                LBS_NOTIFY|LBS_DISABLENOSCROLL|(n==1?LBS_EXTENDEDSEL:0),12+n*148,16,136,142,w,
                (HMENU)(INT_PTR)(800+n),GetModuleHandleW(NULL),NULL);
            REQUIRE(list);SendMessageW(list,WM_SETFONT,(WPARAM)GetStockObject(SYSTEM_FONT),FALSE);
            for(item=0;item<(n==2?4:12);++item)REQUIRE(SendMessageW(list,LB_ADDSTRING,0,(LPARAM)labels[item%4])==item);
            if(n==1){SendMessageW(list,LB_SETSEL,TRUE,0);SendMessageW(list,LB_SETSEL,TRUE,2);}
            else SendMessageW(list,LB_SETCURSEL,1,0);
            if(n==2)EnableWindow(list,FALSE);
            REQUIRE(win16_style_control_install(list,NULL));
            if(test_mode){LONG_PTR original=GetWindowLongPtrW(list,GWL_STYLE);WCHAR text[32];
                REQUIRE(SendMessageW(list,LB_GETCOUNT,0,0)==(n==2?4:12));
                if(n==1)REQUIRE(SendMessageW(list,LB_GETSELCOUNT,0,0)==2);
                else REQUIRE(SendMessageW(list,LB_GETCURSEL,0,0)==1);
                REQUIRE(SendMessageW(list,LB_GETTEXT,2,(LPARAM)text)==5 && !lstrcmpW(text,L"Third"));
                REQUIRE(win16_style_control_remove(list));
                REQUIRE(GetWindowLongPtrW(list,GWL_STYLE)==original);
                REQUIRE(win16_style_control_install(list,NULL));
                {HDC dc=CreateCompatibleDC(NULL);BITMAPINFO info={0};HBITMAP bitmap,old;void *pixels;
                    info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=136;
                    info.bmiHeader.biHeight=-142;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;
                    bitmap=CreateDIBSection(dc,&info,DIB_RGB_COLORS,&pixels,NULL,0);REQUIRE(bitmap);
                    old=SelectObject(dc,bitmap);SendMessageW(list,WM_PRINT,(WPARAM)dc,PRF_CLIENT|PRF_NONCLIENT|PRF_ERASEBKGND);
                    REQUIRE(GetPixel(dc,0,0)==RGB(0,0,0) && GetPixel(dc,135,141)==RGB(0,0,0));
                    SelectObject(dc,old);DeleteObject(bitmap);DeleteDC(dc);}
                if(n==0){unsigned command_before=control_commands;
                    SendMessageW(list,WM_KEYDOWN,VK_DOWN,0);
                    REQUIRE(SendMessageW(list,LB_GETCURSEL,0,0)==2);
                    REQUIRE(control_commands==command_before+1);
                    SendMessageW(list,LB_SETCURSEL,1,0);
                    REQUIRE(SendMessageW(list,LB_SETTOPINDEX,3,0)!=LB_ERR);
                    REQUIRE(SendMessageW(list,LB_GETTOPINDEX,0,0)==3 && SendMessageW(list,LB_GETCURSEL,0,0)==1);
                    REQUIRE(win16_style_control_remove(list));
                    REQUIRE(SendMessageW(list,LB_GETTOPINDEX,0,0)==3 && win16_style_control_install(list,NULL));
                    SendMessageW(list,LB_SETTOPINDEX,0,0);}
            }
        }
        if(test_mode){HWND custom=CreateWindowW(L"LISTBOX",L"",WS_CHILD|LBS_OWNERDRAWFIXED,
                0,0,10,10,w,NULL,GetModuleHandleW(NULL),NULL);
            REQUIRE(custom && !win16_style_control_install(custom,NULL) && GetLastError()==ERROR_NOT_SUPPORTED);
            DestroyWindow(custom);printf("CONTROLS PASS native single/multiple list state, text, removal and owner-draw exclusion\n");}
    }
    if(s->special==INPUTS){
        const WCHAR *texts[]={L"Single line",L"Read only",L"Disabled",L"secret",L"Multiline\r\nNative editing",L"Borderless input"};
        DWORD kinds[]={ES_AUTOHSCROLL,ES_READONLY,0,ES_PASSWORD|ES_AUTOHSCROLL,
            ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL|WS_VSCROLL|WS_HSCROLL,ES_AUTOHSCROLL};int n;
        for(n=0;n<6;++n){RECT initial,current;LRESULT margins;HWND edit=CreateWindowW(L"EDIT",texts[n],
            WS_CHILD|WS_VISIBLE|(n==5?0:WS_BORDER)|WS_TABSTOP|kinds[n],n>=4?240:16,n==4?12:n==5?154:12+n*36,
            n>=4?200:204,n==4?134:28,w,(HMENU)(INT_PTR)(700+n),GetModuleHandleW(NULL),NULL);
            REQUIRE(edit);SendMessageW(edit,WM_SETFONT,(WPARAM)GetStockObject(SYSTEM_FONT),FALSE);
            if(n==2)EnableWindow(edit,FALSE);GetClientRect(edit,&initial);
            margins=SendMessageW(edit,EM_GETMARGINS,0,0);
            if(test_mode)REQUIRE(!win16_style_control_install(edit,NULL) && GetLastError()==ERROR_NOT_SUPPORTED);
            REQUIRE(win16_style_edit_install(edit,n!=5,NULL));GetClientRect(edit,&current);REQUIRE(EqualRect(&initial,&current));
            REQUIRE(SendMessageW(edit,EM_GETMARGINS,0,0)==margins);
            if(test_mode){RECT outer;HDC capture=CreateCompatibleDC(NULL);BITMAPINFO info={0};HBITMAP bitmap,old;void *pixels;
                REQUIRE(GetWindowRect(edit,&outer));info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
                info.bmiHeader.biWidth=outer.right-outer.left;info.bmiHeader.biHeight=-(outer.bottom-outer.top);
                info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;
                bitmap=CreateDIBSection(capture,&info,DIB_RGB_COLORS,&pixels,NULL,0);REQUIRE(bitmap);old=SelectObject(capture,bitmap);
                SendMessageW(edit,WM_PRINT,(WPARAM)capture,PRF_CLIENT|PRF_NONCLIENT|PRF_ERASEBKGND);
                printf("EDIT BORDER n=%d style=%lx client=%ld,%ld outer=%ld,%ld print=%06lx\n",n,
                    (DWORD)GetWindowLongPtrW(edit,GWL_STYLE),current.right,current.bottom,
                    outer.right-outer.left,outer.bottom-outer.top,GetPixel(capture,0,0));
                if(n==5){
                    /* Borderless EDIT keeps native background policy; compare
                     * the same pixel without attachment, not an assumed white. */
                    COLORREF styled=GetPixel(capture,info.bmiHeader.biWidth-1,-info.bmiHeader.biHeight-1);
                    REQUIRE(win16_style_control_remove(edit));
                    SendMessageW(edit,WM_PRINT,(WPARAM)capture,PRF_CLIENT|PRF_NONCLIENT|PRF_ERASEBKGND);
                    REQUIRE(styled==GetPixel(capture,info.bmiHeader.biWidth-1,-info.bmiHeader.biHeight-1));
                    REQUIRE(win16_style_edit_install(edit,FALSE,NULL));
                }
                else REQUIRE(GetPixel(capture,0,0)==RGB(0,0,0));
                SelectObject(capture,old);DeleteObject(bitmap);DeleteDC(capture);
            }}
        if(test_mode){HWND edit=GetDlgItem(w,700);WCHAR text[64];DWORD first,last;LONG_PTR style=GetWindowLongPtrW(edit,GWL_STYLE);
            REQUIRE(SetWindowTextW(edit,L"abcdef"));SendMessageW(edit,EM_SETSEL,2,4);
            SendMessageW(edit,EM_REPLACESEL,TRUE,(LPARAM)L"XY");GetWindowTextW(edit,text,64);REQUIRE(!lstrcmpW(text,L"abXYef"));
            REQUIRE(SendMessageW(edit,EM_CANUNDO,0,0) && SendMessageW(edit,EM_UNDO,0,0));
            GetWindowTextW(edit,text,64);REQUIRE(!lstrcmpW(text,L"abcdef"));
            SendMessageW(edit,EM_SETSEL,1,3);REQUIRE(win16_style_control_remove(edit));
            SendMessageW(edit,EM_GETSEL,(WPARAM)&first,(LPARAM)&last);REQUIRE(first==1 && last==3);
            REQUIRE(GetWindowLongPtrW(edit,GWL_STYLE)==style && win16_style_edit_install(edit,TRUE,NULL));
            REQUIRE(SetWindowTextW(edit,L"Single line"));SendMessageW(edit,EM_SETSEL,0,0);
            SendMessageW(GetDlgItem(w,701),WM_CHAR,L'X',0);GetWindowTextW(GetDlgItem(w,701),text,64);REQUIRE(!lstrcmpW(text,L"Read only"));
            REQUIRE(SendMessageW(GetDlgItem(w,703),EM_GETPASSWORDCHAR,0,0)!=0);
            REQUIRE(SendMessageW(GetDlgItem(w,704),EM_GETLINECOUNT,0,0)==2);
            printf("CONTROLS PASS native edit selection/replace/undo/read-only/password/multiline and removal preservation\n");}
    }
    if(s->special==BASICS){
        const WCHAR *names[]={L"&Push",L"Default",L"Disabled",L"Options",L"Left label",L"Centre",L"Right",L""};
        DWORD kinds[]={BS_PUSHBUTTON,BS_DEFPUSHBUTTON,BS_PUSHBUTTON,BS_GROUPBOX,SS_LEFT,SS_CENTER,SS_RIGHT,SS_BLACKFRAME};
        RECT positions[]={{12,12,130,48},{146,12,274,48},{290,12,424,48},{12,62,424,156},
            {26,88,150,114},{158,88,276,114},{282,88,410,114},{26,120,410,142}};int n;
        for(n=0;n<8;++n){RECT r=positions[n];HWND control=CreateWindowW(n<4?L"BUTTON":L"STATIC",names[n],
            WS_CHILD|WS_VISIBLE|kinds[n]|(n<3?WS_TABSTOP:0),r.left,r.top,r.right-r.left,r.bottom-r.top,
            w,(HMENU)(INT_PTR)(600+n),GetModuleHandleW(NULL),NULL);
            REQUIRE(control);SendMessageW(control,WM_SETFONT,(WPARAM)GetStockObject(SYSTEM_FONT),FALSE);
            if(n==2)EnableWindow(control,FALSE);REQUIRE(win16_style_control_install(control,NULL));}
        if(test_mode){HWND button=GetDlgItem(w,600);unsigned command_before=control_commands;
            SendMessageW(button,BM_CLICK,0,0);REQUIRE(control_commands==command_before+1);
            SendMessageW(button,BM_SETSTATE,TRUE,0);REQUIRE(SendMessageW(button,BM_GETSTATE,0,0)&BST_PUSHED);
            SendMessageW(button,BM_SETSTATE,FALSE,0);REQUIRE(!(SendMessageW(button,BM_GETSTATE,0,0)&BST_PUSHED));
            REQUIRE(win16_style_control_remove(button) && win16_style_control_install(button,NULL));
            printf("CONTROLS PASS native push notification, pressed state and reinstall\n");}
    }
    if(s->special==CHECKS){
        const WCHAR *labels[]={L"&Checkbox",L"Checked",L"Mixed",L"Disabled checked",L"Radio &A",L"Radio &B"};
        DWORD kinds[]={BS_AUTOCHECKBOX,BS_AUTOCHECKBOX,BS_AUTO3STATE,BS_AUTOCHECKBOX,BS_AUTORADIOBUTTON,BS_AUTORADIOBUTTON};int n;
        for(n=0;n<6;++n){HWND control=CreateWindowW(L"BUTTON",labels[n],WS_CHILD|WS_VISIBLE|WS_TABSTOP|
            (n==0 || n==4?WS_GROUP:0)|kinds[n],16+(n/4)*220,12+(n%4)*34,200,28,w,(HMENU)(INT_PTR)(500+n),GetModuleHandleW(NULL),NULL);
            REQUIRE(control);SendMessageW(control,WM_SETFONT,(WPARAM)GetStockObject(SYSTEM_FONT),FALSE);
            SendMessageW(control,BM_SETCHECK,n==2?BST_INDETERMINATE:n==1 || n==3 || n==4?BST_CHECKED:BST_UNCHECKED,0);
            if(n==3)EnableWindow(control,FALSE);REQUIRE(win16_style_control_install(control,NULL));
        }
        if(test_mode){HWND check=GetDlgItem(w,500),radio=GetDlgItem(w,505);LONG_PTR style=GetWindowLongPtrW(check,GWL_STYLE);
            SendMessageW(check,BM_CLICK,0,0);REQUIRE(SendMessageW(check,BM_GETCHECK,0,0)==BST_CHECKED);
            SendMessageW(check,BM_CLICK,0,0);REQUIRE(SendMessageW(check,BM_GETCHECK,0,0)==BST_UNCHECKED);
            SendMessageW(radio,BM_CLICK,0,0);REQUIRE(SendMessageW(radio,BM_GETCHECK,0,0)==BST_CHECKED);
            REQUIRE(SendMessageW(GetDlgItem(w,504),BM_GETCHECK,0,0)==BST_UNCHECKED);
            REQUIRE(win16_style_control_remove(check) && GetWindowLongPtrW(check,GWL_STYLE)==style);
            REQUIRE(win16_style_control_install(check,NULL));
            printf("CONTROLS PASS checkbox native toggling, radio grouping and style-preserving reinstall\n");}
    }
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
                REQUIRE(win16_style_bar_read(GetMenu(w),gallery_font,GetDpiForWindow(w),
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
                REQUIRE(win16_style_bar_read(GetMenu(w),gallery_font,GetDpiForWindow(w),
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
    if(s->special==SHOWCASE || s->special==LISTS || s->special==COMBOS || s->special==MULTILIST)prepare_owned_lists(w);
    if((s->special>=CHECKS && s->special<=AUTOCONTROLS) || s->special==MULTILIST)layout_focused_typography(w);
    if(test_mode && (s->special==CHECKS || s->special==BASICS || s->special==AUTOCONTROLS))test_focused_typography(w,s->special);
    ShowWindow(w,state&&s->special==MAXIMIZED?SW_MAXIMIZE:state&&s->special==ICONIC?SW_MINIMIZE:SW_SHOWNOACTIVATE);
    UpdateWindow(w);
    if(scroll_test_mode && s->special==SHOWCASE){test_combo_scroll_release(w);test_scroll_focus(w);test_idle_control_damage(w);test_list_scroll_stability(w);test_native_list_paint_boundary(w);test_integrated_list_scroll(w);test_multicolumn_scroll(w);test_list_horizontal_content(w);test_edit_scroll(w);}
    return w;
}

static void close_samples(void)
{
    int i;for(i=0;i<COUNT;++i){if(IsWindow(opened[i]))DestroyWindow(opened[i]);opened[i]=NULL;}
}

static LRESULT CALLBACK controller_proc(HWND w,UINT m,WPARAM a,LPARAM b)
{
    if(m==DM_GETDEFID)return MAKELRESULT(1,DC_HASDEFID);
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
    if(m==WM_NCDESTROY){if(gallery_font){win16_style_font_release(gallery_font);gallery_font=NULL;}}
    return DefWindowProcW(w,m,a,b);
}

static void create_controller(void)
{
    WNDCLASSW c={0};int i;HWND button;
    gallery_font=win16_style_font_acquire();REQUIRE(gallery_font);
    register_classes();c.lpfnWndProc=controller_proc;c.hInstance=GetModuleHandleW(NULL);
    c.hCursor=LoadCursorW(NULL,IDC_ARROW);c.hbrBackground=(HBRUSH)(COLOR_BTNFACE+1);
    c.lpszClassName=L"StyleGalleryControl";REQUIRE(RegisterClassW(&c));
    controller=CreateWindowW(c.lpszClassName,L"Win3.1 frame gallery - select a window to inspect",
        WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,40,40,650,620,NULL,NULL,c.hInstance,NULL);REQUIRE(controller);
    listbox=CreateWindowW(L"LISTBOX",NULL,WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_VSCROLL|WS_BORDER|LBS_NOTIFY,
        16,16,595,490,controller,(HMENU)10,c.hInstance,NULL);REQUIRE(listbox);
    SendMessageW(listbox,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),TRUE);
    for(i=0;i<COUNT;++i)SendMessageW(listbox,LB_ADDSTRING,0,(LPARAM)samples[i].name);
    SendMessageW(listbox,LB_SETCURSEL,0,0);
    button=CreateWindowW(L"BUTTON",L"Open selected",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_DEFPUSHBUTTON,
        16,520,180,30,controller,(HMENU)1,c.hInstance,NULL);REQUIRE(button);
    button=CreateWindowW(L"BUTTON",L"Close samples",WS_CHILD|WS_VISIBLE|WS_TABSTOP,
        215,520,180,30,controller,(HMENU)2,c.hInstance,NULL);REQUIRE(button);
}

/* Application message-loop responsibility, not a style-library input hook. */
static BOOL gallery_message(MSG *message)
{
    HWND root=GetAncestor(message->hwnd,GA_ROOT);WCHAR name[32];const sample *s;
    if(root==controller)return IsDialogMessageW(controller,message);
    if(!GetClassNameW(root,name,32) || lstrcmpW(name,L"StyleGallerySample"))return FALSE;
    s=(const sample *)GetWindowLongPtrW(root,GWLP_USERDATA);if(!s)return FALSE;
    if(s->special==MDI)return TranslateMDISysAccel(GetDlgItem(root,2),message);
    if(s->special<CHECKS)return FALSE;
    if(IsDialogMessageW(root,message))return TRUE;
    if(message->message==WM_KEYDOWN && (message->wParam==VK_F2 || message->wParam==VK_F3 || message->wParam==VK_ESCAPE)){
        SendMessageW(root,message->message,message->wParam,message->lParam);return TRUE;}
    return FALSE;
}
static LRESULT CALLBACK keyboard_command_probe(HWND w,UINT m,WPARAM a,LPARAM b,UINT_PTR id,DWORD_PTR data)
{
    UNREFERENCED_PARAMETER(id);
    if(m==WM_COMMAND && HIWORD(a)==BN_CLICKED && LOWORD(a)==1202 && (HWND)b==GetDlgItem(w,1202))
        ++*(unsigned *)data;
    return DefSubclassProc(w,m,a,b);
}
static void test_showcase_keyboard(HWND w)
{
    MSG message={0};HWND child;unsigned n;BYTE keys[256],clear[256]={0};
    REQUIRE(GetKeyboardState(keys) && SetKeyboardState(clear));
    for(child=GetWindow(w,GW_CHILD);child;child=GetWindow(child,GW_HWNDNEXT)){
        WCHAR cls[16];DWORD style=(DWORD)GetWindowLongPtrW(child,GWL_STYLE);REQUIRE(GetClassNameW(child,cls,16));
        if(!lstrcmpW(cls,L"STATIC") || (!lstrcmpW(cls,L"BUTTON") && (style&BS_TYPEMASK)==BS_GROUPBOX))REQUIRE(!(style&WS_TABSTOP));
    }
    SetFocus(GetDlgItem(w,1201));
    for(n=0;n<2;++n){message.hwnd=GetFocus();message.message=WM_KEYDOWN;message.wParam=VK_TAB;message.lParam=1;
        REQUIRE(gallery_message(&message));REQUIRE(GetFocus()==GetDlgItem(w,n?1204:1202));}
    clear[VK_SHIFT]=0x80;REQUIRE(SetKeyboardState(clear));
    message.hwnd=GetFocus();REQUIRE(gallery_message(&message));REQUIRE(GetFocus()==GetDlgItem(w,1202));
    clear[VK_SHIFT]=0;REQUIRE(SetKeyboardState(clear));
    {unsigned clicks=0;REQUIRE(SetWindowSubclass(w,keyboard_command_probe,1,(DWORD_PTR)&clicks));
        SetFocus(GetDlgItem(w,1204));message.hwnd=GetFocus();message.wParam=VK_RETURN;
        REQUIRE(gallery_message(&message));REQUIRE(clicks==1);
        REQUIRE(RemoveWindowSubclass(w,keyboard_command_probe,1));}
    SetFocus(GetDlgItem(w,1204));
    message.hwnd=GetFocus();message.wParam=VK_SPACE;
    if(!gallery_message(&message))DispatchMessageW(&message);
    message.message=WM_KEYUP;if(!gallery_message(&message))DispatchMessageW(&message);
    REQUIRE(SendMessageW(GetDlgItem(w,1204),BM_GETCHECK,0,0)==BST_UNCHECKED);
    SendMessageW(GetDlgItem(w,1204),BM_SETCHECK,BST_CHECKED,0);
    SetFocus(GetDlgItem(w,1208));message.hwnd=GetFocus();message.message=WM_KEYDOWN;message.wParam=VK_RIGHT;
    REQUIRE(gallery_message(&message));REQUIRE(GetFocus()==GetDlgItem(w,1209));
    REQUIRE(SendMessageW(GetDlgItem(w,1208),BM_GETCHECK,0,0)==BST_UNCHECKED);
    REQUIRE(SendMessageW(GetDlgItem(w,1209),BM_GETCHECK,0,0)==BST_CHECKED);
    SendMessageW(GetDlgItem(w,1208),BM_CLICK,0,0);SetFocus(w);REQUIRE(SetKeyboardState(keys));
    printf("SHOWCASE KEYBOARD PASS Tab/Shift+Tab skip disabled, Enter invokes default, Space toggles, Right selects radio sibling\n");
}
static void test_mdi_keyboard(HWND w)
{
    HWND client=GetDlgItem(w,2),original=(HWND)SendMessageW(client,WM_MDIGETACTIVE,0,0),extra;
    BYTE saved[256],keys[256]={0};MSG message={0};
    REQUIRE(original && GetKeyboardState(saved));
    extra=CreateMDIWindowW(L"StyleGalleryMDI",L"Keyboard route witness",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
        30,30,300,120,client,GetModuleHandleW(NULL),0);REQUIRE(extra);
    REQUIRE(win16_style_install(extra,NULL));
    SendMessageW(client,WM_MDIACTIVATE,(WPARAM)original,0);SetFocus(original);
    keys[VK_CONTROL]=0x80;REQUIRE(SetKeyboardState(keys));
    message.hwnd=original;message.message=WM_KEYDOWN;message.wParam=VK_F6;message.lParam=1;
    REQUIRE(gallery_message(&message));
    REQUIRE((HWND)SendMessageW(client,WM_MDIGETACTIVE,0,0)==extra);
    message.hwnd=extra;message.wParam=VK_F4;REQUIRE(gallery_message(&message));
    REQUIRE(!IsWindow(extra) && IsWindow(original) && IsWindow(w));
    REQUIRE((HWND)SendMessageW(client,WM_MDIGETACTIVE,0,0)==original);
    REQUIRE(SetKeyboardState(saved));
    printf("MDI KEYBOARD PASS Ctrl+F6 activates sibling, Ctrl+F4 closes only active child\n");
}
void win16_style_gallery_run(void)
{
    MSG m;create_controller();
    ShowWindow(controller,SW_SHOW);UpdateWindow(controller);
    SetFocus(listbox);
    while(GetMessageW(&m,NULL,0,0)>0)if(!gallery_message(&m)){TranslateMessage(&m);DispatchMessageW(&m);}
}

void win16_style_scroll_test(void)
{
    HWND sample_window;
    test_mode=TRUE;scroll_test_mode=TRUE;create_controller();
    sample_window=create_sample(0,controller,TRUE);
    REQUIRE(DestroyWindow(sample_window));REQUIRE(DestroyWindow(controller));
    puts("SCROLL FIXTURE COMPLETE: known native tracking defects remain diagnostic, not accepted fixes");
}

typedef struct frame_callback_case {
    int mode;unsigned notices,ends;BOOL fired;HWND transfer;
} frame_callback_case;
static LRESULT CALLBACK frame_callback_proc(HWND w,UINT m,WPARAM a,LPARAM b)
{
    frame_callback_case *test=(frame_callback_case *)GetWindowLongPtrW(w,GWLP_USERDATA);
    if(m==WM_NCCREATE){test=(frame_callback_case *)((CREATESTRUCTW *)b)->lpCreateParams;
        SetWindowLongPtrW(w,GWLP_USERDATA,(LONG_PTR)test);}
    if(test && (m==WM_HSCROLL || m==WM_VSCROLL) && !b){
        ++test->notices;if(LOWORD(a)==SB_ENDSCROLL){++test->ends;return 0;}
        if(!test->fired){test->fired=TRUE;
            switch(test->mode){
            case 0:DestroyWindow(w);break;
            case 1:REQUIRE(win16_style_remove(w));break;
            case 2:{SCROLLBARINFO bar={sizeof(bar)};
                REQUIRE(win16_style_remove(w) && win16_style_install(w,NULL));
                REQUIRE(GetScrollBarInfo(w,OBJID_HSCROLL,&bar));
                SendMessageW(w,WM_NCLBUTTONDOWN,HTHSCROLL,
                    MAKELPARAM(bar.rcScrollBar.left+3,(bar.rcScrollBar.top+bar.rcScrollBar.bottom)/2));break;}
            case 3:SetCapture(test->transfer);break;
            case 4:EnableWindow(w,FALSE);break;
            case 5:SendMessageW(w,WM_CANCELMODE,0,0);break;
            case 6:{SCROLLINFO collapsed={sizeof(collapsed),SIF_RANGE|SIF_PAGE|SIF_POS,0,0,1,0,0};
                win16_style_set_scroll_info(w,SB_VERT,&collapsed,TRUE);break;}
            }
        }
        return 0;
    }
    return DefWindowProcW(w,m,a,b);
}
static void test_frame_callbacks(void)
{
    WNDCLASSW cls={0};int mode;DWORD start_gdi;
    GdiFlush();start_gdi=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);
    cls.lpfnWndProc=frame_callback_proc;cls.hInstance=GetModuleHandleW(NULL);cls.lpszClassName=L"StyleFrameCallbacks";
    REQUIRE(RegisterClassW(&cls) || GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
    for(mode=0;mode<7;++mode){
        frame_callback_case test={0};SCROLLBARINFO bar={sizeof(bar)};POINT p;HWND w;
        SCROLLINFO info={sizeof(info),SIF_RANGE|SIF_PAGE|SIF_POS,0,200,20,30,0};unsigned after;
        test.mode=mode;test.transfer=CreateWindowW(L"STATIC",L"capture owner",WS_POPUP,0,0,20,20,NULL,NULL,cls.hInstance,NULL);
        REQUIRE(test.transfer);
        w=CreateWindowW(cls.lpszClassName,L"callback test",WS_OVERLAPPEDWINDOW|WS_HSCROLL|WS_VSCROLL,
            0,0,400,300,NULL,NULL,cls.hInstance,&test);REQUIRE(w && win16_style_install(w,NULL));
        win16_style_set_scroll_info(w,SB_HORZ,&info,FALSE);win16_style_set_scroll_info(w,SB_VERT,&info,FALSE);
        ShowWindow(w,SW_SHOWNOACTIVATE);UpdateWindow(w);REQUIRE(GetScrollBarInfo(w,OBJID_VSCROLL,&bar));
        p.x=(bar.rcScrollBar.left+bar.rcScrollBar.right)/2;p.y=bar.rcScrollBar.top+3;
        SendMessageW(w,WM_NCLBUTTONDOWN,HTVSCROLL,MAKELPARAM(p.x,p.y));REQUIRE(test.fired);
        if(mode==0)REQUIRE(!IsWindow(w));
        else if(mode==2){REQUIRE(GetCapture()==w);SendMessageW(w,WM_CANCELMODE,0,0);REQUIRE(GetCapture()!=w);}
        else if(mode==3)REQUIRE(GetCapture()==test.transfer);
        else if(mode==6){
            /* Requery layout on the next input; a disappeared bar terminates
             * its existing gesture instead of continuing with stale bounds. */
            SendMessageW(w,WM_MOUSEMOVE,MK_LBUTTON,0);REQUIRE(GetCapture()!=w);
        }else REQUIRE(GetCapture()!=w);
        after=test.notices;
        if(IsWindow(w)){
            SendMessageW(w,WM_MOUSEMOVE,0,0);SendMessageW(w,WM_LBUTTONUP,0,0);
            REQUIRE(test.notices==after);DestroyWindow(w);
        }
        if(GetCapture()==test.transfer)ReleaseCapture();DestroyWindow(test.transfer);
        REQUIRE(!GetCapture());
        if(mode>0)REQUIRE(test.ends==(mode==2?2u:1u));
        printf("FRAME CALLBACK PASS mode=%d notices=%u ends=%u capture/reinstall ownership stable GDI=%lu\n",
            mode,test.notices,test.ends,GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS));
    }
    GdiFlush();
    printf("FRAME CALLBACK GDI baseline=%lu final=%lu\n",start_gdi,GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS));fflush(stdout);
    /* Creating/disabling/destroying native windows can also retire USER's
     * transient resources elsewhere on this GUI thread (observed60->58).
     * Whole-process counts detect growth, not identity of our allocations;
     * the separate surviving-HWND attachment test checks repeated lifetime. */
    REQUIRE(GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS)<=start_gdi);
}

static unsigned frame_repaints;
static LRESULT CALLBACK frame_paint_observer(HWND w,UINT m,WPARAM a,LPARAM b,UINT_PTR id,DWORD_PTR data)
{
    (void)id;(void)data;if(m==WM_NCPAINT || m==WM_PAINT)++frame_repaints;
    return DefSubclassProc(w,m,a,b);
}
static void test_frame_scroll(HWND w)
{
    int axis,dpi=(int)GetDpiForWindow(w);win16_scroll_metrics metrics={MulDiv(17,dpi,96),MulDiv(17,dpi,96),max(1,MulDiv(1,dpi,96))};
    REQUIRE(SetWindowSubclass(w,frame_paint_observer,(UINT_PTR)frame_paint_observer,0));
    for(axis=SB_HORZ;axis<=SB_VERT;++axis){
        SCROLLINFO initial={sizeof(initial),SIF_RANGE|SIF_PAGE|SIF_POS,0,200000,20,30000,0};
        SCROLLINFO result={sizeof(result),SIF_ALL},other={sizeof(other),SIF_ALL};
        SCROLLBARINFO bar={sizeof(bar)},untouched={sizeof(untouched)};RECT r,outer;POINT click,move,marker;win16_scroll_layout layout;HDC dc;
        win16_style_set_scroll_info(w,SB_HORZ,&initial,TRUE);win16_style_set_scroll_info(w,SB_VERT,&initial,TRUE);
        REQUIRE(GetScrollBarInfo(w,axis==SB_VERT?OBJID_VSCROLL:OBJID_HSCROLL,&bar));
        SetRect(&r,0,0,bar.rcScrollBar.right-bar.rcScrollBar.left,bar.rcScrollBar.bottom-bar.rcScrollBar.top);
        REQUIRE(win16_scroll_measure(&r,axis==SB_VERT,&metrics,0,199981,30000,&layout));
        click.x=bar.rcScrollBar.left+(layout.thumb.left+layout.thumb.right)/2;
        click.y=bar.rcScrollBar.top+(layout.thumb.top+layout.thumb.bottom)/2;
        REQUIRE(GetWindowRect(w,&outer) && GetScrollBarInfo(w,axis==SB_VERT?OBJID_HSCROLL:OBJID_VSCROLL,&untouched));
        {win16_style_layout frame;RECT local=bar.rcScrollBar;
            REQUIRE(win16_style_measure(outer.right-outer.left,outer.bottom-outer.top,(UINT)dpi,
                (DWORD)GetWindowLongPtrW(w,GWL_STYLE),(DWORD)GetWindowLongPtrW(w,GWL_EXSTYLE),FALSE,&frame));
            OffsetRect(&local,-outer.left,-outer.top);
            if(frame.sizeable && !frame.dialog_frame &&
                !(GetWindowLongPtrW(w,GWL_EXSTYLE)&WS_EX_LEFTSCROLLBAR)){
                int inner=frame.border-frame.unit;POINT edge;
                /* Independent USER18 outer-edge oracle: N=F-B, not F.
                 * Probe the newly shared band, not just the arrow center. */
                REQUIRE(axis==SB_VERT?local.right==frame.outer.right-inner:
                    local.bottom==frame.outer.bottom-inner);
                edge.x=axis==SB_VERT?bar.rcScrollBar.right-1:(bar.rcScrollBar.left+bar.rcScrollBar.right)/2;
                edge.y=axis==SB_VERT?(bar.rcScrollBar.top+bar.rcScrollBar.bottom)/2:bar.rcScrollBar.bottom-1;
                REQUIRE(SendMessageW(w,WM_NCHITTEST,0,MAKELPARAM(edge.x,edge.y))==
                    (axis==SB_VERT?HTVSCROLL:HTHSCROLL));
            }
            printf("FRAME METRICS axis=%d dpi=%d border=%d unit=%d bar=%ld,%ld,%ld,%ld arrow=%d "
                "bottom-strokes-x=%ld,%ld side-strokes-y=%ld,%ld\n",axis,dpi,frame.border,frame.unit,
                local.left,local.top,local.right,local.bottom,metrics.arrow,
                frame.corner.cx-frame.unit,frame.outer.right-frame.corner.cx,
                frame.corner.cy-frame.unit,frame.outer.bottom-frame.corner.cy);}
        marker.x=untouched.rcScrollBar.left-outer.left+3;marker.y=untouched.rcScrollBar.top-outer.top+3;
        dc=GetWindowDC(w);REQUIRE(dc && SetPixelV(dc,marker.x,marker.y,RGB(1,2,3)));ReleaseDC(w,dc);
        frame_repaints=0;
        SendMessageW(w,WM_NCLBUTTONDOWN,axis==SB_VERT?HTVSCROLL:HTHSCROLL,MAKELPARAM(click.x,click.y));
        REQUIRE(GetCapture()==w);
        move=click;if(axis==SB_VERT)move.y+=layout.travel/2;else move.x+=layout.travel/2;
        REQUIRE(ScreenToClient(w,&move));
        SendMessageW(w,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(move.x,move.y));
        REQUIRE(win16_style_get_scroll_info(w,axis,&result) && result.nTrackPos>65535 && result.nPos==result.nTrackPos);
        REQUIRE(GetScrollInfo(w,axis==SB_VERT?SB_HORZ:SB_VERT,&other) && other.nPos==30000);
        SendMessageW(w,WM_LBUTTONUP,0,MAKELPARAM(move.x,move.y));REQUIRE(GetCapture()!=w);
        REQUIRE(frame_repaints==0);
        dc=GetWindowDC(w);REQUIRE(dc && GetPixel(dc,marker.x,marker.y)==RGB(1,2,3));ReleaseDC(w,dc);
        printf("FRAME SCROLL PASS axis=%d position=%ld 32-bit query, independent axis, capture release, frame/client paints=%u\n",
            axis,result.nPos,frame_repaints);
    }
    RemoveWindowSubclass(w,frame_paint_observer,(UINT_PTR)frame_paint_observer);
    {SCROLLBARINFO bar={sizeof(bar)};POINT click;int cancel;
        for(cancel=0;cancel<2;++cancel){
            REQUIRE(GetScrollBarInfo(w,OBJID_VSCROLL,&bar));
            click.x=(bar.rcScrollBar.left+bar.rcScrollBar.right)/2;click.y=bar.rcScrollBar.top+3;
            SendMessageW(w,WM_NCLBUTTONDOWN,HTVSCROLL,MAKELPARAM(click.x,click.y));REQUIRE(GetCapture()==w);
            if(cancel)REQUIRE(win16_style_remove(w));else SendMessageW(w,WM_CANCELMODE,0,0);
            REQUIRE(GetCapture()!=w);
            if(cancel){SCROLLINFO native={sizeof(native),SIF_ALL},adapted={sizeof(adapted),SIF_ALL};
                REQUIRE(GetScrollInfo(w,SB_VERT,&native) && win16_style_get_scroll_info(w,SB_VERT,&adapted));
                REQUIRE(native.nPos==adapted.nPos && native.nTrackPos==adapted.nTrackPos);
                REQUIRE(win16_style_install(w,NULL));}
        }
        printf("FRAME CLEANUP PASS cancel, active detach, native query fallback and reinstall\n");}
    {SCROLLINFO initial={sizeof(initial),SIF_RANGE|SIF_PAGE|SIF_POS,0,100,20,30,0};
        win16_style_set_scroll_info(w,SB_HORZ,&initial,TRUE);win16_style_set_scroll_info(w,SB_VERT,&initial,TRUE);}
}

static void test_late_frame_scroll(HWND parent)
{
    HWND w;int axis;SCROLLINFO initial={sizeof(initial),SIF_RANGE|SIF_PAGE|SIF_POS,0,200000,20,30000,0};
    w=CreateWindowW(L"StyleGallerySample",L"Late frame scrollbars",WS_OVERLAPPEDWINDOW,
        0,0,480,340,parent,NULL,GetModuleHandleW(NULL),(void *)&samples[0]);
    REQUIRE(w && win16_style_install(w,NULL));
    REQUIRE(!(GetWindowLongPtrW(w,GWL_STYLE)&(WS_HSCROLL|WS_VSCROLL)));
    ShowWindow(w,SW_SHOWNOACTIVATE);
    for(axis=SB_HORZ;axis<=SB_VERT;++axis){SCROLLBARINFO bar={sizeof(bar)};POINT click;
        win16_style_set_scroll_info(w,axis,&initial,TRUE);REQUIRE(ShowScrollBar(w,axis,TRUE));
        REQUIRE(GetScrollBarInfo(w,axis==SB_HORZ?OBJID_HSCROLL:OBJID_VSCROLL,&bar));
        printf("LATE FRAME axis=%d style=%08lx state=%08lx rect=%ld,%ld,%ld,%ld\n",axis,
            (DWORD)GetWindowLongPtrW(w,GWL_STYLE),bar.rgstate[0],bar.rcScrollBar.left,bar.rcScrollBar.top,bar.rcScrollBar.right,bar.rcScrollBar.bottom);fflush(stdout);
        REQUIRE(!(bar.rgstate[0]&STATE_SYSTEM_INVISIBLE) && !IsRectEmpty(&bar.rcScrollBar));
        click.x=bar.rcScrollBar.left+3;click.y=bar.rcScrollBar.top+3;
        /* Bound a native modal-tracker negative control. The custom tracker
         * must return before consuming this queued release. */
        REQUIRE(PostMessageW(w,WM_LBUTTONUP,0,0));
        SendMessageW(w,WM_NCLBUTTONDOWN,axis==SB_HORZ?HTHSCROLL:HTVSCROLL,MAKELPARAM(click.x,click.y));
        {MSG queued;PeekMessageW(&queued,w,WM_LBUTTONUP,WM_LBUTTONUP,PM_REMOVE);}
        REQUIRE(GetCapture()==w);SendMessageW(w,WM_CANCELMODE,0,0);REQUIRE(GetCapture()!=w);
        REQUIRE(GetScrollPos(w,axis)==29999);
        REQUIRE(ShowScrollBar(w,axis,FALSE));REQUIRE(ShowScrollBar(w,axis,TRUE));
    }
    test_frame_scroll(w);
    REQUIRE(DestroyWindow(w));
    puts("LATE FRAME SCROLL PASS both axes added after attach, hide/show, 32-bit tracking and removal");
}

static void test_menu_font(HWND w)
{
    HFONT control=(HFONT)SendMessageW(GetDlgItem(w,1201),WM_GETFONT,0,0);
    HFONT old_title=CreateFontW(16,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,
        DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,DEFAULT_PITCH,L"System");
    HDC dc=CreateCompatibleDC(NULL);HGDIOBJ saved;TEXTMETRICW tm,old_tm;
    RECT outer;POINT origin,client={0,0};win16_style_layout frame;win16_style_bar bar;
    UINT dpi=GetDpiForWindow(w);
    REQUIRE(control && old_title && dc);
    saved=SelectObject(dc,old_title);REQUIRE(GetTextMetricsW(dc,&old_tm));
    SelectObject(dc,control);REQUIRE(GetTextMetricsW(dc,&tm) && tm.tmHeight==24 && control==gallery_font);
    SelectObject(dc,saved);DeleteDC(dc);DeleteObject(old_title);
    REQUIRE(GetWindowRect(w,&outer));
    REQUIRE(win16_style_measure(outer.right-outer.left,outer.bottom-outer.top,dpi,
        (DWORD)GetWindowLongPtrW(w,GWL_STYLE),(DWORD)GetWindowLongPtrW(w,GWL_EXSTYLE),FALSE,&frame));
    origin.x=frame.client.left;origin.y=frame.client.top;
    REQUIRE(win16_style_bar_read(GetMenu(w),control,dpi,origin,frame.client.right-frame.client.left,&bar));
    REQUIRE(ClientToScreen(w,&client));
    /* Layout must measure the same realized font as the controls, not the
     * unscaled title raster. Otherwise both text and its hit cell shrink. */
    REQUIRE(client.y-outer.top==origin.y+bar.height+frame.unit);
    {POINT p={outer.left+bar.rects[0].right-1,outer.top+bar.rects[0].bottom-1};
        REQUIRE(SendMessageW(w,WM_NCHITTEST,0,MAKELPARAM(p.x,p.y))==HTMENU);}
    printf("MENU FONT PASS dpi=%u control/menu-height=%ld former-title-height=%ld bar-height=%d\n",
        dpi,tm.tmHeight,old_tm.tmHeight,bar.height);
    win16_style_bar_free(&bar);
    {HWND button=CreateWindowW(L"BUTTON",L"Font lifetime",WS_CHILD,0,0,180,40,w,NULL,GetModuleHandleW(NULL),NULL);
        HFONT borrowed=(HFONT)GetStockObject(DEFAULT_GUI_FONT);LOGFONTW before,after;
        REQUIRE(button && GetObjectW(borrowed,sizeof(before),&before)==sizeof(before));
        REQUIRE(win16_style_control_install(button,NULL));
        SendMessageW(button,WM_SETFONT,(WPARAM)borrowed,FALSE);
        REQUIRE((HFONT)SendMessageW(button,WM_GETFONT,0,0)==gallery_font);
        REQUIRE(win16_style_control_remove(button));
        REQUIRE((HFONT)SendMessageW(button,WM_GETFONT,0,0)==borrowed);
        REQUIRE(GetObjectW(borrowed,sizeof(after),&after)==sizeof(after) && !memcmp(&before,&after,sizeof(before)));
        DestroyWindow(button);puts("FONT24 BINDING PASS effective font remains24; latest borrowed application font restored intact");}
}

void win16_style_capture_test(const WCHAR *path)
{
    HWND w;test_mode=TRUE;menu_capture_path=path;create_controller();w=create_sample(0,controller,FALSE);
    REQUIRE(SetWindowPos(w,NULL,0,0,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE));
    UpdateWindow(w);test_combo_scroll_release(w);test_frame_scroll(w);test_frame_callbacks();test_late_frame_scroll(w);capture_menu(w,L"all-controls-children",FALSE);
    capture_menu(w,L"all-controls-message",FALSE);capture_menu(w,L"all-controls",FALSE);
    /* Keep both capture routes: an explicit repaint distinguishes stale
     * screen contents from duplication inside the WM_PRINT implementation. */
    RedrawWindow(w,NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_FRAME|RDW_ALLCHILDREN|RDW_UPDATENOW);
    capture_menu(w,L"all-controls-screen",FALSE);capture_menu(w,L"all-controls-refreshed",FALSE);
    /* Diagnostic only: test whether parent/child clipping changes the native
     * capture. This does not change the interactive gallery or style library. */
    SetWindowLongPtrW(w,GWL_STYLE,GetWindowLongPtrW(w,GWL_STYLE)|WS_CLIPCHILDREN);
    RedrawWindow(w,NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_FRAME|RDW_ALLCHILDREN|RDW_UPDATENOW);
    capture_menu(w,L"all-controls-clipped",FALSE);
    DestroyWindow(w);DestroyWindow(controller);printf("CAPTURE PROBE COMPLETE\n");
}

void win16_style_gallery_test(const WCHAR *path)
{
    int page,i;test_mode=TRUE;menu_capture_path=path;create_controller();
    for(page=0;page<(COUNT+3)/4;++page){
        BITMAPINFO bi={0};BITMAPFILEHEADER fh={0};HDC dc=CreateCompatibleDC(NULL),part=CreateCompatibleDC(NULL);
        int cell_width=page==0?1435:page>=4?765:525,cell_height=page==0?995:page>=4?405:295;
        int atlas_width=2*cell_width+20,atlas_height=2*cell_height+20;
        HBITMAP bm,old;void *pixels;RECT bg={0,0,atlas_width,atlas_height};WCHAR name[MAX_PATH];DWORD bytes;HANDLE out;
        bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bi.bmiHeader.biWidth=atlas_width;bi.bmiHeader.biHeight=-atlas_height;
        bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;
        bm=CreateDIBSection(dc,&bi,DIB_RGB_COLORS,&pixels,NULL,0);REQUIRE(bm);old=SelectObject(dc,bm);
        FillRect(dc,&bg,(HBRUSH)GetStockObject(LTGRAY_BRUSH));
        for(i=page*4;i<min(COUNT,page*4+4);++i){
            HWND w=create_sample(i,controller,FALSE);RECT r;HBITMAP shot,prior;int width,height,x,y;
            REQUIRE(SetWindowPos(w,NULL,0,0,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE));
            if(samples[i].special==SHOWCASE){test_showcase_keyboard(w);test_menu_font(w);UpdateWindow(w);test_frame_scroll(w);test_frame_callbacks();test_late_frame_scroll(w);capture_menu(w,L"all-controls",FALSE);
                capture_menu(w,L"all-controls-message",FALSE);menu_captured=FALSE;}
            if(samples[i].special==OWNED)REQUIRE(GetWindow(w,GW_OWNER)==controller);
            if(samples[i].special==MDI)test_mdi_keyboard(w);
            if(samples[i].special==COMBOS){
                HWND combo=GetDlgItem(w,900);COMBOBOXINFO info={sizeof(info)};
                SetActiveWindow(w);SetFocus(combo);SendMessageW(combo,CB_SETCURSEL,0,0);
                SendMessageW(combo,CB_SHOWDROPDOWN,TRUE,0);
                REQUIRE(SendMessageW(combo,CB_GETDROPPEDSTATE,0,0) && GetComboBoxInfo(combo,&info));
                REQUIRE(IsWindowVisible(info.hwndList));UpdateWindow(info.hwndList);
                capture_menu(info.hwndList,L"combo-list",TRUE);menu_captured=FALSE;
                SendMessageW(combo,WM_KEYDOWN,VK_DOWN,0);SendMessageW(combo,WM_KEYUP,VK_DOWN,0);
                REQUIRE(SendMessageW(combo,CB_GETCURSEL,0,0)==1);
                SendMessageW(combo,CB_SHOWDROPDOWN,FALSE,0);
                REQUIRE(!SendMessageW(combo,CB_GETDROPPEDSTATE,0,0));
                printf("CONTROLS PASS native dropdown visibility, styled popup capture, Down selection and closure\n");
            }
            if(samples[i].retro && (samples[i].style&WS_SYSMENU)){
                int route;
                WCHAR diagnostic[2];
                int routes=GetEnvironmentVariableW(L"WIN16_STYLE_MOUSE_DIAGNOSTIC",diagnostic,2)?2:1;
                if(routes==1)printf("OUTER MOUSE NOT VERIFIED: opt-in diagnostic requires input-state witness\n");
                for(route=0;route<routes;++route){
                MENUITEMINFOW before={sizeof(before)},after={sizeof(after)};
                before.fMask=after.fMask=MIIM_FTYPE|MIIM_DATA|MIIM_BITMAP;
                REQUIRE(GetMenuItemInfoW(GetSystemMenu(w,FALSE),SC_CLOSE,FALSE,&before));
                menu_test_action=13;menu_test_step=i+1;menu_captured=FALSE;menu_test_timeout=FALSE;
                outer_menu_started=GetTickCount64();outer_menu_capture_ms=0;
                outer_menu_idle_count=outer_menu_init_count=outer_menu_exit_count=0;
                menu_test_mouse=route!=0;
                SetActiveWindow(w);SetFocus(w);REQUIRE(SetTimer(w,97,1000,NULL));
                if(!route)SendMessageW(w,WM_SYSCOMMAND,SC_KEYMENU,L' ');
                else {
                    RECT bounds;win16_style_layout frame;POINT point;MENUBARINFO native={sizeof(native)};MSG message;
                    REQUIRE(GetWindowRect(w,&bounds));
                    REQUIRE(win16_style_measure(bounds.right-bounds.left,bounds.bottom-bounds.top,
                        GetDpiForWindow(w),(DWORD)GetWindowLongPtrW(w,GWL_STYLE),samples[i].ex,FALSE,&frame));
                    point.x=bounds.left+(frame.system.left+frame.system.right)/2;
                    point.y=bounds.top+(frame.system.top+frame.system.bottom)/2;
                    REQUIRE(GetMenuBarInfo(w,OBJID_SYSMENU,0,&native));
                    if(IsRectEmpty(&native.rcBar)){
                        SendMessageW(w,WM_NCLBUTTONDOWN,HTSYSMENU,MAKELPARAM(point.x,point.y));
                        REQUIRE(ScreenToClient(w,&point));
                        SendMessageW(w,WM_LBUTTONUP,0,MAKELPARAM(point.x,point.y));
                    }else {
                        /* Enter USER's mouse-menu command directly: posting a
                         * release before NC down does not model held-button
                         * state and can cancel before any popup is observed.
                         * This is command-route evidence, not physical input. */
                        SendMessageW(w,WM_SYSCOMMAND,SC_MOUSEMENU|HTSYSMENU,MAKELPARAM(point.x,point.y));
                    }
                    while(PeekMessageW(&message,w,WM_SYSCOMMAND,WM_SYSCOMMAND,PM_REMOVE))DispatchMessageW(&message);
                }
                KillTimer(w,97);menu_test_action=0;
                printf("OUTER MENU TIMING sample=%d route=%s elapsed=%llu capture_ms=%llu init=%u idle=%u exit=%u captured=%d timeout=%d window=%d\n",
                    i+1,route?"mouse":"key",GetTickCount64()-outer_menu_started,outer_menu_capture_ms,
                    outer_menu_init_count,outer_menu_idle_count,outer_menu_exit_count,menu_captured,menu_test_timeout,IsWindow(w));fflush(stdout);
                REQUIRE(!menu_test_timeout && menu_captured && IsWindow(w));menu_captured=FALSE;
                REQUIRE(GetMenuItemInfoW(GetSystemMenu(w,FALSE),SC_CLOSE,FALSE,&after));
                REQUIRE(before.fType==after.fType && before.dwItemData==after.dwItemData && before.hbmpItem==after.hbmpItem);
                printf("OUTER MENU CAPTURE sample=%d route=%s restored=1\n",i+1,route?"mouse":"key");
                }
            }
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
                if(GetMenu(w)){p.x=10;p.y=10;ClientToScreen(w,&p);}
                REQUIRE(SendMessageW(w,WM_NCHITTEST,0,MAKELPARAM(p.x,p.y))==HTCLIENT);
                {int shared=l.sizeable && !l.dialog_frame && !(samples[i].ex&WS_EX_LEFTSCROLLBAR)?l.unit:0;
                    GetClientRect(w,&actual);REQUIRE(actual.right==l.client.right-l.client.left-
                        ((samples[i].style&WS_VSCROLL)?GetSystemMetricsForDpi(SM_CXVSCROLL,GetDpiForWindow(w))-shared:0));
                    if(!GetMenu(w))REQUIRE(actual.bottom==l.client.bottom-l.client.top-
                        ((samples[i].style&WS_HSCROLL)?GetSystemMetricsForDpi(SM_CYHSCROLL,GetDpiForWindow(w))-shared:0));}
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
                        HFONT font=gallery_font;
                        REQUIRE(font);
                        REQUIRE(GetWindowRect(w,&outer));
                        REQUIRE(win16_style_measure(outer.right-outer.left,outer.bottom-outer.top,GetDpiForWindow(w),
                            (DWORD)GetWindowLongPtrW(w,GWL_STYLE),(DWORD)GetWindowLongPtrW(w,GWL_EXSTYLE),FALSE,&base));
                        origin.x=base.client.left;origin.y=base.client.top;
                        REQUIRE(win16_style_bar_read(GetMenu(w),font,GetDpiForWindow(w),
                            origin,base.client.right-base.client.left,&bar));
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
            REQUIRE(PrintWindow(w,part,0));x=20+(i%2)*cell_width;y=30+((i%4)/2)*cell_height;
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
        fh.bfType=0x4d42;fh.bfOffBits=sizeof(fh)+sizeof(bi.bmiHeader);fh.bfSize=fh.bfOffBits+atlas_width*atlas_height*4;
        out=CreateFileW(name,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);REQUIRE(out!=INVALID_HANDLE_VALUE);
        REQUIRE(WriteFile(out,&fh,sizeof(fh),&bytes,NULL));REQUIRE(WriteFile(out,&bi.bmiHeader,sizeof(bi.bmiHeader),&bytes,NULL));
        REQUIRE(WriteFile(out,pixels,atlas_width*atlas_height*4,&bytes,NULL));CloseHandle(out);
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
