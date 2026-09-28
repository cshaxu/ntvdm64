#include "ntkvm-exe/native_terminal.h"
#include "ntkvm-exe/native_conpty.h"
#include "ntkvm-exe/native_terminal_screen.h"
#include <stdio.h>
#include <string.h>
#include <wchar.h>
static int failures,checks;
#define CHECK(x) do {++checks;if(!(x)){++failures;printf("FAIL line=%d %s\n",__LINE__,#x);}} while(0)

static void feed(ntkvm_terminal *terminal,const char *bytes)
{
    size_t i;
    for(i=0;i<strlen(bytes);++i)CHECK(ntkvm_terminal_feed(terminal,(const BYTE *)bytes+i,1)==0);
}

static void model(void)
{
    ntkvm_terminal *terminal=NULL;ntkvm_terminal_frame frame={0};BYTE *reply=NULL;DWORD bytes=0;
    CHECK(ntkvm_terminal_open(4,12,&terminal)==0);if(!terminal)return;
    feed(terminal,"A\xe4\xb8\xad" "e\xcc\x81\x1b[2;1H\x1b[38;2;11;22;33mZ");
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    if(frame.cells) {
        CHECK(frame.rows==4 && frame.columns==12 && frame.cells[0].chars[0]=='A');
        CHECK(frame.cells[1].chars[0]==0x4e2d && frame.cells[1].width==2);
        CHECK(frame.cells[3].chars[0]=='e' && frame.cells[3].chars[1]==0x301);
        CHECK(frame.cells[12].chars[0]=='Z' && frame.cells[12].fg.rgb.red==11 &&
            frame.cells[12].fg.rgb.green==22 && frame.cells[12].fg.rgb.blue==33);
    }
    ntkvm_terminal_frame_free(&frame);
    feed(terminal,"\x1b[3;4H\x1b[6n\x1b[?25l");
    CHECK(ntkvm_terminal_take_replies(terminal,&reply,&bytes)==0);
    CHECK(bytes==6 && reply && !memcmp(reply,"\x1b[3;4R",6));
    if(reply)HeapFree(GetProcessHeap(),0,reply);
    CHECK(ntkvm_terminal_take_replies(terminal,&reply,&bytes)==0 && !bytes && !reply);
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    CHECK(!frame.cursor_visible && frame.cursor.row==2 && frame.cursor.col==3);
    ntkvm_terminal_frame_free(&frame);
    feed(terminal,"\x1b[?1049h\x1b[HX");
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    CHECK(frame.cells && frame.cells[0].chars[0]=='X');ntkvm_terminal_frame_free(&frame);
    feed(terminal,"\x1b[?1049l");
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    CHECK(frame.cells && frame.cells[0].chars[0]=='A');ntkvm_terminal_frame_free(&frame);
    CHECK(ntkvm_terminal_resize(terminal,5,20)==0);
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    CHECK(frame.rows==5 && frame.columns==20 && frame.cells && frame.cells[0].chars[0]=='A');
    ntkvm_terminal_frame_free(&frame);ntkvm_terminal_close(terminal);
}

static void frame_revision(void)
{
    ntkvm_terminal *terminal=NULL;ntkvm_terminal_frame frame={0};ULONGLONG revision;
    CHECK(ntkvm_terminal_open(4,12,&terminal)==0);if(!terminal)return;
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);revision=frame.revision;
    ntkvm_terminal_frame_free(&frame);
    feed(terminal,"\x1b[6n"); /* Query/reply alone is not a new picture. */
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0 && frame.revision==revision);
    ntkvm_terminal_frame_free(&frame);
    feed(terminal,"X");
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0 && frame.revision>revision);
    revision=frame.revision;ntkvm_terminal_frame_free(&frame);
    feed(terminal,"\x1b[2;2H");
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0 && frame.revision>revision);
    ntkvm_terminal_frame_free(&frame);ntkvm_terminal_close(terminal);
}

