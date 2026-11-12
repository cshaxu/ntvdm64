# Project Status

## Current Work

**Active: M0 T483 S4 — minimize/consolidate registered mirror differences.**

The owner authorized S1 closure and has now approved the internal T483 sequence:
S2 restore original (76), S3 delete (120), S4 minimize/consolidate (117),
S5 correct (207), S6 resolve missing evidence (755). See the
[category plan](../etc/operations/m0-t483-mirror-register.md#owner-approved-s2s6-remediation-sequence).
Owner now authorizes sequential execution and closure through S5. S2 and S3 are closed; S4 is active;
S5 follows only after S4 closure. S6 must not be admitted without a new
owner instruction.
The previously suggested compat-first S2 is superseded.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T483 S4, Ordinary Mode; one agent, initial and counter-review passes. |
| Admission And Approval | Owner authorizes S2–S5 implementation in category order, with ten-modified-file batch deliveries; S6 explicitly held. |
| Objective | Resolve all 117 minimize/simplify entries in the frozen S1 register, preserving original diagnostic line mapping and necessary product contracts. |
| Non-goals | No S6 admission, guest edits/rebuild, unrelated compat audit, or speculative original-owner expansion. |
| Reference Baseline | S3 closure; package f3524088-d71f-4db1-bcbc-5b26e3a3aa0d; stable T483-D IDs and pinned originals. |
| Files And ABI Surface | The 117 S4 entries and their coupled declarations/callers/tests; no unrelated source changes. |
| Applicable Rules | Source policy, mirror subset/line rules, original contracts, native boundary judgments and category plan. |
| Verification | Original/current range and logical-line checks per changed file; applicable focused tests, cached full build, Full regression, publication hashes, governance and Git checks per batch. |
| Expected Markers | Every assigned ID resolved with implementation and counter-review evidence, or explicit owner-approved changed disposition; no unreported omissions. |
| Asset Needs | Existing pinned sources, build cache and maintained fixtures; preserve winnt.zip. |
| Reporting Requirements | Assigned/completed/remaining entries and files, batch distinct modified files, line evidence, source footprint, tests and publication/commit identities. |
| Stop Conditions | Do not force uncertain restoration over proven native ownership; report architectural tradeoffs or material expansion; no silent evidence-based reclassification. |
| Exit Criteria | All 117 entries disposed, necessary changes verified and delivered; scoped worktree clean; then admit S5. |
| Original Owner Request | Process S2 through S5 by their category checklist order; verify line mappings; deliver after ten modified files or exhaustion of each S; hold S6. |
| Similar-Issue Sweep | Complete all assigned ranges within each file once; inspect coupled callers and declarations without reopening unrelated retain/pending entries. |

## S4 Progress

Assigned: 117 minimize/simplify entries, in frozen register file/line order.
P1 resolves 21/117 entries (19 changes plus two verified S3 overlaps); 96 remain.
D0225 was omitted from the first ledger tally; its free-path change and test were
already included in 73a5ecebc. The reconciliation changes no source or package.
The batch modifies 18 source/test files. Cached x86/x64, Full 16 gates,
observer/NIDDB/package fixtures and real guest DPMI allocation/reallocation/free
and task-exit cleanup passed. Logical-line counter-review is complete, including
three restored cmdexec anchors. The repaired legacy DPMI runner uses the existing
physical-stage/logon-environment setup and retains its original assertions.
The complete 11-image package was published and hash-verified at both destinations.
See [P1 delivery](../../artifacts/mvdm-audit/t483-s4/batch1-delivery.json),
[source review](../../artifacts/mvdm-audit/t483-s4/batch1-source-review.json) and
[S4 ledger](../../artifacts/mvdm-audit/t483-s4/execution.json).
P2 resolves eight more entries: D0435, D0823–D0825 and D0954–D0957.
Total delivered: 29/117; 88 remain. Ten source/test files cover atomic event
consumption, direct keyboard snapshot/reset and removal of unused video declarations.
Cached x86/x64, Full 16 gates, focused input-return/BIOS/device tests and actual
keyboard/PIC 89 assertions passed. Final preprocessed line review is complete;
11-image equivalence after package UUID/PE timestamps permits Full-result reuse.
P2 was published and hash-verified at both destinations. See
[P2 delivery](../../artifacts/mvdm-audit/t483-s4/batch2-delivery.json) and
[source review](../../artifacts/mvdm-audit/t483-s4/batch2-source-review.json).
Next: D0964–D0967 in egwrtm12.c, then video.c in register order.
Use ten-modified-file batches or S exhaustion, preserving original logical lines.

## Latest S Closure

[S3 closure](../etc/evidence/m0-t483-s3-closure.md): 120/120 delivered.
Final seven mirror entries, obsolete test-consumer cleanup and private-desktop
input-test repair are complete. Cached x86/x64 build, Full 16 gates and focused
regressions passed. No user desktop unlock dependency remains in generic queue tests.

## Current Technical Baseline

Package `cf09aab7-46f9-41b4-b321-f2c273ed5beb`, complete 11 images at
assets/release and O:/winnt/system32, manifest hashes equal. Final images equal
the Full-tested P2 candidate after only package UUID and PE timestamp normalization;
this is not raw hash identity. DOS4GW/NESticle remains explicitly skipped because
its external fixture was not supplied. No guest or winnt.zip change.

## Remaining Scope

Functional recommendations and evidence gaps remain in the new register.
Modern Windows API ownership can be the correct original closure boundary;
original omissions alone do not justify restoring code. Subjective boundary
choices require owner review. Compat and product-component minimization are
later work under the
[admitted T scope](../proposals/proposal-original-source-implementation-minimization-review-001.md).

The DOS identity/wrong-waiter defect and Win95 exit-code boundary remain in
[TODO](TODO.md). Win95 restart-to-DOS belongs to the
[XP baseline-upgrade package](../proposals/proposal-windows-xp-ntvdm-baseline-upgrade-001.md).

The owner authorizes submission of the pending queue reorder and its Win203
proposal together. This documentation-only delivery does not change S4 scope.

## Recent M0 Closures

- [T483 S3](../etc/evidence/m0-t483-s3-closure.md): redundant additions and coupled tests removed; isolated input verification repaired.
- [T483 S2](../etc/evidence/m0-t483-s2-closure.md): original restoration and verified delivery.
- [T483 S1](../etc/evidence/m0-t483-s1-closure.md): mirror audit, bounded cleanup and verified delivery.
- [T482](../history/m0-t482-closure.md): investigation transferred, not repaired.
- [T481](../history/m0-t481-closure.md): guest multitask delivery with explicit boundaries.

## Recent Governance

See the [execution rules](../rules/EXECUTION.md). S3 preserved the parallel queue/proposal changes; their subsequent submission
is now explicitly authorized by the owner.
