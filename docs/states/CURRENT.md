# Project Status

## Current Work

**No active M/T/S packet.** M0 T423 remains open. S13 was delivered in P1
`307c4a1b5`; S14 is not admitted.

## Last Closed Packet

| Field | S13 closure brief |
| --- | --- |
| Identifier Mode | M0 T423 S13, Ordinary Mode; closed delivery record. |
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
| Exit Criteria | Met by formal x86, real backend tests, coherent eight-file publication, governance verification and pushed P1 `307c4a1b5`; T423 still awaits owner acceptance. |
| Original Owner Request | Admit new S13 for approved text-size handoff repair; shift former S13. NTKVM stores authoritative geometry; native changes propagate; unsupported DOS extent restores last valid DOS mode. |
| Similar-Issue Sweep | Native root initialization, nested launch/return, buffer replacement, scrolling, display switching, font geometry, viewport offsets and mouse mapping. |

### Current investigation

Final gate update: r70 temporarily published the fully tested candidate;
published Console r71 and Window r75 each passed17/17, WOW r76 preserved all
three frontiers, and DOS/native mouse checks r77/r78 passed. However r79 real
22-row geometry handoff returned1237 from a subsequent native child. S13 stays
open. The complete eight-file runtime was restored to the S12 hashes from
publication-backup-r70. Candidate r81 passed the five-mode matrix but does not
erase the intermittent failure. The later protocol19 candidate holds the
NTKVM frontend lock across NTCON's full multi-tile snapshot; EOF and invalid
operations release it. Candidate r94/r97-r105 pass the lock, Console/Window,
mouse, WOW, five-mode geometry and DDWWDDWW gates. Coherent r106 publication
replaced the eight-file package with backup/manifest. Published r107-r112 pass
the previously failing 22-row case, all five modes, Console/Window17/17 each,
DOS/native mouse and three WOW baseline frontiers. All eight published hashes
match formal output. r79 remains recorded as a real historical failure; its
exact emitting stage was not traced then, and is not asserted retroactively.

Earlier r51-r67: worker-owned reverse-video mouse block is implemented and
tested without TEXT frame ABI/shared-library changes. The input channel is now
version19 for the screen snapshot transaction and worker-owned pointer events.
Five real DOS video modes22/25/28/43/50 pass three native/DOS/native cycles each;
the BIOS probe uses the original INT10 query activation instead of mistaking
dormant stream-mode BIOS data for an applied VGA mode. Original mirrors are
unchanged. Two independent native sessions retain dimensions/history, and the
DDWWDDWW Window chain passes. Styled80x50/font16 uses existing library glyph
slots instead of an oversized DIB. Console/Window r56/r57 each pass17/17;
WOW r63 retains the three distinct frontiers. At that stage, final relink,
whole-set publication, governance and commit/push remained delivery gates;
the newer r106-r112 evidence above supersedes that publication status.

Published-package isolated-desktop probes r135-r137 used no Computer Use.
Both DOS -> CMD -> DOS and CMD -> DOS -> CMD completed with scripted input.
r137 retained one HWND: DOS text 80x22/font16 with client 411x227, native text
53x14/font16 with client 411x218, then DOS 80x22. Native Console buffer was
80x22 but its viewport was 0,0,52,13. NTCON packs srWindow; NTVDM packs guest
logical extents. This proves the physical-viewport leak, not a passing repair.
Raw evidence is O:/winnt/Logs2/t423-window-geometry-r135.txt through r137.txt,
including .geometry.txt and per-line snapshots. r133 failed before input-ready
and is not a passing geometry run; r134 passed the standard round trip.

S13 research r1-r3 adds tests/observation/console_logical_geometry_probe.c,
built /MT x86 under build/M0-T423/S13/console-geometry-r1 through r3.
The probe self-launches on a private desktop; it never switches the desktop,
changes registry/guest/product files, or queries host display dimensions to
choose geometry. Runtime reports are O:/winnt/Logs2/t423-s13-console-geometry-r1.txt
through r3.txt. Default 7x16 hidden font rejected viewport 80x25 with error 87;
1x1 and requested 2x2 (actual 1x2) triggered buffer minimum-size failures.
Fixed actual 2x4 accepted and read back exact buffer/viewport 80x25, 80x50 and
120x40. This is one-host API feasibility evidence, not a production solution
or universal size guarantee. Next: prove independent logical geometry and
application-originated resize observation, then real backend application and
original DOS return conversion. These experiments preceded the production candidate.

