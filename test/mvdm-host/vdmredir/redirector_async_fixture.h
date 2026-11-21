#ifndef TEST_REDIRECTOR_ASYNC_FIXTURE_H
#define TEST_REDIRECTOR_ASYNC_FIXTURE_H
#include "mvdm-host/inc/vrnmpipe.h"
#include "compat/include/mvdm_cpu_access.h"

/* The caller fixture uses real extracted C location operations with controlled
 * address-space I/O. CPU binding installation is covered by the CCPU fixtures. */
const mvdm_cpu_access *mvdm_cpu_access_current(void)
{
    static const mvdm_cpu_access provider = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        mvdm_guest_location_acquire, mvdm_guest_location_release,
        mvdm_guest_location_copy_to_guest
    };
    return &provider;
}

static int fixture_redirector_prepare(PDOS_ASYNC_NAMED_PIPE_INFO request,
    WORD segment, WORD offset, DWORD type, LPBYTE *buffer, WORD *length)
{
    return mvdm_redirector_async_prepare(&request->PrivateAsyncState,
        &request->Buffer, &request->ANR, &request->Type2, &request->Semaphore,
        segment, offset, type == ANP_READ || type == ANP_READ2,
        type == ANP_READ2 || type == ANP_WRITE2, buffer, length);
}
#endif
