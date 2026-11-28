# S15 N024 exitwin migration

P76 runtime92 verified for delivery. Starting HEAD is
cae0623c6b1042343eca9322b7ed3472710cc8e5; accepted package runtime91.
The registered pre-migration OpenNT nonerror difference is1point/646lines.

## Fixed inputs and scope

XP commit a2f6d7c93aa4f11efc51147566d5961c59a8ef89, original path
Source/XPSP1/NT/windows/core/ntuser/kernel/exitwin.c, raw blob
675433e732790375b7d6c9bb01a55bebf86ca84f. Registered OpenNT blob is
b475f44d3102bd70dfde46ff8ed40182a36404c0. Original bytes, starting source and
hashes are retained under build/t485-s15/n024-exitwin.

Restore the complete XP xxxRegisterUserHungAppHandlers body (line574 onward),
including zero-initialized allocation, event-type requirement, failure release,
callback/client-event assignment and process-list publication. Retain the
existing explicit current-process parameter and same-thread registration owner.
The B-owned process view now stores the callback as the original function
pointer type; no DWORD round-trip. Its only reader is the registration fixture;
production registration saves it for the existing process lifetime.

The existing wow_task_reference_event implementation duplicates with unchanged
access, checks EVENT_ALL_ACCESS and the actual Event object type, and closes
the duplicate on failure. Thus the original ExEventObjectType argument remains
represented by the provider contract, not a no-op type check. Kernel pool
zero-init maps to HeapAlloc HEAP_ZERO_MEMORY; original failure frees through
the existing UserFreePool binding. No new state, callback table or API route.

## Whole-file boundary audit

| Original function | XP line | Unavailable host owner |
| --- | --- | --- |
| PrepareForLogoff | 33 | kernel PETHREAD/job UI restriction and Winlogon profile state |
| NotifyLogon | 73 | kernel window-station LUID and global Winlogon notification window |
| InitiateShutdown | 126 | EPROCESS/ETHREAD, privileges, window-station locking and global shutdown state |
| EndShutdown | 332 | kernel desktop heaps, station clipboard/atom table and GDI font teardown |
| xxxClientShutdown2 | 437 | kernel window-list locks, shutdown notifier and timer ownership |
| xxxClientShutdown | 530 | kernel desktop child-list enumeration and server shutdown dispatch |

These are existing unconditionally omitted system-shutdown bodies, not new
unsupported conditional branches. Their actual structure members and providers
are absent from the finite WOW process/thread view. Modern USER owns host
logoff; this registration migration neither imports nor replaces that owner.
No fabricated exclusion condition hides those omissions. The existing LATER
alternative remains #error at its XP diagnostic lines. The harmless original
global declarations, OPTIONMASK and PRERELEASE declaration block are restored;
being unused is not grounds to remove them. The second PRERELEASE block belongs
to the omitted NotifyLogon body and is accounted for with that body.

## Verification plan

Cached full build, dual13-image publication and26hash checks, existing task-order
event reference/negative tests, registered task/process lifecycle cases and
genuine original Win16 controls. Retain allocation/list/event lifetime behavior.
P75 Full24/24 is the current checkpoint; this is one subsequent mirror file.
Final evidence is recorded below.

## Final delivery

[Receipt](n024-exitwin-result.json) and [mirror census](n024-exitwin-census.json).
Cached full build, dual13-image publication/26SHA256 checks, task-order,
task-lifecycle, process-retirement and genuine Win16 controls pass. Event tests
cover null, insufficient-access, wrong-type and process handles; repeated
failure does not publish process state or leak handles. Success preserves the
callback, zero state, list order and retained event lifetime after caller close.
Initial build passed; a #else diagnostic-line mismatch was corrected before
the final cached build/publication. All77unchanged nonblank XP lines now map
correctly. Existing LATER #error exclusion remains; no guest modification.

exitwin.c changes from OpenNT1point/646lines to XP5points/546lines.
The original LF restoration exposes real separate spans; unconditional system
shutdown omissions remain explicitly accounted for above, not hidden as errors.
Whole census: 1332files/792byte-identical/
803nonerror-identical/529different/
7961points/63292lines.
Delta: 0files/0exact/0same/0different/+4points/-100lines.
Baseline counts1316XP/16OpenNT. N0243/9, S15 11/14, overall59/159;
one file since P75 Full. Next getset.c; no S/T closure.
