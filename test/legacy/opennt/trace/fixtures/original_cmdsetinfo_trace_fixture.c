#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "historical_bios_bridge_v1.h"
#include "cmd.h"

typedef void (*runner_bios_entry)(void);

extern runner_bios_entry BIOS[];
extern uint8_t *runner_ccpu_sm0_ram(void);
extern void setBX(uint16_t value);
extern void setCX(uint16_t value);
extern void setDX(uint16_t value);
extern void setCS(uint16_t value);
extern void setDS(uint16_t value);
extern void setIP(uint16_t value);
extern uint16_t getIP(void);

typedef LONG (WINAPI *runner_vectored_exception_handler)(
    EXCEPTION_POINTERS *exception_pointers);
extern PVOID WINAPI AddVectoredExceptionHandler(
    ULONG first_handler,
    runner_vectored_exception_handler handler);
extern ULONG WINAPI RemoveVectoredExceptionHandler(PVOID handle);

enum {
    runner_data_segment = 0x0100u,
    runner_data_base = runner_data_segment << 4,
    runner_scs_offset = 0x0010u,
    runner_binary_offset = 0x0100u,
    runner_fdaccess_offset = 0x0120u
};

static LONG WINAPI runner_cmdsetinfo_trace_exception(
    EXCEPTION_POINTERS *exception_pointers)
{
    char line[160];
    DWORD written;
    uintptr_t module_base = (uintptr_t)GetModuleHandleA(NULL);
    uintptr_t instruction = (uintptr_t)exception_pointers->ContextRecord->Eip;

    _snprintf(line, sizeof(line),
              "cmdsetinfo fixture: exception=%08lx eip-rva=%08lx fault-rva=%08lx\r\n",
              (unsigned long)exception_pointers->ExceptionRecord->ExceptionCode,
              (unsigned long)(instruction - module_base),
              (unsigned long)((uintptr_t)exception_pointers->ExceptionRecord->ExceptionAddress - module_base));
    line[sizeof(line) - 1u] = '\0';
    WriteFile(GetStdHandle(STD_ERROR_HANDLE), line, (DWORD)strlen(line),
              &written, NULL);
    return EXCEPTION_CONTINUE_SEARCH;
}

int main(void)
{
    uint8_t *ram;
    uintptr_t scs;
    uintptr_t binary;
    uintptr_t fdaccess;
    int result = 0;
    PVOID exception_handler;

    exception_handler = AddVectoredExceptionHandler(1u,
        runner_cmdsetinfo_trace_exception);
    if (exception_handler == NULL) return 1;
    if (!runner_historical_bios_bridge_v1_initialize()) return 1;
    ram = runner_ccpu_sm0_ram();
    if (ram == NULL) {
        result = 2;
        goto cleanup;
    }

    setDX(runner_scs_offset);
    setBX(runner_binary_offset);
    setCX(runner_fdaccess_offset);
    setDS(runner_data_segment);
    setCS(0x0070u);
    setIP(0x0478u);
    ram[0x0b78u] = 0x05u;
    BIOS[0x54u]();

    /* NT4 x86 softpc.h defines GetVDMAddr as the linear guest value itself. */
    scs = runner_data_base + runner_scs_offset;
    binary = runner_data_base + runner_binary_offset;
    fdaccess = runner_data_base + runner_fdaccess_offset;

    fprintf(stderr, "cmdsetinfo fixture: ip=%04x scs=%p sync=%p binary=%p fdaccess=%p\n",
            getIP(), pSCSInfo, pSCS_ToSync, pIsDosBinary, pFDAccess);
    if (getIP() != 0x0479u ||
        (uintptr_t)pSCSInfo != scs ||
        (uintptr_t)pSCS_ToSync != scs +
                               offsetof(SCSINFO, SCS_ToSync) ||
        (uintptr_t)pIsDosBinary != binary ||
        (uintptr_t)pFDAccess != fdaccess) {
        result = 4;
    }

cleanup:
    runner_historical_bios_bridge_v1_terminate();
    RemoveVectoredExceptionHandler(exception_handler);
    return result;
}
