/* Controlled host-boundary witness for the actual selected qevnt.obj.
 * This is not a guest or wall-clock integration test. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "../../src/mvdm/softpc.new/host/inc/host_qev.h"
#ifndef MVDM_QEV_EXPECT_HANDLE_BYTES
#define MVDM_QEV_EXPECT_HANDLE_BYTES 8
#endif
typedef char qev_handle_abi_check[(sizeof(q_ev_handle) == MVDM_QEV_EXPECT_HANDLE_BYTES) ? 1 : -1];
typedef char qev_pointer_abi_check[(sizeof(void *) == 4) ? 1 : -1];
typedef q_ev_handle QHANDLE;
typedef void (*QCALLBACK)(long);
extern void q_event_init(void);
extern QHANDLE add_q_event_t(QCALLBACK, unsigned long, long);
extern QHANDLE add_q_event_i(QCALLBACK, unsigned long, long);
extern void delete_q_event(QHANDLE);
extern void dispatch_q_event(void);

static unsigned checks, failures, calls;
static long values[32];
static unsigned lock_depth, unlock_errors;
static ULONG cpu_count;
static LONGLONG now = 100000;
static LONG outstanding;
static int fail_allocation;

#define CHECK(x) do { ++checks; if (!(x)) { ++failures; \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); } } while (0)

/* Deliberately bind the real object's allocator boundary. No queue algorithm
 * is copied into the fixture. HeapAlloc avoids recursive CRT allocation. */
void * __cdecl malloc(size_t bytes)
{
    void *p;
    if (fail_allocation) return NULL;
    p = HeapAlloc(GetProcessHeap(), 0, bytes);
    if (p) InterlockedIncrement(&outstanding);
    return p;
}
void __cdecl free(void *p)
{
    if (p) {
        HeapFree(GetProcessHeap(), 0, p);
        InterlockedDecrement(&outstanding);
    }
}
void host_ica_lock(void) { ++lock_depth; }
void host_ica_unlock(void)
{
    if (!lock_depth) ++unlock_errors;
    else --lock_depth;
}
void host_TimeStamp(PLARGE_INTEGER value) { value->QuadPart = now; }
ULONG host_get_jump_restart(void) { return 100; }
ULONG host_get_q_calib_val(void) { return 0; }
void c_cpu_q_ev_set_count(ULONG value) { cpu_count = value; }
ULONG c_cpu_q_ev_get_count(void) { return cpu_count; }

static void receive(long value)
{
    CHECK(lock_depth == 0); /* qevnt releases ICA before invoking caller. */
    if (calls < sizeof(values) / sizeof(values[0])) values[calls] = value;
    ++calls;
}
static QHANDLE cancel_during_callback;
static void reenter(long value)
{
    receive(value);
    delete_q_event(cancel_during_callback);
    CHECK(add_q_event_t(receive, 20, 42) != 1);
}

int main(void)
{
    QHANDLE a, b, c, old;
    LONG baseline;
    unsigned before;
    q_event_init();
    CHECK(cpu_count == 0 && lock_depth == 0);
    baseline = outstanding;
    dispatch_q_event();
    CHECK(calls == 0 && outstanding == baseline);

    a = add_q_event_t(receive, 300, 3);
    b = add_q_event_t(receive, 100, 1);
    c = add_q_event_t(receive, 200, 2);
    CHECK(a != 1 && b != 1 && c != 1);
    CHECK(a != b && a != c && b != c);
    CHECK(outstanding == baseline + 3 && cpu_count != 0);
    now += 100; dispatch_q_event();
    CHECK(calls == 1 && values[0] == 1);
    now += 100; dispatch_q_event();
    CHECK(calls == 2 && values[1] == 2);
    now += 100; dispatch_q_event();
    CHECK(calls == 3 && values[2] == 3);
    CHECK(cpu_count == 0 && outstanding == baseline);

    a = add_q_event_t(receive, 10, 10);
    b = add_q_event_t(receive, 20, 20);
    c = add_q_event_t(receive, 30, 30);
    delete_q_event(b); delete_q_event(a); delete_q_event(c);
    CHECK(cpu_count == 0 && outstanding == baseline);
    before = calls;
    delete_q_event(a); delete_q_event(0); delete_q_event(1);
    delete_q_event((QHANDLE)0x123456789abcdef0LL);
    dispatch_q_event();
    CHECK(calls == before && outstanding == baseline);

    /* A matching object pointer with a different ID must not retire it. */
    a = add_q_event_t(receive, 10, 11);
    old = a ^ ((QHANDLE)1 << 32);
    delete_q_event(old);
    CHECK(outstanding == baseline + 1);
    now += 10; dispatch_q_event();
    CHECK(calls == before + 1 && values[before] == 11);
    CHECK(outstanding == baseline);

    /* Init is calibration/init, not an invented blanket queue cancellation. */
    a = add_q_event_i(receive, 10, 12);
    CHECK(a != 1);
    q_event_init();
    CHECK(outstanding == baseline + 1);
    now += 10; dispatch_q_event();
    CHECK(values[calls - 1] == 12 && outstanding == baseline);

    a = add_q_event_t(reenter, 10, 41);
    cancel_during_callback = add_q_event_t(receive, 30, 99);
    CHECK(a != 1 && cancel_during_callback != 1);
    now += 10; dispatch_q_event();
    CHECK(values[calls - 1] == 41 && outstanding == baseline + 1);
    now += 20; dispatch_q_event();
    CHECK(values[calls - 1] == 42 && outstanding == baseline);

    fail_allocation = 1;
    a = add_q_event_t(receive, 10, 100);
    fail_allocation = 0;
    CHECK(a == 1); /* Actual original non-MONITOR allocation-failure token. */
    before = calls; dispatch_q_event();
    CHECK(calls == before && outstanding == baseline && cpu_count == 0);

    old = add_q_event_t(receive, 50, 77);
    CHECK(old != 1 && outstanding == baseline + 1);
    fail_allocation = 1;
    a = add_q_event_t(receive, 10, 100);
    fail_allocation = 0;
    CHECK(a == 1 && outstanding == baseline + 1);
    delete_q_event(old);
    CHECK(outstanding == baseline && cpu_count == 0);

    a = add_q_event_t(receive, 0, 13);
    CHECK(a != 1 && outstanding == baseline + 1 && calls == before);
    dispatch_q_event();
    CHECK(values[calls - 1] == 13 && outstanding == baseline);
    CHECK(lock_depth == 0 && unlock_errors == 0);
    printf("QEVNT_OBJECT_CONTRACT checks=%u failures=%u callbacks=%u\n",
           checks, failures, calls);
    return failures ? 1 : 0;
}
