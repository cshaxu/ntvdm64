/* Real native Console storage on an unswitched observer desktop. The child
 * separates its attachment; no guest or alternate terminal emulator is used. */
#include "frontend-exe/native_console_capture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static HANDLE report;
#define CHECK(x) do { if (!(x)) { char line[192];DWORD failure_written; \
    sprintf_s(line,sizeof(line),"FAIL line=%u error=%lu: %s\n",(unsigned)__LINE__,GetLastError(),#x); \
    if(report) WriteFile(report,line,(DWORD)strlen(line),&failure_written,NULL);else fputs(line,stderr);ExitProcess(1); } } while(0)
#define OK(call) do { DWORD test_error=(call);SetLastError(test_error);CHECK(test_error==ERROR_SUCCESS); } while(0)

static void seed(HANDLE buffer,SHORT width,SHORT height,WCHAR base)
{
    CHAR_INFO cells[256];
    DWORD offset=0,total=(DWORD)width*height;
    SMALL_RECT window={0,0,19,1};
    COORD size={width,height},origin={0,0},tile;
    CHECK(SetConsoleWindowInfo(buffer,TRUE,&window));
    CHECK(SetConsoleScreenBufferSize(buffer,size));
    while(offset<total) {
        DWORD x=offset%width,y=offset/width,n=width-x,i;
        SMALL_RECT rect;
        if(n>256)n=256;
        for(i=0;i<n;++i){cells[i].Char.UnicodeChar=(WCHAR)(base+(offset+i)%26);cells[i].Attributes=(WORD)(1+(offset+i)%15);}
        tile.X=(SHORT)n;tile.Y=1;
        rect.Left=(SHORT)x;rect.Top=rect.Bottom=(SHORT)y;rect.Right=(SHORT)(x+n-1);
        CHECK(WriteConsoleOutputW(buffer,cells,tile,origin,&rect));
        CHECK((DWORD)rect.Left==x && (DWORD)rect.Right==x+n-1 && (DWORD)rect.Top==y && (DWORD)rect.Bottom==y);
        offset+=n;
    }
}

static void verify(SHORT width,SHORT height,WCHAR base)
{
    run16_native_capture capture;
    CHAR_INFO cells[4096];
    SMALL_RECT rect;
    DWORD offset=0,count,total=(DWORD)width*height,i;
    OK(run16_native_capture_begin(&capture));
    CHECK(capture.info.dwSize.X==width && capture.info.dwSize.Y==height);
    CHECK(capture.input_codepage==GetConsoleCP() && capture.output_codepage==GetConsoleOutputCP());
    CHECK(run16_native_capture_read(&capture,0,cells,0,&rect,&count)==ERROR_INVALID_PARAMETER && !count);
    while(offset<total) {
        OK(run16_native_capture_read(&capture,offset,cells,4096,&rect,&count));
        CHECK(count && count<=4096 && (DWORD)rect.Left==offset%width && (DWORD)rect.Top==offset/width);
        CHECK(count==(DWORD)(rect.Right-rect.Left+1)*(rect.Bottom-rect.Top+1));
        for(i=0;i<count;++i) {
            CHECK(cells[i].Char.UnicodeChar==(WCHAR)(base+(offset+i)%26));
            CHECK(cells[i].Attributes==(WORD)(1+(offset+i)%15));
        }
        offset+=count;
    }
    CHECK(run16_native_capture_read(&capture,total,cells,4096,&rect,&count)==ERROR_NO_MORE_ITEMS && !count);
    run16_native_capture_end(&capture);
    run16_native_capture_end(&capture);
}

