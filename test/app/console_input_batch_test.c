/* Real production adapter, controlled input queue. No desktop or guest. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "conapi.h"
#include "common/protocol/console_mouse.h"
#include "ntvdm-exe/softpc/include/mvdm_keyboard_history.h"
static mvdm_keyboard_history nt_keyboard_history;
static INPUT_RECORD queue[1024];
static DWORD queued, position, reads, peeks;static BOOL use_receipts;
static uint64_t returned_receipts[100];static DWORD returned_count;static BOOL returned_tagged;
static uint64_t sent_live[100];static DWORD sent_live_count;
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
static BOOL batch_read_tagged(HANDLE h,PINPUT_RECORD out,uint64_t *receipts,DWORD capacity,LPDWORD count,BOOL peek,
    const uint64_t *live,DWORD live_count)
{
    DWORD first=position,i;
    if(live_count>100)return FALSE;
    sent_live_count=live_count;if(live_count)memcpy(sent_live,live,live_count*sizeof(*live));
    BOOL ok=peek ? batch_peek(h,out,capacity,count) : batch_read(h,out,capacity,count);
    if(ok && receipts)for(i=0;i<*count;++i)receipts[i]=use_receipts ? 1000u+first+i : 0;
    return ok;
}
static BOOL batch_prepend_tagged(HANDLE h,PINPUT_RECORD records,const uint64_t *receipts,DWORD count,LPDWORD written)
{
    (void)h;(void)records;if(count>100)return FALSE;
    if(receipts)memcpy(returned_receipts,receipts,count*sizeof(*receipts));
    returned_count=count;returned_tagged=receipts!=NULL;*written=count;return TRUE;
}
static BOOL batch_prepend(HANDLE h,PINPUT_RECORD records,DWORD count,LPDWORD written)
{ return batch_prepend_tagged(h,records,NULL,count,written); }
#undef ReadConsoleInputW
#undef PeekConsoleInputW
#define ReadConsoleInputW batch_read
#define PeekConsoleInputW batch_peek
#define ntvdm_console_read_input_tagged batch_read_tagged
#define ntvdm_console_prepend_keys_tagged batch_prepend_tagged
#define ntvdm_console_prepend_keys batch_prepend
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
    CHECK(ReadConsoleInputExW(input,out,5,&got,CONSOLE_READ_NOREMOVE) && got==1 && position==0);
    CHECK(!memcmp(&out[0],&queue[0],sizeof(out[0])));
    CHECK(ReadConsoleInputExW(input,out,5,&got,CONSOLE_READ_NOWAIT) && got==5 && reads==1);
    for(i=0;i<got;++i) CHECK(!memcmp(&out[i],&queue[i],sizeof(out[i])));
    reset(4);
    for(i=0;i<4;++i) {
        queue[i].EventType=KEY_EVENT;
        queue[i].Event.KeyEvent.bKeyDown=(i%2u)==0u;
        queue[i].Event.KeyEvent.wVirtualKeyCode=VK_RETURN;
        queue[i].Event.KeyEvent.dwControlKeyState=RIGHT_ALT_PRESSED |
            (i>=2u ? LEFT_CTRL_PRESSED : 0u);
    }
    CHECK(ReadConsoleInputExW(input,out,4,&got,CONSOLE_READ_NOREMOVE) && got==1 && position==0);
    CHECK(!memcmp(&out[0],&queue[0],sizeof(out[0])));
    CHECK(ReadConsoleInputExW(input,out,4,&got,CONSOLE_READ_NOWAIT) && got==4 && reads==1);
    for(i=0;i<got;++i) CHECK(!memcmp(&out[i],&queue[i],sizeof(out[i])));
    reset(4);
    for(i=0;i<4;++i) {
        queue[i].EventType=KEY_EVENT;queue[i].Event.KeyEvent.wRepeatCount=1;
        queue[i].Event.KeyEvent.bKeyDown=i<2;
        queue[i].Event.KeyEvent.wVirtualKeyCode=(i==0 || i==3) ? VK_MENU : VK_RETURN;
        queue[i].Event.KeyEvent.wVirtualScanCode=(i==0 || i==3) ? 0x38 : 0x1c;
        queue[i].Event.KeyEvent.dwControlKeyState=i==3 ? 0 : LEFT_ALT_PRESSED;
    }
    CHECK(ntvdm_console_read_pc_input(input,out,NTVDM_PC_INPUT_RECORDS,&got,CONSOLE_READ_NOWAIT));
    CHECK(got==4 && !memcmp(out,queue,4*sizeof(*out)));
    /* Character-bearing scanless records must retain text semantics even
     * when Console/RDP supplies an ordinary virtual key instead of VK_PACKET. */
    {
        HKL prior=GetKeyboardLayout(0),us=LoadKeyboardLayoutW(L"00000409",0);
        const WCHAR chars[]=L":;!?_+Aa";
        CHECK(us && ActivateKeyboardLayout(us,0));
        for(i=0;chars[i];++i) {
            SHORT mapping=VkKeyScanExW(chars[i],us);
            DWORD j,character_down=0;
            reset(1);queue[0].EventType=KEY_EVENT;
            queue[0].Event.KeyEvent.bKeyDown=TRUE;
            queue[0].Event.KeyEvent.wRepeatCount=1;
            queue[0].Event.KeyEvent.wVirtualKeyCode=(WORD)(mapping&255);queue[0].Event.KeyEvent.wRepeatCount=3;
            queue[0].Event.KeyEvent.uChar.UnicodeChar=chars[i];
            CHECK(ntvdm_console_read_pc_input(input,out,NTVDM_PC_INPUT_RECORDS,&got,CONSOLE_READ_NOWAIT));
            for(j=0;j<got;++j)if(out[j].Event.KeyEvent.bKeyDown &&
                out[j].Event.KeyEvent.wVirtualKeyCode==(WORD)(mapping&255)) {
                CHECK(!!(out[j].Event.KeyEvent.dwControlKeyState&SHIFT_PRESSED)==!!(mapping&0x100));
                CHECK(out[j].Event.KeyEvent.uChar.UnicodeChar==chars[i]);CHECK(out[j].Event.KeyEvent.wRepeatCount==3);
                ++character_down;
            }
            CHECK(character_down==1);
            queue[0].Event.KeyEvent.bKeyDown=FALSE;position=0;
            CHECK(ntvdm_console_read_pc_input(input,out,NTVDM_PC_INPUT_RECORDS,&got,CONSOLE_READ_NOWAIT));
            CHECK(got==0);
        }
        const DWORD modifiers[]={LEFT_CTRL_PRESSED,RIGHT_CTRL_PRESSED,LEFT_ALT_PRESSED,RIGHT_ALT_PRESSED};
        for(DWORD m=0;m<ARRAYSIZE(modifiers);++m) {
            DWORD modifier=modifiers[m];
            KEY_EVENT_RECORD key={TRUE,1,'X',0,L'x',modifier};
            CHECK(nt_rdp_normalize_key(&key,out,ARRAYSIZE(out))==1);
            CHECK(out[0].Event.KeyEvent.wVirtualKeyCode=='X' &&
                out[0].Event.KeyEvent.dwControlKeyState==modifier &&
                out[0].Event.KeyEvent.wVirtualScanCode!=0);
        }
        CHECK(ActivateKeyboardLayout(prior,0));
    }
    printf("BATCH-SCANLESS-TEXT punctuation/case/modifiers/key-up PASS\n");
    printf("BATCH-KEY expansion/mixed-order/capacity/Alt-Enter PASS\n");
    memset(&nt_keyboard_history,0,sizeof(nt_keyboard_history));reset(2);use_receipts=TRUE;
    queue[0].EventType=KEY_EVENT;queue[0].Event.KeyEvent.bKeyDown=TRUE;
    queue[0].Event.KeyEvent.wRepeatCount=1;queue[0].Event.KeyEvent.wVirtualKeyCode='A';
    queue[0].Event.KeyEvent.wVirtualScanCode=0x1e;queue[1]=queue[0];
    CHECK(ntvdm_console_read_pc_input(input,out,NTVDM_PC_INPUT_RECORDS,&got,CONSOLE_READ_NOWAIT));
    CHECK(got==2 && nt_keyboard_history.receipt_batch_count==2);
    CHECK(mvdm_keyboard_history_record(&nt_keyboard_history));
    CHECK(mvdm_keyboard_history_record(&nt_keyboard_history));
    mvdm_keyboard_history_select_begin(&nt_keyboard_history);
    mvdm_keyboard_history_select(&nt_keyboard_history,1);mvdm_keyboard_history_select(&nt_keyboard_history,2);
    CHECK(mvdm_keyboard_history_receipt(&nt_keyboard_history,1)==1001 &&
        mvdm_keyboard_history_receipt(&nt_keyboard_history,2)==1000);
    CHECK(WriteConsoleInputVDMW(input,out,2,&got) && got==2 && returned_tagged &&
        returned_count==2 && returned_receipts[0]==1000 && returned_receipts[1]==1001);
    /* A shortened original return contains the newest selected entries,
     * still ordered oldest-first in the emitted subset. */
    CHECK(mvdm_keyboard_history_record_receipt(&nt_keyboard_history,1002));
    mvdm_keyboard_history_select_begin(&nt_keyboard_history);
    mvdm_keyboard_history_select(&nt_keyboard_history,1);mvdm_keyboard_history_select(&nt_keyboard_history,2);
    mvdm_keyboard_history_select(&nt_keyboard_history,3);
    CHECK(WriteConsoleInputVDMW(input,out,2,&got) && got==2 && returned_tagged &&
        returned_receipts[0]==1001 && returned_receipts[1]==1002);
    memset(&nt_keyboard_history,0,sizeof(nt_keyboard_history));returned_tagged=TRUE;
    CHECK(WriteConsoleInputVDMW(input,out,2,&got) && got==2 && !returned_tagged);
    CHECK(mvdm_keyboard_history_record_receipt(&nt_keyboard_history,3001));
    CHECK(mvdm_keyboard_history_record_receipt(&nt_keyboard_history,0));
    CHECK(mvdm_keyboard_history_record_receipt(&nt_keyboard_history,3003));
    reset(1);queue[0].EventType=MOUSE_EVENT;use_receipts=FALSE;
    CHECK(ntvdm_console_read_pc_input(input,out,NTVDM_PC_INPUT_RECORDS,&got,CONSOLE_READ_NOWAIT));
    CHECK(got==1 && sent_live_count==2 && sent_live[0]==3001 && sent_live[1]==3003);
    printf("BATCH-RECEIPT identical keys remain distinct through normalization and original history PASS\n");
    return 0;
}
