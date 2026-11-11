/* Actual production CALLF/RETF/IRET entrypoints, production SAS and descriptor
 * validation. Only the host exception-observation hook is intercepted; no CPU,
 * stack, descriptor, memory or instruction implementation is supplied here. */
#include <stdio.h>
#include <setjmp.h>
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <sas.h>
#include <c_main.h>
#include <c_addr.h>
#include <c_reg.h>
#include <c_bsic.h>
#include <c_seg.h>
#include "ntvdm-exe/session/session.h"

extern void sas_init(PHY_ADDR size), sas_term(void), c_cpu_init(void);
extern void phy_w32(IU32 address, IU32 value);
extern void CALLF(IU32 *operand), RETF(IU32 adjustment), IRET(void);
extern void c_sas_storew(IU32 address, IU16 value);
extern IU32 c_sas_dw_at(IU32 address);
extern IU16 c_sas_w_at(IU32 address);
extern IU32 CCPU_save_EIP;

static jmp_buf fault_boundary;
static int expect_fault, fault_vector, failures, cases;
static IU32 fault_sp, fault_cpl, fault_ss, fault_cs, fault_ip;

/* Existing original host fault boundary, before CPU changes mode/delivers an
 * exception. This observes real validation, not a replacement validator. */
BOOL host_exint_hook(IS32 number, IS32 error)
{
    (void)error;
    fault_vector = number;
    fault_sp = GET_ESP();
    fault_cpl = GET_CPL();
    fault_ss = GET_SS_SELECTOR();
    fault_cs = GET_CS_SELECTOR();
    fault_ip = GET_EIP();
    if (!expect_fault) {
        fprintf(stderr, "unexpected fault %ld\n", (long)number);
    }
    longjmp(fault_boundary, 1);
    return FALSE;
}

static void descriptor(IU16 selector, IU32 base, IU32 limit, IU32 access, int big)
{
    phy_w32(0x1000 + (selector & ~7), (base << 16) | (limit & 0xffff));
    phy_w32(0x1004 + (selector & ~7), (base & 0xff000000) |
        ((base >> 16) & 0xff) | (access << 8) | (limit & 0xf0000) |
        (big ? 0x400000 : 0));
}

static void prepare(int operand, int target_big, int outer)
{
    int i;
    c_cpu_init();
    SET_PE(1); SET_VM(0); SET_PG(0);
    SET_GDT_BASE(0x1000); SET_GDT_LIMIT(0x100);
    descriptor(8, 0x20000, 0xffff, 0x9b, operand);
    descriptor(16, 0x30000, 0xffff, 0xfb, operand);
    descriptor(24, 0x40000, target_big ? 0x1ffff : 0xffff,
        outer ? 0xf3 : 0x93, target_big);
    descriptor(32, 0x60000, 0xffff, outer ? 0x93 : 0xf3, 0);
    SET_CPL(outer ? 0 : 3);
    load_code_seg(outer ? 8 : 19);
    load_stack_seg(outer ? 32 : 35);
    for (i = 0; i < 6; ++i) {
        if (i != CS_REG && i != SS_REG) SET_SR_SELECTOR(i, 0);
    }
    SET_EIP(0x1234); SET_ESP(0xa5a58000);
    CCPU_save_EIP = 0x1234;
    SET_OPERAND_SIZE(operand); SET_ADDRESS_SIZE(operand);
    SET_POP_DISP(0);
    SET_TR_AR_SUPER(XTND_BUSY_TSS); SET_TR_BASE(0x1800); SET_TR_LIMIT(0x67);
    phy_w32(0x1804, target_big ? 0x18000 : 0x8000);
    c_sas_storew(0x1808, 24);
    fault_vector = 0;
}

static void item(int operand, unsigned index, IU32 value)
{
    IU32 address = 0x68000 + index * (operand ? 4 : 2);
    if (operand) phy_w32(address, value);
    else c_sas_storew(address, (IU16)value);
}

static IU32 read_item(int operand, IU32 address)
{
    return operand ? c_sas_dw_at(address) : c_sas_w_at(address);
}

static void check(int ok, const char *name, int operand, int big)
{
    ++cases;
    if (!ok) {
        ++failures;
        fprintf(stderr, "FAIL %s operand=%d SS=%d ESP=%08lx CPL=%lu fault=%d\n",
            name, operand ? 32 : 16, big ? 32 : 16,
            (unsigned long)GET_ESP(), (unsigned long)GET_CPL(), fault_vector);
    }
}