static int child(void)
{
    static const char text[]="\x1b[2J\x1b[HVT-PRODUCTION\x1b[3;4HX";
    DWORD mode,written;HANDLE output=GetStdHandle(STD_OUTPUT_HANDLE);
    if(!GetConsoleMode(output,&mode) || !SetConsoleMode(output,mode|ENABLE_VIRTUAL_TERMINAL_PROCESSING))return 91;
    if(!WriteFile(output,text,sizeof(text)-1,&written,NULL) || written!=sizeof(text)-1)return 92;
    return 37;
}

static void history(void)
{
    ntkvm_terminal *terminal=NULL;ntkvm_terminal_frame frame={0};
    CHECK(ntkvm_terminal_open(2,8,&terminal)==0);if(!terminal)return;
    CHECK(ntkvm_terminal_history_limit(terminal,2)==0);
    feed(terminal,"12345678\r\nB\r\nC");
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    CHECK(frame.history_rows==1 && frame.rows==3 && frame.viewport_rows==2);
    CHECK(frame.cells && frame.cells[0].chars[0]=='1' && frame.cells[7].chars[0]=='8' &&
        frame.cells[8].chars[0]=='B' && frame.cells[16].chars[0]=='C');
    ntkvm_terminal_frame_free(&frame);
    CHECK(ntkvm_terminal_resize(terminal,2,4)==0);
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    CHECK(frame.columns==8 && frame.viewport_columns==4 && frame.cells && frame.cells[7].chars[0]=='8');
    ntkvm_terminal_frame_free(&frame);
    feed(terminal,"\x1b[?1049h\x1b[HX");
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    CHECK(frame.history_rows==0 && frame.rows==2 && frame.columns==4 && frame.cells && frame.cells[0].chars[0]=='X');
    ntkvm_terminal_frame_free(&frame);
    feed(terminal,"\x1b[?1049l");
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    CHECK(frame.history_rows==1 && frame.cells && frame.cells[7].chars[0]=='8');
    ntkvm_terminal_frame_free(&frame);ntkvm_terminal_close(terminal);terminal=NULL;

    CHECK(ntkvm_terminal_open(2,4,&terminal)==0);if(!terminal)return;
    CHECK(ntkvm_terminal_history_limit(terminal,2)==0);
    feed(terminal,"A\r\nB\r\nC\r\nD\r\nE");
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    CHECK(frame.history_rows==2 && frame.rows==4 && frame.cells &&
        frame.cells[0].chars[0]=='B' && frame.cells[4].chars[0]=='C' &&
        frame.cells[8].chars[0]=='D' && frame.cells[12].chars[0]=='E');
    ntkvm_terminal_frame_free(&frame);
    CHECK(ntkvm_terminal_history_limit(terminal,1)==0);
    CHECK(ntkvm_terminal_resize(terminal,3,4)==0);
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    CHECK(frame.history_rows==0 && frame.rows==3 && frame.cells &&
        frame.cells[0].chars[0]=='C' && frame.cells[4].chars[0]=='D' && frame.cells[8].chars[0]=='E');
    ntkvm_terminal_frame_free(&frame);
    feed(terminal,"\r\nF\x1b[3J");
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    CHECK(frame.history_rows==0 && frame.rows==3);
    ntkvm_terminal_frame_free(&frame);ntkvm_terminal_close(terminal);
}

