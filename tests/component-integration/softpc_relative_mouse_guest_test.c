/* Production adapter + bridge/queue + unchanged original coordinate bodies.
 * Guest memory, cursor painting and ICA/IRQ are mocks, not guest acceptance. */
#include <windows.h>
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <sas.h>
#include <mouse_io.h>
#include <nt_mouse.h>
#include "ntvdm-exe/softpc/mvdm_softpc_mouse_guest.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef NDEBUG
#error This fixture requires assertions.
#endif

word VirtualX=640,VirtualY=200;
MOUSE_STATUS os_pointer_data;
static BOOL bFunctionZeroReset,bFunctionFour;
static IS16 newF4x,newF4y;
static struct { BOOL bF7,bF8; IS16 xmin,xmax,ymin,ymax; } confine;
static half_word bios_mode=3,rows=24;
static unsigned locked,interrupts;
static mvdm_mouse_bridge bridge;
half_word c_sas_hw_at(sys_addr address)
{
    if(address==0x449)return bios_mode;
    if(address==0x484)return rows;
    assert(!"unexpected guest-memory read");return 0;
}
void host_ica_lock(void) { assert(!locked);locked=1; }
void host_ica_unlock(void) { assert(locked);locked=0; }
void DoMouseInterrupt(void) { assert(locked);++interrupts; }
mvdm_mouse_bridge *mvdm_softpc_mouse_current(void) { return &bridge; }
int DisplayErrorTerm(int error,DWORD os_error,char *file,int line)
{ (void)error;(void)os_error;(void)file;(void)line;abort(); }
#include "original_mouse_algorithms.h"

static void receive(unsigned action,int dx,int dy,unsigned buttons)
{
    INPUT_RECORD record={0};
    console_mouse_input input={dx,dy,640,200,(uint16_t)buttons,(uint16_t)action};
    if(action==CONSOLE_MOUSE_LEAVE)input.width=input.height=0;
    record.EventType=CONSOLE_INPUT_RELATIVE_MOUSE;
    memcpy(&record.Event,&input,sizeof(input));
    mvdm_softpc_mouse_receive(&record);
}
int main(void)
{
    MOUSE_CURSOR_STATUS cursor={0};MOUSE_VECTOR counter={0};
    int x=100,y=50;BOOL unchanged=FALSE;
    cursor.position.x=100;cursor.position.y=50;
#define APPLY() mvdm_softpc_mouse_apply(&cursor,&counter,&x,&y,&unchanged, \
    bFunctionZeroReset,&bFunctionFour,&newF4x,&newF4y)
    assert(!APPLY());
    assert(!mvdm_softpc_mouse_route_active() && !locked);
    receive(CONSOLE_MOUSE_ENTER,0,0,0);
    assert(APPLY() && x==100 && y==50 && unchanged);
    assert(mvdm_softpc_mouse_route_active() && !locked);
    receive(CONSOLE_MOUSE_MOVE,7,9,0);
    assert(APPLY() && x==107 && y==59 && !unchanged);
    assert(counter.x==7 && counter.y==9 && !os_pointer_data.button_l);
    receive(CONSOLE_MOUSE_MOVE,0,0,1);
    assert(APPLY() && unchanged && os_pointer_data.button_l==1);
    assert(APPLY() && unchanged && os_pointer_data.button_l==1);
    bFunctionFour=TRUE;newF4x=80;newF4y=40;
    receive(CONSOLE_MOUSE_MOVE,3,2,0);
    assert(APPLY() && x==83 && y==42 && !os_pointer_data.button_l);
    bFunctionZeroReset=TRUE;
    receive(CONSOLE_MOUSE_MOVE,7,9,2);
    assert(APPLY() && x==326 && y==108 && os_pointer_data.button_r==1);
    receive(CONSOLE_MOUSE_MOVE,SHRT_MAX,SHRT_MAX,0);
    assert(APPLY() && x==639 && y==199 && counter.x==SHRT_MAX);
    receive(CONSOLE_MOUSE_MOVE,SHRT_MIN,SHRT_MIN,0);
    assert(APPLY() && x==0 && y==0 && counter.x==SHRT_MIN);
    receive(CONSOLE_MOUSE_LEAVE,0,0,0);
    receive(CONSOLE_MOUSE_ENTER,0,0,0);
    assert(APPLY() && !mvdm_softpc_mouse_active(&bridge));
    assert(!mvdm_softpc_mouse_route_active() && !locked);
    assert(APPLY() && mvdm_softpc_mouse_active(&bridge));
    assert(interrupts==9 && !locked);
    mvdm_softpc_mouse_cancel(&bridge);assert(!APPLY());
    /* Guest INT33/4 before Window selection updates base state without an IRQ. */
    cursor.position.x=240;cursor.position.y=88;x=12;y=16;
    receive(CONSOLE_MOUSE_ENTER,0,0,0);
    assert(APPLY() && x==240 && y==88);
    receive(CONSOLE_MOUSE_MOVE,4,2,0);
    assert(APPLY() && x==244 && y==90);
    mvdm_softpc_mouse_cancel(&bridge);
    puts("PASS production mouse adapter: copied ingress, original coordinates, reset/position, idle buttons, overflow, release/re-entry; memory/IRQ mocked, cursor drawing untested");
    return 0;
}
