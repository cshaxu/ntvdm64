/* Native tab-state oracle and explicit known-state styled attachment.
 * No public getter or inference of pre-existing private tab state is used. */
#include <windows.h>
#include <stdio.h>
#include "ntw16s-dll/font.h"
#include "ntw16s-dll/controls.h"
#define CHECK(x) do{if(!(x)){fprintf(stderr,"FAIL line=%d error=%lu\n",__LINE__,GetLastError());return 1;}}while(0)
static COLORREF list_background=RGB(255,255,255),list_text;
static LRESULT CALLBACK owner(HWND w,UINT m,WPARAM a,LPARAM b)
{
    if(m==WM_CTLCOLORLISTBOX){SetBkColor((HDC)a,list_background);SetTextColor((HDC)a,list_text);
        SetDCBrushColor((HDC)a,list_background);return (LRESULT)GetStockObject(DC_BRUSH);}
    return DefWindowProcW(w,m,a,b);
}
/* Native tab layout can be reused by printing, but parent CTLCOLOR does not
 * control selected/disabled colors. Preserve this stopping-boundary witness
 * before considering native printing a replacement for retro list drawing. */
static int print_palette(HWND list,COLORREF background,COLORREF foreground)
{
    HDC dc=CreateCompatibleDC(NULL);HBITMAP bm,old;int x,y;unsigned ink=0,paper=0;
    int selection=(int)SendMessageW(list,LB_GETCURSEL,0,0),top=(int)SendMessageW(list,LB_GETTOPINDEX,0,0);
    HFONT font=(HFONT)SendMessageW(list,WM_GETFONT,0,0);
    CHECK(dc);bm=CreateBitmap(400,60,1,32,NULL);CHECK(bm);old=SelectObject(dc,bm);
    SendMessageW(list,WM_PRINTCLIENT,(WPARAM)dc,PRF_CLIENT);
    for(y=0;y<(int)SendMessageW(list,LB_GETITEMHEIGHT,0,0);++y)for(x=0;x<400;++x){COLORREF color=GetPixel(dc,x,y);
        CHECK(color==background || color==foreground);
        if(color==foreground)++ink;else ++paper;}
    CHECK(background==foreground || (ink>0 && paper>0));
    CHECK(selection==SendMessageW(list,LB_GETCURSEL,0,0) && top==SendMessageW(list,LB_GETTOPINDEX,0,0));
    CHECK(font==(HFONT)SendMessageW(list,WM_GETFONT,0,0));
    printf("NATIVE PRINT enabled=%d selected=%d background=%06lx text=%06lx ink=%u\n",
        IsWindowEnabled(list),selection,background,foreground,ink);
    SelectObject(dc,old);DeleteObject(bm);DeleteDC(dc);return 0;
}
static int tail(HWND w,int minimum)
{
    BITMAPINFO bi={0};HDC dc=CreateCompatibleDC(NULL);HBITMAP bm,old;DWORD *pixels;int x,y,first=999;
    RECT clear={0,0,400,60};
    bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bi.bmiHeader.biWidth=400;bi.bmiHeader.biHeight=-60;
    bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;
    bm=CreateDIBSection(dc,&bi,DIB_RGB_COLORS,(void **)&pixels,NULL,0);if(!bm)return -1;
    old=SelectObject(dc,bm);FillRect(dc,&clear,(HBRUSH)GetStockObject(WHITE_BRUSH));
    SendMessageW(w,WM_PRINTCLIENT,(WPARAM)dc,PRF_CLIENT);GdiFlush();
    for(x=minimum;x<400;++x)for(y=0;y<28;++y)if((pixels[y*400+x]&0xffffff)==0){if(x<first)first=x;}
    SelectObject(dc,old);DeleteObject(bm);DeleteDC(dc);return first;
}
static int transition_cases(HWND root,HFONT font16,HFONT font20)
{
    unsigned multi;for(multi=0;multi<2;++multi){
        HWND list;int item,stop=48,pixel=96,before,attached,removed,second_native=0;
        list=CreateWindowW(L"LISTBOX",L"",WS_CHILD|WS_VISIBLE|LBS_NOINTEGRALHEIGHT|LBS_USETABSTOPS|
            (multi?LBS_MULTICOLUMN:0),0,0,400,60,root,NULL,GetModuleHandleW(NULL),NULL);CHECK(list);
        SendMessageW(list,WM_SETFONT,(WPARAM)font16,FALSE);
        CHECK(SendMessageW(list,LB_SETTABSTOPS,1,(LPARAM)&stop));
        if(multi)SendMessageW(list,LB_SETCOLUMNWIDTH,160,0);
        for(item=0;item<8;++item)CHECK(SendMessageW(list,LB_ADDSTRING,0,(LPARAM)L"X\tX")==item);
        before=tail(list,40);CHECK(before==pixel);
        /* Obtain a native same-font oracle without touching the native tabs,
         * then restore old font before the actual attachment transaction. */
        SendMessageW(list,WM_SETFONT,(WPARAM)font20,FALSE);
        CHECK(tail(list,40)==before);
        if(multi){second_native=tail(list,200);CHECK(second_native==256);}
        SendMessageW(list,WM_SETFONT,(WPARAM)font16,FALSE);
        CHECK(win16_style_tabbed_list_install(list,NULL,1,&pixel));
        CHECK((HFONT)SendMessageW(list,WM_GETFONT,0,0)==font20);
        attached=tail(list,40);CHECK(attached==before);
        if(multi)CHECK(tail(list,200)==second_native);
        CHECK(win16_style_control_remove(list));
        CHECK((HFONT)SendMessageW(list,WM_GETFONT,0,0)==font16);
        removed=tail(list,40);CHECK(removed==before);
        CHECK(win16_style_tabbed_list_install(list,NULL,1,&pixel));
        CHECK(SendMessageW(list,LB_SETTABSTOPS,1,(LPARAM)&stop));
        CHECK(tail(list,40)==120); /* New request uses the installed20px font. */
        CHECK(win16_style_control_remove(list));
        CHECK((HFONT)SendMessageW(list,WM_GETFONT,0,0)==font16 && tail(list,40)==120);
        printf("FONT TRANSITION multicolumn=%u native=%d attached=%d restored=%d new-set/restored=120 second-column=%d\n",
            multi,before,attached,removed,second_native);
        CHECK(DestroyWindow(list));
    }
    return 0;
}
int main(void)
{
    WNDCLASSW cls={0};WCHAR name[80];HDESK desktop;HWND root,list;HFONT font16,font20;int stop=48,a,b,c,d,initial,multiple,repeat;
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    swprintf_s(name,80,L"StyleTabs-%lu",GetCurrentProcessId());desktop=CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL);
    CHECK(desktop && SetThreadDesktop(desktop));
    cls.lpfnWndProc=owner;cls.lpszClassName=L"StyleTabs";cls.hInstance=GetModuleHandleW(NULL);
    CHECK(RegisterClassW(&cls));root=CreateWindowW(cls.lpszClassName,L"Tabs",WS_OVERLAPPEDWINDOW,0,0,500,200,NULL,NULL,cls.hInstance,NULL);CHECK(root);
    list=CreateWindowW(L"LISTBOX",L"",WS_CHILD|WS_VISIBLE|LBS_USETABSTOPS|LBS_NOINTEGRALHEIGHT,
        0,0,400,60,root,NULL,cls.hInstance,NULL);CHECK(list);
    font16=CreateFontW(16,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_RASTER_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,DEFAULT_PITCH,L"System");
    font20=win16_style_font_acquire();CHECK(font16 && font20);
    SendMessageW(list,WM_SETFONT,(WPARAM)font16,FALSE);CHECK(SendMessageW(list,LB_ADDSTRING,0,(LPARAM)L"X\tX")>=0);
    initial=tail(list,40);
    CHECK(SendMessageW(list,LB_SETTABSTOPS,1,(LPARAM)&stop));a=tail(list,40);
    SendMessageW(list,WM_SETFONT,(WPARAM)font20,FALSE);b=tail(list,40);
    CHECK(SendMessageW(list,LB_SETTABSTOPS,1,(LPARAM)&stop));c=tail(list,40);
    CHECK(SendMessageW(list,LB_SETTABSTOPS,0,0));d=tail(list,40);
    printf("default16=%d explicit48 old16=%d font20-kept=%d font20-reset=%d default20=%d\n",initial,a,b,c,d);
    CHECK(initial<999 && a<999 && b<999 && c<999 && d<999 && abs(a-b)<=2 && c>b && d>initial);
    {int stops[]={48,96};
        CHECK(SendMessageW(list,LB_DELETESTRING,0,0)==0);
        CHECK(SendMessageW(list,LB_ADDSTRING,0,(LPARAM)L"X\tX\tX")==0);
        CHECK(SendMessageW(list,LB_SETTABSTOPS,2,(LPARAM)stops));multiple=tail(list,c+40);
        CHECK(SendMessageW(list,LB_SETTABSTOPS,1,(LPARAM)&stop));repeat=tail(list,c+40);
        printf("multiple48/96 third=%d repeated48 third=%d\n",multiple,repeat);
        CHECK(multiple<999 && multiple==repeat && multiple>c);}
    list_background=RGB(234,210,186);list_text=RGB(12,34,56);
    CHECK(!print_palette(list,list_background,list_text));
    CHECK(SendMessageW(list,LB_SETCURSEL,0,0)==0);
    CHECK(!print_palette(list,GetSysColor(COLOR_HIGHLIGHT),GetSysColor(COLOR_HIGHLIGHTTEXT)));
    EnableWindow(list,FALSE);
    CHECK(!print_palette(list,GetSysColor(COLOR_HIGHLIGHT),GetSysColor(COLOR_GRAYTEXT)));
    SendMessageW(list,LB_SETCURSEL,(WPARAM)-1,0);
    CHECK(!print_palette(list,list_background,GetSysColor(COLOR_GRAYTEXT)));
    EnableWindow(list,TRUE);list_background=RGB(255,255,255);list_text=0;
    CHECK(tail(list,c+40)==repeat); /* Painting/state probes never reset tabs. */
    {win16_style_palette palette;int pixels=c,next=40,positions[2]={40,120};
        CHECK(!win16_style_control_install(list,NULL));
        CHECK(!win16_style_tabbed_list_install(list,NULL,1,NULL));
        win16_style_default_palette(&palette);palette.light=RGB(234,210,186);palette.edge=RGB(12,34,56);
        palette.active=RGB(150,180,210);palette.active_text=RGB(60,40,20);palette.shadow=RGB(90,80,70);
        CHECK(win16_style_tabbed_list_install(list,&palette,1,&pixels));pixels=7;
        CHECK(!print_palette(list,palette.light,palette.edge));
        CHECK(SendMessageW(list,LB_SETCURSEL,0,0)==0);
        CHECK(!print_palette(list,palette.active,palette.active_text));
        EnableWindow(list,FALSE);CHECK(!print_palette(list,palette.active,palette.shadow));
        SendMessageW(list,LB_SETCURSEL,(WPARAM)-1,0);CHECK(!print_palette(list,palette.light,palette.shadow));
        EnableWindow(list,TRUE);CHECK(win16_style_control_remove(list));
        CHECK(tail(list,c+40)==repeat); /* No native tab mutation at attach/detach. */
        pixels=c;CHECK(win16_style_tabbed_list_install(list,NULL,1,&pixels));
        CHECK(tail(list,c+40)==repeat);
        CHECK(SendMessageW(list,LB_SETTABSTOPS,1,(LPARAM)&next));CHECK(tail(list,160)==200);
        CHECK(SendMessageW(list,LB_SETTABSTOPS,2,(LPARAM)positions));CHECK(tail(list,160)==300);
        CHECK(!SendMessageW(list,LB_SETTABSTOPS,1,0));CHECK(tail(list,160)==300);
        CHECK(SendMessageW(list,LB_SETTABSTOPS,0,0));CHECK(tail(list,160)==160);
        CHECK(win16_style_control_remove(list));CHECK(tail(list,160)==160);
        printf("STYLED TABS copied initial state, four palette states, updates, rejection and removal pass\n");}
    CHECK(DestroyWindow(list));
    {unsigned column;for(column=0;column<2;++column){int native_x,styled_x;
        list=CreateWindowW(L"LISTBOX",L"",WS_CHILD|WS_VISIBLE|LBS_NOINTEGRALHEIGHT|(column?LBS_MULTICOLUMN:0),
            0,0,400,60,root,NULL,cls.hInstance,NULL);CHECK(list);
        SendMessageW(list,WM_SETFONT,(WPARAM)font20,FALSE);
        CHECK(SendMessageW(list,LB_ADDSTRING,0,(LPARAM)L"X")==0);native_x=tail(list,0);
        CHECK(win16_style_control_install(list,NULL));styled_x=tail(list,0);
        printf("TEXT ORIGIN multicolumn=%u native=%d styled=%d\n",column,native_x,styled_x);
        CHECK(native_x==(column?0:2) && native_x==styled_x && DestroyWindow(list));}}
    CHECK(!transition_cases(root,font16,font20));
    CHECK(DestroyWindow(root));DeleteObject(font16);win16_style_font_release(font20);return 0;
}
