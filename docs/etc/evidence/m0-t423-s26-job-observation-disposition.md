# M0 T423 S26 — Job observation disposition

## Question and baseline

Decide whether the retained Job-completion-port candidate can safely add
best-effort `Observed` elements to NTSRV's single Win32Record list without
changing the accepted S25 direct-command and worker-lifecycle baseline.
Input was `main` at `5cdc3ffe913d05aae1053656cef737999d67a4b4`, the
protocol-24 eight-file package at `O:/winnt`, the isolated Job fixture, and
the Windows Job API contract. No guest or product executable was changed.

## Procedure and observations

1. Inspected the selected product graph and callers. The candidate
   `native_job_tracker.c` was compiled only for
   `ntsrv-native-job-tracker-test.exe`; no production NTSRV caller or link
   selected it. Production Win32Records remain direct-request records.
2. Generated the x86 graph under `build/M0-T423/S26/formal` with
   `tools/build/New-T310OriginalSoftpcNinja.ps1 -Architecture x86`, using
   Node 22.22.1. Built the isolated target with MSVC Win32/x86 `/MT`.
   Fixture SHA-256:
   `DB7AE38F0B1B4375A813CAAC2B531B41C17F714F4AA4E01984D1CEB958677094`.
3. Ran the fixture three times via temporary `subst Z:` and unmapped Z:
   each run reported `new=2 exit=2` and proved that closing a Job handle
   left its still-running target alive. These bounded passes do not prove
   complete notification delivery or PID identity.
4. [Microsoft's Job completion-port contract](https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-jobobject_associate_completion_port)
   explicitly says ordinary messages are not guaranteed and that a PID in a
   message may have been recycled unless an open process handle is held.
   [Nested Job rules](https://learn.microsoft.com/en-us/windows/win32/procthread/nested-jobs)
   also permit descendant breakaway subject to the job hierarchy.
5. The candidate stores an `OPENNT_BASE_CONNECTION *` as an unowned `void *`
   in a Job node. Its completion thread copies that pointer under its own
   lock and calls back after unlocking. The product connection rundown can
   free that object; `Discard` removes only unbound nodes. Thus a bound Job
   outliving the connection would expose an unprotected callback lifetime.
   The fixture has no disconnect/concurrent-rundown assertion. `NEW_PROCESS`
   also queries parent by PID after the event, so a fast exit/reuse cannot
   provide a reliable parent relation in this implementation.

## Decision and disposition

**Reject product admission of this candidate in S26.** A best-effort display
would be legitimate only with a stable callback owner, process-handle-backed
identity, explicit missed-event/unknown-parent representation, and negative
disconnect/PID-reuse tests. Adding those solely for a diagnostic stack would
expand NTSRV state and the direct startup path without improving execution
correctness. The owner-approved alternative in the S26 brief is therefore
used: the untouched algorithm is retained as an isolated research fixture
under `tests/observation/`, and removed from the NTSRV product-source tree.

Product contract after S26: one Win32Record list continues to hold only
authenticated `Direct` entries. NTMON may report those requests and their
actual bound target PID; it must not claim a complete native descendant
graph. Native children without their own Run16 request remain Windows-owned
and unrepresented, including external `AttachConsole` participants. Neither
their existence nor missing Job messages can decide direct receipt,
READY/EMPTY, shutdown, or scheduling. S28's unified management projection
must reflect this limitation explicitly. No package publication is needed:
the production source/link graph and eight-file package are unchanged.

## Verification and follow-up

The relocated fixture compiled and passed three local runs. Source/build
search found no remaining `native_job_tracker` reference in product source;
the only graph entries are the isolated fixture. The negative lifetime and
missed-notification conditions remain **unproved**, not passing tests.
S27 proceeds with the direct-worker control-plane closure; S29 retains the
owner audit of this limitation and the S25 final-screen-history failure.
