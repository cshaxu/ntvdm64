# Project Status

## Current Work

**Active: M0 T423 S13** — logical text geometry handoff repair.

## Active Packet

| Field | Active brief |
| --- | --- |
| Identifier Mode | M0 T423 S13, Ordinary Mode; one active S. |
| Candidate Proposal | [Console/Window proposal](../proposals/proposal-kvm-window-graphics-presentation-001.md) |
| Admission And Approval | Owner approval 2026-09-29: new S13 fixes text-size handoff; former product-experience S13 becomes S14, not admitted. |
| Objective | Preserve logical text dimensions across DOS/native launch and return; eliminate hidden-Console viewport clipping and unintended Window scale changes. |
| Non-goals | No guest/shared-lib modification, helper, ConPTY, new scheduler or S14 lifecycle expansion. |
| Reference Baseline | S12 P2 fbbbe4870 and published eight-file package; read-only r135-r137 geometry probes. |
| Files And ABI Surface | NTKVM state/channel; NTCON Console state/presentation/frame packing; NTVDM original video handoff bindings; interface/worker-base only for needed common copied contracts; tests and evidence. |
| Applicable Rules | docs/README.md task-reading authorities; source-first mirror policy; immutable guest; x86 CCPU40; every-production-P publication gate. |
| Verification | Original mode-path audit; unit and real private-desktop tests of both directions, explicit native resize, DOS fallback, scrolling/cursor/edge markers, failed acknowledgment and isolation; complete DOS/WOW frontier regression. |
| Expected Markers | Unchanged handoff preserves full frame extent; right/bottom edges retained; deliberate native resize propagates; unsupported DOS extent converts and is acknowledged before input resumes. |
| Asset Needs | Existing media and x86 caches; new artifacts under build/M0-T423/S13; runtime logs under O:/winnt/Logs2. |
| Reporting Requirements | Actual supported DOS modes, geometry provenance, conversion results, tests, deployed hashes and unpassed cases. |
| Stop Conditions | Need for guest/shared-lib mutation, API interception, new helper or violation of original execution semantics requires owner decision; ordinary failures remain repair work. |
| Exit Criteria | Real backend application and tests, not frame-header-only agreement; x86 regression, coherent eight-file publication, governance review, commit/push; T423 still awaits owner acceptance. |
| Original Owner Request | Admit new S13 for approved text-size handoff repair; shift former S13. NTKVM stores authoritative geometry; native changes propagate; unsupported DOS extent restores last valid DOS mode. |
| Similar-Issue Sweep | Native root initialization, nested launch/return, buffer replacement, scrolling, display switching, font geometry, viewport offsets and mouse mapping. |

### Current investigation

Published-package isolated-desktop probes r135-r137 used no Computer Use.
Both DOS -> CMD -> DOS and CMD -> DOS -> CMD completed with scripted input.
r137 retained one HWND: DOS text 80x22/font16 with client 411x227, native text
53x14/font16 with client 411x218, then DOS 80x22. Native Console buffer was
80x22 but its viewport was 0,0,52,13. NTCON packs srWindow; NTVDM packs guest
logical extents. This proves the physical-viewport leak, not a passing repair.
Raw evidence is O:/winnt/Logs2/t423-window-geometry-r135.txt through r137.txt,
including .geometry.txt and per-line snapshots. r133 failed before input-ready
and is not a passing geometry run; r134 passed the standard round trip.

Audit actual original DOS mode support and Console resize observability first.
NTKVM owns acknowledged logical geometry, separate from physical viewport and
scrollback. Workers apply backend state before acknowledging/resuming input.
DOS-compatible dimensions pass through original mode paths; otherwise restore
the last valid DOS mode, or original startup default if none. Conversion does
not reflow: left-align columns, select a contiguous cursor-visible row interval,
pad growth and clamp cursor; preserve native scrollback. No arbitrary DOS mode
or metadata-only success is allowed. The approved detailed contract is in the
linked proposal.

## Previous Delivery

M0 T423 S12 implementation and verification are complete. Its P2 delivery
contains the independent NTCON worker, common worker mechanisms, production
integration, obsolete-backend removal and the accumulated reviewed tests.
The coherent eight-file package is published at O:/winnt and post-publication
checks pass. It remains the usable baseline during S13; do not close T423 automatically.

T423 remains open. Product-experience/component-lifetime work is now S14,
not admitted. It is not the cancelled RDP packet.

Authorities: [T423 proposal](../proposals/proposal-kvm-window-graphics-presentation-001.md),
[S12 source/test ledger](../etc/evidence/m0-t423-s12-ntcon-backend.md), and
[Queue](QUEUE.md). The detailed S12 chronology, failed attempts and owner
boundary changes remain in the ledger; they are not current open gates.

## Current Technical Baseline

- MSVC Win32/x86 /MT CCPU40; no guest or shared-library modifications.
- Runtime: run16.exe, ntsrv.exe, ntvdm.exe, ntkvm.exe, ntcon.exe, ntmon.exe,
  WOW32.DLL and VDMREDIR.DLL. Application 0.0.423; service protocol 16,
  copied Console protocol 17, native request protocol 4.
- NTVDM owns original DOS/WOW execution. NTCON owns native text execution
  and its ordinary hidden Console, without ConPTY or a private helper.
  NTSRV handles authenticated registration and management; NTKVM owns visible
  Console/Window and the common frame renderer. run16 waits for direct results.
- worker-base owns matching project-added worker connection/client mechanisms:
  ordered transfer, validation/cancellation, frame chunks, input codec,
  activation/key return and client event lifecycle. Original mirror execution,
  scheduling, task completion, blocking/resume and cleanup remain in place.
