# Project Status

## Current Work

**Active: M0 T483 S5 — correct registered mirror deviations.**

The owner authorized S1 closure and has now approved the internal T483 sequence:
S2 restore original (76), S3 delete (120), S4 minimize/consolidate (117),
S5 correct (207), S6 resolve missing evidence (755). See the
[category plan](../etc/operations/m0-t483-mirror-register.md#owner-approved-s2s6-remediation-sequence).
Owner now authorizes sequential execution and closure through S5. S2, S3 and S4 are closed; S5 is active. S6 must not be admitted without a new
owner instruction.
The previously suggested compat-first S2 is superseded.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T483 S5, Ordinary Mode; one agent, initial and counter-review passes. |
| Admission And Approval | Owner authorizes S2–S5 implementation in category order, with ten-modified-file batch deliveries; S6 explicitly held. |
| Objective | Resolve all 207 correct entries in the frozen S1 register, checking current code and prior deliveries before each correction; preserve original logical lines and necessary product contracts. |
| Non-goals | No S6 admission, guest edits/rebuild, unrelated compat audit, or speculative original-owner expansion. |
| Reference Baseline | S4 closure and P9 Full-tested package; stable T483-D IDs and pinned originals. |
| Files And ABI Surface | The 207 S5 entries across 167 mirror files and their coupled declarations/callers/tests; no unrelated source changes. |
| Applicable Rules | Source policy, mirror subset/line rules, original contracts, native boundary judgments and category plan. |
| Verification | Original/current range and logical-line checks per changed file; applicable focused tests, cached full build, Full regression, publication hashes, governance and Git checks per batch. |
| Expected Markers | Every assigned ID resolved with implementation and counter-review evidence, or explicit owner-approved changed disposition; no unreported omissions. |
| Asset Needs | Existing pinned sources, build cache and maintained fixtures; preserve winnt.zip. |
| Reporting Requirements | Assigned/completed/remaining entries and files, batch distinct modified files, line evidence, source footprint, tests and publication/commit identities. |
| Stop Conditions | Do not force uncertain restoration over proven native ownership; report architectural tradeoffs or material expansion; no silent evidence-based reclassification. |
| Exit Criteria | All 207 entries disposed with specified positive/negative proof, necessary changes delivered and worktree clean; S6 remains held. |
| Original Owner Request | Process S2 through S5 by their category checklist order; verify line mappings; deliver after ten modified files or exhaustion of each S; hold S6. |
| Similar-Issue Sweep | Complete all assigned ranges within each file once; inspect coupled callers and declarations without reopening unrelated retain/pending entries. |

## S5 Progress

S5 P2 is built, regression-verified and published. See the
[P2 receipt](../../artifacts/mvdm-audit/t483-s5/p2-delivery.json).
Six of207 assigned entries are complete;201 remain in frozen file/line order.
P1 completed D0041/D0042/D0043/D0048. P2 completes D0124/D0127 by
restoring the original pExtendedError declaration, definition, DS:CX
registration and four field writes across four mirror files. This removes
36 original-relative diff lines; DTA/PDB/SFT adaptations remain unchanged.
Restored statements exactly match original source and logical line numbers.

The thirteen cmdmisc terminal-error repairs are also delivered, with zero
original-relative diff growth and unchanged physical lines/#line directives.
They are not counted complete: dedicated fault injection and previous-request
redirection cleanup verification remain pending in the
[review](../../artifacts/mvdm-audit/t483-s5/cmdmisc-redesign.json).
Cached x86/x64 build,140 command-capture assertions,65 launch assertions and
all16 Full gates passed. Full is not exhaustive DOSDATA remapping proof.
DOS4GW/NESticle remains skipped because external fixtures are absent.
Nineteen earlier growing candidates remain withdrawn. The rejected five-line
extended-error proposal was not implemented; its historical
[review](../../artifacts/mvdm-audit/t483-s5/demhndl-redesign.json) is superseded.
Every positive original-relative mirror diff needs prior approval per repair.
S6 remains held; original guest and winnt.zip are unchanged.

[S4 closure](../etc/evidence/m0-t483-s4-closure.md): 117/117 delivered in P1-P9.
The final eight restore source positions and simplify selectors/declarations.
Nine implementation files net -36 lines. Cached x86/x64, control-boundary cohort,
class-lifetime, original-line review and Full16 passed. See
[P9 delivery](../../artifacts/mvdm-audit/t483-s4/batch9-delivery.json).

The [dynamic register summary](../../artifacts/mvdm-audit/t483-s1/classification-summary.md)
is reconciled through P2:3042 historical/current IDs,321 completed dispositions,
956 outstanding actions/evidence items; retain is not a repair backlog.
Current category counts differ from frozen S allocations after approved reclassification.

## Current Technical Baseline

Package `36dc35b7-1e69-4617-b1ed-d2662418a07c`, complete 11 images at
assets/release and O:/winnt/system32, raw-hash identical to the Full-tested S5 P2
candidate and manifest. See the [P2 receipt](../../artifacts/mvdm-audit/t483-s5/p2-delivery.json).
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
