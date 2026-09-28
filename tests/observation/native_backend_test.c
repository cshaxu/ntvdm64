#include "ntkvm-exe/native_console_backend.h"
#include "ntkvm-exe/native_terminal.h"
#include <stdio.h>
#include <wchar.h>

static int child(unsigned round)
{
    HANDLE input=GetStdHandle(STD_INPUT_HANDLE),output=GetStdHandle(STD_OUTPUT_HANDLE);
    INPUT_RECORD record;DWORD count,mode;BOOL pressed=FALSE,released=FALSE,moved=FALSE;
    BOOL chord=FALSE,rightOnly=FALSE,chordReleased=FALSE;
    int vertical=0,horizontal=0;unsigned vertical_events=0,horizontal_events=0;
    unsigned up=0,down=0,left=0,right=0;
    WCHAR title[64],marker[64];
    if(!round) {if(!SetConsoleTitleW(L"NTKVM-PERSISTENT-CONPTY"))return 75;}
    else if(!GetConsoleTitleW(title,64) || wcscmp(title,L"NTKVM-PERSISTENT-CONPTY"))return 76;
    if(!GetConsoleMode(input,&mode) || !SetConsoleMode(input,ENABLE_WINDOW_INPUT|ENABLE_MOUSE_INPUT))return 71;
    swprintf_s(marker,64,L"BACKEND-READY-%u",round);
    if(!WriteConsoleW(output,marker,15,&count,NULL) || count!=15)return 72;
    for(;;) {
        if(!ReadConsoleInputW(input,&record,1,&count))return 73;
        if(record.EventType==MOUSE_EVENT) {
            MOUSE_EVENT_RECORD *mouse=&record.Event.MouseEvent;
            if(mouse->dwEventFlags&(MOUSE_WHEELED|MOUSE_HWHEELED)) {
                WCHAR report[128];
                int length=swprintf_s(report,128,L"\r\nWHEEL-EVENT flags=%lu delta=%d buttons=%lu\r\n",
                    mouse->dwEventFlags,(SHORT)HIWORD(mouse->dwButtonState),
                    mouse->dwButtonState&0x1f);
                WriteConsoleW(output,report,(DWORD)length,&count,NULL);
            }
            if(mouse->dwEventFlags==MOUSE_WHEELED) {
                vertical+=(SHORT)HIWORD(mouse->dwButtonState);++vertical_events;
                if((SHORT)HIWORD(mouse->dwButtonState)>0)++up;
                else if((SHORT)HIWORD(mouse->dwButtonState)<0)++down;
            }
            if(mouse->dwEventFlags==MOUSE_HWHEELED) {
                horizontal+=(SHORT)HIWORD(mouse->dwButtonState);++horizontal_events;
                if((SHORT)HIWORD(mouse->dwButtonState)>0)++right;
                else if((SHORT)HIWORD(mouse->dwButtonState)<0)++left;
            }
            if(mouse->dwMousePosition.X==4 && mouse->dwMousePosition.Y==3) {
                if(mouse->dwButtonState&FROM_LEFT_1ST_BUTTON_PRESSED)pressed=TRUE;
                else if(pressed)released=TRUE;
            }
            if(mouse->dwEventFlags==MOUSE_MOVED && mouse->dwButtonState==0 &&
                mouse->dwMousePosition.X==5 && mouse->dwMousePosition.Y==3)moved=TRUE;
            if(mouse->dwMousePosition.X==6 && mouse->dwMousePosition.Y==3) {
                DWORD buttons=mouse->dwButtonState&0x1f;
                if(buttons==(FROM_LEFT_1ST_BUTTON_PRESSED|RIGHTMOST_BUTTON_PRESSED))chord=TRUE;
                if(chord && buttons==RIGHTMOST_BUTTON_PRESSED)rightOnly=TRUE;
                if(rightOnly && !buttons)chordReleased=TRUE;
            }
        }
        if(record.EventType==KEY_EVENT && record.Event.KeyEvent.bKeyDown &&
            record.Event.KeyEvent.uChar.UnicodeChar=='Z') {
            WCHAR report[160];
            int length=swprintf_s(report,160,L"\r\nWHEEL vertical=%d horizontal=%d events=%u,%u\r\n",
                vertical,horizontal,vertical_events,horizontal_events);
            WriteConsoleW(output,report,(DWORD)length,&count,NULL);
            if(!pressed || !released || !moved || !chord || !rightOnly || !chordReleased)return 74;
            /* SGR preserves whole steps/direction, not Win32 delta magnitude:
             * Microsoft's input parser synthesizes +/-128, not WHEEL_DELTA.
             * Require every expected step; do not accept missing horizontal
             * records merely because this host drops them. */
            if(up!=2 || down!=1)return 77;
            if(left!=2 || right!=1)return 78;
            if(vertical_events!=3 || horizontal_events!=3)return 79;
            break;
        }
    }
    return 37;
}

static int scroll_child(void)
{
    HANDLE output=GetStdHandle(STD_OUTPUT_HANDLE);unsigned row;DWORD count;
    for(row=0;row<80;++row) {
        WCHAR line[32];int length=swprintf_s(line,32,L"SCROLL-%03u\r\n",row);
        if(length<=0 || !WriteConsoleW(output,line,(DWORD)length,&count,NULL) || count!=(DWORD)length)return 81;
    }
    return 37;
}

