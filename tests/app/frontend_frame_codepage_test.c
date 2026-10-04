/* Compile the real importer; count host queries and inject only the negative
 * conversion result. No fixture renderer or production instrumentation. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
static unsigned queries,conversions;
static BOOL fail_conversion;
static UINT counted_output_cp(void){++queries;return GetConsoleOutputCP();}
static int checked_conversion(UINT cp,DWORD flags,LPCCH text,int bytes,
    LPWSTR output,int count)
{
    ++conversions;
    if(fail_conversion){SetLastError(ERROR_NO_UNICODE_TRANSLATION);return 0;}
    return MultiByteToWideChar(cp,flags,text,bytes,output,count);
}
#define GetConsoleOutputCP counted_output_cp
#define MultiByteToWideChar checked_conversion
#include "../../src/ntcon-exe/frontend_session.c"
#undef GetConsoleOutputCP
#undef MultiByteToWideChar
static unsigned checks,failures;
static FILE *report;
#define CHECK(x) do{++checks;if(!(x)){++failures;fprintf(report,"FAIL %u %s error=%lu\n",__LINE__,#x,GetLastError());}}while(0)
int wmain(int argc,WCHAR **argv)
{
    frontend_session frontend={0};frontend_video video={0};
    HANDLE incoming=NULL;BYTE *payload=NULL;console_text_style *style;
    UINT saved;DWORD handles_before=0,handles_after=0;BOOL allocated=FALSE;
    unsigned pass,index;COORD size={80,28},origin={0,0};
    CHAR_INFO cells[80*28];CONSOLE_SCREEN_BUFFER_INFO info;
    CONSOLE_CURSOR_INFO cursor;SMALL_RECT rect;
    if(argc!=2 || _wfopen_s(&report,argv[1],L"wx"))return 2;
    saved=GetConsoleOutputCP();
    if(!saved){allocated=AllocConsole();CHECK(allocated);saved=GetConsoleOutputCP();}
    frontend.logical_surface=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
    CHECK(frontend.logical_surface!=INVALID_HANDLE_VALUE);
    if(frontend.logical_surface==INVALID_HANDLE_VALUE)goto done;
    payload=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*style)+80*28*3);
    CHECK(payload!=NULL);if(!payload)goto done;
    style=(console_text_style *)payload;style->font_height=16;
    style->cursor_height=4;style->cursor_visible=1;
    style->cursor_column=7;style->cursor_row=27;
    video.pixels=payload;video.description.kind=CONSOLE_VIDEO_TEXT_FRAME;
    video.description.width=80;video.description.height=28;
    for(pass=0;pass<3;++pass){
        UINT cp=pass ? 850 : 437;unsigned step=pass ? 3 : 2;
        WCHAR expected=0;char glyph=-101; /* byte 0x9b, without narrowing */
        CHECK(SetConsoleOutputCP(cp));
        CHECK(MultiByteToWideChar(cp,0,&glyph,1,&expected,1)==1);
        video.description.stride=80*step;
        for(index=0;index<80*28;++index){
            BYTE *cell=payload+sizeof(*style)+index*step;
            cell[0]=0x9b;cell[1]=0x1e;if(step==3)cell[2]=CONSOLE_TEXT_UNDERLINE;
        }
        queries=conversions=0;
        CHECK(prepare_text_frame(&frontend,&video,&incoming)==0);
        CHECK(queries==1 && conversions==80*28 && incoming!=NULL);
        if(!incoming)break;
        rect=(SMALL_RECT){0,0,79,27};
        CHECK(ReadConsoleOutputW(incoming,cells,size,origin,&rect));
        for(index=0;index<80*28;++index){
            CHECK(cells[index].Char.UnicodeChar==expected);
            CHECK(cells[index].Attributes==(WORD)(0x1e|(step==3 ? COMMON_LVB_UNDERSCORE : 0)));
        }
        CHECK(GetConsoleScreenBufferInfo(incoming,&info));
        CHECK(info.dwSize.X==80 && info.dwSize.Y==28 &&
            info.dwCursorPosition.X==7 && info.dwCursorPosition.Y==27);
        CHECK(GetConsoleCursorInfo(incoming,&cursor));
        CHECK(cursor.bVisible && cursor.dwSize==25);
        CloseHandle(incoming);incoming=NULL;
    }
    /* Conversion failure must release its candidate, not replace the source. */
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&handles_before));
    fail_conversion=TRUE;queries=conversions=0;
    CHECK(prepare_text_frame(&frontend,&video,&incoming)==ERROR_NO_UNICODE_TRANSLATION);
    fail_conversion=FALSE;
    CHECK(!incoming && queries==1 && conversions==1);
    CHECK(GetConsoleScreenBufferInfo(frontend.logical_surface,&info));
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&handles_after));
    CHECK(handles_before==handles_after);
done:
    if(incoming)CloseHandle(incoming);
    if(payload)HeapFree(GetProcessHeap(),0,payload);
    if(frontend.logical_surface && frontend.logical_surface!=INVALID_HANDLE_VALUE)
        CloseHandle(frontend.logical_surface);
    if(saved)CHECK(SetConsoleOutputCP(saved));
    if(allocated)FreeConsole();
    fprintf(report,"FRAME-CODEPAGE checks=%u failures=%u production-import=yes repeated-frame=yes\n",checks,failures);
    fclose(report);return failures ? 1 : 0;
}
