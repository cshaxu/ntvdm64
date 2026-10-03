/* Run with CREATE_NO_WINDOW: real Windows Console operations, no user desktop. */
#include "ntcon-exe/console_frontend.h"
#include "console_geometry_fixture.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL %d error=%lu\n",__LINE__,GetLastError()); return 1; } } while (0)
static console_io_request request;
static console_io_reply reply;
static DWORD begin_error,end_error,screen_begins,screen_ends,screen_leaves;
static BOOL screen_written;
static DWORD title_notifications;
static DWORD published_titles;
static char last_published_title[CONSOLE_IO_TITLE_BYTES];
static void title_changed(void *context)
{
    (void)context;++title_notifications;
}
static void publish_title(void *context,const char *title)
{
    (void)context;
    strcpy_s(last_published_title,sizeof(last_published_title),title);
    ++published_titles;
}
static DWORD begin_screen(void *context)
{
    (void)context;++screen_begins;return begin_error;
}
static DWORD end_screen(void *context,BOOL written)
{
    (void)context;++screen_ends;screen_written=written;return end_error;
}
static void leave_screen(void *context)
{
    (void)context;++screen_leaves;
}
static DWORD relative_input(void *context,BOOL peek,INPUT_RECORD *records,DWORD capacity,DWORD *count)
{
    (void)peek;
    if(!capacity)return ERROR_INSUFFICIENT_BUFFER;
    ZeroMemory(records,sizeof(*records));records->EventType=CONSOLE_INPUT_FRAME_MOUSE;
    memcpy(&records->Event,context,sizeof(console_frame_mouse_input));*count=1;return ERROR_SUCCESS;
}
static void operation(run16_console_frontend *owner,uint32_t op)
{
    ZeroMemory(&request,sizeof(request));
    request.version=CONSOLE_IO_VERSION; request.generation=owner->generation;
    request.sequence=owner->sequence+1; request.operation=op;
}
static int run_on_private_desktop(void)
{
    char name[64],image[MAX_PATH],command[MAX_PATH+3];
    STARTUPINFOA start={sizeof(start)};
    PROCESS_INFORMATION child={0};
    HDESK desktop;
    DWORD code=ERROR_GEN_FAILURE;
    sprintf_s(name,sizeof(name),"NTVDMConsoleTest-%lu",GetCurrentProcessId());
    desktop=CreateDesktopA(name,NULL,NULL,0,GENERIC_ALL,NULL);
    if(!desktop)return (int)GetLastError();
    if(!GetModuleFileNameA(NULL,image,sizeof(image))) { CloseDesktop(desktop);return (int)GetLastError(); }
    sprintf_s(command,sizeof(command),"\"%s\"",image);
    start.lpDesktop=name;
    if(CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&start,&child)) {
        if(WaitForSingleObject(child.hProcess,120000)==WAIT_OBJECT_0)
            GetExitCodeProcess(child.hProcess,&code);
        else { TerminateProcess(child.hProcess,ERROR_TIMEOUT);code=ERROR_TIMEOUT; }
        CloseHandle(child.hThread);CloseHandle(child.hProcess);
    } else code=GetLastError();
    CloseDesktop(desktop);
    fprintf(stderr,"private Console frontend test exit=%lu\n",code);
    return (int)code;
}
int main(void)
{
    run16_console_frontend owner={0};
    CONSOLE_CURSOR_INFO cursor;
    DWORD count,mode;
    char desktop[96];DWORD desktop_size;
    COORD position={0,0};
    char cells[8]={0};
    WORD attributes[3]={0};
    if(!GetUserObjectInformationA(GetThreadDesktop(GetCurrentThreadId()),UOI_NAME,
        desktop,sizeof(desktop),&desktop_size) || strncmp(desktop,"NTVDMConsoleTest-",17))
        return run_on_private_desktop();
    owner.generation=17;
    owner.title_changed=title_changed;
    owner.publish_title=publish_title;
    owner.input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,OPEN_EXISTING,0,NULL);
    owner.output=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
    CHECK(owner.input!=INVALID_HANDLE_VALUE && owner.output!=INVALID_HANDLE_VALUE);
    CHECK(sizeof(console_io_state)==88 && offsetof(console_io_reply,data)==112);
    operation(&owner,CONSOLE_IO_SCREEN_INFO);
    request.version++;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_REVISION_MISMATCH && !owner.sequence);
    request.version=CONSOLE_IO_VERSION-1;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_REVISION_MISMATCH && !owner.sequence);
    request.version=CONSOLE_IO_VERSION;request.generation++;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_ACCESS_DENIED && !owner.sequence);
    request.generation=owner.generation;request.sequence=2;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA && !owner.sequence);
    request.sequence=1;request.bytes=1;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA && !owner.sequence);
    request.bytes=0;
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result);
    CHECK(reply.state.width>=8 && reply.state.height>=4 && reply.sequence==1);
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA && owner.sequence==1);
    operation(&owner,CONSOLE_IO_SET_MODE);
    request.state.mode=ENABLE_PROCESSED_OUTPUT|ENABLE_WRAP_AT_EOL_OUTPUT;
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result);
    operation(&owner,CONSOLE_IO_GET_MODE);
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result);
    CHECK(reply.state.mode==(ENABLE_PROCESSED_OUTPUT|ENABLE_WRAP_AT_EOL_OUTPUT));
    operation(&owner,CONSOLE_IO_GET_MODE);request.state.input=1;
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result);
    CHECK(GetConsoleMode(owner.input,&mode) && mode==reply.state.mode);
    operation(&owner,CONSOLE_IO_CURSOR_POSITION);
    request.state.x=40000;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA);
    request.state.x=0;
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result);
    operation(&owner,CONSOLE_IO_ATTRIBUTE);request.state.attribute=0x1f;
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result);
    operation(&owner,CONSOLE_IO_WRITE);
    request.bytes=CONSOLE_IO_DATA_BYTES+1;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA);
    request.bytes=4;memcpy(request.data,"A\r\nB",4);
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result && reply.state.count==4);
    CHECK(ReadConsoleOutputCharacterA(owner.output,cells,1,position,&count) && count==1 && cells[0]=='A');
    position.Y=1;
    CHECK(ReadConsoleOutputCharacterA(owner.output,cells,1,position,&count) && count==1 && cells[0]=='B');
    operation(&owner,CONSOLE_IO_FILL_CHARACTER);
    request.state.x=2;request.state.y=1;request.state.character='#';request.state.count=3;
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result && reply.state.count==3);
    operation(&owner,CONSOLE_IO_FILL_ATTRIBUTE);
    request.state.x=2;request.state.y=1;request.state.attribute=0x2e;request.state.count=3;
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result && reply.state.count==3);
    operation(&owner,CONSOLE_IO_SCROLL);
    request.state.top=request.state.bottom=1;request.state.right=4;
    request.state.y=2;request.state.character=' ';request.state.attribute=7;
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result);
    position.X=2;position.Y=2;
    CHECK(ReadConsoleOutputCharacterA(owner.output,cells,3,position,&count) && count==3 && !memcmp(cells,"###",3));
    CHECK(ReadConsoleOutputAttribute(owner.output,attributes,3,position,&count) && count==3 &&
        attributes[0]==0x2e && attributes[1]==0x2e && attributes[2]==0x2e);
    operation(&owner,CONSOLE_IO_CURSOR_INFO);request.state.cursor_size=20;
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result);
    CHECK(GetConsoleCursorInfo(owner.output,&cursor) && cursor.dwSize==20 && !cursor.bVisible);
    operation(&owner,CONSOLE_IO_BARRIER);
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result);
    operation(&owner,CONSOLE_IO_SET_TITLE_A);
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA);
    request.bytes=3;memcpy(request.data,"bad",3);
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA);
    request.data[0]=0;request.data[2]=0;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA);
    {
        char original[CONSOLE_IO_DATA_BYTES],actual[CONSOLE_IO_DATA_BYTES];
        GetConsoleTitleA(original,sizeof(original));
        operation(&owner,CONSOLE_IO_SET_TITLE_A);
        request.bytes=sizeof("S36-ATTACHED-CONSOLE");
        memcpy(request.data,"S36-ATTACHED-CONSOLE",request.bytes);
        CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result && title_notifications==1);
        CHECK(GetConsoleTitleA(actual,sizeof(actual))==sizeof("S36-ATTACHED-CONSOLE")-1 &&
            !strcmp(actual,"S36-ATTACHED-CONSOLE"));
        CHECK(SetConsoleTitleA(original));
        puts("PASS successful Console-title call notifies the attached frontend once");
    }
    operation(&owner,CONSOLE_IO_PUBLISH_TITLE_A);
    request.bytes=3;memcpy(request.data,"bad",3);
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA &&
        published_titles==0);
    operation(&owner,CONSOLE_IO_PUBLISH_TITLE_A);
    request.bytes=sizeof("S36-NESTED-CMD");
    memcpy(request.data,"S36-NESTED-CMD",request.bytes);
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result &&
        published_titles==1 && !strcmp(last_published_title,"S36-NESTED-CMD"));
    operation(&owner,CONSOLE_IO_GET_TITLE_A);request.state.count=CONSOLE_IO_DATA_BYTES+1;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA);
    operation(&owner,CONSOLE_IO_SET_POINTER_CLIP);request.state.has_clip=2;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA);
    operation(&owner,CONSOLE_IO_SET_POINTER);request.bytes=1;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA);
    operation(&owner,CONSOLE_IO_CURRENT_FONT);request.state.mode=2;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA);
    operation(&owner,CONSOLE_IO_FONT_SIZE);request.bytes=1;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA);
    operation(&owner,CONSOLE_IO_GET_DISPLAY_MODE);request.bytes=1;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA);
    operation(&owner,CONSOLE_IO_SET_DISPLAY_MODE);request.bytes=1;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA);
    operation(&owner,CONSOLE_IO_CODE_PAGE);request.state.input=2;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA);
    request.state.input=0;request.bytes=1;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA);
    request.bytes=0;
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result &&
        reply.state.count==GetConsoleOutputCP());
    operation(&owner,CONSOLE_IO_CODE_PAGE);request.state.input=1;
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result &&
        reply.state.count==GetConsoleCP());
    operation(&owner,CONSOLE_IO_WRITE_CELLS_W);
    request.state.width=80;request.state.height=50;request.bytes=4;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA);
    request.state.height=32767;request.state.width=32767;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA);
    operation(&owner,CONSOLE_IO_READ_CELLS_W);
    request.state.width=-1;request.state.height=1;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA);
    operation(&owner,CONSOLE_IO_PREPEND_KEYS);
    request.state.count=1;request.bytes=1;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA);
    request.state.count=CONSOLE_IO_INPUT_CAPACITY+1;
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA);
    request.state.count=1;request.bytes=sizeof(console_io_input);
    memset(request.data,0,request.bytes); /* Not a KEY_EVENT: no partial write. */
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && !reply.result &&
        reply.error==ERROR_INVALID_DATA && reply.state.count==0);
    {
        console_frame_mouse_input mouse={INT32_MIN,INT32_MAX,640,400,
            SHIFT_PRESSED|RIGHT_CTRL_PRESSED,3,CONSOLE_MOUSE_MOVE};
        console_io_input wire;
        CHECK(sizeof(mouse)<=sizeof(((INPUT_RECORD *)0)->Event));
        owner.read_input=relative_input;owner.io_context=&mouse;
        operation(&owner,CONSOLE_IO_READ_INPUT);request.state.count=1;
        CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result && reply.state.count==1);
        memcpy(&wire,reply.data,sizeof(wire));
        CHECK(wire.type==CONSOLE_INPUT_FRAME_MOUSE && wire.x==INT32_MIN && wire.y==INT32_MAX &&
            wire.buttons==3 && wire.flags==CONSOLE_MOUSE_MOVE && wire.control==mouse.control &&
            wire.menu==640 && wire.focus==400);
        mouse.buttons=4;
        operation(&owner,CONSOLE_IO_PEEK_INPUT);request.state.count=1;
        CHECK(!run16_console_dispatch(&owner,&request,&reply) && !reply.result && reply.error==ERROR_INVALID_DATA);
        mouse.buttons=0;mouse.dx=mouse.dy=0;mouse.action=CONSOLE_MOUSE_LEAVE;
        CHECK(!console_frame_mouse_input_valid(&mouse));
        mouse.width=mouse.height=0;CHECK(console_frame_mouse_input_valid(&mouse));
        owner.read_input=NULL;owner.io_context=NULL;
        puts("PASS private DOS relative input retains 32-bit motion/geometry; malformed buttons and leave rejected; old protocol rejected");
    }
    {
        console_frame_mouse_input mouse={INT32_MIN,INT32_MAX,640,400,
            SHIFT_PRESSED|RIGHT_CTRL_PRESSED,3,CONSOLE_MOUSE_MOVE};
        console_io_input wire;
        owner.read_input=relative_input;owner.io_context=&mouse;
        operation(&owner,CONSOLE_IO_READ_INPUT);request.state.count=1;
        CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result && reply.state.count==1);
        memcpy(&wire,reply.data,sizeof(wire));
        CHECK(wire.type==CONSOLE_INPUT_FRAME_MOUSE && wire.x==INT32_MIN && wire.y==INT32_MAX &&
            wire.buttons==3 && wire.flags==CONSOLE_MOUSE_MOVE && wire.control==mouse.control);
        mouse.buttons=4;
        operation(&owner,CONSOLE_IO_READ_INPUT);request.state.count=1;
        CHECK(!run16_console_dispatch(&owner,&request,&reply) && !reply.result && reply.error==ERROR_INVALID_DATA);
        mouse.buttons=0;mouse.action=CONSOLE_MOUSE_ENTER;mouse.dx=mouse.dy=0;
        operation(&owner,CONSOLE_IO_READ_INPUT);request.state.count=1;
        CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result && reply.state.count==1);
        memcpy(&wire,reply.data,sizeof(wire));
        CHECK(wire.type==CONSOLE_INPUT_FRAME_MOUSE && wire.flags==CONSOLE_MOUSE_ENTER);
        mouse.action=CONSOLE_MOUSE_LEAVE+1;
        operation(&owner,CONSOLE_IO_READ_INPUT);request.state.count=1;
        CHECK(!run16_console_dispatch(&owner,&request,&reply) && !reply.result && reply.error==ERROR_INVALID_DATA);
        owner.read_input=NULL;owner.io_context=NULL;
        puts("PASS common frame mouse wire: signed motion, geometry, modifiers, button/action validation; no worker type");
    }
    owner.screen_begin=begin_screen;owner.screen_end=end_screen;owner.leave=leave_screen;
    begin_error=ERROR_BUSY;
    operation(&owner,CONSOLE_IO_SCREEN_INFO);
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && !reply.result && reply.error==ERROR_BUSY);
    CHECK(screen_begins==1 && !screen_ends && screen_leaves==1);
    begin_error=0;
    operation(&owner,CONSOLE_IO_CURSOR_POSITION);
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result);
    CHECK(screen_begins==2 && screen_ends==1 && screen_leaves==2 && screen_written);
    operation(&owner,CONSOLE_IO_SCREEN_INFO);
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result);
    CHECK(screen_begins==3 && screen_ends==2 && screen_leaves==3 && !screen_written);
    end_error=ERROR_WRITE_FAULT;
    operation(&owner,CONSOLE_IO_SCREEN_INFO);
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && !reply.result && reply.error==ERROR_WRITE_FAULT);
    CHECK(screen_ends==3 && screen_leaves==4);
    operation(&owner,CONSOLE_IO_GET_MODE);
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result);
    CHECK(screen_begins==4 && screen_ends==3 && screen_leaves==5);
    owner.screen_end=NULL;
    operation(&owner,CONSOLE_IO_SCREEN_INFO);
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_PARAMETER);
    owner.screen_begin=NULL;owner.leave=NULL;end_error=0;
    puts("PASS screen transaction dispatch: write/read classification, begin/end failure, cleanup and non-screen exclusion");
    {
        SMALL_RECT logical={0,10,119,49},tiny={0,0,0,0},saved;
        CONSOLE_SCREEN_BUFFER_INFO actual;
        COORD extent={120,60},cursor_at={99,50};
        SHORT heights[]={22,25,28,43,50};
        SHORT input_heights[]={23,24,26,27,30,35,36,46,47};
        SHORT output_heights[]={22,25,25,28,28,28,43,43,50};
        DWORD row,index;WCHAR cell;
        CHECK(SetConsoleWindowInfo(owner.output,TRUE,&tiny));
        CHECK(SetConsoleScreenBufferSize(owner.output,extent));
        for(row=0;row<60;++row)
            CHECK(FillConsoleOutputCharacterW(owner.output,(WCHAR)('A'+row%26),120,
                (COORD){0,(SHORT)row},&count) && count==120);
        CHECK(SetConsoleCursorPosition(owner.output,cursor_at));
        CHECK(!test_prepare_vga(owner.output,&logical));
        CHECK(GetConsoleScreenBufferInfo(owner.output,&actual));
        CHECK(actual.dwSize.X==80 && actual.dwSize.Y==43 && logical.Right==79 && logical.Bottom==42);
        /* The retained cell-grid primitive wraps an out-of-range X to zero;
         * it clamps Y to the last retained row. Do not assert a new X policy. */
        CHECK(actual.dwCursorPosition.X==0 && actual.dwCursorPosition.Y==42);
        CHECK(ReadConsoleOutputCharacterW(owner.output,&cell,1,(COORD){0,0},&count) && count==1 && cell=='I');
        CHECK(ReadConsoleOutputCharacterW(owner.output,&cell,1,(COORD){79,42},&count) && count==1 && cell=='Y');
        owner.logical_window=&logical;
        for(index=0;index<ARRAYSIZE(heights);++index) {
            logical=(SMALL_RECT){0,0,79,heights[index]-1};
            CHECK(!test_prepare_vga(owner.output,&logical));
            operation(&owner,CONSOLE_IO_SCREEN_INFO);
            CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result);
            CHECK(reply.state.width==80 && reply.state.height==heights[index] &&
                reply.state.right==79 && reply.state.bottom==heights[index]-1);
        }
        for(index=0;index<ARRAYSIZE(input_heights);++index) {
            logical=(SMALL_RECT){0,0,79,input_heights[index]-1};
            CHECK(!test_prepare_vga(owner.output,&logical));
            CHECK(logical.Right==79 && logical.Bottom==output_heights[index]-1);
        }
        logical=(SMALL_RECT){0,0,119,39};saved=logical;
        CHECK(test_prepare_vga(INVALID_HANDLE_VALUE,&logical)==ERROR_INVALID_HANDLE);
        CHECK(!memcmp(&logical,&saved,sizeof(saved)));
        CHECK(!test_prepare_vga(owner.output,&logical));
        CHECK(logical.Right==79 && logical.Bottom==42);
        CHECK(ntvdm_console_return_height(40)==43);
        /* The underlying storage operation must not round to VGA modes or
         * force 80 columns. Only the worker's compatibility selector does. */
        CHECK(!run16_console_prepare_text(owner.output,&logical,(COORD){100,35}));
        CHECK(GetConsoleScreenBufferInfo(owner.output,&actual));
        CHECK(actual.dwSize.X==100 && actual.dwSize.Y==35 &&
            logical.Right==99 && logical.Bottom==34);
        CHECK(!run16_console_prepare_text(owner.output,&logical,(COORD){80,30}));
        CHECK(GetConsoleScreenBufferInfo(owner.output,&actual));
        CHECK(actual.dwSize.X==80 && actual.dwSize.Y==30 &&
            logical.Right==79 && logical.Bottom==29);
        saved=logical;
        CHECK(run16_console_prepare_text(owner.output,&logical,(COORD){0,25})==ERROR_INVALID_PARAMETER);
        CHECK(run16_console_prepare_text(owner.output,&logical,(COORD){80,0})==ERROR_INVALID_PARAMETER);
        CHECK(run16_console_prepare_text(owner.output,&logical,(COORD){-1,25})==ERROR_INVALID_PARAMETER);
        CHECK(!memcmp(&logical,&saved,sizeof(saved)));
        CHECK(GetConsoleScreenBufferInfo(owner.output,&actual) &&
            actual.dwSize.X==80 && actual.dwSize.Y==30);
        CHECK(run16_console_prepare_text(INVALID_HANDLE_VALUE,&logical,(COORD){80,28})==ERROR_INVALID_HANDLE);
        CHECK(!memcmp(&logical,&saved,sizeof(saved)));
        puts("PASS explicit text storage geometry: exact 100x35/80x30, invalid dimensions/handle preserve acknowledgement, no VGA selection");
        owner.logical_window=NULL;
        puts("PASS DOS geometry: original five return modes and midpoint boundaries, no reflow, cursor-visible rows, clamped cursor, failure does not acknowledge");
    }
    CloseHandle(owner.output);owner.output=INVALID_HANDLE_VALUE;
    operation(&owner,CONSOLE_IO_CURRENT_FONT);
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && !reply.result &&
        reply.error==ERROR_INVALID_HANDLE && !reply.state.count && !reply.bytes);
    operation(&owner,CONSOLE_IO_FONT_SIZE);
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && !reply.result &&
        reply.error==ERROR_INVALID_HANDLE && !reply.state.x && !reply.state.y);
    operation(&owner,CONSOLE_IO_SCREEN_INFO);
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && !reply.result && reply.error==ERROR_INVALID_HANDLE);
    owner.sequence=UINT32_MAX;operation(&owner,CONSOLE_IO_BARRIER);
    CHECK(run16_console_dispatch(&owner,&request,&reply)==ERROR_INVALID_DATA);
    CloseHandle(owner.input);
    puts("PASS: real Console stream/scroll/fill/cursor/mode, errors, version/generation/sequence and bounds");
    return 0;
}
