/* Real children inspect the Windows buffer through production ConPTY and
 * terminal code, including its documented optional cursor inheritance.
 * No helper, guest mutation, desktop activation or synthetic success output. */
#define _WIN32_WINNT 0x0A00
#include "ntkvm-exe/native_conpty.h"
#include "ntkvm-exe/native_terminal.h"
#include <stdio.h>
#include <string.h>
#include <wchar.h>

typedef struct launch_context {
    ntkvm_conpty *pty;
    run16_native_start start;
    PROCESS_INFORMATION process;
    DWORD error;
    BOOL retain;
} launch_context;
static DWORD WINAPI launch_child(void *opaque)
{
    launch_context *context=opaque;
    context->error=ntkvm_conpty_launch(context->pty,&context->start,&context->process);
    if(!context->error && !context->retain)context->error=ntkvm_conpty_release(context->pty);
    return 0;
}
static int child(BOOL inherit)
{
    HANDLE output=GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;WCHAR previous=0;DWORD count;
    COORD origin={0,0};
    if(!GetConsoleScreenBufferInfo(output,&info))return 71;
    if(info.dwCursorPosition.X!=(inherit ? 6 : 0) ||
       info.dwCursorPosition.Y!=(inherit ? 4 : 0))return 72;
    /* Inheriting a cursor does NOT copy the parent's characters into ConPTY. */
    if(!ReadConsoleOutputCharacterW(output,&previous,1,origin,&count) || count!=1 || previous!=L' ')return 73;
    if(!WriteConsoleW(output,L"AFTER",5,&count,NULL) || count!=5)return 74;
    return 37;
}
static BOOL text_at(const ntkvm_terminal_frame *frame,int row,int column,const char *text)
{
    unsigned i;
    for(i=0;text[i];++i)if(frame->cells[row*frame->columns+column+i].chars[0]!=(unsigned char)text[i])return FALSE;
    return TRUE;
}
/* An unsolicited CPR is input, not a remote SetConsoleCursorPosition API.
 * Read through an ordered sentinel so the assertion cannot race input parsing. */
