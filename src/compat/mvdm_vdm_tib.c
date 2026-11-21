#include "include/monitor_context.h"
#include "include/opennt_support_thread.h"

ULONG mvdm_monitor_ntvdm_state;
/* Installed before worker/NTIO entry and immutable for the process lifetime. */
static const mvdm_monitor_provider *monitor;

BOOL mvdm_monitor_bind_provider(const mvdm_monitor_provider *provider)
{
    if (!provider || !provider->tib || !provider->printer_view ||
            !provider->printer_close || (monitor && monitor != provider)) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    monitor = provider;
    return TRUE;
}

BOOL mvdm_monitor_get_printer(mvdm_monitor_printer_view *view)
{
    if (!monitor) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    monitor->printer_view(view);
    return TRUE;
}

void mvdm_monitor_close_printer(WORD adapter)
{
    monitor->printer_close(adapter);
}

int mvdm_monitor_bind_current_thread(void)
{
    POPENNT_SUPPORT_TEB teb = opennt_support_current_teb();
    mvdm_monitor_printer_view view;
    if (!teb || !mvdm_monitor_get_printer(&view)) return 0;
    if (*view.status == NULL && view.modes[0] == 0 &&
            view.modes[1] == 0 && view.modes[2] == 0) {
        view.modes[0] = view.no_simulation;
        view.modes[1] = view.no_simulation;
        view.modes[2] = view.no_simulation;
    }
    teb->Vdm = monitor->tib;
    return 1;
}
