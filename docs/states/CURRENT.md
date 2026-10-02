# Project Status

## Current Work

## Active Packet

**Active: M0 T424 S4** — broker-centered launch and control migration,
Ordinary Mode. Owner clarifies that UNBOUND belongs to the next task, not the
current implementation. Finish S4 before admitting planned S5 native GUI work.
S3 remains the delivered baseline `f9fe709aa`; T remains open. Frontend rename
and final audit remain planned S6/S7; no queued T is admitted.

| Field | Admitted record |
| --- | --- |
| Identifier Mode | M0 T424 S4, Ordinary Mode; centralize project-added launch/control in NTSRV. |
| Candidate Proposal | [Admitted naming package with owner-added investigation](../proposals/proposal-native-worker-frontend-renaming-001.md). |
| Admission And Approval | Existing broker-only launcher control and authenticated Console coordination approval remains binding. Latest owner clarification: finish S4; native GUI routing/registration belongs to S5; UNBOUND display belongs to the queue-head NTMON T candidate. Premature replanning admission is superseded. |
| Objective | run16 submits and waits on broker-owned direct results; NTSRV owns frontend/worker creation, authenticated binding and orderly retirement. Retain direct NTKVM/worker I/O and nonpolling broker-death waits. |
| Non-goals | No UNBOUND, native GUI routing/classification change, frontend rename, mirror/guest/media/shared-lib change, helper, Job observation, scheduler, process-tree kill, launch-syntax change, I/O relay through NTSRV or NTSRV Console attachment. Preserve GUI/Win16 startup-only and explicit --wait semantics. |
| Reference Baseline | S3 `f9fe709aa`, protocol/RPC 29, coherent published eight-file package and retained Console17/Window17/WOW frontiers. S3 delivery does not imply this new architecture already exists. |
| Files And ABI Surface | run16 startup/receipt clients; NTSRV project-owned creation, reservation, binding and result transport; NTKVM authenticated Console bootstrap; NTVDM/NTW32 adapters and worker-base; interface and coherent protocol/RPC revision; tests and current design. |
| Applicable Rules | README reading set, EXECUTION, architecture/coding/document rules and source policy; preserve other-session edits. |
| Verification | x86/MIDL affected closure; exact-parent creation and edge checks; authenticated bootstrap and wrong-capability negatives; DOS/native direct result and failure, final I/O restoration, reuse/nesting/root isolation/broker loss; Console17/Window17 and retained WOW frontiers; coherent eight-file publication; governance/link/diff and no-mirror-change checks. |
| Expected Markers | Service creates frontend/workers; text launcher has only service task and Console-handoff receipt/result waits, no NTKVM/worker IPC. Direct worker/frontend I/O does not complete records or decide orderly death. Exact generation/rights, rollback ownership and event-driven broker loss. GUI remains unchanged until S5. |
| Asset Needs | S3 runtime/recovery and retained S4 candidate/evidence. New artifacts only under build/M0-T424/S4; reuse recorded S2/r001 object cache without overwriting sealed evidence. No external acquisition. |
| Reporting Requirements | Maintain edge/source/ownership checklist, distinguish target from implementation, report removed direct paths, retained I/O edges, exact tests and open rows. |
| Stop Conditions | Mirror change, GUI/UNBOUND implementation, arbitrary remote creation/duplication, helper/scheduler, syntax change, weakened authentication or baseline regression requires disposition. Preserve other-session edits. |
| Exit Criteria | All S4 migration rows production-wired, obsolete paths removed, required tests and coherent publication verified, reviewed committed/pushed P. No claim of closure from planning or compilation alone. |
| Original Owner Request | Broker-centered architecture cleanup with all launcher/Console coordination through NTSRV; latest clarification: UNBOUND belongs to the next task and must not be implemented in the current S. |
| Similar-Issue Sweep | Newly created/resident DOS/native workers, inherited frontend scopes, native parent resume, borrowed/owned Console, unchanged GUI/Win16 startup-only, rollback, completion versus infrastructure failure, workerless-root and broker-empty deadlines. |

