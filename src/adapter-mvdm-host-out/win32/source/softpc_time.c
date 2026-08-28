#if 0
/* DEFERRED EXPERIMENT MVDM-SOFTPC-PATCH-002: this same-shaped timing adapter
 * is retained for the later approved runtime route.  The active breakpoint
 * patch remains evidence-only and must not pull a replacement timer into its
 * current link closure. */

#include <windows.h>

#include "insignia.h"
#include "host_def.h"
#include "timeval.h"

/* DIVERGENCE ADAPTER-WIN32-015: OpenNT nt_timer.c obtains the counter through
 * private NT/Rtl arithmetic and is inseparable from the historical timer
 * product shell.  Preserve the reached GetPerfCounterUsecs signature and its
 * microsecond result using the public, width-neutral Win32 counter API. */
void GetPerfCounterUsecs(struct host_timeval *time, PLARGE_INTEGER counter)
{
    LARGE_INTEGER now;
    LARGE_INTEGER frequency;
    ULONGLONG usecs;

    if (!QueryPerformanceCounter(&now) || !QueryPerformanceFrequency(&frequency) ||
        frequency.QuadPart <= 0) {
        if (time != NULL) {
            time->tv_sec = 0;
            time->tv_usec = 0;
        }
        if (counter != NULL) counter->QuadPart = 0;
        return;
    }
    usecs = ((ULONGLONG)now.QuadPart / (ULONGLONG)frequency.QuadPart) * 1000000ULL +
        (((ULONGLONG)now.QuadPart % (ULONGLONG)frequency.QuadPart) * 1000000ULL) /
        (ULONGLONG)frequency.QuadPart;
    if (time != NULL) {
        time->tv_sec = (IS32)(usecs / 1000000ULL);
        time->tv_usec = (IS32)(usecs % 1000000ULL);
    }
    if (counter != NULL) counter->QuadPart = (LONGLONG)usecs;
}
#endif
