# Project Status

## Current Work

## Owner Review Intermission

**No active M/T/S packet.** T423 remains open for owner acceptance. S34 is
delivered in [its evidence record](../etc/evidence/m0-t423-s34-console-projection-root-lifetime.md):
the complete eight-file protocol-26 package is published, while S35 task
tracing remains unadmitted.

## Last Closed S Brief

| Field | S34 brief |
| --- | --- |
| Identifier Mode | M0 T423 S34, Ordinary Mode. |
| Admission And Approval | Owner supplied the full `DIR` screenshot, approved complete logical-page projection and asked this side conversation to take over the related frontend/worker lifecycle work, publication and testing. |
| Objective | Publish a complete DOS logical page into the physical Console without stale border cells or cursor mismatch; make borrowed NTKVM roots follow the actual outer Console lifetime, with NTVDM/NTCON sharing root-loss semantics. |
| Non-goals | No guest or original execution-logic edit, fake prompt-line clear, task-trace work, Console-member task records or timer polling. Host scrollback history beyond the logical DOS page is not guaranteed. |
| Reference Baseline | Published S33 protocol-25 eight-file package and [S33 evidence](../etc/evidence/m0-t423-s33-first-command-screen.md). |
| Files And ABI Surface | NTKVM projection and authenticated frontend-root lease, NTSRV/root protocol 26, worker-base root handle, NTVDM/NTCON root-loss wiring, focused tests. Original MVDM image and guest media remain unchanged. |
| Applicable Rules | Execution, architecture, coding, documentation and immutable-guest source policy. |
| Verification | Capture full `DIR` cell/viewport/cursor transitions; focused tests, x86 build, 17 Console plus 17 Window routes, prior DOS/native/WOW frontiers and published smoke. |
| Expected Markers | Directory summary appears once, prompt line has no stale suffix, S33 screen preservation and COMMAND/MEM/EDIT interactions remain intact. |
| Asset Needs | Existing S33 cache and `O:/winnt` package; new build/test output under `build/M0-T423/S34/`. |
| Reporting Requirements | Exact cause, before/after evidence, changed files, regressions and published hashes. |
| Stop Conditions | A proved immutable original guest limitation or further display-contract expansion requires renewed owner review. The owner explicitly approved the full-page projection and its no-scrollback tradeoff. |
| Exit Criteria | Verified coherent eight-file package in `O:/winnt`, evidence, governance, commit/push and clean worktree; T423 remains open. |
| Original Owner Request | `cmd.exe` → `run16 command` → full `dir` leaves an unclean last line; owner approved fixing, publishing and testing. |
| Similar-Issue Sweep | First/repeated DIR, native→DOS 25/28/30-row geometry, cell/cursor retention, Console/Window and nested native return. |
| Candidate Proposal | [T423 Console/Window proposal](../proposals/proposal-kvm-window-graphics-presentation-001.md). |

S33's [evidence](../etc/evidence/m0-t423-s33-first-command-screen.md)
records the preceding baseline. The read-only task-trace proposal moves to S35
and is not admitted by S34 closure.

## Recent M0 Closures

S32 made NTKVM the sole sampler of visible Console identity and NTSRV the
authenticated owner of matching and resident-worker reuse for both NTVDM and
NTCON. The x86 build, focused service negatives, old-peer rejection, 17
Console plus 17 Window product cases, retained WOW frontiers, and published
smoke passed. A legacy no-presenter NTCON fixture and its obsolete
worker-must-retire assertion remain explicit non-passes, not product passes.
The prior eight-file package is preserved under the S32 build directory.

S31 removed the root frontend's startup geometry/cursor-position restoration
while preserving canonical-buffer, input-mode and cursor-shape cleanup. The
unchanged OpenNT MVDM mirror and guest media were not modified. The package
is published for owner side-testing; T423 is still open.

S30 restored original 22/25/28/43/50-row selection on native-to-DOS return,
removed the last-DOS-size fallback, synchronized MIDL revision 25.0 and
verified the published package. The original MVDM image, guest media and
configuration were not changed.

## Recent Governance

The T423 proposal and the S30/S31/S32 evidence ledgers retain their prior
scope, test results and non-passes. T423 does not close
without owner acceptance.

