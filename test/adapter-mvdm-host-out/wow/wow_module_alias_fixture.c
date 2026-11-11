#include "precomp.h"

PVOID FASTCALL malloc_w(ULONG size)
{
    return HeapAlloc(GetProcessHeap(), 0, size);
}

VOID FASTCALL free_w(PVOID value)
{
    if (value) HeapFree(GetProcessHeap(), 0, value);
}

static DWORD WINAPI wait_for_gate(LPVOID value)
{
    WaitForSingleObject((HANDLE)value, INFINITE);
    return 0;
}

static int exercise_alias_reuse_and_retirement(void)
{
    HANDLE gate = NULL;
    HANDLE threads[32] = {0};
    HANDLE retired_gate = NULL;
    HANDLE retired_thread = NULL;
    HTASK16 first_alias;
    HTASK16 overflow_alias;
    HTASK16 retired_alias;
    DWORD overflow_tid = 0;
    DWORD retired_tid = 0;
    UINT index;
    int result = 1;

    gate = CreateEventA(NULL, TRUE, FALSE, NULL);
    if (!gate) goto done;

    /* The current thread occupies the oldest slot. Thirty-one live worker
     * threads fill the remainder; the thirty-second must reuse slot zero,
     * exactly as the original 32-entry oldest-creation-time policy requires. */
    first_alias = AddHtaskAlias(GetCurrentThreadId());
    if (!first_alias) goto done;
    for (index = 0; index != 32; ++index) {
        DWORD tid = 0;
        threads[index] = CreateThread(NULL, 0, wait_for_gate, gate, 0, &tid);
        if (!threads[index] || !AddHtaskAlias(tid)) goto done;
        if (index == 31) overflow_tid = tid;
    }
    overflow_alias = FindHtaskAlias(overflow_tid);
    if (overflow_alias != first_alias ||
        FindHtaskAlias(GetCurrentThreadId()) != 0 ||
        GetHtaskAlias(first_alias, NULL) != overflow_tid) goto done;

    /* Thread termination alone is not a stale-identity test: the original
     * resolver compares PID and creation time, not thread signaled state.
     * Explicit removal withdraws the slot; later TID reuse is detected by
     * that same PID/creation-time comparison when the OS exposes it. */
    retired_gate = CreateEventA(NULL, TRUE, FALSE, NULL);
    if (!retired_gate) goto done;
    retired_thread = CreateThread(NULL, 0, wait_for_gate, retired_gate, 0,
        &retired_tid);
    if (!retired_thread) goto done;
    retired_alias = AddHtaskAlias(retired_tid);
    if (!retired_alias || GetHtaskAlias(retired_alias, NULL) != retired_tid)
        goto done;
    SetEvent(retired_gate);
    if (WaitForSingleObject(retired_thread, 5000) != WAIT_OBJECT_0 ||
        GetHtaskAlias(retired_alias, NULL) != retired_tid) goto done;
    RemoveHtaskAlias(retired_alias);
    if (GetHtaskAlias(retired_alias, NULL) != 0) goto done;
    result = 0;

done:
    if (retired_gate) SetEvent(retired_gate);
    if (gate) SetEvent(gate);
    if (retired_thread) {
        WaitForSingleObject(retired_thread, 5000);
        CloseHandle(retired_thread);
    }
    for (index = 0; index != ARRAYSIZE(threads); ++index) {
        if (threads[index]) {
            WaitForSingleObject(threads[index], 5000);
            CloseHandle(threads[index]);
        }
    }
    if (retired_gate) CloseHandle(retired_gate);
    if (gate) CloseHandle(gate);
    return result;
}

int __cdecl main(void)
{
    HTASK16 alias;
    CHAR full[32768];
    CHAR short_name[4] = { 'x', 'x', 'x', 'x' };
    CHAR one[1] = { 'x' };
    UINT full_length;
    UINT short_length;
    UINT one_length;
    DWORD written;
    static const CHAR marker[] = "WOW_MODULE_ALIAS_BOUNDS_AND_LIFETIME_OK\r\n";

    alias = AddHtaskAlias(GetCurrentThreadId());
    if (!alias || FindHtaskAlias(GetCurrentThreadId()) != alias) return 1;

    full_length = GetHtaskAliasProcessName(alias, full, sizeof(full));
    if (!full_length || full[full_length] != '\0') return 2;

    short_length = GetHtaskAliasProcessName(alias, short_name, sizeof(short_name));
    if (short_length != sizeof(short_name) - 1 || short_name[3] != '\0' ||
        memcmp(short_name, full, short_length) != 0) return 3;

    one_length = GetHtaskAliasProcessName(alias, one, sizeof(one));
    if (one_length || one[0] != '\0') return 4;
    RemoveHtaskAlias(alias);
    if (GetHtaskAliasProcessName(0, full, sizeof(full)) != 0 ||
        full[0] != '\0') return 5;
    if (exercise_alias_reuse_and_retirement()) return 7;

    UNREFERENCED_PARAMETER(full_length);
    UNREFERENCED_PARAMETER(short_length);
    UNREFERENCED_PARAMETER(one_length);
    if (!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), marker,
                   sizeof(marker) - 1, &written, NULL) ||
        written != sizeof(marker) - 1) return 6;
    return 0;
}