static void integration(PCWSTR directory)
{
    WCHAR self[MAX_PATH],command[1024];ntkvm_terminal *terminal=NULL;ntkvm_conpty *pty=NULL;
    run16_native_start start={0};PROCESS_INFORMATION process={0};ntkvm_terminal_frame frame={0};
    DWORD error=0,code=0;COORD size={80,25};static const WCHAR environment[]={0,0};
    CHECK(GetModuleFileNameW(NULL,self,MAX_PATH)!=0);
    swprintf_s(command,1024,L"\"%ls\" --child",self);
    error=ntkvm_terminal_open(size.Y,size.X,&terminal);CHECK(error==0);if(error)goto done;
    error=ntkvm_conpty_open(size,ntkvm_terminal_feed,terminal,&pty);CHECK(error==0);if(error)goto done;
    start.application=self;start.command=command;start.directory=directory;
    start.environment=environment;start.console_mask=7;
    error=ntkvm_conpty_launch(pty,&start,&process);CHECK(error==0);if(error)goto done;
    CHECK(WaitForSingleObject(process.hProcess,10000)==WAIT_OBJECT_0);
    CHECK(GetExitCodeProcess(process.hProcess,&code) && code==37);
    CHECK(ntkvm_conpty_release(pty)==0);
    CHECK(WaitForSingleObject(ntkvm_conpty_ended(pty),5000)==WAIT_OBJECT_0 && ntkvm_conpty_error(pty)==0);
    error=ntkvm_terminal_capture(terminal,&frame);CHECK(error==0);
    if(!error) {
        static const char expected[]="VT-PRODUCTION";size_t i;
        for(i=0;i<sizeof(expected)-1;++i)CHECK(frame.cells[i].chars[0]==(uint32_t)expected[i]);
        CHECK(frame.cells[2*80+3].chars[0]=='X');
    }
done:
    if(process.hProcess) {
        if(WaitForSingleObject(process.hProcess,0)==WAIT_TIMEOUT) {
            TerminateProcess(process.hProcess,99);WaitForSingleObject(process.hProcess,5000);
        }
        CloseHandle(process.hProcess);CloseHandle(process.hThread);
    }
    ntkvm_conpty_close(pty);ntkvm_terminal_frame_free(&frame);ntkvm_terminal_close(terminal);
}

static void console_seed(void)
{
    ntkvm_terminal *terminal=NULL;ntkvm_terminal_frame frame={0};
    CONSOLE_SCREEN_BUFFER_INFOEX info={sizeof(info)};CHAR_INFO cells[40];
    BYTE *reply=NULL;DWORD count=0;unsigned i;
    ZeroMemory(cells,sizeof(cells));
    for(i=0;i<40;++i) {cells[i].Char.UnicodeChar=L' ';cells[i].Attributes=7;}
    cells[0].Char.UnicodeChar=L'H';cells[8].Char.UnicodeChar=L'I';
    cells[16].Char.UnicodeChar=L'V';cells[17].Char.UnicodeChar=1;
    cells[18].Char.UnicodeChar=0x4e2d;cells[18].Attributes=7|COMMON_LVB_LEADING_BYTE;
    cells[19]=cells[18];cells[19].Attributes=7|COMMON_LVB_TRAILING_BYTE;
    cells[24].Char.UnicodeChar=L'C';cells[24].Attributes=2|COMMON_LVB_UNDERSCORE;
    info.dwSize.X=8;info.dwSize.Y=5;info.dwCursorPosition.X=2;info.dwCursorPosition.Y=4;
    info.wAttributes=7;info.ColorTable[7]=RGB(170,170,170);info.ColorTable[2]=RGB(0,170,0);
    CHECK(ntkvm_terminal_open(3,8,&terminal)==0);if(!terminal)return;
    CHECK(ntkvm_terminal_history_limit(terminal,2)==0);
    CHECK(ntkvm_terminal_seed_console(terminal,&info,cells,3)==ERROR_INVALID_PARAMETER);
    CHECK(ntkvm_terminal_seed_console(terminal,&info,cells,2)==0);
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    CHECK(frame.history_rows==2 && frame.rows==5 && frame.columns==8);
    CHECK(frame.cursor.row==4 && frame.cursor.col==2);
    CHECK(frame.cells && frame.cells[0].chars[0]=='H' && frame.cells[8].chars[0]=='I' && frame.cells[16].chars[0]=='V');
    CHECK(frame.cells && frame.cells[17].chars[0]==0x263a);
    CHECK(frame.cells && frame.cells[18].chars[0]==0x4e2d && frame.cells[18].width==2);
    CHECK(frame.cells && frame.cells[24].attrs.underline && frame.cells[24].fg.rgb.green==170);
    ntkvm_terminal_frame_free(&frame);
    CHECK(ntkvm_terminal_feed(terminal,(const BYTE *)"\033[6nX",5)==0);
    CHECK(ntkvm_terminal_take_replies(terminal,&reply,&count)==0);
    CHECK(count==6 && reply && !memcmp(reply,"\033[3;3R",6));
    if(reply)HeapFree(GetProcessHeap(),0,reply);
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    CHECK(frame.cells && frame.cells[34].chars[0]=='X' && frame.cells[0].chars[0]=='H');
    ntkvm_terminal_frame_free(&frame);ntkvm_terminal_close(terminal);
}