The uncommitted candidate now separates logical and physical viewport state,
applies/readbacks real NTCON geometry, and prepares supported DOS geometry for
the original return path. Targeted capture/conversion tests pass. Real private
desktop r18 DOS -> CMD -> DOS retained one HWND, 80x25/font16 and client
304x190 across 534 probe samples. r20 reverse CMD -> DOS -> CMD also preserved
80x25/font16. Repeated native history reseed tests pass (352 presentation checks).
r21 exposed MODE clipping through inherited hidden font metrics; fixed carrier
initialization corrected r22 to full 120x40, then DOS fallback and native return
at 80x25. Broader supported resize, live scrolling, failure/acknowledgment and
full regression remain open. r25 traced resize-time capture failure; r31
proved the changed-geometry retry and full 43-row DOS/font8 handoff, including
200-column host independence and removal of the stale25 frame. Formal race,
frame and lifecycle fixtures pass. Full Console r33 caught initial DOS
scrollback truncation; repaired nested-mem r34 passes unchanged assertions.
Complete Console/Window suites r35/r36 pass 17/17 each; r37 retains frame
anti-replay serial checks. WOW r38 preserves the three previous frontiers.
Earlier r39 exposed inactive-union native arrow reads; r40's temporary
conversion still failed80x50 at the graphics768-line limit. Both approaches
are superseded by the approved NTCON text-cell pointer below, which passes
80x50 without a graphics frame or library change.
The earlier800-line library-capacity request is withdrawn: the owner has
approved moving native pointer ownership into NTCON, rather than extending
NTKVM's native-only composition. r41 logical mouse geometry and existing
failure/reset assertions pass. Owner selected the reverse-video text-cell
pointer, keeping the text ABI and library unchanged. NTCON now owns the logical
pointer, copied-frame composition and native input translation; NTKVM's native
position/arrow implementation is removed. Candidate input protocol18 carries
relative pointer/modifier records without frontend-selected geometry. r42 has
86 passing packing/pointer checks; r45 has378 passing real-Console/pipe checks.
Full Console/Window regression r46/r47 each passes17/17. Real native80x50
Window input r48 passes movement/press/release through NTCON, with sink receipt
and output marker. WOW r49 preserves all three baseline frontiers. Real r50
native50 -> DOS50 -> native120x40 -> restored DOS50 passes, including actual
DOS font8 frames and output markers. No S closure is claimed.
No candidate has
replaced O:/winnt. Details and failed attempts are
indexed in the [S13 evidence ledger](../etc/evidence/m0-t423-s13-text-geometry.md).

Audit actual original DOS mode support and Console resize observability first.
NTKVM owns acknowledged logical geometry, separate from physical viewport and
scrollback. Workers apply backend state before acknowledging/resuming input.
DOS-compatible dimensions pass through original mode paths; otherwise restore
the last valid DOS mode, or original startup default if none. Conversion does
not reflow: left-align columns, select a contiguous cursor-visible row interval,
pad growth and clamp cursor; preserve native scrollback. No arbitrary DOS mode
or metadata-only success is allowed. The approved detailed contract is in the
linked proposal.

## Latest Delivery

S13 P1 `307c4a1b5` is pushed. NTKVM now carries acknowledged logical text
geometry independent of physical viewport; NTCON applies native geometry and
returns supported DOS geometry through the original path. NTCON owns its
reverse-video text mouse pointer. Project-owned screen publication and the
multi-RPC native snapshot use one frontend I/O lock: first acquirer proceeds,
the other waits, and end/EOF/protocol failure releases it. Native programs
writing their own hidden Console cannot acquire that lock, so capture validates
its before/after geometry and uses only bounded retry on a changed snapshot.
The full verified eight-file set is published at O:/winnt. Published r107-r112
and the lock/channel tests passed; the earlier r79 failure remains in the
[S13 evidence ledger](../etc/evidence/m0-t423-s13-text-geometry.md), not erased
from history. This closes S13 only, not T423. S14 awaits separate admission.

## Previous Delivery

M0 T423 S12 implementation and verification are complete. Its P2 delivery
contains the independent NTCON worker, common worker mechanisms, production
integration, obsolete-backend removal and the accumulated reviewed tests.
The coherent S12 eight-file package was the baseline during S13; do not close
T423 automatically.

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

## S13 Closure Record

Delivered geometry handoff, cross-process screen snapshot locking and native
worker-owned mouse pointer in pushed P1 `307c4a1b5`; full eight-file package
published and checked after publication. [Evidence](../etc/evidence/m0-t423-s13-text-geometry.md).

## Recent M0 Closures

S13 is the latest implementation closure; owner side-test acceptance is pending.
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
