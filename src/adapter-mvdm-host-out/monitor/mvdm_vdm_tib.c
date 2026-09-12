#include "monitor_context.h"

/* Source-shaped storage from the original user-mode monitor: one NTVDM
 * worker has one process-global VDM TIB.  It deliberately does not expose
 * an NT kernel VDM object, a CCPU executor or a cross-session broker. */
VDM_TIB VdmTib;
ULONG mvdm_monitor_ntvdm_state;

int mvdm_monitor_bind_current_thread(void)
{
    POPENNT_SUPPORT_TEB teb = opennt_support_current_teb();
    if (teb == NULL) return 0;
    /* The original monitor initialized the three printer modes before the
     * source-derived registration carrier first observes this worker TIB.
     * This preserves only that scalar initial state; no kernel monitor state
     * or V86 execution context is introduced. */
    if (VdmTib.PrinterInfo.prt_Status == NULL &&
        VdmTib.PrinterInfo.prt_Mode[0] == 0 &&
        VdmTib.PrinterInfo.prt_Mode[1] == 0 &&
        VdmTib.PrinterInfo.prt_Mode[2] == 0) {
        VdmTib.PrinterInfo.prt_Mode[0] = PRT_MODE_NO_SIMULATION;
        VdmTib.PrinterInfo.prt_Mode[1] = PRT_MODE_NO_SIMULATION;
        VdmTib.PrinterInfo.prt_Mode[2] = PRT_MODE_NO_SIMULATION;
    }
    teb->Vdm = &VdmTib;
    return 1;
}
