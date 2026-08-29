#include <windows.h>
#include <winternl.h>

#include "adapter-mvdm-host-out/win32/include/thread_start_compat.h"

/* The original source TU intentionally sees CreateThread through the cdecl
 * bridge.  This fixture itself needs the native WINAPI entry to drive its
 * own worker, so keep that test harness call outside the source-shaped macro. */
#undef CreateThread

extern NTSTATUS DelayHeartBeat(LONG delay);
extern HANDLE hHBResumeEvent;
extern HANDLE hHBSuspendEvent;

static LONG worker_delay = 50000000;

/* These are outbound forms in other original heartbeat functions in the
 * explicitly linked translation unit.  The DelayHeartBeat test asserts none
 * is reached; production composition supplies their original owners. */
void DisplayErrorTerm(void) { ExitProcess(91u); }
void TerminateVDM(void) { ExitProcess(92u); }
void InitSound(void) { ExitProcess(93u); }
void ReinitIdealTime(void) { ExitProcess(94u); }
void rtc_init(void) { ExitProcess(95u); }
void RtcTick(void) { ExitProcess(96u); }
void q_event_init(void) { ExitProcess(97u); }
void quick_tick_recalibrate(void) { ExitProcess(98u); }
void host_ica_lock(void) { ExitProcess(99u); }
void host_ica_unlock(void) { ExitProcess(100u); }
void time_tick(void) { ExitProcess(101u); }
void cpu_interrupt(void) { ExitProcess(102u); }
void WOWIdle(void) { ExitProcess(103u); }
void DemHeartBeat(void) { ExitProcess(104u); }
void time_strobe(void) { ExitProcess(105u); }
void PlayContinuousTone(void) { ExitProcess(106u); }
void host_com_heart_beat(void) { ExitProcess(107u); }
void host_lpt_heart_beat(void) { ExitProcess(108u); }
void host_flpy_heart_beat(void) { ExitProcess(109u); }
void PrioWaitIfIdle(void) { ExitProcess(110u); }
void c_cpu_interrupt(void) { ExitProcess(111u); }
void idetect(void) { ExitProcess(112u); }
void GetScaleEvent(void) { ExitProcess(113u); }
HANDLE opennt_create_cdecl_thread(LPSECURITY_ATTRIBUTES attributes,
    SIZE_T stack_bytes, OPENNT_CDECL_THREAD_START_ROUTINE start_routine,
    LPVOID parameter, DWORD flags, LPDWORD thread_id)
{
    (void)attributes; (void)stack_bytes; (void)start_routine;
    (void)parameter; (void)flags; (void)thread_id;
    ExitProcess(114u);
    return NULL;
}
LARGE_INTEGER NTAPI RtlExtendedLargeIntegerDivide(LARGE_INTEGER value,
    ULONG divisor, PULONG remainder)
{
    (void)value; (void)divisor; (void)remainder;
    ExitProcess(115u);
    value.QuadPart = 0;
    return value;
}
LARGE_INTEGER NTAPI RtlExtendedIntegerMultiply(LONG value, LONG multiplier)
{
    LARGE_INTEGER result;
    (void)value; (void)multiplier;
    ExitProcess(116u);
    result.QuadPart = 0;
    return result;
}
void (*timer_tick_func)(void);
void *working_video_funcs;
void *sc;
BOOL VDMForWOW;
DWORD dwWNTPifFlags;
UCHAR WNTPifFgPr;
UCHAR WNTPifBgPr;
/* host_init_bda_timer is another original form in nt_timer.c, outside this
 * DelayHeartBeat-only exercise.  Supply its CCPU-owned anchor solely so the
 * linker can retain the unchanged original translation unit; any use here is
 * a test failure because no backing guest-memory route is being claimed. */
unsigned char *Start_of_M_area = NULL;

static DWORD WINAPI delay_worker(LPVOID parameter)
{
    (void)parameter;
    return (DWORD)DelayHeartBeat(worker_delay);
}

int main(void)
{
    HANDLE worker;
    DWORD exit_code = 0u;

    hHBResumeEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    hHBSuspendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (hHBResumeEvent == NULL || hHBSuspendEvent == NULL) return 1;
    worker = CreateThread(NULL, 0u, delay_worker, NULL, 0u, NULL);
    if (worker == NULL) return 2;
    Sleep(25u);
    if (WaitForSingleObject(worker, 0u) != WAIT_TIMEOUT) return 3;
    if (!SetEvent(hHBSuspendEvent)) return 4;
    Sleep(25u);
    if (WaitForSingleObject(worker, 0u) != WAIT_TIMEOUT) return 5;
    if (!SetEvent(hHBResumeEvent) ||
        WaitForSingleObject(worker, 5000u) != WAIT_OBJECT_0 ||
        !GetExitCodeThread(worker, &exit_code) ||
        (NTSTATUS)exit_code != STATUS_SUCCESS) return 6;
    CloseHandle(worker);
    CloseHandle(hHBResumeEvent);
    CloseHandle(hHBSuspendEvent);
    hHBResumeEvent = NULL;
    hHBSuspendEvent = NULL;
    return 0;
}
