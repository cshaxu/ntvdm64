/* Independent native observer/instrumentation control; not a VDM worker.
 * Output is deliberately named ntvdm.exe inside this fixture's build root
 * so the same module/range validation is exercised, without a helper HWND. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "wow_page_cost_instrument.h"
static DWORD owner_pid;
#include "wow_page_cost_probe.h"

static int __cdecl compare_ulong(const void *left, const void *right)
{
    ULONG a = *(const ULONG *)left, b = *(const ULONG *)right;
    return (a > b) - (a < b);
}
static void reset_reader(void)
{
    if (page_cost_process) CloseHandle(page_cost_process);
    page_cost_process = NULL; page_cost_address = 0;
}
#define CHECK(value) do { if (!(value)) {fprintf(stderr,"cost control failed line=%u\n",__LINE__);return 1;} } while (0)

int main(void)
{
    ULONG values[] = {4, 2, 3, 1};
    LONGLONG outer, inner;
    DWORD original_rva;
    owner_pid = GetCurrentProcessId();
    original_rva = page_cost_rva = (DWORD)((ULONG_PTR)&T447ProfileData -
        (ULONG_PTR)GetModuleHandleW(NULL));
    SetLastError(0x447c055);
    outer = page_cost_begin();
    inner = page_cost_begin();
    CHECK(T447ProfileData.sequence == 1);
    page_cost_qsort(values, 4, sizeof(*values), compare_ulong);
    CHECK(T447ProfileData.sequence == 1);
    page_cost_end(PAGE_COST_CLASS, inner, 0);
    page_cost_end(PAGE_COST_GRAPH, outer, 4);
    CHECK(GetLastError() == 0x447c055 && T447ProfileData.sequence == 2);
    CHECK(T447ProfileData.rows[PAGE_COST_CLASS_SORT].calls == 1 &&
        T447ProfileData.rows[PAGE_COST_CLASS_SORT].items == 4 &&
        T447ProfileData.rows[PAGE_COST_GRAPH].calls == 1 &&
        T447ProfileData.rows[PAGE_COST_GRAPH].ticks >=
            T447ProfileData.rows[PAGE_COST_CLASS].ticks &&
        T447ProfileData.rows[PAGE_COST_CLASS].ticks >=
            T447ProfileData.rows[PAGE_COST_CLASS_SORT].ticks);
    CHECK(values[0] == 1 && values[3] == 4);
    CHECK(page_cost_snapshot("control", 0));
    T447ProfileData.magic = 0;
    CHECK(!page_cost_snapshot("bad-magic", 0));
    T447ProfileData.magic = WOW_PAGE_COST_MAGIC;
    T447ProfileData.version = 2;
    CHECK(!page_cost_snapshot("bad-version", 0));
    T447ProfileData.version = 1;
    T447ProfileData.sequence = 3;
    CHECK(!page_cost_snapshot("busy", 0));
    T447ProfileData.sequence = 4;
    reset_reader(); page_cost_rva = MAXDWORD;
    CHECK(!page_cost_snapshot("outside-image", 0));
    reset_reader(); page_cost_rva = original_rva; owner_pid = MAXDWORD;
    CHECK(!page_cost_snapshot("wrong-pid", 0));
    reset_reader();
    puts("PASS nested inclusive counters, LastError, real read-only snapshot and five rejected diagnostic identities/states");
    return 0;
}
