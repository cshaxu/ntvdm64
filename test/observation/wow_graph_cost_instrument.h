#include <windows.h>
#include "wow_page_cost_data.h"

/* Reuse only the native counter layout, not the page-kind interpretation.
 * 0=capture,1=normalize,2=project,3=refresh,4=commit,5=unused.
 * Domain entry serializes writers; detect a violated premise rather than
 * publishing plausible counters. All durations include nested kinds. */
wow_page_cost_data T447ProfileData={WOW_PAGE_COST_MAGIC,1};
static volatile LONG graph_cost_owner;
static __declspec(thread) unsigned graph_cost_depth;
static __declspec(thread) BOOL graph_cost_owned;
static LONGLONG graph_cost_begin(void)
{
    DWORD error=GetLastError();
    LARGE_INTEGER now,frequency;
    if(!graph_cost_depth++){
        graph_cost_owned=InterlockedCompareExchange(&graph_cost_owner,
            (LONG)GetCurrentThreadId(),0)==0;
        if(!graph_cost_owned)InterlockedExchange((volatile LONG *)&T447ProfileData.reserved,1);
        else InterlockedIncrement(&T447ProfileData.sequence);
    }
    if(graph_cost_owned && !T447ProfileData.frequency){
        QueryPerformanceFrequency(&frequency);
        T447ProfileData.frequency=frequency.QuadPart;
    }
    QueryPerformanceCounter(&now);SetLastError(error);return now.QuadPart;
}
static void graph_cost_end(unsigned kind,LONGLONG start,ULONGLONG items)
{
    DWORD error=GetLastError();
    LARGE_INTEGER now;
    if(graph_cost_owned){
        wow_page_cost_row *row=&T447ProfileData.rows[kind];
        ULONGLONG ticks;
        QueryPerformanceCounter(&now);ticks=now.QuadPart-start;
        ++row->calls;row->ticks+=ticks;row->items+=items;
        if(items>row->largest_items)row->largest_items=items;
        if(ticks>row->longest_ticks)row->longest_ticks=ticks;
    }
    if(!--graph_cost_depth && graph_cost_owned){
        InterlockedIncrement(&T447ProfileData.sequence);
        InterlockedExchange(&graph_cost_owner,0);
        graph_cost_owned=FALSE;
    }
    SetLastError(error);
}
