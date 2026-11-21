#ifndef ADAPTER_MVDM_MONITOR_CONTEXT_H
#define ADAPTER_MVDM_MONITOR_CONTEXT_H
#include <windows.h>

/* Synchronous view of the one worker-owned record. No layout or offsets
 * of the original TIB cross this boundary; storage outlives every call. */
typedef struct mvdm_monitor_printer_view {
    PUCHAR *state, *control, *status, *host_state;
    PWORD ports, counts;
    PHANDLE handles;
    PUCHAR modes, buffers;
    WORD count, stride;
    BYTE no_simulation, simulate, direct, vdd;
} mvdm_monitor_printer_view;

typedef struct mvdm_monitor_provider {
    void *tib;
    void (*printer_view)(mvdm_monitor_printer_view *view);
    void (*printer_close)(WORD adapter);
} mvdm_monitor_provider;

BOOL mvdm_monitor_bind_provider(const mvdm_monitor_provider *provider);
BOOL mvdm_monitor_get_printer(mvdm_monitor_printer_view *view);
void mvdm_monitor_close_printer(WORD adapter);
int mvdm_monitor_bind_current_thread(void);
extern ULONG mvdm_monitor_ntvdm_state;
#undef pNtVDMState
#define pNtVDMState (&mvdm_monitor_ntvdm_state)
#endif
