/* Compile the real painter with a test-local SaveDC failure boundary. */
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include "win16-style/menu_paint.h"
#include "win16-style/menu_layout.h"
#include <limits.h>
static int save_calls, fail_save;
static int WINAPI test_save_dc(HDC dc)
{
    ++save_calls;
    return save_calls==fail_save?0:SaveDC(dc);
}
#define SaveDC test_save_dc
#include "win16-style/win16_style.c"
#undef SaveDC
#define CHECK(x) do { if(!(x)){fprintf(stderr,"DC FAIL line %d\n",__LINE__);return 1;} } while(0)
static UINT lifetime_message;
static int lifetime_action,lifetime_seen;
static BOOL lifetime_timeout;
static LRESULT CALLBACK lifetime_proc(HWND w,UINT message,WPARAM a,LPARAM b)
{
    if(message==lifetime_message && lifetime_action){
        int action=lifetime_action;lifetime_action=0;++lifetime_seen;
        if(action==1)win16_style_remove(w);else DestroyWindow(w);
        return 0;
    }
    if(message==WM_ENTERIDLE && a==MSGF_MENU){PostMessageW(w,WM_KEYDOWN,VK_LEFT,0);return 0;}
    if(message==WM_TIMER && a==99){lifetime_timeout=TRUE;EndMenu();return 0;}
    return DefWindowProcW(w,message,a,b);
}
int wmain(int argc,WCHAR **argv)
{
    WNDCLASSW wc={0};HWND w;HDC dc;HBITMAP bm,old;style_window s={0};
    BITMAPINFO bi={0};DWORD *pixels;RECT before,after;int i,mode;
    WCHAR name[64];HDESK desktop;
    swprintf_s(name,64,L"StyleDC-%lu",GetCurrentProcessId());
    desktop=CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL);
    CHECK(desktop && SetThreadDesktop(desktop));
    wc.lpfnWndProc=DefWindowProcW;wc.hInstance=GetModuleHandleW(NULL);wc.lpszClassName=L"StyleDCFailure";
    CHECK(RegisterClassW(&wc));
    {HMENU menu=CreatePopupMenu();win16_style_menu_item *items=NULL;MENUITEMINFOW info={sizeof(info)};LRESULT result;
        CHECK(menu && AppendMenuW(menu,MF_STRING,1,L"&Close\tAlt+F4"));
        CHECK(AppendMenuW(menu,MF_STRING|MF_DISABLED,2,L"&Disabled"));
        info.fMask=MIIM_DATA;info.dwItemData=1234;CHECK(SetMenuItemInfoW(menu,0,TRUE,&info));
        CHECK(win16_style_menu_prepare(menu,&items,0));
        info.fMask=MIIM_DATA|MIIM_FTYPE;CHECK(GetMenuItemInfoW(menu,0,TRUE,&info));
        CHECK((info.fType&MFT_OWNERDRAW) && info.dwItemData!=1234);
        CHECK(win16_style_menu_char(items,menu,L'c',&result) && result==MAKELRESULT(0,MNC_EXECUTE));
        CHECK(win16_style_menu_char(items,menu,L'd',&result) && result==MAKELRESULT(0,MNC_IGNORE));
        CHECK(CheckMenuItem(menu,1,MF_BYCOMMAND|MF_CHECKED)!=(UINT)-1);
        {HMENU other=CreatePopupMenu();MENUITEMINFOW edit={sizeof(edit)};
            CHECK(other && AppendMenuW(other,MF_STRING,3,L"&Other"));
            CHECK(win16_style_menu_prepare(other,&items,0));
            win16_style_menu_restore_popup(menu,&items);
            info.fMask=MIIM_DATA;CHECK(GetMenuItemInfoW(menu,0,TRUE,&info) && info.dwItemData==1234);
            CHECK(win16_style_menu_char(items,other,L'o',&result) && result==MAKELRESULT(0,MNC_EXECUTE));
            edit.fMask=MIIM_STRING;edit.dwTypeData=L"&Renamed";CHECK(SetMenuItemInfoW(menu,0,TRUE,&edit));
            CHECK(win16_style_menu_prepare(menu,&items,0));
            CHECK(win16_style_menu_char(items,menu,L'r',&result) && result==MAKELRESULT(0,MNC_EXECUTE));
            CHECK(win16_style_menu_char(items,menu,L'c',&result) && result==MAKELRESULT(0,MNC_IGNORE));
            win16_style_menu_restore_popup(other,&items);CHECK(DestroyMenu(other));}
        win16_style_menu_restore(&items);CHECK(!items);
        info.fMask=MIIM_DATA|MIIM_FTYPE|MIIM_STATE;CHECK(GetMenuItemInfoW(menu,0,TRUE,&info));
        CHECK(!(info.fType&MFT_OWNERDRAW) && info.dwItemData==1234 && (info.fState&MFS_CHECKED));
        CHECK(DestroyMenu(menu));
        printf("PASS native popup item adaptation and original-data restoration with state changes preserved\n");}
    {
        POINT origin={10,20},point;SIZE sizes[]={{30,20},{40,25},{20,20}};
        UINT flags[]={0,0,MFT_RIGHTJUSTIFY};RECT items[3];int height;
        CHECK(win16_style_menu_arrange(origin,120,sizes,flags,3,items,&height));
        CHECK(height==25 && items[0].left==10 && items[1].left==40 && items[2].left==110);
        CHECK(items[0].bottom==45 && items[2].right==130);
        flags[1]=MFT_RIGHTJUSTIFY;flags[2]=0;
        CHECK(win16_style_menu_arrange(origin,120,sizes,flags,3,items,&height));
        CHECK(items[1].left==70 && items[2].left==110);
        flags[1]=0;flags[2]=MFT_RIGHTJUSTIFY;
        CHECK(win16_style_menu_arrange(origin,120,sizes,flags,3,items,&height));
        point.x=110;point.y=20;CHECK(win16_style_menu_hit(items,3,point)==2);
        point.x=100;CHECK(win16_style_menu_hit(items,3,point)==-1);
        point.x=130;CHECK(win16_style_menu_hit(items,3,point)==-1);
        CHECK(win16_style_menu_arrange(origin,60,sizes,flags,3,items,&height));
        CHECK(height==45 && items[1].top==40 && items[2].top==40 && items[2].right==70);
        flags[1]=MFT_MENUBREAK;
        CHECK(win16_style_menu_arrange(origin,120,sizes,flags,3,items,&height));
        CHECK(height==45 && items[1].top==40 && items[2].left==110);
        CHECK(win16_style_menu_arrange(origin,10,sizes,flags,3,items,&height));
        CHECK(height==65 && items[0].right==20 && items[2].top==65);
        CHECK(win16_style_menu_arrange(origin,10,NULL,NULL,0,NULL,&height) && height==0);
        CHECK(!win16_style_menu_arrange(origin,0,sizes,flags,3,items,&height));
        origin.x=LONG_MAX;CHECK(!win16_style_menu_arrange(origin,120,sizes,flags,3,items,&height));
        origin.x=0;origin.y=LONG_MAX;
        CHECK(!win16_style_menu_arrange(origin,120,sizes,flags,3,items,&height));
        printf("PASS menu bar layout: wrap, break, right alignment, clipping, hit boundaries and overflow\n");
    }
    w=CreateWindowW(wc.lpszClassName,L"DC preservation",WS_OVERLAPPEDWINDOW,0,0,320,200,NULL,NULL,wc.hInstance,NULL);
    CHECK(w);CHECK(refresh_font(w,&s));win16_style_default_palette(&s.palette);
    dc=CreateCompatibleDC(NULL);CHECK(dc);
    bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bi.bmiHeader.biWidth=320;bi.bmiHeader.biHeight=-200;
    bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;
    bm=CreateDIBSection(dc,&bi,DIB_RGB_COLORS,(void **)&pixels,NULL,0);CHECK(bm);
    old=(HBITMAP)SelectObject(dc,bm);
    /* Direct shared-font title drawing removed the second SaveDC/stretch
     * scope. Cover failure and success at the one remaining real boundary. */
    for(mode=1;mode<=2;++mode){
        GdiFlush();for(i=0;i<320*200;++i)pixels[i]=0x00123456;
        SelectClipRgn(dc,NULL);IntersectClipRect(dc,2,2,318,198);
        SetStretchBltMode(dc,HALFTONE);SetTextColor(dc,RGB(7,8,9));
        CHECK(GetClipBox(dc,&before)!=ERROR);
        save_calls=0;fail_save=mode==1?1:0;
        paint(w,&s,dc);GdiFlush();
        CHECK(GetClipBox(dc,&after)!=ERROR && EqualRect(&before,&after));
        CHECK(GetStretchBltMode(dc)==HALFTONE && GetTextColor(dc)==RGB(7,8,9));
        CHECK(GetCurrentObject(dc,OBJ_BITMAP)==bm);
        if(mode==1){CHECK(save_calls==1);for(i=0;i<320*200;++i)CHECK(pixels[i]==0x00123456);}
        else CHECK(save_calls==1);
        printf("PASS SaveDC case=%d preserved borrowed state\n",mode);
    }
    {
        RECT row={0,0,320,40};HFONT font=(HFONT)GetStockObject(SYSTEM_FONT);
        const WCHAR *labels[]={L"&Restore",L"&Move",L"&Close\tAlt+F4",L"",L"&Disabled\tCtrl+D"};
        UINT states[]={ODS_DISABLED|ODS_SELECTED,ODS_SELECTED,0,0,ODS_DISABLED};
        SelectClipRgn(dc,NULL);GetClipBox(dc,&before);
        for(i=0;i<5;++i){
            CHECK(win16_style_menu_text(dc,&row,font,96,&s.palette,labels[i],states[i],
                i==3?WIN16_STYLE_MENU_SEPARATOR:WIN16_STYLE_MENU_ROW,i==1));
            CHECK(GetClipBox(dc,&after)!=ERROR && EqualRect(&before,&after));
            CHECK(GetTextColor(dc)==RGB(7,8,9));
            CHECK(GetCurrentObject(dc,OBJ_BITMAP)==bm);
            CHECK(GetPixel(dc,1,row.top+1)==((states[i]&ODS_SELECTED)?s.palette.active:s.palette.light));
            OffsetRect(&row,0,40);
        }
        CHECK(GetPixel(dc,1,140)==s.palette.edge);
        CHECK(!win16_style_menu_text(dc,&row,font,0,&s.palette,L"invalid",0,WIN16_STYLE_MENU_ROW,FALSE));
        {SIZE bar,popup;RECT bounds={0,0,60,25};
            CHECK(win16_style_menu_measure(dc,font,96,L"&File",WIN16_STYLE_MENU_BAR,&bar));
            CHECK(win16_style_menu_measure(dc,font,96,L"&File",WIN16_STYLE_MENU_ROW,&popup));
            CHECK(popup.cx-bar.cx==24 && popup.cy==bar.cy);
            CHECK(win16_style_menu_text(dc,&bounds,font,96,&s.palette,L"&File",ODS_SELECTED,WIN16_STYLE_MENU_BAR,FALSE));
            CHECK(GetPixel(dc,1,1)==s.palette.active);
            CHECK(GetClipBox(dc,&after)!=ERROR && EqualRect(&before,&after));}
        if(argc==2){
            BITMAPFILEHEADER fh={0};HANDLE file;DWORD bytes;
            GdiFlush();fh.bfType=0x4d42;fh.bfOffBits=sizeof(fh)+sizeof(bi.bmiHeader);
            fh.bfSize=fh.bfOffBits+320*200*4;
            file=CreateFileW(argv[1],GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
            CHECK(file!=INVALID_HANDLE_VALUE);
            CHECK(WriteFile(file,&fh,sizeof(fh),&bytes,NULL));
            CHECK(WriteFile(file,&bi.bmiHeader,sizeof(bi.bmiHeader),&bytes,NULL));
            CHECK(WriteFile(file,pixels,320*200*4,&bytes,NULL));CloseHandle(file);
        }
        printf("PASS menu text state/clip/separator cases (not interactive menu acceptance)\n");
    }
    {win16_style_menu_scope *outer=win16_style_menu_begin(NULL),*inner;
        CHECK(outer);inner=win16_style_menu_begin(NULL);CHECK(inner);
        CHECK(win16_style_menu_end(outer)); /* detached owner during inner loop */
        CHECK(win16_style_menu_end(inner));
        outer=win16_style_menu_begin(NULL);CHECK(outer);CHECK(win16_style_menu_end(outer));
        printf("PASS nested menu scope outer-owner teardown and restart\n");}
    {style_window *installed;HMENU outer=CreatePopupMenu(),inner=CreatePopupMenu();MENUITEMINFOW info={sizeof(info)};
        CHECK(outer && inner && AppendMenuW(outer,MF_STRING,1,L"&Outer") && AppendMenuW(inner,MF_STRING,2,L"&Inner"));
        CHECK(win16_style_install(w,NULL));installed=state(w);CHECK(installed);
        {HMENU bar=CreateMenu();win16_style_layout frame;RECT window;
            CHECK(bar && AppendMenuW(bar,MF_STRING,42,L"&Action") && SetMenu(w,bar));
            frame_changed(w);CHECK(layout(w,installed,&frame) && GetWindowRect(w,&window));
            installed->bar_keyboard=TRUE;installed->bar_selected=-1;
            SendMessageW(w,STYLE_NCUAHDRAWCAPTION,0,
                MAKELPARAM(window.left+frame.menu.left+2,window.top+frame.menu.top+2));
            CHECK(installed->bar_selected==-1);
            SendMessageW(w,STYLE_NCUAHDRAWFRAME,0,
                MAKELPARAM(window.left+frame.menu.left+2,window.top+frame.menu.top+2));
            CHECK(installed->bar_selected==-1);
            installed->bar_keyboard=FALSE;
            CHECK(SetMenu(w,NULL) && DestroyMenu(bar));frame_changed(w);
            printf("PASS caption drawing messages cannot change keyboard menu selection\n");}
        SendMessageW(w,WM_ENTERMENULOOP,TRUE,0);CHECK(installed->menus && installed->menu_depth==1);
        SendMessageW(w,WM_INITMENUPOPUP,(WPARAM)outer,0);
        SendMessageW(w,WM_ENTERMENULOOP,TRUE,0);CHECK(installed->menus->previous && installed->menu_depth==2);
        SendMessageW(w,WM_INITMENUPOPUP,(WPARAM)inner,0);
        info.fMask=MIIM_FTYPE;
        CHECK(GetMenuItemInfoW(outer,0,TRUE,&info) && (info.fType&MFT_OWNERDRAW));
        CHECK(GetMenuItemInfoW(inner,0,TRUE,&info) && (info.fType&MFT_OWNERDRAW));
        SendMessageW(w,WM_EXITMENULOOP,TRUE,0);CHECK(installed->menus && !installed->menus->previous);
        CHECK(GetMenuItemInfoW(outer,0,TRUE,&info) && (info.fType&MFT_OWNERDRAW));
        CHECK(GetMenuItemInfoW(inner,0,TRUE,&info) && !(info.fType&MFT_OWNERDRAW));
        SendMessageW(w,WM_EXITMENULOOP,TRUE,0);CHECK(!installed->menus && !installed->menu_depth);
        CHECK(GetMenuItemInfoW(outer,0,TRUE,&info) && !(info.fType&MFT_OWNERDRAW));
        CHECK(!installed->menu_items);CHECK(DestroyMenu(outer) && DestroyMenu(inner));
        SendMessageW(w,WM_ENTERMENULOOP,TRUE,0);CHECK(win16_style_remove(w));
        CHECK(!state(w));
        printf("PASS component menu-loop nesting and remove during loop\n");}
    {UINT messages[]={WM_INITMENU,WM_INITMENUPOPUP,WM_NEXTMENU};int action,k;
        wc.lpfnWndProc=lifetime_proc;wc.lpszClassName=L"StyleMenuLifetime";CHECK(RegisterClassW(&wc));
        for(k=0;k<3;++k)for(action=1;action<=2;++action){
            HWND window=CreateWindowW(wc.lpszClassName,L"Owned menu lifetime",WS_OVERLAPPEDWINDOW,
                0,0,320,200,NULL,NULL,wc.hInstance,NULL);
            HMENU menu=CreateMenu(),popup=CreatePopupMenu();MENUITEMINFOW info={sizeof(info)};
            CHECK(window && menu && popup && AppendMenuW(popup,MF_STRING,1,L"&Entry"));
            CHECK(AppendMenuW(menu,MF_POPUP,(UINT_PTR)popup,L"&File") && SetMenu(window,menu));
            ShowWindow(window,SW_SHOWNOACTIVATE);CHECK(win16_style_install(window,NULL));
            lifetime_message=messages[k];lifetime_action=action;lifetime_seen=0;lifetime_timeout=FALSE;
            CHECK(SetTimer(window,99,1000,NULL));
            SendMessageW(window,WM_SYSCOMMAND,SC_KEYMENU,L'f');
            CHECK(!lifetime_timeout && lifetime_seen==1 && !state(window));
            if(action==1){CHECK(IsWindow(window));KillTimer(window,99);
                info.fMask=MIIM_FTYPE;CHECK(GetMenuItemInfoW(popup,0,TRUE,&info) && !(info.fType&MFT_OWNERDRAW));
                CHECK(win16_style_install(window,NULL) && win16_style_remove(window));
                CHECK(DestroyWindow(window));
            }else CHECK(!IsWindow(window));
            printf("PASS menu callback lifetime message=%x action=%d\n",messages[k],action);
        }}
    SelectObject(dc,old);DeleteObject(bm);DeleteDC(dc);DestroyWindow(w);
    win16_style_font_release(s.title_font);
    return 0;
}
