#include "t198_s23_fastread_attempt_ledger.h"
#include <string.h>
int main(void) {
    struct t198_s23_fastread_attempt_ledger_v1 ledger;
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    struct t198_s23_fastread_attempt_v1 entry;
    unsigned i;
    memset(&event,0,sizeof(event)); memset(&outcome,0,sizeof(outcome));
    t198_s23_fastread_attempt_ledger_v1_reset(&ledger);
    event.window_bytes=4u; event.window[0]=0xc4u; event.window[1]=0xc4u; event.window[2]=0x50u; event.window[3]=0x43u;
    t198_s23_fastread_attempt_ledger_v1_record(&ledger,&event,&outcome,0); if(ledger.count) return 1;
    event.window[3]=0x42u; event.cs=0x32u; event.eip=0x6351u; event.eax=0x4e53u; event.ebp=0x4005u; event.ecx=0x20u;
    t198_s23_fastread_attempt_ledger_v1_record(&ledger,&event,&outcome,0);
    outcome.disposition=BX_NTVDM_GENERIC_UD_RESUME; outcome.resume_rip=0x6355u; outcome.gpr16_write_mask=1u; outcome.gpr16_values[0]=13u; outcome.eflags_write_mask=1u;
    event.eip=0x7000u; t198_s23_fastread_attempt_ledger_v1_record(&ledger,&event,&outcome,1);
    if(ledger.count!=2u || !t198_s23_fastread_attempt_ledger_v1_get(&ledger,0,&entry) || entry.accepted || entry.eip!=0x6351u || entry.ax) return 2;
    if(!t198_s23_fastread_attempt_ledger_v1_get(&ledger,1,&entry) || !entry.accepted || entry.eip!=0x7000u || entry.ax!=13u || entry.resume_rip!=0x6355u) return 3;
    for(i=0;i<16u;i++){event.eip=0x8000u+i;t198_s23_fastread_attempt_ledger_v1_record(&ledger,&event,&outcome,1);} if(ledger.count!=T198_S23_FASTREAD_ATTEMPT_LEDGER_CAPACITY) return 4;
    return t198_s23_fastread_attempt_ledger_v1_get(&ledger,T198_S23_FASTREAD_ATTEMPT_LEDGER_CAPACITY,&entry)?5:0;
}
