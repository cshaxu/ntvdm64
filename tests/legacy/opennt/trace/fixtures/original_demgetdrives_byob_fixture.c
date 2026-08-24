#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "historical_bios_bridge_v1.h"

typedef void (*ntdos64_bios_entry)(void);

extern ntdos64_bios_entry BIOS[];
extern int DemInit(int argc, char *argv[]);
extern uint8_t *ntdos64_ccpu_sm0_ram(void);
extern void setCS(uint16_t value);
extern void setIP(uint16_t value);
extern uint16_t getAX(void);
extern uint16_t getIP(void);
extern uint32_t getCF(void);
extern uint8_t PhysicalDriveTypes[26];
extern uint16_t nDrives;
extern char IsAPresent;
extern char IsBPresent;

typedef LONG (WINAPI *ntdos64_vectored_exception_handler)(
    EXCEPTION_POINTERS *exception_pointers);
extern PVOID WINAPI AddVectoredExceptionHandler(
    ULONG first_handler,
    ntdos64_vectored_exception_handler handler);
extern ULONG WINAPI RemoveVectoredExceptionHandler(PVOID handle);

static LONG WINAPI ntdos64_demgetdrives_trace_exception(
    EXCEPTION_POINTERS *exception_pointers)
{
    char line[192];
    DWORD written;
    uintptr_t module_base = (uintptr_t)GetModuleHandleA(NULL);
    uintptr_t instruction = (uintptr_t)exception_pointers->ContextRecord->Eip;
    uintptr_t fault = (uintptr_t)exception_pointers->ExceptionRecord->ExceptionAddress;
    uintptr_t return_address = *(const uintptr_t *)(uintptr_t)
        exception_pointers->ContextRecord->Esp;

    _snprintf(line, sizeof(line),
              "demgetdrives fixture: exception=%08lx eip-rva=%08lx fault-rva=%08lx return-rva=%08lx\r\n",
              (unsigned long)exception_pointers->ExceptionRecord->ExceptionCode,
              (unsigned long)(instruction - module_base),
              (unsigned long)(fault - module_base),
              (unsigned long)(return_address - module_base));
    line[sizeof(line) - 1u] = '\0';
    WriteFile(GetStdHandle(STD_ERROR_HANDLE), line, (DWORD)strlen(line),
              &written, NULL);
    return EXCEPTION_CONTINUE_SEARCH;
}

int main(int argc, char *argv[])
{
    uint8_t *ram;
    unsigned int index;
    int result = 0;
    PVOID exception_handler;

    fprintf(stderr, "demgetdrives fixture: bridge init\n");
    exception_handler = AddVectoredExceptionHandler(1u,
        ntdos64_demgetdrives_trace_exception);
    if (exception_handler == NULL) return 1;
    if (!ntdos64_historical_bios_bridge_v1_initialize()) return 2;
    fprintf(stderr, "demgetdrives fixture: original DemInit\n");
    if (!DemInit(argc, argv)) {
        result = 4;
        goto cleanup;
    }

    ram = ntdos64_ccpu_sm0_ram();
    if (ram == NULL) {
        result = 8;
        goto cleanup;
    }

    /* MS_bop_0 reads the service byte at the current CS:IP. */
    setCS(0x0070u);
    setIP(0x0478u);
    ram[0x0b78u] = 0x0fu;
    fprintf(stderr, "demgetdrives fixture: BIOS selector 50 service 0f\n");
    BIOS[0x50u]();
    fprintf(stderr,
            "demgetdrives fixture: returned ip=%04x ax=%04x cf=%lu ndrives=%u A=%d B=%d\n",
            getIP(), getAX(), (unsigned long)getCF(), (unsigned int)nDrives,
            (int)IsAPresent, (int)IsBPresent);
    fprintf(stderr, "demgetdrives fixture: physical types");
    for (index = 0u; index < 26u; ++index) {
        fprintf(stderr, " %02x", (unsigned int)PhysicalDriveTypes[index]);
    }
    fprintf(stderr, "\n");

    if (getIP() != 0x0479u || getCF() != 0u ||
        (uint8_t)getAX() != (uint8_t)nDrives) {
        result = 16;
    }

cleanup:
    ntdos64_historical_bios_bridge_v1_terminate();
    RemoveVectoredExceptionHandler(exception_handler);
    return result;
}
