/* Actual standalone NTMON and x86 NTSRV, in a test-owned hidden Console.
 * No renderer replacement, arbitrary process lookup or desktop activation. */
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#define REQUIRE(x) do{if(!(x)){fprintf(stderr,"FAIL line=%u error=%lu\n",(unsigned)__LINE__,GetLastError());result=1;goto done;}}while(0)
static BOOL start_sibling(const WCHAR *directory,const WCHAR *name,DWORD flags,PROCESS_INFORMATION *child,HANDLE input,HANDLE output)
{
    WCHAR path[MAX_PATH];
    STARTUPINFOW start={sizeof(start)};
    if(swprintf_s(path,MAX_PATH,L"%ls\\%ls",directory,name)<0)return FALSE;
    start.dwFlags=STARTF_USESHOWWINDOW;start.wShowWindow=SW_HIDE;
    if(input && output){start.dwFlags|=STARTF_USESTDHANDLES;start.hStdInput=input;start.hStdOutput=output;start.hStdError=output;}
    return CreateProcessW(path,NULL,NULL,NULL,input && output,flags,NULL,NULL,&start,child);
}
static BOOL contains(HANDLE output,const WCHAR *needle)
{
    WCHAR text[80*25+1];DWORD count;
    if(!ReadConsoleOutputCharacterW(output,text,80*25,(COORD){0,0},&count))return FALSE;
    text[count]=0;return wcsstr(text,needle)!=NULL;
}
static BOOL wait_text(HANDLE output,HANDLE monitor,const WCHAR *text)
{
    ULONGLONG deadline=GetTickCount64()+10000;
    do {
        if(contains(output,text))return TRUE;
        if(WaitForSingleObject(monitor,10)!=WAIT_TIMEOUT)return FALSE;
    }while(GetTickCount64()<deadline);
    return FALSE;
}
static BOOL stop_child(PROCESS_INFORMATION *child)
{
    if(!child->hProcess)return TRUE;
    if(WaitForSingleObject(child->hProcess,0)==WAIT_TIMEOUT && !TerminateProcess(child->hProcess,0))return FALSE;
    if(WaitForSingleObject(child->hProcess,5000)!=WAIT_OBJECT_0)return FALSE;
    CloseHandle(child->hThread);CloseHandle(child->hProcess);ZeroMemory(child,sizeof(*child));
    return TRUE;
}
int wmain(int argc,WCHAR **argv)
{
    PROCESS_INFORMATION monitor={0},server={0};
    HANDLE output=INVALID_HANDLE_VALUE,input=INVALID_HANDLE_VALUE;
    CONSOLE_FONT_INFOEX font={sizeof(font)};
    INPUT_RECORD records[2]={0};DWORD written,code;
    int result=0;
    WCHAR directory[MAX_PATH],*slash;
    SECURITY_ATTRIBUTES inherit={sizeof(inherit),NULL,TRUE};
    BOOL allocated=AllocConsole();
    REQUIRE(argc==1 || argc==2);
    if(argc==2)REQUIRE(!wcscpy_s(directory,MAX_PATH,argv[1]));
    else {
        REQUIRE(GetModuleFileNameW(NULL,directory,MAX_PATH));
        slash=wcsrchr(directory,L'\\');REQUIRE(slash);*slash=0;
    }
    output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,&inherit,OPEN_EXISTING,0,NULL);
    input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,&inherit,OPEN_EXISTING,0,NULL);
    REQUIRE(output!=INVALID_HANDLE_VALUE && input!=INVALID_HANDLE_VALUE);
    font.dwFontSize.X=4;font.dwFontSize.Y=6;wcscpy_s(font.FaceName,LF_FACESIZE,L"Terminal");
    REQUIRE(SetCurrentConsoleFontEx(output,FALSE,&font));
    REQUIRE(SetConsoleWindowInfo(output,TRUE,&(SMALL_RECT){0,0,0,0}));
    REQUIRE(SetConsoleCursorPosition(output,(COORD){0,0}));
    REQUIRE(SetConsoleScreenBufferSize(output,(COORD){80,25}));
    REQUIRE(FlushConsoleInputBuffer(input));
    REQUIRE(start_sibling(directory,L"ntmon.exe",0,&monitor,input,output));
    REQUIRE(wait_text(output,monitor.hProcess,L"BaseSrv not connected"));
    REQUIRE(contains(output,L"NTVDM Task Monitor") && contains(output,L"UP/DOWN=Select Task DEL=End Task ESC=EXIT"));
    REQUIRE(start_sibling(directory,L"ntsrv.exe",CREATE_NO_WINDOW,&server,NULL,NULL));
    REQUIRE(wait_text(output,monitor.hProcess,L"Ready"));
    REQUIRE(contains(output,L"No active tasks."));
    REQUIRE(stop_child(&server));
    REQUIRE(wait_text(output,monitor.hProcess,L"BaseSrv not connected"));
    REQUIRE(WaitForSingleObject(monitor.hProcess,0)==WAIT_TIMEOUT);
    REQUIRE(start_sibling(directory,L"ntsrv.exe",CREATE_NO_WINDOW,&server,NULL,NULL));
    REQUIRE(wait_text(output,monitor.hProcess,L"Ready"));
    records[0].EventType=KEY_EVENT;records[0].Event.KeyEvent.bKeyDown=TRUE;
    records[0].Event.KeyEvent.wRepeatCount=1;records[0].Event.KeyEvent.wVirtualKeyCode=VK_ESCAPE;
    records[0].Event.KeyEvent.wVirtualScanCode=1;records[0].Event.KeyEvent.uChar.UnicodeChar=27;
    records[1]=records[0];records[1].Event.KeyEvent.bKeyDown=FALSE;
    REQUIRE(WriteConsoleInputW(input,records,2,&written) && written==2);
    REQUIRE(WaitForSingleObject(monitor.hProcess,10000)==WAIT_OBJECT_0);
    REQUIRE(GetExitCodeProcess(monitor.hProcess,&code) && code==0);
    puts("PASS actual standalone native monitor: exact title/footer, disconnected survival, x86 broker connect/loss/restart, ESC exit0");
done:
    if(!stop_child(&monitor))result=1;
    if(!stop_child(&server))result=1;
    if(input!=INVALID_HANDLE_VALUE)CloseHandle(input);
    if(output!=INVALID_HANDLE_VALUE)CloseHandle(output);
    if(allocated)FreeConsole();
    return result;
}
