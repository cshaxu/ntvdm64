#include <stdint.h>
#include <stdio.h>
#include <windows.h>

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
extern void c_cpu_init(void);
extern uint16_t c_getIP(void);
extern void load_sw_cpu_access_functions(void);
extern void (*host_simulate_func)(void);
extern void host_start_cpu(void);

int main(void)
{
    /* `D6 FE` is the original CCPU BOP-FE exit instruction.  c_main.c
     * advances IP before calling c_cpu_unsimulate(), which returns through
     * the original CCPU TLS simulation frame. */
    SetUnhandledExceptionFilter(fixture_unhandled_exception);
    fputs("sas-init\n", stderr);
    sas_init(UINT32_C(0x00200000));
    fputs("cpu-init\n", stderr);
    c_cpu_init();
    fputs("access-init\n", stderr);
    load_sw_cpu_access_functions();
    fputs("seed\n", stderr);
    c_sas_store(UINT32_C(0x000ffff0), UINT8_C(0xd6));
    c_sas_store(UINT32_C(0x000ffff1), UINT8_C(0xfe));
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
    host_start_cpu();
    fputs("returned\n", stderr);
    if (c_getIP() != UINT16_C(0xfff2)) {
        fprintf(stderr, "CCPU did not return through BOP FE: IP=%04x\n",
            (unsigned)c_getIP());
        sas_term();
        return 1;
    }
    sas_term();
    return 0;
}
