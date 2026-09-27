#include "include/mvdm_keyboard_history.h"
#include <string.h>

int mvdm_keyboard_history_record(mvdm_keyboard_history *state)
{
    if(!state || state->issued==UINT64_MAX)return 0;
    ++state->issued;
    return 1;
}
int mvdm_keyboard_history_begin(mvdm_keyboard_history *state)
{
    if(!state || state->active || state->dispatched>=state->issued)return 0;
    state->active=++state->dispatched;
    return 1;
}
void mvdm_keyboard_history_end(mvdm_keyboard_history *state)
{
    state->active=0;
}
void mvdm_keyboard_history_select_begin(mvdm_keyboard_history *state)
{
    memset(state->selected,0,sizeof(state->selected));
}
void mvdm_keyboard_history_select(mvdm_keyboard_history *state,mvdm_key_origin origin)
{
    mvdm_key_origin age;
    if(!origin || origin>state->issued)return;
    age=state->issued-origin;
    if(age<MVDM_KEY_HISTORY_CAPACITY)state->selected[(unsigned)age]=1;
}
unsigned mvdm_keyboard_history_selected(const mvdm_keyboard_history *state)
{
    unsigned age,count=0;
    for(age=0;age<MVDM_KEY_HISTORY_CAPACITY;++age)count+=state->selected[age]!=0;
    return count;
}
unsigned mvdm_keyboard_history_age(const mvdm_keyboard_history *state,unsigned ordinal)
{
    unsigned age;
    if(!ordinal)return 0;
    for(age=0;age<MVDM_KEY_HISTORY_CAPACITY;++age)
        if(state->selected[age] && !--ordinal)return age+1;
    return 0;
}
unsigned mvdm_keyboard_history_pending(mvdm_keyboard_history *state,
    unsigned first,unsigned next,unsigned capacity,unsigned held,
    int output,int pending,int prefix)
{
    unsigned index;
    mvdm_keyboard_history_select_begin(state);
    if(!capacity || capacity>MVDM_KEY_DEVICE_CAPACITY || first>=capacity ||
        next>=capacity || held>MVDM_KEY_HELD_CAPACITY)return UINT32_MAX;
    for(index=first;index!=next;index=(index+1)%capacity)
        mvdm_keyboard_history_select(state,state->device[index]);
    for(index=0;index<held;++index)mvdm_keyboard_history_select(state,state->held[index]);
    if(output)mvdm_keyboard_history_select(state,state->output);
    if(pending)mvdm_keyboard_history_select(state,state->pending);
    if(prefix)mvdm_keyboard_history_select(state,state->prefix);
    return mvdm_keyboard_history_selected(state);
}
void mvdm_keyboard_history_clear_ring(mvdm_keyboard_history *state)
{
    memset(state->device,0,sizeof(state->device));
    state->translated=state->prefix=0;
}
void mvdm_keyboard_history_clear_device(mvdm_keyboard_history *state)
{
    mvdm_keyboard_history_clear_ring(state);
    memset(state->held,0,sizeof(state->held));
    state->pending=state->output=0;
}