static int child(void)
{
    HANDLE first,second;
    CONSOLE_CURSOR_INFO cursor={37,FALSE};
    COORD position={77,237},resize={81,300};
    SMALL_RECT window={10,220,49,244},region;
    run16_native_capture capture;
    CHAR_INFO cell;
    CONSOLE_FONT_INFOEX original_font={sizeof(original_font)},after_font={sizeof(after_font)};
    DWORD count,flags,members[16],i;
    count=GetConsoleProcessList(members,16);
    CHECK(count>=2 && count<=16);
    CHECK(FreeConsole() && AllocConsole());
    if(GetConsoleWindow()) ShowWindow(GetConsoleWindow(),SW_HIDE);
    count=GetConsoleProcessList(members,16);
    CHECK(count==1 && members[0]==GetCurrentProcessId());
    first=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
    second=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
    CHECK(first!=INVALID_HANDLE_VALUE && second!=INVALID_HANDLE_VALUE);
    CHECK(GetCurrentConsoleFontEx(second,FALSE,&original_font));
    CHECK(SetConsoleActiveScreenBuffer(first));
    seed(first,80,300,0x4e00);
    CHECK(SetConsoleCursorPosition(first,position) && SetConsoleCursorInfo(first,&cursor));
    CHECK(SetConsoleWindowInfo(first,TRUE,&window));
    CHECK(SetConsoleCP(65001) && SetConsoleOutputCP(65001));
    OK(run16_native_capture_begin(&capture));
    CHECK(!memcmp(&capture.info.srWindow,&window,sizeof(window)));
    CHECK(capture.info.dwCursorPosition.X==77 && capture.info.dwCursorPosition.Y==237);
    CHECK(capture.cursor.dwSize==37 && !capture.cursor.bVisible);
    CHECK(GetConsoleMode(first,&flags) && capture.output_mode==flags);
    {
        CONSOLE_SCREEN_BUFFER_INFOEX copied={sizeof(copied)};
        CONSOLE_CURSOR_INFO copied_cursor;
        CONSOLE_SCREEN_BUFFER_INFOEX invalid=capture.info;
        CHAR_INFO sample={{0x4e2d},0x1e};
        COORD read_origin={0,0};
        WCHAR readback;
        DWORD input_mode;
        capture.info.ColorTable[1]=RGB(17,34,51);
        OK(run16_native_screen_apply(second,&capture.info,&capture.cursor));
        CHECK(GetConsoleScreenBufferInfoEx(second,&copied));
        CHECK(copied.dwSize.X==80 && copied.dwSize.Y==300 &&
            !memcmp(&copied.srWindow,&window,sizeof(window)) &&
            copied.dwCursorPosition.X==77 && copied.dwCursorPosition.Y==237 &&
            copied.ColorTable[1]==RGB(17,34,51));
        CHECK(GetConsoleCursorInfo(second,&copied_cursor) &&
            copied_cursor.dwSize==37 && !copied_cursor.bVisible);
        OK(run16_native_cells_write(second,0,&sample,1));
        CHECK(ReadConsoleOutputCharacterW(second,&readback,1,read_origin,&count) && count==1 && readback==0x4e2d);
        invalid.srWindow.Left=-1;
        CHECK(run16_native_screen_apply(second,&invalid,&capture.cursor)==ERROR_INVALID_DATA);
        CHECK(run16_native_cells_write(second,80*300,&sample,1)==ERROR_INVALID_PARAMETER);
        CHECK(GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE),&input_mode));
        CHECK(SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE),input_mode|ENABLE_WINDOW_INPUT));
        CHECK(FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)));
        for(i=0;i<4;++i)OK(run16_native_screen_apply(second,&capture.info,&capture.cursor));
        CHECK(GetNumberOfConsoleInputEvents(GetStdHandle(STD_INPUT_HANDLE),&count) && !count);
        CHECK(SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE),input_mode));
    }
    run16_native_capture_end(&capture);
    verify(80,300,0x4e00); /* Includes scrollback and rows outside the viewport. */
    CHECK(!run16_native_capture_begin(&capture));
    CHECK(SetConsoleScreenBufferSize(first,resize));
    CHECK(run16_native_capture_read(&capture,0,&cell,1,&region,&count)==ERROR_RETRY && !count);
    run16_native_capture_end(&capture);
    CHECK(SetStdHandle(STD_OUTPUT_HANDLE,first));
    CHECK(SetConsoleActiveScreenBuffer(second));
    seed(second,5001,2,L'A'); /* Wider than one tile, not capped at DOS columns. */
    verify(5001,2,L'A'); /* Must capture active CONOUT$, not stale STDOUT. */
    /* nxvm's native Console policy: the viewport is not the backing store.
     * Navigate to the far edge without shrinking the font or losing cells. */
    window=(SMALL_RECT){4981,0,5000,1};
    CHECK(SetConsoleWindowInfo(second,TRUE,&window));
    OK(run16_native_capture_begin(&capture));
    CHECK(!memcmp(&capture.info.srWindow,&window,sizeof(window)));
    OK(run16_native_capture_read(&capture,10001,&cell,1,&region,&count));
    CHECK(count==1 && cell.Char.UnicodeChar==(WCHAR)(L'A'+10001%26));
    CHECK(GetCurrentConsoleFontEx(second,FALSE,&after_font));
    CHECK(original_font.dwFontSize.X==after_font.dwFontSize.X &&
        original_font.dwFontSize.Y==after_font.dwFontSize.Y &&
        original_font.FontFamily==after_font.FontFamily &&
        original_font.FontWeight==after_font.FontWeight &&
        !wcscmp(original_font.FaceName,after_font.FaceName));
    run16_native_capture_end(&capture);
    for(i=0;i<3;++i){CHECK(!run16_native_capture_begin(&capture));run16_native_capture_end(&capture);}
    CloseHandle(first);CloseHandle(second);
    {
        const char text[]="PASS native capture/presentation: Unicode, palette, cursor, exact scrolled viewport, no resize feedback, 300-row history, 5001 columns, bounded tiles, resize retry\n";
        CHECK(WriteFile(report,text,sizeof(text)-1,&count,NULL));
    }
    return 0;
}