## S4 Implementation Progress

The owner's latest clarification supersedes the premature S5 admission and
S4 replanning conclusion. S4 remains active; no production P or functional
closure has been delivered. The following remain mandatory S4 release rows:
DOS/WOW broker-owned worker creation preserving Check/Update ordering;
broker-native submission/preflight/final I/O result and parent resume;
removal of direct launcher/worker pipes, process-result fallbacks and obsolete
bootstrap APIs/fixtures; full regression and coherent eight-file publication.
The candidate is preserved in the main worktree; do not revert or publish it
merely to create a clean closure. Detailed S4 findings remain evidence.

S4's initial source audit and ordered implementation checklist are recorded in
[broker-centered migration](../etc/evidence/m0-t424-s4-broker-centered-launch-control.md).
The retained S4 candidate is in the main worktree, protocol/RPC 30:
NTSRV creates NTKVM and the native text worker, coordinates Console return,
and owns cancellable ten-second workerless-root retirement. Actual process
parent/authentication, restoration, broker loss and grace/cancellation probes
pass, as does same-outer-CMD DOS/native/DOS re-entry. The launcher watcher
now starts at broker admission. Native worker rundown preserves existing
Win32Records on the authenticated launcher, signals failure receipts under
the service lock and supports one-time result queries; native completion
wait no longer infers failure from worker/root process handles. Focused
client/actual-process failure checks pass; these are not the full release gate.
DOS/WOW worker creation, native direct-channel replacement and parent resume,
obsolete fixture/API removal and full regression remain open. This is not a
delivered P or S4 functional closure. O:/winnt remains the accepted S3 package; all
candidate artifacts and run evidence remain in build/. See S4 evidence.
The delivered predecessor is [S3 lifecycle repair](../etc/evidence/m0-t424-s3-abnormal-exit-relaunch.md).

## S3 Closure Record

Delivered as `f9fe709aa`, pushed to main; exact verification/publication facts
are retained in [S3 evidence](../etc/evidence/m0-t424-s3-abnormal-exit-relaunch.md).

NTSRV now owns orderly frontend/worker retirement. Authenticated shutdown has
priority over pending/lease/I/O and native GetNext, including the initial
pre-presentation wait. Workerless roots retire without an idle prerequisite;
only a live startup owner grants bounded ten-second admission. Existing-worker
reuse does not renew that exemption. Original worker close handling remains
local; there is no guest/shared-library change or new process/scheduler.

Affected x86/MIDL/WOW links pass with protocol/RPC 29. Final Console17 and
Window17 each pass 17/17. Four actual DOS/native worker/frontend failure cases,
same-outer-CMD relaunch, modern EDIT return, and independent native sessions
pass. WINMINE retains its guest main window; SOL/WRITE retain their existing
out-of-memory frontiers, not usability passes. Optional legacy native-worker
fixture and earlier test-harness failures are explicitly retained as non-passes
in evidence; physical desktop/RDP observations remain unclaimed.

All eight O:/winnt hashes match the final tested runtime and published manifest;
previous package recovery is under build/M0-T424/S3/r001/published-recovery.
Guest media, SYSTEM.INI and NTVDM.REG are not overwritten. Z: is removed after
testing. T424 remains open. S3 did not implement broker-owned creation;
the subsequent owner implementation approval is admitted separately as S4.
Frontend renaming is now S6 and has not started.

Owner's subsequent implementation approval supersedes research-only scope:
NTKVM exits on NTSRV instruction or user Console closure (fault/broker-loss
remain failure exits); workers obey NTSRV shutdown, not frontend-death policy
of their own. An orphan root retires immediately after legitimate startup
admission ends, independently of idle/lease state. NTSRV's ten-second empty
grace begins only after all frontends/workers and legal admissions are gone.

## S2 Closure Record

Owner directs the project Win32 text worker to be named NTW32 and its native
record family Win32Record, across code/configuration/scripts/documentation,
while preserving genuine Console names. The
[S2 evidence](../etc/evidence/m0-t424-s2-native-worker-identity.md) records the
fourteen-file worker move, seventeen test/script moves, NTSRV record spelling,
all consumers and full-tree classification, exact name-only source review,
fresh x86/MIDL/WOW builds, focused identity/lifecycle tests and publication.
Original mirror/shared-library code and guest media are unchanged.