/* Diagnostic for the rejected shortcut of reusing fresh-only Console seeding
 * on a live native parser. Success here proves the limitation, NOT handoff
 * correctness. Kept separate from the production terminal acceptance count. */
static int live_seed_audit(void)
{
    static const char modes[]="\033[2;3H\0337\033[38;2;11;22;33m\033[?7l";
    ntkvm_terminal *terminal=NULL;ntkvm_terminal_frame frame={0};
    CONSOLE_SCREEN_BUFFER_INFOEX info={sizeof(info)};CHAR_INFO cells[40];
    unsigned i;BOOL pen_lost=FALSE,wrap_lost=FALSE,history_duplicated=FALSE;
    BOOL saved_preserved=FALSE,utf8_preserved=FALSE,csi_lost=FALSE;
    ZeroMemory(cells,sizeof(cells));
    for(i=0;i<40;++i) {cells[i].Char.UnicodeChar=L' ';cells[i].Attributes=7;}
    info.dwSize.X=8;info.dwSize.Y=5;info.dwCursorPosition.X=0;info.dwCursorPosition.Y=1;
    info.wAttributes=7;info.ColorTable[7]=RGB(170,170,170);
    CHECK(ntkvm_terminal_open(4,8,&terminal)==0);if(!terminal)return 1;
    CHECK(ntkvm_terminal_history_limit(terminal,8)==0);
    CHECK(ntkvm_terminal_feed(terminal,(const BYTE *)modes,sizeof(modes)-1)==0);
    CHECK(ntkvm_terminal_seed_console(terminal,&info,cells,1)==0);
    CHECK(ntkvm_terminal_feed(terminal,(const BYTE *)"X",1)==0);
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    if(frame.cells)pen_lost=frame.cells[8].chars[0]=='X' && frame.cells[8].fg.rgb.red==170;
    ntkvm_terminal_frame_free(&frame);
    CHECK(ntkvm_terminal_feed(terminal,(const BYTE *)"\033[1;8HAB",8)==0);
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    if(frame.cells)wrap_lost=frame.cells[16].chars[0]=='B';
    ntkvm_terminal_frame_free(&frame);
    CHECK(ntkvm_terminal_feed(terminal,(const BYTE *)"\0338",2)==0);
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    saved_preserved=frame.cursor.row==2 && frame.cursor.col==2;
    ntkvm_terminal_frame_free(&frame);
    CHECK(ntkvm_terminal_seed_console(terminal,&info,cells,1)==0);
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    history_duplicated=frame.history_rows==2;
    ntkvm_terminal_frame_free(&frame);ntkvm_terminal_close(terminal);terminal=NULL;

    CHECK(ntkvm_terminal_open(4,8,&terminal)==0);if(!terminal)return 1;
    info.dwSize.Y=4;info.dwCursorPosition.Y=0;
    CHECK(ntkvm_terminal_feed(terminal,(const BYTE *)"\xe4",1)==0);
    CHECK(ntkvm_terminal_seed_console(terminal,&info,cells,0)==0);
    CHECK(ntkvm_terminal_feed(terminal,(const BYTE *)"\xb8\xad",2)==0);
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    if(frame.cells)utf8_preserved=frame.cells[0].chars[0]==0x4e2d;
    ntkvm_terminal_frame_free(&frame);ntkvm_terminal_close(terminal);
    terminal=NULL;
    CHECK(ntkvm_terminal_open(4,8,&terminal)==0);if(!terminal)return 1;
    CHECK(ntkvm_terminal_feed(terminal,(const BYTE *)"\033[38;2;",7)==0);
    CHECK(ntkvm_terminal_seed_console(terminal,&info,cells,0)==0);
    CHECK(ntkvm_terminal_feed(terminal,(const BYTE *)"11;22;33mZ",10)==0);
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    if(frame.cells)csi_lost=frame.cells[0].chars[0]=='1' && frame.cells[1].chars[0]=='1';
    ntkvm_terminal_frame_free(&frame);ntkvm_terminal_close(terminal);
    CHECK(pen_lost);CHECK(wrap_lost);CHECK(history_duplicated);
    CHECK(saved_preserved);CHECK(utf8_preserved);CHECK(csi_lost);
    printf("LIVE-SEED-LIMITATION pen-lost=%u wrap-lost=%u history-duplicated=%u saved-preserved=%u utf8-preserved=%u csi-lost=%u checks=%d failures=%d\n",
        pen_lost,wrap_lost,history_duplicated,saved_preserved,utf8_preserved,csi_lost,checks,failures);
    return failures ? 1 : 0;
}