The closed S29 brief and its exact non-passes are retained in the
[S29 acceptance ledger](../etc/evidence/m0-t423-s29-control-plane-acceptance.md).

## S32 Closure Record

[S32 frontend identity evidence](../etc/evidence/m0-t423-s32-frontend-console-identity.md)
records the published package and retained non-passes.

## S31 Closure Record

[S31 Console handoff evidence](../etc/evidence/m0-t423-s31-shared-console-handoff.md)
records the retired startup-geometry restoration.

## S30 Closure Record

[S30 OpenNT geometry evidence](../etc/evidence/m0-t423-s30-opennt-geometry-protocol.md)
records row selection and protocol revision.

## S29 Closure Record

[S29 control-plane acceptance](../etc/evidence/m0-t423-s29-control-plane-acceptance.md)
retains the gate results and non-passes; T423 remains open.

## S28 Closure Record

S28 removed the redundant management `reserved` field and dead native Job
projection fields, corrected WOW depth into the sole `stack_depth` field, and
bumped the shared wire protocol to 25. BaseSrv/NTMON focused tests, all 17
Console plus 17 Window product cases, and the retained WINMINE/SOL/WRITE
frontiers passed. The coherent eight-file package is published to `O:/winnt`;
the source/owner ledger, hashes and non-passes are in the [S28 evidence](../etc/evidence/m0-t423-s28-management-projection.md).

S27/S26 owner and non-pass records remain in their indexed evidence ledgers;
S26's Job-observed participant projection was rejected from production.

## S27 Closure Record

[S27 worker-control audit](../etc/evidence/m0-t423-s27-worker-control-audit.md)
records the bounded shared-worker disposition.

## S26 Closure Record

[S26 Job observation disposition](../etc/evidence/m0-t423-s26-job-observation-disposition.md)
records rejection of observed ConRecords as product authority.

## S25 Closure Record

S25 repaired nested CMD deadlock with NTCON concurrent direct acceptance and
bounded, signalled startup/handoff waits. Its x86, 17+17, WOW, lifecycle and
fault gates passed; the protocol-24 package was published. The supplemental
final-screen-history assertion still has no passing baseline and remains an
S29 audit item, not a pass. Full evidence and hashes:
[S25 record](../etc/evidence/m0-t423-s25-native-nesting-wait.md).

## S24 Closure Record

S24 delivered the direct NTCON baseline after rolling back an initial package
without visible CMD I/O. The Job observer was excluded; S26 later rejected
its product admission. Bounded hidden-Console output sampling remains accepted.
The removed `--internal-console-probe` is not task authority. Full evidence:
[S24 record](../etc/evidence/m0-t423-s24-native-participant-graph.md).

## S23 Closure Record

NTSRV now admits native backend registration only from a prepared native
reservation, projects only authenticated worker watches, retains NTCON after
frontend route loss, and can explicitly close that resident worker after its
root is gone.  Focused reservation/rebind/close, empty monitor RPC and
frontend lifecycle fixtures pass with protocol-19 MIDL regeneration and x86
relink.  PID-accurate participant records are deliberately S24 work.  See
[S23 evidence](../etc/evidence/m0-t423-s23-unified-worker-lifecycle.md).

## S22 Closure Record

Modern EDIT emits only attributes 7/15 into the ordinary hidden Console because
its generic OSC 4/10/11 terminal queries receive no reply.  NTCON packing and
NTKVM palette rendering preserve those captured values; no renderer defect or
safe local production fix exists.  The 130-check packer fixture and probe
passed; no new executable is published.  See [S22 evidence](../etc/evidence/m0-t423-s22-native-edit-colour-audit.md).

## S21 Closure Record

