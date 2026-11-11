/* Retained counterexample for the scalar-count/raw-suffix mock boundary.
 * After DIV-317 this intentionally bypasses production origin selection;
 * keyboard_origin_return_test.c owns the corrected history-return contract.
 * Guest/CPU and the unavailable Console sink are controlled dependencies. */
#include "ntvdm-exe/softpc/include/mvdm_keyboard_history.h"
static mvdm_keyboard_history nt_keyboard_history;
#define main unused_return_fixture_main
#include "unused_key_return_test.c"
#undef main
#define GLOBAL
#define VOID void
#define IFN1(type,name) (type name)
#define NTVDM
#define BUFF_6805_PMASK 63
#define ONECHARCODEMASK 0x80
#define HELD_EVENT_MAX 16
#define KEY_UP_EVENT 1
#define KEY_DOWN_EVENT 2
static unsigned char key_marker_buffer[64];
static int held_event_count,buff_6805_out_ptr,buff_6805_in_ptr,output_full;
static int scanning_discontinued,keyboard_disabled,LastKeyDown;
static int held_event_type[HELD_EVENT_MAX],held_event_key[HELD_EVENT_MAX];
static int key_down_count[127];
static ULONG KbdHdwFull;
static void packet(void) { key_marker_buffer[buff_6805_in_ptr]=(unsigned char)(0x81+buff_6805_in_ptr);++buff_6805_in_ptr; }
static void Reset6805and8042(void) { }
static void do_host_key_down(int key) { (void)key;packet(); }
static void do_host_key_up(int key)
{
    key_down_count[key]=0;packet();
}
#include "keyboard_host_down.inc"
#include "keyboard_host_up.inc"
#include "keyboard_buffer_count.inc"
/* Exercise original dispatch/control decisions, not scan translation. Both
 * actions use one controlled key ID; packet production is the mocked edge. */
