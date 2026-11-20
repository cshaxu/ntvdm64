#ifndef OPENNT_BASE_RESOURCE_SCOPE_H
#define OPENNT_BASE_RESOURCE_SCOPE_H

#include "base_wait.h"
#include "vdm_receipt.h"

typedef struct OPENNT_BASE_RECEIPT_ROUTE {
    /* A lock marks a selected reservation even if the table is absent. */
    CRITICAL_SECTION *lock;
    broker_vdm_receipts *table;
} OPENNT_BASE_RECEIPT_ROUTE;

typedef struct OPENNT_BASE_RESOURCE_SCOPE {
    OPENNT_BASE_RESOURCE_BINDING binding;
    OPENNT_BASE_WAIT_BINDING wait;
    HANDLE worker;
    broker_vdm_receipts *request;
    OPENNT_BASE_RECEIPT_ROUTE reservation, resident;
    ULONG *next_wait;
    DWORD generation, role;
} OPENNT_BASE_RESOURCE_SCOPE;

/* The issuer pins the selected stores and holds its dispatch lock until unbind. */
void OpenNtBaseInitializeResourceScope(OPENNT_BASE_RESOURCE_SCOPE *, HANDLE,
    broker_vdm_receipts *, DWORD generation, DWORD role, ULONG *next_wait);
NTSTATUS OpenNtBaseDeliverScopedWait(void *, HANDLE, uint32_t *);

#endif
