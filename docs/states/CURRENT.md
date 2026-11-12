# Project Status

## Current Work

**Active: M0 T483 S5 — remaining mirror differences, original-first re-audit.**

Owner supersedes the category-only continuation with one ordered re-audit of
all remaining differences, including formerly retained items. S2–S4 and the
completed P1/P2 items remain closed. No separate S6 admission is made.
The [new worklist](../../artifacts/mvdm-audit/t483-remaining-reaudit/README.md)
is the sole active audit order; the old D-register is historical evidence.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T483 S5, Ordinary Mode; one agent, initial and counter-review passes. |
| Admission And Approval | Owner authorizes remaining-diff re-audit in exact new-list order, direct original restoration, and exact original restoration; alternatives go to the pending decision backlog while ordered audit continues. |
| Objective | For each remaining point prove why original restoration would fail in an actual supported context; restore original where appropriate, otherwise minimize necessary differences. |
| Non-goals | Do not reopen completed points or deleted files; no guest edits/rebuild, unrelated compat audit, or automatic retention from old classifications. |
| Reference Baseline | Source4cf4e5a35 / published3ac24c6a7 package; new R identifiers link old D records and pinned current file bytes. |
| Files And ABI Surface | 2720 pending points across365 existing files;1052 live host C/H mirrors. Dependencies may be read without advancing later points. |
| Applicable Rules | Source policy, mirror subset/line rules, original contracts, native boundary judgments and category plan. |
| Verification | Original/current range and logical-line checks per changed file; applicable focused tests, cached full build, Full regression, publication hashes, governance and Git checks per batch. |
| Expected Markers | Per-point current ranges, actual call/build context, concrete original-failure evidence, minimal option, original-relative diff delta, line proof and delivery status. |
| Asset Needs | Existing pinned sources, build cache and maintained fixtures; preserve winnt.zip. |
| Reporting Requirements | New-list position, completed/pending/approval-needed counts, current point conclusion, source/mirror diff delta and verified delivery. |
| Stop Conditions | If the proposed disposition preserves a diff unchanged or increases it, record the decision and continue; obtain owner approval before implementation. Unproved risk is not retention evidence. |
| Exit Criteria | Every admitted remaining point has an owner-compliant evidenced disposition and applicable implementation/delivery; list initialization alone is not audit completion. |
| Original Owner Request | Re-audit all remaining categories with presumption of original validity; exact list order; restore directly where justified; ask before unchanged or larger diff. |
| Similar-Issue Sweep | Read coupled callers as evidence, finish each point in list order, and do not reopen previously completed points. Retire vanished points with byte/range proof. |

## S5 Progress

The [ordered register](../../artifacts/mvdm-audit/t483-remaining-reaudit/review-register.csv)
contains 2720 points in 365 files. Reviewed 85: 22 restored and verified,
2 owner-approved retain, 61 pending decisions (including partial restorations);
2635 unreviewed. Continue at R0086. Decision-required items go to the
[pending backlog](../../artifacts/mvdm-audit/t483-remaining-reaudit/pending-decisions.csv)
without blocking the next ordered review.

P3 restores original code in ten implementation files, reducing normalized
original-relative mirror differences by 126 lines. Excluded #error sequences
are unchanged. Per-point records distinguish source analysis from executed tests.
The [P3 receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p3-checkpoint.json)
records cached x86/x64 builds, two command fixtures, Full16, documentation
checks, and publication of the same 11-image package to assets/release and
O:/winnt/system32. Both destination hashes match the tested candidate.
DOS4GW/NESticle is explicitly skipped: external fixtures were not supplied.
Git delivery is being finalized; S5 remains active, not closed.

Pending DTA/PDB/SFT decisions do not establish that original pointer lifetimes
are defective. The fast-I/O decline has original guest CF fallback evidence;
its retention remains an owner decision. No guest rebuild or winnt.zip change.

## Current Technical Baseline

Package `0db3c11e-0a6e-4e0b-b2b0-ce921eaf5b1b`, complete 11 images at
assets/release and O:/winnt/system32, raw-hash identical to the Full-tested S5 P3
candidate and manifest. See the [P3 receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p3-checkpoint.json).
DOS4GW/NESticle remains explicitly skipped because its external fixture was not
supplied. No guest or winnt.zip change.

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

The approved queue reorder and Win203 proposal were delivered in `0fbbe5768`;
no queue changes remain pending in this batch.

## Recent M0 Closures

- [T483 S4](../etc/evidence/m0-t483-s4-closure.md): 117 minimize/consolidate entries verified and delivered.

- [T483 S3](../etc/evidence/m0-t483-s3-closure.md): redundant additions and coupled tests removed; isolated input verification repaired.
- [T483 S2](../etc/evidence/m0-t483-s2-closure.md): original restoration and verified delivery.
- [T483 S1](../etc/evidence/m0-t483-s1-closure.md): mirror audit, bounded cleanup and verified delivery.
- [T482](../history/m0-t482-closure.md): investigation transferred, not repaired.
- [T481](../history/m0-t481-closure.md): guest multitask delivery with explicit boundaries.

## Recent Governance

See the [execution rules](../rules/EXECUTION.md). Queue/proposal delivery is
complete; S5 proceeds under the owner-approved S2–S5 sequence.
