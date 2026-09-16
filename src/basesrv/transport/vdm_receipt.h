/* Receiver-local VDM resource receipts, never native values on the wire.
 * The authenticated owner serializes operations and resource use/revocation.
 * This storage does not authenticate peers or implement task selection. */
#ifndef BROKER_VDM_RECEIPT_H
#define BROKER_VDM_RECEIPT_H
#include <windows.h>
#include <stdint.h>
enum broker_vdm_receipt_role {
    BROKER_VDM_STDIN=1, BROKER_VDM_STDOUT, BROKER_VDM_STDERR,
    BROKER_VDM_PARENT_WAIT, BROKER_VDM_WORKER_WAIT
};
typedef struct broker_vdm_receipt_entry broker_vdm_receipt_entry;
typedef struct broker_vdm_receipts {
    uint32_t generation, issued;
    broker_vdm_receipt_entry *entries;
} broker_vdm_receipts;
/* Generation is supplied by trusted registration, not command payload. */
DWORD broker_vdm_receipts_initialize(broker_vdm_receipts *, uint32_t generation);
/* Input type must already be validated by the typed OS attachment endpoint.
 * Console resources are not admissible through these file/event operations.
 * Original standard-stream roles retain OBJ_INHERIT; wait roles do not. */
DWORD broker_vdm_receipt_accept(broker_vdm_receipts *, uint32_t role, HANDLE, uint32_t *id);
/* Borrowed native reference, valid until revoke/drain. Owner must not revoke
 * during its use. Neither the pointer nor this HANDLE is a wire field. */
/* Expected role is supplied by the decoded operation, not by the sender's
 * receipt claim. Stream aliases share a family; wait roles remain distinct. */
DWORD broker_vdm_receipt_resolve(broker_vdm_receipts *, uint32_t generation, uint32_t id, uint32_t expected_role, HANDLE *);
/* Transfer the retained reference, removing it WITHOUT closing/duplicating it.
 * Use only after all response validation and successful ownership publication.
 * Wait retries borrow via resolve; final original close takes ownership once.
 * Group aliases by receipt before taking: one ID yields one owned HANDLE.
 * Failure leaves the receipt intact and sets output NULL. Caller owns success. */
DWORD broker_vdm_receipt_take(broker_vdm_receipts *,uint32_t generation,uint32_t id,uint32_t expected_role,HANDLE *);
DWORD broker_vdm_receipt_revoke(broker_vdm_receipts *, uint32_t generation, uint32_t id);
void broker_vdm_receipts_drain(broker_vdm_receipts *);
#endif
