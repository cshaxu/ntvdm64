/* Thin NTW16S demonstration. No renderer or automatic test harness. */

#include "ntw16s-dll/ntw16s.h"

#include <windowsx.h>

#include <commctrl.h>

#include <stdlib.h>

#include <stdio.h>

typedef struct sample {
    const WCHAR *name;
    DWORD style,ex;
    BOOL retro;
    int special;
} sample;
enum {NORMAL,OWNED,MENU,SCROLL,CHILD,MDI,ICONIC,MAXIMIZED,CHECKS,BASICS,INPUTS,LISTS,COMBOS,SCROLLCONTROLS,AUTOCONTROLS,SHOWCASE,MULTILIST,DIALOGCONTROLS};
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
    {L"27 Multicolumn list [trial]",WS_OVERLAPPEDWINDOW,0,TRUE,MULTILIST},
    {L"28 Dialog background / links",WS_POPUP|WS_CAPTION|WS_SYSMENU,0,TRUE,DIALOGCONTROLS}
};
#define COUNT ((int)(sizeof(samples)/sizeof(samples[0])))
#define REQUIRE(x) do{if(!(x)){MessageBoxW(NULL,L"Unable to create the selected demonstration.",L"W16Style",MB_ICONERROR);ExitProcess(1);}}while(0)
static HWND controller,listbox,opened[COUNT];
static HFONT gallery_font;


static const WCHAR *menu_labels[]={L"&Example command\tCtrl+E",L"&Disabled command",L"",L"&Close\tAlt+F4",L"&More",L"&Nested command",L"&File"};
#define MENU_LABEL_COUNT (sizeof(menu_labels)/sizeof(menu_labels[0]))
static win16_style_menu_kind menu_kind(ULONG_PTR data)
{
    return data==2?WIN16_STYLE_MENU_SEPARATOR:data==6?WIN16_STYLE_MENU_BAR:WIN16_STYLE_MENU_ROW;
}


static unsigned menu_measure_count,menu_draw_count,menu_command_count,control_commands;

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

static LRESULT CALLBACK mdi_proc(HWND w,UINT m,WPARAM a,LPARAM b)
{
    if(m==WM_PAINT){PAINTSTRUCT ps;RECT r;HDC dc=BeginPaint(w,&ps);const WCHAR *text=L"Retro child; native MDI management";
        GetClientRect(w,&r);FillRect(dc,&r,(HBRUSH)GetStockObject(WHITE_BRUSH));
        TextOutW(dc,12,12,text,lstrlenW(text));EndPaint(w,&ps);return 0;}
    return DefMDIChildProcW(w,m,a,b);
}

static void showcase_move(HWND w,int dx,int dy)
{
    HWND child;RECT r;
    if(!dx && !dy)return;
    /* Move every direct child, including those currently outside the viewport.
     * ScrollWindowEx's partially visible child handling is not sufficient. */
    for(child=GetWindow(w,GW_CHILD);child;child=GetWindow(child,GW_HWNDNEXT)){
        GetWindowRect(child,&r);MapWindowPoints(NULL,w,(POINT *)&r,2);
        SetWindowPos(child,NULL,r.left+dx,r.top+dy,0,0,
            SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOREDRAW);
    }
    RedrawWindow(w,NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN);
}

