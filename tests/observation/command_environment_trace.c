/* Diagnostic-only wrapper: include the unchanged original translation unit.
 * The formal product graph never selects this source. No guest writes here.
 */
#define cmdGetInitEnvironment s35_original_GetInitEnvironment
#include "../../src/mvdm/dos/command/cmdenv.c"
#undef cmdGetInitEnvironment

#ifndef S35_ENVIRONMENT_TRACE_LOG
#define S35_ENVIRONMENT_TRACE_LOG "O:\\winnt\\logs\\s35-envtrace-r1.events.txt"
#endif

static void s35_environment_snapshot(FILE *log, const char *phase)
{
    mvdm_guest_location location;
    mvdm_guest_location_lease lease;
    WORD value = 0, owner = 0, paragraphs = 0, stack[4] = {0};
    DWORD ds = (WORD)getDS(), es = (WORD)getES();
    int valid = 0;
    if (mvdm_guest_location_set_real_mode(&location, (WORD)ds, 0x203c) &&
        mvdm_guest_location_acquire(&location, 2, GUEST_MEMORY_ACCESS_READ, &lease)) {
        memcpy(&value, lease.bytes, 2);
        valid = mvdm_guest_location_release(&lease, 0);
    }
    if (es && mvdm_guest_location_set_real_mode(&location, (WORD)(es - 1), 0) &&
        mvdm_guest_location_acquire(&location, 16, GUEST_MEMORY_ACCESS_READ, &lease)) {
        memcpy(&owner, (const char *)lease.bytes + 1, 2);
        memcpy(&paragraphs, (const char *)lease.bytes + 3, 2);
        mvdm_guest_location_release(&lease, 0);
    }
    if (mvdm_guest_location_set_real_mode(&location, (WORD)getSS(), (WORD)getSP()) &&
        mvdm_guest_location_acquire(&location, sizeof(stack), GUEST_MEMORY_ACCESS_READ, &lease)) {
        memcpy(stack, lease.bytes, sizeof(stack));
        mvdm_guest_location_release(&lease, 0);
    }
    fprintf(log, "%s CS=%04X IP=%04X SS=%04X SP=%04X DS=%04lX ES=%04lX BX=%04X EnvSiz=%04X valid=%d "
        "MCBowner=%04X MCBparagraphs=%04X containsEnvSiz=%d stack=%04X,%04X,%04X,%04X\n",
        phase, (WORD)getCS(), (WORD)getIP(), (WORD)getSS(), (WORD)getSP(),
        ds, es, (WORD)getBX(), value, valid, owner, paragraphs,
        ds * 16 + 0x203c >= es * 16 &&
        ds * 16 + 0x203e <= (es + paragraphs) * 16,
        stack[0], stack[1], stack[2], stack[3]);
    fflush(log);
}

VOID cmdGetInitEnvironment(VOID)
{
    FILE *log = fopen(S35_ENVIRONMENT_TRACE_LOG, "a");
    if (log) s35_environment_snapshot(log, "before");
    s35_original_GetInitEnvironment();
    if (log) { s35_environment_snapshot(log, "after"); fclose(log); }
}
