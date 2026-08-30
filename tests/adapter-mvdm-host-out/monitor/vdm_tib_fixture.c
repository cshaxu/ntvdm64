#include <stdio.h>

#include "nt.h"
#include "monitor_context.h"

int main(void)
{
    POPENNT_SUPPORT_TEB teb = opennt_support_current_teb();

    if (teb == NULL || teb->Vdm != NULL) return 1;
    if (!mvdm_monitor_bind_current_thread()) return 2;
    if (teb->Vdm != &VdmTib) return 3;
    VdmTib.DpmiInfo.LockCount = 7u;
    *pNtVDMState = VDM_32BIT_APP;
    if (VdmTib.DpmiInfo.LockCount != 7u ||
        *pNtVDMState != VDM_32BIT_APP) return 4;
    puts("PASS: monitor VDM TIB thread carrier");
    return 0;
}
