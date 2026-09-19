/* Test-only observation wrapper; no guest writes or production graph change. */
#define DpmiDispatch s36_original_DpmiDispatch
#include "../../src/mvdm/dpmi32/dpmi32.c"
#undef DpmiDispatch
#include <stdio.h>

static LONG native_fault(EXCEPTION_POINTERS *fault)
{
    FILE *log = fopen("O:\\winnt\\logs\\s36-dpmi-dispatch-r1.events.txt", "a");
    if (log) {
        fprintf(log, "native-exception=%08lX address=%p info=%08lX,%08lX\n",
            fault->ExceptionRecord->ExceptionCode,
            fault->ExceptionRecord->ExceptionAddress,
            fault->ExceptionRecord->ExceptionInformation[0],
            fault->ExceptionRecord->ExceptionInformation[1]);
        {
            ULONG *stack = (ULONG *)fault->ContextRecord->Esp;
            fprintf(log, "native-stack=%08lX %08lX %08lX %08lX %08lX %08lX\n",
                stack[0], stack[1], stack[2], stack[3], stack[4], stack[5]);
        }
        fclose(log);
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

static void snapshot(FILE *log, const char *phase)
{
    fprintf(log, "%s index=%02lX PE=%u CS:IP=%04X:%04X SS:SP=%04X:%04X "
        "DS=%04X ES=%04X AX=%04X BX=%04X CX=%04X DX=%04X SI=%04X DI=%04X\n",
        phase, Index, (unsigned)(getMSW() & 1), (WORD)getCS(), (WORD)getIP(),
        (WORD)getSS(), (WORD)getSP(), (WORD)getDS(), (WORD)getES(),
        (WORD)getAX(), (WORD)getBX(), (WORD)getCX(), (WORD)getDX(),
        (WORD)getSI(), (WORD)getDI());
    fflush(log);
}

VOID DpmiDispatch(VOID)
{
    FILE *log = fopen("O:\\winnt\\logs\\s36-dpmi-dispatch-r1.events.txt", "a");
    /* Before.index is the previous dispatch; after.index is this dispatch. */
    if (log) snapshot(log, "before");
    __try { s36_original_DpmiDispatch(); }
    __except (native_fault(GetExceptionInformation())) { }
    if (log) { snapshot(log, "after"); fclose(log); }
}
