/* Native bitmap-font realization, exact pixels and reference lifetime.
 * No visible window, global font installation or guest-font input. */
#include "ntw16s-dll/font.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"FONT FAIL line %d error=%lu\n",__LINE__,GetLastError());return 1;}}while(0)
int main(int argc,char **argv)
{
    HFONT font,second,base;HDC src,expected,actual;HBITMAP a,b,c;
    HGDIOBJ oa,ob,oc,of,of2;RECT clear={0,0,128,64};TEXTMETRICW tm,scaled;
    unsigned n,pixels=0;DWORD before,after;LOGFONTW lf;
    if(argc==2 && !strcmp(argv[1],"--dpi96"))SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
    else SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    src=CreateCompatibleDC(NULL);expected=CreateCompatibleDC(NULL);actual=CreateCompatibleDC(NULL);
    a=CreateBitmap(128,64,1,1,NULL);b=CreateBitmap(128,24,1,1,NULL);c=CreateBitmap(128,24,1,1,NULL);
    base=CreateFontW(16,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,
        OUT_RASTER_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,DEFAULT_PITCH,L"System");
    CHECK(src && expected && actual && a && b && c && base);
    oa=SelectObject(src,a);ob=SelectObject(expected,b);oc=SelectObject(actual,c);of=SelectObject(src,base);
    CHECK(GetTextMetricsW(src,&tm));before=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);
    font=win16_style_font_acquire();second=win16_style_font_acquire();CHECK(font && second==font);
    win16_style_font_release(second);CHECK(GetObjectW(font,sizeof(lf),&lf)==sizeof(lf));
    of2=SelectObject(actual,font);CHECK(GetTextMetricsW(actual,&scaled) && scaled.tmHeight==24);
    SetBkColor(src,RGB(255,255,255));SetTextColor(src,RGB(0,0,0));
    SetBkColor(actual,RGB(255,255,255));SetTextColor(actual,RGB(0,0,0));SetStretchBltMode(expected,COLORONCOLOR);
    for(n=32;n<=255;++n){char ch=(char)n;WCHAR wc;SIZE from,to;int x,y,width;
        CHECK(MultiByteToWideChar(1252,0,&ch,1,&wc,1)==1);
        CHECK(GetTextExtentPoint32W(src,&wc,1,&from) && GetTextExtentPoint32W(actual,&wc,1,&to));
        width=MulDiv(from.cx,24,tm.tmHeight);CHECK(to.cx==width && to.cy==24);
        FillRect(src,&clear,(HBRUSH)GetStockObject(WHITE_BRUSH));FillRect(actual,&clear,(HBRUSH)GetStockObject(WHITE_BRUSH));
        CHECK(TextOutW(src,0,0,&wc,1) && TextOutW(actual,0,0,&wc,1));
        if(width)CHECK(StretchBlt(expected,0,0,width,24,src,0,0,from.cx,tm.tmHeight,SRCCOPY));
        for(y=0;y<24;++y)for(x=0;x<width;++x){++pixels;CHECK(GetPixel(actual,x,y)==GetPixel(expected,x,y));}
    }
    {HWND edit=CreateWindowW(L"EDIT",L"Line one\r\nLine two",WS_POPUP|ES_MULTILINE,0,0,300,100,NULL,NULL,GetModuleHandleW(NULL),NULL);
        LRESULT first,secondline;CHECK(edit);SendMessageW(edit,WM_SETFONT,(WPARAM)font,FALSE);
        first=SendMessageW(edit,EM_POSFROMCHAR,0,0);secondline=SendMessageW(edit,EM_POSFROMCHAR,10,0);
        CHECK((short)HIWORD(secondline)-(short)HIWORD(first)==24);
        SendMessageW(edit,EM_SETSEL,2,6);CHECK(LOWORD(SendMessageW(edit,EM_GETSEL,0,0))==2);
        SendMessageW(edit,WM_SETFONT,(WPARAM)base,FALSE);DestroyWindow(edit);}
    SelectObject(actual,of2);win16_style_font_release(font);
    for(n=0;n<8;++n){font=win16_style_font_acquire();CHECK(font);win16_style_font_release(font);}
    after=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);CHECK(after<=before);
    printf("FONT24 PASS source=%ld target=24 glyphs=224 pixels=%u exact; native EDIT row/selection; GDI=%lu/%lu\n",tm.tmHeight,pixels,before,after);
    SelectObject(src,of);SelectObject(src,oa);SelectObject(expected,ob);SelectObject(actual,oc);
    DeleteObject(base);DeleteObject(a);DeleteObject(b);DeleteObject(c);DeleteDC(src);DeleteDC(expected);DeleteDC(actual);return 0;
}
