/* Test-only call-boundary observation of the unchanged original INT21 owner. */
#define DpmiSwitchToProtectedMode trace_switch_protected
#define DpmiSegmentToSelector trace_segment_selector
#define DpmiSimulateIretCF trace_iret_cf
#include "../../src/mvdm/dpmi32/int21map.c"
#undef DpmiSwitchToProtectedMode
#undef DpmiSegmentToSelector
#undef DpmiSimulateIretCF
#include <stdio.h>

VOID DpmiSwitchToProtectedMode(VOID);
USHORT DpmiSegmentToSelector(USHORT Segment);
VOID DpmiSimulateIretCF(VOID);

static void pointer_snapshot(const char *phase, USHORT segment, USHORT selector)
{
    DWORD error = GetLastError();
    FILE *log = fopen(DPMI_TRACE_LOG, "a");
    if (log) {
        fprintf(log, "%s AX=%04X DS=%04X SI=%04X segment=%04X selector=%04X flags=%04X ES=%04X DI=%04X\n",
            phase, (USHORT)getAX(), (USHORT)getDS(), (USHORT)getSI(),
            segment, selector, (USHORT)getSTATUS(), (USHORT)getES(), (USHORT)getDI());
        fclose(log);
    }
    SetLastError(error);
}

VOID trace_switch_protected(VOID)
{
    if (getAH() == 0x63 || getAX() == 5) pointer_snapshot("real-return", 0, 0);
    DpmiSwitchToProtectedMode();
    if (getAH() == 0x63 || getAX() == 5) pointer_snapshot("protected-entry", 0, 0);
}

USHORT trace_segment_selector(USHORT segment)
{
    USHORT selector;
    pointer_snapshot("convert-before", segment, 0);
    selector = DpmiSegmentToSelector(segment);
    pointer_snapshot("convert-after", segment, selector);
    return selector;
}

VOID trace_iret_cf(VOID)
{
    BOOL observe = getAH() == 6 || getAH() == 0x0c || getAX() == 5;
    if (observe) pointer_snapshot("iret-before", 0, 0);
    DpmiSimulateIretCF();
    if (observe) pointer_snapshot("iret-after", 0, 0);
}
