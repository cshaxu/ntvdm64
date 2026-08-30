#include <stdint.h>
#include <stdio.h>
#include <setjmp.h>
#include <windows.h>

#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_execution.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_physical_mapping.h"

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
extern uint8_t *c_GetPhyAdd(uint32_t address);
extern void c_cpu_init(void);
extern uint16_t c_getIP(void);
extern void c_setIP(uint16_t value);
extern uintptr_t c_setDS(uint16_t value);
extern void load_sw_cpu_access_functions(void);
extern void (*host_simulate_func)(void);
extern void host_start_cpu(void);
extern void host_simulate(void);
extern jmp_buf *ccpu386ThrdExptnPtr(void);
extern NTSTATUS VdmAddVirtualMemory(ULONG host_address, ULONG size,
    PULONG intel_address);
extern NTSTATUS VdmRemoveVirtualMemory(ULONG intel_address);

int main(void)
{
    /* The program uses ordinary original CCPU decode and SAS RAM access
     * before its `D6 FE` exit.  `c_main.c` advances IP before calling
     * c_cpu_unsimulate(), which returns through the original CCPU TLS
     * simulation frame. */
    SetUnhandledExceptionFilter(fixture_unhandled_exception);
    /* Before SAS/CCPU initialization there is no TLS simulation context.
     * The original pointer-returning exception-frame API must decline this
     * state with a typed null result, never an undefined native pointer. */
    if (ccpu386ThrdExptnPtr() != NULL) {
        fputs("exception-frame accessor accepted an uninitialized TLS context\n", stderr);
        return 1;
    }
    fputs("sas-init\n", stderr);
    sas_init(UINT32_C(0x00200000));
    {
        session physical_owner;
        uint8_t external_page_storage[4097] = { 0 };
        uint8_t *external_page = &external_page_storage[1];
        uint8_t *resolved_page = NULL;
        uint32_t mapping_identifier;
        ULONG intel_address = 0u;

        session_initialize(&physical_owner, 2u);
        if (!session_activate(&physical_owner) ||
            !session_thread_bind(&physical_owner) ||
            !mvdm_softpc_physical_mapping_publish(external_page,
                UINT32_C(4096), &mapping_identifier) ||
            VdmAddVirtualMemory(mapping_identifier, UINT32_C(4096),
                &intel_address) != STATUS_SUCCESS) {
            fputs("external physical-page binding setup failed\n", stderr);
            sas_term();
            return 1;
        }
        if (mvdm_softpc_physical_mapping_prepare(mapping_identifier, 0u,
                NULL) || mvdm_softpc_physical_mapping_resolve(
                UINT32_C(0xffffffff), &resolved_page)) {
            fputs("external physical-page binding accepted invalid span\n", stderr);
            (void)session_thread_unbind(&physical_owner);
            (void)session_dispose(&physical_owner);
            sas_term();
            return 1;
        }
        /* `c_GetPhyAdd` is the selected original CCPU physical-access
         * operation.  `c_sas_store` is a linear BIOS/SAS entry and is not a
         * proof that the external physical page was selected. */
        *c_GetPhyAdd(intel_address) = UINT8_C(0x6d);
        if (external_page[0] != UINT8_C(0x6d) ||
            *c_GetPhyAdd(intel_address) != UINT8_C(0x6d) ||
            VdmRemoveVirtualMemory(intel_address) != STATUS_SUCCESS ||
            session_guest_memory_mappings(&physical_owner)->active_count != 0u ||
            mvdm_softpc_physical_mapping_prepare(mapping_identifier,
                UINT32_C(4096), NULL) ||
            mvdm_softpc_physical_mapping_resolve(intel_address,
                &resolved_page)) {
            fputs("external physical-page binding did not remain live and release\n",
                stderr);
            (void)session_thread_unbind(&physical_owner);
            (void)session_dispose(&physical_owner);
            sas_term();
            return 1;
        }
        if (!session_thread_unbind(&physical_owner) ||
            !session_dispose(&physical_owner)) {
            fputs("external physical-page binding teardown failed\n", stderr);
            sas_term();
            return 1;
        }
    }
    fputs("cpu-init\n", stderr);
    c_cpu_init();
    fputs("access-init\n", stderr);
    load_sw_cpu_access_functions();
    fputs("seed\n", stderr);
    (void)c_setDS(0u);
    /* mov al,5ah; mov [8000h],al; fld1; fstp dword [8004h]; d6 fe */
    c_sas_store(UINT32_C(0x000ffff0), UINT8_C(0xb0));
    c_sas_store(UINT32_C(0x000ffff1), UINT8_C(0x5a));
    c_sas_store(UINT32_C(0x000ffff2), UINT8_C(0xa2));
    c_sas_store(UINT32_C(0x000ffff3), UINT8_C(0x00));
    c_sas_store(UINT32_C(0x000ffff4), UINT8_C(0x80));
    c_sas_store(UINT32_C(0x000ffff5), UINT8_C(0xd9));
    c_sas_store(UINT32_C(0x000ffff6), UINT8_C(0xe8));
    c_sas_store(UINT32_C(0x000ffff7), UINT8_C(0xd9));
    c_sas_store(UINT32_C(0x000ffff8), UINT8_C(0x1e));
    c_sas_store(UINT32_C(0x000ffff9), UINT8_C(0x04));
    c_sas_store(UINT32_C(0x000ffffa), UINT8_C(0x80));
    c_sas_store(UINT32_C(0x000ffffb), UINT8_C(0xd6));
    c_sas_store(UINT32_C(0x000ffffc), UINT8_C(0xfe));
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
    if (c_getIP() != UINT16_C(0xfffd) ||
        c_sas_hw_at(UINT32_C(0x00008000)) != UINT8_C(0x5a) ||
        c_sas_hw_at(UINT32_C(0x00008004)) != UINT8_C(0x00) ||
        c_sas_hw_at(UINT32_C(0x00008005)) != UINT8_C(0x00) ||
        c_sas_hw_at(UINT32_C(0x00008006)) != UINT8_C(0x80) ||
        c_sas_hw_at(UINT32_C(0x00008007)) != UINT8_C(0x3f)) {
        fprintf(stderr, "CCPU host_start_cpu did not execute original RAM/FPU writes: IP=%04x\n",
            (unsigned)c_getIP());
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
    if (c_getIP() != UINT16_C(0xfffd) ||
        c_sas_hw_at(UINT32_C(0x00008000)) != UINT8_C(0x5a) ||
        c_sas_hw_at(UINT32_C(0x00008004)) != UINT8_C(0x00) ||
        c_sas_hw_at(UINT32_C(0x00008005)) != UINT8_C(0x00) ||
        c_sas_hw_at(UINT32_C(0x00008006)) != UINT8_C(0x80) ||
        c_sas_hw_at(UINT32_C(0x00008007)) != UINT8_C(0x3f)) {
        fprintf(stderr, "CCPU host_simulate did not execute original RAM/FPU writes: IP=%04x\n",
            (unsigned)c_getIP());
        sas_term();
        return 1;
    }
    sas_term();
    return 0;
}
