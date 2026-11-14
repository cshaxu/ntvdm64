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
1176 points reviewed:428 restored delivered,0 local,8 approved,
740 qualified pending,0 restores decided;1544 unreviewed. Files148/365 audited,217 remain.
Coupled later-file edits do not complete those file audits.
Point counts are not completed-file or reproduced-defect counts.

Backlog requalification began at R0010. Prior cmdexec.c restorations and
R0171 residual evidence are delivered/indexed in the ordered register.

The [pending backlog](../../artifacts/mvdm-audit/t483-remaining-reaudit/pending-decisions.csv)
contains only unresolved portions of these points. All current entries have been requalified; historical speculation is not
established necessity. A concrete
source/ABI failure or observed failure is required for decision admission.
The current concurrency evidence is a source-level eligible ordering counterexample,
not a reproduced runtime race; it does not prove every capture helper necessary.

## Current Technical Baseline

P103 package `f5194129-5529-41d2-867a-4e125354b05a` published11x2; hashes match.
Cached full x86/x64, Full19, CVID, WOW boundary143/private header9 and governance passed.
[Receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p103-checkpoint.json).
No guest or winnt.zip changes; absent external fixtures remain skips.
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

P102 nt_hosts.c whole-file changes delivered: mirror86->56(-30);four originals restored,
one QuickEdit residual. Actual preprocessing matches121 original selected lines,
zero line mismatches;cached full build,publish11x2,Full19 passed.
[Review](../../artifacts/mvdm-audit/t483-remaining-reaudit/r1146-r1150-nt-hosts-review.json).

P103 nt_inthk.c original hook restored and delivered;mirror326->22(-304),only7 excluded
branches remain. Coupled c_main monitor-only entrypoints removed(-25). Cached full
build,publish11x2,focused/Full19 passed;Git delivery accompanies receipt.
[Review](../../artifacts/mvdm-audit/t483-remaining-reaudit/r1151-r1169-nt-inthk-review.json).

P104 nt_keycd.c reviewed;mirror6->4(-2),comment-only;P103 binaries unchanged.
[Review](../../artifacts/mvdm-audit/t483-remaining-reaudit/r1170-r1172-nt-keycd-review.json).

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

P9-P13 delivered; per-batch receipts remain indexed in the active audit README.

## P17 Completed File Batch

P17 delivered the whole-file DPMI restoration and paired address cleanup.
[Receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p17-checkpoint.json)
retains mirror deltas, line proof, publication and tests; unavailable external
fixtures remain skips. No fresh Win3.x interactive acceptance was claimed.

P18 delivered;[receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p18-checkpoint.json).

P47-P48 delivered;receipts remain indexed in the active audit README.

P49-P51 delivered;see ordered register and batch receipts.
R0483 manifest omission pending;[evidence](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0483-sources-review.json).
P52:R0484-R0487 restored;mirror diff net-8;full delivery verified.
[Review](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0484-r0487-zfrsrvd-review.json).
P54:ev_glue.c R0489-R0494 complete;mirror diff54->25(net-29).
Two restorations delivered;four residuals await owner.
[Evidence](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0489-r0494-ev-glue-review.json).

P55:R0495 original header restored;mirror diff net-30;local controls pass.
P55 five-file batch net-65;sevid00070/70 disposition complete.
Cached build,11x2 publication,focused and Full16 passed;Git delivery follows.
[Evidence](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0495-evidfunc-header-review.json).

[P55 accessor evidence](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0496-r0499-evidgen-review.json).

P55 R0500 original restored;[evidence](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0500-j-c-lang-review.json).

P55 R0501 restored;35 consumers/4722 object sections equal;BIGEND line repaired.
[Evidence](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0501-j-c-header-review.json).

R0502:16-case address control;[evidence](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0502-chain2-byte-address-review.json).

R0562/R0563:CHAIN4 word high-byte mask missing;current320/512fail;repair net-increase pending owner. See [proof](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0562-r0563-chain4-word-gap-review.json).

R0568-R0571 restored ineffective byte offsets;original UNCHAINED string width defect still outputs0. [Evidence](../../artifacts/mvdm-audit/t483-remaining-reaudit/r0564-r0571-final-string-review.json).

P56:sevid00133/33;eight restorations delivered(net-24);next R0605.

sevid004:R0605-R0607 qualified pending owner;source unchanged;next R0608.

sevid016:R0608-R0611 qualified. sevid017:11/11 qualified. sevid018:9/9;LF restored. sevid019:49/49;46 restored;3 residuals;net-140;P57 gates passed;next R0681.

P66 verified/published(diff-127);P67 ios3/3;main4/4 diff-18;cached build/publish11x2 and Full passed;next R0823.

Owner requests committing/pushing the current tree after stopping RDP investigation.
That RDP checkpoint package was e08de2fa-ed4f-409e-bf87-376e59bb3756; Full19 passed,
and the owner accepts the RDP input fix. The reported exit is suspected to be
concurrent-test interference; causality is unconfirmed. See the
[RDP checkpoint](../../artifacts/mvdm-audit/t483-remaining-reaudit/rdp-input-checkpoint.json).

P68-P74 delivered;indexed batch receipts preserve mirror deltas and Full results.
P75-P77 delivered;see indexed batch receipts for exact deltas and gates.

P78:five files complete;mirror net-50;five restores delivered,one minimized residual.
778 line checks0 mismatch;#error retained. Cached build,publish11x2,Full passed;Git delivery accompanies checkpoint;next R1005.

P79:nt_bop.c10/10 complete;mirror329->188(-141);three restores delivered.
Seven residuals pending;963 actual preprocessed lines match original;#error retained.
Focused controls, cached build,publish11x2,Full passed;zero-delta P78 line correction included;next R1015.

P80:nt_cga6/6 reviewed;mirror353->338(-15),comments/lines only;build,publish11x2,Full19 passed;nextR1021.

P81:nt_cpu19/19;mirror188->118(-70);13 restored,5 residuals,1 retained exclusion framework;final build,publish11x2,Full19 passed;nextR1040.

P82:nt_emm7/7;mirror131->92(-39);real controls0/AV/0,all5 directions/failures,217 lines;build,publish11x2,Full19 passed;nextR1047.

P101 nt_graph24/24 implemented;[receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p101-checkpoint.json).Diff329->262;5restored delivered;lines/focused/Full19 passed;next R1146.