static void showcase_extent(HWND w)
{
    HWND child;RECT r,client;int x=GetScrollPos(w,SB_HORZ),y=GetScrollPos(w,SB_VERT),right=0,bottom=0;
    SCROLLINFO info={sizeof(info),SIF_RANGE|SIF_PAGE|SIF_POS|SIF_DISABLENOSCROLL};
    if(!GetPropW(w,L"ShowcaseReady"))return;
    for(child=GetWindow(w,GW_CHILD);child;child=GetWindow(child,GW_HWNDNEXT)){
        GetWindowRect(child,&r);MapWindowPoints(NULL,w,(POINT *)&r,2);
        right=max(right,r.right+x);bottom=max(bottom,r.bottom+y);
    }
    GetClientRect(w,&client);info.nMax=right+15-1;info.nPage=(UINT)client.right;info.nPos=x;
    win16_style_set_scroll_info(w,SB_HORZ,&info,TRUE);
    info.nMax=bottom+15-1;info.nPage=(UINT)client.bottom;info.nPos=y;
    win16_style_set_scroll_info(w,SB_VERT,&info,TRUE);
    showcase_move(w,x-GetScrollPos(w,SB_HORZ),y-GetScrollPos(w,SB_VERT));
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
    if(s && s->special==SHOWCASE && m==WM_SIZE && a!=SIZE_MINIMIZED)showcase_extent(w);
    if(s && s->special==SHOWCASE && (m==WM_MOUSEWHEEL || m==WM_MOUSEHWHEEL)){
        int steps=GET_WHEEL_DELTA_WPARAM(a)/WHEEL_DELTA;
        UINT command=steps>0?SB_LINEUP:SB_LINEDOWN;
        if(m==WM_MOUSEHWHEEL)command=steps>0?SB_LINEDOWN:SB_LINEUP;
        for(steps=abs(steps)*3;steps;--steps)SendMessageW(w,m==WM_MOUSEWHEEL?WM_VSCROLL:WM_HSCROLL,command,0);
        return 0;
    }
    if(s && s->special==MENU && m==WM_MENUCHAR){
        return menu_mnemonic((HMENU)b,(WCHAR)LOWORD(a));
    }
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
        int bar=m==WM_HSCROLL?SB_HORZ:SB_VERT,old,step=s->special==SHOWCASE?20:1;SCROLLINFO info={sizeof(info),SIF_ALL};
        REQUIRE(win16_style_get_scroll_info(w,bar,&info));
        old=info.nPos;
        switch(LOWORD(a)){
        case SB_LINEUP:info.nPos-=step;break;case SB_LINEDOWN:info.nPos+=step;break;
        case SB_PAGEUP:info.nPos-=(int)info.nPage;break;case SB_PAGEDOWN:info.nPos+=(int)info.nPage;break;
        case SB_TOP:info.nPos=info.nMin;break;case SB_BOTTOM:info.nPos=info.nMax;break;
        case SB_THUMBTRACK:case SB_THUMBPOSITION:info.nPos=info.nTrackPos;break;
        default:return 0;}
        ++control_commands;info.fMask=SIF_POS;win16_style_set_scroll_info(w,bar,&info,TRUE);
        if(s->special==SHOWCASE){int delta=old-GetScrollPos(w,bar);
            showcase_move(w,bar==SB_HORZ?delta:0,bar==SB_VERT?delta:0);}
        else InvalidateRect(w,NULL,FALSE);
        return 0;
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
        if(s && s->special==SCROLL)OffsetRect(&r,-GetScrollPos(w,SB_HORZ),-GetScrollPos(w,SB_VERT));
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
            MulDiv(items[n].x,5,4),MulDiv(items[n].y,5,4),MulDiv(items[n].cx,5,4),MulDiv(items[n].cy,5,4),
            w,(HMENU)(INT_PTR)(1200+n),GetModuleHandleW(NULL),NULL);
        REQUIRE(control);SendMessageW(control,WM_SETFONT,(WPARAM)GetStockObject(SYSTEM_FONT),FALSE);
        if(!lstrcmpW(items[n].cls,L"STATIC") && (items[n].style&SS_TYPEMASK)==SS_ICON)
            SendMessageW(control,STM_SETICON,(WPARAM)icon,0);
        if(!lstrcmpW(items[n].cls,L"STATIC") && (items[n].style&SS_TYPEMASK)==SS_BITMAP)
            SendMessageW(control,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)bitmap);
        if(!lstrcmpW(items[n].cls,L"LISTBOX") || !lstrcmpW(items[n].cls,L"COMBOBOX")){
            BOOL list=!lstrcmpW(items[n].cls,L"LISTBOX");int k;
            if(list && (items[n].style&LBS_MULTICOLUMN))SendMessageW(control,LB_SETCOLUMNWIDTH,140,0);
            if(list && (items[n].style&WS_HSCROLL) && !(items[n].style&LBS_MULTICOLUMN))
                SendMessageW(control,LB_SETHORIZONTALEXTENT,200,0);
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

    /* Fresh demo-owned list: initial native tabs are known to be default.
     * Explicit attachment does not guess opaque state in arbitrary lists. */
    {HWND list;int item,stop=40;
        list=CreateWindowW(L"LISTBOX",L"",WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER|WS_VSCROLL|LBS_USETABSTOPS|LBS_NOINTEGRALHEIGHT,
            MulDiv(606,5,4),MulDiv(216,5,4),MulDiv(248,5,4),MulDiv(36,5,4),w,(HMENU)(INT_PTR)1510,GetModuleHandleW(NULL),NULL);REQUIRE(list);
        REQUIRE(win16_style_tabbed_list_install(list,NULL,0,NULL));
        REQUIRE(SendMessageW(list,LB_SETTABSTOPS,1,(LPARAM)&stop));
        for(item=0;item<16;++item){WCHAR text[40];swprintf_s(text,40,L"Item %02d\tValue %02d",item+1,item+1);
            REQUIRE(SendMessageW(list,LB_ADDSTRING,0,(LPARAM)text)==item);}
        REQUIRE(SendMessageW(list,LB_SETCURSEL,1,0)==1);
        }
}

