/*
 * DIVERGENCE(MVDM-HOST-DIV-017): This is the private, source-shaped subset of
 * original softpc.new/host/src/nt_bop.c for MS_bop_0 and MS_bop_4. The full
 * historical translation unit also carries unrelated PIC/CCPU/XMS/DPMI/WOW
 * selector bodies and cannot enter the sole-Bochs-executor product closure.
 *
 * Both functions retain the original name, dispatcher call and IP sequencing.
 * MS_bop_0 changes only its raw Sim32GetVDMPointer dereference to the already
 * admitted checked real-mode SAS byte read: an NT4 process pointer cannot be
 * made safe on x86/x64 and would bypass Bochs guest-RAM bounds.
 */
#include <nt.h>

#include "demexp.h"
#include "cmdsvc.h"
#include "host_idle.h"
#include "mvdm_command_registers.h"
#include "mvdm_sas.h"

void MS_bop_0(void)
{
    ULONG DemCmd;
    uint8_t service;

    sas_load(((ULONG)getCS() << 4) + getIP(), &service);
    DemCmd = (ULONG)service;
    DemDispatch(DemCmd);
    setIP((USHORT)(getIP() + 1));

    if (DemCmd != 0x15 && DemCmd != 0x14)
        HostIdleNoActivity();
}

void MS_bop_4(void)
{
    uint8_t Command;

    sas_load(((ULONG)getCS() << 4) + getIP(), &Command);
    CmdDispatch((ULONG)Command);
    setIP((USHORT)(getIP() + 1));
}
