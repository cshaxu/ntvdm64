/*
 * Source-shaped include boundary for the historical private USER header.
 *
 * DIVERGENCE(ADAPTER-WIN32-022): the complete NT4 winuserp.h belongs to the
 * USER product shell and redeclares private, non-public UI contracts.  The
 * selected original SoftPC nt_event.c reaches no declaration from that file:
 * its concrete input, console and window types come from its other original
 * headers and the modern public SDK.  Keep the include spelling buildable
 * without importing the private USER product shell or inventing a provider.
 */
#ifndef ADAPTER_MVDM_HOST_OUT_WIN32_WINUSERP_H
#define ADAPTER_MVDM_HOST_OUT_WIN32_WINUSERP_H

#include <windows.h>

#endif /* ADAPTER_MVDM_HOST_OUT_WIN32_WINUSERP_H */
