#include "bx_ntvdm_guest_exec_lifecycle_ledger_v1.h"
#include <string.h>
static void event_for(struct bx_ntvdm_generic_ud_event_v1 *event,
 uint8_t selector, uint8_t service, uint64_t rip) {
 memset(event, 0, sizeof(*event)); event->magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
 event->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
 event->struct_bytes = sizeof(*event); event->vector = 6u; event->fault_rip = rip;
 event->window_bytes = 4u; event->window[0] = 0xc4u; event->window[1] = 0xc4u;
 event->window[2] = selector; event->window[3] = service;
}
static void resume_for(struct bx_ntvdm_generic_ud_outcome_v1 *outcome, uint64_t rip) {
 memset(outcome, 0, sizeof(*outcome));
 outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
 outcome->disposition = BX_NTVDM_GENERIC_UD_RESUME; outcome->resume_rip = rip + 4u;
}
int main(void) {
 struct bx_ntvdm_generic_ud_event_v1 event; struct bx_ntvdm_generic_ud_outcome_v1 outcome;
 struct bx_ntvdm_guest_exec_lifecycle_ledger_v1 ledger; uint32_t index;
 event_for(&event, 0x50u, 0x36u, 0x1000u); resume_for(&outcome, 0x1000u);
 bx_ntvdm_guest_exec_lifecycle_ledger_v1_enable(0u);
 bx_ntvdm_guest_exec_lifecycle_ledger_v1_consider(&event, &outcome);
 if (bx_ntvdm_guest_exec_lifecycle_ledger_v1_copy(&ledger)) return 1;
 bx_ntvdm_guest_exec_lifecycle_ledger_v1_enable(1u);
 for (index = 0u; index < 4u; ++index) {
 static const uint8_t selectors[] = {0x50u,0x50u,0x50u,0x50u};
 static const uint8_t services[] = {0x36u,0x36u,0x3cu,0x36u};
 event_for(&event, selectors[index], services[index], UINT64_C(0x2000)+index*0x10u);
 resume_for(&outcome, event.fault_rip);
 bx_ntvdm_guest_exec_lifecycle_ledger_v1_consider(&event, &outcome);
 }
 event_for(&event,0x54u,0x0bu,0x2040u); resume_for(&outcome,event.fault_rip);
 bx_ntvdm_guest_exec_lifecycle_ledger_v1_consider(&event,&outcome);
 if (!bx_ntvdm_guest_exec_lifecycle_ledger_v1_copy(&ledger)) return 20;
 if (ledger.record_count != 5u) return 21;
 if (ledger.overflowed) return 22;
 if (ledger.records[0].phase != BX_NTVDM_GUEST_EXEC_LIFECYCLE_PHASE_V1_ENTRY) return 23;
 if (ledger.records[2].phase != BX_NTVDM_GUEST_EXEC_LIFECYCLE_PHASE_V1_TERMINATE) return 24;
 if (ledger.records[4].phase != BX_NTVDM_GUEST_EXEC_LIFECYCLE_PHASE_V1_PARENT_RETURN) return 25;
 if (ledger.records[4].outcome.resume_rip != UINT64_C(0x2044)) return 26;
 outcome.disposition=BX_NTVDM_GENERIC_UD_PASS_THROUGH;
 bx_ntvdm_guest_exec_lifecycle_ledger_v1_consider(&event,&outcome);
 if (!bx_ntvdm_guest_exec_lifecycle_ledger_v1_copy(&ledger) || ledger.record_count != 5u) return 3;
 for (index=ledger.record_count; index<=BX_NTVDM_GUEST_EXEC_LIFECYCLE_LEDGER_V1_MAX_RECORDS; ++index) {
 event_for(&event,0x50u,0x36u,UINT64_C(0x3000)+index*0x10u); resume_for(&outcome,event.fault_rip);
 bx_ntvdm_guest_exec_lifecycle_ledger_v1_consider(&event,&outcome);
 }
 if (!bx_ntvdm_guest_exec_lifecycle_ledger_v1_copy(&ledger) ||
 ledger.record_count!=BX_NTVDM_GUEST_EXEC_LIFECYCLE_LEDGER_V1_MAX_RECORDS || !ledger.overflowed) return 4;
 bx_ntvdm_guest_exec_lifecycle_ledger_v1_enable(0u);
 return bx_ntvdm_guest_exec_lifecycle_ledger_v1_copy(&ledger)?5:0;
}