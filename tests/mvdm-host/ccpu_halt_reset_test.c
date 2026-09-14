#include <stdint.h>
#include <windows.h>
#include "session/session.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_guest_memory.h"

/* Formal original CCPU/SAS providers, not replacement decode/reset bodies. */
extern void *setup_global_data_ptr(void);
extern void setup_vga_globals(void);
extern void sas_init(uint32_t);
extern void sas_term(void);
extern void c_cpu_init(void);
extern void load_sw_cpu_access_functions(void);
extern void c_cpu_simulate(void);
extern void c_cpu_interrupt(int, unsigned short);
extern void c_setIP(unsigned short);
extern unsigned short c_getAX(void);
extern void c_sas_stores(uint32_t, unsigned char *, uint32_t);
extern unsigned char *c_GetPhyAdd(uint32_t);

static void report(const char *text)
{
    DWORD written;
    WriteFile(GetStdHandle(STD_ERROR_HANDLE), text, lstrlenA(text), &written, NULL);
}

static DWORD WINAPI raise_reset(void *context)
{
    volatile unsigned char *entered = context;
    DWORD start = GetTickCount();
    while (*entered != 1) {
        if (GetTickCount() - start > 5000) return 1;
        Sleep(1);
    }
    /* Guest writes marker immediately before HLT; allow it to enter wait. */
    Sleep(50);
    c_cpu_interrupt(0, 0); /* original CPU_HW_RESET enum */
    return 0;
}

static LONG WINAPI report_exception(EXCEPTION_POINTERS *e)
{
    char message[128];
    wsprintfA(message, "exception=%08lx rva=%lx\n",
        e->ExceptionRecord->ExceptionCode,
        (unsigned long)((uintptr_t)e->ExceptionRecord->ExceptionAddress -
            (uintptr_t)GetModuleHandle(NULL)));
    report(message);
    wsprintfA(message, "caller-rva=%lx\n",
        *(DWORD *)(uintptr_t)e->ContextRecord->Esp - (DWORD)(uintptr_t)GetModuleHandle(NULL));
    report(message);
    return EXCEPTION_EXECUTE_HANDLER;
}

int main(void)
{
    session owner;
    HANDLE producer;
    DWORD producer_result = 1;
    unsigned short ax;
    char message[128];
    /* Padding permits either retained-IP or post-HLT-IP continuation in the
     * deliberately broken baseline to reach the DEAD sentinel safely. */
    unsigned char program[] = {0xc6,0x06,0x00,0x02,0x01,0xf4,0x90,
        0xb8,0xad,0xde,0xd6,0xfe};
    unsigned char reset_vector[] = {0xb8,0xef,0xbe,0xd6,0xfe};
    SetUnhandledExceptionFilter(report_exception);
    report("session/gdp\n");
    session_initialize(&owner, 412);
    if (!session_activate(&owner) || !session_thread_bind(&owner) ||
        setup_global_data_ptr() == NULL) return 2;
    report("sas-init\n");
    sas_init(0x200000);
    if (!mvdm_softpc_guest_memory_begin(&owner)) return 3;
    report("vga/cpu-init\n");
    setup_vga_globals();
    load_sw_cpu_access_functions();
    c_cpu_init();
    report("guest-program\n");
    c_sas_stores(0xf1000, program, sizeof(program));
    c_sas_stores(0xffff0, reset_vector, sizeof(reset_vector));
    /* The formal SAS initialization installs ROM protection. This test owns
     * its synthetic image: fill its backing explicitly, without asking guest
     * writes to overwrite ROM or changing the product ROM policy. */
    CopyMemory(c_GetPhyAdd(0xf1000), program, sizeof(program));
    CopyMemory(c_GetPhyAdd(0xffff0), reset_vector, sizeof(reset_vector));
    *c_GetPhyAdd(0x200) = 0;
    producer = CreateThread(NULL, 0, raise_reset, c_GetPhyAdd(0x200), 0, NULL);
    if (!producer) return 4;
    c_setIP(0x1000);
    report("simulate\n");
    c_cpu_simulate();
    ax = c_getAX();
    if (WaitForSingleObject(producer, 6000) != WAIT_OBJECT_0 ||
        !GetExitCodeThread(producer, &producer_result)) return 5;
    CloseHandle(producer);
    wsprintfA(message, "original-CCPU HALT RESET: AX=%04x producer=%lu\n", ax, producer_result);
    report(message);
    /* BEEF proves execution restarted at the actual c_cpu_reset vector;
     * DEAD means it merely left HLT and continued after the instruction. */
    return ax == 0xbeef && producer_result == 0 ? 0 : 1;
}
