/* Generated include contains current production functions verbatim.
 * This tests the project atomic adapter, not an alternative CPU/PIC loop. */
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
typedef uint32_t IUM32;
typedef int IBOOL;
#define LOCAL static
#define GLOBAL
#define VOID void
#define IFN0() (void)
#define IFN1(type, name) (type name)
static volatile IUM32 cpu_interrupt_map;
#include "notification_body.inc"

typedef struct lane {
    IUM32 mask;
    HANDLE start, raised, consumed;
    unsigned rounds;
} lane;

static DWORD WINAPI producer(void *opaque)
{
    lane *p = (lane *)opaque;
    unsigned i;
    for (i = 0; i < p->rounds; ++i) {
        if (WaitForSingleObject(p->start, 5000) != WAIT_OBJECT_0) return 1;
        c_cpu_raise_event(p->mask);
        SetEvent(p->raised);
        if (WaitForSingleObject(p->consumed, 5000) != WAIT_OBJECT_0) return 2;
    }
    return 0;
}

int main(void)
{
    lane lanes[2] = {0};
    HANDLE threads[2] = {0}, raised[2];
    IUM32 all = CPU_HW_INT_MASK | CPU_SIGIO_EXCEPTION_MASK |
        CPU_SAD_EXCEPTION_MASK | CPU_RESET_EXCEPTION_MASK | CPU_SIGALRM_EXCEPTION_MASK;
    unsigned i, round;
    int result = 1;
    DWORD exit_code;
    c_cpu_raise_event(all);
    clear_any_thingies();
    if (c_cpu_event_snapshot() != (all & ~CPU_SIGALRM_EXCEPTION_MASK)) goto done;
    if (!c_cpu_take_event(CPU_HW_INT_MASK) || c_cpu_take_event(CPU_HW_INT_MASK)) goto done;
    if (c_cpu_event_snapshot() != (all & ~(CPU_SIGALRM_EXCEPTION_MASK | CPU_HW_INT_MASK))) goto done;
    if (!c_cpu_take_event(all) || c_cpu_event_snapshot() != 0u) goto done;
    /* Distinct simultaneous raises, followed by consumption of one bit while
     * the other remains. Events establish per-round ownership, not Sleeps. */
    for (i = 0; i < 2; ++i) {
        lanes[i].mask = i ? CPU_SIGIO_EXCEPTION_MASK : CPU_HW_INT_MASK;
        lanes[i].rounds = 4096;
        lanes[i].start = CreateEvent(NULL, FALSE, FALSE, NULL);
        lanes[i].raised = CreateEvent(NULL, FALSE, FALSE, NULL);
        lanes[i].consumed = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (!lanes[i].start || !lanes[i].raised || !lanes[i].consumed) goto done;
        raised[i] = lanes[i].raised;
        threads[i] = CreateThread(NULL, 0, producer, &lanes[i], 0, NULL);
        if (!threads[i]) goto done;
    }
    for (round = 0; round < 4096; ++round) {
        SetEvent(lanes[0].start); SetEvent(lanes[1].start);
        if (WaitForMultipleObjects(2, raised, TRUE, 5000) != WAIT_OBJECT_0) goto done;
        if (c_cpu_event_snapshot() != (lanes[0].mask | lanes[1].mask)) goto done;
        if (!c_cpu_take_event(lanes[0].mask) || c_cpu_event_snapshot() != lanes[1].mask) goto done;
        if (!c_cpu_take_event(lanes[1].mask) || c_cpu_event_snapshot() != 0u) goto done;
        SetEvent(lanes[0].consumed); SetEvent(lanes[1].consumed);
    }
    if (WaitForMultipleObjects(2, threads, TRUE, 5000) != WAIT_OBJECT_0) goto done;
    for (i = 0; i < 2; ++i)
        if (!GetExitCodeThread(threads[i], &exit_code) || exit_code) goto done;
    printf("CCPU_NOTIFICATION_PASS rounds=4096 raises=8192 (adapter, not guest IRQ rate)\n");
    result = 0;
done:
    if (result) fprintf(stderr, "CCPU_NOTIFICATION_FAIL\n");
    /* On failure producers have bounded waits. Never close their objects
     * while they can still use them. */
    for (i = 0; i < 2; ++i) {
        if (threads[i]) {
            if (WaitForSingleObject(threads[i], 11000) != WAIT_OBJECT_0)
                ExitProcess(3); /* Fail closed; do not close in-use objects. */
            CloseHandle(threads[i]);
        }
        if (lanes[i].start) CloseHandle(lanes[i].start);
        if (lanes[i].raised) CloseHandle(lanes[i].raised);
        if (lanes[i].consumed) CloseHandle(lanes[i].consumed);
    }
    return result;
}