static int console_handoff(void)
{
    ntkvm_terminal *terminal=NULL;ntkvm_terminal_frame frame={0};
    CONSOLE_SCREEN_BUFFER_INFOEX info={sizeof(info)};CHAR_INFO cells[40];
    static const char partial[]="\033[38;2;11;22;33m\033[";
    unsigned i;
    ZeroMemory(cells,sizeof(cells));
    for(i=0;i<40;++i) {cells[i].Char.UnicodeChar=L' ';cells[i].Attributes=7;}
    cells[0].Char.UnicodeChar=L'H';cells[8].Char.UnicodeChar=L'I';
    cells[16].Char.UnicodeChar=L'D';
    info.dwSize.X=8;info.dwSize.Y=5;info.dwCursorPosition.X=2;info.dwCursorPosition.Y=4;
    info.wAttributes=7;info.ColorTable[7]=RGB(170,170,170);
    CHECK(ntkvm_terminal_open(3,8,&terminal)==0);if(!terminal)return 1;
    CHECK(ntkvm_terminal_history_limit(terminal,2)==0);
    CHECK(ntkvm_terminal_seed_console(terminal,&info,cells,2)==0);
    CHECK(ntkvm_terminal_feed(terminal,(const BYTE *)partial,sizeof(partial)-1)==0);
    cells[16].Char.UnicodeChar=L'P';
    CHECK(ntkvm_terminal_import_console(terminal,&info,cells,2)==0);
    CHECK(ntkvm_terminal_import_console(terminal,&info,cells,2)==0);
    CHECK(ntkvm_terminal_feed(terminal,(const BYTE *)"1mZ",3)==0);
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    CHECK(frame.history_rows==2 && frame.rows==5);
    CHECK(frame.cells && frame.cells[0].chars[0]=='H' && frame.cells[8].chars[0]=='I' && frame.cells[16].chars[0]=='P');
    CHECK(frame.cells && frame.cells[34].chars[0]=='Z' && frame.cells[34].attrs.bold && frame.cells[34].fg.rgb.red==11);
    CHECK(frame.cursor.row==4 && frame.cursor.col==3);
    ntkvm_terminal_frame_free(&frame);
    CHECK(ntkvm_terminal_import_console(terminal,&info,cells,3)==ERROR_INVALID_PARAMETER);
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    CHECK(frame.cells && frame.cells[34].chars[0]=='Z' && frame.history_rows==2);
    ntkvm_terminal_frame_free(&frame);ntkvm_terminal_close(terminal);
    printf("CONSOLE-HANDOFF checks=%d failures=%d\n",checks,failures);
    return failures ? 1 : 0;
}

