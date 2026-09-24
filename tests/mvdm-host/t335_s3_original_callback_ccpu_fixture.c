/*
 * A bounded, source-shaped CallBack16 transaction proof.
 *
 * The test deliberately enters the selected original wcall16.c body.  The
 * temporary VDMFRAME/CBVDMFRAME aliases are the production bounded leases,
 * and the recursive execution spelling is the original nt_cpu.c
 * host_simulate() wrapper selected by CCPU40.  The outer guest stream enters
 * the existing BIOS BOP table and its inner stream uses CCPU's original
 * unsimulate return opcode (D6 FE); it proves the host-side callback order
 * without claiming a completed WOW16 trampoline.
 */
#include <stdio.h>
#include <string.h>

#include "precomp.h"
#include "insignia.h"
#include "host_def.h"
#include "xt.h"
#include "sas.h"
#include "cpu.h"
#include "c_main.h"
#include "ntvdm-exe/session/session.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h"

extern void sas_init(PHY_ADDR size);
extern void sas_term(void);
extern void c_cpu_init(void);
extern void c_cpu_simulate(void);
extern void c_cpu_unsimulate(void);
extern void load_sw_cpu_access_functions(void);
extern void c_sas_store(IU32 address, IU8 value);
extern void c_sas_stores(IU32 address, IU8 *bytes, IU32 length);
extern void c_sas_loads(IU32 address, IU8 *bytes, IU32 length);
extern void c_setIP(IU16 value);
extern void c_setSP(IU16 value);
extern void c_setDX(IU16 value);
extern IU16 c_getIP(void);
extern IUH c_setSS(IU16 value);
extern ISM32 c_setCS(IU16 value);

typedef void (*fixture_bop)(void);
extern fixture_bop BIOS[];

static VPVOID callback_return;
static int callback_result;
static int callback_bop_entered;
static const IU16 callback_program_ip = UINT16_C(0x8000);

/*
 * This is a fixture-only occupant of CCPU's existing BIOS BOP table.  The
 * guest executes D6 05, so CallBack16 is reached while the outer simulator
 * owns a genuine CCPU setjmp frame.  The callback itself enters the nested
 * original host_simulate() body and returns through D6 FE.  Nothing here is
 * linked into the product BOP table or defines a new guest ABI.
 */
static void callback_bop(void)
{
    callback_bop_entered = 1;
    callback_result = CallBack16(RET_TASKSTARTED, NULL, 0u,
        &callback_return);

    /* The controlled outer guest program has no next instruction.  Use the
     * original direct-return BOP mechanism to leave its CCPU invocation. */
    c_sas_store(UINT32_C(0x00008004), UINT8_C(0xd6));
    c_sas_store(UINT32_C(0x00008005), UINT8_C(0xfe));
    c_setIP((IU16)(callback_program_ip + 4u));
    c_cpu_unsimulate();
}

static void write_guest(IU32 address, const void *value, size_t count)
{
    c_sas_stores(address, (IU8 *)value, (IU32)count);
}

static int verify_guest(IU32 address, const void *value, size_t count)
{
    unsigned char actual[sizeof(CBVDMFRAME)];

    if (count > sizeof(actual)) return 0;
    c_sas_loads(address, actual, (IU32)count);
    return memcmp(actual, value, count) == 0;
}

