#include "bx_ntvdm_guest_exec_lifecycle_ledger_v1.h"
#include <string.h>
static uint32_t enabled;
static struct bx_ntvdm_guest_exec_lifecycle_ledger_v1 ledger;
static void clear(void) {
 memset(&ledger, 0, sizeof(ledger));
 ledger.magic = BX_NTVDM_GUEST_EXEC_LIFECYCLE_LEDGER_V1_MAGIC;
 ledger.abi_version = BX_NTVDM_GUEST_EXEC_LIFECYCLE_LEDGER_V1_VERSION;
 ledger.struct_bytes = (uint32_t)sizeof(ledger);
}
static uint32_t phase_for(const struct bx_ntvdm_generic_ud_event_v1 *event,
 const struct bx_ntvdm_generic_ud_outcome_v1 *outcome) {
 if (!event || !outcome || event->magic != BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC ||
 event->abi_version != BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION ||
 event->struct_bytes != sizeof(*event) || event->vector != 6u ||
 event->fault_rip > UINT64_MAX - 4u || event->window_bytes < 4u ||
 event->window[0] != 0xc4u || event->window[1] != 0xc4u ||
 outcome->abi_version != BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION ||
 outcome->disposition != BX_NTVDM_GENERIC_UD_RESUME ||
 outcome->resume_rip != event->fault_rip + 4u) return 0u;
 if (event->window[2] == 0x50u && event->window[3] == 0x36u)
 return BX_NTVDM_GUEST_EXEC_LIFECYCLE_PHASE_V1_ENTRY;
 if (event->window[2] == 0x50u && event->window[3] == 0x3cu)
 return BX_NTVDM_GUEST_EXEC_LIFECYCLE_PHASE_V1_TERMINATE;
 if (event->window[2] == 0x54u && event->window[3] == 0x0bu)
 return BX_NTVDM_GUEST_EXEC_LIFECYCLE_PHASE_V1_PARENT_RETURN;
 return 0u;
}
void bx_ntvdm_guest_exec_lifecycle_ledger_v1_enable(uint32_t value) {
 enabled = value == 1u; clear();
}
int bx_ntvdm_guest_exec_lifecycle_ledger_v1_copy(
 struct bx_ntvdm_guest_exec_lifecycle_ledger_v1 *value) {
 if (!value || !enabled || ledger.record_count == 0u) return 0;
 *value = ledger; return 1;
}
void bx_ntvdm_guest_exec_lifecycle_ledger_v1_consider(
 const struct bx_ntvdm_generic_ud_event_v1 *event,
 const struct bx_ntvdm_generic_ud_outcome_v1 *outcome) {
 uint32_t phase; struct bx_ntvdm_guest_exec_lifecycle_ledger_record_v1 *record;
 if (!enabled || !(phase = phase_for(event, outcome))) return;
 if (ledger.record_count >= BX_NTVDM_GUEST_EXEC_LIFECYCLE_LEDGER_V1_MAX_RECORDS) {
 ledger.overflowed = 1u; return;
 }
 record = &ledger.records[ledger.record_count++]; record->phase = phase;
 record->event = *event; record->outcome = *outcome;
}