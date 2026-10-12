/* Read-only original-font composition probe. No global/file font installation. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "win16-style/system_font.h"

static int glyph_test(BYTE *original)
{
    static const char name[]="Win31 VGA System";
    DWORD length=5441+sizeof(name),face=5441,count=0;BYTE *copy=(BYTE *)malloc(length);
    HANDLE memory;HFONT font;HDC dc;HBITMAP bitmap;HGDIOBJ oldfont,oldbitmap;
    unsigned checked=0,different=0;int ch,x,y;TEXTMETRICA tm;
    if(!copy)return 6;
    memcpy(copy,original,5441);memcpy(copy+5441,name,sizeof(name));
    memcpy(copy+2,&length,4);memcpy(copy+105,&face,4);
    memory=AddFontMemResourceEx(copy,length,NULL,&count);free(copy);
    if(!memory || count!=1)return 7;
    font=CreateFontA(16,0,0,0,FW_BOLD,0,0,0,ANSI_CHARSET,OUT_RASTER_PRECIS,
        CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,DEFAULT_PITCH,name);
    dc=CreateCompatibleDC(NULL);{HDC screen=GetDC(NULL);bitmap=CreateCompatibleBitmap(screen,64,16);ReleaseDC(NULL,screen);}
    oldbitmap=SelectObject(dc,bitmap);oldfont=SelectObject(dc,font);
    GetTextMetricsA(dc,&tm);printf("FONT height=%ld ascent=%ld weight=%ld\n",tm.tmHeight,tm.tmAscent,tm.tmWeight);
    SetBkColor(dc,RGB(255,255,255));SetTextColor(dc,RGB(0,0,0));
    for(ch=32;ch<=126;++ch){
        unsigned at=118+4*(ch-32),width=original[at]|original[at+1]<<8,offset=original[at+2]|original[at+3]<<8;
        char c=(char)ch;SIZE extent;RECT r={0,0,64,16};
        FillRect(dc,&r,(HBRUSH)GetStockObject(WHITE_BRUSH));
        GetTextExtentPoint32A(dc,&c,1,&extent);
        if(extent.cx!=(LONG)width || extent.cy!=16)++different;
        TextOutA(dc,0,0,&c,1);GdiFlush();
        for(y=0;y<16;++y)for(x=0;x<(int)width;++x){
            BOOL ink=(original[offset+(x/8)*16+y]&(0x80>>(x%8)))!=0;
            ++checked;if(GetPixel(dc,x,y)!=(ink?RGB(0,0,0):RGB(255,255,255)))++different;
        }
    }
    printf("GLYPHS ASCII=95 pixels=%u mismatches=%u\n",checked,different);
    SelectObject(dc,oldfont);SelectObject(dc,oldbitmap);DeleteObject(font);DeleteObject(bitmap);DeleteDC(dc);
    RemoveFontMemResourceEx(memory);return different?8:0;
}

int wmain(int argc,wchar_t **argv)
{
    HANDLE file,font;DWORD size,got,count=0;BYTE *data;
    if(argc!=2)return 2;
    file=CreateFileW(argv[1],GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
    if(file==INVALID_HANDLE_VALUE)return 3;
    size=GetFileSize(file,NULL);if(size!=7280){CloseHandle(file);return 4;}
    data=(BYTE *)malloc(size);
    if(!data || !ReadFile(file,data,size,&got,NULL) || got!=size){free(data);CloseHandle(file);return 5;}
    CloseHandle(file);
    if(sizeof(style_system_font)!=5441 || memcmp(data+1344,style_system_font,5441)){free(data);return 9;}
    /* Exact source identity is pinned by the runner. RT_FONT #31 starts at
     * 1344, dfSize=5441; NE resource size includes alignment padding. */
    font=AddFontMemResourceEx(data,size,NULL,&count);
    printf("MEMORY whole-FON handle=%p count=%lu\n",font,count);
    if(font)RemoveFontMemResourceEx(font);
    count=0;font=AddFontMemResourceEx(data+1344,5441,NULL,&count);
    printf("MEMORY raw-FNT handle=%p count=%lu\n",font,count);
    if(font)RemoveFontMemResourceEx(font);
    {int result=glyph_test(data+1344);free(data);return result;}
}
