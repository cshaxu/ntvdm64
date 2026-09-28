/* Direct SGR -> Windows Console probe, bypassing the product mouse encoder. */
#include "ntkvm-exe/native_conpty.h"
#include <stdio.h>
#include <string.h>
#include <wchar.h>

typedef struct probe_output {
    HANDLE ready;
    char bytes[8192];
    DWORD used;
} probe_output;

static DWORD drain(void *context,const BYTE *bytes,DWORD count)
{
    probe_output *output=context;
    if(count>=sizeof(output->bytes)-output->used)return ERROR_BUFFER_OVERFLOW;
    memcpy(output->bytes+output->used,bytes,count);output->used+=count;
    output->bytes[output->used]=0;
    if(strstr(output->bytes,"WHEEL-READY"))SetEvent(output->ready);
    return 0;
}

static int child(void)
{
    HANDLE input=GetStdHandle(STD_INPUT_HANDLE),output=GetStdHandle(STD_OUTPUT_HANDLE);
    INPUT_RECORD record;DWORD count;int vertical=0,horizontal=0;unsigned v=0,h=0;
    if(!SetConsoleMode(input,ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT))return 71;
    WriteConsoleA(output,"WHEEL-READY",11,&count,NULL);
    for(;;) {
        if(!ReadConsoleInputW(input,&record,1,&count) || count!=1)return 72;
        if(record.EventType==MOUSE_EVENT) {
            MOUSE_EVENT_RECORD *mouse=&record.Event.MouseEvent;
            if(mouse->dwEventFlags&MOUSE_WHEELED) {
                vertical+=(SHORT)HIWORD(mouse->dwButtonState);++v;
            }
            if(mouse->dwEventFlags&MOUSE_HWHEELED) {
                horizontal+=(SHORT)HIWORD(mouse->dwButtonState);++h;
            }
        }
        if(record.EventType==KEY_EVENT && record.Event.KeyEvent.bKeyDown &&
           record.Event.KeyEvent.uChar.UnicodeChar=='Z')break;
    }
    {
        char report[160];int length=sprintf_s(report,sizeof(report),
            "\r\nRAW-WHEEL vertical=%d horizontal=%d events=%u,%u\r\n",vertical,horizontal,v,h);
        WriteConsoleA(output,report,(DWORD)length,&count,NULL);
    }
    return 37; /* Probe completion, NOT a wheel parity verdict. */
}

int wmain(int argc,WCHAR **argv)
{
    ntkvm_conpty *pty=NULL;probe_output output={0};PROCESS_INFORMATION target={0};
    run16_native_start start={0};WCHAR self[MAX_PATH],command[1024],directory[MAX_PATH];
    static const WCHAR environment[]={0,0};COORD size={80,25};DWORD error=0,code=0,delivered=0;
    static const char input[]="\033[<64;7;4M\033[<64;7;4M\033[<65;7;4M"
        "\033[<66;7;4M\033[<66;7;4M\033[<67;7;4M\033[90;44;90;1;0;1_";
    int result=1;
    if(argc==2 && !wcscmp(argv[1],L"--child"))return child();
    if(!GetModuleFileNameW(NULL,self,MAX_PATH) || !GetCurrentDirectoryW(MAX_PATH,directory))return 2;
    output.ready=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!output.ready)goto done;
    error=ntkvm_conpty_open(size,drain,&output,&pty);if(error)goto done;
    swprintf_s(command,1024,L"\"%ls\" --child",self);
    start.application=self;start.command=command;start.directory=directory;
    start.environment=environment;start.console_mask=7;
    error=ntkvm_conpty_launch(pty,&start,&target);
    if(error || WaitForSingleObject(output.ready,5000)!=WAIT_OBJECT_0)goto done;
    error=ntkvm_conpty_write(pty,input,sizeof(input)-1,&delivered);
    if(error || delivered!=sizeof(input)-1 || WaitForSingleObject(target.hProcess,5000)!=WAIT_OBJECT_0 ||
       !GetExitCodeProcess(target.hProcess,&code) || code!=37)goto done;
    error=ntkvm_conpty_release(pty);
    if(error || WaitForSingleObject(ntkvm_conpty_ended(pty),5000)!=WAIT_OBJECT_0)goto done;
    result=0;
done:
    if(target.hProcess && WaitForSingleObject(target.hProcess,0)==WAIT_TIMEOUT)TerminateProcess(target.hProcess,99);
    if(target.hThread)CloseHandle(target.hThread);
    if(target.hProcess)CloseHandle(target.hProcess);
    ntkvm_conpty_close(pty);
    printf("%s\nRAW-WHEEL-PROBE completion=%d error=%lu exit=%lu bytes=%lu (not parity)\n",
        output.bytes,result,error,code,delivered);
    if(output.ready)CloseHandle(output.ready);
    return result;
}
