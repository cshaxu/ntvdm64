#include "mvdm_vm_wake.h"
#include "mvdm_vm_notification.h"

extern void host_ica_lock(void);
extern void host_ica_unlock(void);
typedef struct vm_entry {
    struct vm_entry *next;
    mvdm_vm_wake_identity identity;
} vm_entry;
typedef struct wake_entry {
    struct wake_entry *next;
    mvdm_vm_wake_identity identity;
} wake_entry;
static vm_entry *vms;
static wake_entry *first, **tail = &first;

static vm_entry **find_vm(uint32_t vm)
{
    vm_entry **p;
    for (p = &vms; *p && (*p)->identity.vm != vm; p = &(*p)->next) {}
    return p;
}

uint32_t mvdm_vm_wake_register(uint32_t vm, uint64_t generation)
{
    vm_entry *entry;
    uint64_t serial;
    DWORD error = ERROR_SUCCESS;
    if (!vm || !generation) return ERROR_INVALID_PARAMETER;
    entry = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*entry));
    if (!entry) return ERROR_NOT_ENOUGH_MEMORY;
    entry->identity.vm = vm;
    entry->identity.generation = generation;
    host_ica_lock();
    error = mvdm_vm_notification_snapshot(&serial);
    if (!error) {
        if (*find_vm(vm)) error = ERROR_ALREADY_EXISTS;
        else { entry->next = vms; vms = entry; entry = NULL; }
    }
    host_ica_unlock();
    if (entry) HeapFree(GetProcessHeap(), 0, entry);
    return error;
}

uint32_t mvdm_vm_wake_retire(uint32_t vm, uint64_t generation)
{
    vm_entry **found, *entry = NULL;
    wake_entry **p;
    DWORD error = ERROR_NOT_FOUND;
    host_ica_lock();
    found = find_vm(vm);
    if (*found && (*found)->identity.generation == generation) {
        entry = *found;
        *found = entry->next;
        error = ERROR_SUCCESS;
        for (p = &first; *p;) {
            wake_entry *wake = *p;
            if (wake->identity.vm == vm && wake->identity.generation == generation) {
                *p = wake->next;
                HeapFree(GetProcessHeap(), 0, wake);
            } else p = &wake->next;
        }
        tail = p;
    }
    host_ica_unlock();
    if (entry) HeapFree(GetProcessHeap(), 0, entry);
    return error;
}

uint32_t mvdm_vm_wake_lookup(uint32_t vm, mvdm_vm_wake_identity *identity)
{
    vm_entry *entry;
    if (!identity) return ERROR_INVALID_PARAMETER;
    ZeroMemory(identity, sizeof(*identity));
    host_ica_lock();
    entry = *find_vm(vm);
    if (entry) *identity = entry->identity;
    host_ica_unlock();
    return entry ? ERROR_SUCCESS : ERROR_NOT_FOUND;
}

uint32_t mvdm_vm_wake_publish(uint32_t vm, uint64_t generation, uint64_t *serial)
{
    vm_entry **found;
    wake_entry *wake;
    DWORD error = ERROR_NOT_FOUND;
    if (!serial) return ERROR_INVALID_PARAMETER;
    *serial = 0;
    wake = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*wake));
    if (!wake) return ERROR_NOT_ENOUGH_MEMORY;
    wake->identity.vm = vm;
    wake->identity.generation = generation;
    host_ica_lock();
    found = find_vm(vm);
    if (*found && (*found)->identity.generation == generation) {
        /* Posting only raises a CCPU interrupt flag; guest code cannot run
         * inside this native call or before the ICA lock is released. */
        error = mvdm_vm_notification_post(serial);
        if (!error) { *tail = wake; tail = &wake->next; wake = NULL; }
    }
    host_ica_unlock();
    if (wake) HeapFree(GetProcessHeap(), 0, wake);
    return error;
}

uint32_t mvdm_vm_wake_take(mvdm_vm_wake_identity *identity)
{
    wake_entry *wake;
    if (!identity) return ERROR_INVALID_PARAMETER;
    ZeroMemory(identity, sizeof(*identity));
    host_ica_lock();
    wake = first;
    if (wake) {
        first = wake->next;
        if (!first) tail = &first;
        *identity = wake->identity;
    }
    host_ica_unlock();
    if (!wake) return ERROR_NO_MORE_ITEMS;
    HeapFree(GetProcessHeap(), 0, wake);
    return ERROR_SUCCESS;
}

void mvdm_vm_wake_reset(void)
{
    host_ica_lock();
    while (first) {
        wake_entry *next = first->next;
        HeapFree(GetProcessHeap(), 0, first);
        first = next;
    }
    tail = &first;
    while (vms) {
        vm_entry *next = vms->next;
        HeapFree(GetProcessHeap(), 0, vms);
        vms = next;
    }
    host_ica_unlock();
}
