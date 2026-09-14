/* Private, synchronous resource transport binding. No pointer is wire data. */
#ifndef OPENNT_BASE_RESOURCE_H
#define OPENNT_BASE_RESOURCE_H
#include <nt.h>
typedef struct OPENNT_BASE_RESOURCE_BINDING {
    void *Context;
    NTSTATUS (*Duplicate)(void *,HANDLE,HANDLE,HANDLE,PHANDLE,ACCESS_MASK,ULONG,ULONG);
    NTSTATUS (*Close)(void *,HANDLE);
} OPENNT_BASE_RESOURCE_BINDING;
/* Trusted dispatch binds both operations and restores the prior scope.
 * It must preserve synchronous delivery and source-owned cleanup order. */
const OPENNT_BASE_RESOURCE_BINDING *OpenNtBaseBindResources(const OPENNT_BASE_RESOURCE_BINDING *);
NTSTATUS NTAPI OpenNtBaseDuplicateObject(HANDLE,HANDLE,HANDLE,PHANDLE,ACCESS_MASK,ULONG,ULONG);
NTSTATUS NTAPI OpenNtBaseClose(HANDLE);
#endif