| Field | S21 worker-base audit |
| --- | --- |
| Delivery | `9610f9e19`，已推送 `main`。 |
| Outcome | 全量来源审计确认：项目新增且语义相同的 broker 连接/回滚和有序 frontend 协议客户端已在 `worker-base` 中并同时链接进 NTVDM/NTCON；没有残余的安全同形候选。 |
| Retained ownership | 原始 DOS/WOW bootstrap、record 完成与 guest input 留在 NTVDM；真实 native Console、target completion、成员观察和 presentation 留在 NTCON。 |
| Verification | S20 的 590 节点 x86 正式包保持完全相同的生产源基线；重新运行 frontend scope、native backend reservation、empty monitor RPC，以及真实 COMMAND/MEM/EDIT/嵌套/原生路径和双会话 NTCON 管理隔离均通过；治理及 diff 检查通过。 |
| Publication | 无生产代码/ABI/构建图变更，故不伪造新发布；`O:/winnt` 继续为 S20 已验证的 protocol-18 七组件包。 |
| Evidence | [S21 worker-base audit](../etc/evidence/m0-t423-s21-worker-base-audit.md)。 |

## S20 Closure Record

| Field | S20 event-driven retirement |
| --- | --- |
| Delivery | Protocol 18 `FrontendStateChanged` capability, NTSRV mutation signalling and NTKVM direct wait-set; completion commit pending. |
| Outcome | Removed the project-added 100ms creator/`ERROR_BUSY` timer path. `ERROR_BUSY` retries only after a root-authenticated NTSRV state event; S17 restoration acknowledgement remains independent. |
| Verification | Focused lost-wake/root-authorisation fixtures; full 590-node x86 build; published COMMAND/MEM/EDIT/nested/native regressions; NTCON management isolation; isolated broker loss returns 1722; governance and diff checks. |
| Publication | Coherent seven-component protocol 18 package published to `O:/winnt`; exact hashes and observations are in [S20 evidence](../etc/evidence/m0-t423-s20-event-driven-retirement.md). |
| Non-work | No guest, shared-library, original DOS/WOW scheduling or execution-lifecycle change. |
| Evidence | [S20 event-driven retirement](../etc/evidence/m0-t423-s20-event-driven-retirement.md). |

## S19 Closure Record

Protocol 17 PID-first management is implemented, the full 573-node x86 graph
has completed, and the coherent seven-file package is published to
`O:/winnt`.  [S19 P1](../etc/evidence/m0-t423-s19-pid-worker-management-p1.md)
records the focused provider proof; [S19 P2](../etc/evidence/m0-t423-s19-p2-runtime-validation.md)
records the published hashes, real PID-close isolation and the complete
17-route text regression.  S20 now consumes this completed P2 baseline.

## S18 Closure Record

| Field | S18 native root Window exit to outer CMD recovery |
| --- | --- |
| Delivery | P1 pending this closure commit. |
| Outcome | Native root `run16 cmd` now uses the existing root `retire -> restored` barrier before it returns an outer CMD, matching the accepted DOS root path. |
| Cause | The native path waited for target/final presentation then returned; it had omitted the retirement signal and restoration acknowledgement that the DOS path already used. |
| Verification | x86 incremental build; frontend lifecycle fixture; isolated private-desktop ordinary Console and Window native routes both recorded `native-submit -> frontend-retire -> frontend-restored -> exit`; DOS Window control retained `task-completed -> native-resume -> frontend-retire -> frontend-restored -> exit`. |
| Owner Closure Decision | Owner explicitly authorized S18 closure. The incomplete whole-matrix rerun and two transient matrix observations remain retained evidence, not fabricated passes and not a blocker for this owner-directed closure. |
| Non-work | No guest/shared-library change, delay, redraw, injected recovery input, helper or changed execution-lifetime policy. |
| Evidence | [S18 native root restoration](../etc/evidence/m0-t423-s18-native-root-restoration.md). |

## S17 Closure Record

| Field | S17 Window exit to outer CMD input recovery |
| --- | --- |
| Delivery | P0 `4089ebaa2`, production P1 `0b1bc30f3`, both pushed to `main`. |
| Outcome | Root `run16` now waits for NTKVM's successful original-buffer/input-mode restoration acknowledgement before it returns an outer CMD to its Console. |
| Cause | The prior root completed its DOS record and returned while NTKVM could still be switching away from its temporary Window screen buffer. |
| Verification | Low-perturbation order witness; focused lifetime fixture; isolated ordinary Console and Window routes; full 17-row ordinary actual-output/exit matrix; x86 incremental build and coherent eight-file `O:/winnt` publication. |
| Non-work | No guest change, forced redraw, injected input or recovery sleep. The separate 100 ms `creator`/`ERROR_BUSY` poll needs a broker state-change protocol and is TODO debt, not a claimed S17 repair. |
| Evidence | [S17 recovery ledger](../etc/evidence/m0-t423-s17-window-exit-input-recovery.md). |

