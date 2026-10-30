/* Actual CCPU decoder and IOS routing with a controlled byte device. */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <setjmp.h>
#include <windows.h>
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <sas.h>
#include <ios.h>
#include "ntvdm-exe/session/session.h"

extern void sas_init(PHY_ADDR), c_cpu_init(void), phy_w32(IU32, IU32);
extern IU32 phy_r32(PHY_ADDR);
extern NTSTATUS VdmAllocateVirtualMemory(PULONG, ULONG, BOOL);
extern void host_simulate(void), c_sas_fills(IU32, IU8, IU32);
extern void c_sas_store(IU32, IU8), c_setCR0(IU32), c_setCR3(IU32);
extern IU8 c_sas_hw_at(IU32);
extern void c_setCPL(ISM32), c_setGDT_BASE_LIMIT(IU32, IU16);
extern IU32 c_getCR0(void);
extern IU32 c_getEAX(void);
extern IU32 c_getCR2(void);
extern IU16 c_getIP(void);
extern ISM32 c_setCS(IU16);
extern uintptr_t c_setDS(uint16_t);
extern void c_setIP(IU16);
extern void c_setTR_SELECTOR(IU16), c_setTR_BASE_LIMIT_AR(IU32, IU32, IU16);

static jmp_buf boundary;
static int vector = -1, error_code;
static IU16 fault_ip;
static IU32 fault_address;
static unsigned reads, writes;
static half_word first;
BOOL host_exint_hook(IS32 number, IS32 error)
{
    vector = number; error_code = error; fault_ip = c_getIP();
    fault_address = c_getCR2(); longjmp(boundary, 1);
    return FALSE;
}
static void fixture_read(io_addr port, half_word *value)
{ (void)port; ++reads; *value = 0xa5; }
static void fixture_write(io_addr port, half_word value)
{ (void)port; if (!writes) first = value; ++writes; }

