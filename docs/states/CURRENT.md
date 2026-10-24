# Project Status

## Current Work

**Active: M0 T451 S5**

## Active Packet

### M0 T451 S5 — SOL Options access-violation investigation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T451 S5, Ordinary Mode. |
| Admission And Approval | Owner admitted S5 on 2026-10-23 after observing that SOL can now enter its Options route but faults in `WOW32.DLL`; S4 remains its completed, separately bounded handoff/acceptance delivery. |
| Objective | Reproduce and source-trace the SOL Options `WOW32.DLL` access violation to its original consumer, copied-view/host boundary and lifetime state; determine a finite repair proposal or a source-proven retained limitation. |
| Non-goals | No guest modification/rebuild, USER-server reconstruction, new object table, speculative catch-all exception handling, broad control/dialog/message implementation, external source import, or production repair before the root-cause design is recorded. |
| Reference Baseline | S1 object ledger; S2 publication closure; S3 mutation/destruction closure; S4 handoff audit; owner screenshot of SOL Options fault. |
| Files And ABI Surface | Investigation evidence and, if needed, a narrowly scoped source-level diagnostic fixture only; no guest or new cross-component ABI. |
| Applicable Rules | [Execution](../rules/EXECUTION.md), [architecture](../rules/ARCHITECTURE.md), [coding](../rules/CODING.md), and [source policy](../etc/operations/policy/source-policy.md). |
| Verification | First obtain a reproducible Options route and map the fault offset to the exact source/binary; trace the original call chain and every object/state owner.  Then compare the reached original contract with the modern-host adapter without changing guest media. |
| Expected Markers | Exact faulting module/address and source range; reproducible input sequence; one identified invalid identity, stale projection, invalid host assumption, or a source-proven non-project limitation; no guessed ownership. |
| Asset Needs | Existing selected original sources, current Ninja symbols/build graph, immutable guest media and the owner-supplied runtime observation only. |
| Reporting Requirements | State the reproducer, fault mapping, original algorithm, adapter boundary, identity/lifetime owner, root cause confidence, repair options and any semantic boundary. |
| Stop Conditions | Pause for a new observable semantic concession, unapproved source import, guest change, or an unbounded dependency closure. |
| Exit Criteria | A finite, evidence-backed root-cause report and repair/limitation disposition; no repair is claimed merely because the fault is caught or hidden. |
| Original Owner Request | “不用打开，你准入一个S5来调研看看吧”。 |
| Similar-Issue Sweep | Cover SOL dialog/control/menu calls that share the faulting original consumer, including stale/reused HWND, failed dialog creation and nested callback paths when reached by the trace. |
| Candidate Proposal | [WOW32 USER objects/shared view](../proposals/proposal-wow32-user-objects-shared-view-001.md). |

## Current Technical Baseline

The current ten-image host package is published in both `assets/release/` and
`O:\winnt\system32`; guest media and guest configuration remain immutable and
unchanged.  During S3, the published Run16 image additionally corrected the
host-specific cold-NTSRV RPC startup classification without changing its
existing five-second bootstrap grace; its exact build, cold Console exercise
and ten-image hash evidence are in the [S3 audit](../etc/evidence/m0-t451-s3-mutation-destruction-audit.md).
The S2 USER publication acceptance remains linked below.
T450 is closed at
[its closure record](../history/m0-t450-closure.md). T451 S2 begins from the
S1 inventory; no new WOW32 capability is claimed until its coherent delivery.

## S1 Closure Record

The [S1 closure record](../etc/evidence/m0-t451-s1-closure.md) fixes the
completed inventory boundary at `110f2f078` and links its source-first ledger.

## S1 Completed Inventory

The indexed [USER object contract ledger](../etc/operations/m0-t451-s1-user-object-contract-ledger.md)
reconciles ten finite rows across direct WND/CLS/MENU/ITEM readers, desktop and
system roots, read-only external objects, geometry/data publication, and
retirement.  It assigns the remaining publication, mutation and acceptance
obligations to S2--S4 without reopening T447/T449's accepted local execution
paths.  This documentation-only delivery changed no production source, build
input, guest asset, release asset or installed file.  Documentation governance
passed; it was committed and pushed as `110f2f078`.