static int reused_child(BOOL absolute)
{
    HANDLE input=GetStdHandle(STD_INPUT_HANDLE),output=GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;INPUT_RECORD record;DWORD count;
    if(!SetConsoleMode(input,0))return 81;
    for(;;) {
        if(!ReadConsoleInputW(input,&record,1,&count) || count!=1)return 82;
        if(record.EventType==KEY_EVENT && record.Event.KeyEvent.bKeyDown &&
           record.Event.KeyEvent.uChar.UnicodeChar==L'Z')break;
    }
    if(!GetConsoleScreenBufferInfo(output,&info))return 83;
    if(info.dwCursorPosition.X!=11 || info.dwCursorPosition.Y!=4)return 84;
    if(!WriteConsoleW(output,L"SECOND",6,&count,NULL) || count!=6)return 85;
    if(absolute) {
        COORD position={2,2};
        if(!SetConsoleCursorPosition(output,position) ||
           !WriteConsoleW(output,L"ABS",3,&count,NULL) || count!=3)return 86;
    }
    return 37;
}
static int reused_cursor(BOOL absolute)
{
    ntkvm_terminal *terminal=NULL;ntkvm_terminal_frame frame={0};launch_context context={0};
    PROCESS_INFORMATION second={0};HANDLE thread=NULL;
    WCHAR image[MAX_PATH],command[1024],directory[MAX_PATH];
    static const WCHAR environment[]={0,0};COORD size={80,25};
    static const char initial[]="BEFORE\x1b[5;7H",local_cursor[]="\x1b[11;7H";
    static const BYTE unsolicited[]="\x1b[11;7RZ";
    DWORD error=0,first_code=MAXDWORD,second_code=MAXDWORD,delivered=0;
    ULONGLONG deadline;int result=1;BOOL first_frame=FALSE,local_moved=FALSE;
    if(!GetModuleFileNameW(NULL,image,MAX_PATH) || !GetCurrentDirectoryW(MAX_PATH,directory))goto done;
    error=ntkvm_terminal_open(size.Y,size.X,&terminal);
    if(!error)error=ntkvm_terminal_feed(terminal,(const BYTE *)initial,sizeof(initial)-1);
    if(!error)error=ntkvm_conpty_open_events(size,ntkvm_terminal_feed,terminal,NULL,NULL,TRUE,&context.pty);
    if(error)goto done;
    swprintf_s(command,1024,L"\"%ls\" --child-1",image);
    context.start.application=image;context.start.command=command;context.start.directory=directory;
    context.start.environment=environment;context.start.console_mask=7;context.retain=TRUE;
    thread=CreateThread(NULL,0,launch_child,&context,0,NULL);if(!thread)goto done;
    deadline=GetTickCount64()+10000;
    while(GetTickCount64()<deadline) {
        BYTE *bytes=NULL;DWORD count=0;
        error=ntkvm_terminal_take_replies(terminal,&bytes,&count);
        if(!error && count)error=ntkvm_conpty_write(context.pty,bytes,count,&delivered);
        if(bytes)HeapFree(GetProcessHeap(),0,bytes);
        if(error)break;
        ntkvm_terminal_frame_free(&frame);error=ntkvm_terminal_capture(terminal,&frame);
        if(error)break;
        first_frame=text_at(&frame,4,6,"AFTER");
        if(WaitForSingleObject(thread,0)==WAIT_OBJECT_0 && (context.error ||
           (context.process.hProcess && WaitForSingleObject(context.process.hProcess,0)==WAIT_OBJECT_0 && first_frame)))break;
        Sleep(5);
    }
    if(error || !first_frame || WaitForSingleObject(thread,0)!=WAIT_OBJECT_0 || context.error ||
       !GetExitCodeProcess(context.process.hProcess,&first_code) || first_code!=37)goto done;
    /* Model a DOS-owned cursor change without pretending it updates ConPTY. */
    error=ntkvm_terminal_feed(terminal,(const BYTE *)local_cursor,sizeof(local_cursor)-1);
    ntkvm_terminal_frame_free(&frame);
    if(!error)error=ntkvm_terminal_capture(terminal,&frame);
    if(error)goto done;
    local_moved=frame.cursor.row==10 && frame.cursor.col==6;
    if(!local_moved)goto done;
    error=ntkvm_conpty_write(context.pty,unsolicited,sizeof(unsolicited)-1,&delivered);
    if(error || delivered!=sizeof(unsolicited)-1)goto done;
    swprintf_s(command,1024,L"\"%ls\" --reused-child%ls",image,absolute ? L"-absolute" : L"");
    error=ntkvm_conpty_launch(context.pty,&context.start,&second);
    if(error || WaitForSingleObject(second.hProcess,10000)!=WAIT_OBJECT_0 ||
       !GetExitCodeProcess(second.hProcess,&second_code) || second_code!=37)goto done;
    /* This fixture now finishes admission. Production retains its resource. */
    error=ntkvm_conpty_release(context.pty);
    if(error || WaitForSingleObject(ntkvm_conpty_ended(context.pty),5000)!=WAIT_OBJECT_0)goto done;
    ntkvm_terminal_frame_free(&frame);
    error=ntkvm_terminal_capture(terminal,&frame);
    if(error)goto done;
    {
        int row,column,hits=0;
        for(row=0;row<frame.rows;++row)for(column=0;column+6<=frame.columns;++column)
            if(text_at(&frame,row,column,"SECOND")) {
                ++hits;
                printf("REUSED-PRESENTATION absolute=%u second-at=%d,%d cursor=%d,%d\n",
                    absolute,column,row,frame.cursor.col,frame.cursor.row);
            }
        if(hits!=1)goto done;
        if(!text_at(&frame,10,6,"SECOND"))goto done;
        if(absolute && !text_at(&frame,2,2,"ABS"))goto done;
    }
    result=0;
done:
    if(second.hProcess && WaitForSingleObject(second.hProcess,0)==WAIT_TIMEOUT)TerminateProcess(second.hProcess,99);
    if(context.process.hProcess && WaitForSingleObject(context.process.hProcess,0)==WAIT_TIMEOUT)
        TerminateProcess(context.process.hProcess,99);
    if(thread && WaitForSingleObject(thread,5000)!=WAIT_OBJECT_0)ExitProcess(2);
    if(thread)CloseHandle(thread);
    ntkvm_conpty_close(context.pty);
    if(second.hProcess)CloseHandle(second.hProcess);
    if(second.hThread)CloseHandle(second.hThread);
    if(context.process.hProcess)CloseHandle(context.process.hProcess);
    if(context.process.hThread)CloseHandle(context.process.hThread);
    ntkvm_terminal_frame_free(&frame);ntkvm_terminal_close(terminal);
    printf("REUSED-CURSOR local-moved=%u first=%lu second=%lu error=%lu remote-still=11,4 %s\n",
        local_moved,first_code,second_code,error,result ? "FAIL" : "PASS");
    printf("REUSED-ADDRESSING absolute=%u %s\n",absolute,result ? "FAIL" : "PASS");
    return result;
}
static int probe(BOOL inherit)
{
    ntkvm_terminal *terminal=NULL;ntkvm_terminal_frame frame={0};launch_context context={0};
    HANDLE thread=NULL;WCHAR image[MAX_PATH],command[1024],directory[MAX_PATH];
    static const WCHAR environment[]={0,0};COORD size={80,25};
    static const char initial[]="BEFORE\x1b[5;7H";
    DWORD error=0,code=MAXDWORD;ULONGLONG deadline;int result=1;BOOL retained=FALSE,positioned=FALSE;
    if(!GetModuleFileNameW(NULL,image,MAX_PATH) || !GetCurrentDirectoryW(MAX_PATH,directory))goto done;
    error=ntkvm_terminal_open(size.Y,size.X,&terminal);
    if(!error)error=ntkvm_terminal_feed(terminal,(const BYTE *)initial,sizeof(initial)-1);
    if(!error)error=ntkvm_conpty_open_events(size,ntkvm_terminal_feed,terminal,NULL,NULL,inherit,&context.pty);
    if(error)goto done;
    swprintf_s(command,1024,L"\"%ls\" --child-%u",image,inherit!=FALSE);
    context.start.application=image;context.start.command=command;context.start.directory=directory;
    context.start.environment=environment;context.start.console_mask=7;
    /* Startup may wait for cursor replies, so service them outside launch and
     * outside the reader callback, exactly like the intended frontend owner. */
    thread=CreateThread(NULL,0,launch_child,&context,0,NULL);
    if(!thread)goto done;
    deadline=GetTickCount64()+10000;
    while(GetTickCount64()<deadline) {
        BYTE *bytes=NULL;DWORD count=0,delivered=0;
        error=ntkvm_terminal_take_replies(terminal,&bytes,&count);
        if(count) {
            DWORD i;printf("CURSOR-REPLY inherit=%u bytes=",inherit);
            for(i=0;i<count;++i)printf("%02x",bytes[i]);
            putchar('\n');
        }
        if(!error && count)error=ntkvm_conpty_write(context.pty,bytes,count,&delivered);
        if(bytes)HeapFree(GetProcessHeap(),0,bytes);
        if(error==ERROR_NO_MORE_ITEMS)error=0;
        if(error)break;
        if(WaitForSingleObject(thread,0)==WAIT_OBJECT_0 &&
           (context.error || WaitForSingleObject(ntkvm_conpty_ended(context.pty),0)==WAIT_OBJECT_0))break;
        Sleep(5);
    }
    if(error || WaitForSingleObject(thread,0)!=WAIT_OBJECT_0 || context.error ||
       !context.process.hProcess || WaitForSingleObject(context.process.hProcess,0)!=WAIT_OBJECT_0 ||
       !GetExitCodeProcess(context.process.hProcess,&code) || code!=37 ||
       WaitForSingleObject(ntkvm_conpty_ended(context.pty),0)!=WAIT_OBJECT_0)goto done;
    error=ntkvm_terminal_capture(terminal,&frame);
    if(error)goto done;
    retained=text_at(&frame,0,0,"BEFORE");
    positioned=text_at(&frame,inherit ? 4 : 0,inherit ? 6 : 0,"AFTER");
    if(retained!=inherit || !positioned)goto done;
    result=0;
done:
    if(context.process.hProcess && WaitForSingleObject(context.process.hProcess,0)==WAIT_TIMEOUT)
        TerminateProcess(context.process.hProcess,99); /* This test's exact child only. */
    if(thread && WaitForSingleObject(thread,5000)!=WAIT_OBJECT_0) {
        fprintf(stderr,"INHERIT-CURSOR startup stuck; refusing unsafe resource free\n");
        ExitProcess(2);
    }
    if(thread)CloseHandle(thread);
    ntkvm_conpty_close(context.pty);
    if(context.process.hProcess)CloseHandle(context.process.hProcess);
    if(context.process.hThread)CloseHandle(context.process.hThread);
    ntkvm_terminal_frame_free(&frame);ntkvm_terminal_close(terminal);
    printf("INHERIT-CURSOR flag=%lu retained=%u positioned=%u child=%lu error=%lu launch=%lu %s\n",
        inherit ? (DWORD)PSEUDOCONSOLE_INHERIT_CURSOR : 0,retained,positioned,code,error,context.error,result ? "FAIL" : "PASS");
    return result;
}
int wmain(int argc,WCHAR **argv)
{
    if(argc==2 && !wcscmp(argv[1],L"--child-0"))return child(FALSE);
    if(argc==2 && !wcscmp(argv[1],L"--child-1"))return child(TRUE);
    if(argc==2 && !wcscmp(argv[1],L"--reused-child"))return reused_child(FALSE);
    if(argc==2 && !wcscmp(argv[1],L"--reused-child-absolute"))return reused_child(TRUE);
    if(argc!=1)return 2;
    return probe(FALSE)|probe(TRUE)|reused_cursor(FALSE)|reused_cursor(TRUE);
}
