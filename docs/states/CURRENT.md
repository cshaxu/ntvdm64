# Project Status

## Current Work

**Active: M0 T473 S13**

Mirror-condition audit: compare each compiled mirror file against its original
and finish all its conditional branches before marking that file complete;
build each batch of 10 modified code files. The earlier per-depth BFS is superseded. Process the pending per-file inventory in its existing order; review
and register files even when no source edit is needed. If binaries have no
substantive change, reuse test evidence for the P; otherwise run Full before
publication. [Progress](../../artifacts/mvdm-audit/s13-bfs-progress.md);
[observed per-file inventory](../../artifacts/mvdm-audit/s13-r489-per-file-observed-inventory.json).
Latest checkpoint: [r1196](../../artifacts/mvdm-audit/s13-r1196-delivery.md).
The 1,058-file [census](../../artifacts/mvdm-audit/s13-r624-current-file-census.json)
includes one product-generator input; supported-context verification remains open.

Prior owner-approved delivery: restore the four MVDM ANSI-profile files changed
by MVDM-HOST-DIV-479, preserving authored build boundaries; verify actual
cached product builds and product verification before delivery. The restoration
is published; [evidence](../../artifacts/mvdm-audit/condition-inventory/ansi-restoration/README.md).

S11 closed its owner-narrowed non-MVDM compilation-condition audit and
cleanup; see [closure evidence](../etc/evidence/m0-t473-s11-condition-cleanup-closure.md).
Unfinished original foundation and later recovery work is transferred to the
first queued recovery proposal. The NTCON input-source handoff moves to S13;
its original S12 evidence and scope remain in the handoff document.
The DOS4GW/NESticle integration case and its unresolved environment-sensitive
startup issue are recorded in [TODO](TODO.md) and [evidence](../../artifacts/mvdm-audit/dos4gw-nesticle-integration.md).

CPU40 monitor/TLB remain a tested partial baseline; CPU30 removal is published,
CCPU V86 remains, and physical mouse input is unverified. Unfinished DPMI,
operand-fault, reentry and NPX/debug work transfers to queued recovery.
[Results and limits](../../artifacts/mvdm-audit/s11-foundation-evidence/code32-prebuilt-search/review.md).

S10 closed as a source-role audit at its [closure](../etc/evidence/m0-t473-s10-closure.md); this does not assert functional recovery. The revised [stage plan](../etc/operations/m0-t473-whole-recovery-stage-plan.md) preserves the earlier [monitor audit](../../artifacts/mvdm-audit/s11-monitor-minimization.md) as handoff evidence. Preserve unrelated work and immutable guest media.

The published foundation group includes far-transfer ownership and WOW
entry ABI; the bridge passed controlled loader cycles. S13's [test-failure recheck](../../artifacts/mvdm-audit/s13-test-failure-recheck.md) tracks CPU/NIDDB changes. The current package passed 17 Full gates and was published as a baseline; NIDDB VM semantics and historical T313/T335 tests remain unresolved.

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

Restoring the worker-shared idle query passed cross-task, separate-worker,
common-dialog and Full controls.

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

S13 reintroduced DIV-467/468 and CCPU test-failure candidates. The tested
r477 package is [published](../../artifacts/mvdm-audit/s13-r483-publication.json);
NIDDB multi-VM semantics remain [unproved](../../artifacts/mvdm-audit/s13-test-failure-recheck.md).
Earlier versions remain in [held patches](../../artifacts/mvdm-audit/s11-held-cpu/).

## Active Packet

