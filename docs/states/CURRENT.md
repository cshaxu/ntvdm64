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
| Files And ABI Surface | 1053 current host C/H plus9 previously audited text mirrors;255 nonzero files in initial conservative inventory;semantic changes require complete caller/provider review. |
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
21/255 initial difference files reviewed;234 remain. Sixteen changed files
verified across P2-P11;five reviewed unchanged. Current census:1062 files
(1053 host C/H plus9 other text mirrors),510 original-byte matches,807 zero
normalized/error-masked differences,and255 nonzero files. P10 adds one
byte-exact original declaration header;it is not a reduced difference point.
Current total2312 blocks/53752 lines;cumulative delivered reduction21 blocks/
771 lines. Mouse's earlier two-line counting correction remains separate.

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
| wcntl32.c |34->34|69->69|No edit;residuals pending owner|
| mouse_io.c |33->33|349->349|No edit;separate two-line counting correction|
| wumsg.c |33->32|113->109|P9 single-use cleanup inlined;Full20 passed|
| client/clres.c |31->30|3352->2733|P10 original complete units restored;Full20 and final equivalent build passed|
| client/clmsg.c |27->27|1042->979|P11 two complete original functions and five macros restored;Full20 passed|


The canonical pending backlog remains1644 rows/1649 historical R identifiers.
Partial restorations do not close residual owner decisions. Historical R points
are not the current zero-context difference block count. Do not repeat already
completed file reviews. Next:ntuser/kernel/userk.h,27 non-error blocks.

## Current Technical Baseline

P11 cached x86/x64 full builds passed. Packagee01121ca-8682-4a96-8705-743ca5061054 contains11
images,published and hash-verified to assets/release and O:/winnt/system32.
Bitmap/message/DDE,controls/task-order,Full20 including real DOS4GW/NESticle,
shared-boundary and governance passed. Final source identity matches the
built/published/tested checkpoint. Four original function compiler anchors
match;16 error bodies remain unchanged. No guest or winnt.zip changes.

[P11 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/batch-p11-checkpoint.json)
and [evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/p11-verification-evidence.zip)
bind source,package and tests. [Whole-file review](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/clmsg-whole-file-review.json)
records original units restored and residual owner decisions. Earlier receipts
remain indexed in the worklist. Git delivery accompanies this receipt.

## Remaining Scope

S6 remains active. Residual owner decisions are not silently approved. Broader
compat/autonomous and eleven-component minimization remain later work in this T.
Existing DOS identity and Win95 exit-code debt remains in [TODO](TODO.md);
restart-to-DOS belongs to the XP-baseline queue item. No queue reorder.

## Recent M0 Closures

[S5 closure](../etc/evidence/m0-t483-s5-closure.md) delivered8c1c54aea;S6 is
owner-admitted.

## Recent Governance

Follow [execution rules](../rules/EXECUTION.md) and
[source policy](../etc/operations/policy/source-policy.md). Prior P receipts
are indexed in the worklist README. Current remains below16KiB.