- Native actual Console members are independent of worker residency. Returning
  to DOS does not destroy NTCON; direct target completion does not kill its
  surviving descendants. Parent output waits for the final presentation fence.
- Cross-component declarations are under interface. The displaced frontend
  executor, ConPTY parser/carrier and duplicate native renderer are removed.

Formal caches remain under build/M0-T423/S1/restart-formal-x86 and
restart-wow-x86; selected source/build inputs, provenance and run evidence are
recorded by S12. Candidate tests use build/M0-T423/S12/p. Formal publication
backup and exact old/new SHA-256 manifest are under
build/M0-T423/S12/publication-backup-r131. O:/winnt is the usable package,
not the build directory. Existing original guest/configuration hashes were
checked unchanged before publication; NTVDM.REG/user state was not replaced.

## S12 Closure Evidence

| Requirement | Verified evidence |
| --- | --- |
| Native execution, registration/reuse, version/auth and failure cleanup | Actual NTCON RPC/public-launch tests; concurrent creation, forged/stale context, stream/EOF, direct results and failed export/launch cases in S12 ledger. |
| Common mechanisms and owner boundaries | r106-r120 provenance audit, both production worker links, strict frontend leakage negative controls; transport fixture 336/0 and execution lifecycle 333/0 with zero remaining handles. |
| DOS/native I/O, completion barrier, key return | Real production round trips, copied input FIFO/negative tests, real unread Console return, final-ack failure/EOF and resume failure fixtures; original DOS block/resume remains the caller. |
| Nesting and isolation | r130 DDWWDDWW: sixteen input/output checkpoints, same worker identities and restored original DOS depths/tasks. Both twelve-target chains pass separate frontend groups and final retirement. |
| Members, management and failures | Surviving attached client in both display modes; expanded lifecycle faults and two-session management/frontend/worker-loss tests; unrelated session survives and direct worker failure returns 1067. |
| Published DOS regression | r131 Console17 and Window17 each 17/17, guest text and interaction checked. r132 CMD return and surviving-client checks pass in both modes. |
| Mouse | r128 candidate and r132 published burst/retire/latency: 1000/1000/200 records, guest PASS and input-sink acknowledgment. Physical desktop focus/clipping remains owner-waived, not passed. |
| WOW non-regression | r127 candidate and r132 published: WINMINE main window; SOL and WRITE original OOM frontiers. Separate headless observations, not full SOL/WRITE acceptance or interactive play. |
| Publication | All eight published hashes match the tested formal candidate; final process query empty. Old coherent seven-file set retained for recovery. |

Known immutable-guest limitation: sufficiently large inherited environments can
overwrite COMMAND's discarded INIT references. r126 matches the original
S35 binary/source defect, including with DOS=HIGH. It is registered in
[TODO](TODO.md), not fixed or counted as a passing capability. No environment
truncation, guest patch or allocator workaround is introduced. Historical r70
lacks the same memory witness and is not independently attributed by resemblance.

## S1 Closure Record

Delivered lifecycle baseline; [evidence](../etc/evidence/m0-t423-s1-restart-lifecycle.md).

## S2 Closure Record

Delivered copied I/O boundary; [evidence](../etc/evidence/m0-t423-s2-console-boundary-ledger.md).

## S3 Closure Record

Preserved and replanned, not functional closure; [evidence](../etc/evidence/m0-t423-s3-hidden-console-ledger.md).

## S4 Closure Record

Delivered independent frontend; [evidence](../etc/evidence/m0-t423-s3-hidden-console-ledger.md).

## S5 Closure Record

Delivered hidden backend baseline; [evidence](../etc/evidence/m0-t423-s5-hidden-backend-acceptance.md).

## S6 Closure Record

Delivered Console/Window display; [evidence](../etc/evidence/m0-t423-s6-window-display.md).

## S7 Closure Record

Delivered Window mouse and product renames; [evidence](../etc/evidence/m0-t423-s7-window-mouse.md).

## S8 Closure Record

Delivered GUI launch/wait policy; [evidence](../etc/evidence/m0-t423-s8-gui-launch-wait.md).

## S9 Closure Record

Delivered former ConPTY baseline; horizontal wheel remained an approved limitation; [evidence](../etc/evidence/m0-t423-s9-conpty-migration.md).

## S10 Closure Record

Delivered component cleanup, not unresolved screen continuity; [evidence](../etc/evidence/m0-t423-s10-component-minimization-audit.md).

## S11 Closure Record

Owner accepted mouse delivery 965083eec; [evidence](../etc/evidence/m0-t423-s11-interaction-retirement.md).

## S12 Closure Record

Delivered NTCON/shared-worker package fbbbe4870; S13 addresses owner-reported geometry defect; [evidence](../etc/evidence/m0-t423-s12-ntcon-backend.md).

## Recent M0 Closures

S12 is the latest implementation closure; owner side-test acceptance is pending.
S11 965083eec remains the recoverable owner-accepted baseline;
[S11 evidence](../etc/evidence/m0-t423-s11-interaction-retirement.md).
Earlier T423 stage records are linked by the proposal and S12 ledger.
T422 remains owner-closed; T423 itself is not closed.

## Recent Governance

This delivery preserves and includes the owner's authorized side-chat Queue/WOW
proposal changes. That delivery did not admit queued candidates. The subsequent
owner request now admits geometry S13 and defers product experience to S14. No forced push,
guest/lib change, new scheduler, recursive kill or helper is part of S12.
Commit/push and repository synchronization are verified as the final delivery
step; source/test evidence cannot substitute for that check.
