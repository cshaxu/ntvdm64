/* Selected CCPU x87 segment-limit witness, one case per process. */
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <windows.h>
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <sas.h>
#include <c_reg.h>
#include "ntvdm-exe/session/session.h"

extern void sas_init(PHY_ADDR), c_cpu_init(void), phy_w32(IU32, IU32);
extern NTSTATUS VdmAllocateVirtualMemory(PULONG, ULONG, BOOL);
extern void host_simulate(void);
extern void c_sas_fills(IU32, IU8, IU32);
extern void c_sas_loads(IU32, IU8 *, IU32);
extern void c_setCR0(IU32), c_setCPL(ISM32), c_setGDT_BASE_LIMIT(IU32, IU16);
extern IU32 c_getCR0(void);
extern ISM32 c_setCS(IU16);
extern uintptr_t c_setDS(uint16_t);
extern void c_setIP(IU16);
extern IU16 c_getIP(void);

static jmp_buf boundary;
static int vector = -1, error_code;
static IU16 fault_ip;
BOOL host_exint_hook(IS32 number, IS32 error)
{
    vector = number;
    error_code = error;
    fault_ip = c_getIP();
    longjmp(boundary, 1);
    return FALSE;
}

int main(int argc, char **argv)
{
    session owner;
    ULONG gdt = 0;
    IU8 program[10] = {0xdb, 0xe3}; /* FNINIT */
    IU8 memory[128];
    unsigned index = 2, i, span, limit;
    int which = argc == 2 ? atoi(argv[1]) : -1;
    int is32, is_restore, short_limit;
    if (which < 0 || which >= 8) return 2;
    is32 = (which & 1) != 0;
    is_restore = (which & 2) != 0;
    short_limit = (which & 4) != 0;
    span = is32 ? 108u : 94u;
    limit = 0x1000u + span - 1u - (short_limit ? 1u : 0u);
    if (is32) program[index++] = 0x66;
    program[index++] = 0xdd;
    program[index++] = is_restore ? 0x26 : 0x36; /* FRSTOR/FNSAVE [disp16] */
    program[index++] = 0x00;
    program[index++] = 0x10;
    program[index++] = 0xd6;
    program[index++] = 0xfe;
    session_initialize(&owner, 41);
    if (!session_activate(&owner) || !session_thread_bind(&owner)) return 2;
    sas_init(0x200000);
    c_cpu_init();
    if (VdmAllocateVirtualMemory(&gdt, 4096, TRUE) != 0) return 2;
    c_sas_fills(gdt, 0, 4096);
    phy_w32(gdt + 0x18, 0x0000ffffu);
    phy_w32(gdt + 0x1c, 0x0000fa00u);
    phy_w32(gdt + 0x20, limit);
    phy_w32(gdt + 0x24, 0x0000f200u);
    for (i = 0; i < index; ++i) c_sas_store(0x8000u + i, program[i]);
    c_sas_fills(0x1000u, 0, sizeof(memory));
    c_setCR0(c_getCR0() | 1u);
    c_setCPL(3);
    c_setGDT_BASE_LIMIT(gdt, 0x27u);
    (void)c_setCS(0x1bu);
    (void)c_setDS(0x23u);
    c_setIP(0x8000u);
    if (!setjmp(boundary)) host_simulate();
    fprintf(stderr, "case=%d bits=%d restore=%d short=%d vector=%d error=%d ip=%04x\n",
        which, is32 ? 32 : 16, is_restore, short_limit, vector, error_code, fault_ip);
    if (short_limit && (vector != 13 || error_code != 0 || fault_ip != 0x8002u)) return 1;
    if (!short_limit && vector != -1) return 1;
    c_sas_loads(0x1000u, memory, sizeof(memory));
    if (short_limit && !is_restore) {
        for (i = 0; i < sizeof(memory); ++i) if (memory[i]) return 1;
    }
    return 0;
}
