#include "compat/include/wow_shared_clock.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#define CHECK(x) do { ++checks; if (!(x)) { \
    fprintf(stderr, "clock failure line=%u: %s\n", __LINE__, #x); return 1; } } while (0)

int main(void)
{
    compat_wow_shared_clock a = {0}, b = {0};
    unsigned checks = 0;
    uint32_t initial, stopped;
    DWORD started, code;
    HANDLE thread;
    CHECK(!compat_wow_shared_clock_start(&a));
    compat_wow_shared_clock_stop(&a);
    compat_wow_shared_clock_dispose(&a);
    CHECK(compat_wow_shared_clock_allocate(&a));
    CHECK(a.page && a.page[1] == 0x01000000u);
    CHECK(!compat_wow_shared_clock_allocate(&a));
    CHECK(compat_wow_shared_clock_allocate(&b));
    CHECK(a.page != b.page);
    CHECK(compat_wow_shared_clock_start(&a));
    CHECK(!compat_wow_shared_clock_start(&a));
    CHECK(DuplicateHandle(GetCurrentProcess(), a.thread, GetCurrentProcess(),
        &thread, SYNCHRONIZE | THREAD_QUERY_INFORMATION, FALSE, 0));
    initial = a.page[0]; started = GetTickCount();
    while (a.page[0] == initial && GetTickCount() - started < 2000u) Sleep(1);
    CHECK(a.page[0] != initial && a.page[1] == 0x01000000u);
    compat_wow_shared_clock_stop(&a);
    CHECK(WaitForSingleObject(thread, 0) == WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(thread, &code) && code == 0);
    CloseHandle(thread);
    CHECK(!a.thread && a.stop && a.page);
    stopped = a.page[0]; Sleep(30);
    CHECK(a.page[0] == stopped);
    compat_wow_shared_clock_stop(&a);
    /* The host page stays alive through the caller's guest-unmap phase. */
    CHECK(a.page[1] == 0x01000000u);
    compat_wow_shared_clock_dispose(&a);
    CHECK(!a.page && !a.thread && !a.stop);
    CHECK(b.page && !b.thread && !b.stop);
    /* State after successful event creation but failed thread creation. */
    b.stop = CreateEventA(NULL, TRUE, FALSE, NULL);
    CHECK(b.stop != NULL);
    compat_wow_shared_clock_stop(&b);
    CHECK(WaitForSingleObject(b.stop, 0) == WAIT_OBJECT_0 && b.page);
    compat_wow_shared_clock_dispose(&b);
    CHECK(!b.page && !b.thread && !b.stop);
    CHECK(compat_wow_shared_clock_allocate(&a));
    CHECK(compat_wow_shared_clock_start(&a));
    compat_wow_shared_clock_dispose(&a);
    CHECK(!a.page && !a.thread && !a.stop);
    printf("WOW_SHARED_CLOCK_PASS checks=%u\n", checks);
    return 0;
}
