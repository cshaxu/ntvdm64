#include <windows.h>
#include "wow_callback_cost_data.h"

/* Diagnostic copy only. Each reservation has one writer and is never reused.
 * complete publishes immutable row fields; readers reject incomplete or
 * overflowing records. Nested callback intervals are inclusive, not CPU time. */
__declspec(align(8)) __declspec(dllexport) callback_cost_data T447CallbackCosts = {
    CALLBACK_COST_MAGIC,2,CALLBACK_COST_CAPACITY
};
static __declspec(thread) LONG callback_cost_active=-1;
static void callback_cost_phase_enter(void)
{
    DWORD error=GetLastError();
    LONG slot=callback_cost_active;
    if(slot>=0 && (ULONG)slot<CALLBACK_COST_CAPACITY){
        LARGE_INTEGER now;
        callback_cost_row *row=&T447CallbackCosts.rows[slot];
        QueryPerformanceCounter(&now);row->phase_entered=now.QuadPart;
        ++row->phase_count;
    }
    SetLastError(error);
}
static void callback_cost_phase_leave(void)
{
    DWORD error=GetLastError();
    LONG slot=callback_cost_active;
    if(slot>=0 && (ULONG)slot<CALLBACK_COST_CAPACITY){
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        T447CallbackCosts.rows[slot].phase_returned=now.QuadPart;
    }
    SetLastError(error);
}
static LONG callback_cost_begin(HWND window,UINT message)
{
    LONG slot;
    LARGE_INTEGER now,frequency;
    DWORD error=GetLastError();
    if(message!=WM_RBUTTONDOWN && message!=WM_RBUTTONUP &&
            message!=WM_LBUTTONDOWN && message!=WM_LBUTTONUP)return -1;
    slot=InterlockedIncrement(&T447CallbackCosts.reserved)-1;
    if(slot>=0 && (ULONG)slot<CALLBACK_COST_CAPACITY){
        callback_cost_row *row=&T447CallbackCosts.rows[slot];
        QueryPerformanceFrequency(&frequency);
        (void)InterlockedCompareExchange64(&T447CallbackCosts.frequency,
            frequency.QuadPart,0);
        row->window=(DWORD)(ULONG_PTR)window;row->message=message;
        row->thread=GetCurrentThreadId();QueryPerformanceCounter(&now);
        row->entered=now.QuadPart;
    }
    SetLastError(error);return slot;
}
static void callback_cost_end(LONG slot,BOOL accepted,BOOL normal,LONG result,
    DWORD error)
{
    if(slot>=0 && (ULONG)slot<CALLBACK_COST_CAPACITY){
        LARGE_INTEGER now;
        callback_cost_row *row=&T447CallbackCosts.rows[slot];
        QueryPerformanceCounter(&now);row->returned=now.QuadPart;
        row->accepted=accepted?1:0;row->normal=normal?1:0;
        row->result=result;row->error=error;
        InterlockedExchange(&row->complete,1);
    }
    SetLastError(error);
}
