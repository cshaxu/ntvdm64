/* NTSRV-local empty grace decisions. Caller holds idle_lock; timer and service
 * ownership stay in main.c. Explicit monotonic time permits boundary tests,
 * never a runtime timeout override. */
#ifndef NTSRV_IDLE_POLICY_H
#define NTSRV_IDLE_POLICY_H
#include <windows.h>
#define BASESRV_EMPTY_GRACE_MS 10000u
typedef struct BASESRV_IDLE_POLICY {
    ULONGLONG deadline;
    ULONG pending;
    BOOL stopping;
} BASESRV_IDLE_POLICY;
typedef enum BASESRV_IDLE_DECISION {
    BASESRV_IDLE_WAIT, BASESRV_IDLE_REARM, BASESRV_IDLE_STOP
} BASESRV_IDLE_DECISION;

static __inline BOOL basesrv_idle_arm(BASESRV_IDLE_POLICY *state,ULONGLONG now,BOOL empty)
{
    if(state->stopping || state->deadline || state->pending || !empty)return FALSE;
    state->deadline=now+BASESRV_EMPTY_GRACE_MS;
    return TRUE;
}
static __inline DWORD basesrv_idle_connect(BASESRV_IDLE_POLICY *state)
{
    if(state->stopping)return RPC_S_SERVER_UNAVAILABLE;
    if(state->pending==MAXDWORD)return ERROR_ARITHMETIC_OVERFLOW;
    ++state->pending;
    state->deadline=0;
    return ERROR_SUCCESS;
}
static __inline BASESRV_IDLE_DECISION basesrv_idle_expire(BASESRV_IDLE_POLICY *state,
    ULONGLONG now,BOOL empty)
{
    if(state->pending || !state->deadline)return BASESRV_IDLE_WAIT;
    if(now<state->deadline)return BASESRV_IDLE_REARM;
    if(empty){state->stopping=TRUE;return BASESRV_IDLE_STOP;}
    state->deadline=0;
    return BASESRV_IDLE_WAIT;
}
#endif
