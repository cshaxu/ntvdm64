# Project Status

## Current Work

**Active: M0 T475 S1 — NXVM and Win32 library separation; RPC**

The owner directs local build, test, publication to this worktree's
assets/release and local commits for later cherry-pick; no push or publication
to the main worktree runtime. The owner closes S2 locally and admits S3 to
extract product-independent RPC and I/O mechanisms from common to lib/rpc
and lib/io while retaining NTVDM64 protocol and policy in common.

S1 local closure: Run16-owned implementation no longer enters the inspected
other-product source/link closures. Hook-specific classification belongs to
Hook; NTCON and Hook32 dropped unused Run16 link inputs. Builds, focused
regressions and dependency negative controls passed. Local ten-image release
4249ef52-c3d3-4954-bfe9-c375ef046e1f was hash-verified and committed; three
changed images were freshly built and seven unchanged baseline images reused.
Full integration and publication to the main runtime remain pending.

S2 is owner-closed locally after implementation, publication and Control verification;
main integration and Full product verification remain pending. Shared clients, scope and transport belong to
common/base_srv; service policy remains NTSRV-owned. Candidate package
e34caccf-13d8-4999-9be2-183ba247b6ee is published locally with ten matching
hashes. The complete Control suite passed all 12 selected gates with the
matching-width observer and an explicitly bounded test-only PATH for nested
window handoff. The optional DOS4GW/NESticle fixture was absent and skipped;
Full was not run. The original guest long-PATH issue remains unresolved.
[Evidence](../etc/evidence/m0-t474-s2-basesrv-shared-ownership.md).

S3 implementation and local publication are ready for owner review. Generic
RPC and I/O mechanics now reside in lib; product protocol and policy remain
in common. Fresh local package e05f16a8-e865-40e1-8a6e-d86bc5d25809 passed
all 12 selected Control gates and ten release hash checks. Full was not run;
the optional DOS4GW/NESticle fixture was unavailable. The original guest
long-PATH issue remains. [S3 evidence](../etc/evidence/m0-t474-s3-generic-libraries.md).
Owner adds an S3 P to relocate the complete NTCON lib tree to src/lib,
including its pinned SoftPC and terminal assets. Preserve source bytes and
import provenance; update all consumers, build graphs and tests, then build,
test, publish locally and commit. The ten-image local package
e719bbee-ba87-42c4-8438-80f0d62fdd0a passed all 12 selected Control gates;
Full was not run, and the optional DOS4GW/NESticle fixture was absent.
[S3 P evidence](../etc/evidence/m0-t474-s3-generic-libraries.md).
Temporary evidence: build/t474-s3-p2.

Owner directs rebasing this worktree onto main commit b919dd182 and rebuilding
all release assets from the combined committed sources. Retain this worktree's
active packet on status conflicts; never treat either pre-rebase binary package
as the final combined release. Main's uncommitted S13 changes are not included.
Fresh combined package 9f45db63-8a83-44c4-84b7-f4a45fd9297a is published to
this worktree; ten new build inputs, release hashes and all 12 selected Control
gates passed. Full was not run; the optional DOS4GW/NESticle fixture was absent.
The S3 evidence records the Git byte-preservation correction and build runs.
Temporary evidence: build/t474-s3-rebase.

T475 S1 is locally implemented and ready for owner review. NXVM and Win32
source ownership is separated, and Win32 RPC includes the extracted local
peer-process mechanism while NTSRV retains authentication and policy. Local
package b08813cb-cf4a-48d5-8ec8-cd0590a35f47 passed all 12 selected Control
gates and ten release hash checks. Full was not run; the optional external
DOS4GW/NESticle fixture was absent. [Evidence](../etc/evidence/m0-t475-s1-win32-rpc.md).

