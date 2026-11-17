# Project Status

## Current Work

**Active: M0 T483 S6 — descending non-error mirror difference reduction.**

Owner closes S5 after its final batch and admits S6. S5 delivery8c1c54aea is
pushed;all required verification passed. T483 remains active. This is the owner
superseding the earlier S6 hold,not general admission of compat/component minimization. Owner now explicitly
authorizes moving the sole retained ntstubs.c FNID function into src/compat
and retiring that mirror file and its active difference entries (P19).

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
39/255 initial difference files reviewed;216 remain. P19,P21-P29 retire ten
reviewed files by owner approval;live population is29 reviewed/245,216 remain. Twenty-five changed files
verified across P2-P18;eight reviewed unchanged. Current census:1053 files
(1044 host C/H plus9 other text mirrors),511 original-byte matches,808 zero
normalized/error-masked differences,and245 nonzero files.
Current delivered total2065 blocks/29810 lines. P19 retires130 blocks/9237
lines;this is mirror reclassification,not original restoration. Prior delivered
reduction remains40 blocks/1194 lines. Mouse's earlier two-line counting correction remains separate.

| File | Non-error blocks | Changed lines | Result |
| --- | --- | --- | --- |
| ntstubs.c |130->retired|9237->retired|P19 compat extraction;focused38 and Full20 passed|
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
| kernel/handtabl.c |24->retired|1381->retired|P23 compat extraction;original ownership and targeted lifecycle passed|
| client/client.c |8->retired|3564->retired|P24 five compat units;original bodies and targeted lifecycle passed|
| rtl/time.c |6->removed|1216->removed|P25 unused archive member removed;no compat replacement|
| kernel/inctlpan.c |4->retired|881->retired|P29 sysBmpStyles extraction; targeted WOW validation passed|
| kernel/miscutil.c |3->retired|198->retired|P28 SetDialogPointer extraction; targeted WOW validation passed|
| rtl/rect.c |3->retired|368->retired|P27 CopyInflateRect extraction; targeted WOW validation passed|
| support/xt.c |4->retired|339->retired|P26 verbose storage extraction;historical mask ambiguity retained in evidence|
| kernel/input.c |25->retired|5503->retired|P22 compat extraction;targeted input and lifecycle passed|
| sevid001.c |25->25|50->50|Unchanged;25 evidenced decisions;prior-source identity checked|
| wow32/wkman.c |25->25|46->45|P14 extra blank removed;Full20 passed|
| command/cmdexec.c |24->17|123->82|P14 declaration/format restored;Full20 passed|
| keymouse/keyba.c |24->24|72->56|P15 added guard pairs removed;actual keyboard90 and Full20 passed|
| client/vdm.c |23->20|796->632|P16 four original functions restored;six compile contexts and Full20 passed|
| client/dlgmgr.c |22->17|295->242|P17 original declarations/CvtDec/comments restored;focused37 and Full20 passed|
| client/wow.c |21->retired|832->retired|P21 compat extraction;focused38 and Full20 passed|
| kernel/cleanup.c |21->20|66->44|P18 original ClientDied restored;focused37,lifecycle and Full20 passed|


The canonical pending backlog has1576 rows after retiring ten owner-approved mirror files,
plus2 non-R items. Extraction removes active mirror entries;it is not original-code restoration.
Partial restorations do not close residual owner decisions. Three complete
handtabl restorations removed from pending;remaining private-kernel and ABI
boundaries remain pending. Historical R identifiers are not current diff blocks.
P13 original input declarations are restored and verified. The later P22 owner-approved extraction retires the input mirror and its
previous residual macro decisions;history remains in the extraction record.
No residual omission is silently approved. Owner superseding direction:after P18,
stop automatic progression and hand over every remaining nonzero file ordered by
non-error blocks descending,path ascending for ties. Await owner file selection.
[Manual review order](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/owner-review-order.csv).

## Current Technical Baseline

P29 cached x86/x64 build, link provider and import checks, WOW formal graph,
control fixtures and WOW lifecycle passed. Package 80700809-491c-444e-a2e8-a029fb3768dc contains
11 images published and hash verified in assets/release and O:/winnt/system32.
[Receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/batch-p29-checkpoint.json).
Full was outside the owner-approved targeted scope.

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

## P19 Delivery

[Owner-approved extraction](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/ntstubs-compat-extraction.json) moves the sole selected FNID function to compat, removes its dedicated macros and mirror file, and updates active registers. Cached build,publish11x2,focused38,lifecycle and Full20 passed;Git accompanies receipt. No next-file admission.

## P20 Usage Ratio Scan