### M0 T473 S13 â€” NTCON input-source handoff

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T473 S13, Ordinary Mode. |
| Admission And Approval | Owner accepted and closed S12 and admitted S13. [S12 evidence](../../artifacts/mvdm-audit/s12-excluded-branch-errors.md). |
| Objective | Input handoff; owner adds [mirror conditional reconciliation](../../artifacts/mvdm-audit/s13-mirror-conditional-plan.md). |
| Delivery State | Handoff published; review active. Checkpoint [r1082](../../artifacts/mvdm-audit/s13-r1082-delivery.md) was built and published; 162 files remain incomplete. Full 16 gates, DOS4GW startup, four Win3.1 modes and synthetic DOS/native three-CAF and Alt+Enter return passed. Physical input and injected handoff failures remain unverified; command output is in TODO. [Compile-branch evidence](../../artifacts/mvdm-audit/compile-branch-selection/README.md). [Evidence](../etc/evidence/m0-t473-s13-input-handoff.md). |
| Non-goals | Guest modification or rebuilding; DOS-specific shortcut workarounds; unrelated performance work. |
| Reference Baseline | Published ten-image package at 1d2ddfb18; [handoff scope and prior observations](../etc/operations/m0-t473-s12-ntcon-input-source-handoff.md). |
| Files And ABI Surface | NTCON frontend session, keyboard and channel; worker input client and original DOS scan-code handling; tests and evidence. |
| Applicable Rules | Source policy, immutable guest, original input semantics and centrally coordinated lifecycle. |
| Verification | Baseline and post-fix product probes for DOS and native CUI, source-switch and input ordering controls, Win3.1 Alt+Enter, cached build, Full and coherent publication. |
| Expected Markers | No stranded delivered keys or duplicate releases across source transitions; faithful input and preserved ordering. |
| Asset Needs | Existing build caches, isolated test-owned process/desktop and unchanged Win3.1 copy. |
| Reporting Requirements | Root cause, observed behavior, negative and lifecycle evidence, image hashes and unverified physical-input boundaries. |
| Stop Conditions | Guest modification or unapproved observable semantic change. |
| Exit Criteria | Scoped review, regression evidence, coherent publication, commit and push; await owner acceptance. |
| Original Owner Request | Execute S13 input handoff; comprehensively reconcile mirror conditionals under the adopted rule, then build, test, publish, commit and push. |
| Similar-Issue Sweep | Console-to-Window and Window-to-Console, DOS and native CUI, modifiers, focus and lifecycle. |
| Candidate Proposal | [NTCON input handoff](../proposals/proposal-ntcon-input-source-handoff-001.md). |
| Temporary Evidence | `build/t473-s13`, owner S13, 60-minute phase, 3-GiB size and 10-minute no-progress budget; stable caches retained. |

## S12 Closure Record

The owner accepted [S12 source layout and compile-time boundaries](../etc/evidence/m0-t473-s12-source-layout-closure.md).

## S11 Closure Record

The [S11 compilation-condition closure](../etc/evidence/m0-t473-s11-condition-cleanup-closure.md) records the selected non-MVDM cleanup, published package equivalence and deferred recovery boundaries.

## S10 Closure Record

The [S10 audit](../etc/evidence/m0-t473-s10-closure.md) records bounded source-role decisions and S11â€“S19 handoff.

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

T467 S3/S4 disposable build and acceptance evidence uses `build/t467-s3`
(60-minute phase, 3-GiB, 10-minute no-progress budget).

T471 S1 package-freeze evidence uses `build/t471-s1` (30-minute phase,
1-GiB, 10-minute no-progress budget); S2/S3 acceptance evidence uses
`build/t471-s2` (60-minute phase, 3-GiB, 10-minute no-progress budget).

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

T454 and T451 task build roots are disposable evidence, not shared caches.
Retain durable evidence and verify ownership and quiescence before cleanup;
T451 process inventory was denied at closure. See their linked closure records.
The shared `.ninja` and `.ccache` caches remain reusable.

T456 S1/S2 disposable evidence uses `build/t456-s1` and `build/t456-s2`.

## Recent M0 Closures

[T460](../history/m0-t460-closure.md), [T454](../history/m0-t454-closure.md),
[T453](../history/m0-t453-closure.md), T452,
[T451](../history/m0-t451-closure.md), [T450](../history/m0-t450-closure.md),
[T449](../history/m0-t449-closure.md), [T448](../history/m0-t448-closure.md).

## Recent Governance

T460 package and Clipboard acceptance are recorded in its closure above.