Console17 and Window17 each pass 17/17. Actual COMMAND -> CMD -> modern EDIT ->
CMD -> DOS MEM -> completion passes both staging and the actual O:/winnt
publication. Independent-session management and frontend-loss tests pass;
previous application identity and wrong protocol are rejected by the real
service. WINMINE's guest main window is retained; SOL/WRITE retain their known
out-of-memory frontiers, not usability passes. The supplemental eight-layer
chain ran normally and captured same-worker reuse, but its legacy validator
fails an obsolete EPOCH requirement; its whole-script result is not a pass.
Physical desktop/RDP observation and synthetic Window Ctrl+Q remain unclaimed.

All eight O:/winnt hashes match the verified staging package. The old worker
executable was backed up under build/M0-T424/S2/r001/published-recovery and
removed after verification. SYSTEM.INI, NTVDM.REG and guest media were not
overwritten. Temporary Z: mapping is removed. Frontend is still NTKVM.

## S1 Closure Record

Read-only naming audit delivered as `3c8e6b7d1`, pushed to main. The
[S1 evidence](../etc/evidence/m0-t424-s1-name-referent-audit.md) retains the
complete-tree inventory, original Console exclusions, ABI decision and exact
pre-migration baseline in its sealed inventory and S1 Git revision.

## Admitted T plan

The [T424 working plan](../etc/operations/t424-worker-frontend-renaming-plan.md)
and [proposal](../proposals/proposal-native-worker-frontend-renaming-001.md)
retain seven ordered stages. S1 audits names; S2 delivers the native worker
as NTW32 and Win32Record while retaining NTKVM frontend. Added S3 delivers the
approved broker-owned abnormal-exit/re-launch repair before further renaming.
S4 centralizes creation, binding, Console handoff coordination and direct result
transport in NTSRV while keeping Console operations local to NTKVM and
NTKVM/worker I/O direct; finish its open rows before S5. Planned S5 adds broker-routed
native GUI startup, NTW32 classification and service-held handles, not monitor
display. UNBOUND display belongs to the queue-head NTMON candidate. S6 renames
NTKVM to the reserved NTCON frontend identity after S5.
S7 owns final referent and
semantic audit. T closure
remains owner-controlled. No later queued candidate is admitted here.

## Current Technical Baseline

Published S3 package at O:/winnt: run16.exe, ntsrv.exe, ntvdm.exe, ntw32.exe,
ntkvm.exe, ntmon.exe, WOW32.DLL, VDMREDIR.DLL. APP_VERSION is 0.0.424;
protocol/RPC is 29 with unchanged UUID and a typed authenticated worker shutdown
event RPC. MSVC Win32/x86 /MT CCPU40 is unchanged. Exact hashes and commands are in S3
evidence; predecessor accepted T423 S40 `f64559086` is recoverable.

## Previous T closure

[T423 closure](../history/m0-t423-console-window-runtime-closure.md) records
the owner's 2026-10-02 acceptance and preserves prior status/evidence and
explicit limitations. T423 is closed. T422 remains owner-closed.

## Recent M0 Closures

| Task | Outcome and retained evidence |
| --- | --- |
| T423 | Owner-accepted on 2026-10-02, S40 baseline `f64559086`; [closure and prior status](../history/m0-t423-console-window-runtime-closure.md). |
| T422 | Initial WOW32 and planning milestone; [closure](../history/m0-t422-initial-wow32-closure.md). |

## Recent Governance

The migration's zero gate is zero old project-worker and record referents,
not corruption of original Console APIs/source citations or reserved future
frontend identity. Full-tree audits retain all raw hits and classification.
Approved other-session launch-hook/trace proposals and Queue planning changes
are included without changing this pure naming scope. They remain proposals,
not implemented capability. Final governance, relative-link, diff and
eight-file publication review passed for the S2 P delivery.
