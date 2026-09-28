# Project Status

## Current Work

S9 published ntkvm 27106497 retains one ConPTY across all native-text targets
and DOS intervals. No project helper or hidden-Console fallback remains in
the selected graph. DOS continues using the original worker I/O route.

Passing evidence includes Console17/Window17, both twelve-target chains,
both lifecycle matrices, shared output, NTMon, graphics/text return, DOS mouse,
native GUI16 and Win16 shared startup/wait/faults. Post-return DOS prompt
checks pass; ungated ahead-of-time input failures remain explicit. The owner
approved leaving delivered native input in ConPTY, not reclaiming it to DOS.

Resources pass 26 cases and terminal assertions pass 288 checks. Overall
component acceptance is still 14/15: horizontal wheel fails on this host.
The owner explicitly accepted its registration as a host limitation and
approved publication/commit/push with the strict failure retained in TODO.
Physical focus/clipping is owner-waived, not
physically tested. The coherent seven-file package is now published at
O:/winnt with a recoverable backup under build/M0-T423/S9/publication-backup-r1.

Published Console17, Window MEM/nested MEM/EDIT and all three WOW frontiers
pass. Final source/checklist review and governance checks pass. S9 P5
3b40345f8 is committed and pushed; S9 is closed with its explicit limitations.
Owner now directs S10 cleanup closure and commit/push first, then S11 repair
of mouse latency and frontend retirement. RDP pointer escape belongs to the
subsequent S12. This supersedes the initial S11 admission-and-wait direction.
T423 remains open. Detailed identities and superseded
status chronology are preserved in the
[S9 ledger](../etc/evidence/m0-t423-s9-conpty-migration.md).

## Active Packet

**Active: M0 T423 S11**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T423 S11, Ordinary Mode; admitted after pushed S10 f98825653. |
| Candidate Proposal | [Console/Window frontend](../proposals/proposal-kvm-window-graphics-presentation-001.md). |
| Admission And Approval | Latest owner direction: close and commit/push S10, then admit S11 for the two interaction/lifecycle repairs; reserve RDP capture for S12. Preserve all existing work and evidence. T423 must not close. |
| Objective | Repair Window DOS mouse latency and frontend retirement using actual ConPTY membership, preserving one reusable backend. Resolve inherited Window text-continuity failures without weakening assertions. |
| Non-goals | No guest/shared-library mutation, DOS/WOW scheduler, path-search repair, persistent/transient helper role, launcher/worker host Console I/O, permanent legacy backend fallback, full Unicode font library or automatic T closure. |
| Reference Baseline | Published S9 seven-file package, pushed audit 142c619fe, and preserved uncommitted S10 P4 candidate. Identify the owner's actual tested hashes on resume; do not conflate candidate and publication. The component audit ledger records S10 evidence and its open Window text-continuity gate. |
| Files And ABI Surface | Original event consumer and worker input adapter; ntkvm ConPTY membership, admission and teardown; affected tests and evidence. No new scheduler or frontend owner. |
| Applicable Rules | EXECUTION, source policy, original mirror/ABI, output hygiene and the owner-approved independent frontend boundary. |
| Verification | Review exact cleanup diff and source identities; x86 builds, affected tests, preserved Console17/twelve-chain/fault/NTMon/WOW evidence; documentation governance and diff checks. Window text-continuity failure and owner interaction reports remain failures, not passes, with S11/S12 receivers. |
| Expected Markers | Obsolete symbols absent, retained providers present, worker-only declarations/package binding owned by ntvdm, no NTMon USER32 import; mirror delta and authored-code counts explicit. |
| Asset Needs | Preserved S10 cache/candidate/evidence under build/M0-T423/S10, original source and published identities. Logs remain under authorized Logs2; no physical desktop manipulation. |
| Reporting Requirements | Separate owner-observed symptoms, supplied preliminary source findings, hypotheses and independently measured causes. Do not assert ConPTY deadlock or an RDP limitation without evidence. Retain S10 accounting and unresolved gates. |
| Stop Conditions | No guest/shared-library mutation, ownership/scheduler expansion, fabricated test passes or unverified publication. Regressions introduced by cleanup must be resolved before its delivery. |
| Exit Criteria | Both repairs proved with production-path tests, ordinary and retained-descendant retirement plus admission races, mouse ordering/latency and existing regression gates; coherent verified publication, governance, commit/push. T remains open; RDP capture belongs to S12. |
| Original Owner Request | First close S10 and commit/push; then admit S11 for EDIT mouse latency and genuine ConPTY-member retirement; RDP pointer issue goes to S12. |
| Similar-Issue Sweep | All audited obsolete frontend wrappers, production diagnostic dependencies, shared/private declaration consumers and actual PE dependencies; interaction repairs are assigned to the next stages. |

## S10 Closure Record

S10 f98825653 is committed and pushed. The [S10 ledger](../etc/evidence/m0-t423-s10-component-minimization-audit.md)
records component consolidation, net removal of 144 authored C/H lines and
the exact passing and failing tests. Owner-directed closure is bounded to
cleanup/source handoff, not product acceptance. The unpublished candidate
has two Window text-continuity failures assigned to S11. O:/winnt remains S9.
S11 now owns mouse latency and actual-use retirement; S12 owns RDP capture.