int main(int argc,char **argv)
{
    HANDLE input,output;
    SECURITY_ATTRIBUTES security={sizeof(security),NULL,TRUE};
    STARTUPINFOEXW startup={0};
    PROCESS_INFORMATION process={0};
    SIZE_T bytes=0;
    WCHAR image[MAX_PATH],command[MAX_PATH+80];
    char text[4096];
    DWORD count,code;
    if(argc==3 && !strcmp(argv[1],"--child")) {
        report=(HANDLE)(ULONG_PTR)strtoul(argv[2],NULL,16);
        return child();
    }
    CHECK(CreatePipe(&input,&output,&security,4096));
    CHECK(SetHandleInformation(input,HANDLE_FLAG_INHERIT,0));
    startup.StartupInfo.cb=sizeof(startup);
    startup.StartupInfo.dwFlags=STARTF_USESHOWWINDOW;
    startup.StartupInfo.wShowWindow=SW_HIDE;
    InitializeProcThreadAttributeList(NULL,1,0,&bytes);
    startup.lpAttributeList=HeapAlloc(GetProcessHeap(),0,bytes);CHECK(startup.lpAttributeList);
    CHECK(InitializeProcThreadAttributeList(startup.lpAttributeList,1,0,&bytes));
    CHECK(UpdateProcThreadAttribute(startup.lpAttributeList,0,PROC_THREAD_ATTRIBUTE_HANDLE_LIST,&output,sizeof(output),NULL,NULL));
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));
    swprintf_s(command,MAX_PATH+80,L"\"%ls\" --child %lx",image,(unsigned long)(ULONG_PTR)output);
    CHECK(CreateProcessW(image,command,NULL,NULL,TRUE,EXTENDED_STARTUPINFO_PRESENT,NULL,NULL,&startup.StartupInfo,&process));
    CloseHandle(output);CloseHandle(process.hThread);
    DeleteProcThreadAttributeList(startup.lpAttributeList);HeapFree(GetProcessHeap(),0,startup.lpAttributeList);
    if(WaitForSingleObject(process.hProcess,15000)!=WAIT_OBJECT_0) {
        TerminateProcess(process.hProcess,ERROR_TIMEOUT);
        CHECK(WaitForSingleObject(process.hProcess,5000)==WAIT_OBJECT_0);
        CHECK(FALSE);
    }
    CHECK(GetExitCodeProcess(process.hProcess,&code));
    CHECK(ReadFile(input,text,sizeof(text)-1,&count,NULL));text[count]=0;fputs(text,stdout);
    CloseHandle(input);CloseHandle(process.hProcess);
    CHECK(code==0 && strstr(text,"PASS"));
    return 0;
}
