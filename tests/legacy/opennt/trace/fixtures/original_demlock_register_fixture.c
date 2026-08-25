/*
 * R1.3 original DEM caller proof for demLockOper. It uses a temporary owned
 * host file only to exercise the original successful LockFile/UnlockFile
 * path; it executes no guest, BIOS, BOP, SVC dispatcher, or DOS program.
 */
#include <stdint.h>
#include <stdlib.h>
#include <windows.h>

extern void c_cpu_init(void);
extern void c_cpu_terminate(void);
extern uint16_t c_getCF(void);
extern unsigned long runner_ccpu_sm0_unexpected_calls(void);
extern void demLockOper(void);
extern void setAX(uint16_t value);
extern void setBP(uint16_t value);
extern void setBX(uint16_t value);
extern void setCX(uint16_t value);
extern void setDI(uint16_t value);
extern void setDX(uint16_t value);
extern void setSI(uint16_t value);

/* demLockOper reaches this only after a failed Win32 lock. The test accepts no
 * synthesized DEM error result, so the unexpected branch terminates instead. */
void demClientError(HANDLE file, CHAR drive) {
    (void)file;
    (void)drive;
    abort();
}

/* This fixture never leaves initialized real mode. */
int selector_outside_table(uint16_t selector, uint32_t *descriptor_address) {
    (void)selector;
    (void)descriptor_address;
    abort();
    return 1;
}

static void set_lock_arguments(HANDLE file, uint16_t operation) {
    uintptr_t handle_value = (uintptr_t)file;
    setAX(operation);
    setBX((uint16_t)(handle_value >> 16));
    setBP((uint16_t)handle_value);
    setCX(0); setDX(0);
    setSI(0); setDI(1);
}

int main(void) {
    CHAR directory[MAX_PATH];
    CHAR filename[MAX_PATH];
    HANDLE file = INVALID_HANDLE_VALUE;
    int result = 0;

    if (!GetTempPathA(MAX_PATH, directory)) return 1;
    if (!GetTempFileNameA(directory, "ndm", 0, filename)) return 2;
    file = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                       CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
    if (file == INVALID_HANDLE_VALUE) { DeleteFileA(filename); return 3; }

    c_cpu_init();
    if (runner_ccpu_sm0_unexpected_calls() != 0) result = 4;
    if (result == 0) {
        set_lock_arguments(file, 0);
        demLockOper();
        if (c_getCF() != 0) result = 5;
    }
    if (result == 0) {
        set_lock_arguments(file, 1);
        demLockOper();
        if (c_getCF() != 0) result = 6;
    }
    if (result == 0 && runner_ccpu_sm0_unexpected_calls() != 0) result = 7;

    c_cpu_terminate();
    CloseHandle(file);
    DeleteFileA(filename);
    return result;
}
