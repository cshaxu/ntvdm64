#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_execution.h"

static LONG WINAPI fixture_unhandled_exception(EXCEPTION_POINTERS *exception)
{
    uintptr_t address = (uintptr_t)exception->ExceptionRecord->ExceptionAddress;
    uintptr_t image = (uintptr_t)GetModuleHandleA(NULL);

    fprintf(stderr, "exception=%08lx address=%p rva=%#llx\n",
        (unsigned long)exception->ExceptionRecord->ExceptionCode,
        exception->ExceptionRecord->ExceptionAddress,
        (unsigned long long)(address - image));
    return EXCEPTION_EXECUTE_HANDLER;
}

/* These are the original CCPU/SoftPC public spellings.  The fixture does not
 * provide an executor, memory implementation, BOP handler, or stop hook. */
extern void sas_init(uint32_t size);
extern void sas_term(void);
extern void c_sas_store(uint32_t address, uint8_t value);
extern uint8_t c_sas_hw_at(uint32_t address);
extern void c_cpu_init(void);
extern uint16_t c_getIP(void);
extern void c_setIP(uint16_t value);
extern uintptr_t c_setDS(uint16_t value);
extern void load_sw_cpu_access_functions(void);
extern void (*host_simulate_func)(void);
extern void host_start_cpu(void);
extern void host_simulate(void);

int main(void)
{
    /* The program uses ordinary original CCPU decode and SAS RAM access
     * before its `D6 FE` exit.  `c_main.c` advances IP before calling
     * c_cpu_unsimulate(), which returns through the original CCPU TLS
     * simulation frame. */
    SetUnhandledExceptionFilter(fixture_unhandled_exception);
    fputs("sas-init\n", stderr);
    sas_init(UINT32_C(0x00200000));
    fputs("cpu-init\n", stderr);
    c_cpu_init();
    fputs("access-init\n", stderr);
    load_sw_cpu_access_functions();
    fputs("seed\n", stderr);
    (void)c_setDS(0u);
    /* mov al,5ah; mov [8000h],al; d6 fe */
    c_sas_store(UINT32_C(0x000ffff0), UINT8_C(0xb0));
    c_sas_store(UINT32_C(0x000ffff1), UINT8_C(0x5a));
    c_sas_store(UINT32_C(0x000ffff2), UINT8_C(0xa2));
    c_sas_store(UINT32_C(0x000ffff3), UINT8_C(0x00));
    c_sas_store(UINT32_C(0x000ffff4), UINT8_C(0x80));
    c_sas_store(UINT32_C(0x000ffff5), UINT8_C(0xd6));
    c_sas_store(UINT32_C(0x000ffff6), UINT8_C(0xfe));
    fputs("start\n", stderr);
    /* `nt_cprgs.c` selects the original CCPU executor through this historical
     * SoftPC CPU-access dispatch slot.  The fixture must not bypass that
     * original selection by calling c_cpu_simulate directly. */
    if (host_simulate_func == NULL) {
        fputs("original host_simulate selection was not installed\n", stderr);
        sas_term();
        return 1;
    }
    /* Enter through the original host wrapper rather than directly calling
     * the CCPU executor.  Under CPU_40_STYLE+CCPU its untouched source body
     * resolves cpu_simulate to c_cpu_simulate. */
    {
        session owner;
        session_initialize(&owner, 1u);
        if (!session_select_machine_backend(&owner, SESSION_MACHINE_BACKEND_SOFTPC) ||
            !session_activate(&owner) ||
            !mvdm_softpc_execution_run_until_return(&owner) ||
            session_mechanical_resume_status(&owner) !=
                SESSION_MECHANICAL_STATUS_SOFTPC_RETURNED) {
            fputs("typed SoftPC outer return was not recorded\n", stderr);
            sas_term();
            return 1;
        }
        (void)session_dispose(&owner);
    }
    fputs("returned-start\n", stderr);
    if (c_getIP() != UINT16_C(0xfff7) ||
        c_sas_hw_at(UINT32_C(0x00008000)) != UINT8_C(0x5a)) {
        fprintf(stderr, "CCPU host_start_cpu did not execute original RAM write: IP=%04x mem=%02x\n",
            (unsigned)c_getIP(), (unsigned)c_sas_hw_at(UINT32_C(0x00008000)));
        sas_term();
        return 1;
    }

    /* Re-enter through the original recursive SoftPC spelling.  The source
     * body preserves its historical lock/TEB guard, then invokes the same
     * CPU macro.  Reusing the direct exit keeps this a bounded mechanical
     * proof, not a BIOS callback claim. */
    c_setIP(UINT16_C(0xfff0));
    fputs("reenter\n", stderr);
    host_simulate();
    fputs("returned-recursive\n", stderr);
    if (c_getIP() != UINT16_C(0xfff7) ||
        c_sas_hw_at(UINT32_C(0x00008000)) != UINT8_C(0x5a)) {
        fprintf(stderr, "CCPU host_simulate did not execute original RAM write: IP=%04x mem=%02x\n",
            (unsigned)c_getIP(), (unsigned)c_sas_hw_at(UINT32_C(0x00008000)));
        sas_term();
        return 1;
    }
    sas_term();
    return 0;
}