## S16 Closure Record

| Field | S16 zero-delay DOS/native input handoff |
| --- | --- |
| Identifier Mode | M0 T423 S16, Ordinary Mode; closed by owner direction. |
| Delivery | P1 `0e31c1505`, P2 `f348d049d`, production P3 `4c867f332`, all pushed to `main`. |
| Outcome | Corrected a Console transcript false negative and retired the standalone pending keyboard IRQ/PIC carrier before the original 8042 reset returns a user-owned key across DOS/native handoff. |
| Verification | Focused original-device fixture; six zero-delay repetitions each in Console and Window; 17/17 Console and 17/17 Window matrices; coherent eight-file `O:/winnt` publication; two more published zero-delay repetitions per route; preserved WINMINE/SOL/WRITE frontiers. |
| Limitations | The supplementary Window reverse nested script also times out on the untouched S15 baseline. It is recorded as unresolved observer/product evidence and is not counted as passed or as a DIV-321 regression. |
| Evidence | [S16 input handoff evidence](../etc/evidence/m0-t423-s16-typeahead-handoff.md). |

S15 production P1 `de720a74c` is published as a coherent eight-file package
to `O:/winnt`. Focused mouse tests, published 17/17 Console and 17/17 Window
matrices, and preserved WINMINE/SOL/WRITE frontiers are recorded in
[S15 evidence](../etc/evidence/m0-t423-s15-mouse-ownership-position.md).
Physical RDP observation remains owner-waived, not passed. A repeat of the
supplemental `dos-native-typeahead` test passed once and timed out once with
`eexit`; the owner explicitly assigned this separate instability to S16 and
directed S15 closure without treating that test as passing.

S14 candidate and coherent eight-file `O:/winnt` publication passed the
focused modern Edit VT mouse-click fixture, Ctrl+Alt+M capture-release fixture,
17/17 Console and 17/17 Window routes, private-desktop direct CMD/Notepad/
WINMINE retirement, and the three preserved WOW frontiers. The owner then
reported verification passed and explicitly directed S14 closure. Evidence:
[S14 product experience](../etc/evidence/m0-t423-s14-product-experience.md).

## S15 Closure Record

| Field | S15 closure brief |
| --- | --- |
| Identifier Mode | M0 T423 S15, Ordinary Mode; closed by owner direction. |
| Delivery | Production P1 `de720a74c`; documentation closure P2. |
| Outcome | Mouse capture ownership and native absolute-position conversion delivered in the published eight-file package. |
| Verification | Focused tests, 17/17 Console, 17/17 Window, three WOW frontiers; physical RDP interaction owner-waived. |
| Limitations | Supplemental zero-delay DOS/native typeahead is unstable and explicitly transferred to S16; not a pass or a mouse-repair regression claim. |
| Evidence | [S15 mouse ownership and position](../etc/evidence/m0-t423-s15-mouse-ownership-position.md). |

## S14 Closure Record

| Field | S14 closure brief |
| --- | --- |
| Evidence | [S14 product experience](../etc/evidence/m0-t423-s14-product-experience.md). |
| Identifier Mode | M0 T423 S14, Ordinary Mode; closed by owner acceptance. No new S admitted. |
| Summary | Closed; detailed scope, verification and acceptance remain in the linked S14 evidence. |

## Previous Closed Packet

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

### Retained S13 investigation

The completed S13 attempt chronology, including failures and final publication,
is retained in the [S13 evidence ledger](../etc/evidence/m0-t423-s13-text-geometry.md).

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

Historical S12 handoff and subsequent supersession are retained in the
[S12 ledger](../etc/evidence/m0-t423-s12-ntcon-backend.md) and
[T423 proposal](../proposals/proposal-kvm-window-graphics-presentation-001.md).

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

## T423 Ownership

T422 remains owner-closed. T423 remains open pending owner acceptance after S30.