/* Real Windows Console output, not synthetic VT fed to the parser. Verify
 * bounded history preserves a contiguous suffix after repeated scrolling. */
static int scroll_backend(void)
{
    run16_native_backend *backend=NULL;run16_native_start start={0};HANDLE target=NULL;
    ntkvm_terminal_frame frame={0};WCHAR image[MAX_PATH],command[1024],directory[MAX_PATH];
    static const WCHAR environment[]={0,0};COORD size={80,12};
    DWORD error=0,code=MAXDWORD;ULONGLONG deadline;int result=1,row,first=-1,last=-1,lines=0;
    if(!GetModuleFileNameW(NULL,image,MAX_PATH) || !GetCurrentDirectoryW(MAX_PATH,directory))goto done;
    error=run16_native_backend_open(&backend);
    if(!error)error=run16_native_backend_configure(backend,size,50);
    if(error)goto done;
    swprintf_s(command,1024,L"\"%ls\" --scroll-child",image);
    start.application=image;start.command=command;start.directory=directory;
    start.environment=environment;start.console_mask=7;
    error=run16_native_backend_launch(backend,&start,&target);
    if(error || WaitForSingleObject(target,10000)!=WAIT_OBJECT_0 ||
       !GetExitCodeProcess(target,&code) || code!=37)goto done;
    deadline=GetTickCount64()+5000;
    do {
        ntkvm_terminal_frame_free(&frame);
        error=run16_native_backend_pump(backend);
        if(!error)error=run16_native_backend_capture(backend,&frame);
        if(error)goto done;
        first=last=-1;lines=0;
        for(row=0;row<frame.rows;++row) {
            const VTermScreenCell *cells=frame.cells+(SIZE_T)row*frame.columns;
            static const char prefix[]="SCROLL-";unsigned i;int number;
            for(i=0;i<7 && cells[i].chars[0]==(unsigned char)prefix[i];++i) {}
            if(i!=7)continue;
            for(i=7;i<10;++i)if(cells[i].chars[0]<'0' || cells[i].chars[0]>'9')goto done;
            number=(cells[7].chars[0]-'0')*100+(cells[8].chars[0]-'0')*10+cells[9].chars[0]-'0';
            if(first<0)first=number;
            else if(number!=last+1)goto done;
            last=number;++lines;
        }
        /* Process completion and the last text marker can precede delivery of
         * the final CR/LF. Wait for the complete expected frame, not just text. */
        if(first==19 && last==79 && lines==61 && frame.history_rows==50 &&
           frame.rows==62 && frame.cursor.row==61 && frame.cursor.col==0)break;
        Sleep(5);
    }while(GetTickCount64()<deadline);
    if(frame.columns!=80 || frame.viewport_rows!=12 || frame.history_rows!=50 ||
       frame.rows!=62 || first!=19 || last!=79 || lines!=61 ||
       frame.cursor.row!=61 || frame.cursor.col!=0)goto done;
    result=0;
done:
    printf("BACKEND-SCROLL %s first=%d last=%d lines=%d history=%d rows=%d cursor=%d,%d exit=%lu error=%lu\n",
        result ? "FAIL" : "PASS",first,last,lines,frame.history_rows,frame.rows,
        frame.cursor.col,frame.cursor.row,code,error);
    if(target && WaitForSingleObject(target,0)==WAIT_TIMEOUT)TerminateProcess(target,99);
    if(target) {WaitForSingleObject(target,5000);CloseHandle(target);}
    ntkvm_terminal_frame_free(&frame);run16_native_backend_close(backend);
    return result;
}

