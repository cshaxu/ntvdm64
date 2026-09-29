/* Real native Console storage on an unswitched observer desktop. The child
 * separates its attachment; no guest or alternate terminal emulator is used. */
#include "ntcon-exe/console_state.h"
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

static int child(void)
{
    HANDLE output;
    CONSOLE_SCREEN_BUFFER_INFOEX info={sizeof(info)},actual={sizeof(actual)},invalid;
    CONSOLE_CURSOR_INFO cursor={37,FALSE},actual_cursor;
    CONSOLE_FONT_INFOEX font={sizeof(font)},after={sizeof(after)};
    CHAR_INFO cells[80],readback[80];
    COORD origin={0,0},size={80,1};
    SMALL_RECT row={0,0,79,0};
    DWORD i,count;
    CHECK(FreeConsole() && AllocConsole());
    if(GetConsoleWindow())ShowWindow(GetConsoleWindow(),SW_HIDE);
    output=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
    CHECK(output!=INVALID_HANDLE_VALUE);
    seed(output,80,300,L'A');
    CHECK(GetCurrentConsoleFontEx(output,FALSE,&font));
    CHECK(GetConsoleScreenBufferInfoEx(output,&info));
    info.srWindow=(SMALL_RECT){10,220,29,221};
    info.dwCursorPosition=(COORD){77,237};
    info.ColorTable[1]=RGB(17,34,51);
    OK(ntcon_screen_apply(output,&info,&cursor));
    CHECK(GetConsoleScreenBufferInfoEx(output,&actual));
    CHECK(actual.dwSize.X==80 && actual.dwSize.Y==300 &&
        !memcmp(&actual.srWindow,&info.srWindow,sizeof(info.srWindow)) &&
        actual.dwCursorPosition.X==77 && actual.dwCursorPosition.Y==237 &&
        actual.ColorTable[1]==RGB(17,34,51));
    CHECK(GetConsoleCursorInfo(output,&actual_cursor) &&
        actual_cursor.dwSize==37 && !actual_cursor.bVisible);
    invalid=info;invalid.srWindow.Left=-1;
    CHECK(ntcon_screen_apply(output,&invalid,&cursor)==ERROR_INVALID_DATA);
    for(i=0;i<80;++i) {cells[i].Char.UnicodeChar=(WCHAR)(0x4e00+i);cells[i].Attributes=(WORD)(1+i%15);}
    OK(ntcon_cells_write(output,0,cells,80));
    CHECK(ReadConsoleOutputW(output,readback,size,origin,&row));
    CHECK(!memcmp(cells,readback,sizeof(cells)));
    CHECK(ntcon_cells_write(output,24000,cells,1)==ERROR_INVALID_PARAMETER);
    CHECK(ntcon_cells_write(output,23999,cells,2)==ERROR_INVALID_PARAMETER);
    CHECK(ntcon_cells_write(output,79,cells,2)==ERROR_INVALID_PARAMETER);
    CHECK(ntcon_cells_write(output,0,NULL,1)==ERROR_INVALID_PARAMETER);
    {
        DWORD mode;
        CHECK(GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE),&mode));
        CHECK(SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE),mode|ENABLE_WINDOW_INPUT));
        CHECK(FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)));
        for(i=0;i<4;++i)OK(ntcon_screen_apply(output,&info,&cursor));
        CHECK(GetNumberOfConsoleInputEvents(GetStdHandle(STD_INPUT_HANDLE),&count) && !count);
        CHECK(SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE),mode));
    }
    seed(output,5001,2,L'A');
    CHECK(GetConsoleScreenBufferInfoEx(output,&info));
    info.srWindow=(SMALL_RECT){4981,0,5000,1};
    info.dwCursorPosition=(COORD){4990,1};
    OK(ntcon_screen_apply(output,&info,&cursor));
    OK(ntcon_cells_write(output,10001,cells,1));
    {
        WCHAR value;
        CHECK(ReadConsoleOutputCharacterW(output,&value,1,(COORD){5000,1},&count) &&
            count==1 && value==0x4e00);
    }
    CHECK(GetCurrentConsoleFontEx(output,FALSE,&after));
    CHECK(font.dwFontSize.X==after.dwFontSize.X && font.dwFontSize.Y==after.dwFontSize.Y &&
        font.FontFamily==after.FontFamily && font.FontWeight==after.FontWeight &&
        !wcscmp(font.FaceName,after.FaceName));
    CloseHandle(output);
    {
        const char text[]="PASS native Console presentation: scrollback, Unicode, palette, cursor, 5001 columns, invalid spans, no font scaling\n";
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
