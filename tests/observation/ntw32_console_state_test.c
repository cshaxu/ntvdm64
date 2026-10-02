/* Real hidden ordinary Console fixture; no ConPTY, helper or frontend. */
#define _WIN32_WINNT 0x0A00
#include "ntw32-exe/console_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

static HANDLE input_signal;
static volatile LONG input_signal_kind;
static BOOL WINAPI input_control(DWORD event)
{
    if(event!=CTRL_C_EVENT && event!=CTRL_BREAK_EVENT)return FALSE;
    InterlockedExchange(&input_signal_kind,(LONG)event);
    SetEvent(input_signal);return TRUE;
}

static int input_contract(void)
{
    HANDLE input=INVALID_HANDLE_VALUE;DWORD saved=0,count=0,queued=0,error=0;
    BOOL have_mode=FALSE,handler=FALSE;INPUT_RECORD records[4]={0},readback[48]={0};
    WCHAR line[8]={0};
#define VERIFY_INPUT(expression) do {if(!(expression)){error=__LINE__;goto done;}} while(0)
    input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    VERIFY_INPUT(input!=INVALID_HANDLE_VALUE);
    VERIFY_INPUT(GetConsoleMode(input,&saved));have_mode=TRUE;
    VERIFY_INPUT(SetConsoleMode(input,ENABLE_MOUSE_INPUT) && FlushConsoleInputBuffer(input));
    records[0].EventType=KEY_EVENT;
    records[0].Event.KeyEvent.bKeyDown=TRUE;
    records[0].Event.KeyEvent.wRepeatCount=1;
    records[0].Event.KeyEvent.wVirtualKeyCode='A';
    records[0].Event.KeyEvent.uChar.UnicodeChar=L'a';
    records[1]=records[0];records[1].Event.KeyEvent.bKeyDown=FALSE;
    records[2].EventType=MOUSE_EVENT;
    records[2].Event.MouseEvent.dwMousePosition.X=7;
    records[2].Event.MouseEvent.dwMousePosition.Y=3;
    records[2].Event.MouseEvent.dwButtonState=FROM_LEFT_1ST_BUTTON_PRESSED;
    records[3]=records[2];records[3].Event.MouseEvent.dwButtonState=0;
    VERIFY_INPUT(!ntw32_input_write(input,records,4,&count) && count==4);
    VERIFY_INPUT(GetNumberOfConsoleInputEvents(input,&queued) && queued==4);
    VERIFY_INPUT(ReadConsoleInputW(input,readback,4,&count) && count==4);
    VERIFY_INPUT(readback[0].Event.KeyEvent.uChar.UnicodeChar==L'a' &&
        readback[0].Event.KeyEvent.bKeyDown && !readback[1].Event.KeyEvent.bKeyDown &&
        readback[2].Event.MouseEvent.dwMousePosition.X==7 &&
        readback[2].Event.MouseEvent.dwButtonState==FROM_LEFT_1ST_BUTTON_PRESSED &&
        !readback[3].Event.MouseEvent.dwButtonState);
    {
        CONSOLE_SCREEN_BUFFER_INFO screen;char expected[48];int length;unsigned index;
        VERIFY_INPUT(GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&screen));
        VERIFY_INPUT(SetConsoleMode(input,ENABLE_VIRTUAL_TERMINAL_INPUT|ENABLE_EXTENDED_FLAGS));
        records[2].Event.MouseEvent.dwButtonState=FROM_LEFT_1ST_BUTTON_PRESSED;
        records[2].Event.MouseEvent.dwEventFlags=0;
        length=sprintf_s(expected,sizeof(expected),"\x1b[<0;%u;%uM",
            (unsigned)(8-screen.srWindow.Left),(unsigned)(4-screen.srWindow.Top));
        VERIFY_INPUT(length>0 && FlushConsoleInputBuffer(input));
        /* The VT client sees characters, while classic clients above saw
         * unchanged native records. No target-specific executable check. */
        VERIFY_INPUT(!ntw32_input_write(input,records+2,1,&count) && count==1);
        VERIFY_INPUT(ReadConsoleInputW(input,readback,ARRAYSIZE(readback),&count) &&
            count==(DWORD)length);
        for(index=0;index<count;++index)VERIFY_INPUT(readback[index].EventType==KEY_EVENT &&
            readback[index].Event.KeyEvent.bKeyDown &&
            readback[index].Event.KeyEvent.uChar.UnicodeChar==(WCHAR)expected[index]);
        records[2].Event.MouseEvent.dwButtonState=0;
        VERIFY_INPUT(!ntw32_input_write(input,records+2,1,&count) && count==1);
        VERIFY_INPUT(ReadConsoleInputW(input,readback,ARRAYSIZE(readback),&count) &&
            count==(DWORD)length && readback[count-1].Event.KeyEvent.uChar.UnicodeChar==L'm');
    }
    /* The actual Console, not a frontend parser, performs cooked line editing. */
    VERIFY_INPUT(SetConsoleMode(input,ENABLE_LINE_INPUT|ENABLE_PROCESSED_INPUT));
    records[1]=records[0];records[1].Event.KeyEvent.wVirtualKeyCode=VK_RETURN;
    records[1].Event.KeyEvent.uChar.UnicodeChar=L'\r';
    VERIFY_INPUT(!ntw32_input_write(input,records,2,&count) && count==2);
    VERIFY_INPUT(ReadConsoleW(input,line,8,&count,NULL) && count==3 &&
        line[0]==L'a' && line[1]==L'\r' && line[2]==L'\n');
    input_signal=CreateEventW(NULL,TRUE,FALSE,NULL);VERIFY_INPUT(input_signal!=NULL);
    VERIFY_INPUT(SetConsoleCtrlHandler(input_control,TRUE));handler=TRUE;
    records[0].Event.KeyEvent.wVirtualKeyCode='C';
    records[0].Event.KeyEvent.uChar.UnicodeChar=3;
    records[0].Event.KeyEvent.dwControlKeyState=LEFT_CTRL_PRESSED;
    VERIFY_INPUT(!ntw32_input_write(input,records,1,&count) && count==1);
    VERIFY_INPUT(WaitForSingleObject(input_signal,3000)==WAIT_OBJECT_0 && input_signal_kind==CTRL_C_EVENT);
    VERIFY_INPUT(ResetEvent(input_signal));
    records[0].Event.KeyEvent.wVirtualKeyCode=VK_CANCEL;
    VERIFY_INPUT(!ntw32_input_write(input,records,1,&count) && count==1);
    VERIFY_INPUT(WaitForSingleObject(input_signal,3000)==WAIT_OBJECT_0 && input_signal_kind==CTRL_BREAK_EVENT);
    /* Raw Ctrl-C remains a key record, not a generated signal. */
    VERIFY_INPUT(SetConsoleMode(input,0) && FlushConsoleInputBuffer(input));
    records[0].Event.KeyEvent.wVirtualKeyCode='C';
    VERIFY_INPUT(!ntw32_input_write(input,records,1,&count) && count==1);
    VERIFY_INPUT(ReadConsoleInputW(input,readback,1,&count) && count==1 &&
        readback[0].Event.KeyEvent.uChar.UnicodeChar==3);
