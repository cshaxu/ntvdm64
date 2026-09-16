#include "mvdm_ica_eoi_bridge.h"

#include <windows.h>

/* Same selected declaration as the original nt_eoi.h carrier.  The bridge
 * deliberately imports only the typed PIC leaf, not the CCPU macro surface. */
void ica_eoi(ULONG adapter, LONG *line, int rotate);

void mvdm_ica_eoi_bridge(int adapter, int *line)
{
    LONG pic_line;

    if (line == 0) {
        return;
    }

    pic_line = (LONG)*line;
    ica_eoi((ULONG)adapter, &pic_line, 0);
    *line = (int)pic_line;
}