int main(void)
{
    session owner;
    TD task;
    VDMFRAME caller;
    CBVDMFRAME callback;
    VPVOID returned = 0u;
    const IU32 caller_address = UINT32_C(0x0000f000);
    const IU32 callback_address = caller_address - (IU32)sizeof(CBVDMFRAME);

    memset(&task, 0, sizeof(task));
    memset(&caller, 0, sizeof(caller));
    memset(&callback, 0, sizeof(callback));
    caller.wTDB = UINT16_C(0x0042);
    caller.wLocalBP = UINT16_C(0x519a);

    sas_init(UINT32_C(0x00200000));
    c_cpu_init();
    load_sw_cpu_access_functions();

    session_initialize(&owner, 335u);
    if (!session_select_machine_backend(&owner, SESSION_MACHINE_BACKEND_SOFTPC) ||
        !session_activate(&owner) || !session_thread_bind(&owner) ||
        !mvdm_softpc_guest_memory_begin(&owner)) {
        fputs("unable to bind source-shaped callback session\n", stderr);
        sas_term();
        return 1;
    }

    /* This is the exact storage site used by original wkman.c before it
     * dispatches WOW work.  The fixture creates no second task model. */
    task.vpStack = (VPVOID)caller_address;
    task.vpCBStack = (VPVOID)caller_address;
    task.dwFlags = TDF_INITCALLBACKSTACK;
    task.htask16 = caller.wTDB;
    NtCurrentTeb()->WOW32Reserved = &task;
    write_guest(caller_address, &caller, sizeof(caller));

    /* The outer guest program reaches the fixture BOP; after CCPU advances
     * EIP past D6 05, the nested CallBack16 simulation begins at 8002 and
     * takes the original D6 FE direct-return branch. */
    c_sas_store(UINT32_C(0x00008000), UINT8_C(0xd6));
    c_sas_store(UINT32_C(0x00008001), UINT8_C(0x05));
    c_sas_store(UINT32_C(0x00008002), UINT8_C(0xd6));
    c_sas_store(UINT32_C(0x00008003), UINT8_C(0xfe));
    /* The program is at a real-mode physical address.  Set the matching
     * original CS explicitly: CCPU initialization has no test contract for
     * an implicit code selector. */
    (void)c_setCS(0u);
    c_setIP(callback_program_ip);
    c_setSS(0u);
    c_setSP((IU16)caller_address);
    c_setDX(UINT16_C(0xbeef));

    BIOS[5] = callback_bop;
    c_cpu_simulate();
    BIOS[5] = NULL;
    returned = callback_return;

    /* D6 FE exits the nested CCPU level directly; unlike the immutable
     * WOW16_From_CallBack16 trampoline it deliberately does not write AX/DX
     * back into CBVDMFRAME.  A zero return is therefore the exact expected
     * result for this bounded ordering proof, not a fabricated callback
     * result. */
    if (!callback_bop_entered || !callback_result || returned != 0u ||
        c_getIP() != (IU16)(callback_program_ip + 4u) ||
        task.vpStack != (VPVOID)caller_address ||
        task.vpCBStack != (VPVOID)callback_address) {
        fprintf(stderr, "original CallBack16/CCPU40 ordering failed "
            "entered=%d result=%d return=%08lx ip=%04x stack=%08lx callback=%08lx\n",
            callback_bop_entered, callback_result,
            (unsigned long)returned, (unsigned)c_getIP(),
            (unsigned long)task.vpStack, (unsigned long)task.vpCBStack);
        NtCurrentTeb()->WOW32Reserved = NULL;
        mvdm_softpc_guest_memory_end(&owner);
        (void)session_thread_unbind(&owner);
        (void)session_dispose(&owner);
        sas_term();
        return 2;
    }

    /* CallBack16 overwrites the callback record.  Verify only the original
     * fields selected by this transaction, not a synthetic guest callback. */
    c_sas_loads(callback_address, (IU8 *)&callback, (IU32)sizeof(callback));
    if (callback.vpStack != (VPVOID)caller_address ||
        callback.wRetID != (WORD)RET_TASKSTARTED ||
        callback.wTDB != caller.wTDB ||
        callback.wLocalBP != caller.wLocalBP ||
        callback.wAX != 0u) {
        fprintf(stderr, "original CallBack16 frame contents failed "
            "stack=%08lx ret=%04x tdb=%04x bp=%04x ax=%04x\n",
            (unsigned long)callback.vpStack, callback.wRetID,
            callback.wTDB, callback.wLocalBP, callback.wAX);
        NtCurrentTeb()->WOW32Reserved = NULL;
        mvdm_softpc_guest_memory_end(&owner);
        (void)session_thread_unbind(&owner);
        (void)session_dispose(&owner);
        sas_term();
        return 3;
    }

    NtCurrentTeb()->WOW32Reserved = NULL;
    mvdm_softpc_guest_memory_end(&owner);
    if (!session_thread_unbind(&owner) || !session_dispose(&owner)) {
        sas_term();
        return 4;
    }
    sas_term();
    puts("ORIGINAL_CALLBACK16_ACTIVE_CCPU_BOP_ORDER_OK");
    return 0;
}
