# Project Status

## Current Work

**Active: M0 T475 S4 — NTHook dependency cleanup**

The owner accepts the latest local audit and admits S4 to remove the private
NTHook source/header/static-link dependencies of Run16, NTVDM and NTVWM. The
same audit found that NTHook32 and VDMREDIR reach NTVDM's private SoftPC symbol
shim through actually linked shared/mirror objects; include that related
build-boundary defect in S4. S3's earlier header-closure claim was incomplete.
The owner directs worktree-local build, tests, asset publication and commits,
with later cherry-pick and no push or main-runtime publication.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T475 S4, Ordinary Mode. |
| Admission And Approval | Owner closes the local audit and admits a new NTHook-dependency cleanup task. |
| Objective | Remove private NTHook dependencies from other components and the reached private SoftPC forced-header dependencies without altering hook behavior. |
| Non-goals | Change hook interception policy, packet identity, session authority, guest media, the allowed Worker Base edges, or separately queued NTVDM runtime imports. |
| Reference Baseline | Local commit 9b339696d, release package 8b055e88-0703-4652-a14d-720d2d9f61e6; prior S3 and S2 evidence. |
| Files And ABI Surface | NTHook, Run16, NTVDM, NTVWM, common, build graph, compiler-selected headers, hook packet and process injection boundaries. |
| Applicable Rules | Shared product policy belongs in common; generic Win32 mechanism belongs in lib-win32; preserve packet identity, widths, process ownership, failure and cleanup semantics. |
| Verification | Per production P: affected x86/x64 builds, targeted positive/negative/lifecycle tests, all selected Control gates, ten-image local publication and hash checks, governance and diff review. |
| Expected Markers | Other products select no NTHook-owned source or private header; NTHook32 and VDMREDIR selected closures do not reach the NTVDM-private forced header; actual linkage and runtime behavior verified. |
| Asset Needs | Existing toolchain, fixtures and local package; no new external source or guest media. |
| Reporting Requirements | Identify each removed edge and remaining runtime boundaries, ownership, tests and local publication limitations. |
| Stop Conditions | Required semantic concession, duplicate state authority, incompatible ABI or missing required evidence. |
| Exit Criteria | All admitted dependencies remediated and audited; every production P built, tested, published locally and committed; main integration deferred per owner. |
| Original Owner Request | Close the current S and admit a new task to clean up NTHook dependencies. |
| Similar-Issue Sweep | Run16, NTVDM, NTVWM, Hook32/64, worker and generic-library actual link selections and compiler-recorded transitive headers. |
| Temporary Evidence | Worktree-local build/t475-s4 with owned compiler and test processes. |

Planned P boundaries: P1 shares the hook context and payload contract used by
Run16 and NTHook; P2 shares the installer orchestration used by both workers
and NTHook; P3 removes the private forced-header reach from selected non-worker
closures and audits the complete link and compiler dependency graph. Adjust a
boundary if it cannot build independently without an unsafe temporary state.
P1 passed the x86/x64 build, launcher boundary negative control, 118 x86 and
118 x64 Hook checks, 168 cross-width checks and 12 Control gates. Local package
`563bc615-556c-49e6-869e-6d7ea0004396` has ten verified release hashes; see
[S4 evidence](../etc/evidence/m0-t475-s4-nthook-dependency-cleanup.md). Full
and DOS4GW/NESticle were not run. P2 and P3 remain active.

The owner closes the [S2 dependency inventory](../etc/evidence/m0-t475-s2-component-dependency-inventory.md)
and admits S3 to address the illegal Worker Base and NTVDM dependencies through
common and libraries. The [S3 design](../etc/operations/m0-t475-s3-worker-ntvdm-dependency-plan.md)
now assigns Redir-only logic to Redir, shared product contracts to common, and
generic Win32 mechanisms to lib. Seven P deliveries are sequential. The owner
separately queues removal of the remaining NTVDM PE runtime imports.
S3 produced seven locally tested and published commits, ending at package
`8b055e88-0703-4652-a14d-720d2d9f61e6`; its [evidence](../etc/evidence/m0-t475-s3-worker-ntvdm-remediation.md)
records verification and the subsequent header-boundary correction above.
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

## T475 S3 Local Closure Record

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T475 S3, Ordinary Mode; locally concluded. |
| Admission And Approval | Owner admits the S and seven P deliveries; source, header and static-link scope is confirmed, with runtime-import removal separately queued. |
| Objective | Remove prohibited Worker Base and NTVDM source, header and static-link dependencies through common and platform libraries. |
| Non-goals | Alter the allowed two-worker Worker Base dependency, NTHook ownership, guest media, original ABI semantics or main worktree. |
| Reference Baseline | S2 inventory commit 5bc6eb897 and local package 2b1325bd-17fb-4435-b2be-55995de325f6. |
| Files And ABI Surface | VDMREDIR, WOW32, NTVDM, common and lib-win32 source/headers, source/link graph, module interfaces, tests and documentation. |
| Applicable Rules | Preserve mirror provenance, original behavior, one session/page-domain authority, callback lifetime and calling conventions; generic libraries cannot depend on common or private product code. |
| Verification | Per production P: affected x86/x64 builds, real positive/negative/lifecycle fixtures, Control, ten-image local publication and hash checks, governance and diff review. |
| Expected Markers | Non-worker components select no Worker Base input and no NTVDM-private implementation/header; required runtime contracts and state ownership remain verified. |
| Asset Needs | Existing toolchain, fixtures and local package; no external source import. |
| Reporting Requirements | Per consumer/provider, actual use, removed edge, shared owner, preserved state, verification and runtime-ABI disposition. |
| Stop Conditions | Required semantic concession, missing provenance or duplicate state owner; none accepted for the local closure. |
| Exit Criteria | Source, header and static-link scope locally verified and committed; runtime-import removal separately queued per owner. Main-worktree integration, system32 publication and push remain owner-directed deferrals. |
| Original Owner Request | Admit one S to address Worker Base and NTVDM dependencies by placing suitable code into common and lib; provide a design. |
| Similar-Issue Sweep | VDMREDIR and WOW32 source, private headers, selected archive members, forced includes, NTVDM module ABI and all Worker Base consumers. |
| Temporary Evidence | build/t475-s3, worktree-local evidence with owned test processes. |
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
