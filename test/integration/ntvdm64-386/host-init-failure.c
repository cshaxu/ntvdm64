#include "mvdm_vm_bridge.h"
#include <windows.h>

/* Test-only link seam: fail arming after both host and guest IRQ ownership
 * exist. The real guest driver must call its rollback path. Never shipped. */
extern int mvdm_vm_bridge_bop_direct(uint32_t, uint32_t, uint32_t,
    uint32_t, uint32_t, int, mvdm_vm_bridge_result *);
extern void host_ica_lock(void);
extern void host_ica_unlock(void);
extern WORD VDDReserveIrqLine(HANDLE, WORD);
extern BOOL VDDReleaseIrqLine(HANDLE, WORD);
extern BOOL RegisterEOIHook(int, void (*)(int, int));
extern BOOL RemoveEOIHook(int, void (*)(int, int));
static WORD irq = 0xffff;
static BOOL injected;
static void unused_eoi(int line, int count) { (void)line; (void)count; }

int mvdm_vm_bridge_bop(uint32_t signature, uint32_t operation,
    uint32_t low, uint32_t high, uint32_t vm, int ring0,
    mvdm_vm_bridge_result *result)
{
    int handled;
    if (signature == MVDM_VM_BRIDGE_SIGNATURE && ring0 && operation == 1) {
        if (injected || irq == 0xffff) ExitProcess(ERROR_INVALID_STATE);
        injected = TRUE;
        ZeroMemory(result, sizeof(*result));
        result->status = ERROR_NOT_READY;
        return 1;
    }
    handled = mvdm_vm_bridge_bop_direct(signature, operation, low, high, vm,
        ring0, result);
    if (signature != MVDM_VM_BRIDGE_SIGNATURE || !ring0) return handled;
    if (operation == 0 && !result->status) irq = (WORD)result->value;
    if (operation == 7) ExitProcess(ERROR_INVALID_STATE);
    if (operation == 5 && injected) {
        WCHAR path[MAX_PATH];
        DWORD length, written, marker = 1;
        HANDLE file;
        if (result->status) ExitProcess(ERROR_INVALID_STATE);
        host_ica_lock();
        if (VDDReserveIrqLine(&injected, irq) != irq ||
            !RegisterEOIHook(irq, unused_eoi) ||
            !RemoveEOIHook(irq, unused_eoi) ||
            !VDDReleaseIrqLine(&injected, irq))
            ExitProcess(ERROR_INVALID_STATE);
        host_ica_unlock();
        length = GetEnvironmentVariableW(L"MVDM_INIT_FAILURE_REPORT", path,
            MAX_PATH);
        if (!length || length >= MAX_PATH) ExitProcess(ERROR_INVALID_NAME);
        file = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_NEW,
            FILE_ATTRIBUTE_NORMAL, NULL);
        if (file == INVALID_HANDLE_VALUE ||
            !WriteFile(file, &marker, sizeof(marker), &written, NULL) ||
            written != sizeof(marker)) ExitProcess(ERROR_WRITE_FAULT);
        CloseHandle(file);
    }
    return handled;
}