static int console_unicode(void)
{
    ntkvm_terminal *terminal=NULL;ntkvm_terminal_frame frame={0};
    CONSOLE_SCREEN_BUFFER_INFOEX info={sizeof(info)};CHAR_INFO cells[24];unsigned i;
    static const char native[]="\033[38;2;11;22;33m\033[1;3me\xcc\x81\xe4\xb8\xad\xf0\x9f\x98\x80\r\nB\xcc\x81\r\nC";
    ZeroMemory(cells,sizeof(cells));
    for(i=0;i<24;++i) {cells[i].Char.UnicodeChar=L' ';cells[i].Attributes=7;}
    info.dwSize.X=8;info.dwSize.Y=3;info.dwCursorPosition.X=1;info.dwCursorPosition.Y=2;
    info.wAttributes=7;info.ColorTable[7]=RGB(170,170,170);
    info.ColorTable[1]=RGB(10,20,30);info.ColorTable[2]=RGB(80,90,100);
    cells[0].Char.UnicodeChar=L'e';cells[0].Attributes=1;
    cells[1].Char.UnicodeChar=0x4e2d;cells[1].Attributes=1|COMMON_LVB_LEADING_BYTE;
    cells[2]=cells[1];cells[2].Attributes=1|COMMON_LVB_TRAILING_BYTE;
    cells[3].Char.UnicodeChar=0xd83d;cells[3].Attributes=1|COMMON_LVB_LEADING_BYTE;
    cells[4].Char.UnicodeChar=0xde00;cells[4].Attributes=1|COMMON_LVB_TRAILING_BYTE;
    cells[8].Char.UnicodeChar=L'B';cells[8].Attributes=1;
    cells[16].Char.UnicodeChar=L'C';cells[16].Attributes=1;
    CHECK(ntkvm_terminal_open(2,8,&terminal)==0);if(!terminal)return 1;
    CHECK(ntkvm_terminal_history_limit(terminal,2)==0);
    CHECK(ntkvm_terminal_feed(terminal,(const BYTE *)native,sizeof(native)-1)==0);
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    CHECK(frame.history_rows==1);ntkvm_terminal_frame_free(&frame);
    CHECK(ntkvm_terminal_import_console(terminal,&info,cells,1)==0);
    CHECK(ntkvm_terminal_import_console(terminal,&info,cells,1)==0);
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    CHECK(frame.cells && frame.rows==3 && frame.history_rows==1);
    CHECK(frame.cells && frame.cells[0].chars[1]==0x301 && frame.cells[0].fg.rgb.red==11 && frame.cells[0].fg.rgb.blue==33);
    CHECK(frame.cells && frame.cells[1].chars[0]==0x4e2d && frame.cells[1].width==2 && frame.cells[3].chars[0]==0x1f600 && frame.cells[3].width==2);
    CHECK(frame.cells && frame.cells[8].chars[1]==0x301 && frame.cells[8].attrs.italic && frame.cells[8].attrs.bold);
    ntkvm_terminal_frame_free(&frame);
    cells[8].Char.UnicodeChar=L'X';cells[8].Attributes=2;
    CHECK(ntkvm_terminal_import_console(terminal,&info,cells,1)==0);
    CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
    CHECK(frame.cells && frame.cells[8].chars[0]=='X' && !frame.cells[8].chars[1] && frame.cells[8].fg.rgb.red==80);
    CHECK(frame.cells && frame.cells[16].chars[0]=='C' && frame.cells[16].fg.rgb.red==11);
    CHECK(frame.cells && frame.cells[0].chars[1]==0x301);
    ntkvm_terminal_frame_free(&frame);ntkvm_terminal_close(terminal);
    printf("CONSOLE-UNICODE checks=%d failures=%d\n",checks,failures);
    return failures ? 1 : 0;
}

