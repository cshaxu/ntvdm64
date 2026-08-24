#include "opennt_dem_dispatch_composition.h"

static __declspec(thread) bx_ntvdm_demhndl_call *g_demdisp_call;

static void bx_ntvdm_demdisp_invoke_original(void)
{
    if (g_demdisp_call != 0)
        (void)DemDispatch((ULONG)g_demdisp_call->service);
}

int bx_ntvdm_demdisp_invoke(bx_ntvdm_demhndl_call *call)
{
    int result;
    if (!bx_ntvdm_demhndl_call_valid(call) || call->service >= SVC_DEMLASTSVC ||
        g_demdisp_call != 0) return 0;
    g_demdisp_call = call;
    result = bx_ntvdm_demhndl_invoke_body(call, bx_ntvdm_demdisp_invoke_original);
    g_demdisp_call = 0;
    return result;
}

size_t bx_ntvdm_demdisp_service_count(void)
{
    return SVC_DEMLASTSVC;
}

PFNSVC bx_ntvdm_demdisp_handler_at(uint8_t service)
{
    return service < SVC_DEMLASTSVC ? apfnSVC[service] : 0;
}