int wmain(int argc,WCHAR **argv)
{
    run16_native_backend *backend=NULL;run16_native_start start={0};
    HANDLE target=NULL,ended=NULL,stop=NULL;DWORD error=0,code=0,members=99,round;
    WCHAR image[MAX_PATH],command[1024],directory[MAX_PATH];
    static const WCHAR environment[]={0,0};COORD size={90,30};int result=1;
    if(argc==3 && !wcscmp(argv[1],L"--child"))return child(wcstoul(argv[2],NULL,10));
    if(argc==2 && !wcscmp(argv[1],L"--scroll-child"))return scroll_child();
    if(scroll_backend())return 1;
    if(!GetModuleFileNameW(NULL,image,MAX_PATH) || !GetCurrentDirectoryW(MAX_PATH,directory))goto done;
    stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!stop || run16_native_backend_open_cancel(stop,&backend))goto done;
    ended=run16_native_backend_process(backend);
    if(run16_native_backend_members(backend,&members) || members)goto done;
    if(run16_native_backend_configure(backend,size,100))goto done;
    start.application=image;start.command=command;start.directory=directory;
    start.environment=environment;start.console_mask=7;
    for(round=0;round<2;++round) {
        ULONGLONG deadline=GetTickCount64()+10000;BOOL ready=FALSE;
        WCHAR marker[64];
        INPUT_RECORD input[14]={0};
        swprintf_s(command,1024,L"\"%ls\" --child %lu",image,round);
        swprintf_s(marker,64,L"BACKEND-READY-%lu",round);
        error=run16_native_backend_launch(backend,&start,&target);
        if(error || ended!=run16_native_backend_process(backend))goto done;
        if(run16_native_backend_members(backend,&members) || !members)goto done;
        while(GetTickCount64()<deadline) {
            ntkvm_terminal_frame frame={0};unsigned i;
            error=run16_native_backend_pump(backend);
            if(!error)error=run16_native_backend_capture(backend,&frame);
            if(error)goto done;
            for(i=0;i+15<=(unsigned)(frame.rows*frame.columns);++i) {
                unsigned j;
                for(j=0;j<15 && frame.cells[i+j].chars[0]==marker[j];++j) {}
                if(j==15) {ready=TRUE;break;}
            }
            ntkvm_terminal_frame_free(&frame);
            if(ready)break;
            Sleep(10);
        }
        if(!ready)goto done;
        input[0].EventType=MOUSE_EVENT;
        input[0].Event.MouseEvent.dwMousePosition.X=4;input[0].Event.MouseEvent.dwMousePosition.Y=3;
        input[0].Event.MouseEvent.dwButtonState=FROM_LEFT_1ST_BUTTON_PRESSED;
        input[1]=input[0];input[1].Event.MouseEvent.dwButtonState=0;
        input[2]=input[1];input[2].Event.MouseEvent.dwMousePosition.X=5;
        input[2].Event.MouseEvent.dwEventFlags=MOUSE_MOVED;
        input[3]=input[0];input[3].Event.MouseEvent.dwMousePosition.X=6;
        input[3].Event.MouseEvent.dwButtonState=FROM_LEFT_1ST_BUTTON_PRESSED|RIGHTMOST_BUTTON_PRESSED;
        input[4]=input[3];input[4].Event.MouseEvent.dwButtonState=RIGHTMOST_BUTTON_PRESSED;
        input[5]=input[4];input[5].Event.MouseEvent.dwButtonState=0;
        {
            static const SHORT deltas[]={240,-60,-60,-240,60,60};unsigned i;
            for(i=0;i<6;++i) {
                input[6+i]=input[5];
                input[6+i].Event.MouseEvent.dwEventFlags=i<3 ? MOUSE_WHEELED : MOUSE_HWHEELED;
                input[6+i].Event.MouseEvent.dwButtonState=(DWORD)(WORD)deltas[i]<<16;
            }
        }
        input[12].EventType=KEY_EVENT;input[12].Event.KeyEvent.bKeyDown=TRUE;
        input[12].Event.KeyEvent.wVirtualKeyCode='Z';input[12].Event.KeyEvent.wVirtualScanCode=0x2c;
        input[12].Event.KeyEvent.uChar.UnicodeChar='Z';input[12].Event.KeyEvent.wRepeatCount=1;
        input[13]=input[12];input[13].Event.KeyEvent.bKeyDown=FALSE;
        error=run16_native_backend_input(backend,input,14);
        if(error || WaitForSingleObject(target,5000)!=WAIT_OBJECT_0 ||
            !GetExitCodeProcess(target,&code) || code!=37 ||
            WaitForSingleObject(ended,0)!=WAIT_TIMEOUT ||
            run16_native_backend_members(backend,&members) || !members ||
            WaitForSingleObject(stop,0)!=WAIT_TIMEOUT)goto done;
        {
            DWORD consumed=99;
            error=run16_native_backend_input_some(backend,input+13,1,&consumed);
            if(error || consumed!=1 ||
                run16_native_backend_members(backend,&members) || !members ||
                run16_native_backend_pump(backend))goto done;
            error=0;
            puts("BACKEND-RETAINED after-target-exit=1 same-console-title=1 input-accepted=1");
        }
        CloseHandle(target);target=NULL;
        printf("BACKEND-BURST %lu PASS exit=%lu\n",round,code);
    }
    SetEvent(stop);
    error=run16_native_backend_launch(backend,&start,&target);
    if(error!=ERROR_OPERATION_ABORTED || target)goto done;
    error=0;
    result=0;
done:
    if(result && backend) {
        ntkvm_terminal_frame frame={0};unsigned i;
        Sleep(100);
        if(!run16_native_backend_capture(backend,&frame)) {
            for(i=0;i<(unsigned)(frame.rows*frame.columns);++i) {
                unsigned ch=frame.cells[i].chars[0];
                putchar(ch>=32 && ch<127 ? ch : ' ');
                if((i+1)%frame.columns==0)putchar('\n');
            }
        }
        ntkvm_terminal_frame_free(&frame);
    }
    if(target && WaitForSingleObject(target,0)==WAIT_TIMEOUT)TerminateProcess(target,99);
    if(target) {WaitForSingleObject(target,5000);CloseHandle(target);}
    run16_native_backend_close(backend);
    if(stop)CloseHandle(stop);
    printf("NATIVE-BACKEND %s error=%lu exit=%lu\n",result ? "FAIL" : "PASS",error,code);
    return result;
}
