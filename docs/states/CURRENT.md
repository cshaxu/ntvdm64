# Project Status

## Current Work

**Active: M0 T452 S2**

The owner accepted T451 and approved closure on 2026-10-24.
The [closure record](../history/m0-t451-closure.md) owns completed scope,
evidence and remaining boundaries. The owner now admits W03 modules, memory
and aliases, following ntvdm64-wow32-audit for minimum diff and maximum recovery.

## Active Packet

### M0 T452 S2 — bounded module-name and allocation recovery

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T452 S2, Ordinary Mode; single-agent author and separate reviewer passes. |
| Admission And Approval | Owner admitted W03 on 2026-10-24 and required ntvdm64-wow32-audit findings, minimum differences and maximum original recovery. S1 source matrix is accepted as the finite implementation denominator. |
| Objective | Repair the bounded module-name/alias output edge and prove the selected original CCPU40 allocation/free pair without creating a new owner. |
| Non-goals | No guest changes/rebuild, second heap/mapping/task authority, scheduler, external audit edits, WinOldAp lifetime rewrite or unrelated resource conversion. |
| Reference Baseline | Accepted product 7f87c7f98 and T451 closure 7f12beef1; content-pinned external OpenNT audit in the working plan. |
| Files And ABI Surface | Narrow original-mirror changes only at `wkernel.c` and `wole2.c`, plus focused tests/build registration as necessary. K233/K234, `nt_mem.c`, dispatch tables and the guest ABI remain unchanged. |
| Applicable Rules | Execution, architecture, coding, documentation, source policy and WOW32 minimum-diff admission governance. |
| Verification | One grouped x86 CCPU40 build/test pass after all S2 changes: module name zero/exact/truncated/failure/retired alias; k233/k234 selected allocation/free/failure semantics; then applicable package regression and documentation/diff gates. |
| Expected Markers | Bounded guest writes, zero on unavailable alias/process query, no transient leak; original guest-linear allocation/free path demonstrably selected; no page-domain or raw-pointer substitution. |
| Asset Needs | Pinned original OpenNT/audit, immutable guests and existing tests; no guest replacement. |
| Reporting Requirements | Separate static/build/runtime evidence; exact mirror and adapter line deltas; published package/hash evidence before owner acceptance. |
| Stop Conditions | New semantic concession, guest mutation, unbounded import, competing owner or material scope expansion. |
| Exit Criteria | Coherent bounded repair and focused tests pass on one production artifact; all S2 production artifacts are published with hashes, reviewed and pushed. S3 retains alias-table/WinOldAp lifetime work. |
| Original Owner Request | 准入队列第一条；按照 ntvdm64-wow32-audit 审计结果执行最小化 diff 和最大化复通。 |
| Similar-Issue Sweep | Module/query identities, allocation/free pairing, selectors/aliases, rollback, reentry, unload/task cleanup and reuse; unrelated services retain their queue owners. |
| Candidate Proposal | [Modules, memory and aliases](../proposals/proposal-wow32-modules-memory-aliases-001.md). |

The [working plan](../etc/operations/t452-modules-memory-aliases-plan.md)
and [S1 source matrix](../etc/evidence/m0-t452-s1-modules-memory-alias-source-matrix.md)
own the four-stage sequence and finite denominator. Only S2 is active.

## Current Technical Baseline

Accepted implementation baseline: `7f87c7f98`. S1 source-audit receipt:
`861e9112f`; it changed no product artifacts. S2 candidate r12 is deployed as
a coherent ten-image test baseline and awaits its independent review/commit;
its durable stage, Userenv runtime and publication receipts are in
`build/t452-s2`.

## Successor Boundary

The [Queue](QUEUE.md) now begins with files, environment and OEM; remaining
relative order is unchanged. SOL Options remains in the
queued everyday-usability package; startup is not full interaction acceptance.

## Temporary Evidence

S2 owns `build/t452-s2`: one-agent implementation/build/test evidence, 1 GiB
budget and one-hour diagnostic run budget. These are operational budgets, not
product timeouts. Join owned jobs and retain durable evidence before exact-target cleanup.

`build/M0-T451`, `build/t451-s3`, `build/t451-s4` and `build/t451-s5` remain
temporary evidence, not reusable caches. Process inventory access was denied
during closure; deletion requires verified quiescence and the existing
exact-target ownership checks. Durable results are linked from the closure.
Stable `.ninja` and `.ccache` caches remain untouched.

## Recent M0 Closures

- [T451](../history/m0-t451-closure.md): USER objects/shared view and CUI handoff.
- [T450](../history/m0-t450-closure.md): style removal and Run16 entry.
- [T449](../history/m0-t449-closure.md): WOW32 execution continuation.
- [T448](../history/m0-t448-closure.md): lifecycle.

## Recent Governance

T451 is closed. T452 S1 source reconciliation passed documentation governance,
relative links and diff checks at `861e9112f`; S2 product changes require the
full production delivery gates.
