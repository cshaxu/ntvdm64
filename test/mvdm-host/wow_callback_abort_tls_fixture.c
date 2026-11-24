/* Inspect actual original TLS state without exporting a production getter.
 * Keep this owner in its own TU: original host_def.h has an unguarded
 * anonymous extern structure and cannot be included twice in one C TU. */
#include "../../src/mvdm-host/softpc.new/base/ccpu386/ntthread.c"
int fixture_simulation_level(void)
{
    DWORD error=GetLastError();
    ThreadSimBufPtr state=(ThreadSimBufPtr)TlsGetValue(ccpuSimId);
    int result=state?state->level:0;
    SetLastError(error);return result;
}
void fixture_thread_exit(void)
{
    ccpu386exitthread();TlsSetValue(ccpuSimId,NULL);
}