static void call_case(int operand, int big, int bad)
{
    IU32 destination[2] = {0, 43};
    IU32 new_sp = big ? 0x18000 : 0x8000;
    IU32 step = operand ? 4 : 2;
    prepare(operand, big, 0);
    /* DPL3 gate to ring0 code, two parameters, 16/32 gate width. */
    phy_w32(0x1028, (8u << 16) | 0x2345);
    phy_w32(0x102c, ((operand ? 0xecu : 0xe4u) << 8) | 2);
    item(operand, 0, 0x4567); item(operand, 1, 0x5678);
    if (bad == 1) descriptor(24, 0x40000, 0xffff, 0x13, big);
    if (bad == 2) phy_w32(0x1804, 4); /* frame exceeds target stack */
    expect_fault = bad;
    if (!setjmp(fault_boundary)) CALLF(destination);
    if (bad) {
        check(fault_vector == 12 && fault_cpl == 3 && fault_ss == 35 &&
            fault_sp == 0xa5a58000 && fault_cs == 19 && fault_ip == 0x1234,
            "CALL fault-before-commit", operand, big);
        return;
    }
    check(!fault_vector && GET_CPL() == 0 && GET_SS_SELECTOR() == 24 &&
        GET_EIP() == 0x2345 && GET_ESP() ==
        (big ? new_sp - 6 * step : 0xa5a50000 | (new_sp - 6 * step)) &&
        read_item(operand, 0x40000 + new_sp - 6 * step) == 0x1234 &&
        c_sas_w_at(0x40000 + new_sp - 5 * step) == 19 &&
        read_item(operand, 0x40000 + new_sp - 4 * step) == 0x4567 &&
        read_item(operand, 0x40000 + new_sp - 3 * step) == 0x5678 &&
        read_item(operand, 0x40000 + new_sp - 2 * step) ==
            (operand ? 0xa5a58000 : 0x8000) &&
        c_sas_w_at(0x40000 + new_sp - step) == 35,
        "CALL frame", operand, big);
}

static void return_case(int operand, int big, int interrupt, int bad)
{
    IU32 new_sp = operand ? 0x12348000 : 0x8000;
    IU32 wanted = big ? new_sp : 0xa5a58000;
    unsigned stack_index;
    prepare(operand, big, 1);
    if (big) descriptor(24, 0x40000, 0x1ffff, 0xf3, 1);
    /* Use a byte-granular bounded stack in the ordinary positive cases. */
    if (big && operand) {
        new_sp = 0x18000; wanted = new_sp;
    }
    item(operand, 0, 0x2345); item(operand, 1, 19);
    if (interrupt) { item(operand, 2, 2); stack_index = 3; }
    else { item(operand, 2, 0x4567); item(operand, 3, 0x5678); stack_index = 4; }
    item(operand, stack_index, new_sp);
    item(operand, stack_index + 1, 27);
    if (bad == 1) descriptor(24, 0x40000, 0xffff, 0xf1, big); /* readonly SS */
    if (bad == 2) descriptor(16, 0x30000, 0x1000, 0xfb, operand);
    expect_fault = bad;
    if (!setjmp(fault_boundary)) {
        if (interrupt) IRET(); else RETF(2 * (operand ? 4 : 2));
    }
    if (bad) {
        check(fault_vector == 13 && fault_cpl == 0 && fault_ss == 32 &&
            fault_sp == 0xa5a58000 && fault_cs == 8 && fault_ip == 0x1234,
            interrupt ? "IRET fault" : "RETF fault",
            operand, big);
        return;
    }
    if (!interrupt) wanted += 2 * (operand ? 4 : 2);
    check(!fault_vector && GET_CPL() == 3 && GET_SS_SELECTOR() == 27 &&
        GET_EIP() == 0x2345 && GET_ESP() == wanted &&
        (!interrupt || (!GET_IF() && !GET_NT() && !GET_VM())),
        interrupt ? "IRET outer" : "RETF outer adjustment", operand, big);
}

int main(void)
{
    session owner;
    int operand, big, bad;
    session_initialize(&owner, 1);
    if (!session_activate(&owner) || !session_thread_bind(&owner)) return 2;
    sas_init(0x200000);
    for (operand = 0; operand < 2; ++operand)
        for (big = 0; big < 2; ++big)
            for (bad = 0; bad < 3; ++bad) {
                call_case(operand, big, bad);
                return_case(operand, big, 0, bad);
                return_case(operand, big, 1, bad);
            }
    sas_term();
    (void)session_thread_unbind(&owner);
    (void)session_dispose(&owner);
    fprintf(stderr, "CCPU_STACK_PROFILE cases=%d failures=%d\n", cases, failures);
    return failures ? 1 : 0;
}
