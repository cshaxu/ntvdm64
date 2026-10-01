#ifndef OPENNT_NATIVE_JOB_TRACKER_H
#define OPENNT_NATIVE_JOB_TRACKER_H
#include <windows.h>

/* A server-owned, event-only Job projection.  Its notifications are useful
 * monitor observations only; callers must not use them as task completion or
 * worker-readiness authority.  NTCON supplies a suspended target through its
 * existing authenticated bind RPC; only NTSRV ever owns the Job handle. */
typedef struct OPENNT_NATIVE_JOB_TRACKER OPENNT_NATIVE_JOB_TRACKER;
/* owner, worker generation, direct request, Job message, subject PID,
 * observed parent PID, and the Job's suspended direct-root PID. */
typedef void (WINAPI *OPENNT_NATIVE_JOB_REPORT)(void *,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD);
DWORD OpenNtNativeJobTrackerOpen(OPENNT_NATIVE_JOB_TRACKER **,
    OPENNT_NATIVE_JOB_REPORT);
DWORD OpenNtNativeJobTrackerCreate(OPENNT_NATIVE_JOB_TRACKER *,void *,DWORD,DWORD);
DWORD OpenNtNativeJobTrackerAssign(OPENNT_NATIVE_JOB_TRACKER *,void *,DWORD,DWORD,HANDLE);
void OpenNtNativeJobTrackerDiscard(OPENNT_NATIVE_JOB_TRACKER *,void *,DWORD,DWORD);
void OpenNtNativeJobTrackerClose(OPENNT_NATIVE_JOB_TRACKER *);
#endif
