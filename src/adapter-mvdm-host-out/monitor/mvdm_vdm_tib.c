#include "vdm.h"

/* Same-shaped storage for source bodies that historically accessed the
 * user-mode VDM TIB.  It deliberately does not expose an NT kernel VDM
 * object, a CCPU executor or a cross-session broker. */
__declspec(thread) VDM_TIB VdmTib;
__declspec(thread) ULONG mvdm_monitor_ntvdm_state;
