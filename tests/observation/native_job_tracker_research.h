#ifndef OPENNT_NATIVE_JOB_TRACKER_H
#define OPENNT_NATIVE_JOB_TRACKER_H
#include <windows.h>

/* Retained S26 research fixture, not linked into NTSRV. Ordinary Job
 * notifications are not guaranteed and this candidate has no connection
 * rundown barrier; it must not provide product completion or readiness. */
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
