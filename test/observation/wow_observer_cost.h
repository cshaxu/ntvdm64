#ifndef WOW_OBSERVER_COST_H
#define WOW_OBSERVER_COST_H
#include <windows.h>

/* Test-only native leaf brackets. Their wall times can include waiting for
 * the painter/compositor; never subtract them to manufacture guest CPU time.
 * Include before defining the API redirection macros in the probe. */
typedef struct observer_cost_sample {
    LONGLONG blit_ticks, flush_ticks, sleep_ticks;
    ULONG blit_calls, flush_calls, sleep_calls;
} observer_cost_sample;
static observer_cost_sample observer_cost;

static BOOL WINAPI observer_cost_BitBlt(HDC dst,int x,int y,int width,int height,
    HDC src,int sx,int sy,DWORD operation)
{
    LARGE_INTEGER before,after;
    DWORD saved=GetLastError(),error;
    BOOL result;
    QueryPerformanceCounter(&before);SetLastError(saved);
    result=BitBlt(dst,x,y,width,height,src,sx,sy,operation);
    error=GetLastError();QueryPerformanceCounter(&after);
    observer_cost.blit_ticks+=after.QuadPart-before.QuadPart;
    ++observer_cost.blit_calls;SetLastError(error);return result;
}
static BOOL WINAPI observer_cost_GdiFlush(void)
{
    LARGE_INTEGER before,after;
    DWORD saved=GetLastError(),error;
    BOOL result;
    QueryPerformanceCounter(&before);SetLastError(saved);
    result=GdiFlush();error=GetLastError();QueryPerformanceCounter(&after);
    observer_cost.flush_ticks+=after.QuadPart-before.QuadPart;
    ++observer_cost.flush_calls;SetLastError(error);return result;
}
static void WINAPI observer_cost_Sleep(DWORD milliseconds)
{
    LARGE_INTEGER before,after;
    DWORD saved=GetLastError(),error;
    QueryPerformanceCounter(&before);SetLastError(saved);
    Sleep(milliseconds);error=GetLastError();QueryPerformanceCounter(&after);
    observer_cost.sleep_ticks+=after.QuadPart-before.QuadPart;
    ++observer_cost.sleep_calls;SetLastError(error);
}
#endif
