/* Extracted original return algorithms with controlled BIOS memory, history
 * and Console prepend. Scan/character translation is a fixture dependency;
 * real guest integration remains a separate gate. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
typedef unsigned short word;
#define VOID void
#define MAX_KEY_EVENTS 100
#define NUMBBIRECS 32
#define BIOS_KB_BUFFER_HEAD 0
#define BIOS_KB_BUFFER_TAIL 2
#define BIOS_KB_BUFFER_START 4
#define BIOS_KB_BUFFER_END 6
#define BIOS_VAR_START 0x400
static word bios[1024];
static KEY_EVENT_RECORD history[MAX_KEY_EVENTS];
static INPUT_RECORD first_returned_key,output[256];
static struct { HANDLE InputHandle; } sc;
static DWORD output_count;
static int available,fail_first,calls;
static DWORD translated_flags;
static UCHAR aNumPadSCode[10]; /* Scanless conversion is not selected here. */
static void sas_loadw(unsigned address,word *value) { *value=bios[address/2]; }
static void sas_storew(unsigned address,word value) { bios[address/2]=value; }
static int GetHistoryKeyEvent(PKEY_EVENT_RECORD key,int number)
{
    if(number>available)return FALSE;
    *key=history[available-number];return TRUE;
}
static void InitKeyHistory(void) { available=0; }
static void InitQueue(void) { }
#define always_trace0(message) ((void)0)
static BOOL WriteConsoleInputVDMW(HANDLE input,PINPUT_RECORD records,DWORD count,LPDWORD written)
{
    (void)input;*written=0;
    if(++calls==1 && fail_first)return fail_first==2;
    if(count+output_count>ARRAYSIZE(output))return FALSE;
    memmove(output+count,output,output_count*sizeof(*output));
    memcpy(output,records,count*sizeof(*output));output_count+=count;*written=count;
    return TRUE;
}
static BOOL BiosKeyToInputRecord(PKEY_EVENT_RECORD key)
{
    UCHAR character=(UCHAR)key->uChar.AsciiChar;
    if(character=='?')return FALSE;
    key->wVirtualKeyCode=(WORD)(character-'a'+'A');
    key->uChar.UnicodeChar=character;key->wRepeatCount=1;
    key->dwControlKeyState=translated_flags;return TRUE;
}
#include "unused_key_return.inc"
#include "keyboard_bios_return.inc"
static int check(const char *name,int hardware,WORD vk,WORD scan,DWORD flags,
                 int down,int fail,int wrap,int two,int skip_last,DWORD expected)
{
    word start=0x20,end=two>1 ? 0x80:0x40,head=wrap ? 0x3e:0x20,tail=head;
    DWORD i,makes=0,releases=0,entries=two>1 ? two:(two ? 2:1);int failed;
    memset(bios,0,sizeof(bios));memset(history,0,sizeof(history));
    memset(output,0,sizeof(output));output_count=0;calls=0;fail_first=fail;
    translated_flags=0;available=hardware;
    history[0].wVirtualKeyCode=vk;history[0].wVirtualScanCode=scan;
    history[0].dwControlKeyState=flags;history[0].bKeyDown=down;
    history[0].wRepeatCount=1;
    sas_storew(BIOS_KB_BUFFER_START,start);sas_storew(BIOS_KB_BUFFER_END,end);
    sas_storew(BIOS_KB_BUFFER_HEAD,head);
    for(i=0;i<entries;++i) {
        sas_storew(BIOS_VAR_START+tail,(word)(0x1200 | (skip_last && i==1 ? '?':'e')));
        tail+=2;if(tail==end)tail=start;
    }
    sas_storew(BIOS_KB_BUFFER_TAIL,tail);
    ReturnUnusedKeyEvents(hardware);ReturnBiosBufferKeys();
    for(i=0;i<output_count;++i)if(output[i].Event.KeyEvent.wVirtualKeyCode=='E') {
        if(output[i].Event.KeyEvent.bKeyDown)++makes;else ++releases;
    }
    failed=output_count!=expected || bios[BIOS_KB_BUFFER_TAIL/2]!=head;
    if(hardware && !fail && output_count) {
        const KEY_EVENT_RECORD *last=&output[output_count-1].Event.KeyEvent;
        failed|=last->wVirtualKeyCode!=vk || last->wVirtualScanCode!=scan ||
            last->dwControlKeyState!=flags || last->bKeyDown!=down;
    }
    if(hardware && vk=='E' && scan==0x12 && !flags && !down && !fail && !skip_last)
        failed|=makes!=releases || makes!=entries;
    printf("%s %s records=%lu expected=%lu E-make/up=%lu/%lu drained=%d\n",
        failed ? "FAIL":"PASS",name,output_count,expected,makes,releases,
        bios[BIOS_KB_BUFFER_TAIL/2]==head);
    return failed;
}
int main(void)
{
    int failures=check("matching-release",1,'E',0x12,0,0,0,0,0,0,2);
    failures+=check("no-hardware",0,0,0,0,0,0,0,0,0,2);
    failures+=check("different-key",1,'M',0x32,0,0,0,0,0,0,3);
    failures+=check("different-scan",1,'E',0x20,0,0,0,0,0,0,3);
    failures+=check("different-extended",1,'E',0x12,ENHANCED_KEY,0,0,0,0,0,3);
    failures+=check("hardware-make",1,'E',0x12,0,1,0,0,0,0,3);
    failures+=check("failed-prepend",1,'E',0x12,0,0,1,0,0,0,2);
    failures+=check("zero-written-prepend",1,'E',0x12,0,0,2,0,0,0,2);
    failures+=check("physical-release-modifiers",1,'E',0x12,SHIFT_PRESSED,0,0,0,0,0,2);
    failures+=check("wrapped-bios",1,'E',0x12,0,0,0,1,0,0,2);
    failures+=check("only-newest-overlaps",1,'E',0x12,0,0,0,0,1,0,4);
    failures+=check("odd-batch-flush",1,'E',0x12,0,0,0,0,20,0,40);
    failures+=check("untranslated-newest",1,'E',0x12,0,0,0,0,1,1,3);
    /* This also follows a matched call: stale boundary state must not survive. */
    failures+=check("subsequent-empty-hardware",0,0,0,0,0,0,0,0,0,2);
    return failures ? 1:0;
}
