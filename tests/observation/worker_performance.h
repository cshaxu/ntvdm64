#ifndef TEST_WORKER_PERFORMANCE_H
#define TEST_WORKER_PERFORMANCE_H
#include <windows.h>
#include "ntvdm-exe/softpc/mvdm_softpc_mouse_input.h"
/* Test-link only. No file I/O on measured paths; flush after ownership release.
 * One imported machine per worker, matching the selected product profile. */
BOOL worker_performance_enabled(void);
LONGLONG worker_performance_clock(void);
void worker_performance_record(const char *,LONGLONG,DWORD,DWORD);
/* Sum of disjoint API-call durations, not a continuous phase interval. */
void worker_performance_record_total(const char *,LONGLONG,DWORD,DWORD);
void worker_performance_push(mvdm_mouse_input *,DWORD,int,LONGLONG);
void worker_performance_take(mvdm_mouse_input *,DWORD,DWORD,int,LONGLONG);
void worker_performance_flush(void);
#endif