Owner requests all254 nonzero mirror files sorted by original-source usage ratio.
[Read-only report](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/usage-ratio/REPORT.md) records806 successful current-product preprocessing contexts and unchanged object hashes. It lists200 C implementations,51 headers and3 build/export descriptions. Original function spans include internal comments/blanks; macros/header usage limits are explicit. Six C files have a proved footprint below5 percent;13 below10 percent. This is a lower-bound source-footprint census,not runtime coverage or deletion authority. No source,package or audited-file-count change;owner selection remains required.

## P21 Delivery

Owner approves extraction of client/wow.c::_GetDlgItem into compat, preserving
its original algorithm and callers. Only this file is admitted;no next-file
progression. Update product/fixture selection and active difference registers;
Cached full,publish11x2,focused38,lifecycle and Full20 passed;Git accompanies receipt.
The system-metrics compatibility boundary remains recorded,not declared fixed.

P21 retires21 blocks/832 lines;combined P19/P21 retire151 blocks/10069 lines.
Together with original restoration reductions40 blocks/1194 lines,the census
is down191 blocks/11263 lines. Main register2703 rows;pending1631 rows.
No next-file admission.

## P22 Delivery

Owner approves extraction of kernel/input.c AddAttachment/Recalc2 into compat.
Preserve both algorithms and the existing explicit current-thread argument.
Only this file is admitted. Owner replaces Full with related targeted tests:
cached build,input membership/native focus/input lifetime,WOW lifecycle and
formal graph verification,then publish11x2,commit,push. No Full claim.

P22 retires25 blocks/5503 lines;combined extractions retire176 blocks/15572 lines.
With original restoration40 blocks/1194 lines,the census is down216 blocks/16766 lines.
Main register2687 rows;pending1617 rows. All targeted checks and publish11x2
passed;governance and Git accompany receipt. No next-file admission.

## P23 Delivery

Owner approves retiring kernel/handtabl.c and retaining its consumed original
object-ownership table in compat. Preserve original values,types,flags and
cleanup consumer;remove unused unexported residual definitions. Only this file
is admitted. Cached full build,ownership source comparison,focused control
and actual WOW lifecycle tests,formal graph,publish11x2,commit,push.
Owner-selected targeted verification replaces Full for this extraction.

P23 retires24 blocks/1381 lines;combined extractions retire200 blocks/16953 lines.
With original restoration40 blocks/1194 lines,the census is down240 blocks/18147 lines.
Main register2664 rows;pending1608 rows. Cached build,targeted verification
and publish11x2 passed;governance and Git accompany receipt. No next-file admission.

## P24 Delivery

Owner approves extracting client/client.c into five responsibility-specific
compat units, preserving all eight original bodies and existing ABI/bindings.
Keep original ntcftxt.h registration implementation;remove multi-entry source
selection. Only this mirror is admitted. Cached full build,publish11x2,original
body comparison,focused controls,formal graph and actual WOW lifecycle,then
commit/push. Related targeted verification replaces Full by owner approval.

P24 retires8 blocks/3564 lines;combined extractions retire208 blocks/20517 lines.
With original restoration40 blocks/1194 lines,the census is down248 blocks/21711 lines.
Main register2653 rows;pending1601 rows. Cached build,targeted tests and
publish11x2 passed;governance and Git accompany receipt. No next-file admission.

## P25 Delivery

Owner approves researching and removing unused base/ntos/rtl/time.c,without
compat replacement. Only consumer is an unreferenced Netlib wrapper;current
product maps contain neither symbol. Remove the archive input and obsolete
local-body test;require final cached x86/x64 links and archive/member checks.
Run native time boundary checks and existing WOW lifecycle smoke,publish11x2,
commit/push. Full remains replaced by related targeted verification.

P25 removes6 blocks/1216 lines without replacement. Cumulative original
restoration40 blocks/1194 lines,compat extractions208 blocks/20517 lines,
unused removal6 blocks/1216 lines:total254 blocks/22927 lines.
Main register2648 rows;pending1596 rows. Build,publish11x2,targeted tests
passed;governance and Git accompany receipt. No next-file admission.

## P26 Delivery

Owner approves retiring support/xt.c and retaining its sole consumed global
int verbose in existing shared SoftPC execution compatibility storage.
Original main.c initialization,CCPU interrupt/parity owners and sources manifest
remain unchanged. Exclude xt.c in the build generator;require full cached
x86/x64 link,archive/symbol proof,CCPU boundary tests and WOW lifecycle.
Publish11x2,commit/push;targeted verification replaces Full. No next file.

P26 retires4 recorded blocks/339 lines,including two historical mask ambiguities.
Cumulative original restoration40 blocks/1194 lines,extractions212 blocks/20856
lines,unused removal6 blocks/1216 lines:total258 blocks/23266 lines.
Main register2644 rows;pending1594 rows. Build,publish11x2,targeted tests passed;
governance and Git accompany receipt. No next-file admission.
