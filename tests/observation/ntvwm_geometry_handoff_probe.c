/* Ordinary native run16 target. Exercise real DOS/native handoffs without
 * desktop input, mocks, guest changes or direct worker control. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE *log;
#define CHECK(x) do {if(!(x)){fprintf(log,"FAIL line=%u error=%lu\n",__LINE__,GetLastError());fclose(log);return 1;}} while(0)
static DWORD child(const WCHAR *launcher,const WCHAR *arguments)
{
    WCHAR command[2048];STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION process={0};
    DWORD result=ERROR_GEN_FAILURE;
    if(swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" %ls",launcher,arguments)<0 ||
        !CreateProcessW(launcher,command,NULL,NULL,TRUE,0,NULL,NULL,&startup,&process))return GetLastError();
    CloseHandle(process.hThread);
    if(WaitForSingleObject(process.hProcess,20000)==WAIT_OBJECT_0)
        GetExitCodeProcess(process.hProcess,&result);
    else result=ERROR_TIMEOUT; /* Observer owns timeout cleanup; do not kill a tree. */
    fprintf(log,"CHILD arguments=%ls result=%lu\n",arguments,result);fflush(log);
    CloseHandle(process.hProcess);return result;
}
int wmain(int argc,WCHAR **argv)
{
    HANDLE output=GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    SMALL_RECT tiny={0,0,0,0},view;
    COORD size;DWORD count,cycle,cycles=3;unsigned rows;WCHAR expected[80],actual[80];
    WCHAR grid[MAX_PATH],grid_command[MAX_PATH+32],*name;
    if((argc!=4 && argc!=5) || _wfopen_s(&log,argv[3],L"wx"))return 2;
    if(argc==5){cycles=wcstoul(argv[4],NULL,10);CHECK(cycles && cycles<=100);}
    rows=wcstoul(argv[2],NULL,10);CHECK(rows==22 || rows==25 || rows==28 || rows==43 || rows==50);
    CHECK(GetModuleFileNameW(NULL,grid,ARRAYSIZE(grid))<ARRAYSIZE(grid));
    name=wcsrchr(grid,L'\\');CHECK(name!=NULL);
    CHECK(!wcscpy_s(name+1,ARRAYSIZE(grid)-(name+1-grid),L"GRID.COM"));
    CHECK(swprintf_s(grid_command,ARRAYSIZE(grid_command),L"\"%ls\" %u",grid,rows)>0);
    CHECK(GetConsoleScreenBufferInfo(output,&info));
    CHECK(info.srWindow.Right-info.srWindow.Left+1==80 && info.srWindow.Bottom-info.srWindow.Top+1==25);
    /* Leave scrollback headroom: at a full buffer's bottom a native echo
     * legitimately discards its oldest row, independently of any handoff. */
    size=(COORD){80,1000};view=(SMALL_RECT){0,200,79,(SHORT)(199+rows)};
    CHECK(SetConsoleWindowInfo(output,TRUE,&tiny));
    CHECK(SetConsoleScreenBufferSize(output,size));
    CHECK(SetConsoleWindowInfo(output,TRUE,&view));
    CHECK(SetConsoleCursorPosition(output,(COORD){0,200}));
    for(unsigned i=0;i<80;++i)expected[i]=(WCHAR)(L'A'+rows%26);
    CHECK(WriteConsoleOutputCharacterW(output,expected,80,(COORD){0,0},&count) && count==80);
    for(cycle=0;cycle<cycles;++cycle) {
        CHECK(GetConsoleScreenBufferInfo(output,&info));
        CHECK(info.srWindow.Right-info.srWindow.Left+1==80 &&
            info.srWindow.Bottom-info.srWindow.Top+1==(SHORT)rows);
        fprintf(log,"BEFORE cycle=%lu view=%d,%d,%d,%d buffer=%dx%d\n",cycle,
            info.srWindow.Left,info.srWindow.Top,info.srWindow.Right,info.srWindow.Bottom,info.dwSize.X,info.dwSize.Y);
        fflush(log);
        CHECK(child(argv[1],grid_command)==0);
        CHECK(child(argv[1],L"command.com /c mem.exe")==0);
        CHECK(GetConsoleScreenBufferInfo(output,&info));
        CHECK(info.srWindow.Right-info.srWindow.Left+1==80 &&
            info.srWindow.Bottom-info.srWindow.Top+1==(SHORT)rows);
        CHECK(info.dwSize.Y>=1000);
        CHECK(ReadConsoleOutputCharacterW(output,actual,80,(COORD){0,0},&count) && count==80);
        CHECK(!memcmp(actual,expected,sizeof(expected)));
        CHECK(child(argv[1],L"cmd.exe /d /c echo NATIVE-CHAIN-CONTINUED")==0);
        CHECK(GetConsoleScreenBufferInfo(output,&info));
        CHECK(info.srWindow.Right-info.srWindow.Left+1==80 &&
            info.srWindow.Bottom-info.srWindow.Top+1==(SHORT)rows);
        fprintf(log,"AFTER cycle=%lu rows=%u history-preserved=yes native-child=yes\n",cycle,rows);
        fflush(log);
    }
    fprintf(log,"PASS rows=%u cycles=%lu real-DOS-MEM=yes real-native-child=yes history-preserved=yes\n",rows,cycles);
    fclose(log);return 0;
}