static DWORD ToggleKeyState;
static int BWVKey;
static KEY_EVENT_RECORD fake_shift,fake_alt,fake_ctl;
static void SyncToggleKeys(WORD key,DWORD flags) { (void)key;(void)flags; }
static void nt_process_screen_scale(void) { }
static void nt_key_down_action(PKEY_EVENT_RECORD key) { (void)key;host_key_down(19); }
static void nt_key_up_action(PKEY_EVENT_RECORD key) { (void)key;host_key_up(19); }
#define TOGGLEKEYBITS (SHIFT_PRESSED|NUMLOCK_ON|SCROLLLOCK_ON|CAPSLOCK_ON)
#include "keyboard_process.inc"
static int control(const char *name,WORD vk,int scanning,int disabled,int repeat,int expected)
{
    KEY_EVENT_RECORD key={0};int count,partial,i;
    memset(key_marker_buffer,0,sizeof(key_marker_buffer));
    memset(key_down_count,0,sizeof(key_down_count));
    buff_6805_out_ptr=buff_6805_in_ptr=held_event_count=output_full=0;
    scanning_discontinued=scanning;keyboard_disabled=disabled;LastKeyDown=-1;KbdHdwFull=0;
    if(repeat) {
        for(i=0;i<8;++i)packet();
        KbdHdwFull=8;LastKeyDown=19;key_down_count[19]=1;key.bKeyDown=TRUE;
    }
    key.wVirtualKeyCode=vk;key.wRepeatCount=1;
    nt_process_keys(&key);
    count=keys_in_6805_buff(&partial);
    printf("%s control-%s device=%d expected=%d held=%d partial=%d\n",
        count==expected ? "PASS":"FAIL",name,count,expected,held_event_count,partial);
    return count!=expected;
}
static int check(const char *name,int ignored_position)
{
    int partial,unread,m_index=ignored_position==0 ? 1:0;
    memset(history,0,sizeof(history));memset(captured,0,sizeof(captured));
    memset(key_marker_buffer,0,sizeof(key_marker_buffer));
    memset(key_down_count,0,sizeof(key_down_count));
    available=ignored_position<0 ? 1:2;sent=calls=0;
    held_event_count=output_full=scanning_discontinued=keyboard_disabled=0;
    buff_6805_out_ptr=0;buff_6805_in_ptr=1;key_marker_buffer[0]=0x81;
    history[m_index].bKeyDown=TRUE;history[m_index].wVirtualKeyCode='M';
    history[m_index].wVirtualScanCode=0x32;history[m_index].wRepeatCount=1;
    if(ignored_position>=0) {
        history[ignored_position].wVirtualKeyCode='E';
        history[ignored_position].wVirtualScanCode=0x12;
        history[ignored_position].wRepeatCount=1;
        host_key_up(19); /* Original device ignores an E up without E down. */
    }
    unread=keys_in_6805_buff(&partial);
    ReturnUnusedKeyEvents(unread);
    printf("%s %s history=%d device=%d returned=%lu vk=%u down=%d\n",
        sent==1 && captured[0].Event.KeyEvent.wVirtualKeyCode=='M' &&
        captured[0].Event.KeyEvent.bKeyDown ? "PASS":"FAIL",name,available,
        unread,sent,captured[0].Event.KeyEvent.wVirtualKeyCode,
        captured[0].Event.KeyEvent.bKeyDown);
    return sent!=1 || captured[0].Event.KeyEvent.wVirtualKeyCode!='M' ||
        !captured[0].Event.KeyEvent.bKeyDown;
}
static int captured_handoff(void)
{
    /* Exact ring markers/output-full and raw-history order from the failing
     * history-only-r1 handoff at tick 66787656. No guest state is patched. */
    static const unsigned char markers[]={114,114,243,116,116,245,118,118,
        247,120,120,249,122,122,251,124,124,253,126,126,255,1,1,130,3,3};
    static const char text[]="exit\rmem\rexit\r";
    int i,index=0,count,partial,failed;
    memset(history,0,sizeof(history));memset(captured,0,sizeof(captured));
    memset(key_marker_buffer,0,sizeof(key_marker_buffer));
    sent=calls=0;
    for(i=0;text[i];++i) {
        KEY_EVENT_RECORD key={0};
        key.wVirtualKeyCode=text[i]=='\r' ? VK_RETURN:(WORD)(text[i]-'a'+'A');
        key.wRepeatCount=1;key.uChar.UnicodeChar=text[i];key.bKeyDown=TRUE;
        history[index++]=key;key.bKeyDown=FALSE;
        if(i>=5)key.uChar.UnicodeChar=0;
        /* First M has character-bearing and zero-character releases. Their
         * producers are not inferred by this return-boundary fixture. */
        if(i==5) { key.uChar.UnicodeChar='m';history[index++]=key;key.uChar.UnicodeChar=0; }
        history[index++]=key;
    }
    available=index;held_event_count=0;output_full=1;
    buff_6805_out_ptr=16;buff_6805_in_ptr=42;
    memcpy(key_marker_buffer+16,markers,sizeof(markers));
    count=keys_in_6805_buff(&partial);
    ReturnUnusedKeyEvents(count);
    failed=count!=18 || available!=29 || !sent ||
        captured[0].Event.KeyEvent.wVirtualKeyCode!='M' ||
        !captured[0].Event.KeyEvent.bKeyDown;
    printf("%s captured-handoff history=%d device=%d returned=%lu first-vk=%u down=%d\n",
        failed ? "FAIL":"PASS",available,count,sent,
        captured[0].Event.KeyEvent.wVirtualKeyCode,captured[0].Event.KeyEvent.bKeyDown);
    return failed;
}
static int expanded_release_return(const char *name,WORD vk)
{
    KEY_EVENT_RECORD key={0};int count,partial,failed;
    memset(history,0,sizeof(history));memset(captured,0,sizeof(captured));
    memset(key_marker_buffer,0,sizeof(key_marker_buffer));
    memset(key_down_count,0,sizeof(key_down_count));
    held_event_count=output_full=scanning_discontinued=keyboard_disabled=0;
    buff_6805_out_ptr=buff_6805_in_ptr=0;LastKeyDown=-1;KbdHdwFull=0;
    available=2;sent=calls=0;
    /* The old M is already consumed: there is no M packet in the device.
     * The remaining single raw special-key release reaches the actual
     * nt_process_keys fake-make branch and generates two device packets. */
    history[0].bKeyDown=TRUE;history[0].wVirtualKeyCode='M';
    history[0].wVirtualScanCode=0x32;history[0].wRepeatCount=1;
    key.wVirtualKeyCode=vk;key.wRepeatCount=1;history[1]=key;
    nt_process_keys(&key);
    count=keys_in_6805_buff(&partial);
    ReturnUnusedKeyEvents(count);
    failed=count!=2 || sent!=1 || captured[0].Event.KeyEvent.wVirtualKeyCode!=vk;
    printf("%s expanded-%s history=%d device=%d returned=%lu first-vk=%u consumed-M-replayed=%d\n",
        failed ? "FAIL":"PASS",name,available,count,sent,
        captured[0].Event.KeyEvent.wVirtualKeyCode,
        sent && captured[0].Event.KeyEvent.wVirtualKeyCode=='M');
    return failed;
}
static int non_console_device_return(const char *name,int controller_output)
{
    int count,partial,failed;
    memset(history,0,sizeof(history));memset(captured,0,sizeof(captured));
    memset(key_marker_buffer,0,sizeof(key_marker_buffer));
    memset(key_down_count,0,sizeof(key_down_count));
    held_event_count=output_full=scanning_discontinued=keyboard_disabled=0;
    buff_6805_out_ptr=buff_6805_in_ptr=0;LastKeyDown=-1;KbdHdwFull=0;
    available=1;sent=calls=0;
    /* M was consumed already. Original AltUpDownUp/SyncToggleKeys can
     * generate hardware actions without a corresponding Console record;
     * controller output likewise need not represent a raw input record.
     * The fixture controls that provenance, not the keyboard translation. */
    history[0].bKeyDown=TRUE;history[0].wVirtualKeyCode='M';
    history[0].wVirtualScanCode=0x32;history[0].wRepeatCount=1;
    if(controller_output)output_full=1;
    else host_key_down(19);
    count=keys_in_6805_buff(&partial);
    ReturnUnusedKeyEvents(count);
    failed=count!=1 || (sent && captured[0].Event.KeyEvent.wVirtualKeyCode=='M');
    printf("%s non-console-%s device=%d returned=%lu consumed-M-replayed=%d\n",
        failed ? "FAIL":"PASS",name,count,sent,
        sent && captured[0].Event.KeyEvent.wVirtualKeyCode=='M');
    return failed;
}
int main(void)
{
    int failures=check("unread-make",-1);
    failures+=check("ignored-release-before-make",0);
    failures+=check("ignored-release-after-make",1);
    failures+=control("ordinary-unpaired-up",'E',0,0,0,0);
    failures+=control("ctrl-break-fake-make",VK_CANCEL,0,0,0,2);
    failures+=control("printscreen-fake-make",VK_SNAPSHOT,0,0,0,2);
    failures+=control("held-unpaired-up",'E',1,0,0,1);
    failures+=control("disabled-ctrl-break",VK_CANCEL,0,1,0,0);
    failures+=control("contiguous-repeat-suppressed",'E',0,0,1,8);
    failures+=captured_handoff();
    failures+=expanded_release_return("ctrl-break",VK_CANCEL);
    failures+=expanded_release_return("printscreen",VK_SNAPSHOT);
    failures+=non_console_device_return("synthetic-action",0);
    failures+=non_console_device_return("controller-output",1);
    return failures ? 1:0;
}
