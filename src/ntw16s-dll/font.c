#include "font.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

static __declspec(thread) HFONT shared_font;
static __declspec(thread) HANDLE shared_resource;
static __declspec(thread) unsigned references;

static void word(BYTE *p,unsigned n,unsigned v)
{p[n]=(BYTE)v;p[n+1]=(BYTE)(v>>8);}
static void dword(BYTE *p,unsigned n,DWORD v)
{memcpy(p+n,&v,4);}

static BOOL create_font(void)
{
    /* FNT v2 is the public bitmap-resource form already exercised by the
     * standalone font probe. Only newly rasterized host glyphs go into it.
     * The original USER/font loader is not imported or reconstructed. */
    BYTE *data=(BYTE *)calloc(65536,1);char face[32];
    unsigned cursor=118+225*4,n,maxwidth=0,sum=0;
    DWORD count=0;HANDLE resource=NULL;HFONT base=NULL,font=NULL;
    HDC src=NULL,scaled=NULL;HBITMAP a=NULL,b=NULL;HGDIOBJ olda=NULL,oldb=NULL,oldfont=NULL;
    TEXTMETRICW tm;SIZE extent;RECT clear={0,0,128,64};BOOL ok=FALSE;
    if(!data)return FALSE;
    wsprintfA(face,"Win16 System20 %08lX",GetCurrentThreadId());
    src=CreateCompatibleDC(NULL);scaled=CreateCompatibleDC(NULL);
    a=CreateBitmap(128,64,1,1,NULL);b=CreateBitmap(128,20,1,1,NULL);
    base=CreateFontW(16,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,
        OUT_RASTER_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,DEFAULT_PITCH,L"System");
    if(!src || !scaled || !a || !b || !base)goto done;
    olda=SelectObject(src,a);oldb=SelectObject(scaled,b);oldfont=SelectObject(src,base);
    if(!olda || !oldb || !oldfont || !GetTextMetricsW(src,&tm) ||
        tm.tmHeight<1 || tm.tmHeight>64 || tm.tmCharSet!=ANSI_CHARSET)goto done;
    SetBkColor(src,RGB(255,255,255));SetTextColor(src,RGB(0,0,0));
    SetStretchBltMode(scaled,COLORONCOLOR);
    for(n=0;n<224;++n){char ch=(char)(n+32);WCHAR wc;unsigned x,y,w,bytes;
        if(MultiByteToWideChar(1252,0,&ch,1,&wc,1)!=1 ||
            !GetTextExtentPoint32W(src,&wc,1,&extent) || extent.cx<0 || extent.cx>128)goto done;
        w=(unsigned)MulDiv(extent.cx,20,tm.tmHeight);if(w>128)goto done;
        bytes=((w+7)/8)*20;if(cursor+bytes+sizeof(face)>65535)goto done;
        word(data,118+4*n,w);word(data,120+4*n,cursor);
        if(!FillRect(src,&clear,(HBRUSH)GetStockObject(WHITE_BRUSH)) || !TextOutW(src,0,0,&wc,1))goto done;
        if(w && !StretchBlt(scaled,0,0,(int)w,20,src,0,0,extent.cx,tm.tmHeight,SRCCOPY))goto done;
        for(x=0;x<w;++x)for(y=0;y<20;++y){COLORREF pixel=GetPixel(scaled,(int)x,(int)y);
            if(pixel==CLR_INVALID)goto done;
            if(pixel==RGB(0,0,0))data[cursor+(x/8)*20+y]|=(BYTE)(0x80>>(x%8));}
        cursor+=bytes;sum+=w;if(w>maxwidth)maxwidth=w;
    }
    word(data,118+224*4,0);word(data,120+224*4,cursor);
    word(data,0,0x200);word(data,68,15);word(data,70,96);word(data,72,96);
    word(data,74,(unsigned)MulDiv(tm.tmAscent,20,tm.tmHeight));
    word(data,76,(unsigned)MulDiv(tm.tmInternalLeading,20,tm.tmHeight));
    word(data,83,(unsigned)tm.tmWeight);word(data,88,20);data[90]=1;
    word(data,91,sum/224);word(data,93,maxwidth);data[95]=32;data[96]=255;data[97]='?'-32;
    /* Total byte-columns of the font bitmap, not the widest glyph. */
    word(data,99,(cursor-(118+225*4))/20);dword(data,105,cursor);dword(data,113,118+225*4);
    memcpy(data+cursor,face,strlen(face)+1);cursor+=(unsigned)strlen(face)+1;dword(data,2,cursor);
    resource=AddFontMemResourceEx(data,cursor,NULL,&count);if(!resource || count!=1)goto done;
    font=CreateFontA(20,0,0,0,tm.tmWeight,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_RASTER_PRECIS,
        CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,DEFAULT_PITCH,face);
    if(font){char actual[32];HGDIOBJ old=SelectObject(scaled,font);
        ok=old && GetTextMetricsW(scaled,&tm) && tm.tmHeight==20 &&
            GetTextFaceA(scaled,32,actual)>0 && !strcmp(actual,face);
        if(old)SelectObject(scaled,old);}
done:
    if(oldfont)SelectObject(src,oldfont);
    if(olda)SelectObject(src,olda);if(oldb)SelectObject(scaled,oldb);
    if(base)DeleteObject(base);if(a)DeleteObject(a);if(b)DeleteObject(b);
    if(src)DeleteDC(src);if(scaled)DeleteDC(scaled);free(data);
    if(ok){shared_font=font;shared_resource=resource;return TRUE;}
    if(font)DeleteObject(font);if(resource)RemoveFontMemResourceEx(resource);return FALSE;
}

HFONT win16_style_font_acquire(void)
{
    if(!references && !create_font())return NULL;
    if(references==UINT_MAX)return NULL;
    ++references;return shared_font;
}
void win16_style_font_release(HFONT font)
{
    if(!references || font!=shared_font)return;
    if(!--references){DeleteObject(shared_font);RemoveFontMemResourceEx(shared_resource);
        shared_font=NULL;shared_resource=NULL;}
}
