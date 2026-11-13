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
| Reporting Requirements | File start/end and point start/end reports: list position, path/IDs, original-relative diff scope/delta, dispositions, separate file/point counts and build/publication/test/commit/push state. |
| Stop Conditions | If the proposed disposition preserves a diff unchanged or increases it, record the decision and continue; obtain owner approval before implementation. Unproved risk is not retention evidence. |
| Exit Criteria | Every admitted remaining point has an owner-compliant evidenced disposition and applicable implementation/delivery; list initialization alone is not audit completion. |
| Original Owner Request | Re-audit all remaining categories with presumption of original validity; exact list order; restore directly where justified; ask before unchanged or larger diff. |
| Similar-Issue Sweep | Read coupled callers as evidence, finish each point in list order, and do not reopen previously completed points. Retire vanished points with byte/range proof. |

## S5 Progress

The [ordered register](../../artifacts/mvdm-audit/t483-remaining-reaudit/review-register.csv)
contains2720 points in365 files, with1052 live host C/H mirror files.
177 points have a prior or current review record:103 restored/verified/published,
0 local restorations pending verification,2 approved retain,19 qualified pending
decisions and53 historical classifications requiring requalification. Another2543
points are unreviewed. Sequential file disposition is15/365;350 remain.
Coupled edits in later files do not count as completed sequential file audits.
These are point counts, not completed-file counts or reproduced defect counts.

Owner-directed backlog requalification started at R0010 (restored and delivered).
Current file cmdexec.c has had its remaining register points handled: R0014-R0020
restore original SCS construction; R0022/R0026/R0028 restore original ordinary
text conversion and ownership. Capture/capacity alternatives remain owner choices;
no unapproved alternative has been implemented. R0171 remains unexamined.

The [pending backlog](../../artifacts/mvdm-audit/t483-remaining-reaudit/pending-decisions.csv)
contains only unresolved portions of these points. Older entries are explicitly
unqualified historical investigations, not established necessity. A concrete
source/ABI failure or observed failure is required for decision admission.
The current concurrency evidence is a source-level eligible ordering counterexample,
not a reproduced runtime race; it does not prove every capture helper necessary.

Final minimized P6 source passed cached full build in build/t483-s5-p6-r2.
Original-relative cmdexec.c difference falls from245 to123 lines (net -122), normalizing newlines and excluding #line.
This includes SCS restoration, conversion restoration, activation-wrapper removal
and COMSPEC minimization. These are mirror diff counts, not total source lines.
Matched original-code runs have zero logical-line mismatches; #error is unchanged.
Final command fixtures and Full16 passed; DOS4GW/NESticle was explicitly skipped
for absent external fixtures. Both publication destinations match all11 hashes.
This receipt accompanies P6; Git commit/push verification follows its creation.
See [P6 checkpoint](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p6-checkpoint.json),
[scheduling proof](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0021-scheduling-requalification.json),
[conversion restoration](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0022-r0028-conversion-restoration.json),
and [remaining decisions](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0029-r0036-requalification.json).

## Current Technical Baseline

P10 package `27754767-b670-45f2-b8b7-bb626760161f` is published to
assets/release and O:/winnt/system32; all11 image hashes match at both targets.
Cached full build, command fixtures, FCB enumeration, Full16 and governance
passed. DOS4GW/NESticle skipped: verified external fixtures not supplied.
No original guest or winnt.zip changes. This record accompanies P10 Git delivery.
See [P10 receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p10-checkpoint.json).
S5 remains active; next ordered point is R0076.

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

## Current Owner Execution Rules

Review in list order, beginning with historical pending items. Restore original
where supported; speculative races and current-adapter dependencies alone do not
prove necessity. Necessary non-original alternatives remain pending owner choice.
Synchronize the register and backlog, removing restored portions promptly.
Report each point's ID, added/deleted scope, original rationale, evidence,
disposition and cumulative counts; distinguish source proof from runtime results.

