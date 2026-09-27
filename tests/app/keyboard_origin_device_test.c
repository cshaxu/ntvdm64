/* Extracted production slot/translation/held/EOI paths. Scan generation,
 * IRQ delivery and guest BIOS are controlled dependencies, not guest proof. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ntvdm-exe/softpc/include/mvdm_keyboard_history.h"
#define NTVDM
#define GLOBAL
#define LOCAL static
#define VOID void
#define IFN0() (void)
#define IFN1(t,n) (t n)
#define IFN2(t,n,u,m) (t n,u m)
#define TRUE 1
#define FALSE 0
#define BUFF_6805_PMAX 512
#define BUFF_6805_PMASK 511
#define BUFF_6805_VMAX 511
#define ONECHARCODEMASK 0x80
#define IMMEDIATE_OUTPUT 1
#define HELD_EVENT_MAX 16
#define KEY_DOWN_EVENT 2
#define KEY_UP_EVENT 1
#define always_trace0(x) ((void)0)
#define sure_note_trace1(a,b,c) ((void)0)
typedef unsigned char half_word;
typedef uint16_t word;
static mvdm_keyboard_history nt_keyboard_history;
static half_word buff_6805[512],trans_8042[256];
static unsigned char key_marker_buffer[512],key_marker_value;
static int buff_6805_out_ptr,buff_6805_in_ptr,free_6805_buff_size=511;
static unsigned long KbdHdwFull;
static int held_event_count,held_event_type[16],held_event_key[16],key_down_count[127];
static int scanning_discontinued,waiting_for_next_code,keyboard_disabled,LastKeyDown=-1;
static int translating=1,waiting_for_upcode,output_full,pending_8042,pending_8042_value;
static char output_contents;static int KbdData,keyboard_interface_disabled;
static int bKbdEoiPending,bBiosOwnsKbdHdw,bKbdIntHooked,bBiosBufferSpace,bPifFastPaste,bForceDelayInts;
static word KbdInt09Off,KbdInt09Seg;static unsigned char guest[1024],*Start_of_M_area=guest;
static int interrupts;
static int current_light_pattern,set_3_key_state[127],set_3_default_key_state[127];
static int waiting_for_next_8042_code,shift_on,l_shift_on,r_shift_on,ctrl_on,l_ctrl_on,r_ctrl_on;
static int alt_on,l_alt_on,r_alt_on,kbd_status,cmd_byte_8042,op_port_remembered_bits;
static int in_anomalous_state,int_enabled,num_lock_on;
#define CHECK(x) do { if(!(x)){printf("FAIL line=%u: %s\n",(unsigned)__LINE__,#x);exit(1);} } while(0)
static void calc_buff_6805_left(void) { free_6805_buff_size=511-((buff_6805_in_ptr-buff_6805_out_ptr)&511); }
static void KbdIntDelay(void) { ++interrupts; }
static int WaitKbdHdw(unsigned long timeout) { (void)timeout;return 0; }
static void HostReleaseKbd(void) { }
static int bios_buffer_size(void) { return 0; }
#include "keyboard_device_add.inc"
#include "keyboard_device_remove.inc"
#include "keyboard_device_clear.inc"
#include "keyboard_device_reset.inc"
#include "keyboard_device_mark.inc"
static void do_host_key_down(int key)
{
    mark_key_codes_6805_buff(1);add_to_6805_buff((half_word)key,0);mark_key_codes_6805_buff(0);
}
static void do_host_key_up(int key)
{
    key_down_count[key]=0;mark_key_codes_6805_buff(1);
    add_to_6805_buff(0xf0,0);add_to_6805_buff((half_word)key,0);mark_key_codes_6805_buff(0);
}
#include "keyboard_host_down.inc"
#include "keyboard_host_up.inc"
#include "keyboard_device_immediate.inc"
#include "keyboard_device_translate.inc"
#include "keyboard_device_output.inc"
#include "keyboard_device_continue.inc"
#include "keyboard_device_eoi.inc"
#include "keyboard_origin_pending.inc"
static void replay(void) { int i;
#include "keyboard_device_replay.inc"
}
static void begin(void) { CHECK(mvdm_keyboard_history_record(&nt_keyboard_history));CHECK(mvdm_keyboard_history_begin(&nt_keyboard_history)); }
static void end(void) { mvdm_keyboard_history_end(&nt_keyboard_history); }
int main(void)
{
    unsigned i;
    for(i=0;i<256;++i)trans_8042[i]=(half_word)i;
    begin();host_key_down(0x32);end();
    begin();host_key_up(0x12);end(); /* No corresponding E make. */
    CHECK(PendingKeyboardHistory()==1 && mvdm_keyboard_history_age(&nt_keyboard_history,1)==2);
    continue_output();
    CHECK(output_full && output_contents==0x32 && nt_keyboard_history.output==1);
    CHECK(!nt_keyboard_history.device[0] && PendingKeyboardHistory()==1);
    bKbdEoiPending=1;KbdEOIHook(1,0);
    CHECK(!output_full && !nt_keyboard_history.output && !PendingKeyboardHistory());
    puts("PASS actual enqueue/remove/output/EOI preserves then retires origin; ignored release has none");
    buff_6805_out_ptr=buff_6805_in_ptr=511;calc_buff_6805_left();
    begin();host_key_up(0x32);end();
    CHECK(nt_keyboard_history.device[511]==3 && nt_keyboard_history.device[0]==3);
    continue_output();
    CHECK((unsigned char)output_contents==0xb2 && nt_keyboard_history.output==3);
    CHECK(!nt_keyboard_history.device[511] && !nt_keyboard_history.device[0]);
    CHECK(PendingKeyboardHistory()==1);
    bKbdEoiPending=1;KbdEOIHook(1,0);CHECK(!PendingKeyboardHistory());
    puts("PASS actual wrapped two-byte translation preserves one raw release");
    scanning_discontinued=1;begin();host_key_up(0x12);end();
    CHECK(held_event_count==1 && nt_keyboard_history.held[0]==4);
    CHECK(PendingKeyboardHistory()==1);replay();
    CHECK(!scanning_discontinued && !nt_keyboard_history.active);
    CHECK(PendingKeyboardHistory()==1);continue_output();
    CHECK(nt_keyboard_history.output==4);bKbdEoiPending=1;KbdEOIHook(1,0);
    CHECK(!PendingKeyboardHistory()); /* Stale held storage is not pending. */
    puts("PASS actual held admission/replay transfers identity once and ignores inactive held storage");
    begin();add_to_6805_buff(0xf0,0);end();continue_output();
    CHECK(waiting_for_upcode && nt_keyboard_history.prefix==5 && !output_full);
    CHECK(PendingKeyboardHistory()==1);
    nt_keyboard_history.active=5;add_to_6805_buff(0x32,0);end();continue_output();
    CHECK(!waiting_for_upcode && nt_keyboard_history.output==5);
    bKbdEoiPending=1;KbdEOIHook(1,0);CHECK(!PendingKeyboardHistory());
    puts("PASS actual split-prefix translation keeps pending origin until output retirement");
    AddTo6805BuffImm(0xfa);continue_output();
    CHECK(output_full && !nt_keyboard_history.output && !PendingKeyboardHistory());
    bKbdEoiPending=1;KbdEOIHook(1,0);
    begin();host_key_down(0x32);end();CHECK(PendingKeyboardHistory()==1);
    clear_buff_6805();CHECK(!PendingKeyboardHistory());
    puts("PASS actual internal response has no raw origin and buffer clear discards pending metadata");
    begin();host_key_down(0x12);end();continue_output();
    CHECK(PendingKeyboardHistory()==1);
    Reset6805and8042();
    CHECK(mvdm_keyboard_history_selected(&nt_keyboard_history)==1);
    CHECK(!output_full && !pending_8042 && !scanning_discontinued && !held_event_count);
    CHECK(!nt_keyboard_history.output && !nt_keyboard_history.pending && !nt_keyboard_history.prefix);
    CHECK(!PendingKeyboardHistory());
    puts("PASS original full hardware reset clears device provenance but preserves captured return selection");
    CHECK(interrupts>0);return 0;
}
