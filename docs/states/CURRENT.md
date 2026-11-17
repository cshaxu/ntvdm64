# Project Status

## Current Work

**Active: M0 T483 S5 — remaining mirror differences, original-first re-audit.**

Owner supersedes the category-only continuation with one ordered re-audit of
all remaining differences, including formerly retained items. S2–S4 and the
completed P1/P2 items remain closed. Owner now authorizes S5 closure after P255 and immediate S6 admission.
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
| Reporting Requirements | File start/end and point start/end reports: list position, path/IDs, original-relative diff scope/delta, dispositions, separate file/point counts and build/publication/test/commit/push state. |
| Stop Conditions | If the proposed disposition preserves a diff unchanged or increases it, record the decision and continue; obtain owner approval before implementation. Unproved risk is not retention evidence. |
| Exit Criteria | Every admitted remaining point has an owner-compliant evidenced disposition and applicable implementation/delivery; list initialization alone is not audit completion. |
| Original Owner Request | Re-audit all remaining categories with presumption of original validity; exact list order; restore directly where justified; ask before unchanged or larger diff. |
| Similar-Issue Sweep | Read coupled callers as evidence, finish each point in list order, and do not reopen previously completed points. Retire vanished points with byte/range proof. |

## S5 Progress

The [ordered register](../../artifacts/mvdm-audit/t483-remaining-reaudit/review-register.csv)
contains2720 points in365 files, with1052 live host C/H mirror files.
2720 source-reviewed:1018 restored/confirmed delivered,0 local,52 approved/policy,
1650 pending owner;0 unreviewed. Files365/365 verified;0 remaining in this pass.
P253 final seven files:2061->1249lines(-812),66->54blocks(-12).
Eight restored/confirmed points;sixteen residual owner-boundary points.
Cached full,compiler33 function anchors,publish11x2,focused37/task-order,
Full20,boundary/governance PASS. Current89379lines/4862blocks;cumulative
reduction30534lines/435blocks versus audit baseline. This is file-pass
completion,not owner-decision approval or S5/T closure.

Prior batch receipts and their scope/limitations are indexed in the ordered worklist README. Approved synchronization remains R1797 revisit.
Point counts are not completed-file or reproduced-defect counts.

Backlog requalification began at R0010. Prior cmdexec.c restorations and
R0171 residual evidence are delivered/indexed in the ordered register.

The [pending backlog](../../artifacts/mvdm-audit/t483-remaining-reaudit/pending-decisions.csv)
contains only unresolved portions of these points. All current entries have been requalified; historical speculation is not
established necessity. A concrete
source/ABI failure or observed failure is required for decision admission.
The current concurrency evidence is a source-level eligible ordering counterexample,
not a reproduced runtime race; it does not prove every capture helper necessary.

P247 Git delivery:0490f544b. P248 userk.h R2658-R2666 verified/published;
Git delivery accompanies receipt. Residual differences remain pending owner;
verification does not approve their semantics or block-growth tradeoff.
Owner-ordered complete-unit file pass is finished. Previously recorded
fragmentation and remaining owner decisions require their own next action.
ntstubs experiment withdrawn;source equals delivered HEAD.
[Fragment inventory](../../artifacts/mvdm-audit/t483-remaining-reaudit/p248-block-growth-followup.json).
[Current recheck](../../artifacts/mvdm-audit/t483-remaining-reaudit/p254-fragmentation-current-baseline.json):
9 prior block-growth files remain,ordered by the existing R register;
queue.c27->23blocks is resolved for this criterion and is not reopened.
Evidence-only P254;production/package and P253 verification identities unchanged.
Owner requests one final combined batch for the9 eligible files.
[P255 follow-up](../../artifacts/mvdm-audit/t483-remaining-reaudit/p255-fragmentation-batch.json):
all9 source-reviewed;three files restore6 original blank lines,5blocks removed.
Nonempty source/logical lines and#error unchanged;cached/publish11x2,
focused37/task-order and Full20 pass. Final governance/Git delivery pending.
Residual owner decisions remain open and transfer to S6 after this delivery.
[S5 closure](../etc/evidence/m0-t483-s5-closure.md) records the owner decision.

## Current Technical Baseline

P255 package `87c73097-d004-454e-925b-fea9a16f9552` published11x2;hash verified.
Final seven-file whole-unit restoration and test closure integration delivered.
No new production shim;residual boundaries remain pending owner.
Cached x86/x64,compiler33 anchors,focused37/task-order,Full20 and governance pass.
First Full version-negative1740 failed;separate isolated rerun passed.
Guest and winnt.zip unchanged. [Receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p253-checkpoint.json).
P252 Git:5d6f81242. P253 Git:8345a60c4;ordered pass finished.

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

- [T483 S4](../etc/evidence/m0-t483-s4-closure.md):prior minimization batch.
- [T483 S5 closure record](../etc/evidence/m0-t483-s5-closure.md):P255 final delivery in progress;residual decisions transfer to S6 after push.

Older P receipts remain indexed in the [S5 worklist](../../artifacts/mvdm-audit/t483-remaining-reaudit/README.md);they are historical evidence,not the current packet.

## Recent Governance

See [execution rules](../rules/EXECUTION.md). P255 final delivery and owner-approved S5 closure precede the newly authorized S6 packet. No T queue reorder.

## Current Owner Execution Rules

Original validity is presumed. Require concrete full-call-chain evidence for
retention;adapter dependencies alone are not proof. Synchronize main and pending
registers;only full restoration retires a point. Report file/point progress,
original-relative blocks and added+deleted lines,including cumulative reductions.
Complete each file before substantive delivery;do not remove harmless original
code or transfer complexity into shims to shrink counts. Preserve#error and
logical lines. Owner overrides the batch cadence for P255:all9 files together,
then cached build,publish,test,commit,push. S6 changes file order to descending
non-error difference blocks;other evidence and approval rules persist.
