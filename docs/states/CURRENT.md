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
210 points have a prior or current review record:124 restored/verified/published,
0 local restorations pending verification,2 approved retain,84 qualified pending
decisions and0 prior classifications requiring requalification. Another2510
points are unreviewed. Sequential file disposition is30/365;335 remain.
Coupled later-file edits do not complete those file audits.
Point counts are not completed-file or reproduced-defect counts.

Owner-directed backlog requalification started at R0010 (restored and delivered).
Earlier cmdexec.c delivery handled its remaining register points: R0014-R0020
restore original SCS construction; R0022/R0026/R0028 restore original ordinary
text conversion and ownership. Capture/capacity alternatives remain owner choices;
no unapproved alternative has been implemented. R0171 has source-qualified residual evidence recorded in P17.

The [pending backlog](../../artifacts/mvdm-audit/t483-remaining-reaudit/pending-decisions.csv)
contains only unresolved portions of these points. All current entries have been requalified; historical speculation is not
established necessity. A concrete
source/ABI failure or observed failure is required for decision admission.
The current concurrency evidence is a source-level eligible ordering counterexample,
not a reproduced runtime race; it does not prove every capture helper necessary.

P6 cmdexec.c delivery reduced mirror diff245->123. Final build, publication11x2,
command fixtures and Full16 passed; evidence is in the
[P6 checkpoint](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p6-checkpoint.json).

## Current Technical Baseline

P20 package `79ae5698-34e1-49b2-811e-f414e0905f87` is published to
assets/release and O:/winnt/system32; all11 image hashes match at both targets.
Final-source cached full build and current-source observer Full16 passed.
DOS4GW/NESticle remains skipped for absent external fixtures. No original guest
or winnt.zip changes. See [P20 receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p20-checkpoint.json).
S5 remains active; demmisc.c whole-file verification found only the retained DBG
#error exclusion and diagnostic #line directives; no source change or new restored
point. See [file check](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0114-r0116-whole-file-check.json).
demsrch.c dispositions complete; P14 restores three exact FETCHWORD expressions
(net mirror diff -7: six expression lines plus one extra blank), correcting earlier
equivalent-but-not-exact restoration claims. P14 was delivered in `4820f90b4`;
its [receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p14-checkpoint.json) retains the verification.
[Requalification](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0117-r0133-requalification.json).
data.c R0134-R0136 is source-complete: required guest-address/table publication
remains pending owner choice; IDT minimum-size comparison shrinks mirror diff by3.
P15 final build/publication/tests passed; this receipt accompanies Git delivery.
[Requalification](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0134-r0136-requalification.json).
dpmi32.c R0137-R0139 audit corrected the CVIDC wrapper/prototype attribution;
comment-only cleanup reduces mirror diff by4, with executable tokens unchanged.
P15 remains the build/publication/runtime baseline; no fresh runtime check claimed.
[Review](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0137-r0139-requalification.json).
dpmi32p.h R0140-R0142 is requalified against original guest/kernel and CCPU
address domains; mirror bytes unchanged. Removed one identical redundant
autonomous function declaration without behavior/binding change.
[Header review](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0140-r0142-requalification.json).
dpmidata.h R0143-R0146 source review is complete; mirror diff60 to50, net-10.
Duplicate conditional and explicit prototype removed; original macro-expanded
prototype remains. Two points are restored/verified/published, two residual
boundaries remain pending owner decision.
[Review](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0143-r0146-requalification.json).
P16 cached build, publication11x2, descriptor2097152, Full16 and governance
passed; this receipt accompanies Git delivery. dpmiint.c whole-file disposition is complete in P17. R0147 remains restored; R0148
is source-qualified. Actual DOSX binary identity/build rules select486/dxintr.asm,
so earlier top-level guest lock-state citations require correction. R0149-R0151
and coupled PM-address necessity were re-examined; the restoration candidate below
supersedes that investigation checkpoint.
[Source attribution](../../artifacts/mvdm-audit/t483-remaining-reaudit/dpmiint-guest-source-attribution.json).
That read-only checkpoint is followed by the active local restoration below.

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

