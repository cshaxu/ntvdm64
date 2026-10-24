# T452 modules, memory and aliases plan

## Authority and sequence

Owner admitted W03 on 2026-10-24. [Current](../../states/CURRENT.md) alone
owns active execution. The [candidate design](../../proposals/proposal-wow32-modules-memory-aliases-001.md)
and [minimum-diff gate](wow32-queue-minimal-diff-admission-governance.md)
bound residual work. Inherited execution and EDIT/dialog DS work is not
reimplemented. An OOM dialog is not itself allocator failure evidence.

| Stage | Work | Exit |
| --- | --- | --- |
| S1 | Reconcile audit, original contracts, current production graph and tests. | Finite owner/ABI/lifetime matrix, grouped repair/test plan and diff estimates; research only. |
| S2 | Restore missing module/query/allocation bindings and failure rollback. | Actual selected caller/provider success, failure and cleanup; tested coherent delivery. |
| S3 | Alias reuse/retirement, failed/repeated load and safe module release. | No stale identity, duplicated authority or leaked resource; reentry/task cleanup proven. |
| S4 | Whole W03 acceptance and resource-loading handoff. | Full applicable regression, cached build, coherent publication, hashes, review and push; owner T acceptance. |

Do not defer foundation failure/cleanup defects across stages for convenience.
Resource conversion, GDI, multimedia and ToolHelp do not enter W03 merely
because they also use aliases.

## External audit receipt

Read-only root: sibling repository `../ntvdm64-wow32-audit`, resolved from
the product repository root; it is evidence, not a build/runtime dependency.
Read `management/README.md`, `BASELINE-STATUS.md`, current seam records and
selected memory/module reports. Current generated status is dated
2026-10-24T09:00:16Z; its source baseline is OpenNT commit
`5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`.

| Input | SHA-256 |
| --- | --- |
| management/seam-records.json | 6BB9FA592FE492F19DE0851811CA9020A79163A2061796BD6BCD03297CAA36E8 |
| management/seam-work.json | 7C5C88AF2D9FF8F9A8346EE64A91BEB7FA44630309ABC1893E19FDA4F677C3CA |
| management/BASELINE-STATUS.md | 596129EDD33425542576AA6C65ED34DB68677068B75B13121C80206AEDE44188 |

External Git identity query was denied; consumed file content is pinned
above instead. No audit input is modified. Use current exact seam records
with scoped evidence, not stale partial/U headings in older reports or
whole-family completion assumptions. Static closure explicitly records
`not_implemented_or_tested_by_audit`. Product image and runtime validation
remain this task's obligation. Existing product mirror provenance is checked
independently; do not silently replace selected source editions.

## Initial anchors, not a complete denominator

| Seam | Contract |
| --- | --- |
| SEAM:ef05926cf522c1c97f6b | Module filename: separate guest module/task alias/native identity; bounded capacity/conversion/flush, exact/truncated/failure and retired identity. |
| SEAM:42571d704af29168b45b | WinOldAp module-handle fallback: guest lookup first, existing child association, matching/duplicate-name rules and concurrent retirement. |
| SEAM:d8f111b47450e5cbf412 | K233 VirtualAlloc: selected CPU40/i386 conditions, guest-linear/backing identity, flags, failure and compatibility fill. |
| SEAM:d558e9391f6aafcab7c7 | K234 VirtualFree: matching identity, size/type and release ordering; WOW allocation must not be paired with a non-WOW release to invent a defect. |

Navigation confirms definitions in current `src/mvdm/wow32/wkernel.c`,
`wkman.c`, `wkmem.c` and `wole2.c`, not their build/runtime correctness.
Local-memory findings retain guest LOCAL algorithms and DS-qualified identity,
not same-name native LocalAlloc forwarding. Inherited EDIT/shared-DS support
is a dependency to reconcile, not a second implementation opportunity.

S1 must finish the actual W03 denominator from selected entries, conditions
and direct consumers. Each row needs source/hash, caller/provider, current
binding, state authority, creation/rollback/reentry/retirement, exact tests
and uncovered boundaries. These four anchors do not claim all gaps are known.

The resulting finite review is [M0 T452 S1 source matrix](../evidence/m0-t452-s1-modules-memory-alias-source-matrix.md).
It identifies the selected non-`i386` CPU40 branch, the direct source owners,
and the two bounded repair loci; it is not runtime proof.

## Implementation and evidence rule

Reuse original bodies first, then the smallest original-shaped binding.
Keep material necessary adaptation in its existing specialist owner and
delete displaced autonomous behavior. No second heap, mapping service, alias
registry or terminator: reconcile generic audit recommendations with existing
authoritative product state before selecting an implementation.

Compare affected mirrors bytewise and normalized against pinned source;
register each necessary divergence and report mirror, adapter and test line
additions/deletions separately. Admission changes zero runtime/mirror lines;
implementation delta is not estimated before the finite source review.
No new semantic waiver follows from API availability or a passing build.

Actual caller/provider normal/failure/cleanup tests and independent accepted
application frontiers remain required. Use stable Ninja/ccache trees,
background tests and coherent ten-image publication for production changes.
No computer use, guest modification, new polling or timeout is admitted.
External/guest limitations remain explicit non-passes. T closure waits for
owner acceptance.

S2 uses `tests/observation/verify-wow-module-alias-bounds.ps1` against the
actual CCPU40 `wole2.c` production compile command. Its fixture proves
exact/truncated/one-byte/unavailable alias output without substituting a
provider. `original-external-memory-test.exe` remains the direct selected
`VdmAllocateVirtualMemory`/`VdmFreeVirtualMemory` normal allocation, resize
and release proof. Neither test transfers `nt_mem.c` ownership to a new WOW
allocator.

Candidate `build/t452-s2/candidate-r12` staged the current coherent ten-image
set. Its `logon-wow-r12` proof used the complete supported Userenv block
(`CreateEnvironmentBlock`, current token, `inherit=false`) rather than an
inherited development shell and passed the owned WinMine flag/reset/open/close
path. Publication `build/t452-s2/publication-r2/publication-r2.json` records
the matching candidate, `assets/release` and `O:\\winnt\\system32` image
hashes. The historical oversized/inherited COMMAND environment failure is not
claimed as a module-name regression or repaired in S2.

## S3 admission and audit boundary

S2 closed at `0f8487f04`. S3 begins source-first review of the original
`wole2.c` 32-slot alias table and of the existing `PTD::pWOAList` chain in
`wkman.c`, `walias.c` and `wuwind.c`. These are separate authorities: an OLE
HTASK alias is neither a WOW task nor a WinOldAp representative. The existing
task-scoped WOA list, original selector encoding and original 32-slot
oldest-creation-time replacement rule remain the default contract.

The review must separately account for WOA node allocation, pending
representative association, caller-scoped module and instance lookup, child
wait/query completion, parent task retirement, node disposal, native process
handle close and native process termination. Original code proves that node
disposal is not itself permission to close or terminate a child. S3 may add a
narrow guard or serialization only if a reachable project-owned concurrent or
stale-access path is established. It may not introduce a global registry,
opaque-token coordinator, retained process-handle owner or changed capacity
policy merely to make a probe easier.
