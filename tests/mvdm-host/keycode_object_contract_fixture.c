/* Actual original nt_input.obj/nt_keycd.obj. Host LED synchronization is a
 * recording boundary; BIOS conversion uses real public OEM/layout services.
 * No keyboard layout, actual input queue or physical LEDs are changed. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "insignia.h"
#include "keyboard.h"
#ifndef MVDM_KEYCODE_EXPECT_EVENT_BYTES
#define MVDM_KEYCODE_EXPECT_EVENT_BYTES 16
#endif
typedef char key_event_abi[(sizeof(KEY_EVENT_RECORD)==MVDM_KEYCODE_EXPECT_EVENT_BYTES && sizeof(KEYBDFUNCS)==24 && sizeof(void *)==4) ? 1 : -1];
extern BYTE Scan1ToKeynum[133],Scan1ToKeynumExtended[84];
extern BYTE KeyMsgToKeyCode(PKEY_EVENT_RECORD);
extern BOOL BiosKeyToInputRecord(PKEY_EVENT_RECORD);
extern KEYBDFUNCS nt_keybd_funcs;
extern void nt_kb_prepare(void),nt_kb_light_on(unsigned char);
DWORD ToggleKeyState;
static unsigned checks,failures,callbacks;
static WORD observed_key;
static DWORD observed_state;
static unsigned calls;
#define CHECK(x) do {++checks;if(!(x)){++failures;fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);}}while(0)
void SyncToggleKeys(WORD key,DWORD state)
{++callbacks;++calls;observed_key=key;observed_state=state;}
static BYTE translate(unsigned scan,DWORD flags)
{KEY_EVENT_RECORD k;memset(&k,0,sizeof(k));k.wVirtualScanCode=(WORD)scan;k.dwControlKeyState=flags;return KeyMsgToKeyCode(&k);}
static int boundary(void)
{
    BYTE *tables[]={Scan1ToKeynum,Scan1ToKeynumExtended};
    unsigned sizes[]={133,84},i;
    unsigned rejected=0;
    for(i=0;i<2;++i){
        BYTE saved=0,sentinel=0x7b,actual,before;
        BYTE *address=tables[i]+sizes[i];
        SIZE_T done=0;
        /* Fault seam in this fixture's writable PE padding/adjacent data,
         * not an original guest or production process. Always restore it. */
        if(!ReadProcessMemory(GetCurrentProcess(),address,&saved,1,&done)||done!=1)return 2;
        before=translate(sizes[i],i?ENHANCED_KEY:0);
        if(!WriteProcessMemory(GetCurrentProcess(),address,&sentinel,1,&done)||done!=1)return 2;
        actual=translate(sizes[i],i?ENHANCED_KEY:0);
        if(!WriteProcessMemory(GetCurrentProcess(),address,&saved,1,&done)||done!=1)return 2;
        printf("KEYCODE_BOUNDARY enhanced=%u scan=%u before=%u sentinel=%u observed=%u restored=%u\n",i,sizes[i],before,sentinel,actual,translate(sizes[i],i?ENHANCED_KEY:0));
        rejected+=(actual==0);
    }
    printf("KEYCODE_BOUNDARY expected_rejected=2 actual_rejected=%u\n",rejected);
    return rejected==2?0:1;
}
int main(int argc,char **argv)
{
    unsigned enhanced,scan,state,pattern;
    const DWORD toggles[]={SCROLLLOCK_ON,NUMLOCK_ON,CAPSLOCK_ON};
    const unsigned lights[]={1,2,4};
    (void)argv;
    if(argc>1)return boundary();
    for(enhanced=0;enhanced<2;++enhanced){
        unsigned size=enhanced?84:133;
        BYTE *table=enhanced?Scan1ToKeynumExtended:Scan1ToKeynum;
        for(scan=0;scan<65536;++scan){
            KEY_EVENT_RECORD k,saved;
            BYTE actual,expected;
            if(scan==size)continue; /* separate strict failure witness */
            memset(&k,0xa5,sizeof(k));k.wVirtualScanCode=(WORD)scan;
            k.dwControlKeyState=enhanced?ENHANCED_KEY|CAPSLOCK_ON:CAPSLOCK_ON;
            saved=k;expected=scan<size?table[scan]:0;
            actual=KeyMsgToKeyCode(&k);
            CHECK(actual==expected);CHECK(memcmp(&k,&saved,sizeof(k))==0);
        }
    }
    CHECK(translate(0x1c,0)==43 && translate(0x1c,ENHANCED_KEY)==108);
    CHECK(translate(0x1d,0)==58 && translate(0x1d,ENHANCED_KEY)==64);
    CHECK(translate(0x3b,0)==112 && translate(0x47,ENHANCED_KEY)==80);
    CHECK(nt_keybd_funcs.kb_prepare==nt_kb_prepare && nt_keybd_funcs.kb_restore==nt_kb_prepare);
    CHECK(nt_keybd_funcs.kb_init==nt_kb_prepare && nt_keybd_funcs.kb_shutdown==nt_kb_prepare);
    CHECK(nt_keybd_funcs.kb_light_on==nt_kb_light_on && nt_keybd_funcs.kb_light_off==nt_kb_light_on);
    nt_keybd_funcs.kb_prepare();nt_keybd_funcs.kb_restore();nt_keybd_funcs.kb_init();nt_keybd_funcs.kb_shutdown();
    for(state=0;state<8;++state)for(pattern=0;pattern<256;++pattern){
        DWORD initial=LEFT_ALT_PRESSED|SHIFT_PRESSED,expected;unsigned j;
        for(j=0;j<3;++j)if(state&(1u<<j))initial|=toggles[j];
        expected=initial;
        for(j=0;j<3;++j)if((pattern>>4)&lights[j]){
            expected&=~toggles[j];if(pattern&lights[j])expected|=toggles[j];
        }
        ToggleKeyState=initial;calls=0;observed_key=0xffff;observed_state=0;
        nt_kb_light_on((BYTE)pattern);
        CHECK(calls==(unsigned)(initial!=expected));
        CHECK(initial==expected || (observed_key==0 && observed_state==expected));
        CHECK(ToggleKeyState==initial); /* state authority remains the callback owner */
    }
    {
        static const struct {BYTE ascii;WORD scan,out;DWORD flags;} cases[]={
            {0,0x54,0x3b,SHIFT_PRESSED},{0xf0,0x54,0x3b,SHIFT_PRESSED},
            {0,0x5e,0x3b,LEFT_CTRL_PRESSED},{0,0x68,0x3b,LEFT_ALT_PRESSED},
            {0xe0,0x73,0x4b,ENHANCED_KEY|LEFT_CTRL_PRESSED},
            {0x2f,0xe0,0x35,ENHANCED_KEY},{0x0d,0xe0,0x1c,ENHANCED_KEY},
            {0x0a,0xe0,0x1c,ENHANCED_KEY|LEFT_CTRL_PRESSED}};
        unsigned i;
        for(i=0;i<sizeof(cases)/sizeof(cases[0]);++i){
            struct{DWORD before;KEY_EVENT_RECORD k;DWORD after;} guarded;
            memset(&guarded,0,sizeof(guarded));guarded.before=0x12345678;guarded.after=0x87654321;
            guarded.k.bKeyDown=TRUE;guarded.k.uChar.AsciiChar=(CHAR)cases[i].ascii;guarded.k.wVirtualScanCode=cases[i].scan;
            CHECK(BiosKeyToInputRecord(&guarded.k));
            CHECK(guarded.k.wVirtualScanCode==cases[i].out && guarded.k.dwControlKeyState==cases[i].flags);
            CHECK(guarded.k.bKeyDown && guarded.k.wRepeatCount==1 && guarded.k.wVirtualKeyCode!=0);
            CHECK(guarded.before==0x12345678 && guarded.after==0x87654321);
        }
    }
    printf("KEYCODE_OBJECT_CONTRACT checks=%u failures=%u callbacks=%u\n",checks,failures,callbacks);
    return failures?1:0;
}
