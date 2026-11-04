# Project Status

## Current Work

**Active: M0 T475 S2 — NTCON shared implementation ownership**

The owner closes T475 S1 locally and directs continuation of the component
ownership review. Audit NTCON production consumers, distinguish static/source
dependencies from explicit runtime protocols and test-only selections, and
move genuinely shared implementations to their proper common or library owner.

T475 S1 separated NXVM and Win32 library ownership. Its final four local P
commits extracted object-handle comparison, overlapped pipe-pair creation,
parent-process observation and file identity. Their local packages each
passed the selected Control suite; final package
`2b1325bd-17fb-4435-b2be-55995de325f6` has ten verified release hashes.
Full was not run and the optional DOS4GW/NESticle fixture was absent. The
four commits remain local for later cherry-pick. See the
[T475 S1 evidence](../etc/evidence/m0-t475-s1-win32-rpc.md).

Earlier independent-worktree shared ownership stages locally closed Run16 and
NTSRV source edges; [T474 S2 evidence](../etc/evidence/m0-t474-s2-basesrv-shared-ownership.md)
and [T474 S3 evidence](../etc/evidence/m0-t474-s3-generic-libraries.md)
retain the decisions, rebase and publication limits. Main-worktree integration
and full product verification remain pending. No push or main-runtime
publication is authorized by the local workflow.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T475 S2, Ordinary Mode. |
| Admission And Approval | Owner closes S1 and directs the next startup-order component ownership review. |
| Objective | Audit NTCON-owned production implementation and remove unauthorized dependencies from other product components while preserving behavior. |
| Non-goals | No blanket Console rewrite, broker import, guest modifications, main-worktree changes or moving NTVDM policy into generic libraries. |
| Reference Baseline | T475 S1 local package 2b1325bd-17fb-4435-b2be-55995de325f6; T474 Run16 and NTSRV ownership evidence above. |
| Files And ABI Surface | NTCON sources and headers, other production source/link closures, appropriate common or library owners, build graphs, focused tests and evidence. |
| Applicable Rules | Preserve original source provenance, runtime protocol and lifecycle; component-private sources cannot be production dependencies of sibling components. Worker-base exception applies only to its two workers. Local release and commits only. |
| Verification | Inventory all real production consumers in x86 and x64 graph selections; distinguish test-only and runtime edges; focused positive, negative and lifecycle checks; relevant builds, coherent local package, Control, hash checks and governance for implementation. |
| Expected Markers | Each NTCON-private production dependency is removed or explicitly justified under the owner rule, and actual use and disposition are evidenced. |
| Asset Needs | Existing toolchain, production build graphs and local release package; no new import. |
| Reporting Requirements | Consumer, symbol, actual use, ownership decision, moved or retained code, verification and remaining limitations. |
| Stop Conditions | Required semantic concession, missing provenance or a proposed dependency inconsistent with the owner direction. |
| Exit Criteria | Ownership audit complete and any necessary corrections built, tested, locally published and committed; no push or main-runtime publication. |
| Original Owner Request | Continue startup-order shared implementation ownership review after closing the current S. |
| Similar-Issue Sweep | NTCON sources, public/private includes, static archives, native/x86 production graphs, test-only consumers, common protocol and Worker Base boundaries. |
| Temporary Evidence | build/t475-s2, ordinary worktree-local evidence with recorded owned test processes. |

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
