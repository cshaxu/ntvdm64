# Project Status

## Current Work

**Active: M0 T483 S3 — delete redundant local mirror additions.**

The owner authorized S1 closure and has now approved the internal T483 sequence:
S2 restore original (76), S3 delete (120), S4 minimize/consolidate (117),
S5 correct (207), S6 resolve missing evidence (755). See the
[category plan](../etc/operations/m0-t483-mirror-register.md#owner-approved-s2s6-remediation-sequence).
Owner now authorizes sequential execution and closure through S5. S2 is closed; S3 is active;
S4–S5 follow only after preceding closure. S6 must not be admitted without a new
owner instruction.
The previously suggested compat-first S2 is superseded.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T483 S3, Ordinary Mode; one agent, initial and counter-review passes. |
| Admission And Approval | Owner authorizes S2–S5 implementation in category order, with ten-modified-file batch deliveries; S6 explicitly held. |
| Objective | Resolve all 120 delete entries in the frozen S1 register, preserving original diagnostic line mapping and necessary product contracts. |
| Non-goals | No S6 admission, guest edits/rebuild, unrelated compat audit, or speculative original-owner expansion. |
| Reference Baseline | S2 P3 closure; package fd1f2722-a425-4881-97f6-174be44d6b6f; stable T483-D IDs and pinned originals. |
| Files And ABI Surface | The 120 S3 entries and their coupled declarations/callers/tests; no unrelated source changes. |
| Applicable Rules | Source policy, mirror subset/line rules, original contracts, native boundary judgments and category plan. |
| Verification | Original/current range and logical-line checks per changed file; applicable focused tests, cached full build, Full regression, publication hashes, governance and Git checks per batch. |
| Expected Markers | Every assigned ID resolved with implementation and counter-review evidence, or explicit owner-approved changed disposition; no unreported omissions. |
| Asset Needs | Existing pinned sources, build cache and maintained fixtures; preserve winnt.zip. |
| Reporting Requirements | Assigned/completed/remaining entries and files, batch distinct modified files, line evidence, source footprint, tests and publication/commit identities. |
| Stop Conditions | Do not force uncertain restoration over proven native ownership; report architectural tradeoffs or material expansion; no silent evidence-based reclassification. |
| Exit Criteria | All 120 entries disposed, necessary changes verified and delivered; scoped worktree clean; then admit S4. |
| Original Owner Request | Process S2 through S5 by their category checklist order; verify line mappings; deliver after ten modified files or exhaustion of each S; hold S6. |
| Similar-Issue Sweep | Complete all assigned ranges within each file once; inspect coupled callers and declarations without reopening unrelated retain/pending entries. |

## S3 Progress

Assigned: 120 entries across 51 mirror files, in the existing register order.
P1–P5 delivered 99/120 entries; 21 remain. P6 completes coupled callback-frame
provider/declaration cleanup and six unused integration runner settings (ten
modified source/test files, no new register IDs or mirror edits). Functional
provider bodies and runner failure/acceptance assertions remain unchanged.
Cached x86/x64 builds, original callback/abort checks, script parsing and all
16 selected Full gates passed. WOW32 changed beyond timestamps, so Full was
rerun rather than reused. Complete package is published and hash-verified at
both destinations. See [P6 delivery](../../artifacts/mvdm-audit/t483-s3/batch6-delivery.json)
and [review](../../artifacts/mvdm-audit/t483-s3/batch6-review.json).
Remaining old diagnostic consumers, including Shell task-identity and window
destruction witnesses, must be reconciled before S3 closure. Next mirror file:
wow32/wkfileio.c. No claim that those old diagnostic runners currently pass.
Shared providers remain until their
last required caller is removed, then are cleaned in the same S.
See [S3 ledger](../../artifacts/mvdm-audit/t483-s3/execution.json).
Batch delivery after ten distinct modified files or S exhaustion.

## Latest S Closure

[S2 closure](../etc/evidence/m0-t483-s2-closure.md): 76/76 resolved (75 implemented,
one explicit corrected-retain classification). Cached builds, focused checks,
all 16 selected Full gates and both publication destinations verified per batch.
See [final delivery](../../artifacts/mvdm-audit/t483-s2/batch3-delivery.json).
S1 register and identifiers remain immutable; S6 remains held.

## Current Technical Baseline

Package `9e1219c1-885a-4402-ba36-edead29282f8` (S3 P6), complete 11 images at
assets/release and O:/winnt/system32, manifest hashes equal. DOS4GW/NESticle was
explicitly skipped because its external fixture was not supplied. No guest or
winnt.zip change. This package is not claimed byte-equivalent to S2.

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

Parallel-session QUEUE.md edits and the win203 setup proposal are preserved and
excluded from S1 delivery.

## Recent M0 Closures

- [T483 S2](../etc/evidence/m0-t483-s2-closure.md): original restoration and verified delivery.
- [T483 S1](../etc/evidence/m0-t483-s1-closure.md): mirror audit, bounded cleanup and verified delivery.
- [T482](../history/m0-t482-closure.md): investigation transferred, not repaired.
- [T481](../history/m0-t481-closure.md): guest multitask delivery with explicit boundaries.

## Recent Governance

See the [execution rules](../rules/EXECUTION.md). S2 closure preserves the
parallel-session queue and proposal changes.
