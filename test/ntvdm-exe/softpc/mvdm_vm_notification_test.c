#include "mvdm_vm_notification.h"
#include "mvdm_vm_bridge.h"
#include "mvdm_vm_wake.h"
#include <stdio.h>
#include <stdlib.h>

/* Unit boundary substitutes only: these do not prove real PIC/guest delivery. */
static void (*eoi)(int, int);
static HANDLE owner;
static CRITICAL_SECTION ica;
static int depth, raises, cancels, busy;
static LONG assertions;
#define CHECK(x) do { InterlockedIncrement(&assertions); if (!(x)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #x); exit(1); } } while (0)
void host_ica_lock(void) { EnterCriticalSection(&ica); ++depth; }
void host_ica_unlock(void) { CHECK(depth > 0); --depth; LeaveCriticalSection(&ica); }
WORD VDDReserveIrqLine(HANDLE h, WORD irq)
{
    CHECK(depth && irq == 0xffff && h);
    if (busy || owner) return 0xffff;
    owner = h;
    return 15;
}
BOOL VDDReleaseIrqLine(HANDLE h, WORD irq)
{
    CHECK(depth && owner == h && irq == 15);
    owner = NULL;
    return TRUE;
}
BOOL RegisterEOIHook(int irq, void (*proc)(int, int))
{
    CHECK(depth && irq == 15);
    if (eoi) return FALSE;
    eoi = proc;
    return TRUE;
}
BOOL RemoveEOIHook(int irq, void (*proc)(int, int))
{
    CHECK(depth && irq == 15 && eoi == proc);
    eoi = NULL;
    return TRUE;
}
void ica_hw_interrupt(ULONG adapter, ULONG line, LONG count)
{
    CHECK(depth && owner && adapter == 1 && line == 7 && count == 1);
    ++raises;
}
void ica_hw_interrupt_cancel(ULONG adapter, ULONG line)
{
    CHECK(depth && adapter == 1 && line == 7);
    ++cancels;
    eoi(15, -1);
}
static void occupied(int irq, int count) { (void)irq; (void)count; }
typedef struct producer_context {
    HANDLE published, resume;
    DWORD first_error, retired_error, reused_error;
} producer_context;
static DWORD WINAPI publish_from_host(void *argument)
{
    producer_context *p = argument;
    uint64_t serial;
    p->first_error = mvdm_vm_wake_publish(3, 30, &serial);
    SetEvent(p->published);
    if (WaitForSingleObject(p->resume, INFINITE) != WAIT_OBJECT_0)
        return ERROR_INVALID_HANDLE;
    p->retired_error = mvdm_vm_wake_publish(3, 30, &serial);
    p->reused_error = mvdm_vm_wake_publish(3, 31, &serial);
    return ERROR_SUCCESS;
}
int main(void)
{
    WORD irq;
    uint64_t first, second, latest;
    mvdm_vm_bridge_result result = {123, 456, 789, 0};
    mvdm_vm_wake_identity wake;
    producer_context producer = {0};
    HANDLE thread;
    DWORD thread_result;
    InitializeCriticalSection(&ica);
    CHECK(!mvdm_vm_bridge_bop(0, 0, 0, 0, 0, 1, &result));
    CHECK(result.status == 123 && result.value == 456);
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 0, 0, 0, 0, 0, &result));
    CHECK(result.status == ERROR_ACCESS_DENIED && !owner);
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 99, 0, 0, 0, 1, &result));
    CHECK(result.status == ERROR_INVALID_FUNCTION && !owner);
    CHECK(mvdm_vm_notification_post(&first) == ERROR_NOT_READY && !first);
    CHECK(mvdm_vm_wake_register(1, 1) == ERROR_NOT_READY);
    busy = 1;
    CHECK(mvdm_vm_notification_open(&irq) == ERROR_BUSY && irq == 0xffff);
    busy = 0;
    eoi = occupied;
    CHECK(mvdm_vm_notification_open(&irq) == ERROR_BUSY && !owner);
    eoi = NULL;
    CHECK(!mvdm_vm_notification_open(&irq) && irq == 15);
    CHECK(mvdm_vm_notification_open(&irq) == ERROR_ALREADY_EXISTS);
    CHECK(!mvdm_vm_notification_post(&first) && raises == 0);
    CHECK(!mvdm_vm_notification_ready() && raises == 1);
    CHECK(!mvdm_vm_notification_post(&second) && second > first && raises == 1);
    eoi(15, -1);
    CHECK(raises == 2); /* reset must not lose unacknowledged work */
    CHECK(!mvdm_vm_notification_snapshot(&latest) && latest == second);
    CHECK(mvdm_vm_notification_acknowledge(second + 1) == ERROR_INVALID_DATA);
    CHECK(!mvdm_vm_notification_acknowledge(first));
    eoi(15, 0);
    CHECK(raises == 3); /* old ack cannot consume the newer post */
    CHECK(!mvdm_vm_notification_acknowledge(second));
    eoi(15, 0);
    CHECK(raises == 3);
    CHECK(mvdm_vm_notification_acknowledge(first) == ERROR_INVALID_DATA);
    CHECK(!mvdm_vm_notification_post(&latest) && raises == 4);
    CHECK(!mvdm_vm_notification_close() && raises == 4 && cancels == 1);
    CHECK(!owner && !eoi);
    CHECK(!mvdm_vm_notification_close() && cancels == 1);
    CHECK(!mvdm_vm_notification_open(&irq));
    CHECK(mvdm_vm_notification_acknowledge(first) == ERROR_INVALID_DATA);
    CHECK(!mvdm_vm_notification_post(&second) && second > latest);
    CHECK(!mvdm_vm_notification_close());
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 0, 0, 0, 0, 1, &result));
    CHECK(!result.status && result.value == 15);
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 2, 0, 0, 0, 1, &result));
    CHECK(!result.status && result.serial_low > (uint32_t)second);
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 5, 0, 0, 0, 1, &result));
    CHECK(!result.status && !owner);
    CHECK(!mvdm_vm_notification_open(&irq));
    CHECK(!mvdm_vm_notification_ready());
    CHECK(!mvdm_vm_wake_register(1, 11));
    CHECK(!mvdm_vm_wake_register(2, 12));
    CHECK(mvdm_vm_wake_register(1, 99) == ERROR_ALREADY_EXISTS);
    CHECK(mvdm_vm_wake_publish(1, 99, &latest) == ERROR_NOT_FOUND);
    CHECK(!mvdm_vm_wake_publish(1, 11, &first));
    CHECK(!mvdm_vm_wake_publish(2, 12, &second) && second > first);
    CHECK(!mvdm_vm_wake_publish(1, 11, &latest));
    CHECK(!mvdm_vm_wake_retire(1, 11));
    CHECK(!mvdm_vm_wake_take(&wake) && wake.vm == 2 && wake.generation == 12);
    CHECK(mvdm_vm_wake_take(&wake) == ERROR_NO_MORE_ITEMS);
    CHECK(!mvdm_vm_wake_publish(2, 12, &latest));
    CHECK(mvdm_vm_wake_retire(2, 11) == ERROR_NOT_FOUND);
    CHECK(!mvdm_vm_wake_take(&wake) && wake.vm == 2);
    CHECK(!mvdm_vm_wake_register(1, 22));
    CHECK(mvdm_vm_wake_publish(1, 11, &latest) == ERROR_NOT_FOUND);
    CHECK(!mvdm_vm_wake_publish(1, 22, &latest));
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 5, 0, 0, 0, 1, &result));
    CHECK(!result.status && !owner);
    CHECK(mvdm_vm_wake_take(&wake) == ERROR_NO_MORE_ITEMS);
    CHECK(mvdm_vm_wake_retire(1, 22) == ERROR_NOT_FOUND);
    CHECK(mvdm_vm_wake_register(1, 23) == ERROR_NOT_READY);
    CHECK(!mvdm_vm_notification_open(&irq));
    CHECK(!mvdm_vm_notification_ready());
    CHECK(!mvdm_vm_wake_register(3, 30));
    producer.published = CreateEventW(NULL, FALSE, FALSE, NULL);
    producer.resume = CreateEventW(NULL, FALSE, FALSE, NULL);
    CHECK(producer.published && producer.resume);
    thread = CreateThread(NULL, 0, publish_from_host, &producer, 0, NULL);
    CHECK(thread != NULL);
    CHECK(WaitForSingleObject(producer.published, INFINITE) == WAIT_OBJECT_0);
    CHECK(!producer.first_error);
    /* Retire with an unread completion, then reuse the same VM handle.
     * The old producer must not publish into the new generation. */
    CHECK(!mvdm_vm_wake_retire(3, 30));
    CHECK(!mvdm_vm_wake_register(3, 31));
    CHECK(SetEvent(producer.resume));
    CHECK(WaitForSingleObject(thread, INFINITE) == WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(thread, &thread_result) && !thread_result);
    CHECK(producer.retired_error == ERROR_NOT_FOUND && !producer.reused_error);
    CHECK(!mvdm_vm_wake_take(&wake) && wake.vm == 3 && wake.generation == 31);
    CHECK(mvdm_vm_wake_take(&wake) == ERROR_NO_MORE_ITEMS);
    CloseHandle(thread);
    CloseHandle(producer.resume);
    CloseHandle(producer.published);
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 5, 0, 0, 0, 1, &result));
    CHECK(!result.status && !owner);
    CHECK(!depth);
    DeleteCriticalSection(&ica);
    printf("PASS %ld notification unit assertions; no real PIC claim\n", assertions);
    return 0;
}
