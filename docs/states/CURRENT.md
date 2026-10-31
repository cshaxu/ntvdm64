# Project Status

## Current Work

**Active: M0 T473 S12**

S11 closed its owner-narrowed non-MVDM compilation-condition audit and
cleanup; see [closure evidence](../etc/evidence/m0-t473-s11-condition-cleanup-closure.md).
Unfinished original foundation and later recovery work is transferred to the
first queued recovery proposal. S12 owns NTCON input-source handoff.

The CPU40 monitor and TLB boundary are in a
tested partial baseline. Full (16 gates), 93 CCPU cases,
Winmine/SOL/WRITE, four SOL Options cycles and four Win3.1 modes passed; both
release destinations matched all ten image hashes. CPU30 source removal published; CCPU V86 stays. Mouse unverified. Unfinished DPMI mapped-code, operand-fault,
reentry and NPX/debug work transfers to the queued recovery; earlier publication
does not prove that recovery complete. [Evidence](../../artifacts/mvdm-audit/s11-foundation-evidence/code32-prebuilt-search/review.md).

S10 closed as a source-role audit at its [closure](../etc/evidence/m0-t473-s10-closure.md); this does not assert functional recovery. The revised [stage plan](../etc/operations/m0-t473-whole-recovery-stage-plan.md) preserves the earlier [monitor audit](../../artifacts/mvdm-audit/s11-monitor-minimization.md) as handoff evidence. Preserve unrelated work and immutable guest media.

The published foundation group includes far-transfer ownership and WOW
entry ABI; NIDDB internal fixes remain outside scope. The selected bridge
passed controlled loader cycles; forced WOW32 attach failure now returns correctly. [Evidence](../../artifacts/mvdm-audit/s11-foundation-candidates.md).

The owner moved further SOL and related WOW multi-window performance work to
the [queued candidate](../proposals/proposal-wow-multiwindow-performance-recovery-001.md).
Retain independently justified changes and evidence; do not treat latency as
a condition-cleanup gate or claim the performance issue fixed.

The installed `O:/winnt` lacks 15 guest-manifest files, including WIN87EM.
The isolated single-file absence control prevents Calculator startup; adding
guest files to the installation was rejected by automatic review. [Evidence](../../artifacts/mvdm-audit/s11-foundation-evidence/win87em-install-gap/review.md).

Winmine 5023 has a focused idle-hook candidate. SOL also reproduced a
`publish-native-return` 1400 fail-fast on the baseline; its failing substage
remains unknown. See [evidence](../../artifacts/mvdm-audit/s11-foundation-evidence/atomic-task-retire/execution-check/baseline-failfast-trace/review.md)
and the [performance investigation](../../artifacts/mvdm-audit/s11-dialog-performance/investigation.md).

An independent prebuilt Win16 witness found that T460 narrowed the accepted
worker-shared idle query to the selected thread. Restoring the shared query
passes same-worker cross-task, separate-worker and common-dialog controls; the
published candidate also passed the Full suite.

Winmine probes previously changed a shared VirtualStore profile without an
original backup. The owner now explicitly permits restarting Winmine and
changing its configuration; the authorized Full test ran. See
[window evidence](../../artifacts/mvdm-audit/s11-dialog-performance/published-winmine-window.md).

Retirement preserves the first cleanup failure; focused probes and the current
Full gate passed. [Evidence](../../artifacts/mvdm-audit/s11-foundation-evidence/idle-comment-integrated/review.md). Intermittent 1400 attribution remains open.

Long-PATH WOW startup reproduces the accepted COMMAND guest EnvSiz overwrite. [Evidence](../../artifacts/mvdm-audit/s11-foundation-evidence/idle-comment-integrated/wow-env-attribution/review.md).

The current [ten-component release](../../artifacts/mvdm-audit/s11-swint-frame-recovery.md)
also restores DPMI software-interrupt frames and stack limits. Full, game, Win3.1 and
automated mouse witnesses passed; both destinations match. An earlier
Window-17 timeout remains unlocated. [Win386 mouse](../../artifacts/mvdm-audit/s11-win386-mouse-freeze-investigation.md): prior guard tested; physical input unverified. The prior
[closure audit](../../artifacts/mvdm-audit/s11-closure-audit.md) lists its
remaining boundaries.

Host C/H retention checks pass; runtime completion remains open. [Evidence](../../artifacts/mvdm-audit/s11-live-mirror-delta.md).

The published NTVDM export implementation selects the original CPU40 template and
records released ordinals. Its PE retains all 269 released name/ordinal pairs
and adds two memory exports; automated ordinal and selected alias/owner checks
pass. A real-worker VDD witness and the Full suite passed; broader provider
behavior remains under review. See [export review](../../artifacts/mvdm-audit/export-source-recovery-review.md).

The unpublished DIV-467/468/469/470/471 internal fixes are saved as held
[patches](../../artifacts/mvdm-audit/s11-held-cpu/) and removed from the active
runtime: the owner clarified boundary-recovery scope.

## Active Packet

