/* Isolated cost of the actual project diagnostic provider; not an emulator
 * benchmark or evidence of a product speedup. Link the original provider TU
 * with function sections, retaining only this referenced function. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
extern void mvdm_softpc_report_nt_transition(unsigned short, unsigned long,
    unsigned long, unsigned short, unsigned long);
/* Unused boundaries of the provider translation unit. The isolated diagnostic
 * below reaches none of them; fail closed if that changes. No guest or
 * provider behavior is simulated by these link-only sentinels. */
void *Ldt;
unsigned long IntelBase, Cpu40GdtShadowAddress, Cpu40LdtShadowAddress;
unsigned long FlatAddress[1];
int session_terminate_current(unsigned long code)
{ (void)code; ExitProcess(90); }
void host_applClose(void) { ExitProcess(91); }
int xtrn2phy(unsigned long address,unsigned char access,unsigned long *physical)
{ (void)address;(void)access;(void)physical;ExitProcess(92); }
void c_sas_loads(unsigned long address,unsigned char *destination,unsigned long bytes)
{ (void)address;(void)destination;(void)bytes;ExitProcess(93); }
int main(void)
{
    LARGE_INTEGER frequency, start, finish;
    volatile unsigned long control=0;
    unsigned trial, index;
    const unsigned iterations=5000000;
    if(GetEnvironmentVariableA("MVDM_WOW_NT_TRACE_PATH",NULL,0))return 64;
    if(!QueryPerformanceFrequency(&frequency))return 65;
    /* Initialize the optional path before measuring the disabled hot path. */
    mvdm_softpc_report_nt_transition(0,0,0,0,0);
    for(trial=0;trial<6;++trial){
        QueryPerformanceCounter(&start);
        for(index=0;index<iterations;++index)control^=index;
        QueryPerformanceCounter(&finish);
        printf("trial=%u kind=control iterations=%u ticks=%lld frequency=%lld\n",
            trial,iterations,finish.QuadPart-start.QuadPart,frequency.QuadPart);
        SetLastError(0x53510001);
        QueryPerformanceCounter(&start);
        for(index=0;index<iterations;++index)
            mvdm_softpc_report_nt_transition(0x1234,index,0x202,0x5678,0x8000);
        QueryPerformanceCounter(&finish);
        if(GetLastError()!=0x53510001)return 66;
        printf("trial=%u kind=diagnostic-disabled iterations=%u ticks=%lld frequency=%lld\n",
            trial,iterations,finish.QuadPart-start.QuadPart,frequency.QuadPart);
    }
    return control==0 ? 0 : 67;
}