## S9 Closure Record

S9 P5 3b40345f8 is committed and pushed. The [S9 ledger](../etc/evidence/m0-t423-s9-conpty-migration.md)
records helper removal, the shared ConPTY/bitmap implementation, x86 build,
candidate and published-path regression, seven-file hashes and backup.
Horizontal wheel remains an owner-accepted host limitation with strict failed
tests preserved; physical desktop checks remain owner-waived, not passed.
S10 f98825653 is committed and pushed as bounded cleanup delivery; S11 and S12
own the subsequent reported defects. T423 remains open for owner acceptance.

## S7 Closure Record

Owner addition dated 2026-09-27: rename frontend.exe/frontend-exe to
ntkvm.exe/ntkvm-exe and basesrv.exe/basesrv-exe to ntsrv.exe/ntsrv-exe within
S7. run16.exe and ntvdm.exe keep their names. Preserve historical OpenNT
BaseSrv symbols and semantics, authenticated endpoint contracts, imported
library bytes and guest. The old-name seven-file mouse checkpoint has been
published and its formal-path tests retained; it is not renamed-package
acceptance. Build/startup/process-identity/test/package references have been
updated and the verified new-name package is now coherently published with a
recoverable old-name backup. S7 P1 99276d68d is committed and pushed to main;
closure review confirmed clean synchronization and all seven live hashes.

The [S7 ledger](../etc/evidence/m0-t423-s7-window-mouse.md) records real guest
mouse count, text/graphics painting, relative input/callback and retirement
passes; native Window mouse and retained Console mouse pass. Console/Window
DOS17, both twelve-target chains, both five-case fault matrices, graphics
return and separate inherited WOW frontiers pass on the candidate. The exact
new-name seven-file set is published at O:/winnt with recoverable old-name
backup and unchanged configuration. It passed component17, Console/Window
DOS17, both twelve-target chains, both five-case fault matrices, real DOS
mouse positive/negative cases, graphics return and inherited WOW frontiers.
Published Console17, Window MEM/nested MEM/EDIT and separate WOW frontiers
also passed. S7 is closed at its admitted mouse/naming scope. Shared libraries
and guest media are unchanged. Physical focus/capture remains owner-waived,
and SOL/WRITE retain their known OOM frontier, not usability acceptance.

## S8 Closure Record

Owner addition: rename the product executable monitor.exe to ntmon.exe in
this S8 delivery. Keep src/monitor-exe and the NTVDM Task Monitor UI title;
only the executable name and its live build/test/package references change.
The verified seven-file publication must contain ntmon.exe and retire the old
monitor.exe with a recoverable backup, never leave two product names active.

The [S8 ledger](../etc/evidence/m0-t423-s8-gui-launch-wait.md) retains detailed
source analysis, every failed attempt, exact hashes and reproducible evidence.
Implemented: default GUI asynchronous startup, explicit --wait, authenticated
WOW startup receipt distinct from task completion, original WOWEXEC no-op
acknowledgement and bounded unfiltered WOWEXEC pending-post binding. Protocol
9 is selected consistently; original DOS/WOW record policy remains unchanged.
The approved side-chat repair retains the ten-second empty-broker grace and
passes seven real RPC cases; it introduces no idle-worker eviction.

Candidate verification passed option19, native GUI16, CMD/batch/input-loop
Win16 callers, original COMMAND /c and persistent interactive GUI callers,
new/reused WOW startup faults, real loader failure/recovery, component17,
Console/Window DOS17, both twelve-target chains and both lifecycle matrices.
Interactive tests require actual MEM output; explicit waits also require no
MEM execution before target release. Five real guest mouse cases, a no-input
negative, Console keymouse and graphics return pass. Separate headless WOW
observations retain WINMINE and the inherited SOL/WRITE OOM frontiers, not
new SOL/WRITE usability. Physical desktop focus remains owner-waived.

Final source review is complete. The coherent tested seven-file S8 set is now
at O:/winnt, including ntmon.exe; guest/configuration and NTVDM.REG were
preserved. The complete previous package and before/after hash manifest are
under build/M0-T423/S8/pre-publication. Both formal graphs have no pending
work, and candidate/published hashes match. Published-path Console DOS17,
Window MEM/nested MEM/EDIT and the three separate WOW frontiers passed.
S8 P2 d253e55af is committed and pushed to main, including ntmon naming and
the approved side-chat ntsrv repair. P3 6ef96410d records delivery; clean
HEAD/origin identity and the requirement-by-requirement ledger support bounded
S8 closure. This does not close T423 or claim SOL/WRITE usability.

## Current Technical Baseline

S9's selected formal graph uses frontend-owned ConPTY and the shared DOS/native
bitmap renderer. The four imported KVM libraries and guest media are unchanged.
The owner's host_cpu.h endian correction and ntmon-exe source move are included
in the candidate scope. Retained release/resource probes are test callers,
not production target-completion policy.