### M0 T473 S12 — NTCON input-source handoff

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T473 S12, Ordinary Mode. |
| Admission And Approval | Owner directed automatic continuation after S11; [S11 closure](../etc/evidence/m0-t473-s11-condition-cleanup-closure.md). |
| Objective | Repair Console/Window input-source key lifecycle and investigate the Win3.1 MS-DOS Prompt Alt+Enter observation. |
| Non-goals | Guest modification or rebuilding, Computer Use, guest-specific shortcuts, unrelated original-code recovery. |
| Reference Baseline | [S12 handoff](../etc/operations/m0-t473-s12-ntcon-input-source-handoff.md); reproduce the documented failures before selecting a repair. |
| Files And ABI Surface | NTCON input/session and worker input boundaries; preserve input record identity, ordering and lifecycle. |
| Applicable Rules | Source policy, lifecycle ownership, immutable guest, minimal diff and event-driven execution. |
| Verification | Same-package baseline and fixed probes for DOS and native CUI, source changes, modifier/focus/rollback paths, Alt+Enter, Full gates and coherent ten-image delivery. |
| Expected Markers | Exact delivered-key ownership and symmetric source retirement, with positive, negative and failure evidence. |
| Asset Needs | Existing immutable guest, approved isolated probes and desktop-capable runner. |
| Reporting Requirements | Root cause, ownership, regressions, publication identity and remaining physical-input limits. |
| Stop Conditions | Guest changes, inaccessible original source, private host-layout assumptions or unapproved observable semantic concessions. |
| Exit Criteria | Handoff acceptance and required publication, commit and push; do not claim physical-input verification without evidence. |
| Original Owner Request | Finish S11 condition cleanup; admit S12 input-source repair; transfer other original recovery to the next queued task. |
| Similar-Issue Sweep | Review both source directions, both worker kinds, key identity, repeat, focus loss, failure and exit. |
| Candidate Proposal | [MVDM recovery proposal](../proposals/proposal-mvdm-whole-closure-recovery-001.md); S12 stays in the current task. |
| Temporary Evidence | `build/t473-s12`, 60-minute phase, 3-GiB size and 10-minute no-progress budget; archive required evidence before cleanup. |

## S11 Closure Record

The [S11 compilation-condition closure](../etc/evidence/m0-t473-s11-condition-cleanup-closure.md) records the selected non-MVDM cleanup, published package equivalence and deferred recovery boundaries.

## S10 Closure Record

The [S10 audit](../etc/evidence/m0-t473-s10-closure.md) records bounded source-role decisions and S11–S19 handoff.

## T471 Closure Record

T471 closes at [its closure](../history/m0-t471-closure.md): same-artifact
application/concurrency/abnormal evidence passed, while physical WRITE input
remains the explicitly approved non-pass. No runtime or guest bytes changed.

## T469 Closure Record

T469 COMM records: [S1](../history/m0-t469-s1-closure.md),
[S2](../etc/evidence/m0-t469-s2-comm-source-disposition.md),
[S3](../etc/evidence/m0-t469-s3-comm-retained-route.md),
[closure](../history/m0-t469-closure.md). No guest bytes changed.

## T463 S2 Closure Record

The [T463 S2 record](../etc/evidence/m0-t463-s2-sol-options-attribution.md)
retains the public HWND binding, SOL witness and Full gate. S3 owns remaining
task lifecycle acceptance.

The [T461 S3 record](../etc/evidence/m0-t461-s3-registry-persistence-candidate.md)
retains the durable-overlay delivery and its acceptance evidence.

T461 S1's unchanged inventory is retained in its
[owner matrix](../etc/evidence/m0-t461-s1-shell-registry-owner-matrix.md).

## T461 S2 Closure Record

The [S2 acceptance](../etc/evidence/m0-t461-s2-binding-implementation.md)
records the reviewed Registry bindings and product gate. S3 owns durable
commit, restart tombstones and cross-worker serialization.

## S1 Closure Record

The [T473 baseline](../etc/evidence/m0-t473-s1-whole-closure-baseline.md) records original source and composition inventory.

## S2 Closure Record

The [T473 CPU40/DPMI reconciliation](../etc/evidence/m0-t473-s2-cpu40-dpmi-reconciliation.md) and companion owner records retain the per-package decisions.

## S3 Closure Record

The [T473 CPU40 lifecycle](../etc/evidence/m0-t473-s3-cpu40-execution-lifecycle.md) records the execution and retirement boundary.

## S4 Closure Record

The [T473 DOS/COMMAND/XMS lifecycle](../etc/evidence/m0-t473-s4-dos-command-xms-lifecycle.md) records the selected original owner paths.

## S5 Closure Record

The [T473 SoftPC device lifecycle](../etc/evidence/m0-t473-s5-softpc-device-lifecycle.md) records the bounded device-owner audit.

## S7 Closure Record

S7 closed at [its session/callback ownership audit](../etc/evidence/m0-t473-s7-wow-session-callback-audit.md)
without a runtime change; S8 owns public-USER object identity and retirement.

## S8 Closure Record

S8 closes at its [public-USER evidence](../etc/evidence/m0-t473-s8-public-user-identity-recovery.md).
The ten-image package is published; the skipped direct completion/input
control observation remains a non-pass. S9 owns generic wrappers.

