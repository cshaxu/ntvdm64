/* Carrier tests only. Actual device metadata hooks and guest execution are
 * separate required gates; artificial slot values cannot prove wiring. */
#include "ntvdm-exe/softpc/include/mvdm_keyboard_history.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if(!(x)) { printf("FAIL line=%u: %s\n",(unsigned)__LINE__,#x);return 1;} } while(0)
int main(void)
{
    mvdm_keyboard_history state={0};unsigned index;
    CHECK(!mvdm_keyboard_history_begin(&state));
    for(index=0;index<3;++index)CHECK(mvdm_keyboard_history_record(&state));
    CHECK(mvdm_keyboard_history_begin(&state) && state.active==1);
    CHECK(!mvdm_keyboard_history_begin(&state) && state.active==1);
    state.device[511]=state.active;
    mvdm_keyboard_history_end(&state);
    CHECK(mvdm_keyboard_history_begin(&state) && state.active==2);
    /* Ignored release: no device slot acquires this origin. */
    mvdm_keyboard_history_end(&state);
    CHECK(mvdm_keyboard_history_begin(&state) && state.active==3);
    state.device[0]=state.device[1]=state.held[15]=state.active;
    mvdm_keyboard_history_end(&state);
    CHECK(!mvdm_keyboard_history_begin(&state));
    mvdm_keyboard_history_select_begin(&state);
    mvdm_keyboard_history_select(&state,state.device[0]);
    mvdm_keyboard_history_select(&state,state.held[15]);
    mvdm_keyboard_history_select(&state,state.device[511]);
    mvdm_keyboard_history_select(&state,0);
    mvdm_keyboard_history_select(&state,4);
    CHECK(mvdm_keyboard_history_selected(&state)==2);
    CHECK(mvdm_keyboard_history_age(&state,1)==1);
    CHECK(mvdm_keyboard_history_age(&state,2)==3);
    CHECK(!mvdm_keyboard_history_age(&state,0) && !mvdm_keyboard_history_age(&state,3));
    puts("PASS zero/many device expansion, deduplication and original history order");
    state.translated=state.device[0];state.device[0]=0;
    state.pending=state.translated;state.translated=0;
    state.output=state.pending;state.pending=0;
    mvdm_keyboard_history_select_begin(&state);
    mvdm_keyboard_history_select(&state,state.output);
    CHECK(mvdm_keyboard_history_selected(&state)==1 && mvdm_keyboard_history_age(&state,1)==1);
    state.output=0; /* Cleared slot cannot revive the most recent raw key. */
    mvdm_keyboard_history_select_begin(&state);
    mvdm_keyboard_history_select(&state,state.output);
    CHECK(!mvdm_keyboard_history_selected(&state));
    puts("PASS modeled slot movement and unattributed output never select consumed history");
    state.output=2;state.pending=3;state.prefix=1;
    CHECK(mvdm_keyboard_history_pending(&state,511,2,512,16,0,1,1)==2);
    CHECK(mvdm_keyboard_history_age(&state,1)==1 && mvdm_keyboard_history_age(&state,2)==3);
    CHECK(mvdm_keyboard_history_pending(&state,511,2,512,16,1,1,1)==3);
    mvdm_keyboard_history_clear_device(&state);
    CHECK(mvdm_keyboard_history_selected(&state)==3 && state.issued==3 && state.dispatched==3);
    CHECK(!state.output && !state.pending && !state.prefix && !state.device[511] && !state.held[15]);
    CHECK(!mvdm_keyboard_history_pending(&state,511,2,512,16,1,1,1));
    CHECK(mvdm_keyboard_history_pending(&state,0,0,0,0,0,0,0)==UINT32_MAX);
    CHECK(mvdm_keyboard_history_pending(&state,512,0,512,0,0,0,0)==UINT32_MAX);
    CHECK(mvdm_keyboard_history_pending(&state,0,0,512,17,0,0,0)==UINT32_MAX);
    puts("PASS actual pending-slot selector, wrapped ring, held/prefix/output gates and reset-before-return");
    for(index=3;index<203;++index)CHECK(mvdm_keyboard_history_record(&state));
    mvdm_keyboard_history_select_begin(&state);
    mvdm_keyboard_history_select(&state,3);
    mvdm_keyboard_history_select(&state,103);
    mvdm_keyboard_history_select(&state,104);
    mvdm_keyboard_history_select(&state,203);
    CHECK(mvdm_keyboard_history_selected(&state)==2);
    CHECK(mvdm_keyboard_history_age(&state,1)==1 && mvdm_keyboard_history_age(&state,2)==100);
    puts("PASS history wrap expires old origins instead of aliasing overwritten records");
    memset(&state,0,sizeof(state));state.issued=UINT64_MAX;
    CHECK(!mvdm_keyboard_history_record(&state) && state.issued==UINT64_MAX);
    mvdm_keyboard_history_select(&state,UINT64_MAX);
    CHECK(mvdm_keyboard_history_age(&state,1)==1);
    memset(&state,0,sizeof(state));
    CHECK(!mvdm_keyboard_history_selected(&state) && !state.output && !state.active);
    puts("PASS serial overflow refusal and empty reset");
    return 0;
}
