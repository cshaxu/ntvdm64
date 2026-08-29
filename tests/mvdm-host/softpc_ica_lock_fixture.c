#include <windows.h>

extern void InitializeIcaLock(void);
extern void host_ica_lock(void);
extern void host_ica_unlock(void);

BOOL VDMForWOW = FALSE;
static volatile LONG entered;
static volatile LONG unexpected_call;

/* The explicitly linked original object also contains delayed-IRQ and WOW
 * entry points.  These test-only definitions satisfy their unselected
 * outbound references; the lock fixture asserts none is reached. */
void host_clear_hw_int(void) { InterlockedExchange(&unexpected_call, 1); }
void ActivityCheckAfterTimeSlice(void) { InterlockedExchange(&unexpected_call, 1); }
void delete_q_event(void) { InterlockedExchange(&unexpected_call, 1); }
int add_q_event_i(void) { InterlockedExchange(&unexpected_call, 1); return 0; }
int ica_scan_irr(void) { InterlockedExchange(&unexpected_call, 1); return 0; }
void ica_interrupt_cpu(void) { InterlockedExchange(&unexpected_call, 1); }
void ica_eoi(void) { InterlockedExchange(&unexpected_call, 1); }
void ica_clear_int(void) { InterlockedExchange(&unexpected_call, 1); }
void ica_hw_interrupt(void) { InterlockedExchange(&unexpected_call, 1); }
void *Start_of_M_area;
void *VirtualIca;
LARGE_INTEGER CurrHeartBeat;

int DisplayErrorTerm(int error, DWORD os_error, char *file, int line)
{
    (void)error; (void)os_error; (void)file; (void)line;
    return 0;
}

VOID TerminateVDM(VOID)
{
    ExitProcess(99u);
}

static DWORD WINAPI lock_worker(LPVOID parameter)
{
    (void)parameter;
    host_ica_lock();
    InterlockedExchange(&entered, 1);
    host_ica_unlock();
    return 0u;
}

int main(void)
{
    HANDLE worker;
    DWORD exit_code = 0u;

    InitializeIcaLock();
    host_ica_lock();
    worker = CreateThread(NULL, 0u, lock_worker, NULL, 0u, NULL);
    if (worker == NULL) return 1;
    Sleep(25u);
    if (InterlockedCompareExchange(&entered, 0, 0) != 0) return 2;
    host_ica_unlock();
    if (WaitForSingleObject(worker, 5000u) != WAIT_OBJECT_0 ||
        !GetExitCodeThread(worker, &exit_code) || exit_code != 0u ||
        InterlockedCompareExchange(&entered, 0, 0) != 1 ||
        InterlockedCompareExchange(&unexpected_call, 0, 0) != 0) return 3;
    CloseHandle(worker);
    return 0;
}
