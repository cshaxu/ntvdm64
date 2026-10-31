# Project Status

## Current Work

**Active: M0 T473 S11**

The CPU40 protected-instruction monitor and cached-TLB boundary are now in a
tested partial ten-component baseline. Full (16 gates), 93 CCPU cases,
Winmine/SOL/WRITE, four SOL Options cycles and four Win3.1 modes passed; both
release destinations matched all ten image hashes. V86 dispatch corrected; DPMI mapped-code, operand-fault,
reentry and NPX/debug behavior remain S11 work; publication is
not S11 closure. [Evidence](../../artifacts/mvdm-audit/s11-foundation-evidence/code32-prebuilt-search/review.md).

S10 closed as a source-role audit at its [closure](../etc/evidence/m0-t473-s10-closure.md); this does not assert functional recovery. The owner approved automatic progression and the [stage plan](../etc/operations/m0-t473-whole-recovery-stage-plan.md). S11 recovers execution foundations and includes the owner-approved [monitor audit](../../artifacts/mvdm-audit/s11-monitor-minimization.md). Preserve unrelated work and immutable guest media.

The published foundation group includes far-transfer ownership and WOW
entry ABI; NIDDB internal fixes remain outside scope. The selected bridge
passed controlled loader cycles; forced WOW32 attach failure now returns correctly. [Evidence](../../artifacts/mvdm-audit/s11-foundation-candidates.md).

The owner moved further SOL and related WOW multi-window performance work to
the [queued candidate](../proposals/proposal-wow-multiwindow-performance-recovery-001.md).
Retain independently justified changes and evidence; do not treat latency as
an S11 closure gate or claim the performance issue fixed.

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
Window-17 timeout remains unlocated. [Win386 mouse](../../artifacts/mvdm-audit/s11-win386-mouse-freeze-investigation.md): V86 fix tested; physical input unverified. S11 remains active; the
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

### M0 T473 S11 — execution, identity and lifetime foundations

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T473 S11, Ordinary Mode. |
| Admission And Approval | Automatic progression authorized by the owner; [S10 closure](../etc/evidence/m0-t473-s10-closure.md) and approved stage plan. |
| Objective | Restore and verify the S11 execution, callback, identity and lifetime owner groups without increasing unnecessary mirror differences. |
| Non-goals | Guest modification or rebuilding; 486-specific repair; unrelated internal defects in connected components; speculative replacement; unrelated Win95 tooling. |
| Reference Baseline | S10 audit `822c5fe58`; [recovery worklist](../../artifacts/mvdm-audit/recovery-worklist.md); unpublished [NIDDB candidate](../../artifacts/mvdm-audit/host-ch-scope/niddb-allocation-rollback-review.md). |
| Files And ABI Surface | MVDM CPU, SAS, VxD lifecycle and project-owned callback/identity adapters as justified by source trace; exact changed paths recorded in evidence. |
| Applicable Rules | Project source policy, architecture and execution rules; original logic first, minimum mirror diff, registered exceptions and immutable guest. |
| Verification | Cached coherent builds, focused normal/failure/retirement tests, relevant immutable-guest integration and required Full gates; bind each result to exact built package. |
| Expected Markers | Reproducible witnesses and disposition for each admitted owner group; performance optimization is queued, while correctness failures remain in scope. |
| Asset Needs | Original sources, captured build graphs, isolated package, probes and immutable guest inputs; no guest rebuild. |
| Reporting Requirements | Each change, original owner, mirror diff, evidence, failures and finite S15/S16 handoff; preserve unresolved external Win95 tooling. |
| Stop Conditions | Guest change, inaccessible original source, private host-layout assumptions or an unapproved observable semantic concession. |
| Exit Criteria | S11 foundation groups reconciled and verified with coherent publication and required reviews; any allowed boundary recorded, not inferred from compilation. |
| Original Owner Request | Reconnect feasible original MVDM capabilities, audit/repair in dependency order, then complete product-wide validation and delivery. |
| Similar-Issue Sweep | Examine adjacent allocation, rollback, reentry, handle identity, callback and lifecycle owners as one group. |
| Candidate Proposal | [MVDM whole-closure recovery](../proposals/proposal-mvdm-whole-closure-recovery-001.md). |
| Temporary Evidence | `build/t473-s11`, 60-minute phase, 3-GiB size and 10-minute no-progress budget; durable evidence under `artifacts/mvdm-audit`. |

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

## S10 Current P — add-on ownership and Win3.1 Setup

Owner requires this sequential P before resuming the original S10 census.
Scope and pending gates are recorded in the
[add-on review](../../artifacts/mvdm-audit/addon-layout-review.md#current-p-setup-correction-and-closure-gates).
Complete add-on/tool cleanup, graphical Setup mouse investigation and
profile-only finalization. Prove pure-DOS DOSONLY=0/1 with Standard/386 Enhanced
startup. Future WOW31 conversion is excluded. [Video7 pointer evidence](../../artifacts/mvdm-audit/video7-pointer-review.md) records the published checkpoint and pending Raw Input/Setup completion.

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

T461 S2 may use only `build/t461-s2` for disposable Shell/Registry fixture and
build evidence, with a 45-minute phase, 2-GiB size and 10-minute no-progress
budget. Stable `.ninja` and `.ccache` remain reusable caches and are never task
evidence roots.

T461 S3 may use only `build/t461-s3` for disposable persistence/concurrency
evidence, with a 45-minute phase, 2-GiB size and 10-minute no-progress budget.
Stable `.ninja` and `.ccache` remain reusable caches and are never task evidence roots.

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

T456 S1 may use only `build/t456-s1` for disposable evidence. Stable `.ninja`
and `.ccache` remain reusable caches and are never task evidence roots.

T456 S2 may use only `build/t456-s2` for disposable evidence. Stable `.ninja`
and `.ccache` remain reusable caches and are never task evidence roots.

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
