#include <windows.h>
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <sas.h>
#include <mouse_io.h>
#include <nt_mouse.h>
#include <host_rrr.h>
#include <nt_uis.h>
#include "mvdm_softpc_mouse_guest.h"
#include <limits.h>
#include <string.h>

extern void EmulateCoordinates(half_word,IS16,IS16,IS16 *,IS16 *);
extern void host_ica_lock(void);
extern void host_ica_unlock(void);
extern void mouse_pointer_route_changed(int);

BOOL mvdm_softpc_mouse_route_active(void)
{
    BOOL active;
    host_ica_lock();
    active=mvdm_softpc_mouse_active(mvdm_softpc_mouse_current());
    host_ica_unlock();
    return active;
}

/* The original routine narrows its addition to IS16 before range limiting.
 * Saturate only that intermediate sum; original LimitCoordinates still owns
 * guest bounds. The raw motion counter retains the delivered displacement. */
static IS16 coordinate_delta(IS16 position,int32_t delta)
{
    int32_t next=(int32_t)position+delta;
    if(next>SHRT_MAX)delta=SHRT_MAX-(int32_t)position;
    if(next<SHRT_MIN)delta=SHRT_MIN-(int32_t)position;
    return (IS16)delta;
}
BOOL mvdm_softpc_mouse_apply(MOUSE_CURSOR_STATUS *cursor,MOUSE_VECTOR *counter,
    int *old_x,int *old_y,BOOL *unchanged,BOOL resetting,BOOL *position_set,
    IS16 *new_x,IS16 *new_y)
{
    mvdm_mouse_bridge *state=mvdm_softpc_mouse_current();
    mvdm_mouse_input_sample input;
    half_word mode;
    if(!mvdm_softpc_mouse_next(state,&input))return FALSE;
    sas_load(0x449,&mode);
    if(input.action==CONSOLE_MOUSE_ENTER && !resetting && !*position_set) {
        /* The base INT33 owner may have changed position since the last IRQ.
         * Its current guest position, not the host IRQ cache, seeds the route. */
        *new_x=cursor->position.x;*new_y=cursor->position.y;*position_set=TRUE;
    }
    /* Synchronize reset/guest-set-position before consuming real movement. */
    EmulateCoordinates(mode,0,0,&cursor->position.x,&cursor->position.y);
    EmulateCoordinates(mode,coordinate_delta(cursor->position.x,input.dx),
        coordinate_delta(cursor->position.y,input.dy),&cursor->position.x,&cursor->position.y);
    counter->x=(IS16)input.dx;counter->y=(IS16)input.dy;
    *unchanged=*old_x==cursor->position.x && *old_y==cursor->position.y;
    *old_x=cursor->position.x;*old_y=cursor->position.y;
    os_pointer_data.button_l=(SHORT)(input.buttons&1);
    os_pointer_data.button_r=(SHORT)((input.buttons>>1)&1);
    if(input.action==CONSOLE_MOUSE_LEAVE) {
        mvdm_softpc_mouse_leave(state);
    }
    /* Consume the route edge on the original CPU owner, after committing
     * guest position and route state. Do not invent POSITION callback bits. */
    if(input.action==CONSOLE_MOUSE_ENTER || input.action==CONSOLE_MOUSE_LEAVE)
        mouse_pointer_route_changed(input.action==CONSOLE_MOUSE_ENTER);
    return TRUE;
}
void mvdm_softpc_mouse_receive(const INPUT_RECORD *record)
{
    console_mouse_input input;DWORD error;
    memcpy(&input,&record->Event,sizeof(input));
    error=mvdm_softpc_mouse_submit(mvdm_softpc_mouse_current(),&input);
    if(error)DisplayErrorTerm(EHS_FUNC_FAILED,error,__FILE__,__LINE__);
}
