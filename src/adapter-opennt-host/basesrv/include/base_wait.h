#ifndef OPENNT_BASE_WAIT_H
#define OPENNT_BASE_WAIT_H
#include <base_resource.h>
#include <stdint.h>
/* One original wait-pair scope. Local event stays owned by the original
 * record; remote delivery must be authenticated and journalled by the caller.
 * For later close, initialize local_event from that original record. */
typedef struct OPENNT_BASE_WAIT_BINDING {
    HANDLE target_process,local_event;
    void *context;
    NTSTATUS (*deliver)(void *,HANDLE,uint32_t *);
    NTSTATUS (*revoke)(void *,uint32_t);
} OPENNT_BASE_WAIT_BINDING;
NTSTATUS OpenNtBaseDuplicateWait(void *,HANDLE,HANDLE,HANDLE,PHANDLE,ACCESS_MASK,ULONG,ULONG);
NTSTATUS OpenNtBaseCloseWait(void *,HANDLE);
#endif