typedef struct concurrent_output {
    ntkvm_terminal *terminal;
    HANDLE ready;
    DWORD error;
} concurrent_output;
static DWORD WINAPI feed_concurrent(void *context)
{
    concurrent_output *output=context;
    static const char bytes[]="\033[2;1HBG";
    SetEvent(output->ready);
    output->error=ntkvm_terminal_feed(output->terminal,(const BYTE *)bytes,sizeof(bytes)-1);
    return 0;
}
static int console_concurrent(void)
{
    ntkvm_terminal *terminal=NULL;ntkvm_terminal_frame frame={0};
    CONSOLE_SCREEN_BUFFER_INFOEX info={sizeof(info)};CHAR_INFO cells[24];
    concurrent_output output={0};HANDLE thread=NULL;unsigned i;
    ZeroMemory(cells,sizeof(cells));
    for(i=0;i<24;++i) {cells[i].Char.UnicodeChar=L' ';cells[i].Attributes=7;}
    cells[0].Char.UnicodeChar=L'A';cells[1].Char.UnicodeChar=L'D';
    info.dwSize.X=8;info.dwSize.Y=3;info.dwCursorPosition.X=2;
    info.wAttributes=7;info.ColorTable[7]=RGB(170,170,170);
    CHECK(ntkvm_terminal_open(3,8,&terminal)==0);if(!terminal)return 1;
    CHECK(ntkvm_terminal_feed(terminal,(const BYTE *)"A",1)==0);
    output.terminal=terminal;output.ready=CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(output.ready!=NULL);if(!output.ready)goto done;
    ntkvm_terminal_screen_enter(terminal);
    thread=CreateThread(NULL,0,feed_concurrent,&output,0,NULL);
    CHECK(thread!=NULL);
    if(thread) {
        CHECK(WaitForSingleObject(output.ready,5000)==WAIT_OBJECT_0);
        CHECK(WaitForSingleObject(thread,0)==WAIT_TIMEOUT);
        CHECK(ntkvm_terminal_import_console(terminal,&info,cells,0)==0);
    }
    ntkvm_terminal_screen_leave(terminal);
    if(thread) {
        CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);
        CHECK(output.error==0);
        CHECK(ntkvm_terminal_capture(terminal,&frame)==0);
        CHECK(frame.cells && frame.cells[0].chars[0]=='A' && frame.cells[1].chars[0]=='D');
        CHECK(frame.cells && frame.cells[8].chars[0]=='B' && frame.cells[9].chars[0]=='G');
        CHECK(frame.cursor.row==1 && frame.cursor.col==2);
        /* Join before freeing thread-owned context even after a test timeout. */
        WaitForSingleObject(thread,INFINITE);CloseHandle(thread);
    }
done:
    if(output.ready)CloseHandle(output.ready);
    ntkvm_terminal_frame_free(&frame);ntkvm_terminal_close(terminal);
    printf("CONSOLE-CONCURRENT checks=%d failures=%d\n",checks,failures);
    return failures ? 1 : 0;
}

