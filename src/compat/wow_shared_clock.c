#include "compat/include/wow_shared_clock.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static DWORD WINAPI clock_publisher(void *context)
{
    compat_wow_shared_clock *clock = context;
    for (;;) {
        if (WaitForSingleObject(clock->stop, 10u) != WAIT_TIMEOUT) return 0;
        clock->page[0] = GetTickCount();
        clock->page[1] = 0x01000000u;
    }
}

int compat_wow_shared_clock_allocate(compat_wow_shared_clock *clock)
{
    if (!clock || clock->page || clock->stop || clock->thread) return 0;
    clock->page = VirtualAlloc(NULL, COMPAT_WOW_CLOCK_PAGE_BYTES,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!clock->page) return 0;
    clock->page[0] = GetTickCount();
    clock->page[1] = 0x01000000u;
    return 1;
}

int compat_wow_shared_clock_start(compat_wow_shared_clock *clock)
{
    if (!clock || !clock->page || clock->stop || clock->thread) return 0;
    clock->stop = CreateEventA(NULL, TRUE, FALSE, NULL);
    if (!clock->stop) return 0;
    clock->thread = CreateThread(NULL, 0u, clock_publisher, clock, 0u, NULL);
    return clock->thread != NULL;
}

void compat_wow_shared_clock_stop(compat_wow_shared_clock *clock)
{
    if (clock->stop) SetEvent(clock->stop);
    if (clock->thread) {
        WaitForSingleObject(clock->thread, INFINITE);
        CloseHandle(clock->thread);
        clock->thread = NULL;
    }
}

void compat_wow_shared_clock_dispose(compat_wow_shared_clock *clock)
{
    compat_wow_shared_clock_stop(clock);
    if (clock->stop) CloseHandle(clock->stop);
    if (clock->page) VirtualFree((void *)clock->page, 0u, MEM_RELEASE);
    ZeroMemory(clock, sizeof(*clock));
}
