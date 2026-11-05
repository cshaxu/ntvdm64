# Project Status

## Current Work

**Active: M0 T476 S1** — audit original mirror conditional branches and diagnostic lines in an independent worktree.

The owner admitted the former queue item 11. T474 S1 was delivered on main
([integration evidence](../etc/evidence/m0-t474-s1-shared-range-integration.md));
main's separate T474 S2 work remains untouched. The preceding independent
[T475 closure](../history/m0-t475-closure.md) is indexed separately. The former queue item 2 is
independent and remains queued. This worktree starts at `eac2932d5`.

## Active Packet

### M0 T476 S1 — inventory and recover missing conditional structure

The host C/H audit, full marker build, 30 outermost exclusions, selected
product rebuild and Control gate have passed. The coherent ten-image package
is published only to this worktree's `assets/release`; see the
[audit evidence](../etc/evidence/m0-t476-s1-mirror-conditional.md). This is
a local delivery awaiting integration; main and its runtime were not changed.

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T476 S1, Ordinary Mode. |
| Admission And Approval | Owner directed a new T in a worktree based on main's latest commit for the queued original mirror excluded-branch `#error` and `#line` audit. |
| Objective | Enumerate host mirror C/H files and match original identities; recover missing original conditional boundaries without restoring bodies already replaced by #error or changing existing #line directives. Then instrument all remaining branches and collect full-build hits before any new exclusion decisions. |
| Non-goals | Do not audit or alter guest mirrors or host mirror files other than C/H; do not enable unsupported profiles, change selected behavior apart from the required T476 product version, or modify main's unfinished T474 S2. |
| Reference Baseline | `eac2932d5`; historical [S13 closure](../history/m0-t473-s13-closure.md) is inherited evidence, not proof of complete-file coverage. |
| Files And ABI Surface | Host mirror `.c` and `.h` files in `src/mvdm-host` and `src/non-mvdm`, their product build contexts, necessary include adapters, focused tests and audit records; Guest and non-C/H mirrors are excluded. |
| Applicable Rules | Source policy, original source provenance, per-file complete audit, preserve unrelated work. |
| Verification | Pin original/current identities; verify recovered conditional structures and preserved existing guards, instrument remaining branches, full x86/x64 build and hit collection, and validate marker equivalence. Apply later exclusions from the complete hit ledger; run package comparison and required gates. |
| Expected Markers | Every in-scope host C/H mirror file is inventoried and mapped to its recorded original; existing #error/#line retained; missing condition structures recovered; every not-yet-excluded arm marked and full-build hits recorded. |
| Asset Needs | Existing pinned original sources and supported local compiler/build tools; no external fetch. |
| Reporting Requirements | Report completed/identical/incomplete/deleted counts and tested contexts, limitations and local commits. |
| Stop Conditions | Unavailable verified originals, unsupported profile selection, guest mutation or unapproved selected behavior change. |
| Exit Criteria | Complete inventory and conditional-structure recovery, comprehensive marker build/hit ledger, and subsequent file-by-file unhit-arm exclusion; validate and deliver batches under the proposal. |
| Original Owner Request | Owner specified: audit only host mirror C/H files; match original and recover conditional structure, preserving existing #error/#line; mark non-excluded branches; full build and register hits; replace unhit arms with #error and remove their nested branch structure. Guest and non-C/H mirrors are excluded. |
| Similar-Issue Sweep | All in-scope host C/H mirror files and actual build contexts; preserve already excluded arms and line markers; use owner-directed full-build hit results for remaining branch decisions. |
| Candidate Proposal | [Complete mirror conditional audit by file](../proposals/proposal-mirror-conditional-file-audit-001.md). |
| Temporary Evidence | `build/t476-s1`, 90-minute phase, 4-GiB limit; build-owned caches only. |

## M0 T475 S5 Closure

The imported process-hook is selected by the product; the recorded local
helper fix is retained. C++ source/build selection and Detours dependencies
were removed. x86/x64 builds, all four caller/target width pairs, missing-DLL,
negative and lifecycle checks, real guest tests, and the Full selected gates
passed. WRITE edit, save and restart passed against the pre-replacement and
current packages. The owner excludes DOS4GW/NESticle. The late observer's
new-thread exclusion limit was also present in the former Detours use and is
not a new closure requirement. Local release, commits and exact evidence are
recorded in [S5 evidence](../etc/evidence/m0-t475-s5-process-hook-import.md).
The owner directs no push or main-runtime publication.

## S12 Closure Record

The owner accepted [S12 source layout and compile-time boundaries](../etc/evidence/m0-t473-s12-source-layout-closure.md).

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

[T460](../history/m0-t460-closure.md), [T454](../history/m0-t454-closure.md),
[T453](../history/m0-t453-closure.md), T452,
[T451](../history/m0-t451-closure.md), [T450](../history/m0-t450-closure.md),
[T449](../history/m0-t449-closure.md), [T448](../history/m0-t448-closure.md).

## Recent Governance

T460 package and Clipboard acceptance are recorded in its closure above.