O:/winnt received the coherent S9 seven-file package, including ntkvm 27106497,
with all hashes and publication regression recorded in the S9 ledger. S10's
audit has not changed or republished production binaries.

Owner added authored-architecture minimization, per-EXE/DLL dependency review
and opennt-abi/product-abi/product-package consolidation to S10. The
[component audit](../etc/evidence/m0-t423-s10-component-minimization-audit.md)
records recommended ownership, 132 lines of obsolete C candidates and the
original-header collision constraints. Owner now authorizes component
consolidation: move product-package into its worker owner, retain product-abi,
and migrate proved private compatibility declarations without changing ABI or
runtime behavior. Reconcile original-header collisions before any mirror move.
Owner also authorizes the audited obsolete-code cleanup, not just relocation.
S10 P4 cleanup f98825653 is committed and pushed as source/handoff delivery; no
candidate has been published. This bounded closure is not a production release.
The component audit ledger now records the candidate's consolidation and
144-line net authored C/H cleanup, x86 build, Console17, both twelve-target
chains, both five-case fault matrices, NTMon and preserved WOW frontiers.
Window native-zero fails strict text continuity under the current 120-column
conditions; the unchanged S9 package reproduces it too. Cause is not yet
proved. Do not weaken the check or publish the candidate while this gate is
open. S11 receives its investigation alongside the owner's exit report; S12
receives RDP pointer capture. The final Window matrix was 15 pass / 2 fail
(native-zero and missing); neither failure is counted as acceptance.
S10 is closed at its owner-directed bounded scope. S11 is now active.

## S11 Owner-Reported Issues

1. Window EDIT.COM has a moving block cursor but unusually slow mouse response.
   Owner supplied a preliminary comparison: nt_event.c around lines 526/642
   reads one event per iteration and sleeps 10 ms, with IPC overhead; the older
   worker-owned Window drained mouse events in one read. Queue accumulation and
   sole causality are not measured here. Reuse the backup as evidence, not as
   authority to restore worker-owned host UI or change original IRQ timing.
2. run16 COMMAND.COM, CAF into Window, start cmd.exe, exit CMD, then outer
   COMMAND cannot exit again. ConPTY blocking is an owner hypothesis, not a
   diagnosis. Distinguish newly typed post-return input from previously delivered
   native input, target completion from backend retention, and outer guest exit.
3. Under RDP, the host pointer can leave kvm-window after capture. Logical
   capture and physical clipping must be distinguished. Prior physical testing
   was waived, not passed; it does not dismiss this concrete user report.

Latest owner direction narrows S11 to issues 1 and 2, and assigns issue 3 to
S12. Preserve the side-chat findings as supplied evidence, not as independently
verified conclusions. S11 implementation is now admitted after S10 commit/push.

For issue 2, the required retirement predicate is: no pending admission, no
active DOS task, no unfinished direct native request, and no other processes
actually attached to this ConPTY. Close admission atomically before teardown.
ConPTY existence, direct-target exit and process ancestry are not member counts.
Keep the same ConPTY during CMD-to-DOS return. Removing the members guard alone
is prohibited. Verify ordinary final exit, retained attached descendants, and
automatic retirement after their actual departure, including admission races.

## Remaining admitted sequence

S9 ConPTY migration closed at pushed P5 3b40345f8. S10 bounded cleanup closed
at f98825653. S11 mouse/retirement repair is active. S12 owns RDP pointer
capture and is not yet active. T423 stays open.

Physical focus/clipping observation is owner-waived with source/unit evidence;
private-desktop integration remains required. Separate WINMINE, SOL and WRITE
headless frontiers remain mandatory; existing SOL/WRITE OOM is not usability.
Guest and imported library changes remain prohibited without distinct approval.

## S1 Closure Record

Delivered lifecycle baseline; [S1 evidence](../etc/evidence/m0-t423-s1-restart-lifecycle.md).

## S2 Closure Record

Delivered copied I/O boundary; [S2 ledger](../etc/evidence/m0-t423-s2-console-boundary-ledger.md).

## S3 Closure Record

Preserved and replanned, not functional closure; [S3 handoff](../etc/evidence/m0-t423-s3-hidden-console-ledger.md).

## S4 Closure Record

Delivered independent frontend; [S4 evidence](../etc/evidence/m0-t423-s3-hidden-console-ledger.md).

## S5 Closure Record

Delivered hidden backend; [S5 acceptance](../etc/evidence/m0-t423-s5-hidden-backend-acceptance.md).

## S6 Closure Record

Delivered Console/Window display; [S6 ledger](../etc/evidence/m0-t423-s6-window-display.md).

## Recent M0 Closures

T422 remains owner-closed. T423 S10 f98825653 is the last closed
subtask; S11 is admitted. Prior closure records
are preserved in the indexed status archive above.

## Recent Governance

One active S only. Preserve concurrent owner-approved queue/proposal changes
in the reviewed delivery. T422 stays owner-closed. The full objective and T423
remain open until all admitted stages and owner acceptance are complete.