Owner admits a further T475 S1 P to extract generic Win32 process attribute
list allocation, initialization, update and cleanup from the service's Worker
and frontend startup paths into `lib-win32/process`. Preserve admission, resume,
rollback, handle ownership and caller-specific attributes. Verify x86/x64 build,
focused positive/failure/lifecycle tests, product Control and local publication.
Local package f3161950-6ff1-4558-9080-9686bd37cb98 passed all 12 selected
Control gates, 36 process unit checks and release hash checks. Full was not
run; the optional DOS4GW/NESticle fixture was absent. [Evidence](../etc/evidence/m0-t475-s1-win32-rpc.md).
Temporary evidence: build/t475-process-p; no push or main-runtime publication.

Owner admits a further T475 S1 P for `lib-win32/handle`, shared by NTSRV RPC
handle export and NTHOOK child-process handle delivery. Preserve each caller's
allocation, ownership, rollback and error rules; test local and remote handle
cleanup. Local build, tests, assets/release and commit; no push or main runtime
change. The 13-check local/remote handle test, 118 Hook assertions, all 12
selected Control gates and ten release hash checks passed for local package
9baf506d-436a-411a-aeda-cf70fcfea4a0. Full was not run; the optional
DOS4GW/NESticle fixture was absent. [Evidence](../etc/evidence/m0-t475-s1-win32-rpc.md).
Temporary evidence: build/t475-handle-p.

Owner admits a further T475 S1 P for `lib-win32/process-hook`: extract generic
suspended-child payload and same-/cross-width hook installation from NTHOOK;
preserve product payload, routing, caller-owned rollback, Detours provenance
and helper behavior. Build and test locally, publish only this worktree's
assets/release and commit; no push. The seven mechanism checks, 118 Hook
assertions on each architecture, repeated Control 12/12 and ten release hash
checks passed for package 4d73890b-33b8-4edf-8ee7-0856c8b61761. The initial
Control handoff observer failure, focused pass and full repeat are recorded in
[S1 evidence](../etc/evidence/m0-t475-s1-win32-rpc.md); Full was not run and
the optional DOS4GW/NESticle fixture was absent. Temporary evidence:
build/t475-hook-p.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T475 S1, Ordinary Mode. |
| Admission And Approval | Owner requests a new T to separate NXVM and project Win32 libraries, starting with RPC. |
| Objective | Rename the NXVM library owner to src/lib-nxvm, introduce src/lib-win32, and place generic RPC, process attributes and shared handle mechanisms and generic process-hook installation there. |
| Non-goals | Change RPC behavior, guest media, main worktree, or alter process admission, resume, hook propagation or rollback semantics. |
| Reference Baseline | T474 S3 local release 9f45db63-8a83-44c4-84b7-f4a45fd9297a; [evidence](../etc/evidence/m0-t474-s3-generic-libraries.md). |
| Files And ABI Surface | Library trees, include paths, imported manifest and Git attributes, build graphs, tests and architecture authorities. |
| Applicable Rules | Preserve NXVM provenance and functionality; no product dependency in low-level libraries. Owner directs local release and commit without push. |
| Verification | Import identity and dependency checks, affected builds, RPC, process, handle and NXVM focused tests, local package, Control, governance and relative links. |
| Expected Markers | NXVM and Win32 ownership distinct; generic RPC clients use lib-win32/rpc; product protocol stays common. |
| Asset Needs | Existing toolchain and local package; no new source import. |
| Reporting Requirements | Relocation, behavior, test results and Full/fixture limits. |
| Stop Conditions | Required semantic change or inability to preserve imported-source provenance. |
| Exit Criteria | Local build, test, assets/release publication and local commit; no push. |
| Original Owner Request | Rename current lib as lib-nxvm, create lib-win32 for project common capabilities, first establish lib-win32/rpc. |
| Similar-Issue Sweep | Existing generic lib/io placement and all lib path references. |
| Temporary Evidence | build/t475-s1, owner T475 S1, 60-minute phase, 3-GiB and 10-minute no-progress budget. |

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
