/*
 * A bounded, source-shaped CallBack16 transaction proof.
 *
 * The test deliberately enters the selected original wcall16.c body.  The
 * temporary VDMFRAME/CBVDMFRAME aliases are the production bounded leases,
 * and the recursive execution spelling is the original nt_cpu.c
 * host_simulate() wrapper selected by CCPU40.  The guest instruction stream
 * is only the original CCPU unsimulate return opcode (D6 FE); it proves the
 * host-side callback order without claiming a completed WOW16 trampoline.
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
#include "session/session.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_guest_memory.h"

extern void sas_init(PHY_ADDR size);
extern void sas_term(void);
extern void c_cpu_init(void);
extern void load_sw_cpu_access_functions(void);
extern void host_simulate(void);
extern void c_sas_store(IU32 address, IU8 value);
extern void c_sas_stores(IU32 address, IU8 *bytes, IU32 length);
extern void c_sas_loads(IU32 address, IU8 *bytes, IU32 length);
extern void c_setIP(IU16 value);
extern void c_setSP(IU16 value);
extern void c_setDX(IU16 value);
extern IU16 c_getIP(void);
extern IUH c_setSS(IU16 value);

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

    /* The original CCPU D6 FE direct-return instruction is sufficient for
     * this narrow recursive host_simulate() proof.  It intentionally does
     * not stand in for WOW16_From_CallBack16. */
    c_sas_store(UINT32_C(0x0000fff0), UINT8_C(0xd6));
    c_sas_store(UINT32_C(0x0000fff1), UINT8_C(0xfe));
    c_setIP(UINT16_C(0xfff0));
    c_setSS(0u);
    c_setSP((IU16)caller_address);
    c_setDX(UINT16_C(0xbeef));

    if (!CallBack16(RET_TASKSTARTED, NULL, 0u, &returned) ||
        returned != UINT32_C(0xbeef0000) ||
        c_getIP() != UINT16_C(0xfff2) ||
        task.vpStack != (VPVOID)caller_address ||
        task.vpCBStack != (VPVOID)caller_address) {
        fputs("original CallBack16/CCPU40 return ordering failed\n", stderr);
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
        fputs("original CallBack16 frame contents failed\n", stderr);
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
    return 0;
}