done:
    if(handler)SetConsoleCtrlHandler(input_control,FALSE);
    if(input_signal){CloseHandle(input_signal);input_signal=NULL;}
    if(have_mode)SetConsoleMode(input,saved);
    if(input!=INVALID_HANDLE_VALUE)CloseHandle(input);
    return (int)error;
#undef VERIFY_INPUT
}


static int attached_client(PCWSTR self)
{
    ntw32_capture capture={0};CONSOLE_SCREEN_BUFFER_INFOEX info={sizeof(info)};
    CONSOLE_CURSOR_INFO cursor={25,TRUE};CHAR_INFO cells[24],readback[24];
    SMALL_RECT region;DWORD count=0,error=0,index,code=0;
    HANDLE output=INVALID_HANDLE_VALUE,alternate=INVALID_HANDLE_VALUE,ready=NULL,finish=NULL;
    PROCESS_INFORMATION child={0};STARTUPINFOW startup={sizeof(startup)};
    SECURITY_ATTRIBUTES security={sizeof(security),NULL,TRUE};WCHAR command[1024];
#define CHECK(expression) do { if(!(expression)) {error=__LINE__;goto done;} } while(0)
    CHECK(!GetConsoleWindow() || !IsWindowVisible(GetConsoleWindow()));
    {int input_result=input_contract();if(input_result){error=20000+input_result;goto done;}}
    output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    CHECK(output!=INVALID_HANDLE_VALUE);
    CHECK(GetConsoleScreenBufferInfoEx(output,&info));
    info.dwCursorPosition.X=7;info.dwCursorPosition.Y=3;
    CHECK(ntw32_screen_apply(output,&info,&cursor)==0);
    for(index=0;index<24;++index) {cells[index].Char.UnicodeChar=(WCHAR)('A'+index);cells[index].Attributes=0x1e;}
    CHECK(ntw32_cells_write(output,(DWORD)info.dwSize.X*3,cells,24)==0);
    CHECK(ntw32_capture_begin(&capture)==0);
    CHECK(capture.info.dwCursorPosition.X==7 && capture.info.dwCursorPosition.Y==3);
    CHECK(ntw32_capture_read(&capture,(DWORD)info.dwSize.X*3,readback,24,&region,&count)==0);
    CHECK(count==24 && !memcmp(cells,readback,sizeof(cells)));
    /* Never silently accept a tile against geometry from an older capture. */
    {COORD enlarged=info.dwSize;++enlarged.Y;
        if(!SetConsoleScreenBufferSize(output,enlarged)) {
            error=0x80000000u|((DWORD)(USHORT)info.dwSize.Y<<16)|(GetLastError()&0xffffu);
            goto done;
        }
        CHECK(ntw32_capture_read(&capture,0,readback,24,&region,&count)==ERROR_RETRY);
        CHECK(count==0 && SetConsoleScreenBufferSize(output,info.dwSize));}
    ntw32_capture_end(&capture);
    info.dwCursorPosition.X=info.dwSize.X;
    CHECK(ntw32_screen_apply(output,&info,&cursor)==ERROR_INVALID_DATA);
    CHECK(GetConsoleScreenBufferInfoEx(output,&info) && info.dwCursorPosition.X==7);
    CHECK(ntw32_cells_write(output,MAXDWORD,cells,24)==ERROR_INVALID_PARAMETER);

    /* A native program reads the actual cursor and continues after a DOS-shaped
     * transfer, rather than a frontend-only parser change. */
    {DWORD written;CHECK(WriteConsoleW(output,L"AFTER",5,&written,NULL) && written==5);}
    CHECK(GetConsoleScreenBufferInfoEx(output,&info));
    CHECK(info.dwCursorPosition.X==12 && info.dwCursorPosition.Y==3);
    alternate=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
    CHECK(alternate!=INVALID_HANDLE_VALUE && SetConsoleActiveScreenBuffer(alternate));
    CHECK(ntw32_cells_write(alternate,0,cells,24)==0);
    CHECK(ntw32_capture_begin(&capture)==0);
    CHECK(ntw32_capture_read(&capture,0,readback,24,&region,&count)==0);
    CHECK(count==24 && !memcmp(cells,readback,sizeof(cells)));
    ntw32_capture_end(&capture);
    CHECK(SetConsoleActiveScreenBuffer(output));

    ready=CreateEventW(&security,TRUE,FALSE,NULL);finish=CreateEventW(&security,TRUE,FALSE,NULL);
    CHECK(ready && finish);
    swprintf_s(command,1024,L"\"%ls\" --peer %lx %lx",self,
        (unsigned long)(ULONG_PTR)ready,(unsigned long)(ULONG_PTR)finish);
    CHECK(CreateProcessW(self,command,NULL,NULL,TRUE,0,NULL,NULL,&startup,&child));
    CHECK(WaitForSingleObject(ready,5000)==WAIT_OBJECT_0);
    CHECK(SetEvent(finish) && WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
    CHECK(GetExitCodeProcess(child.hProcess,&code) && code==73);
    CloseHandle(child.hThread);CloseHandle(child.hProcess);ZeroMemory(&child,sizeof(child));
    ResetEvent(ready);ResetEvent(finish);
    swprintf_s(command,1024,L"\"%ls\" --spawn-peer %lx %lx",self,
        (unsigned long)(ULONG_PTR)ready,(unsigned long)(ULONG_PTR)finish);
    CHECK(CreateProcessW(self,command,NULL,NULL,TRUE,0,NULL,NULL,&startup,&child));
    CHECK(WaitForSingleObject(ready,5000)==WAIT_OBJECT_0);
    CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
    CHECK(GetExitCodeProcess(child.hProcess,&code) && code==73);
    /* Participant retention/order is verified by the Job event fixture,
     * not by polling this Console. */
    CHECK(SetEvent(finish));
    CloseHandle(child.hThread);CloseHandle(child.hProcess);ZeroMemory(&child,sizeof(child));
    ResetEvent(ready);ResetEvent(finish);
    swprintf_s(command,1024,L"\"%ls\" --peer %lx %lx",self,
        (unsigned long)(ULONG_PTR)ready,(unsigned long)(ULONG_PTR)finish);
    CHECK(CreateProcessW(self,command,NULL,NULL,TRUE,DETACHED_PROCESS,NULL,NULL,&startup,&child));
    CHECK(WaitForSingleObject(ready,5000)==WAIT_OBJECT_0);
    CHECK(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT);
    CHECK(SetEvent(finish) && WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
    CHECK(GetExitCodeProcess(child.hProcess,&code) && code==73);
done:
    ntw32_capture_end(&capture);
    if(output!=INVALID_HANDLE_VALUE)SetConsoleActiveScreenBuffer(output);
    if(finish)SetEvent(finish);
    if(child.hProcess) {
        if(WaitForSingleObject(child.hProcess,5000)==WAIT_TIMEOUT)TerminateProcess(child.hProcess,99);
        CloseHandle(child.hThread);CloseHandle(child.hProcess);
    }
    if(ready)CloseHandle(ready);if(finish)CloseHandle(finish);
    if(alternate!=INVALID_HANDLE_VALUE)CloseHandle(alternate);
    if(output!=INVALID_HANDLE_VALUE)CloseHandle(output);
    return (int)error;
#undef CHECK
}

int wmain(int argc,WCHAR **argv)
{
    WCHAR self[MAX_PATH],command[1024];
    PROCESS_INFORMATION process={0};STARTUPINFOW startup={sizeof(startup)};
    DWORD error=0,code=MAXDWORD;
    if(argc==4 && !wcscmp(argv[1],L"--peer")) {
        HANDLE ready=(HANDLE)(ULONG_PTR)wcstoul(argv[2],NULL,16);
        HANDLE finish=(HANDLE)(ULONG_PTR)wcstoul(argv[3],NULL,16);
        return SetEvent(ready) && WaitForSingleObject(finish,10000)==WAIT_OBJECT_0 ? 73 : 74;
    }
    if(!GetModuleFileNameW(NULL,self,MAX_PATH))return 2;
    if(argc==4 && !wcscmp(argv[1],L"--spawn-peer")) {
        PROCESS_INFORMATION peer={0};
        swprintf_s(command,1024,L"\"%ls\" --peer %ls %ls",self,argv[2],argv[3]);
        if(!CreateProcessW(self,command,NULL,NULL,TRUE,0,NULL,NULL,&startup,&peer))return 74;
        CloseHandle(peer.hThread);CloseHandle(peer.hProcess);return 73;
    }
    if(argc==2 && !wcscmp(argv[1],L"--attached"))return attached_client(self);
    swprintf_s(command,1024,L"\"%ls\" --attached",self);
    startup.dwFlags=STARTF_USESHOWWINDOW;
    startup.wShowWindow=SW_HIDE;
    /* The worker is born with its own nonvisible ordinary Console. The test
     * driver is not attached and owns no backend Console resource. */
    if(!CreateProcessW(self,command,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&startup,&process))
        {error=GetLastError();goto done;}
    if(WaitForSingleObject(process.hProcess,20000)!=WAIT_OBJECT_0)error=ERROR_TIMEOUT;
    else if(!GetExitCodeProcess(process.hProcess,&code))error=GetLastError();
done:
    if(process.hProcess && WaitForSingleObject(process.hProcess,0)==WAIT_TIMEOUT)
        TerminateProcess(process.hProcess,99);
    if(process.hThread)CloseHandle(process.hThread);if(process.hProcess)CloseHandle(process.hProcess);
    printf("NTW32-STATE hidden-console error=%lu client-line=%lu\n",error,code);
    if(error || code)return 1;
    puts("NTW32-STATE PASS real-cells cursor active-buffer detached-survivor stale-geometry raw-cooked-input mouse-pair ctrl-c-break; participant graph covered by ntw32-job-tracker-test");
    return 0;
}
