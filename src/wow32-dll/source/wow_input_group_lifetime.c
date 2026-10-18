#include "wow_input_group_lifetime.h"

/* ADAPTER-WOW-084: original input.c computes connected membership, not modern
 * USER queue storage. Its Q identities are scratch component labels here.
 * Actual input/focus/capture and native thread death belong to USER32. */
typedef struct wow_input_native_link {
    struct wow_input_native_link *next;
    wow_task_order_thread *first, *second;
} wow_input_native_link;

typedef struct input_plan_row {
    wow_task_order_thread *thread;
    wow_task_order_queue label;
    ULONG group;
} input_plan_row;

struct wow_input_group_plan {
    wow_input_group_owner *owner;
    input_plan_row *rows;
    ULONG count;
    wow_input_native_link *reserved;
};

static wow_task_order_queue *WINAPI label_allocate(wow_input_desktop *desktop)
{
    (void)desktop;
    /* Production calls original Recalc2, not the private native-Q allocation
     * strategy in RecalcThreadAttachment. Every label is already reserved. */
    wow_user_native_call_failed("input-unplanned-label", ERROR_INVALID_STATE);
}

static void WINAPI label_release(wow_task_order_queue *label,
    wow_task_order_thread *thread)
{
    (void)thread;
    if (!label || !label->cThreads)
        wow_user_native_call_failed("input-label-reference", ERROR_INVALID_STATE);
    --label->cThreads; /* Scratch storage is owned by the complete plan. */
}

void WINAPI wow_input_group_initialize(wow_input_group_owner *owner,
    wow_input_desktop *desktop)
{
    ZeroMemory(owner, sizeof(*owner));
    owner->desktop = desktop;
    desktop->allocate_queue = label_allocate;
    desktop->destroy_queue = label_release;
}

static BOOL linked(wow_input_group_owner *owner, wow_task_order_thread *a,
    wow_task_order_thread *b)
{
    wow_input_native_link *link;
    for (link = owner->links; link; link = link->next)
        if ((link->first == a && link->second == b) ||
                (link->first == b && link->second == a)) return TRUE;
    return FALSE;
}

void WINAPI wow_input_group_release_plan(wow_input_group_plan *plan)
{
    DWORD error = GetLastError();
    wow_input_native_link *link;
    if (!plan) return;
    while ((link = plan->reserved) != NULL) {
        plan->reserved = link->next;
        HeapFree(GetProcessHeap(), 0, link);
    }
    if (plan->rows) HeapFree(GetProcessHeap(), 0, plan->rows);
    HeapFree(GetProcessHeap(), 0, plan);
    SetLastError(error);
}

static BOOL plan_contains(const wow_input_group_plan *plan,
    const wow_task_order_thread *thread)
{
    ULONG i;
    for (i = 0; i < plan->count; ++i)
        if (plan->rows[i].thread == thread) return TRUE;
    return FALSE;
}

wow_input_group_plan *WINAPI wow_input_group_prepare(wow_input_group_owner *owner)
{
    LIST_ENTRY *head = &owner->desktop->PtiList, *entry;
    ULONG count = 0, i, j;
    DWORD saved = GetLastError();
    wow_input_group_plan *plan;
    /* All fallible plan allocation precedes original InitTask/TDB commit.
     * Do not introduce an ignored BOOL failure inside original reattachment. */
    for (entry = head->Flink; entry != head; entry = entry->Flink) {
        if (count == MAXDWORD / sizeof(input_plan_row)) {
            SetLastError(ERROR_ARITHMETIC_OVERFLOW); return NULL;
        }
        ++count;
    }
    if (!count) { SetLastError(ERROR_INVALID_STATE); return NULL; }
    plan = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*plan));
    if (!plan) goto no_memory;
    plan->rows = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        count * sizeof(*plan->rows));
    if (!plan->rows) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto failed; }
    plan->count = count;
    plan->owner = owner;
    for (entry = head->Flink, i = 0; entry != head; entry = entry->Flink, ++i) {
        wow_task_order_thread *thread = CONTAINING_RECORD(entry,
            wow_task_order_thread, PtiLink);
        if (thread->pq || thread->pqAttach || !thread->native_thread ||
                GetProcessIdOfThread(thread->native_thread) != GetCurrentProcessId() ||
                GetThreadId(thread->native_thread) != thread->native_thread_id ||
                WaitForSingleObject(thread->native_thread, 0) != WAIT_TIMEOUT) {
            SetLastError(ERROR_INVALID_STATE); goto failed;
        }
        for (j = 0; j < i; ++j)
            if (plan->rows[j].thread->native_thread_id == thread->native_thread_id) {
                SetLastError(ERROR_INVALID_STATE); goto failed;
            }
        plan->rows[i].thread = thread;
    }
    {
        wow_input_attachment *edge;
        wow_input_native_link *link;
        for (edge = owner->desktop->attachments; edge; edge = edge->paiNext)
            if (!plan_contains(plan, edge->pti1) || !plan_contains(plan, edge->pti2)) {
                SetLastError(ERROR_INVALID_STATE); goto failed;
            }
        for (link = owner->links; link; link = link->next)
            if (!plan_contains(plan, link->first) || !plan_contains(plan, link->second)) {
                SetLastError(ERROR_INVALID_STATE); goto failed;
            }
    }
    for (i = 0; i < count; ++i) for (j = i + 1u; j < count; ++j) {
        wow_input_native_link *link;
        if (linked(owner, plan->rows[i].thread, plan->rows[j].thread)) continue;
        link = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*link));
        if (!link) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto failed; }
        link->first = plan->rows[i].thread;
        link->second = plan->rows[j].thread;
        link->next = plan->reserved; plan->reserved = link;
    }
    SetLastError(saved);
    return plan;
