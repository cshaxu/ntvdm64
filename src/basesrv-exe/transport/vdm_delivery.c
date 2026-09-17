/* Only tracks pending resource acquisitions; original BaseSrv owns commands.
 * Mixed source/destination stream fields cannot safely serve as this journal. */
#include "vdm_delivery.h"
struct broker_vdm_delivery_item {
    struct broker_vdm_delivery_item *next;
    void *recipient;
    broker_vdm_revoke_receipt revoke;
    uint32_t generation,receipt;
};
DWORD broker_vdm_delivery_prepare(broker_vdm_delivery *state,void *recipient,
    broker_vdm_revoke_receipt revoke,broker_vdm_delivery_item **ticket)
{
    broker_vdm_delivery_item *entry;
    if (!ticket) return ERROR_INVALID_PARAMETER;
    *ticket=NULL;
    if (!state || !revoke) return ERROR_INVALID_PARAMETER;
    entry=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*entry));
    if (!entry) return ERROR_NOT_ENOUGH_MEMORY;
    entry->recipient=recipient; entry->revoke=revoke;
    entry->next=state->pending; state->pending=entry; *ticket=entry;
    return ERROR_SUCCESS;
}
DWORD broker_vdm_delivery_acknowledge(broker_vdm_delivery_item *ticket,uint32_t generation,uint32_t receipt)
{
    if (!ticket || !generation || !receipt || ticket->receipt) return ERROR_INVALID_PARAMETER;
    ticket->generation=generation; ticket->receipt=receipt;
    return ERROR_SUCCESS;
}
DWORD broker_vdm_delivery_forget(broker_vdm_delivery *state,broker_vdm_delivery_item *ticket)
{
    broker_vdm_delivery_item **link;
    if (!state || !ticket) return ERROR_INVALID_PARAMETER;
    for (link=&state->pending;*link;link=&(*link)->next) if (*link==ticket) {
        *link=ticket->next; HeapFree(GetProcessHeap(),0,ticket); return ERROR_SUCCESS;
    }
    return ERROR_NOT_FOUND;
}
DWORD broker_vdm_delivery_commit(broker_vdm_delivery *state)
{
    broker_vdm_delivery_item *entry;
    if (!state) return ERROR_INVALID_PARAMETER;
    for (entry=state->pending;entry;entry=entry->next) if (!entry->receipt) return ERROR_IO_PENDING;
    while (state->pending) broker_vdm_delivery_forget(state,state->pending);
    return ERROR_SUCCESS;
}
DWORD broker_vdm_delivery_rollback(broker_vdm_delivery *state)
{
    broker_vdm_delivery_item **link,*entry;
    DWORD result=ERROR_SUCCESS,error;
    if (!state) return ERROR_INVALID_PARAMETER;
    for (link=&state->pending;*link;) {
        entry=*link;
        error=entry->receipt?entry->revoke(entry->recipient,entry->generation,entry->receipt):ERROR_IO_PENDING;
        if (!error) { *link=entry->next; HeapFree(GetProcessHeap(),0,entry); }
        else { if (!result) result=error; link=&entry->next; }
    }
    return result;
}