There is no numeric file threshold. Pure formatting/newline/reference changes
without behavioral or binding effects may accumulate. A semantic restoration
triggers cached full build, publication, tests, commit and push of the accumulated
batch only after all outstanding points in the current file and coupled edits
have been handled. Never build repeatedly for partially handled points in a file.
Unresolved investigation is not file completion. Source-qualified owner decisions
are not approved retention or implemented repairs. S5 remains active.

## Owner refinement: same-file minimization

Implement supported necessary-diff reductions directly; another original
restoration in that file is not a prerequisite.
Net increases still need prior approval. Minimization does not approve residual
differences: keep them in the pending backlog with current evidence and size.
Only fully restored points leave that list.

R0029 removes activate and restores original cmdExec32 assignments; two captured
pointer callsites remain pending. The prior P6 candidate passed Full, but does
not validate these newer edits. [Evidence](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0029-minimization.json).

The owner's consolidated [17 execution rules](../rules/EXECUTION.md#original-first-mirror-diff-audit)
now govern this packet, superseding earlier minimization-approval restrictions.
Residual minimized differences remain pending; final-code verification is required.

## Current P9 Batch

P8 `8d4f8a5ef` was pushed and remote-verified with a clean worktree.
P9 handles dem.c comment-only minimization and dem.h original-first review,
including the coupled DTA declaration/definition/registration/search/FCB chain.
R0067/R0068 remain qualified package-directory decisions; R0070 remains an
explicit type dependency for unchanged closed-scope PDB/SFT declarations.
R0069 restores the actual WIN_32 ntdddisk.h include. Its first probe hit the
commented preamble and was invalid; the corrected r2 probe passes and logs
the actual SDK header. R0071 restores the original DTA cohort, including
original Sim32FlushVDMPointer/Sim32FreeVDMPointer calls. No guest changes.

Six mirror files now reduce original-relative diff by85 lines, excluding
#line and normalizing newlines. demfcb.c has no remaining normalized diff.
All6 matched-original code-line checks pass and all #error bodies are unchanged.
The first P9 candidate built but the publication guard rejected a logical-line
mismatch; it was not published or runtime-tested. Corrected r2 passed cached
full build, both-location publication, Full16 and the actual FCB enumeration
probe. Package `7e6012d6-3ad4-45e8-a7ba-44e775b12e55` has11 matching images in
assets/release and O:\winnt\system32. DOS4GW/NESticle remains explicitly skipped
without external fixtures. FCB record read/write and remapping are not claimed
covered by the enumeration probe. The runner now uses the existing owned short
physical stage instead of SUBST; its authored COM uses the working directory.

P9 source, tests, release artifacts, register and evidence are one Git delivery.
Progress:13/365 sequential files complete;2720 points:89 restored and verified,
0 local restores pending,2 approved retains,19 qualified decisions,67 historical
backlog items requiring requalification,2543 unreviewed. Later coupled edits do
not count as sequential file completion. S5 remains active; next point R0072.

See [P9 checkpoint](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p9-checkpoint.json),
[line check](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p9-line-check.json),
[diff metrics](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p9-mirror-diff.json),
and [DTA restoration](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0071-dta-restoration.json).

## Current P10 Batch

R0072 demdasd.h is still byte-identical to original; prior P3 verification applies.
R0073 is already delivered in P9. R0074/R0075 restore the complete original PDB/SFT
producer and consumer chain. demdata.c now matches original excluding #line and
newline encoding. Seven coupled mirror files reduce original-relative diff by143
lines, excluding #line. All excluded #error bodies remain. No new shim or guest
changes. The dem.h guest-location include remains for existing demhndl.c leases;
PDB/SFT globals no longer require it. This supersedes the old include rationale.

P10 cached build passed. Initial publication hit an exiting ntvdm image lock;
resume24112 published the same frozen package27754767-b670-45f2-b8b7-bb626760161f
to both targets with11 matching hashes. Final Full16 and FCB enumeration passed in build/t483-s5-p10. Fourteen formerly
pending points are restored and verified, included in P10 Git delivery. Future
coupled-file points are not counted as completed sequential audits.
See [P10 restoration](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0074-r0075-restoration.json),
[checkpoint](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p10-checkpoint.json),
[metrics](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p10-mirror-diff.json),
and [line checks](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p10-line-check.json).
