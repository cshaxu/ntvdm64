/* Finite receiver retention below original BaseSrv duplication/release calls.
 * CSR target-local HANDLE delivery cannot cross the admitted wire boundary.
 * IDs name only this authenticated generation's standard-stream/wait receipts. */
#include "vdm_receipt.h"
struct broker_vdm_receipt_entry {
    struct broker_vdm_receipt_entry *next;
    uint32_t id, role;
    HANDLE resource;
};
DWORD broker_vdm_receipts_initialize(broker_vdm_receipts *state, uint32_t generation)
{
    if (!state || !generation) return ERROR_INVALID_PARAMETER;
    state->generation=generation; state->issued=0; state->entries=NULL;
    return ERROR_SUCCESS;
}
DWORD broker_vdm_receipt_accept(broker_vdm_receipts *state, uint32_t role, HANDLE resource, uint32_t *id)
{
    broker_vdm_receipt_entry *entry;
    DWORD mode, error;
    if (!id) return ERROR_INVALID_PARAMETER;
    *id=0;
    if (!state || !state->generation || role<BROKER_VDM_STDIN || role>BROKER_VDM_WORKER_WAIT)
        return ERROR_INVALID_PARAMETER;
    if (role<=BROKER_VDM_STDERR && GetConsoleMode(resource,&mode)) return ERROR_NOT_SUPPORTED;
    /* BaseSrv stores parent/worker waits in a HANDLE-shaped field and its
     * GetVDMExitCode path masks bit zero as an original tag.  A remote wait
     * receipt must therefore reserve that bit too; ordinary stream receipts
     * have no such source-level constraint. */
    if (state->issued==UINT32_MAX ||
        (role>=BROKER_VDM_PARENT_WAIT && state->issued>=UINT32_MAX-1))
        return ERROR_ARITHMETIC_OVERFLOW;
    entry=HeapAlloc(GetProcessHeap(),0,sizeof(*entry));
    if (!entry) return ERROR_NOT_ENOUGH_MEMORY;
    /* Original BaseSrvDupStandardHandles requests OBJ_INHERIT, whereas
     * BaseSrvCreatePairWaitHandles requests non-inheritable wait references. */
    if (!DuplicateHandle(GetCurrentProcess(),resource,GetCurrentProcess(),&entry->resource,
        0,role<=BROKER_VDM_STDERR,DUPLICATE_SAME_ACCESS)) {
        error=GetLastError(); HeapFree(GetProcessHeap(),0,entry); return error;
    }
    entry->id=++state->issued;
    if (role>=BROKER_VDM_PARENT_WAIT && (entry->id&1)) entry->id=++state->issued;
    entry->role=role;
    entry->next=state->entries; state->entries=entry;
    *id=entry->id;
    return ERROR_SUCCESS;
}
DWORD broker_vdm_receipt_resolve(broker_vdm_receipts *state, uint32_t generation, uint32_t id, uint32_t expected_role, HANDLE *resource)
{
    broker_vdm_receipt_entry *entry;
    if (!resource) return ERROR_INVALID_PARAMETER;
    *resource=NULL;
    if (expected_role<BROKER_VDM_STDIN || expected_role>BROKER_VDM_WORKER_WAIT) return ERROR_INVALID_PARAMETER;
    if (!state || !generation || generation!=state->generation) return ERROR_ACCESS_DENIED;
    for (entry=state->entries;entry;entry=entry->next) if (entry->id==id) {
        if (entry->role!=expected_role &&
            !(entry->role<=BROKER_VDM_STDERR && expected_role<=BROKER_VDM_STDERR)) return ERROR_ACCESS_DENIED;
        *resource=entry->resource; return ERROR_SUCCESS;
    }
    return ERROR_NOT_FOUND;
}
DWORD broker_vdm_receipt_take(broker_vdm_receipts *state,uint32_t generation,uint32_t id,uint32_t role,HANDLE *resource)
{
    broker_vdm_receipt_entry **link,*entry;
    HANDLE borrowed;
    DWORD error;
    if (!resource) return ERROR_INVALID_PARAMETER;
    *resource=NULL;
    error=broker_vdm_receipt_resolve(state,generation,id,role,&borrowed);
    if (error) return error;
    for (link=&state->entries;*link;link=&(*link)->next) if ((*link)->id==id) {
        entry=*link; *link=entry->next;
        *resource=borrowed;
        HeapFree(GetProcessHeap(),0,entry);
        return ERROR_SUCCESS;
    }
    return ERROR_NOT_FOUND;
}
DWORD broker_vdm_receipt_revoke(broker_vdm_receipts *state, uint32_t generation, uint32_t id)
{
    broker_vdm_receipt_entry **link, *entry;
    if (!state || !generation || generation!=state->generation) return ERROR_ACCESS_DENIED;
    if (!id || id>state->issued) return ERROR_NOT_FOUND;
    for (link=&state->entries;*link;link=&(*link)->next) if ((*link)->id==id) {
        entry=*link; *link=entry->next;
        CloseHandle(entry->resource); HeapFree(GetProcessHeap(),0,entry); break;
    }
    return ERROR_SUCCESS; /* Repeated revocation of an issued ID is harmless. */
}
void broker_vdm_receipts_drain(broker_vdm_receipts *state)
{
    if (!state) return;
    while (state->entries) broker_vdm_receipt_revoke(state,state->generation,state->entries->id);
    state->generation=0;
}
