#include "compat/include/mvdm_softpc_mouse_guest.h"
#include <limits.h>
#include <string.h>
BOOL mvdm_softpc_mouse_route_active(void)
{
    mvdm_mouse_bridge *state=mvdm_softpc_mouse_current();
    BOOL active;
    if(!state)return FALSE;
    state->services->lock();
    active=mvdm_softpc_mouse_active(state);
    state->services->unlock();
    return active;
}
int16_t mvdm_mouse_coordinate_delta(int16_t position,int32_t delta)
{
    int32_t next=(int32_t)position+delta;
    if(next>SHRT_MAX)delta=SHRT_MAX-(int32_t)position;
    if(next<SHRT_MIN)delta=SHRT_MIN-(int32_t)position;
    return (int16_t)delta;
}
DWORD mvdm_softpc_mouse_receive(const INPUT_RECORD *record)
{
    console_mouse_input input;
    memcpy(&input,&record->Event,sizeof(input));
    return mvdm_softpc_mouse_submit(mvdm_softpc_mouse_current(),&input);
}
