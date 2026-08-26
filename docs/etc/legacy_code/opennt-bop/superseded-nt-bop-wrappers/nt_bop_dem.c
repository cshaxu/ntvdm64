/*
 * True source subset: OpenNT base/mvdm/softpc.new/host/src/nt_bop.c,
 * MS_bop_0.  The original unit's WOW, XMS, DPMI and product-shell BOP
 * families are not admitted by this DEM package.
 */

#include "adapter-softpc/dem/ccpu_sas_facade.h"
#include "nt_bop_dem.h"
#include "adapter-softpc/idle_shim.h"

/* DIVERGENCE(BOP-DIV-097): the original product include closure supplied
 * sim32.h, idetect.h and all CCPU/SAS declarations.  This recovered subset
 * receives same-shaped bounded facades; HostIdleNoActivity is an
 * adapter-softpc entry rather than an NT4 product-global worker. */
#define IDLE_disk() HostIdleNoActivity()
#define SEGOFF(seg,off) (((ULONG)(seg) << 16) + ((off)))
#define Sim32GetVDMPointer(address, bytes, protect) \
    runtime_demhndl_sim32_get_vdm_pointer((address), (bytes), (protect))

// DOS EMULATION BOP
void MS_bop_0(void) {
    ULONG DemCmd;

    DemCmd = (ULONG)(*Sim32GetVDMPointer(SEGOFF(getCS(),getIP()),
                                         1,
                                         FALSE
                                         ));
    DemDispatch( DemCmd );
    setIP((USHORT)(getIP() + 1));

    // we need to prevent the idle system from going off on intensive file
    // reads. However, we don't want to disable it for continuous 'Get Time'
    // calls (command 0x15). Nor for Get Date (0x15).
    if (DemCmd != 0x15 && DemCmd != 0x14)
        IDLE_disk();
}
