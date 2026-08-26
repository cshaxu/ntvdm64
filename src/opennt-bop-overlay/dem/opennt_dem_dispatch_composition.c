#include "opennt-bop-overlay/dem/opennt_dem_dispatch_composition.h"

static __declspec(thread) runtime_demhndl_call *g_demdisp_call;

static void runtime_demdisp_invoke_original(void)
{
    if (g_demdisp_call != 0)
        (void)DemDispatch((ULONG)g_demdisp_call->service);
}

int runtime_demdisp_invoke(runtime_demhndl_call *call)
{
    int result;
    if (!runtime_demhndl_call_valid(call) || call->service >= SVC_DEMLASTSVC ||
        g_demdisp_call != 0) return 0;
    g_demdisp_call = call;
    /* The source-shaped mechanical facade remains selector blind.  This
     * composition installs the only retained cross-call guest-layout flush. */
    runtime_demhndl_set_post_body_hook(runtime_demerror_flush_hard_error);
    result = runtime_demhndl_invoke_body(call, runtime_demdisp_invoke_original);
    runtime_demhndl_set_post_body_hook(0);
    g_demdisp_call = 0;
    return result;
}

size_t runtime_demdisp_service_count(void)
{
    return SVC_DEMLASTSVC;
}

PFNSVC runtime_demdisp_handler_at(uint8_t service)
{
    return service < SVC_DEMLASTSVC ? apfnSVC[service] : 0;
}
