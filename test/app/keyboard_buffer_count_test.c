/* Test the selected original 6805 unread-event counter, not a reimplementation.
 * Only ring state and reset are controlled. No guest or CPU is executed. */
#include <stdio.h>
#include <string.h>
#define GLOBAL
#define TRUE 1
#define FALSE 0
#define BUFF_6805_PMASK 15
#define ONECHARCODEMASK 0x80
static unsigned char key_marker_buffer[16];
static int held_event_count, buff_6805_out_ptr, buff_6805_in_ptr;
static int output_full, reset_calls;
static void Reset6805and8042(void) { ++reset_calls; }
#include "keyboard_buffer_count.inc"

static int check(const char *name,const unsigned char *markers,int count,
    int start,int held,int output,int expected,int partial)
{
    int index,actual,actual_partial=-1;
    memset(key_marker_buffer,0,sizeof(key_marker_buffer));
    for(index=0;index<count;++index)
        key_marker_buffer[(start+index)&BUFF_6805_PMASK]=markers[index];
    buff_6805_out_ptr=start;
    buff_6805_in_ptr=(start+count)&BUFF_6805_PMASK;
    held_event_count=held;output_full=output;reset_calls=0;
    actual=keys_in_6805_buff(&actual_partial);
    printf("%s %s count=%d expected=%d partial=%d expected=%d reset=%d\n",
        actual==expected && actual_partial==partial && reset_calls==1 ? "PASS":"FAIL",
        name,actual,expected,actual_partial,partial,reset_calls);
    return actual!=expected || actual_partial!=partial || reset_calls!=1;
}
int main(void)
{
    static const unsigned char single[]={0x81};
    static const unsigned char singles[]={0x81,0x82,0x83,0x84};
    static const unsigned char pair[]={1,0,1};
    static const unsigned char mixed[]={0x81,2,0,2,0x83};
    static const unsigned char partial[]={1,0};
    /* The start of this multibyte event has already left the ring. Its
     * remaining end marker must not hide later complete single-byte events. */
    static const unsigned char partial_then_singles[]={1,0x82,0x83};
    static const unsigned char partial_then_pair[]={1,2,0,2,0x83};
    int failures=0;
    failures+=check("empty",single,0,0,0,0,0,FALSE);
    failures+=check("single",single,1,0,0,0,1,FALSE);
    failures+=check("four-single",singles,4,0,0,0,4,FALSE);
    failures+=check("wrapped-single",singles,4,14,0,0,4,FALSE);
    failures+=check("multi-byte",pair,3,0,0,0,1,FALSE);
    failures+=check("mixed",mixed,5,13,0,0,3,FALSE);
    failures+=check("held-and-output",singles,4,0,2,1,7,FALSE);
    failures+=check("partial",partial,2,0,0,0,0,TRUE);
    failures+=check("partial-then-singles",partial_then_singles,3,0,0,0,2,TRUE);
    failures+=check("wrapped-partial-then-singles",partial_then_singles,3,15,0,0,2,TRUE);
    failures+=check("partial-then-complete-pair",partial_then_pair,5,0,0,0,2,TRUE);
    return failures ? 1:0;
}
