/* Real production adapter, controlled input queue. No desktop or guest. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "conapi.h"
#include "common/protocol/console_mouse.h"
static INPUT_RECORD queue[1024];
static DWORD queued, position, reads, peeks;
static DWORD mouse_capacity=256;
static BOOL WINAPI batch_read(HANDLE h,PINPUT_RECORD out,DWORD capacity,LPDWORD count)
{
    DWORD take=min(capacity,queued-position);
    (void)h; ++reads;
    memcpy(out,queue+position,take*sizeof(*out));position+=take;*count=take;
    return TRUE;
}
static BOOL WINAPI batch_peek(HANDLE h,PINPUT_RECORD out,DWORD capacity,LPDWORD count)
{
    DWORD take=min(capacity,queued-position);
    (void)h; ++peeks;
    memcpy(out,queue+position,take*sizeof(*out));*count=take;
    return TRUE;
}
#undef ReadConsoleInputW
#undef PeekConsoleInputW
#define ReadConsoleInputW batch_read
#define PeekConsoleInputW batch_peek
#define mvdm_softpc_mouse_capacity batch_mouse_capacity
#include "../../src/ntvdm-exe/win32/console_compat.c"
DWORD batch_mouse_capacity(mvdm_mouse_bridge *state)
{ (void)state;return mouse_capacity; }
/* No frontend is bound by this controlled-queue fixture. */
void OpenNtBaseClientSetCommandBinding(DWORD (*ready)(void *),void *context)
{ (void)ready;(void)context; }
DWORD OpenNtBaseClientTakeFrontend(HANDLE *p,HANDLE *s,DWORD *g,HANDLE *r)
{ (void)p;(void)s;(void)g;(void)r;return ERROR_NOT_SUPPORTED; }
DWORD OpenNtBaseClientWaitFrontend(HANDLE *p,HANDLE *s,DWORD *g,HANDLE *r)
{ (void)p;(void)s;(void)g;(void)r;return ERROR_NOT_SUPPORTED; }
DWORD OpenNtBaseClientWorkerFrontendCapability(HANDLE *capability)
{ *capability=NULL;return ERROR_NOT_SUPPORTED; }
DWORD OpenNtBaseClientAcquireConsoleContext(HANDLE frontend,HANDLE *capability)
{ (void)frontend;*capability=NULL;return ERROR_NOT_SUPPORTED; }
BOOL CntrlHandler(ULONG type) { (void)type;return FALSE; }
#define CHECK(x) do { if(!(x)) { printf("FAIL line=%d error=%lu\n",__LINE__,GetLastError());return 1; } } while(0)
static void reset(DWORD count)
{
    ZeroMemory(queue,sizeof(queue));queued=count;position=reads=peeks=0;
}
int main(void)
{
    INPUT_RECORD out[NTVDM_PC_INPUT_RECORDS+1];
    DWORD got,i,total,batches;
    HANDLE input=(HANDLE)1;
    reset(5);
    for(i=0;i<5;++i)queue[i].EventType=MOUSE_EVENT;
    mouse_capacity=0;
    CHECK(ntvdm_console_read_pc_input(input,out,NTVDM_PC_INPUT_RECORDS,&got,CONSOLE_READ_NOWAIT));
    CHECK(!got && !position && !reads && !peeks);
    mouse_capacity=2;
    CHECK(ntvdm_console_read_pc_input(input,out,NTVDM_PC_INPUT_RECORDS,&got,CONSOLE_READ_NOWAIT));
    CHECK(got==2 && position==2 && reads==1);
    mouse_capacity=256;
    CHECK(ntvdm_console_read_pc_input(input,out,NTVDM_PC_INPUT_RECORDS,&got,CONSOLE_READ_NOWAIT));
    CHECK(got==3 && position==5);
    printf("BATCH-PRESSURE full queue consumes nothing; partial capacity and recovery PASS\n");
    reset(1000);
    for(i=0;i<queued;++i) {
        queue[i].EventType=MOUSE_EVENT;
        queue[i].Event.MouseEvent.dwMousePosition.X=(SHORT)i;
        queue[i].Event.MouseEvent.dwButtonState=(i%3==1) ? 1 : 0;
        queue[i].Event.MouseEvent.dwEventFlags=MOUSE_MOVED;
    }
    for(total=batches=0;total<1000;++batches) {
        memset(out,0x55,sizeof(out));
        CHECK(ntvdm_console_read_pc_input(input,out,NTVDM_PC_INPUT_RECORDS,&got,CONSOLE_READ_NOWAIT));
        CHECK(got==5);
        for(i=0;i<got;++i) CHECK(!memcmp(&out[i],&queue[total+i],sizeof(out[i])));
        CHECK(out[NTVDM_PC_INPUT_RECORDS].EventType==0x5555);
        total+=got;
    }
    CHECK(batches==200 && reads==200 && peeks==200);
    printf("BATCH-MOUSE 1000 records / 200 reads; order, buttons, bounds PASS\n");
    CHECK(ntvdm_console_read_pc_input(input,out,NTVDM_PC_INPUT_RECORDS,&got,CONSOLE_READ_NOWAIT) && got==0);
    reset(1000);
    for(i=0;i<queued;++i) {
        console_mouse_input mouse={0};
        mouse.width=640;mouse.height=400;
        mouse.action=CONSOLE_MOUSE_MOVE;
        mouse.dx=(int)i-500;mouse.dy=500-(int)i;mouse.buttons=(WORD)(i%4);
        if(!i) {mouse.action=CONSOLE_MOUSE_ENTER;mouse.dx=mouse.dy=mouse.buttons=0;}
        if(i==queued-1) {ZeroMemory(&mouse,sizeof(mouse));mouse.action=CONSOLE_MOUSE_LEAVE;}
        CHECK(console_mouse_input_valid(&mouse));
        queue[i].EventType=CONSOLE_INPUT_RELATIVE_MOUSE;
        memcpy(&queue[i].Event,&mouse,sizeof(mouse));
    }
    for(total=batches=0;total<queued;++batches) {
        CHECK(ntvdm_console_read_pc_input(input,out,NTVDM_PC_INPUT_RECORDS,&got,CONSOLE_READ_NOWAIT));
        CHECK(got==5);
        for(i=0;i<got;++i)CHECK(!memcmp(&out[i],&queue[total+i],sizeof(out[i])));
        total+=got;
    }
    CHECK(batches==200 && reads==200);
    printf("BATCH-RELATIVE 1000 records / 200 reads; signed motion, buttons, enter/leave PASS\n");
    reset(5);
    for(i=0;i<5;++i) {
        queue[i].EventType=KEY_EVENT;
        queue[i].Event.KeyEvent.bKeyDown=TRUE;
        queue[i].Event.KeyEvent.wRepeatCount=1;
        queue[i].Event.KeyEvent.uChar.UnicodeChar=L'a';
    }
    CHECK(!ntvdm_console_read_pc_input(input,out,NTVDM_PC_INPUT_RECORDS-1,&got,CONSOLE_READ_NOWAIT));
    CHECK(GetLastError()==ERROR_INVALID_PARAMETER && position==0);
    CHECK(ntvdm_console_read_pc_input(input,out,NTVDM_PC_INPUT_RECORDS,&got,CONSOLE_READ_NOWAIT));
    CHECK(got==10 && position==5 && reads==1);
    for(i=0;i<got;i+=2) {
        CHECK(out[i].Event.KeyEvent.bKeyDown && !out[i+1].Event.KeyEvent.bKeyDown);
        CHECK(out[i].Event.KeyEvent.wVirtualScanCode && out[i].Event.KeyEvent.wVirtualScanCode==out[i+1].Event.KeyEvent.wVirtualScanCode);
    }
    reset(5);
    queue[0].EventType=MOUSE_EVENT;queue[0].Event.MouseEvent.dwButtonState=1;
    queue[1].EventType=KEY_EVENT;queue[1].Event.KeyEvent.bKeyDown=TRUE;queue[1].Event.KeyEvent.uChar.UnicodeChar=L'a';
    queue[2].EventType=FOCUS_EVENT;queue[2].Event.FocusEvent.bSetFocus=FALSE;
    queue[3].EventType=MOUSE_EVENT;
    queue[4].EventType=KEY_EVENT;queue[4].Event.KeyEvent.wVirtualScanCode=0x1e;queue[4].Event.KeyEvent.wVirtualKeyCode='A';
    CHECK(ntvdm_console_read_pc_input(input,out,NTVDM_PC_INPUT_RECORDS,&got,CONSOLE_READ_NOWAIT));
    CHECK(got==6 && out[0].Event.MouseEvent.dwButtonState==1);
    CHECK(out[1].Event.KeyEvent.bKeyDown && !out[2].Event.KeyEvent.bKeyDown);
    CHECK(out[3].EventType==FOCUS_EVENT && out[4].EventType==MOUSE_EVENT && !out[4].Event.MouseEvent.dwButtonState);
    CHECK(out[5].EventType==KEY_EVENT &&
        !memcmp(&out[5].Event.KeyEvent,&queue[4].Event.KeyEvent,sizeof(KEY_EVENT_RECORD)));
    reset(5);
    queue[0].EventType=KEY_EVENT;queue[0].Event.KeyEvent.bKeyDown=TRUE;
    queue[0].Event.KeyEvent.wVirtualKeyCode=VK_RETURN;queue[0].Event.KeyEvent.dwControlKeyState=LEFT_ALT_PRESSED;
    queue[1]=queue[0];queue[1].Event.KeyEvent.bKeyDown=FALSE;queue[1].Event.KeyEvent.dwControlKeyState=0;
    for(i=2;i<5;++i) {queue[i].EventType=MOUSE_EVENT;queue[i].Event.MouseEvent.dwMousePosition.X=(SHORT)i;}
    CHECK(ReadConsoleInputExW(input,out,5,&got,CONSOLE_READ_NOWAIT) && got==3 && reads==1);
    for(i=0;i<got;++i) CHECK(out[i].Event.MouseEvent.dwMousePosition.X==(SHORT)(i+2));
    printf("BATCH-KEY expansion/mixed-order/capacity/shortcut PASS\n");
    return 0;
}