static int screen_import(void)
{
    VTerm *parser=vterm_new(4,8);VTermScreen *screen;VTermState *state;
    VTermScreenCell cells[32],cell={0};VTermPos position;unsigned i;
    static const char setup[]="PRIMARY\033[2;3H\0337\033[38;2;11;22;33m\033[?7l\033[";
    CHECK(parser!=NULL);if(!parser)return 1;
    vterm_set_utf8(parser,1);screen=vterm_obtain_screen(parser);
    state=vterm_obtain_state(parser);vterm_screen_enable_altscreen(screen,1);
    vterm_screen_reset(screen,1);ZeroMemory(cells,sizeof(cells));
    for(i=0;i<32;++i) {
        cells[i].width=1;cells[i].chars[0]=' ';
        vterm_color_rgb(&cells[i].fg,170,170,170);vterm_color_rgb(&cells[i].bg,0,0,0);
    }
    cells[0].chars[0]='D';
    CHECK(vterm_input_write(parser,setup,sizeof(setup)-1)==sizeof(setup)-1);
    CHECK(ntkvm_vterm_replace_screen(screen,4,8,cells,(VTermPos){2,7})==1);
    CHECK(vterm_input_write(parser,"1mAB",4)==4);
    CHECK(vterm_screen_get_cell(screen,(VTermPos){2,7},&cell)==1);
    CHECK(cell.chars[0]=='B' && cell.attrs.bold && cell.fg.rgb.red==11 &&
        cell.fg.rgb.green==22 && cell.fg.rgb.blue==33);
    vterm_state_get_cursorpos(state,&position);
    CHECK(position.row==2 && position.col==7); /* Original no-wrap preserved. */
    CHECK(vterm_input_write(parser,"\0338",2)==2);
    vterm_state_get_cursorpos(state,&position);
    CHECK(position.row==1 && position.col==2); /* Original saved cursor. */
    CHECK(vterm_input_write(parser,"\033[?1049h",8)==8);
    cells[0].chars[0]='A';
    CHECK(ntkvm_vterm_replace_screen(screen,4,8,cells,(VTermPos){0,0})==1);
    CHECK(vterm_screen_get_cell(screen,(VTermPos){0,0},&cell)==1 && cell.chars[0]=='A');
    CHECK(vterm_input_write(parser,"\033[?1049l",8)==8);
    CHECK(vterm_screen_get_cell(screen,(VTermPos){0,0},&cell)==1 && cell.chars[0]=='D');
    CHECK(vterm_screen_get_cell(screen,(VTermPos){2,7},&cell)==1 && cell.chars[0]=='B');
    vterm_state_get_cursorpos(state,&position);
    CHECK(position.row==1 && position.col==2);
    cells[31].width=2;
    CHECK(ntkvm_vterm_replace_screen(screen,4,8,cells,(VTermPos){0,0})==0);
    cells[31].width=1;
    CHECK(ntkvm_vterm_replace_screen(screen,4,8,cells,(VTermPos){4,0})==0);
    CHECK(ntkvm_vterm_replace_screen(screen,3,8,cells,(VTermPos){0,0})==0);
    CHECK(vterm_screen_get_cell(screen,(VTermPos){0,0},&cell)==1 && cell.chars[0]=='D');
    vterm_state_get_cursorpos(state,&position);
    CHECK(position.row==1 && position.col==2);
    CHECK(vterm_input_write(parser,"\xe4",1)==1);
    cells[0].chars[0]='D';cells[8].chars[0]=0x4e2d;cells[8].width=2;
    CHECK(ntkvm_vterm_replace_screen(screen,4,8,cells,(VTermPos){0,1})==1);
    CHECK(vterm_input_write(parser,"\xb8\xad",2)==2);
    CHECK(vterm_screen_get_cell(screen,(VTermPos){0,1},&cell)==1 && cell.chars[0]==0x4e2d && cell.width==2);
    CHECK(vterm_screen_get_cell(screen,(VTermPos){1,0},&cell)==1 && cell.chars[0]==0x4e2d && cell.width==2);
    CHECK(vterm_input_write(parser,"\033[4;4HZ",7)==7);
    CHECK(vterm_screen_get_cell(screen,(VTermPos){3,3},&cell)==1 && cell.chars[0]=='Z');
    vterm_free(parser);
    printf("SCREEN-IMPORT checks=%d failures=%d\n",checks,failures);
    return failures ? 1 : 0;
}

int wmain(int argc,WCHAR **argv)
{
    if(argc==2 && !wcscmp(argv[1],L"--child"))return child();
    if(argc==2 && !wcscmp(argv[1],L"--live-seed-audit"))return live_seed_audit();
    if(argc==2 && !wcscmp(argv[1],L"--screen-import"))return screen_import();
    if(argc==2 && !wcscmp(argv[1],L"--console-handoff"))return console_handoff();
    if(argc==2 && !wcscmp(argv[1],L"--console-unicode"))return console_unicode();
    if(argc==2 && !wcscmp(argv[1],L"--console-concurrent"))return console_concurrent();
    if(argc!=2)return 2;
    model();history();console_seed();frame_revision();integration(argv[1]);
    printf("NATIVE-TERMINAL checks=%d failures=%d\n",checks,failures);
    return failures ? 1 : 0;
}
