#ifndef OPENNT_BASE_STREAM_H
#define OPENNT_BASE_STREAM_H
#include <base_resource.h>
#include "basesrv/transport/vdm_receipt.h"
/* Trusted synchronous scope; process handles and receipt ownership are pinned
 * by the service. These are source-shaped local carriers, never native values
 * serialized as command resources. Deliver must journal/ack before success. */
typedef struct OPENNT_BASE_STREAM_BINDING {
    HANDLE source_process,target_process;
    broker_vdm_receipts *source_receipts;
    uint32_t source_generation;
    void *context;
    NTSTATUS (*deliver)(void *,HANDLE,uint32_t *);
    NTSTATUS (*revoke)(void *,uint32_t);
} OPENNT_BASE_STREAM_BINDING;
/* Only original standard-stream duplicate/close operations are admitted.
 * Wait events and worker process handles need their separate bindings. */
NTSTATUS OpenNtBaseDuplicateStream(void *,HANDLE,HANDLE,HANDLE,PHANDLE,ACCESS_MASK,ULONG,ULONG);
#endif