## S2 Closure Record

The [S2 publication audit](../etc/evidence/m0-t451-s2-user-publication-audit.md)
confirms the selected original USER16/WOW32 readers and current copied-view
publishers for R01/R03/R04/R06/R07/R08. It also removes a stale test prelude
left by T450's style deletion: the lifecycle fixture now begins with its real
third-class failure injection and passes its rollback/retry contract. The
admitted first-task handoff is now proven by its focused real-CCPU row:
registered `pfnInitTask` publishes the immediately readable desktop/task-base
view and completes normal cleanup. A later private-desktop run completed the
four independent external-graph/menu fixtures with exit zero; their detailed
markers are retained in the audit. The admitted first-task handoff publication
correction is now a verified and published product image; the closure record
states its exact package/guest evidence.

The finalized focused batch is green: the selected x86 `wow32` target is up to
date after its nested worker dependency; task-lifecycle, initialization
notification and real-CCPU initial publication pass; and the private-desktop
window-borrow, graph-snapshot, menu-resource and standard-class fixtures pass.
The latter four retain actual graph/menu, negative and lifetime markers; they
are not inferred from a successful build. The complete ten-image candidate
build, immutable-guest acceptance, dual-destination hash verification and
delivery package are complete. See the
[S2 closure record](../etc/evidence/m0-t451-s2-closure.md).

## S4 Closure Record

The [S4 closure record](../etc/evidence/m0-t451-s4-closure.md) records the
completed selected-consumer accounting, 19-row direct object matrix, WINMINE
interaction acceptance and SOL/WRITE startup frontiers. It changed no product
binary: the candidate, `assets/release`, and installed `WOW32.DLL` remain
hash-identical.  The subsequently owner-admitted S5 is a separate, observed
SOL Options fault investigation; it does not rewrite S4's selected handoff
evidence or claim that its startup frontier proved complete SOL interaction.

## S3 Closure Record

S3 is closed at [its closure record](../etc/evidence/m0-t451-s3-closure.md).

## Active temporary outputs
Its evidence root `build/t451-s3/` remains disposable task evidence. S4 owns
its own fresh bounded evidence root under `build/t451-s4/`.
S4's same-artifact WINMINE private-desktop interaction gate has passed using
the current source-built standard observer.  Remaining SOL/WRITE frontiers
use the current one-app-at-a-time observer, which already understands the
single `system32` package layout; the completed consumer/handoff audit records
the passed game frontier and that retained test selection.
The tracked native-slot group requires a private desktop. Its first
low-integrity execution received `ERROR_ACCESS_DENIED` from `CreateDesktopW`,
but the same checked-in runner was then rerun in the desktop-capable execution
context used by the historical acceptance path. The fresh, source/artifact
hashed matrix at `build/t451-s3/desktop-current/matrix/` passed all 19 rows;
the [S3 audit](../etc/evidence/m0-t451-s3-mutation-destruction-audit.md)
records its exact scope and the access-context diagnosis. The same audit now
separately records the finite modern-host
boundary for original kernel-owned real destruction: neither a return hook,
WinEvent nor CBT covers every original root/child/failure path.  No synthetic
host replacement or production semantic concession is selected. Stable
Ninja/ccache roots remain untouched.

## Recent M0 Closures

| Task | Disposition |
| --- | --- |
| T450 | [Owner-accepted style removal and Run16 Console-entry closure](../history/m0-t450-closure.md). |
| T449 | [Owner-accepted WOW32 execution continuation closure](../history/m0-t449-closure.md). |
| T448 | [Owner-accepted lifecycle closure](../history/m0-t448-closure.md). |

## Recent Governance

T450's completed S5 evidence is retained with its closure record. This
documentation-only transition admits T451 S1; it does not rebuild, replace or
republish the already hash-verified ten-image package.
