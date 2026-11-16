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
2182 reviewed:816 restored/confirmed published,0 local restorations,8 approved,
1358 qualified pending;538 unreviewed. Files307/365 audited,58 remain.
P223 file307 verified/published;Git delivery accompanies receipt.
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

P223 package `0bdc09f3-fa83-46a3-b461-c843b0b57477` published11x2;hashes verified.
clmsg1608->1535(-73);original ordering/common headers restored and selectors
consolidated. Six restored/confirmed points;two residual owner decisions.
Cached x86/x64,32 controls,Full20 including DOS4GW and governance passed.
P222 DPMI repair remains included. No guest/winnt.zip changes.
Cumulative365-file diff119913->104527(-15386).
[Receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p223-checkpoint.json).
Git delivery accompanies receipt;ordered audit resumes at R2183.

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

P176–P218 delivery details remain indexed in the active audit README; their
receipts retain source, line, publication and test evidence.

P219 DOS4GW supplement is delegated by owner to a separate session. Current PIF and direct minimal-environment runs failed to reach graphics; retained diagnostics are not acceptance. Preserve its uncommitted test changes; this session does not debug that issue.

P220 verified/published:client.c3664->3572(-92);four restored points,seven minimized residual decisions.163 source/76 selected lines,three bare errors;cached x86/x64,publish11x2,32 focused,Full19 and governance passed. Package 7fe90997-4dae-4832-858f-59c084d38d18. Cumulative365-file mirror diff119913->104579(-15334). DOS4GW debugging remains assigned to side session,not claimed repaired. [Receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p220-checkpoint.json). NextR2161.

## Owner-priority regression repair after P221

Owner assigns P222 to the source-proven P103 DPMI/DOS4GW regression after
clmenu delivery. Retain CCPU checks and host hook delegation; repair bounded
DPMI instruction/FLAGS/IRQ semantics and make actual NESticle graphics startup
mandatory in Full. Necessary mirror increases enter pending decisions. Preserve
side-session test edits;no guest changes,Computer Use or unrelated sevid019 fix.
[Plan](../../artifacts/mvdm-audit/dos4gw-pif-sti-20261115/main-repair-plan.json).
Resume ordered R2175 audit after this repair is delivered.

P221 verified/published:clmenu396->343(-53);original parser signatures,CLIENTINFO predicate and cast increments restored.12 points restored/confirmed;header3 and SetMenu6 pending owner.340 source/222 selected lines;cached,publish11x2,menu-resource/32 controls,Full19 and governance passed. Package bf48125e-5657-4ce3-ac9d-5b11b188c1df. Cumulative mirror reduction15387. [Receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p221-checkpoint.json). P222 DPMI/DOS4GW repair is active;R2175 follows its delivery.


P222 acceptance:actual published NESticle menu/Escape;Full20;58 CCPU cases
(including48 scalar combinations and16 decode controls);Win3.1 std/386 x
DOSONLY0/1 startup and normal exit. Probe copy removed. Inherited large-agent
environment COMMAND failure is separate;standard fixture uses minimal environment
and the original short-PATH PIF. Strings/CLTS/LMSW/privileged MOV DR remain
unimplemented,not claimed repaired. Necessary residual +74 remains pending.

P223 verified/published:clmsg1608->1535(-73). Six restored/confirmed points,
two minimized residual owner decisions. Default/CPD code identical after
preprocessing;332 original source lines and16 errors checked. Cached x86/x64,
publish11x2,32 controls,Full20 and governance passed.
[Receipt](../../artifacts/mvdm-audit/t483-remaining-reaudit/batch-p223-checkpoint.json).
Cumulative365-file mirror reduction15386. NextR2183.