int main(int argc, char **argv)
{
    /* One OUT precedes one IN; D6 FE is the fixture stop BOP. */
    IU8 program[32] = {0xba,0x00,0x22};
    const half_word adapter = IO_MAX_NUMBER_ADAPTORS - 1;
    session owner;
    ULONG allocation = 0;
    IU32 directory, old_cr0;
    unsigned i, used = 3, width, port, repeat, fault_offset;
    int form;
    int scenario = argc == 5 ? atoi(argv[1]) : -1, ring, allowed, ok;
    width = argc == 5 ? (unsigned)atoi(argv[2]) : 0;
    form = argc == 5 ? atoi(argv[3]) : -1;
    repeat = argc == 5 ? (unsigned)atoi(argv[4]) : 3;
    if (scenario < 0 || scenario > 10 || (width != 1 && width != 2 && width != 4) ||
            form < 0 || form > 3 || (form == 2 ? repeat > 2 : repeat != 1) ||
            (scenario >= 7 && (form != 2 || repeat != 2))) return 2;
    port = form == 1 ? 0x80u : form == 3 ? 0x2207u : 0x2200u;
    program[1] = (IU8)port; program[2] = (IU8)(port >> 8);
    ring = scenario ? 3 : 0;
    allowed = scenario == 0 || scenario == 2;
    if (form == 2) {
        program[used++] = 0xbe; program[used++] = 0x00; program[used++] = 0x90;
        program[used++] = 0xbf; program[used++] = 0x00; program[used++] = 0x91;
        program[used++] = 0xb9; program[used++] = (IU8)repeat; program[used++] = 0;
    } else {
        program[used++] = 0x66; program[used++] = 0xb8; program[used++] = 0x5a;
        program[used++] = 0; program[used++] = 0; program[used++] = 0;
    }
    fault_offset = used;
    if (form == 2) program[used++] = 0xf3;
    if (width == 4) program[used++] = 0x66;
    program[used++] = scenario == 7 || scenario == 9 ? (width == 1 ? 0x6c : 0x6d) :
                      form == 1 ? (width == 1 ? 0xe6 : 0xe7) :
                      form == 2 ? (width == 1 ? 0x6e : 0x6f) : (width == 1 ? 0xee : 0xef);
    if (form == 1) program[used++] = (IU8)port;
    if (scenario < 7) {
        if (form == 2) { program[used++] = 0xb9; program[used++] = (IU8)repeat; program[used++] = 0; }
        if (form == 2) program[used++] = 0xf3;
        if (width == 4) program[used++] = 0x66;
        program[used++] = form == 1 ? (width == 1 ? 0xe4 : 0xe5) :
                          form == 2 ? (width == 1 ? 0x6c : 0x6d) : (width == 1 ? 0xec : 0xed);
        if (form == 1) program[used++] = (IU8)port;
    }
    program[used++] = 0xd6; program[used++] = 0xfe;
    session_initialize(&owner, 41);
    if (!session_activate(&owner) || !session_thread_bind(&owner)) return 2;
    sas_init(0x200000); c_cpu_init(); io_init();
    io_define_inb(adapter, fixture_read); io_define_outb(adapter, fixture_write);
    for (i = 0; i < 4; ++i) io_connect_port((io_addr)(port + i), adapter, IO_READ | IO_WRITE);
    if (VdmAllocateVirtualMemory(&allocation, 5u * 4096u, TRUE) != 0) return 2;
    c_sas_fills(allocation, 0, 5u * 4096u);
    directory = allocation;
    phy_w32(directory, (allocation + 3u * 4096u) | 7u);
    for (i = 0; i < 512u; ++i)
        phy_w32(allocation + 3u * 4096u + i * 4u, (i * 4096u) | 7u);
    phy_w32(allocation + 4u * 4096u + 0x18u, 0x0000ffffu);
    phy_w32(allocation + 4u * 4096u + 0x1cu, ring ? 0x0000fa00u : 0x00009a00u);
    phy_w32(allocation + 4u * 4096u + 0x20u, 0x0000ffffu);
    phy_w32(allocation + 4u * 4096u + 0x24u, ring ? 0x00cff200u : 0x00cf9200u);
    for (i = 0; i < used; ++i) c_sas_store(0x8000u + i, program[i]);
    c_sas_fills(0x9000u, 0, 0x200u); c_sas_store(0x9000u, 0x5a);
    if (scenario == 7 || scenario == 10) phy_w32(allocation + 3u * 4096u + 9u * 4u, 0x9000u | 5u);
    if (scenario == 8 || scenario == 9) phy_w32(allocation + 3u * 4096u + 9u * 4u, 0);
    old_cr0 = c_getCR0(); c_setCR3(directory); c_setCR0(old_cr0 | 0x80000001u);
    c_setCPL(ring); c_setGDT_BASE_LIMIT(allocation + 4u * 4096u, 0x27u);
    (void)c_setCS(ring ? 0x1bu : 0x18u); (void)c_setDS(ring ? 0x23u : 0x20u);
    { extern uintptr_t c_setES(uint16_t); (void)c_setES(ring ? 0x23u : 0x20u); }
    if (scenario >= 2) {
        IU32 tss = allocation + 2u * 4096u;
        IU32 map_offset = 0x80u + port / 8u;
        unsigned first_bit = port & 7u, last_bit = first_bit + width - 1u;
        c_sas_fills(tss, 0, 0x1000u);
        c_sas_store(tss + 0x66u, 0x80); c_sas_store(tss + 0x67u, 0);
        if (scenario == 3 || scenario == 9) c_sas_store(tss + map_offset, (IU8)(1u << first_bit));
        if (scenario == 6) c_sas_store(tss + map_offset + last_bit / 8u, (IU8)(1u << (last_bit & 7u)));
        c_setTR_SELECTOR(0x28);
        c_setTR_BASE_LIMIT_AR(tss, scenario == 4 ? map_offset : 0xfffu,
                              scenario == 5 ? 0x83u : 0x8bu);
    }
    c_setIP(0x8000u);
    if (!setjmp(boundary)) host_simulate();
    if (scenario == 7 || scenario == 8)
        ok = vector == 14 && error_code == (scenario == 7 ? 7 : 4) &&
             fault_ip == 0x800cu && fault_address == (scenario == 7 ? 0x9100u : 0x9000u) &&
             !reads && !writes;
    else if (scenario == 9)
        ok = vector == 13 && error_code == 0 && fault_ip == 0x800cu && !reads && !writes;
    else if (scenario == 10)
        ok = vector == -1 && !reads && writes == width * repeat && first == 0x5a;
    else ok = allowed ? vector == -1 && reads == width * repeat && writes == width * repeat &&
        first == (repeat ? 0x5a : 0) && (form == 2 ?
        c_sas_hw_at(0x9100u) == (repeat ? 0xa5 : 0) &&
        (!repeat || c_sas_hw_at(0x9100u + width * repeat - 1u) == 0xa5) &&
        c_sas_hw_at(0x9100u + width * repeat) == 0 :
        (c_getEAX() & (width == 1 ? 0xffu : width == 2 ? 0xffffu : 0xffffffffu)) ==
        (width == 1 ? 0xa5u : width == 2 ? 0xa5a5u : 0xa5a5a5a5u)) :
        vector == 13 && error_code == 0 && fault_ip == 0x8000u + fault_offset && !reads && !writes;
    if (scenario >= 7)
        ok = ok && (phy_r32(0x9000u) & 0xffu) == 0x5a && phy_r32(0x9100u) == 0 &&
             phy_r32(0x9104u) == 0 && phy_r32(0x9108u) == 0;
    fprintf(stderr, "CCPU_PORT_PRIVILEGE case=%d ring=%d width=%u form=%d repeat=%u vector=%d error=%d ip=%04x reads=%u writes=%u first=%02x result=%d cr2=%08lx\n",
        scenario, ring, width, form, repeat, vector, error_code, fault_ip, reads, writes, first, ok, (unsigned long)fault_address);
    return ok ? 0 : 1;
}
