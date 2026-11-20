#include "include/base_resource_scope.h"
#include "include/base_stream.h"

NTSTATUS OpenNtBaseDeliverScopedWait(void *context, HANDLE event, uint32_t *receipt)
{
    OPENNT_BASE_RESOURCE_SCOPE *scope = context;
    DWORD error;
    if (!scope || !event || !receipt) return (NTSTATUS)0xc000000dL;
    /* Original DOS records compare these IDs across the shared Console list. */
    if (scope->request->issued < *scope->next_wait)
        scope->request->issued = *scope->next_wait;
    error = broker_vdm_receipt_accept(scope->request, scope->role, event, receipt);
    if (!error) *scope->next_wait = *receipt;
    return error ? (NTSTATUS)0xc0000008L : 0;
}

static NTSTATUS revoke_wait(void *context, uint32_t receipt)
{
    OPENNT_BASE_RESOURCE_SCOPE *scope = context;
    if (!scope) return (NTSTATUS)0xc000000dL;
    return broker_vdm_receipt_revoke(scope->request, scope->generation, receipt)
        ? (NTSTATUS)0xc0000008L : 0;
}

static NTSTATUS deliver_stream(void *context, HANDLE stream, uint32_t *receipt)
{
    const OPENNT_BASE_RECEIPT_ROUTE *route = context;
    DWORD error;
    if (!stream || !receipt) return (NTSTATUS)0xc000000dL;
    if (!route->table) return (NTSTATUS)0xc0000008L;
    EnterCriticalSection(route->lock);
    error = broker_vdm_receipt_accept(route->table, BROKER_VDM_STDIN, stream, receipt);
    LeaveCriticalSection(route->lock);
    return error ? (NTSTATUS)0xc0000008L : 0;
}

static NTSTATUS duplicate_resource(void *context, HANDLE source_process,
    HANDLE source, HANDLE target_process, PHANDLE target, ACCESS_MASK access,
    ULONG attributes, ULONG options)
{
    OPENNT_BASE_RESOURCE_SCOPE *scope = context;
    if (!scope) return (NTSTATUS)0xc000000dL;

    /* The pseudo-handle selects the live worker retained by the issuer. */
    if (source == GetCurrentProcess() && target_process == GetCurrentProcess() &&
        target && !access && !attributes && options == DUPLICATE_SAME_ACCESS) {
        if (scope->worker && DuplicateHandle(GetCurrentProcess(), scope->worker,
                target_process, target, 0, FALSE, DUPLICATE_SAME_ACCESS))
            return 0;
        return (NTSTATUS)0xc0000022L;
    }

    if (source && target && !access && attributes == OBJ_INHERIT &&
        options == DUPLICATE_SAME_ACCESS) {
        OPENNT_BASE_RECEIPT_ROUTE *route = target_process && scope->resident.lock
            ? &scope->resident : &scope->reservation;
        if (route->lock) {
            OPENNT_BASE_STREAM_BINDING streams = {0};
            streams.source_process = source_process;
            streams.target_process = target_process;
            streams.source_receipts = scope->request;
            streams.source_generation = scope->generation;
            streams.context = route;
            streams.deliver = deliver_stream;
            return OpenNtBaseDuplicateStream(&streams, source_process, source,
                target_process, target, access, attributes, options);
        }
    }
    return OpenNtBaseDuplicateWait(&scope->wait, source_process, source,
        target_process, target, access, attributes, options);
}

static NTSTATUS close_resource(void *context, HANDLE handle)
{
    OPENNT_BASE_RESOURCE_SCOPE *scope = context;
    uint32_t receipt = (uint32_t)(ULONG_PTR)handle;
    const OPENNT_BASE_RECEIPT_ROUTE *route;
    if (!scope || !handle) return (NTSTATUS)0xc0000008L;
    if (handle == scope->wait.local_event) return OpenNtBaseCloseWait(&scope->wait, handle);
    route = &scope->reservation;
    if (receipt && route->table) {
        DWORD error;
        EnterCriticalSection(route->lock);
        error = broker_vdm_receipt_revoke(route->table, route->table->generation, receipt);
        LeaveCriticalSection(route->lock);
        if (!error) return 0;
    }
    if (!broker_vdm_receipt_revoke(scope->request, scope->generation, receipt))
        return 0;
    return CloseHandle(handle) ? 0 : (NTSTATUS)0xc0000008L;
}

void OpenNtBaseInitializeResourceScope(OPENNT_BASE_RESOURCE_SCOPE *scope,
    HANDLE process, broker_vdm_receipts *request, DWORD generation, DWORD role,
    ULONG *next_wait)
{
    ZeroMemory(scope, sizeof(*scope));
    scope->request = request;
    scope->generation = generation;
    scope->role = role;
    scope->next_wait = next_wait;
    scope->wait.target_process = process;
    scope->wait.context = scope;
    scope->wait.deliver = OpenNtBaseDeliverScopedWait;
    scope->wait.revoke = revoke_wait;
    scope->binding.Context = scope;
    scope->binding.Duplicate = duplicate_resource;
    scope->binding.Close = close_resource;
}
