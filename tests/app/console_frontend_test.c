/* Run with CREATE_NO_WINDOW: real Windows Console operations, no user desktop. */
#include "ntkvm-exe/console_frontend.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL %d error=%lu\n",__LINE__,GetLastError()); return 1; } } while (0)
static console_io_request request;
static console_io_reply reply;
static DWORD begin_error,end_error,screen_begins,screen_ends,screen_leaves;
static BOOL screen_written;
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
    ZeroMemory(records,sizeof(*records));records->EventType=CONSOLE_INPUT_RELATIVE_MOUSE;
    memcpy(&records->Event,context,sizeof(console_mouse_input));*count=1;return ERROR_SUCCESS;
}
static DWORD pointer_input(void *context,BOOL peek,INPUT_RECORD *records,DWORD capacity,DWORD *count)
{
    DWORD error=relative_input(context,peek,records,capacity,count);
    if(!error)records->EventType=CONSOLE_INPUT_POINTER;
    return error;
}
static void operation(run16_console_frontend *owner,uint32_t op)
{
    ZeroMemory(&request,sizeof(request));
    request.version=CONSOLE_IO_VERSION; request.generation=owner->generation;
    request.sequence=owner->sequence+1; request.operation=op;
}
int main(void)
{
    run16_console_frontend owner={0};
    CONSOLE_CURSOR_INFO cursor;
    DWORD count,mode;
    COORD position={0,0};
    char cells[8]={0};
    WORD attributes[3]={0};
    owner.generation=17;
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
        console_mouse_input mouse={INT32_MIN,INT32_MAX,640,400,3,CONSOLE_MOUSE_MOVE};
        console_io_input wire;
        CHECK(sizeof(mouse)<=sizeof(((INPUT_RECORD *)0)->Event));
        owner.read_input=relative_input;owner.io_context=&mouse;
        operation(&owner,CONSOLE_IO_READ_INPUT);request.state.count=1;
        CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result && reply.state.count==1);
        memcpy(&wire,reply.data,sizeof(wire));
        CHECK(wire.type==CONSOLE_INPUT_RELATIVE_MOUSE && wire.x==INT32_MIN && wire.y==INT32_MAX &&
            wire.buttons==3 && wire.flags==CONSOLE_MOUSE_MOVE && wire.control==(640u|(400u<<16)));
        mouse.buttons=4;
        operation(&owner,CONSOLE_IO_PEEK_INPUT);request.state.count=1;
        CHECK(!run16_console_dispatch(&owner,&request,&reply) && !reply.result && reply.error==ERROR_INVALID_DATA);
        mouse.buttons=0;mouse.dx=mouse.dy=0;mouse.action=CONSOLE_MOUSE_LEAVE;
        CHECK(!console_mouse_input_valid(&mouse));
        mouse.width=mouse.height=0;CHECK(console_mouse_input_valid(&mouse));
        owner.read_input=NULL;owner.io_context=NULL;
        puts("PASS private DOS relative input retains 32-bit motion/geometry; malformed buttons and leave rejected; old protocol rejected");
    }
    {
        console_pointer_input mouse={INT32_MIN,INT32_MAX,SHIFT_PRESSED|RIGHT_CTRL_PRESSED,3,CONSOLE_MOUSE_MOVE};
        console_io_input wire;
        owner.read_input=pointer_input;owner.io_context=&mouse;
        operation(&owner,CONSOLE_IO_READ_INPUT);request.state.count=1;
        CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result && reply.state.count==1);
        memcpy(&wire,reply.data,sizeof(wire));
        CHECK(wire.type==CONSOLE_INPUT_POINTER && wire.x==INT32_MIN && wire.y==INT32_MAX &&
            wire.buttons==3 && wire.flags==CONSOLE_MOUSE_MOVE && wire.control==mouse.control);
        mouse.buttons=4;
        operation(&owner,CONSOLE_IO_READ_INPUT);request.state.count=1;
        CHECK(!run16_console_dispatch(&owner,&request,&reply) && !reply.result && reply.error==ERROR_INVALID_DATA);
        mouse.buttons=0;mouse.action=CONSOLE_MOUSE_LEAVE+1;
        operation(&owner,CONSOLE_IO_READ_INPUT);request.state.count=1;
        CHECK(!run16_console_dispatch(&owner,&request,&reply) && !reply.result && reply.error==ERROR_INVALID_DATA);
        owner.read_input=NULL;owner.io_context=NULL;
        puts("PASS worker-owned pointer wire: full signed motion, modifiers, button/action validation; no frontend dimensions");
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
        COORD extent={120,60},last={80,28},cursor_at={99,40};
        SHORT heights[]={22,25,28,43,50};DWORD row,index;WCHAR cell;
        CHECK(SetConsoleWindowInfo(owner.output,TRUE,&tiny));
        CHECK(SetConsoleScreenBufferSize(owner.output,extent));
        for(row=0;row<60;++row)
            CHECK(FillConsoleOutputCharacterW(owner.output,(WCHAR)('A'+row%26),120,
                (COORD){0,(SHORT)row},&count) && count==120);
        CHECK(SetConsoleCursorPosition(owner.output,cursor_at));
        CHECK(!run16_console_prepare_dos(owner.output,&logical,last));
        CHECK(GetConsoleScreenBufferInfo(owner.output,&actual));
        CHECK(actual.dwSize.X==80 && actual.dwSize.Y==28 && logical.Right==79 && logical.Bottom==27);
        CHECK(actual.dwCursorPosition.X==79 && actual.dwCursorPosition.Y==27);
        CHECK(ReadConsoleOutputCharacterW(owner.output,&cell,1,(COORD){0,0},&count) && count==1 && cell=='N');
        CHECK(ReadConsoleOutputCharacterW(owner.output,&cell,1,(COORD){79,27},&count) && count==1 && cell=='O');
        owner.logical_window=&logical;
        for(index=0;index<ARRAYSIZE(heights);++index) {
            logical=(SMALL_RECT){0,0,79,heights[index]-1};
            CHECK(!run16_console_prepare_dos(owner.output,&logical,last));
            operation(&owner,CONSOLE_IO_SCREEN_INFO);
            CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result);
            CHECK(reply.state.width==80 && reply.state.height==heights[index] &&
                reply.state.right==79 && reply.state.bottom==heights[index]-1);
        }
        logical=(SMALL_RECT){0,0,119,39};saved=logical;
        CHECK(run16_console_prepare_dos(INVALID_HANDLE_VALUE,&logical,last)==ERROR_INVALID_HANDLE);
        CHECK(!memcmp(&logical,&saved,sizeof(saved)));
        CHECK(!run16_console_prepare_dos(owner.output,&logical,(COORD){0,0}));
        CHECK(logical.Right==79 && logical.Bottom==24);
        CHECK(!run16_console_dos_size((COORD){40,25}) && !run16_console_dos_size((COORD){80,40}));
        owner.logical_window=NULL;
        puts("PASS DOS geometry: original five return modes, last-mode/default fallback, no reflow, cursor-visible rows, clamped cursor, failure does not acknowledge");
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
