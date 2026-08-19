#include <windows.h>

#include "bop/shim/dem_direct_host_session.h"

/* This token-lifecycle fixture has no #UD consumer.  The formal CPU core
 * expects one external generic bridge, so supply a decline-only test stub. */
int bx_ntvdm_mantle_generic_ud_bridge_v1(const void *event, void *outcome)
{ (void)event; (void)outcome; return 0; }

int main(void)
{
    bx_ntvdm_dem_direct_host_session session;
    bx_ntvdm_dem_direct_context *context;
    HANDLE event, returned;
    uint32_t token;
    DWORD error;
    if (!bx_ntvdm_dem_direct_host_session_initialize(&session) ||
        (context = bx_ntvdm_dem_direct_host_session_context(&session)) == NULL)
        return 1;
    event = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (event == NULL) return 2;
    if (!context->publish_handle(context->state, event, &token, &error) ||
        token == 0u || error != ERROR_SUCCESS) return 3;
    if (!context->lookup_handle(context->state, token, &returned) || returned != event)
        return 4;
    if (!context->release_handle(context->state, token, &error) ||
        error != ERROR_SUCCESS) return 5;
    if (context->lookup_handle(context->state, token, &returned)) return 6;
    bx_ntvdm_dem_direct_host_session_reset(&session);
    return 0;
}
