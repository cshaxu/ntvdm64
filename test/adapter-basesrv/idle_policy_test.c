#include "ntsrv-exe/idle_policy.h"
#include <stdio.h>
#define CHECK(value) do {if(!(value)){fprintf(stderr,"FAIL idle policy line %d\n",__LINE__);return 1;}}while(0)
int main(void)
{
    BASESRV_IDLE_POLICY state={0};
    CHECK(!basesrv_idle_arm(&state,100,FALSE) && !state.deadline);
    CHECK(basesrv_idle_arm(&state,100,TRUE) && state.deadline==5100);
    CHECK(!basesrv_idle_arm(&state,500,TRUE) && state.deadline==5100);
    CHECK(basesrv_idle_expire(&state,5099,TRUE)==BASESRV_IDLE_REARM && !state.stopping);
    CHECK(!basesrv_idle_connect(&state) && state.pending==1 && !state.deadline);
    CHECK(!basesrv_idle_arm(&state,20000,TRUE));
    CHECK(basesrv_idle_expire(&state,20000,TRUE)==BASESRV_IDLE_WAIT);
    --state.pending; /* Actual end-connect transition, serialized by idle_lock. */
    CHECK(basesrv_idle_arm(&state,20000,TRUE) && state.deadline==25000);
    CHECK(basesrv_idle_expire(&state,25000,FALSE)==BASESRV_IDLE_WAIT && !state.deadline && !state.stopping);
    CHECK(basesrv_idle_arm(&state,31000,TRUE) && state.deadline==36000);
    CHECK(basesrv_idle_expire(&state,36000,TRUE)==BASESRV_IDLE_STOP && state.stopping);
    CHECK(basesrv_idle_connect(&state)==RPC_S_SERVER_UNAVAILABLE && !state.pending);
    CHECK(!basesrv_idle_arm(&state,50000,TRUE));
    state.stopping=FALSE;state.pending=MAXDWORD;
    CHECK(basesrv_idle_connect(&state)==ERROR_ARITHMETIC_OVERFLOW && state.pending==MAXDWORD);
    puts("PASS idle: exact 5s, no renewal, before/at expiry, Connect cancellation, rearm, occupied service, stopping and overflow");
    return 0;
}
