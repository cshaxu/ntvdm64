#include <windows.h>
#include <stdlib.h>
#include "wow_page_cost_data.h"

/* Included only in a generated copy of the project adapter. Domain execution
 * serializes its native writers. The outermost instrumented call publishes
 * an even sequence only after its nested counters have finished updating. */
wow_page_cost_data T447ProfileData = {WOW_PAGE_COST_MAGIC, 1};
static __declspec(thread) unsigned page_cost_depth;

static LONGLONG page_cost_begin(void)
{
    DWORD error = GetLastError();
    LARGE_INTEGER now, frequency;
    if (!page_cost_depth++) InterlockedIncrement(&T447ProfileData.sequence);
    if (!T447ProfileData.frequency) {
        QueryPerformanceFrequency(&frequency);
        T447ProfileData.frequency = frequency.QuadPart;
    }
    QueryPerformanceCounter(&now);
    SetLastError(error);
    return now.QuadPart;
}

static void page_cost_end(unsigned kind, LONGLONG started, ULONGLONG items)
{
    DWORD error = GetLastError();
    LARGE_INTEGER now;
    wow_page_cost_row *row = &T447ProfileData.rows[kind];
    ULONGLONG ticks;
    QueryPerformanceCounter(&now);
    ticks = now.QuadPart - started;
    ++row->calls; row->ticks += ticks; row->items += items;
    if (items > row->largest_items) row->largest_items = items;
    if (ticks > row->longest_ticks) row->longest_ticks = ticks;
    if (!--page_cost_depth) InterlockedIncrement(&T447ProfileData.sequence);
    SetLastError(error);
}

static void __cdecl page_cost_qsort(void *base, size_t count, size_t width,
    int (__cdecl *compare)(const void *, const void *))
{
    LONGLONG started = page_cost_begin();
    qsort(base, count, width, compare);
    page_cost_end(width == sizeof(ULONG) ? PAGE_COST_CLASS_SORT :
        PAGE_COST_VIEW_SORT, started, count);
}