## S9 Closure Record

The [T473 USER wrapper evidence](../etc/evidence/m0-t473-s9-generic-user-wrapper-audit.md) records the mapped carrier family and delivered r10 package.

## S6 Closure Record

S6 was a no-runtime [status-sequence correction](../etc/evidence/m0-t473-s6-status-sequence-correction.md);
S7 owns the WOW32 session/callback audit.

## T459 Closure Record

W18 is closed in [the T459 closure](../history/m0-t459-closure.md). It
preserves original common-dialog and OLE alias authorities, passes real guest
modeless-dialog retirement plus exact alias lifecycle evidence, and records
guest OLE client/server as an explicit immutable stopping boundary. No product
image or guest input changed.

## Closed predecessor evidence

- [T457 closure](../history/m0-t457-closure.md): dialog/input/caret and hook/timer recovery; per-S evidence is indexed there.
- [T456 closure](../history/m0-t456-closure.md): font/text/metafile recovery and retained acceptance limits.
- [T455 closure](../history/m0-t455-closure.md): original GDI/DC/DIB/palette recovery and application observations.
- [T453 closure](../history/m0-t453-closure.md): files, environment and OEM recovery.

Detailed closed-stage chronology belongs to those retained records; the active
packet above remains the only current S authority.

## Current Technical Baseline

The [latest source delivery](../../artifacts/mvdm-audit/latest-source-release-review.md)
builds all ten components from committed production code. Full selected
product gates pass; both deployment targets match. Guest media is unchanged.

## Temporary Evidence

T459 S1--S4 used `build/t459-s1` through `build/t459-s4` only for disposable
evidence. Their retained results are indexed from the closure; stable `.ninja`
and `.ccache` remain reusable caches and are never task evidence roots.

T467 S3/S4 may use only `build/t467-s3` for disposable build, probe,
publication-recovery and acceptance evidence, with a 60-minute phase, 3-GiB
size and 10-minute no-progress budget. Stable `.ninja` and `.ccache` remain
reusable caches and are never task evidence roots.

T471 S1 may use only `build/t471-s1` for disposable package-freeze evidence,
with a 30-minute phase, 1-GiB size and 10-minute no-progress budget. Stable
`.ninja` and `.ccache` remain reusable caches and are never task evidence roots.

T471 S2/S3 may use only `build/t471-s2` for disposable acceptance evidence,
with a 60-minute phase, 3-GiB size and 10-minute no-progress budget. Stable
`.ninja` and `.ccache` remain reusable caches and are never task evidence roots.

T473 S1/S9 may use only `build/t473-s9` for disposable closure-ledger and
WOW-session evidence,
with a 60-minute phase, 3-GiB size and 10-minute no-progress budget. Stable
`.ninja` and `.ccache` remain reusable caches and are never task evidence roots.

T461 S1 may use only `build/t461-s1` for disposable source-audit evidence,
with a 30-minute phase, 2-GiB size and 10-minute no-progress budget. Stable
`.ninja` and `.ccache` remain reusable caches and are never task evidence roots.

T461 S2/S3 disposable evidence uses `build/t461-s2` and `build/t461-s3`;
each has a 45-minute phase, 2-GiB size and 10-minute no-progress budget.

T462 and T463 disposable evidence remains under their `build/t462-s1` and
`build/t463-s2`/`s3` roots; the shared caches are not evidence roots.

T457's closed-stage evidence is indexed in its [closure](../history/m0-t457-closure.md).

T454 may use only declared `build/t454-s1`, `build/t454-s2`, `build/t454-s3`
and `build/t454-s4` roots for disposable evidence; stable `.ninja` and
`.ccache` remain reusable caches. Retain durable results before exact-target
cleanup; never use a task root as a build cache.

`build/M0-T451`, `build/t451-s3`, `build/t451-s4` and `build/t451-s5` remain
temporary evidence, not reusable caches. Process inventory access was denied
during closure; deletion requires verified quiescence and the existing
exact-target ownership checks. Durable results are linked from the closure.
Stable `.ninja` and `.ccache` caches remain untouched.

T456 S1/S2 disposable evidence uses `build/t456-s1` and `build/t456-s2`.

## Recent M0 Closures

- [T460](../history/m0-t460-closure.md): WOW32 Clipboard selected-format and
  lifetime acceptance, project-owned PIC/keyboard repairs, and the bounded
  Win95 Start Menu PIF-template provenance correction.
- [T454](../history/m0-t454-closure.md): WOW32 resource discovery and loading.
- [T453](../history/m0-t453-closure.md): files, directories, environment and OEM.
- T452: modules, memory and aliases.
- [T451](../history/m0-t451-closure.md): USER objects/shared view and CUI handoff.
- [T450](../history/m0-t450-closure.md): style removal and Run16 entry.
- [T449](../history/m0-t449-closure.md): WOW32 execution continuation.
- [T448](../history/m0-t448-closure.md): lifecycle.

## Recent Governance

T460 closure records a current-package metadata comparison and a sealed
source-identical Clipboard witness. Its S4 acceptance changed no product image
or guest media; the later independent package/tool audit is `c1729f5a9`.
