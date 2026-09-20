/* Test-only observation wrapper; no guest writes or production graph change. */
#define DpmiDispatch s36_original_DpmiDispatch
#include "../../src/mvdm/dpmi32/dpmi32.c"
#undef DpmiDispatch
#include <stdio.h>
#ifdef DPMI_TRACE_XMEM_FAILURE
extern BOOL s38_grow_failed;
#endif
#ifndef DPMI_TRACE_LOG
#define DPMI_TRACE_LOG "O:\\winnt\\logs\\s36-dpmi-dispatch-r1.events.txt"
#endif

static LONG native_fault(EXCEPTION_POINTERS *fault)
{
    FILE *log = fopen(DPMI_TRACE_LOG, "a");
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

static void snapshot(FILE *log, const char *phase, ULONG dispatch_index)
{
    fprintf(log, "%s index=%02lX PE=%u CS:IP=%04X:%04X SS:SP=%04X:%04X "
        "DS=%04X ES=%04X AX=%04X BX=%04X CX=%04X DX=%04X SI=%04X DI=%04X\n",
        phase, dispatch_index, (unsigned)(getMSW() & 1), (WORD)getCS(), (WORD)getIP(),
        (WORD)getSS(), (WORD)getSP(), (WORD)getDS(), (WORD)getES(),
        (WORD)getAX(), (WORD)getBX(), (WORD)getCX(), (WORD)getDX(),
        (WORD)getSI(), (WORD)getDI());
    fflush(log);
}

VOID DpmiDispatch(VOID)
{
    DWORD saved_error = GetLastError();
    ULONG dispatch_index = *(Sim32GetVDMPointer(
        ((getCS() << 16) | getIP()), 1, (UCHAR)(getMSW() & MSW_PE)));
#ifdef DPMI_TRACE_XMEM_FAILURE
    if (!s38_grow_failed || dispatch_index != 0) {
        s36_original_DpmiDispatch();
        return;
    }
    {
        FILE *descriptor_log = fopen(DPMI_TRACE_LOG, "a");
        LDT_ENTRY *entry = (LDT_ENTRY *)Sim32GetVDMPointer(
            (getES() << 16) | getBX(), 8, (UCHAR)(getMSW() & MSW_PE));
        if (descriptor_log) {
            fprintf(descriptor_log, "S38_POSTFAIL_DESCRIPTOR selector=%04X count=%u base=%08lX limit=%04X\n",
                getAX(), getCX(), (ULONG)entry->BaseLow |
                ((ULONG)entry->HighWord.Bytes.BaseMid << 16) |
                ((ULONG)entry->HighWord.Bytes.BaseHi << 24), entry->LimitLow);
            fclose(descriptor_log);
        }
    }
#endif
#ifdef DPMI_TRACE_LIFECYCLE_ONLY
    if (dispatch_index != 0x11 && dispatch_index != 5 &&
        dispatch_index != 0x0d && dispatch_index != 0x10 && dispatch_index != 0x13) {
        SetLastError(saved_error);
        s36_original_DpmiDispatch();
        return;
    }
#endif
    FILE *log = fopen(DPMI_TRACE_LOG, "a");
    /* Keep no log resource across guest execution or a non-local return.
     * Index is global and may change during a nested dispatch. */
    if (log) {
        snapshot(log, "before", dispatch_index);
        fclose(log);
    }
    SetLastError(saved_error);
    __try { s36_original_DpmiDispatch(); }
    __except (native_fault(GetExceptionInformation())) { }
    saved_error = GetLastError();
    log = fopen(DPMI_TRACE_LOG, "a");
    if (log) {
        snapshot(log, "after", dispatch_index);
        if ((dispatch_index == 0x11 || dispatch_index == 5 ||
             dispatch_index == 0x0d || dispatch_index == 0x13) &&
            Cpu40PmStackInfoAddress) {
            PVDM_DPMIINFO info = (PVDM_DPMIINFO)(IntelBase + Cpu40PmStackInfoAddress);
            fprintf(log, "pm-stack address=%08lX size=%lu lock=%u flags=%04X ss=%04X "
                "saved-ss=%04X saved-esp=%08lX saved-eip=%08lX "
                "int16=%08lX int32=%08lX fault16=%08lX fault32=%08lX reflector=%08lX\n",
                Cpu40PmStackInfoAddress, (ULONG)sizeof(*info), info->LockCount,
                info->Flags, info->SsSelector, info->SaveSsSelector,
                info->SaveEsp, info->SaveEip, info->DosxIntIret,
                info->DosxIntIretD, info->DosxFaultIret, info->DosxFaultIretD,
                info->DosxRmReflector);
            fprintf(log, "pm-stack-owner index=%02lX app-flags=%04X\n",
                dispatch_index, CurrentAppFlags);
        }
        fclose(log);
    }
    SetLastError(saved_error);
}