static void layout_focused_typography(HWND w)
{
    HWND child;RECT outer;REQUIRE(GetWindowRect(w,&outer));
    /* Authored coordinates use the original16px demo grid; match20px type,
     * not the superseded24px expansion. No application layout is changed. */
    REQUIRE(SetWindowPos(w,NULL,0,0,MulDiv(outer.right-outer.left,5,4),MulDiv(outer.bottom-outer.top,5,4),
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
        REQUIRE(SetWindowPos(child,NULL,MulDiv(r.left,5,4),MulDiv(r.top,5,4),
            MulDiv(r.right-r.left,5,4),MulDiv(r.bottom-r.top,5,4),SWP_NOZORDER|SWP_NOACTIVATE));
    }
}

static INT_PTR CALLBACK coverage_dialog(HWND w,UINT m,WPARAM a,LPARAM b)
{
    if(m==WM_INITDIALOG){
        HINSTANCE instance=GetModuleHandleW(NULL);
        REQUIRE(CreateWindowW(L"STATIC",L"Dialog body and text share the same white background.",
            WS_CHILD|WS_VISIBLE,16,16,520,50,w,(HMENU)1,instance,NULL));
        REQUIRE(CreateWindowW(WC_LINK,L"Read the <a id=\"sample\">sample link</a> using mouse or Enter.",
            WS_CHILD|WS_VISIBLE|WS_TABSTOP,16,70,520,55,w,(HMENU)2,instance,NULL));
        REQUIRE(CreateWindowW(L"STATIC",L"",WS_CHILD|WS_VISIBLE|SS_ETCHEDHORZ,
            16,130,520,2,w,(HMENU)3,instance,NULL));
        REQUIRE(CreateWindowW(L"EDIT",L"Multiline edit without scrollbars\r\nNative selection and input remain enabled.",
            WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER|ES_MULTILINE,16,145,520,65,w,(HMENU)4,instance,NULL));
        REQUIRE(CreateWindowW(L"STATIC",L"Link not yet activated",WS_CHILD|WS_VISIBLE,
            16,225,520,30,w,(HMENU)5,instance,NULL));
        REQUIRE(CreateWindowW(L"BUTTON",L"Close",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_DEFPUSHBUTTON,
            390,270,145,40,w,(HMENU)IDCANCEL,instance,NULL));
        return TRUE;
    }
    if(m==WM_NOTIFY){NMHDR *n=(NMHDR *)b;
        if(n->idFrom==2 && (n->code==NM_CLICK || n->code==NM_RETURN)){
            SetDlgItemTextW(w,5,n->code==NM_CLICK?L"Native link notification: mouse":L"Native link notification: Enter");
            return TRUE;
        }
    }
    if(m==WM_CLOSE || (m==WM_COMMAND && LOWORD(a)==IDCANCEL)){DestroyWindow(w);return TRUE;}
    return FALSE;
}
static HWND create_sample(int index,HWND owner,BOOL state)
{
    const sample *s=&samples[index];RECT before,after;POINT p={0,0};HWND w;
    if(s->special==DIALOGCONTROLS){
        struct {DLGTEMPLATE dialog;WORD menu,window_class,title;} resource={0};
        INITCOMMONCONTROLSEX cc={sizeof(cc),ICC_LINK_CLASS};
        ntw16s_creation_scope *scope;ntw16s_creation_result result;
        REQUIRE(InitCommonControlsEx(&cc));scope=Ntw16sCreationBegin(TRUE);REQUIRE(scope);
        resource.dialog.style=s->style|DS_MODALFRAME;resource.dialog.cx=320;resource.dialog.cy=200;
        w=CreateDialogIndirectParamW(GetModuleHandleW(NULL),&resource.dialog,owner,coverage_dialog,0);
        REQUIRE(Ntw16sCreationEnd(scope,&result) && !result.failure && !result.unsupported && w);
        SetWindowTextW(w,s->name);SetWindowPos(w,NULL,100,100,570,360,SWP_NOZORDER);
        ShowWindow(w,SW_SHOWNOACTIVATE);return w;
    }
    if(s->special==OWNED){
        struct {DLGTEMPLATE dialog;WORD menu,window_class,title;} resource={0};
        resource.dialog.style=s->style|DS_MODALFRAME;resource.dialog.dwExtendedStyle=s->ex;
        resource.dialog.cx=240;resource.dialog.cy=115;
        w=CreateDialogIndirectParamW(GetModuleHandleW(NULL),&resource.dialog,owner,dialog_proc,(LPARAM)s);
        REQUIRE(w);SetWindowTextW(w,s->name);

        SetWindowPos(w,NULL,80+(index%4)*40,80+(index%4)*35,480,230,SWP_NOZORDER|SWP_NOACTIVATE);
    }else w=CreateWindowExW(s->ex,L"StyleGallerySample",s->name,s->style,
        80+(index%4)*40,80+(index%4)*35,480,230,s->special==OWNED?owner:NULL,
        NULL,GetModuleHandleW(NULL),(void *)s);REQUIRE(w);
    GetClientRect(w,&before);ClientToScreen(w,&p);OffsetRect(&before,p.x,p.y);
    if(s->retro){REQUIRE(win16_style_install(w,NULL));
        p.x=p.y=0;GetClientRect(w,&after);ClientToScreen(w,&p);OffsetRect(&after,p.x,p.y);
        REQUIRE(EqualRect(&before,&after));}
    if(s->special==SHOWCASE){
        int width=MulDiv(930,5,4),height=MulDiv(630,5,4),x=40,y=40;
        {MONITORINFO monitor={sizeof(monitor)};
            if(GetMonitorInfoW(MonitorFromWindow(w,MONITOR_DEFAULTTONEAREST),&monitor)){
                width=min(width,monitor.rcWork.right-monitor.rcWork.left);
                height=min(height,monitor.rcWork.bottom-monitor.rcWork.top);
                x=monitor.rcWork.left;y=monitor.rcWork.top;}}
        REQUIRE(SetWindowPos(w,NULL,x,y,width,height,SWP_NOZORDER|SWP_NOACTIVATE));

        create_showcase(w);
        REQUIRE(SetPropW(w,L"ShowcaseReady",(HANDLE)1));showcase_extent(w);
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
    if(s->special==SCROLL){SCROLLINFO info={sizeof(info),SIF_RANGE|SIF_PAGE|SIF_POS,0,100,20,30,0};
        SetScrollInfo(w,SB_HORZ,&info,TRUE);SetScrollInfo(w,SB_VERT,&info,TRUE);

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

        }
        REQUIRE(win16_style_controls_end(scope,&counts));

    }
    if(s->special==SCROLLCONTROLS){int n;
        for(n=0;n<4;++n){SCROLLINFO info={sizeof(info),SIF_RANGE|SIF_PAGE|SIF_POS,0,100,20,30,0};BOOL vertical=(n&1)!=0;
            HWND scroll=CreateWindowW(L"SCROLLBAR",L"",WS_CHILD|WS_VISIBLE|WS_TABSTOP|(vertical?SBS_VERT:SBS_HORZ),
                vertical?400+(n/2)*36:16,vertical?16:16+n*36,vertical?24:360,vertical?142:24,w,
                (HMENU)(INT_PTR)(1000+n),GetModuleHandleW(NULL),NULL);
            REQUIRE(scroll);SetScrollInfo(scroll,SB_CTL,&info,TRUE);if(n>=2)EnableWindow(scroll,FALSE);
            REQUIRE(win16_style_control_install(scroll,NULL));


        }

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

        }

    }
    if(s->special==MULTILIST){
        HWND list=CreateWindowW(L"LISTBOX",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|WS_HSCROLL|WS_TABSTOP|
            LBS_MULTICOLUMN|LBS_NOTIFY,12,16,440,142,w,(HMENU)(INT_PTR)1500,GetModuleHandleW(NULL),NULL);
        RECT item_rects[30];int n,second=-1;LONG_PTR style;LRESULT top,selection;
        REQUIRE(list);SendMessageW(list,WM_SETFONT,(WPARAM)GetStockObject(SYSTEM_FONT),FALSE);
        SendMessageW(list,LB_SETCOLUMNWIDTH,140,0);
        for(n=0;n<30;++n){WCHAR label[80];swprintf_s(label,80,L"Item %02d: long label clipped to its native column",n+1);
            REQUIRE(SendMessageW(list,LB_ADDSTRING,0,(LPARAM)label)==n);}
        for(n=0;n<30;++n){
            REQUIRE(SendMessageW(list,LB_GETITEMRECT,n,(LPARAM)&item_rects[n])!=LB_ERR);
            if(second<0 && item_rects[n].left>0)second=n;}
        REQUIRE(second>0);SendMessageW(list,LB_SETCURSEL,second,0);

        style=GetWindowLongPtrW(list,GWL_STYLE);top=SendMessageW(list,LB_GETTOPINDEX,0,0);
        selection=SendMessageW(list,LB_GETCURSEL,0,0);REQUIRE(win16_style_control_install(list,NULL));

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

        }

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

            REQUIRE(win16_style_edit_install(edit,n!=5,NULL));GetClientRect(edit,&current);REQUIRE(EqualRect(&initial,&current));
            REQUIRE(SendMessageW(edit,EM_GETMARGINS,0,0)==margins);
            }

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

    }
    if(s->special==SHOWCASE || s->special==LISTS || s->special==COMBOS || s->special==MULTILIST)prepare_owned_lists(w);
    if((s->special>=CHECKS && s->special<=AUTOCONTROLS) || s->special==MULTILIST)layout_focused_typography(w);

    ShowWindow(w,state&&s->special==MAXIMIZED?SW_MAXIMIZE:state&&s->special==ICONIC?SW_MINIMIZE:SW_SHOWNOACTIVATE);
    UpdateWindow(w);

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
    if(m==WM_DESTROY){PostQuitMessage(0);return 0;}
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