Delivered in275d6e029: original DTA chain and ntdddisk include, mirror diff net-85.
Cached build, both-location publication, Full16 and FCB enumeration passed.
See [P9 receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p9-checkpoint.json).

## Current P10 Batch

Delivered in8a0a55ff3: original PDB/SFT chain, mirror diff net-143;14 points restored.
Cached build, both-location publication, Full16 and FCB enumeration passed.
See [P10 receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p10-checkpoint.json).

## Current P11 Batch

Delivered in858af5e7c: original software-CPU slow-I/O selection makes the added
fast-I/O fallback redundant. R0076-R0078 restored; mirror diff net-15. Initial
retention proof was withdrawn before publication after upstream-gate review.
Final build/publication/Full16 passed. Format-only final blank-line restoration
preserves nonblank content and logical lines. See
[P11 receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p11-checkpoint.json).

## Delivered P12 Audit And Format Batch

Original-byte/line checks and the R0098 label proof are retained in the
[P12 checkpoint](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p12-checkpoint.json).
Its nonbehavioral changes reused P11 runtime evidence; no new guest pass claimed.

## Current P13 Batch

Complete-file demhndl.c review R0102-R0113 restores two original ULONG declarations
and removes redundant read-lease active state. Original-relative diff drops from
+53/-37 to+34/-35, net-21 (newline normalized, #line excluded). Both excluded
#error directives remain; original matched selected-source logical lines agree.
The original SIM32 contiguous-pointer boundary fails a real CCPU low-page mapping
fixture; its source/fixture proof qualifies the remaining ten buffer adaptations
for owner decision, not retention approval or a production-application repro.
See [P13 review](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0102-r0113-requalification.json)
and [checkpoint](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p13-checkpoint.json).
Final-source cached build, both-location publication, focused tests, Full16
and governance passed; this receipt accompanies P13 Git delivery. Residual
buffer differences remain owner decisions, not approved retention.

## P17 Completed File Batch

Whole-file dpmiint.c review R0147-R0179 and coupled PM projection cleanup
reduce mirror diff by93 lines: dpmiint.c379->293, data.c56->50,
dpmi32p.h37->36. Excluded#error bodies remain; original line evidence is
[recorded](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p17-line-check.json).
Five points are restored; partial reductions remain owner decisions.
Cached full build, both-location publication11x2, DPMI selected-object tests,
CCPU143, descriptor tests, Full16 and governance passed. This record accompanies
Git delivery. Package `6b318f5f-3c92-4572-a9e7-f850066b3ec4` is installed.
[Final receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p17-checkpoint.json).
No original guest or winnt.zip changes; unavailable DOS4GW/NESticle fixtures
are skips, not passes. No fresh Win3.x interactive acceptance is claimed.
Next sequential file: dpmimscr.c, starting R0180.

## P18 Original-Restoration Delivery

dpmimscr.c R0180-R0182 is byte-identical to original; mirror diff net-10.
NoFastWow, slow BOP and 04F2 descriptor fallback are the original providers.
Cached full build, publication11x2 and Full16 passed; this record accompanies
Git delivery. WOW frontier coverage is not gameplay acceptance; absent
DOS4GW/NESticle fixtures are skipped. No original guest or winnt.zip changes.
[Receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p18-checkpoint.json).
Next sequential file is dpmiselr.c, R0183.

## P19 Delivered Baseline

P19 removed obsolete fast-BOP adaptation; six mirrors net-195 diff lines.
Cached build, publication11x2, focused, Full16 and supplemental gates passed.
Commit41569ec4a is pushed; [receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p19-checkpoint.json).

## P20 Verified Batch

int21map.c R0187-R0203: R0202 restored;16 residuals await owner decision.
Mirror diff159->139 (-20); executable tokens/#error unchanged; lines checked.
Cached build, publication11x2, current-source observer Full16 and3 handoff
repeats passed. First historical-observer handoff failure is retained, not
claimed as a repaired product bug. Final governance/Git follows.
[Receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p20-checkpoint.json).
Next file: modesw.c R0204.
