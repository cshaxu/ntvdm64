/* Original TD layout, controlled registration witness; no guest is running. */
#include "precomp.h"
PTD gptdTaskHead;
CRITICAL_SECTION gcsWOW;
static TD startup_td;
static PVOID saved_td;
void WINAPI wow_fixture_startup_td_begin(WORD task)
{
    InitializeCriticalSection(&gcsWOW);
    ZeroMemory(&startup_td,sizeof(startup_td));
    startup_td.htask16=task;
    startup_td.dwThreadID=GetCurrentThreadId();
    saved_td=NtCurrentTeb()->WOW32Reserved;
    NtCurrentTeb()->WOW32Reserved=&startup_td;
    gptdTaskHead=&startup_td;
}
void WINAPI wow_fixture_startup_td_end(void)
{
    gptdTaskHead=NULL;
    NtCurrentTeb()->WOW32Reserved=saved_td;
    DeleteCriticalSection(&gcsWOW);
}
