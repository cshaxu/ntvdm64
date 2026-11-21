/* Original TD layout, controlled registration witness; no guest is running. */
#include "precomp.h"
#include <stdio.h>
#include "wow_task_cleanup_entry.h"
PTD gptdTaskHead;
CRITICAL_SECTION gcsWOW;
#define wow_original_task_cleanup_uninitialized fixture_startup_uninitialized
#include "startup-owner.inc"
#undef wow_original_task_cleanup_uninitialized
extern VOID WINAPI wow_original_task_retire_async(void);
static VOID WINAPI unexpected_free(PVOID task) { (void)task; fprintf(stderr,"UNEXPECTED_STARTUP_FREE\n"); RaiseFailFastException(NULL,NULL,0); }
static const wow_task_cleanup_provider startup_provider = {
    fixture_startup_uninitialized, wow_original_task_retire_async, unexpected_free };
static TD startup_td;
static PVOID saved_td;
void WINAPI wow_fixture_startup_bind(void)
{
    if (!wow_task_cleanup_bind(&startup_provider)) { fprintf(stderr,"STARTUP_BIND_FAILED error=%lu\n",GetLastError()); RaiseFailFastException(NULL,NULL,0); }
}
void WINAPI wow_fixture_startup_td_begin(WORD task)
{
    wow_fixture_startup_bind();
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
