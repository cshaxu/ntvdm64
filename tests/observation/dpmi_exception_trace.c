/* Test-only original exception-boundary observation; never changes CPU state. */
#define host_exint_hook s38_original_host_exint_hook
#include "../../src/mvdm/softpc.new/host/src/nt_inthk.c"
#undef host_exint_hook
/* Keep the original TU's CRT binding, but write observations to the host file. */
#undef fprintf
#include <stdio.h>

extern IU32 CCPU_DR[8];
extern IU16 c_getCS(void);
extern IU16 c_getSS(void);
extern IU32 c_getEIP(void);
extern IU32 c_getESP(void);

BOOL host_exint_hook(IS32 number, IS32 error)
{
    static unsigned count;
    FILE *log = NULL;
    BOOL result;
    if (count++ < 32) log = fopen(DPMI_TRACE_LOG, "a");
    if (log) {
        fprintf(log, "before n=%u exception=%ld error=%ld cs:ip=%04X:%08lX "
            "ss:sp=%04X:%08lX dr0=%08lX dr6=%08lX dr7=%08lX\n",
            count, number, error, c_getCS(), c_getEIP(), c_getSS(), c_getESP(),
            CCPU_DR[0], CCPU_DR[6], CCPU_DR[7]);
        fflush(log);
    }
    result = s38_original_host_exint_hook(number, error);
    if (log) {
        fprintf(log, "after result=%d cs:ip=%04X:%08lX ss:sp=%04X:%08lX "
            "dr6=%08lX\n", result, c_getCS(), c_getEIP(), c_getSS(),
            c_getESP(), CCPU_DR[6]);
        fclose(log);
    }
    return result;
}
