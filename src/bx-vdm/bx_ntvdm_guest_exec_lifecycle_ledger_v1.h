#ifndef BX_NTVDM_GUEST_EXEC_LIFECYCLE_LEDGER_V1_H
#define BX_NTVDM_GUEST_EXEC_LIFECYCLE_LEDGER_V1_H
#include "bx_ntvdm_generic_ud_bridge.h"
#include <stdint.h>
#define BX_NTVDM_GUEST_EXEC_LIFECYCLE_LEDGER_V1_MAGIC UINT32_C(0x42584547)
#define BX_NTVDM_GUEST_EXEC_LIFECYCLE_LEDGER_V1_VERSION UINT32_C(1)
#define BX_NTVDM_GUEST_EXEC_LIFECYCLE_LEDGER_V1_MAX_RECORDS 16u
enum bx_ntvdm_guest_exec_lifecycle_phase_v1 {
 BX_NTVDM_GUEST_EXEC_LIFECYCLE_PHASE_V1_ENTRY=1u,
 BX_NTVDM_GUEST_EXEC_LIFECYCLE_PHASE_V1_TERMINATE=2u,
 BX_NTVDM_GUEST_EXEC_LIFECYCLE_PHASE_V1_PARENT_RETURN=3u
};
/* Copied, default-off evidence only. A record identifies an accepted
 * selector/service event; it neither names a guest target nor reads guest RAM. */
struct bx_ntvdm_guest_exec_lifecycle_ledger_record_v1 {
 uint32_t phase; struct bx_ntvdm_generic_ud_event_v1 event;
 struct bx_ntvdm_generic_ud_outcome_v1 outcome;
};
struct bx_ntvdm_guest_exec_lifecycle_ledger_v1 {
 uint32_t magic, abi_version, struct_bytes; uint32_t record_count, overflowed;
 struct bx_ntvdm_guest_exec_lifecycle_ledger_record_v1
 records[BX_NTVDM_GUEST_EXEC_LIFECYCLE_LEDGER_V1_MAX_RECORDS];
};
#ifdef __cplusplus
extern "C" {
#endif
void bx_ntvdm_guest_exec_lifecycle_ledger_v1_enable(uint32_t enabled);
int bx_ntvdm_guest_exec_lifecycle_ledger_v1_copy(
 struct bx_ntvdm_guest_exec_lifecycle_ledger_v1 *value);
void bx_ntvdm_guest_exec_lifecycle_ledger_v1_consider(
 const struct bx_ntvdm_generic_ud_event_v1 *event,
 const struct bx_ntvdm_generic_ud_outcome_v1 *outcome);
#ifdef __cplusplus
}
#endif
#endif