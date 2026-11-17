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
| Files And ABI Surface | 1054 current host C/H plus9 previously audited text mirrors;255 nonzero files in initial conservative inventory;semantic changes require complete caller/provider review. |
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
33/255 initial difference files reviewed;222 remain. Twenty-five changed files
verified across P2-P18;eight reviewed unchanged. Current census:1063 files
(1054 host C/H plus9 other text mirrors),511 original-byte matches,808 zero
normalized/error-masked differences,and255 nonzero files.
Current total2293 blocks/53329 lines;cumulative delivered reduction40 blocks/
1194 lines. Mouse's earlier two-line counting correction remains separate.

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
| kernel/userk.h |27->27|117->115|P12 two added comments removed;Full20 passed|
| wow32/wdde.c |26->26|143->143|P12 reviewed unchanged;residual decisions pending|
| kernel/handtabl.c |26->24|1481->1381|P12 original declarations and KernelPtoH restored;Full20 passed|
| kernel/input.c |26->25|5527->5503|P13 original declarations restored;Full20 and input services/lifetime passed|
| sevid001.c |25->25|50->50|Unchanged;25 evidenced decisions;prior-source identity checked|
| wow32/wkman.c |25->25|46->45|P14 extra blank removed;Full20 passed|
| command/cmdexec.c |24->17|123->82|P14 declaration/format restored;Full20 passed|
| keymouse/keyba.c |24->24|72->56|P15 added guard pairs removed;actual keyboard90 and Full20 passed|
| client/vdm.c |23->20|796->632|P16 four original functions restored;six compile contexts and Full20 passed|
| client/dlgmgr.c |22->17|295->242|P17 original declarations/CvtDec/comments restored;focused37 and Full20 passed|
| client/wow.c |21->21|832->832|No source change;94 original logical rows match;residual decisions pending|
| kernel/cleanup.c |21->20|66->44|P18 original ClientDied restored;focused37,lifecycle and Full20 passed|


The canonical pending backlog has1641 rows/1643 distinct historical R identifiers,
plus2 non-R items. Corrected counting only;not a source reduction.
Partial restorations do not close residual owner decisions. Three complete
handtabl restorations removed from pending;remaining private-kernel and ABI
boundaries remain pending. Historical R identifiers are not current diff blocks.
P13 original input declarations are restored and verified. Seven scattered
harmless macros would increase file blocks26->27;owner approval remains pending.
No residual omission is silently approved. Owner superseding direction:after P18,
stop automatic progression and hand over every remaining nonzero file ordered by
non-error blocks descending,path ascending for ties. Await owner file selection.
[Manual review order](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/owner-review-order.csv).

## Current Technical Baseline

P18 cached x86/x64 full,WOW focused37,actual lifecycle and Full20 passed.
Package 160cb119-0892-44a7-a24f-7f6e50692f45 contains11 images,published and
hash-verified to assets/release and O:/winnt/system32. Full includes actual
DOS4GW/NESticle. No guest or winnt.zip changes. Original ClientDied restored
through existing native RTL exception provider;254 meaningful original logical
lines match. Residual decisions remain pending;governance and Git accompany receipt.

[P18 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/batch-p18-checkpoint.json)
and [evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/p18-verification-evidence.zip).

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
