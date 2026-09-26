/* Compile the exact selected original function body. Only its external
 * history/Console/cleanup dependencies are controlled; no guest is patched. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#define MAX_KEY_EVENTS 100
static KEY_EVENT_RECORD history[MAX_KEY_EVENTS];
static int available,cleared_history,cleared_queue;
static DWORD sent,calls;
static INPUT_RECORD captured[MAX_KEY_EVENTS];
static struct { HANDLE InputHandle; } sc;
static int GetHistoryKeyEvent(PKEY_EVENT_RECORD record,int number)
{
    if (number>available) return FALSE;
    *record=history[available-number];return TRUE;
}
static BOOL WriteConsoleInputVDMW(HANDLE handle,PINPUT_RECORD records,DWORD count,LPDWORD written)
{
    (void)handle;
    ++calls;sent=count;*written=0;
    /* Reject before reading a caller's uninitialized/out-of-range tail. */
    if (count>(DWORD)available || count>MAX_KEY_EVENTS) return FALSE;
    memcpy(captured,records,count*sizeof(*records));*written=count;return TRUE;
}
static void InitKeyHistory(void) { ++cleared_history; }
static void InitQueue(void) { ++cleared_queue; }
#define always_trace0(message) ((void)0)
#include "unused_key_return.inc"
static int test(int requested,int present)
{
    int index,expected=min(requested,present);
    available=present;sent=calls=0;cleared_history=cleared_queue=0;
    memset(captured,0,sizeof(captured));
    for(index=0;index<present;++index) {
        memset(&history[index],0,sizeof(history[index]));
        history[index].wVirtualKeyCode=(WORD)(index+1);
        history[index].wRepeatCount=1;
    }
    ReturnUnusedKeyEvents(requested);
    if(sent!=(DWORD)expected || calls!=(DWORD)(expected!=0) ||
        cleared_history!=1 || cleared_queue!=1) {
        printf("FAIL requested=%d present=%d sent=%lu calls=%lu cleanup=%d/%d\n",
            requested,present,sent,calls,cleared_history,cleared_queue);return 1;
    }
    for(index=0;index<expected;++index)
        if(captured[index].EventType!=KEY_EVENT ||
            captured[index].Event.KeyEvent.wVirtualKeyCode!=present-expected+index+1) {
            printf("FAIL order/type requested=%d present=%d index=%d\n",requested,present,index);return 1;
        }
    printf("PASS requested=%d present=%d count/order/type/cleanup\n",requested,present);
    return 0;
}
int main(void)
{
    return test(2,2) || test(1,3) || test(0,0) || test(3,2) ||
        test(2,0) || test(100,100) || test(101,100);
}
