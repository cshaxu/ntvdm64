/* Run with CREATE_NO_WINDOW: real Windows Console operations, no user desktop. */
#include "run16-exe/console_frontend.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL %d error=%lu\n",__LINE__,GetLastError()); return 1; } } while (0)
static console_io_request request;
static console_io_reply reply;
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