static BOOL gallery_message(MSG *message)
{
    HWND root=GetAncestor(message->hwnd,GA_ROOT);WCHAR name[32];const sample *s;
    if(root==controller)return IsDialogMessageW(controller,message);
    if(root==opened[COUNT-1])return IsDialogMessageW(root,message);
    if(!GetClassNameW(root,name,32) || lstrcmpW(name,L"StyleGallerySample"))return FALSE;
    s=(const sample *)GetWindowLongPtrW(root,GWLP_USERDATA);if(!s)return FALSE;
    if(s->special==MDI)return TranslateMDISysAccel(GetDlgItem(root,2),message);
    if(s->special<CHECKS)return FALSE;
    if(IsDialogMessageW(root,message))return TRUE;
    if(message->message==WM_KEYDOWN && (message->wParam==VK_F2 || message->wParam==VK_F3 || message->wParam==VK_ESCAPE)){
        SendMessageW(root,message->message,message->wParam,message->lParam);return TRUE;}
    return FALSE;
}

void win16_style_gallery_run(void)
{
    MSG m;create_controller();
    ShowWindow(controller,SW_SHOW);UpdateWindow(controller);
    SetFocus(listbox);
    while(GetMessageW(&m,NULL,0,0)>0)if(!gallery_message(&m)){TranslateMessage(&m);DispatchMessageW(&m);}
}
