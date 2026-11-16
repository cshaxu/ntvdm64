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
2031 reviewed:708 restored/confirmed published,0 local confirmations/restorations,8 approved,
1315 qualified pending;689 unreviewed. Files294/365 audited,71 remain.
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

## Current Technical Baseline

P181 candidate `08431d90-1f10-49d0-b505-574e4a79f22f` published11x2;hashes match.
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

P102-P106 delivered;see active audit README indexed receipts.

P107-P109 delivered;see active audit README indexed receipts.

P110-P112 delivered;see active audit README indexed receipts.

P113 nt_timer.c mirror109->96(-13);cached build,publish11x2,native alert/Full19 passed.
[Checkpoint](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p113-checkpoint.json).

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

## Earlier Delivery Evidence

P17–P149 historical batch receipts and reviews remain indexed in the
[active audit evidence](../../artifacts/mvdm-audit/t483-remaining-reaudit/README.md).
The latest published baseline and active work are reported above.

P176 final:environ14->11(-3);ordinary semantic diff removed,excluded11 retained;449 MSVC lines match. Original RTL names restored with x86/x64 provider and private-PEB clone proof. Cached,publish11x2,focused,Full19,RDP Clipboard18 passed. First RDP failure was confirmed RadeonSoftware Clipboard occupancy;unchanged retry passed. Package 83adbf7e-5ee3-4e21-b906-e339b3085d6a;Git accompanies receipt.

P181 srvvdm.c93->84(-9):redundant comment/null-output guard removed;CRLF and endif logical lines restored;2317 original lines match. Notification current5/5,original control3 failures;native casts fail actual x64 build. Cached,publish11x2,service/Full passed;10 residual decisions;Git accompanies receipt.

P186 apimsg533->7(-526):original descriptors and complete function bodies restored;header binding2 remains pending,excluded5 retained. 276 MSVC-selected lines match original;cached,publish11x2,Full and actual DOS NetMessage failure/retirement passed. Prior-package shared RDP Clipboard recheck remains unsuccessful due recorded external ownership;not a new pass. Git accompanies receipt.

P187 apisrv1067->190(-877):original linkable bodies and error semantics restored;includes4 and Enum3 omission180 pending,excluded6 retained.901 source/607 MSVC lines match;focused37,cached,publish11x2,Full passed. Original C4701 remains disclosed;RDP shared Clipboard still fails with external holders. Git accompanies receipt.

P201 vdm.h26 unchanged(all excluded),raw40 unchanged:conditional boundary lines restored;5#error retained.8 representative host/DEM/DPMI/WOW contexts tokens/header lines equal;4 negative profiles rejected;literal if0 retained;366 source lines match. P187 product evidence reused. [Receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p201-checkpoint.json).
P202 debuglib.h6 unchanged(all excluded),raw9->8(-1):else149 and original CRLF restored;DBG#error retained;line directives3->2.6 contexts tokens equal;negative DBG rejected;109 source lines match. P187 product evidence reused. [Receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p202-checkpoint.json).

P203 netdebug.h380 unchanged(all excluded),raw418 unchanged:original boundary lines and CRLF restored;13#error retained.20 contexts tokens/selected lines equal;3 negative profiles rejected;247 source lines match. P187 product evidence reused. [Receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p203-checkpoint.json).

P204 netlib.h21 unchanged(all excluded),raw33->25(-8):original boundary lines and CRLF restored;5#error retained;directives12->4.14 contexts tokens/186 header lines equal;4 negative controls;451 source lines match. P187 product evidence reused. [Receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p204-checkpoint.json).

P205 rap.h31 unchanged(all excluded),raw33 unchanged:endif140 corrected;Unicode#error retained;19 contexts/57 preprocessings tokens and104 header lines equal;282 source lines match;Unicode negative passes. P187 product reused. Current RDP Clipboard18 passed after removing evidenced RadeonSoftware occupancy;earlier failures retained in receipts. [Receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p205-checkpoint.json).

P206 rapdebug.h8 unchanged(all excluded),raw12->10(-2):else60 restored;DBG#error retained;2 actual contexts/6 preprocessings equal;35 source lines match;DBG negative passes. P187 product and P205 RDP evidence reused. [Receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p206-checkpoint.json).

P207 winddi.h41 unchanged(all excluded),raw44->43(-1):else2406 restored;_X86_#error retained;actual WOW32 fax context tokens/1357 lines equal;2040 source lines match;negative passes. P187 product and P205 RDP evidence reused. [Receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p207-checkpoint.json).

P208 ntcsrmsg.h6->4(-2),raw7->4(-3):original comments/natural lines restored;two native-width capture fields remain pending. x86 three controls pass;x64 original header misaligns captured STARTUPINFOA,before/final pass;ABI evidence,not user crash.108 retained lines match;no behavior change. P187 product reused. [Receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p208-checkpoint.json).
