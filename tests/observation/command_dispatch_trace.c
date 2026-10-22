/* Diagnostic-only wrapper around the unchanged COMMAND dispatcher. */
#define CmdDispatch observed_CmdDispatch
#include "../../src/mvdm/dos/command/cmddisp.c"
#undef CmdDispatch

BOOL CmdDispatch(ULONG service)
{
    FILE *log = fopen(S35_ENVIRONMENT_TRACE_LOG, "a");
    BOOL result;
    if (log) {
        fprintf(log, "before svc=%lu cs=%04x ip=%04x ss=%04x sp=%04x\n",
            service, (unsigned)getCS(), (unsigned)getIP(),
            (unsigned)getSS(), (unsigned)getSP());
        fflush(log);
    }
    result = observed_CmdDispatch(service);
    if (log) {
        fprintf(log, "after svc=%lu cs=%04x ip=%04x ss=%04x sp=%04x cf=%u ax=%04x\n",
            service, (unsigned)getCS(), (unsigned)getIP(),
            (unsigned)getSS(), (unsigned)getSP(), (unsigned)getCF(), (unsigned)getAX());
        fclose(log);
    }
    return result;
}
