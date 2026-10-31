#ifndef OPENNT_HOST_NTEXAPI_SUBSET_H
#define OPENNT_HOST_NTEXAPI_SUBSET_H

/*
 * DIVERGENCE(OPENNT-HOST-001): This is the selected true subset of the
 * original OpenNT public/sdk/inc/ntexapi.h.  The original non-MONITOR
 * SoftPC EOI controller keeps its original include, while the original
 * SoftPC heartbeat reaches NtQueryPerformanceCounter and NtGetTickCount.
 * The MONITOR-only NtVdmControl branch is outside the selected SoftPC
 * profile.  The modern SDK may define the original `_NTEXAPI_` guard before this
 * selected source header is reached, so this subset has a private guard.
 * Retaining only this original include carrier avoids importing a kernel-service
 * surface into a modern user-mode build; it does not provide a substitute
 * controller or kernel-VDM implementation.
 */

/* Retained byte-identical declarations from the selected original header.
 * Their implementation binding is intentionally outside this mirror. */
NTSYSAPI
NTSTATUS
NTAPI
NtQueryPerformanceCounter (
    OUT PLARGE_INTEGER PerformanceCounter,
    OUT PLARGE_INTEGER PerformanceFrequency OPTIONAL
    );

NTSYSAPI
ULONG
NTAPI
NtGetTickCount (
    VOID
    );

#endif
