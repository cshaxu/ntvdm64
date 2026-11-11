#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "historical_bios_bridge_v1.h"
#include "demdasd.h"

typedef void (*runner_bios_entry)(void);

extern runner_bios_entry BIOS[];
extern int DemInit(int argc, char *argv[]);
extern uint8_t *runner_ccpu_sm0_ram(void);
extern uint8_t *c_GetLinAdd(uint32_t address);
extern void setBP(uint16_t value);
extern void setCS(uint16_t value);
extern void setES(uint16_t value);
extern void setIP(uint16_t value);
extern uint16_t getBP(void);
extern uint16_t getIP(void);
extern uint8_t PhysicalDriveTypes[26];

typedef LONG (WINAPI *runner_vectored_exception_handler)(
    EXCEPTION_POINTERS *exception_pointers);
extern PVOID WINAPI AddVectoredExceptionHandler(
    ULONG first_handler,
    runner_vectored_exception_handler handler);
extern ULONG WINAPI RemoveVectoredExceptionHandler(PVOID handle);

enum {
    runner_dpb_segment = 0x0200u,
    runner_dpb_offset = 0x0010u
};

static LONG WINAPI runner_demgetdpblist_trace_exception(
    EXCEPTION_POINTERS *exception_pointers)
{
    DWORD written;
    char line[160];
    uintptr_t module_base = (uintptr_t)GetModuleHandleA(NULL);

    _snprintf(line, sizeof(line),
              "demgetdpblist fixture: exception=%08lx eip-rva=%08lx fault-rva=%08lx\r\n",
              (unsigned long)exception_pointers->ExceptionRecord->ExceptionCode,
              (unsigned long)((uintptr_t)exception_pointers->ContextRecord->Eip - module_base),
              (unsigned long)((uintptr_t)exception_pointers->ExceptionRecord->ExceptionAddress - module_base));
    line[sizeof(line) - 1u] = '\0';
    WriteFile(GetStdHandle(STD_ERROR_HANDLE), line, (DWORD)strlen(line),
              &written, NULL);
    return EXCEPTION_CONTINUE_SEARCH;
}

int main(int argc, char *argv[])
{
    uint8_t *ram;
    DPB *dpb;
    unsigned int drive;
    unsigned int count = 0u;
    int result = 0;
    PVOID exception_handler;

    exception_handler = AddVectoredExceptionHandler(1u,
        runner_demgetdpblist_trace_exception);
    if (exception_handler == NULL) return 1;
    if (!runner_historical_bios_bridge_v1_initialize()) return 2;
    if (!DemInit(argc, argv)) {
        result = 3;
        goto cleanup;
    }

    ram = runner_ccpu_sm0_ram();
    dpb = (DPB *)c_GetLinAdd((runner_dpb_segment << 4) + runner_dpb_offset);
    if (ram == NULL || dpb == NULL) {
        result = 4;
        goto cleanup;
    }

    setCS(0x0070u);
    setIP(0x0478u);
    ram[0x0b78u] = 0x0fu;
    BIOS[0x50u]();
    if (getIP() != 0x0479u) {
        result = 5;
        goto cleanup;
    }

    setES(runner_dpb_segment);
    setBP(runner_dpb_offset);
    setIP(0x0478u);
    ram[0x0b78u] = 0x46u;
    BIOS[0x50u]();

    for (drive = 0u; drive < 26u; ++drive) {
        if (PhysicalDriveTypes[drive] == DRIVE_REMOVABLE ||
            PhysicalDriveTypes[drive] == DRIVE_FIXED) {
            uint32_t expected_next;
            unsigned int next_drive;
            int has_next = 0;
            if (dpb[count].DriveNum != drive || dpb[count].Unit != drive) {
                result = 6;
                goto cleanup;
            }
            for (next_drive = drive + 1u; next_drive < 26u; ++next_drive) {
                if (PhysicalDriveTypes[next_drive] == DRIVE_REMOVABLE ||
                    PhysicalDriveTypes[next_drive] == DRIVE_FIXED) {
                    has_next = 1;
                    break;
                }
            }
            expected_next = has_next ?
                (((uint32_t)runner_dpb_segment << 16) |
                 (runner_dpb_offset + (uint16_t)((count + 1u) * sizeof(DPB)))) :
                0xffffffffu;
            if (dpb[count].Next != (PDPB)(uintptr_t)expected_next) {
                fprintf(stderr,
                        "demgetdpblist fixture: next mismatch drive=%u index=%u actual=%08lx expected=%08lx\n",
                        drive, count, (unsigned long)(uintptr_t)dpb[count].Next,
                        (unsigned long)expected_next);
                result = 7;
                goto cleanup;
            }
            ++count;
        }
    }

    if (count == 0u || getIP() != 0x0479u ||
        getBP() != runner_dpb_offset + count * sizeof(DPB) ||
        dpb[count - 1u].Next != (PDPB)(uintptr_t)0xffffffffu) {
        result = 8;
        goto cleanup;
    }

    fprintf(stderr, "demgetdpblist fixture: ip=%04x count=%u bp=%04x size=%u\n",
            getIP(), count, getBP(), (unsigned int)sizeof(DPB));

cleanup:
    runner_historical_bios_bridge_v1_terminate();
    RemoveVectoredExceptionHandler(exception_handler);
    return result;
}
