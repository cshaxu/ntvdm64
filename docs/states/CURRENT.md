# Project Status

## Current Work

**Active: M0 T452 S3**

The owner accepted T451 and approved closure on 2026-10-24.
The [closure record](../history/m0-t451-closure.md) owns completed scope,
evidence and remaining boundaries. The owner now admits W03 modules, memory
and aliases, following ntvdm64-wow32-audit for minimum diff and maximum recovery.

## Active Packet

### M0 T452 S3 — original alias and WinOldAp retirement recovery

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T452 S3, Ordinary Mode; single-agent author and separate reviewer passes. |
| Admission And Approval | S2 is published at `0f8487f04`. The owner-approved T452 matrix now admits its finite S3 row under the same minimum-difference and maximum-original-recovery rule. |
| Objective | Trace and, only where source evidence proves a project-owned gap, restore original HTASK-alias reuse/retirement and caller-scoped WinOldAp module/instance identity without replacing either original authority. |
| Non-goals | No guest changes/rebuild, second alias/module/task registry, token/coordinator redesign, child-process termination policy, scheduler, external audit edits or unrelated resource conversion. |
| Reference Baseline | Accepted product 7f87c7f98 and T451 closure 7f12beef1; content-pinned external OpenNT audit in the working plan. |
| Files And ABI Surface | Research begins at original `wole2.c`, `walias.c`, `wkman.c` and `wuwind.c`; any repair must remain at an already authoritative owner. K233/K234, `nt_mem.c`, dispatch tables and the guest ABI remain unchanged. |
| Applicable Rules | Execution, architecture, coding, documentation, source policy and WOW32 minimum-diff admission governance. |
| Verification | First establish a finite lifecycle/ownership matrix for alias add/find/get/remove and WOA create/associate/query/normal wait/parent retirement. Only then run one grouped implementation build/test pass. |
| Expected Markers | Original selector shape, 32-slot eviction and task-scoped fallback survive; pending/retired representative access has an explicit source-proved disposition; node disposal, process-handle close and process termination remain distinct. |
| Asset Needs | Pinned original OpenNT/audit, immutable guests and existing tests; no guest replacement. |
| Reporting Requirements | Separate static/build/runtime evidence; exact original/mirror delta; explicit state owner and normal/failure/retirement proof. |
| Stop Conditions | New semantic concession, guest mutation, unbounded import, competing owner, needed session-wide coordinator or material scope expansion. |
| Exit Criteria | A finite original-owner disposition is implemented and tested, or every unrecoverable semantic dependency is proved and handed to its correct later packet without disguising it as an S3 completion. |
| Original Owner Request | 准入队列第一条；按照 ntvdm64-wow32-audit 审计结果执行最小化 diff 和最大化复通。 |
| Similar-Issue Sweep | Module/query identities, allocation/free pairing, selectors/aliases, rollback, reentry, unload/task cleanup and reuse; unrelated services retain their queue owners. |
| Candidate Proposal | [Modules, memory and aliases](../proposals/proposal-wow32-modules-memory-aliases-001.md). |

The [working plan](../etc/operations/t452-modules-memory-aliases-plan.md)
and [S1 source matrix](../etc/evidence/m0-t452-s1-modules-memory-alias-source-matrix.md)
own the four-stage sequence and finite denominator. Only S3 is active.

## Current Technical Baseline

Accepted implementation baseline: `7f87c7f98`. S1 source-audit receipt:
`861e9112f`; it changed no product artifacts. S2 delivery `0f8487f04` is the
current coherent ten-image deployed baseline; its durable stage, Userenv
runtime and publication receipts are in `build/t452-s2`.

## Successor Boundary

The [Queue](QUEUE.md) now begins with files, environment and OEM; remaining
relative order is unchanged. SOL Options remains in the
queued everyday-usability package; startup is not full interaction acceptance.

## Temporary Evidence

S3 reuses `build/t452-s2`: one-agent audit/implementation/build/test evidence, 1 GiB
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
