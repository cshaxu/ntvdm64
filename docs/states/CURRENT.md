# Project Status

## Current Work

**Active: M0 T483 S6 — descending non-error mirror difference reduction.**

Owner closes S5 after its final batch and admits S6. S5 delivery8c1c54aea is
pushed;all required verification passed. T483 remains active. This is the owner
superseding the earlier S6 hold,not admission of compat/component minimization.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T483 S6,Ordinary Mode;single agent,initial and counter-review passes. |
| Admission And Approval | Owner:close S5 after current batch;admit S6,sort mirrors by remaining diff point count after excluding#error,largest first,and synchronize pending decisions. |
| Objective | Reduce unnecessary original-relative difference blocks by restoring complete original units;keep changed-line count from growing without approval. |
| Non-goals | No guest modification/rebuild,no#error-policy changes,no unrelated compat/component audit,no deletion of harmless original code to merge hunks. |
| Reference Baseline | S5 commit8c1c54aeac9b97d020e86f5db50ca7d744aebd91;published package87c73097-d004-454e-925b-fea9a16f9552;pinned original sources. |
| Files And ABI Surface | 1052 current host C/H plus9 previously audited text mirrors;255 nonzero files in initial conservative inventory;semantic changes require complete caller/provider review. |
| Applicable Rules | Source policy,original-first execution rules,mirror subset/logical-line policy;modern USER ownership can be a valid boundary. |
| Verification | Pair error-only bodies before counting;per-file source/ABI and#line proof;cached x86/x64,publish11x2,focused and Full gates,governance,commit,push for substantive batches. |
| Expected Markers | Current file hash/origin,raw and filtered blocks/lines,precise restored units,remaining evidence and decisions,final artifact identities. |
| Asset Needs | Existing pinned OpenNT sources,build caches and fixtures;no new guest assets or winnt.zip changes. |
| Reporting Requirements | File/point start and end;blocks and lines before/after,cumulative reductions,file/point progress;distinguish classification,implementation,verification and delivery. |
| Stop Conditions | Net mirror increase requires prior owner approval;unsupported necessity is investigation,not a pending choice. No unsafe masking or guessed original match. |
| Exit Criteria | Every admitted file has a complete evidenced disposition,appropriate reductions and delivery;remaining owner decisions explicitly handed off,never silently approved. |
| Original Owner Request | After this batch close S5 and admit a new S;sort mirror files by descending diff points excluding#error,reduce points file by file,and update pending decisions. |
| Similar-Issue Sweep | Restore complete code/declaration units;avoid isolated comment islands and deleting harmless declarations;read coupled callers without jumping the worklist. |

## S6 Progress

[Worklist and counting contract](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/README.md).
1061 files inventoried;509 strict original-byte matches.806 files have zero
normalized/error-masked differences;255 have nonzero counts. The initial2333
block sum is conservative:75 mask-review cases in31 files remain counted.
These are counting uncertainties,not permission to delete or retain code.
Top8 order is unaffected. First file:ntuser/kernel/ntstubs.c,131 blocks.
S6 review16/255 complete;239 files remain. Thirteen changed files verified across
P2/P3/P4/P5/P6/P7/P8,three unchanged files fully reviewed. Current census2314 blocks;
cumulative delivered reduction19 blocks/85 lines;P8 reduction0 blocks/7 lines. P5 cached full,publish11x2,focused,
Full20 and governance passed;Git delivery accompanies the P5 receipt.

| File | Non-error blocks | Changed lines | Result |
| --- | --- | --- | --- |
| ntstubs.c |131->130|9238->9237|P2 restored original declaration|
| wmdisp32.c |78->75|228->224|P2 restored original stores/format|
| ntcftxt.h |65->62|1065->1057|P3 restored seven original declarations|
| sevid000.c |64->64|132->132|No code edit;64 evidenced pending decisions|
| gfx_updt.c |58->58|138->138|No code edit;58 evidenced pending decisions|
| usercli.h |49->45|125->114|P3 restored declaration/macros/format|
| process.c |47->47|168->167|P4 removed added blank;residuals pending owner|
| user.h |45->45|112->110|P4 redundant marker guard removed|
| nt_event.c |43->43|124->121|P5 original declarations/comment restored|
| dpmiint.c |42->34|231->221|P5 original formatting/declaration restored|
| wowhooks.c |38->38|114->114|No edit;residual decisions remain pending|
| ntpsapi.h |38->38|339->310|P5 original signatures/typedef restored|
| wmsg16.c |37->37|150->146|P6 original entry/declaration restored;Full20 passed|
| c_main.c |36->36|163->158|P7 single-use IRQ helper inlined;Full20 passed|
| client/ntstubs.c |35->35|1805->1803|P8 original include restored;five contexts equivalent|
| kernel/queue.c |35->35|4178->4173|P8 original declarations restored;Full20 passed|

