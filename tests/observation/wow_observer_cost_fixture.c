#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

static unsigned errors,blits,flushes,sleeps;
static LONGLONG ticks;
#define CHECK(value) do {if(!(value)){fprintf(stderr,"observer-leaf line=%u\n",__LINE__);++errors;}}while(0)
static BOOL WINAPI fixture_clock(LARGE_INTEGER *value)
{value->QuadPart=(ticks+=7);SetLastError(0x447999u);return TRUE;}
static BOOL WINAPI fixture_blit(HDC dst,int x,int y,int width,int height,
    HDC src,int sx,int sy,DWORD operation)
{
    CHECK(GetLastError()==0x447100u && dst==(HDC)1 && src==(HDC)2 &&
        x==3 && y==4 && width==5 && height==6 && sx==7 && sy==8 && operation==SRCCOPY);
    ++blits;SetLastError(0x447200u);return blits==2;
}
static BOOL WINAPI fixture_flush(void)
{CHECK(GetLastError()==0x447100u);++flushes;SetLastError(0x447300u);return flushes==2;}
static void WINAPI fixture_sleep(DWORD ms)
{CHECK(GetLastError()==0x447100u && ms==20);++sleeps;SetLastError(0x447400u);}
#define QueryPerformanceCounter fixture_clock
#define BitBlt fixture_blit
#define GdiFlush fixture_flush
#define Sleep fixture_sleep
#include "wow_observer_cost.h"
#undef QueryPerformanceCounter
#undef BitBlt
#undef GdiFlush
#undef Sleep
int main(void)
{
    unsigned i;
    for(i=0;i<2;++i){
        BOOL result;
        SetLastError(0x447100u);
        result=observer_cost_BitBlt((HDC)1,3,4,5,6,(HDC)2,7,8,SRCCOPY);
        CHECK(result==(BOOL)i && GetLastError()==0x447200u);
        SetLastError(0x447100u);result=observer_cost_GdiFlush();
        CHECK(result==(BOOL)i && GetLastError()==0x447300u);
    }
    SetLastError(0x447100u);observer_cost_Sleep(20);
    CHECK(GetLastError()==0x447400u && sleeps==1 && blits==2 && flushes==2 &&
        observer_cost.blit_calls==2 && observer_cost.flush_calls==2 &&
        observer_cost.sleep_calls==1 && observer_cost.blit_ticks==14 &&
        observer_cost.flush_ticks==14 && observer_cost.sleep_ticks==7);
    ZeroMemory(&observer_cost,sizeof(observer_cost));
    CHECK(!observer_cost.blit_ticks && !observer_cost.flush_ticks &&
        !observer_cost.sleep_ticks && !observer_cost.blit_calls &&
        !observer_cost.flush_calls && !observer_cost.sleep_calls);
    printf("OBSERVER_COST_UNIT errors=%u native_calls=5 exact_args=1 success_failure=1 exact_errors=1 qpc=1 reset=1\n",errors);
    return errors?1:0;
}
