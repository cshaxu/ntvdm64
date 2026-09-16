/* Native regression for the NT4 row-preserving resize binding. */
#include <windows.h>
#include <stdio.h>
#include "conapi.h"
#undef SetConsoleScreenBufferSize
#undef SetConsoleWindowInfo
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL line=%d error=%lu\n",__LINE__,GetLastError()); return 1; } } while (0)
int main(void)
{
    HANDLE output, readonly;
    CONSOLE_SCREEN_BUFFER_INFO info;
    CHAR_INFO cells[80*25], readback[80*25];
    COORD size={80,25}, zero={0,0}, cursor={70,20};
    SMALL_RECT window={0,0,19,9}, rect={0,0,79,24};
    int x,y;
    FreeConsole(); CHECK(AllocConsole()); ShowWindow(GetConsoleWindow(),SW_HIDE);
    output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    CHECK(output!=INVALID_HANDLE_VALUE);
    CHECK(SetConsoleWindowInfo(output,TRUE,&window));
    CHECK(SetConsoleScreenBufferSize(output,size));
    for(y=0;y<25;++y)for(x=0;x<80;++x) {
        cells[y*80+x].Char.UnicodeChar=(WCHAR)(L'A'+y);
        cells[y*80+x].Attributes=(WORD)(x==79?0x1e:7);
    }
    CHECK(WriteConsoleOutputW(output,cells,size,zero,&rect));
    CHECK(SetConsoleCursorPosition(output,cursor));
    CHECK(MvdmSetConsoleWindowInfo(output,TRUE,&window));
    size.X=40; size.Y=12;
    CHECK(MvdmSetConsoleScreenBufferSize(output,size));
    CHECK(GetConsoleScreenBufferInfo(output,&info));
    CHECK(info.dwCursorPosition.X==0 && info.dwCursorPosition.Y==11);
    rect.Left=rect.Top=0;rect.Right=39;rect.Bottom=11;
    CHECK(ReadConsoleOutputW(output,readback,size,zero,&rect));
    for(y=0;y<12;++y)for(x=0;x<40;++x)
        CHECK(readback[y*40+x].Char.UnicodeChar==(WCHAR)(L'A'+y+9));
    size.X=80;size.Y=25;
    CHECK(MvdmSetConsoleScreenBufferSize(output,size));
    rect.Right=79;rect.Bottom=24;
    CHECK(ReadConsoleOutputW(output,readback,size,zero,&rect));
    for(y=0;y<25;++y)for(x=0;x<80;++x)
        CHECK(readback[y*80+x].Char.UnicodeChar==(WCHAR)(y<12&&x<40?L'A'+y+9:L' '));
    readonly=CreateFileW(L"CONOUT$",GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,OPEN_EXISTING,0,NULL);
    CHECK(readonly!=INVALID_HANDLE_VALUE);
    CHECK(!MvdmSetConsoleScreenBufferSize(readonly,size));
    CloseHandle(readonly);
    CHECK(!MvdmSetConsoleScreenBufferSize(INVALID_HANDLE_VALUE,size));
    size.X=0; CHECK(!MvdmSetConsoleScreenBufferSize(output,size));
    rect.Left=10;rect.Right=5;
    CHECK(!MvdmSetConsoleWindowInfo(output,TRUE,&rect));
    CloseHandle(output);FreeConsole();
    fprintf(stderr,"PASS row crop, grow, cursor clamp, invalid handle/size/window\n");
    return 0;
}
