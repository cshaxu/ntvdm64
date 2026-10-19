/* Inspect actual original TLS state without exporting a production getter.
 * Keep this owner in its own TU: original host_def.h has an unguarded
 * anonymous extern structure and cannot be included twice in one C TU. */
#ifndef WOW_TEST_NTTHREAD_SOURCE
#define WOW_TEST_NTTHREAD_SOURCE "../../src/mvdm/softpc.new/base/ccpu386/ntthread.c"
#endif
#include WOW_TEST_NTTHREAD_SOURCE
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