[P3 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/batch-p3-checkpoint.json)
records cached x86/x64,publish11x2,message/focused/task-order,Full20,
boundary143/9 and governance passes. Git delivery accompanies the receipt.
Per-file evidence is indexed in the worklist README. Residual decisions remain
pending owner,not approved. Known sevid000 CHAIN4 word/UNCHAINED string
limitations are not represented as passing tests. Backlog1644 rows/1649 R points;
R1519 remains restored,not deferred. P5 committed/pushed f6d6f78c5. wmsg16.c whole-file review complete;P6 verified delivery accompanies receipt. P6 committed/pushed2202c9239. P7 committed/pushed e1d946570. Client ntstubs whole-file review complete;nonbehavioral include restoration delivered in P8. queue.c whole-file review complete;P8 verification complete;delivery accompanies receipt. Next:wow32/wcntl32.c,34 blocks. nt_event.c reviewed:43->43 blocks,
124->121 lines;two added X86GFX exclusions remain conservatively counted.
[P4 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/batch-p4-checkpoint.json)
records final cached/full/publication/message/focused/boundary/governance passes.
Owned build/pipeline are terminal;Git delivery accompanies receipt.
Do not restart completed file reviews.
[wmsg16 review](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/wmsg16-whole-file-review.json)
records native COPYDATA rejection,59/14/18 source cases,7 negative-control failures
and compiler shared-line1003/0. P6 cached full,publish11x2,focused and Full20 passed.

S5's2720-point register is historical identity/evidence:1018 restored/confirmed
and delivered,52 approved/policy,1650 pending owner. The
[pending backlog](../../artifacts/mvdm-audit/t483-remaining-reaudit/pending-decisions.csv)
remains canonical.2333 counted blocks are not additional R identifiers.
Complete each file,then update both register and backlog;recount and reorder
unprocessed files when masks or differences change. Pure formatting may
accumulate;substantive whole-file batches follow build,publish,test,commit,push.

[P5 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/batch-p5-checkpoint.json)
records final source hashes,logical-line checks and Full20 (419006ms).
[Verification evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/p5-verification-evidence.zip)
retains the initial failed WOW frontier and fresh-process passing control.
Added keyboard fixtures imported VS environment into the first parent runner;
subsequent execution isolates them in child processes. No product change or
weakened assertion separated the failed and passing package runs.

## Current Technical Baseline

S6 P8:11 images published/hash-verified to assets/release and O:/winnt/system32.
Package91a237ec-2924-4e4b-b472-e1b933981346. Full20 includes actual NESticle
menu/Escape. No guest or winnt.zip changes. P8 receipt binds final source,
manifest and test results;P7 publication is superseded. All owned runners are
terminal. Git delivery accompanies this receipt;S6 remains active.

S5 fixed365-file raw metric:#error included,#line excluded,4862blocks/89379lines;
cumulative reduction435blocks/30534lines. Do not compare this directly to the
new1061-file error-body-masked census.

## Remaining Scope

Owner decisions and concrete boundary choices transfer to S6,not approved by
S5 closure. Broader compat/autonomous and eleven-component minimization remain
later work in the admitted T. Existing DOS identity and Win95 exit-code debt
stays in [TODO](TODO.md);restart-to-DOS belongs to the XP-baseline queue item.
No T queue reorder.

## Recent M0 Closures

- [T483 S5](../etc/evidence/m0-t483-s5-closure.md):delivered8c1c54aea;residual decisions handed off.
- [T483 S4](../etc/evidence/m0-t483-s4-closure.md):prior bounded minimization.

## Recent Governance

S6 is owner-admitted after S5's verified,pushed closure. Current stays below
16KiB;older P details remain in the indexed S5 evidence. Follow
[execution rules](../rules/EXECUTION.md) and [source policy](../etc/operations/policy/source-policy.md).

[P6 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/batch-p6-checkpoint.json) binds the final whole-file restoration and published package.

[c_main whole-file review](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/c-main-whole-file-review.json) records P7 final source identity and residual necessity;verified delivery accompanies receipt.

[P7 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/batch-p7-checkpoint.json) binds cached full,publish11x2,CPU focused tests,Full20 and3054 shared compiler line checks.

[Client ntstubs review](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/client-ntstubs-whole-file-review.json):35->35 non-error points,1805->1803 lines;five actual include contexts identical. Verified and delivered in P8.

[Queue review](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/queue-whole-file-review.json):35->35 points,4178->4173 lines;P8 also includes client ntstubs -2 lines. Verified and delivered in P8.

[P8 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/batch-p8-checkpoint.json) binds the two-file batch,compiler equivalence and Full20.
