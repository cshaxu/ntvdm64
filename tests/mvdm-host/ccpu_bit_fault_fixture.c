/* Per-process protected-mode bit-operation and page-fault observation. */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <setjmp.h>
#include <windows.h>
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <sas.h>
#include <c_reg.h>
#include "ntvdm-exe/session/session.h"

extern void sas_init(PHY_ADDR), sas_term(void), c_cpu_init(void), phy_w32(IU32, IU32);
extern IU32 phy_r32(PHY_ADDR);
extern NTSTATUS VdmAllocateVirtualMemory(PULONG, ULONG, BOOL);
extern void host_simulate(void);
extern void c_sas_fills(IU32, IU8, IU32);
extern void c_setCR0(IU32), c_setCR3(IU32), c_setCPL(ISM32), c_setGDT_BASE_LIMIT(IU32, IU16);
extern IU32 c_getCR0(void);
extern int32_t c_getCF(void);
extern void c_setCF(int32_t);
extern ISM32 c_setCS(IU16);
extern uintptr_t c_setDS(uint16_t);
extern void c_setIP(IU16);

static jmp_buf boundary;
static int vector = -1, error_code, captured_cf;
static IU32 captured_cr2;
BOOL host_exint_hook(IS32 number, IS32 error)
{
    vector = number;
    error_code = error;
    captured_cf = c_getCF();
    captured_cr2 = GET_CR(CR_PFLA);
    longjmp(boundary, 1);
    return FALSE;
}

int main(int argc, char **argv)
{
    static const IU8 program[] = {
        0x66,0xbb,0x00,0x10,0xfe,0x7f, /* EBX = 7ffe1000h */
        0x66,0xb9,0xff,0xff,0xff,0xff, /* ECX = -1 */
        0x67,0x66,0x0f,0xa3,0x0b,      /* BT [EBX],ECX; optionally BTC */
        0xd6,0xfe
    };
    session owner;
    ULONG allocation = 0;
    IU32 directory, table, first, second, old_cr0;
    unsigned i;
    int which = argc == 2 ? atoi(argv[1]) : -1;
    int result;
    if (which < 0 || which > 12) return 2;
    session_initialize(&owner, 41);
    if (!session_activate(&owner) || !session_thread_bind(&owner)) return 2;
    sas_init(0x200000);
    c_cpu_init();
    if (VdmAllocateVirtualMemory(&allocation, 6u * 4096u, TRUE) != 0) return 2;
    directory = allocation; table = allocation + 4096u;
    first = allocation + 8192u; second = allocation + 5u * 4096u;
    c_sas_fills(directory, 0, 6u * 4096u);
    phy_w32(directory + ((0x7ffe0000u >> 22) * 4u), table | 7u);
    phy_w32(table + (((0x7ffe0000u >> 12) & 1023u) * 4u),
        which == 2 ? 0 : first | (which >= 7 ? 7u : 5u));
    phy_w32(table + (((0x7ffe1000u >> 12) & 1023u) * 4u),
        (which == 8 || which == 10 || which == 12) ? 0 :
        second | ((which == 7 || which == 9 || which == 11) ? 5u : 7u));
    phy_w32(first + 4092u, 0x80000000u);
    phy_w32(second, 0x12345678u);
    phy_w32(directory, (allocation + 3u * 4096u) | 7u);
    for (i = 0; i < 512u; ++i)
        phy_w32(allocation + 3u * 4096u + i * 4u, (i * 4096u) | 7u);
    phy_w32(allocation + 4u * 4096u + 0x18u, 0x0000ffffu);
    phy_w32(allocation + 4u * 4096u + 0x1cu, 0x0000fa00u);
    phy_w32(allocation + 4u * 4096u + 0x20u, 0x0000ffffu);
    phy_w32(allocation + 4u * 4096u + 0x24u, 0x00cff200u);
    for (i = 0; i < sizeof(program); ++i) {
        IU8 byte = program[i];
        /* With ECX=-1, bases 1001h, 1002h and 1003h select dwords
         * beginning at ffdh, ffeh and fffh respectively. */
        if (i == 2u && which >= 5) byte = which >= 11 ? 3u : which >= 9 ? 1u : 2u;
        if (i == 15u && which && which != 5)
            byte = which == 3 ? 0xb3u : which == 4 ? 0xabu : 0xbbu;
        c_sas_store(0x8000u + i, byte);
    }
    old_cr0 = c_getCR0();
    c_setCR3(directory); c_setCR0(old_cr0 | 0x80000001u); c_setCPL(3);
    c_setGDT_BASE_LIMIT(allocation + 4u * 4096u, 0x27u);
    (void)c_setCS(0x1bu); (void)c_setDS(0x23u); c_setIP(0x8000u);
    c_setCF(which == 0 || which == 5 ? 0 : 1);
    if (!setjmp(boundary)) host_simulate();
    /* Original usr_chk_dword tests the last byte of a split access. This
     * asserts the retained CCPU policy, not a claim about every x86 CPU. */
    result = which == 0 ? (vector == -1 && c_getCF() == 1) :
        which == 5 ? (vector == -1 && c_getCF() == 0) :
        (vector == 14 && captured_cf == 1 && captured_cr2 ==
         (which == 6 ? 0x7ffe0ffeu : which == 7 || which == 8 ? 0x7ffe1001u :
          which == 9 || which == 10 ? 0x7ffe1000u :
          which >= 11 ? 0x7ffe1002u : 0x7ffe0ffcu) &&
         (error_code & 6) == 6 &&
         ((error_code & 1) == ((which == 2 || which == 8 || which == 10 || which == 12) ? 0 : 1)));
    result = result && phy_r32(first + 4092u) == 0x80000000u &&
        phy_r32(second) == 0x12345678u;
    fprintf(stderr, "bit-fault case=%d vector=%d error=%d cf=%d cr2=%08lx result=%d\n",
        which, vector, error_code, which == 0 || which == 5 ? (int)c_getCF() : captured_cf,
        (unsigned long)captured_cr2, result);
    /* Fault hooks longjmp past the original exception cleanup; do not resume
     * this CPU. The fixture runs each case in its own process. */
    return result ? 0 : 1;
}
