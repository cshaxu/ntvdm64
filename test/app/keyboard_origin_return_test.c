/* Actual selected history/count/return functions with controlled pending
 * device origins and Console sink. This is not scan-translation or guest
 * execution proof; those metadata transitions have a separate gate. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ntvdm-exe/softpc/include/mvdm_keyboard_history.h"
#define MAX_KEY_EVENTS 100
#define GLOBAL
#define BUFF_6805_PMAX 512
#define BUFF_6805_PMASK 511
#define ONECHARCODEMASK 0x80
#define EHS_FUNC_FAILED 1
#define CHECK(x) do { if(!(x)){printf("FAIL line=%u: %s\n",(unsigned)__LINE__,#x);exit(1);} } while(0)
static KEY_EVENT_RECORD history_store[MAX_KEY_EVENTS];
static PKEY_EVENT_RECORD key_history=history_store,key_history_head,key_history_tail;
static int key_history_count;
static mvdm_keyboard_history nt_keyboard_history;
static INPUT_RECORD first_returned_key,captured[MAX_KEY_EVENTS];
static struct { HANDLE InputHandle; } sc;
static struct { int KeyCount; } KeyQueue;
static unsigned char key_marker_buffer[BUFF_6805_PMAX];
static int buff_6805_out_ptr,buff_6805_in_ptr,held_event_count,scanning_discontinued;
static int output_full,pending_8042,waiting_for_upcode,resets,queue_resets;
static DWORD sent;static BOOL fail_write;
static void DisplayErrorTerm(int kind,DWORD error,const char *file,int line)
{ (void)kind;printf("unexpected error=%lu %s:%d\n",error,file,line);exit(1); }
static void InitQueue(void) { KeyQueue.KeyCount=0;++queue_resets; }
static BOOL WriteConsoleInputVDMW(HANDLE handle,PINPUT_RECORD records,DWORD count,LPDWORD written)
{
    (void)handle;*written=0;
    if(fail_write)return FALSE;
    CHECK(count<=MAX_KEY_EVENTS);memcpy(captured,records,count*sizeof(*records));
    sent=count;*written=count;return TRUE;
}
#define always_trace0(message) ((void)0)
static void Reset6805and8042(void)
{
    ++resets;mvdm_keyboard_history_clear_device(&nt_keyboard_history);
    buff_6805_out_ptr=buff_6805_in_ptr=held_event_count=0;
    output_full=pending_8042=waiting_for_upcode=scanning_discontinued=0;
    memset(key_marker_buffer,0,sizeof(key_marker_buffer));
}
#include "keyboard_buffer_count.inc"
#include "keyboard_origin_pending.inc"
#include "keyboard_origin_init.inc"
#include "keyboard_origin_update.inc"
#include "keyboard_origin_get.inc"
#include "keyboard_origin_calculate.inc"
#include "unused_key_return.inc"
static void fresh(void)
{
    Reset6805and8042();InitKeyHistory();resets=queue_resets=0;sent=0;fail_write=FALSE;
    memset(captured,0,sizeof(captured));first_returned_key.EventType=0;
}
static mvdm_key_origin raw(WORD vk,BOOL down)
{
    INPUT_RECORD record={0};mvdm_key_origin origin;
    record.EventType=KEY_EVENT;record.Event.KeyEvent.wVirtualKeyCode=vk;
    record.Event.KeyEvent.bKeyDown=down;record.Event.KeyEvent.wRepeatCount=1;
    record.Event.KeyEvent.wVirtualScanCode=vk=='M' ? 0x32 : 0x12;
    update_key_history(&record,1);
    CHECK(mvdm_keyboard_history_begin(&nt_keyboard_history));origin=nt_keyboard_history.active;
    mvdm_keyboard_history_end(&nt_keyboard_history);return origin;
}
static void slot(mvdm_key_origin origin)
{
    nt_keyboard_history.device[buff_6805_in_ptr]=origin;
    key_marker_buffer[buff_6805_in_ptr]=origin ? 0x81 : 0;
    buff_6805_in_ptr=(buff_6805_in_ptr+1)&BUFF_6805_PMASK;
}
static void returned(unsigned count,WORD first,BOOL down)
{
    int pending=CalcNumberOfUnusedKeyEvents();
    CHECK(pending==(int)count && resets==1);
    ReturnUnusedKeyEvents(pending);
    CHECK(sent==(fail_write ? 0 : count) && queue_resets==1 && !key_history_count);
    CHECK(!nt_keyboard_history.issued && !nt_keyboard_history.output);
    if(count && !fail_write)CHECK(captured[0].Event.KeyEvent.wVirtualKeyCode==first &&
        captured[0].Event.KeyEvent.bKeyDown==down);
}
int main(void)
{
    mvdm_key_origin make,release;unsigned i;
    fresh();make=raw('M',TRUE);raw('E',FALSE);slot(make);returned(1,'M',TRUE);
    puts("PASS ignored release after unread make does not displace it");
    fresh();raw('E',FALSE);make=raw('M',TRUE);slot(make);returned(1,'M',TRUE);
    puts("PASS ignored release before unread make retains original order");
    for(i=0;i<2;++i) {
        WORD special=i ? VK_SNAPSHOT : VK_CANCEL;
        fresh();raw('M',TRUE);release=raw(special,FALSE);slot(release);slot(release);
        returned(1,special,FALSE);
    }
    puts("PASS Ctrl-Break/PrintScreen expansion returns its one raw record, never consumed M");
    fresh();raw('M',TRUE);slot(0);returned(0,0,FALSE);
    fresh();raw('M',TRUE);output_full=1;returned(0,0,FALSE);
    puts("PASS synthetic data and unattributed output do not replay consumed M");
    fresh();
    /* Recorded 29-entry history shape: M down is origin 11; duplicate
     * release 13 produced no device data. Select the 18 pending origins. */
    for(i=1;i<=29;++i) {
        mvdm_key_origin origin=raw(i==11 ? 'M':'E',i==11);
        if(i>=11 && i!=13)slot(origin);
    }
    returned(18,'M',TRUE);
    puts("PASS captured-history shape preserves first M despite ignored duplicate release");
    fresh();buff_6805_out_ptr=buff_6805_in_ptr=511;
    make=raw('M',TRUE);release=raw('M',FALSE);slot(make);slot(release);
    returned(2,'M',TRUE);CHECK(!captured[1].Event.KeyEvent.bKeyDown);
    puts("PASS wrapped device metadata retains paired original order");
    fresh();raw('M',TRUE);make=raw('E',TRUE);
    nt_keyboard_history.held[0]=make;held_event_count=1;scanning_discontinued=1;
    nt_keyboard_history.output=nt_keyboard_history.prefix=make;output_full=waiting_for_upcode=1;
    returned(1,'E',TRUE);
    puts("PASS held/output/prefix duplicate identity survives hardware reset once");
    fresh();
    for(i=1;i<=203;++i) {
        mvdm_key_origin origin=raw((WORD)i,TRUE);
        if(i==3 || i==103 || i==104 || i==203)slot(origin);
    }
    returned(2,104,TRUE);CHECK(captured[1].Event.KeyEvent.wVirtualKeyCode==203);
    puts("PASS original history ring wrap rejects overwritten identities without aliasing");
    fresh();make=raw('M',TRUE);slot(make);fail_write=TRUE;returned(1,'M',TRUE);
    CHECK(!first_returned_key.EventType);
    puts("PASS failed Console prepend keeps original cleanup and no fabricated BIOS join");
    return 0;
}
