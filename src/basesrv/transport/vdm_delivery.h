/* Sender-side pending receipt cleanup. Local state, not a wire record. */
#ifndef BROKER_VDM_DELIVERY_H
#define BROKER_VDM_DELIVERY_H
#include <windows.h>
#include <stdint.h>
typedef DWORD (*broker_vdm_revoke_receipt)(void *,uint32_t,uint32_t);
typedef struct broker_vdm_delivery_item broker_vdm_delivery_item;
typedef struct broker_vdm_delivery { broker_vdm_delivery_item *pending; } broker_vdm_delivery;
/* Zero-initialize owner state. Reserve bookkeeping BEFORE remote acquisition.
 * Recipient context must remain alive until commit or successful rollback. */
DWORD broker_vdm_delivery_prepare(broker_vdm_delivery *,void *,broker_vdm_revoke_receipt,broker_vdm_delivery_item **);
DWORD broker_vdm_delivery_acknowledge(broker_vdm_delivery_item *,uint32_t generation,uint32_t receipt);
/* Only after definitive non-delivery, or confirmed recipient rundown. Never
 * use forget for a missing/indeterminate RPC reply from a live recipient. */
DWORD broker_vdm_delivery_forget(broker_vdm_delivery *,broker_vdm_delivery_item *);
/* Commit only at original successful ownership/publication boundary. */
DWORD broker_vdm_delivery_commit(broker_vdm_delivery *);
/* Retryable failures and unknown receipts remain owned by the journal. */
DWORD broker_vdm_delivery_rollback(broker_vdm_delivery *);
#endif
