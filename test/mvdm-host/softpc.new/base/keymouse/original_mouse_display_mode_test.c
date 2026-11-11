#include <stdio.h>
#define EGG 1
typedef unsigned short word;
static unsigned bios_mode, Currently_emulated_video_mode, display_state;
static int dispatched;
#define vd_video_mode 0
#define sas_hw_at(address) bios_mode
#define alpha_num_mode() (bios_mode < 4 || bios_mode == 7)
/* egagraph.h's original is_it_text() mask; the hardware owner is independent
 * of the BIOS data area and last BIOS mode setter. */
#define is_it_text() ((display_state & 0x80) == 0x80)
#include "mouse-mode.h"
#define getAX() 3
#define getBX() 0
#define getCX() 0
#define getDX() 0
#define setAX(value) ((void)(value))
#define setBX(value) ((void)(value))
#define setCX(value) ((void)(value))
#define setDX(value) ((void)(value))
#define note_trace0(...) ((void)0)
#define note_trace4(...) ((void)0)
static void do_mouse_function(word *a,word *b,word *c,word *d)
{ (void)a;(void)b;(void)c;(void)d;dispatched=jap_mouse; }
#include "mouse-entry.h"
int main(void)
{
    static const struct {unsigned bios,last,hardware;int fake;} cases[]={
        {3,103,0xc0,0}, /* Retained corrupt-cursor scene: actual text. */
        {3,103,0x40,1}, /* Genuine graphics-backed BIOS text remains supported. */
        {3,3,0xc0,0},{7,103,0xc0,0},{7,103,0x40,1},
        {0x12,0x12,0,0},{0x67,0x67,0,0},{3,3,0x40,0}
    };
    unsigned i;
    for(i=0;i<sizeof(cases)/sizeof(cases[0]);++i){
        bios_mode=cases[i].bios;Currently_emulated_video_mode=cases[i].last;
        display_state=cases[i].hardware;dispatched=-1;
        mouse_io_interrupt();
        if(dispatched!=cases[i].fake){fprintf(stderr,"FAIL case %u: painter=%d expected=%d\n",i,dispatched,cases[i].fake);return 1;}
    }
    /* VDD changes hardware state without another INT33 call. All original
     * painter/erase/shape users must observe the new state immediately. */
    bios_mode=3;Currently_emulated_video_mode=103;display_state=0x40;
    mouse_io_interrupt();
    display_state=0xc0;
    if(jap_mouse){fputs("FAIL stale graphics painter after VDD text restore\n",stderr);return 1;}
    display_state=0x40;
    if(!jap_mouse){fputs("FAIL graphics-text mode lost without INT33\n",stderr);return 1;}
    puts("PASS 8 mode cases and bidirectional VDD transitions without INT33");
    return 0;
}
