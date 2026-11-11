/* Selected CCPU x87 cross-page witness, one case per process. */
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
extern IU32 phy_r32(PHY_ADDR);
extern NTSTATUS VdmAllocateVirtualMemory(PULONG, ULONG, BOOL);
extern void host_simulate(void), c_sas_fills(IU32, IU8, IU32);
extern void c_setCR0(IU32), c_setCR3(IU32), c_setCPL(ISM32), c_setGDT_BASE_LIMIT(IU32, IU16);
extern IU32 c_getCR0(void);
extern ISM32 c_setCS(IU16);
extern uintptr_t c_setDS(uint16_t);
extern void c_setIP(IU16);

static jmp_buf boundary;
static int vector = -1, error_code;
static IU32 fault_address;
BOOL host_exint_hook(IS32 number, IS32 error)
{
    vector = number; error_code = error; fault_address = GET_CR(CR_PFLA);
    longjmp(boundary, 1);
    return FALSE;
}

int main(int argc, char **argv)
{
    session owner;
    ULONG allocation = 0;
    IU32 dir, table, identity, gdt, first, second, guard;
    IU8 program[16] = {0xdb, 0xe3}; /* FNINIT */
    unsigned index = 2, i, group, span;
    int which = argc == 2 ? atoi(argv[1]) : -1;
    int is32, is_restore, absent, readonly, first_supervisor, fault;
    if (which < 0 || which >= 128 || ((which & 32) && (which & 4)) ||
            ((which & 64) && (which & (32 | 4)))) return 2;
    group = ((unsigned)which >> 3) & 3u;
    is32 = (which & 1) != 0; is_restore = (which & 2) != 0;
    absent = (which & 4) != 0;
    readonly = (which & 32) != 0;
    first_supervisor = (which & 64) != 0;
    fault = first_supervisor || absent || (readonly && !is_restore);
    span = group == 0 ? (is32 ? 108u : 94u) :
        group == 1 ? (is32 ? 28u : 14u) : group == 2 ? 8u : 10u;
    if (is32) program[index++] = 0x66;
    program[index++] = 0x67;
    program[index++] = group == 1 ? 0xd9 : group == 3 ? 0xdb : 0xdd;
    program[index++] = group == 0 || group == 1 ? (is_restore ? 0x25 : 0x35) :
        group == 2 ? (is_restore ? 0x05 : 0x15) : (is_restore ? 0x2d : 0x3d);
    program[index++] = 0xfc; program[index++] = 0x0f;
    program[index++] = 0xfe; program[index++] = 0x7f;
    if (group == 2 && is_restore && !fault) {
        program[index++] = 0xdd; program[index++] = 0x1e; /* FSTP qword [1200h] */
        program[index++] = 0x00; program[index++] = 0x12;
    }
    program[index++] = 0xd6; program[index++] = 0xfe;
    session_initialize(&owner, 41);
    if (!session_activate(&owner) || !session_thread_bind(&owner)) return 2;
    sas_init(0x200000); c_cpu_init();
    if (VdmAllocateVirtualMemory(&allocation, 10u * 4096u, TRUE) != 0) return 2;
    dir = allocation; table = allocation + 4096u; identity = allocation + 2u * 4096u;
    gdt = allocation + 3u * 4096u; first = allocation + 4u * 4096u;
    guard = allocation + 5u * 4096u; second = allocation + 7u * 4096u;
    c_sas_fills(allocation, 0, 10u * 4096u);
    c_sas_fills(guard, 0xa5, 4096);
    c_sas_fills(second, 0x5a, 4096);
    if (group == 2 && is_restore && !fault) phy_w32(second, 0x3ff00000u);
    phy_w32(dir + ((0x7ffe0000u >> 22) * 4u), table | 7u);
    phy_w32(table + (((0x7ffe0000u >> 12) & 1023u) * 4u), first | (first_supervisor ? 3u : 7u));
    phy_w32(table + (((0x7ffe1000u >> 12) & 1023u) * 4u), absent ? 0 : second | (readonly ? 5u : 7u));
    phy_w32(dir, identity | 7u);
    for (i = 0; i < 512u; ++i) phy_w32(identity + i * 4u, (i * 4096u) | 7u);
    phy_w32(gdt + 0x18u, 0x0000ffffu); phy_w32(gdt + 0x1cu, 0x0000fa00u);
    phy_w32(gdt + 0x20u, 0x0000ffffu); phy_w32(gdt + 0x24u, 0x00cff200u);
    for (i = 0; i < index; ++i) c_sas_store(0x8000u + i, program[i]);
    c_setCR3(dir); c_setCR0(c_getCR0() | 0x80000001u); c_setCPL(3);
    c_setGDT_BASE_LIMIT(gdt, 0x27u); (void)c_setCS(0x1bu); (void)c_setDS(0x23u); c_setIP(0x8000u);
    if (!setjmp(boundary)) host_simulate();
    fprintf(stderr, "case=%d group=%u span=%u bits=%d read=%d absent=%d readonly=%d first_supervisor=%d vector=%d error=%d cr2=%08lx guard=%08lx second=%08lx first_tail=%08lx\n",
        which, group, span, is32 ? 32 : 16, is_restore, absent, readonly, first_supervisor, vector, error_code,
        fault_address, phy_r32(guard), phy_r32(second), phy_r32(first + 4092));
    if (phy_r32(guard) != 0xa5a5a5a5u) return 1;
    if (fault) {
        if (vector != 14 || error_code != (first_supervisor ? (is_restore ? 5 : 7) :
                (is_restore ? 4 : (readonly ? 7 : 6))) ||
                fault_address != (first_supervisor ? 0x7ffe0ffcu : 0x7ffe0ffcu + span - 1u)) return 1;
        if (!is_restore && phy_r32(first + 4092) != 0) return 1;
    } else {
        if (vector != -1) return 1;
        if (!is_restore && phy_r32(second) == 0x5a5a5a5au) return 1;
        if (is_restore && phy_r32(second) !=
                (group == 2 ? 0x3ff00000u : 0x5a5a5a5au)) return 1;
        if (group == 2 && is_restore &&
                (phy_r32(0x1200u) != 0 || phy_r32(0x1204u) != 0x3ff00000u)) return 1;
    }
    return 0;
}
