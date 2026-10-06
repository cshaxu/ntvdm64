#include "ntvdm_dos_observer.h"
#include "mvdm_guest_location.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include <nt_vdd.h>
#include <string.h>
/* Original callback ABI has no context parameter. Bind one explicit owner
 * during this carrier's sole guest session; never introduce another machine. */
static ntvdm_dos_observer *registered;
static BOOL guest_copy(uint16_t segment,uint16_t offset,void *bytes,DWORD count)
{
    mvdm_guest_location location;mvdm_guest_location_lease lease;
    if(!mvdm_guest_location_set_real_mode(&location,segment,offset) ||
        !mvdm_guest_location_acquire(&location,count,GUEST_MEMORY_ACCESS_READ,&lease))return FALSE;
    memcpy(bytes,lease.bytes,count);
    return mvdm_guest_location_release(&lease,0)!=0;
}
static uint16_t word(const unsigned char *bytes)
{
    return (uint16_t)(bytes[0]|(uint16_t)bytes[1]<<8);
}
static ntvdm_dos_occurrence *active_psp(ntvdm_dos_observer *state,uint16_t psp)
{
    unsigned index;
    for(index=0;index<DOS_OBSERVER_SLOTS;++index)
        if(state->slots[index].active && state->slots[index].psp==psp)return &state->slots[index];
    return NULL;
}
static void copy_image(ntvdm_dos_observer *state,uint16_t environment,wchar_t image[260])
{
    unsigned char arena[16];DWORD bytes,index,start,end;
    if(!environment || !guest_copy((uint16_t)(environment-1),0,arena,sizeof(arena)) ||
        (arena[0]!='M' && arena[0]!='Z'))return;
    bytes=(DWORD)word(arena+3)*16;
    if(bytes>sizeof(state->environment))bytes=sizeof(state->environment);
    if(bytes<5 || !guest_copy(environment,0,state->environment,bytes))return;
    for(index=0;index+4<bytes;++index) {
        if(state->environment[index] || state->environment[index+1])continue;
        /* Original DOS environment suffix: double zero, count word, image.
         * Unsupported/malformed suffix is unknown, not a guessed basename. */
        if(word(state->environment+index+2)!=1)return;
        start=index+4;
        for(end=start;end<bytes && end-start<260 && state->environment[end];++end){}
        if(end==bytes || end-start>=260 || end==start)return;
        if(!MultiByteToWideChar(CP_OEMCP,0,(const char *)state->environment+start,
            (int)(end-start+1),image,260))image[0]=0;
        return;
    }
}
static ntvdm_dos_observer *callback_owner(void)
{
    ntvdm_dos_observer *state=registered;
    return state && state->thread==GetCurrentThreadId() && session_thread_current()==state->owner ? state : NULL;
}
static VOID created(USHORT psp)
{
    ntvdm_dos_observer *state=callback_owner();ntvdm_dos_occurrence *slot,*parent;
    common_dos_observation fact={0};unsigned char header[48];unsigned index;
    if(!state)return;
    slot=active_psp(state,psp);
    if(!slot)for(index=0;index<DOS_OBSERVER_SLOTS;++index)
        if(!state->slots[index].active){slot=&state->slots[index];break;}
    if(!slot || state->sequence==UINT64_MAX) {
        fact.event=DOS_OBSERVATION_GAP;worker_task_observer_offer(&state->outbox,&fact);return;
    }
    fact.event=DOS_OBSERVATION_ENTER;fact.psp=psp;fact.occurrence=++state->sequence;
    fact.direct=OpenNtBaseClientObservationDirect();
    if(guest_copy(psp,0,header,sizeof(header)) && header[0]==0xcd && header[1]==0x20) {
        fact.parent_psp=word(header+0x16);
        parent=fact.parent_psp!=psp ? active_psp(state,(uint16_t)fact.parent_psp) : NULL;
        if(parent)fact.parent=parent->occurrence;
        copy_image(state,word(header+0x2c),fact.image);
    }
    /* PSP reuse is another occurrence; absence of old termination remains
     * uncertain in the server, never synthesised as an EXIT here. */
    slot->psp=psp;slot->occurrence=fact.occurrence;slot->direct=fact.direct;slot->active=TRUE;
    worker_task_observer_offer(&state->outbox,&fact);
}
static VOID terminated(USHORT psp)
{
    ntvdm_dos_observer *state=callback_owner();ntvdm_dos_occurrence *slot;
    common_dos_observation fact={0};
    if(!state || !(slot=active_psp(state,psp)))return;
    fact.event=DOS_OBSERVATION_EXIT;fact.psp=psp;
    fact.occurrence=slot->occurrence;fact.direct=slot->direct;slot->active=FALSE;
    worker_task_observer_offer(&state->outbox,&fact);
}
static DWORD publish(void *context,const common_dos_observation *fact,BOOL gap)
{
    ntvdm_dos_observer *state=context;
    return OpenNtBaseClientObserveDosEvent(fact,gap,state->outbox.stop);
}
static void cancel(void *context)
{
    ntvdm_dos_observer *state=context;
    SetEvent(state->outbox.stop); /* Actual common RPC wait-set cancellation. */
}
DWORD ntvdm_dos_observer_start(ntvdm_dos_observer *state,session *owner)
{
    DWORD error;
    if(!state || !owner || registered || session_thread_current()!=owner)return ERROR_INVALID_STATE;
    memset(state,0,sizeof(*state));state->owner=owner;state->thread=GetCurrentThreadId();
    error=worker_task_observer_start(&state->outbox,publish,cancel,state);if(error)return error;
    state->registration=GetModuleHandleW(NULL);registered=state;
    if(VDDInstallUserHook(state->registration,created,terminated,NULL,NULL))return ERROR_SUCCESS;
    error=GetLastError();registered=NULL;state->registration=NULL;
    worker_task_observer_stop(&state->outbox);return error;
}
void ntvdm_dos_observer_stop(ntvdm_dos_observer *state)
{
    if(!state || !state->outbox.thread)return;
    if(registered==state) {
        (void)VDDDeInstallUserHook(state->registration);registered=NULL;
    }
    /* Producer is now quiescent. Normal teardown gives accepted facts a
     * finite chance to finish; failure cannot change the guest result. */
    (void)worker_task_observer_drain(&state->outbox,10000);
    worker_task_observer_stop(&state->outbox);state->registration=NULL;
}
