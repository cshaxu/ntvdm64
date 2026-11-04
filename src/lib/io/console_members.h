#ifndef COMMON_CONSOLE_MEMBERS_H
#define COMMON_CONSOLE_MEMBERS_H
#include <windows.h>

/* One on-demand snapshot of this process's actual Console attachment.
 * No task, ancestry, authentication or lifetime interpretation. Output is
 * process-heap owned and released with common_console_members_release.
 * reads==0 permits capacity growth until limit; otherwise bound API attempts.
 * Failure leaves no allocation and count zero. No timers or waiting. */
DWORD common_console_members_read(DWORD initial,DWORD limit,DWORD reads,
    DWORD **members,DWORD *count);
void common_console_members_release(DWORD *members);
#endif
