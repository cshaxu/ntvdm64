#include "vdm.h"

/* Same-shaped storage for source bodies that historically accessed the
 * user-mode VDM TIB.  It deliberately does not expose an NT kernel VDM
 * object, a CCPU executor or a cross-session broker. */
__declspec(thread) VDM_TIB VdmTib;
__declspec(thread) ULONG mvdm_monitor_ntvdm_state;

int mvdm_monitor_bind_current_thread(void)
{
    POPENNT_SUPPORT_TEB teb = opennt_support_current_teb();
    if (teb == NULL) return 0;
    teb->Vdm = &VdmTib;
    return 1;
}
