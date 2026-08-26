/* DIVERGENCE(BOP-DIV-097): this translation unit retains the reached
 * `MS_bop_0` subset in its original OpenNT owner file. The unavailable
 * CCPU/SAS backing is supplied by the same-shaped adapter-softpc facade. */
#include "adapter-softpc/dem/ccpu_sas_facade.h"
#include "adapter-softpc/idle_shim.h"

BOOL DemDispatch(ULONG iSvc);

#define IDLE_disk() HostIdleNoActivity()
#define SEGOFF(seg,off) (((ULONG)(seg) << 16) + ((off)))
#define Sim32GetVDMPointer(address, bytes, protect) \
    runtime_demhndl_sim32_get_vdm_pointer((address), (bytes), (protect))

void MS_bop_0(void)
{
    ULONG DemCmd;

    DemCmd = (ULONG)(*Sim32GetVDMPointer(SEGOFF(getCS(),getIP()), 1, FALSE));
    DemDispatch(DemCmd);
    setIP((USHORT)(getIP() + 1));
    if (DemCmd != 0x15 && DemCmd != 0x14) IDLE_disk();
}

/* COMMAND's same-shaped frame facade intentionally has its own source
 * spelling. Do not let the DEM compatibility macro scope leak into it. */
#undef IDLE_disk
#undef SEGOFF
#undef Sim32GetVDMPointer
#undef getAX
#undef getBX
#undef getCX
#undef getDX
#undef getSI
#undef getDI
#undef getBP
#undef getDS
#undef getES
#undef getCS
#undef getIP
#undef getAL
#undef getCL
#undef getCH
#undef getBL
#undef getDH
#undef getDL
#undef getZF
#undef getCF
#undef setAX
#undef setAL
#undef setBX
#undef setBL
#undef setBP
#undef setSI
#undef setDI
#undef setCX
#undef setCL
#undef setCH
#undef setDX
#undef setDL
#undef setDH
#undef setCF
#undef setZF
#undef setCS
#undef setIP
#undef GETHANDLE
#undef GetVDMAddr
#undef STOREWORD

/* DIVERGENCE(BOP-DIV-099): this is the reached source-order `MS_bop_4`
 * subset; its CCPU/SAS backing remains the declared bounded facade. */
#include "opennt-bop/command/opennt_command_composition.h"

typedef UCHAR half_word;
#define getCS() runtime_ccpu_get_cs()
#define getIP() runtime_ccpu_get_ip()
#define setIP(value) runtime_ccpu_set_ip((USHORT)(value))
#define sas_load(address, target) runtime_ccpu_sas_load((address), (target))
#define CmdDispatch(service) runtime_command_misc_dispatch_source_command((service))

void MS_bop_4(void)
{
    half_word Command;

    sas_load(((ULONG)getCS() << 4) + getIP(), &Command);
    CmdDispatch((ULONG)Command);
    setIP((USHORT)(getIP() + 1));
}

#undef getCS
#undef getIP
#undef setIP
#undef sas_load
#undef CmdDispatch
#undef getDX
#undef getBX
#undef getCX
#undef getSI
#undef getDS
#undef getES
#undef getSS
#undef getBP
#undef getAL
#undef getAH
#undef getAX
#undef setAX
#undef setAL
#undef setCF
#undef setDX
#undef setBX
#undef setCX
#undef setDS
#undef setES
#undef GetVDMAddr

/* DIVERGENCE(BOP-DIV-081): `nt_bop.c` is the original-name linkage boundary
 * for the reached MS_bop_E/UMBNotify fragment.
 * DIVERGENCE(BOP-DIV-110): it also retains the separately reached MS_bop_F
 * keyboard-vector fragment.  Both bodies are private because the full NT4
 * product dispatcher exceeds the mirror threshold. */
#include "opennt-bop-overlay/softpc/opennt_config_complete_composition.c"
/* The previous COMMAND scope deliberately undefines this facade spelling.
 * Restore only the exact AL getter needed by the independent MS_bop_E source
 * fragment before its private overlay is included. */
#define getAL() runtime_ccpu_get_al()
#include "opennt-bop-overlay/softpc/nt_bop_config_done.c"
/* DIVERGENCE(BOP-DIV-110): `MS_bop_E` uses the DEM source facade's macro spellings.  MS_bop_F is a
 * separate original fragment with its own same-named SoftPC facade, so end
 * the first private include's macro scope at this mirror boundary. */
#undef getAX
#undef getSI
#undef getDS
#undef getCS
#undef setCF
#include "opennt-bop-overlay/softpc/nt_bop_keyboard.c"