failed:
    {
        DWORD error = GetLastError();
        wow_input_group_release_plan(plan);
        SetLastError(error ? error : ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }
no_memory:
    SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL;
}

static BOOL same_group(const wow_input_group_plan *plan,
    wow_task_order_thread *a, wow_task_order_thread *b)
{
    ULONG i, first = 0, second = 0;
    for (i = 0; i < plan->count; ++i) {
        if (plan->rows[i].thread == a) first = plan->rows[i].group;
        if (plan->rows[i].thread == b) second = plan->rows[i].group;
    }
    return first && first == second;
}

static void detach_link(wow_input_native_link *link)
{
    /* A confirmed dead native thread has no remaining USER attachments. Do
     * not address a recycled numeric TID in that case. Handles are owned by
     * the actual task carriers and remain open until after link retirement. */
    if (WaitForSingleObject(link->first->native_thread, 0) == WAIT_OBJECT_0 ||
            WaitForSingleObject(link->second->native_thread, 0) == WAIT_OBJECT_0) return;
    if (!AttachThreadInput(link->first->native_thread_id,
            link->second->native_thread_id, FALSE))
        wow_user_native_call_failed("input-native-detach", GetLastError());
}

void WINAPI wow_input_group_commit(wow_input_group_owner *owner,
    wow_input_group_plan *plan, wow_task_order_thread *current)
{
    ULONG i, j;
    DWORD saved = GetLastError();
    wow_input_native_link **at, *link;
    if (!plan || plan->owner != owner || !current ||
            current->rpdesk != owner->desktop || !plan_contains(plan, current))
        wow_user_native_call_failed("input-plan-owner", ERROR_INVALID_STATE);
    /* Same original fixed point and TIF_16BIT/explicit-edge policy. The old
     * RecalcThreadAttachment wrapper chooses private kernel Q storage; that
     * storage is not represented as another authoritative native queue. */
    for (i = 0; i < plan->count; ++i) if (!plan->rows[i].thread->pqAttach) {
        BOOL changed = FALSE;
        AddAttachment(plan->rows[i].thread, &plan->rows[i].label, &changed);
        Recalc2(&plan->rows[i].label, current);
    }
    for (i = 0; i < plan->count; ++i) {
        for (j = 0; j < plan->count; ++j)
            if (plan->rows[i].thread->pqAttach == &plan->rows[j].label) break;
        if (j == plan->count)
            wow_user_native_call_failed("input-component-label", ERROR_INVALID_STATE);
        plan->rows[i].group = j + 1u;
    }
    /* No scratch pointer escapes into a later callback or task retirement. */
    for (i = 0; i < plan->count; ++i) plan->rows[i].thread->pqAttach = NULL;
    at = &plan->reserved;
    while ((link = *at) != NULL) {
        if (!same_group(plan, link->first, link->second)) { at = &link->next; continue; }
        if (!AttachThreadInput(link->first->native_thread_id,
                link->second->native_thread_id, TRUE))
            wow_user_native_call_failed("input-native-attach", GetLastError());
        *at = link->next;
        link->next = owner->links; owner->links = link;
    }
    /* Add before subtract: every surviving pair is explicit. No fragile
     * root/chain, repeat attachment, application-owned detach or queue replay. */
    for (at = &owner->links; (link = *at) != NULL;) {
        if (same_group(plan, link->first, link->second)) { at = &link->next; continue; }
        detach_link(link); *at = link->next; HeapFree(GetProcessHeap(), 0, link);
    }
    SetLastError(saved);
}

void WINAPI wow_input_group_retire(wow_input_group_owner *owner,
    wow_task_order_thread *thread)
{
    wow_input_native_link **at = &owner->links, *link;
    DWORD saved = GetLastError();
    while ((link = *at) != NULL) {
        if (link->first != thread && link->second != thread) { at = &link->next; continue; }
        detach_link(link); *at = link->next; HeapFree(GetProcessHeap(), 0, link);
    }
    SetLastError(saved);
}
